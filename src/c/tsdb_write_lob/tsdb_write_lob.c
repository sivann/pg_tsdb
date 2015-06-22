//  SELECT tsdb_write_lob(var_dataformat,var_granularity_days,var_dataoid,  value_arg, quality_arg, mt_arg, rt_arg);
// 	/src/postgres/src/backend/libpq/be-fsstubs.c
// http://prabi1.inrialpes.fr/trac/OBITypes/browser/trunk/src/seq_utils.c?rev=232 (inv_*)

// https://www.pgcon.org/2010/schedule/attachments/142_HackingWithUDFs.pdf
//Macros like DatumGetInt64 hide if you’re passing a 8 byte integer by value (64-bit platform) or reference (32-bit)

// read lo: http://www.postgresql.org/message-id/44B35C31.2060703@antora.ru
// lo_read,lo_seek,lo_creat lo: https://github.com/sunyangkobe/cscd43/blob/master/postgresql-7.4.13/src/tutorial/beard.c

// msgpack format: https://github.com/msgpack/msgpack/blob/master/spec.md#formats-array

// lz4: https://code.google.com/p/lz4/source/browse/trunk/lz4.h?r=102

// EXAMPLE:
//  SELECT tsdb_write_lob(1::smallint,1::smallint, 43175::oid,'0.001'::varchar,'99'::varchar,now()::timestamp, now()::timestamp);
//  select lo_export(43175, '/tmp/lo43175.txt')


#include "postgres.h"
#include "fmgr.h"
#include "utils/timestamp.h"

#include "libpq/libpq-fs.h"
#include "libpq/be-fsstubs.h"

#include <msgpack.h>
#include <stdio.h>
#include <string.h>

#include <lz4.h>
#include <stdint.h>
#include <stdlib.h>

PG_MODULE_MAGIC;

Datum tsdb_write_lob(PG_FUNCTION_ARGS) ;
void text2cstring(text* , char **);

PG_FUNCTION_INFO_V1(tsdb_write_lob);

#pragma pack(push, 1) //disable byte padding on this struct
struct lob_header {
	uint8_t compression;//0:not compressed, 1: compressed
	uint16_t datasize; 	//size before compression. Needed for fast decompression using LZ4_decompress_fast
	uint16_t reserved1;
};
#pragma pack(pop)


#define BUFSIZE 8192 //maximum lob size

Datum tsdb_write_lob(PG_FUNCTION_ARGS) {
    int16 arg_dataformat = PG_GETARG_INT16(0);
    int16 arg_granularity_days = PG_GETARG_INT16(1);
    Oid arg_dataoid = PG_GETARG_OID(2);
    VarChar* arg_value = (VarChar *)PG_GETARG_VARCHAR_P(3);
    VarChar* arg_quality = (VarChar *)PG_GETARG_VARCHAR_P(4);
    Timestamp arg_mt = PG_GETARG_TIMESTAMP(5); 
    Timestamp arg_rt = PG_GETARG_TIMESTAMP(6); //result = arg_rt / 1000 - ((POSTGRES_EPOCH_JDATE - UNIX_EPOCH_JDATE) * 86400);

	char * c_arg_value, * c_arg_quality;
	int inv_fd;
	char data_c_buf[BUFSIZE];
	char data_u_buf[BUFSIZE];
	char data_u2_buf[BUFSIZE];
	char data_c2_buf[BUFSIZE];
	int data_c_sz=0, data_u_sz=0, data_u2_sz=0, data_c2_sz=0;
	int r;

	struct lob_header lobhdr, lobhdr_new;

	msgpack_sbuffer sbuf;
	msgpack_packer pk;

	char *charhdr;

	charhdr=palloc(sizeof(struct lob_header)); // to avoid pointer casts when using lo_read/write
	


	/* copy arg_x strings for safety since the former are readonly */
	text2cstring(arg_value,&c_arg_value);
	text2cstring(arg_quality,&c_arg_quality);



	ereport( INFO, (errcode( ERRCODE_SUCCESSFUL_COMPLETION ),
				errmsg("Arguments were(2): %d, %d, %ld,   [(%s), (%s), %ld, %ld]\n",
					arg_dataformat, arg_granularity_days, DatumGetInt64(arg_dataoid),
					c_arg_value,c_arg_quality, arg_mt,arg_rt)));

	//open large object
	inv_fd = DatumGetInt32(DirectFunctionCall2(lo_open, arg_dataoid, Int32GetDatum(INV_READ|INV_WRITE)));
	if (inv_fd < 0) {
		elog(ERROR, "lo_open: lookup failed for oid:%ld", DatumGetInt64(arg_dataoid));
        PG_RETURN_INT32(-1);
	}


	/* 
	   LOB HDR format: 
		Byte1: 0:msgpack, 1:lz4 compressed msgpack
		Byte2-Byte3: Original size
		Byte4-Byte5: reserved for future
		Byte6-Byten: data
	    * pack new data
	    * read lob HDR 
	    * read/write compressed
			READ
		    * read old data 
		    * uncompress
			* append packed data

			WRITE
			* compress 
			* truncate lob
			* write new header
			* write data
		* read/write uncompressed
			READ
			* seek to end of lob end (for uncompressed)

			WRITE
			* write (append) new packed data

	   Compression y/n should come as a parameter.
	   */



	/*****************/
	/* Pack new data */
	/*****************/
	//create msgpack
	/* msgpack::sbuffer is a simple buffer implementation. */
	msgpack_sbuffer_init(&sbuf);

	/* serialize values into the buffer using msgpack_sbuffer_write callback function. */
	msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);

	//value_arg, quality_arg, mt_arg, rt_arg
	msgpack_pack_array(&pk, 4);

	//value
	msgpack_pack_str(&pk, strlen(c_arg_value));
	msgpack_pack_str_body(&pk, c_arg_value, strlen(c_arg_value));

	//quality
	msgpack_pack_str(&pk, strlen(c_arg_quality));
	msgpack_pack_str_body(&pk, c_arg_quality, strlen(c_arg_quality));

#ifdef HAVE_INT64_TIMESTAMP
	msgpack_pack_int64(&pk, arg_mt);
	msgpack_pack_int64(&pk, arg_rt);
#else
	#error "How long is timestamp on this machine?"
#endif


    //DBG: write packed data to a file
	/*
	FILE * fp;
	fp=fopen("/tmp/packed-buf","w");
	fwrite(sbuf.data,1,sbuf.size,fp);
	fclose(fp);
	*/


	/*****************/
	/* Read old data */
	/*****************/
	/*
	if ((r=lo_read(inv_fd, &lobhdr.compression, 1))!=1) {
		elog(ERROR, "lo_read 1 byte lobhdr.compression: %d", r);
		lobhdr.compression=0;
	};

	if ((r=lo_read(inv_fd, &lobhdr.datasize, 2))!=2) {
		elog(ERROR, "error: lo_read 2 bytes lobhdr.datasize: %d", r);
		lobhdr.datasize=0;
	};
	*/

	//We may need to read char[5] and then copy to lobhdr
	//if ((r=lo_read(inv_fd, &lobhdr, sizeof(struct lob_header)))!=1) 
	if ((r=lo_read(inv_fd, charhdr, sizeof(struct lob_header)))!=1) {
		elog(ERROR, "lo_read lobhdr: %d", r);
		lobhdr.compression=0;
		lobhdr.datasize=0;
	}
	memcpy(&lobhdr,charhdr,sizeof(struct lob_header));

	elog(INFO, "lobhdr.compression: %d", lobhdr.compression);
	elog(INFO, "lobhdr.datasize: %d", lobhdr.datasize);


	//determine writing compression
	if (arg_dataformat>=100 && sbuf.size > arg_dataformat) {
		lobhdr_new.compression=1; //compress, data size above threshold
	}
	if (arg_dataformat>=100) {
		lobhdr_new.compression=0; //no compress, size below threshold
	}
	else if (arg_dataformat == 1) {
		lobhdr_new.compression=1; //compress
	}
	else if (arg_dataformat == 0) {
		lobhdr_new.compression=0; //no compress
	}
	else
		lobhdr_new.compression=lobhdr.compression;

	/* read uncompressed, write uncompressed */
	if (lobhdr.compression == 0 && lobhdr_new.compression== 0) {
		/* Just Seek to the end of file */
		if ((data_u_sz = DatumGetInt32(DirectFunctionCall3(
							lo_lseek,
							Int32GetDatum(inv_fd),
							Int32GetDatum(0),
							Int32GetDatum(SEEK_END)))) < 0)
			elog(ERROR, "Cannot seek to the end of large object %ld",DatumGetInt64(arg_dataoid));
		data_u_sz-=sizeof(struct lob_header); //lseek will contain header size

		/* Create new data */
		memcpy(data_u2_buf, sbuf.data, sbuf.size); //new data
		data_u2_sz=sbuf.size;

		/* Write */
		//just append msgpacked data to previous
		if (lo_write(inv_fd, sbuf.data, sbuf.size ) != sbuf.size)
			elog(ERROR, "error while writing large object (uc/uc)");

	}
	/* Read compressed */
	else if (lobhdr.compression > 0) { 
		/* uncompress previously compressed data before appdending */
		// uncompressed data will be @data_u_buf
		data_c_sz=lo_read(inv_fd, (char *) data_c_buf, BUFSIZE);
		r=LZ4_decompress_fast (data_c_buf, data_u_buf, lobhdr.datasize); //returns bytes read
		data_u_sz=lobhdr.datasize;
		if (r<0) 
			elog(ERROR, "Error: LZ4_decompress_fast:%d",r);

		/*
		//this version does not need to know uncompressed size beforehand, but needs input size
		r=LZ4_decompress_safe (data_buf, data_buf2, data_sz, BUFSIZE); //returns bytes decompressed
		if (r<0) {
			data_u_sz=0;
			elog(ERROR, "Error: LZ4_decompress_fast:%d",r);
		}
		else
			data_u_sz=r;
		*/

		/* Create new data */
		memcpy(data_u2_buf, data_u_buf, data_u_sz); //old data
		memcpy(data_u2_buf+data_u_sz, sbuf.data, sbuf.size); //append new data
		data_u2_sz=data_u_sz+sbuf.size;

		/* Write 
		 * truncate LOB (TODO: copy to some safe place first?)
		 * and seek to start
		 */
		r=DatumGetInt32(DirectFunctionCall3(
							lo_lseek,
							Int32GetDatum(inv_fd),
							Int32GetDatum(0),
							Int32GetDatum(SEEK_SET)));
		r=DatumGetInt32(DirectFunctionCall2(
							lo_truncate,
							Int32GetDatum(inv_fd),
							Int32GetDatum(0)));


		//write uncompressed
		if (lobhdr_new.compression==0) {

			//write header
			lobhdr_new.compression=0;
			lobhdr_new.datasize=data_u2_sz;
			//lo_write(inv_fd, &lobhdr_new,sizeof(struct lob_header));
			memcpy(charhdr,&lobhdr_new,sizeof(struct lob_header));
			lo_write(inv_fd, charhdr,sizeof(struct lob_header));

			//write data
			lo_write(inv_fd, data_u2_buf, data_u2_sz ) ;

		}
		//write compressed
		else {
			//compress
			data_c2_sz=LZ4_compress (data_u2_buf, data_c2_buf, data_u2_sz); //returns the number of bytes written in destination buffer 

			//write header
			lobhdr_new.compression=1;
			lobhdr_new.datasize=data_u2_sz;
			//lo_write(inv_fd, &lobhdr_new,sizeof(struct lob_header));
			memcpy(charhdr,&lobhdr_new,sizeof(struct lob_header));
			lo_write(inv_fd, charhdr,sizeof(struct lob_header));

			//write data
			lo_write(inv_fd, data_u2_buf, data_u2_sz ) ;
		}
	}

	/* read uncompressed, write compressed */
	else if (lobhdr.compression == 0 && lobhdr_new.compression > 0) {
		data_u_sz=lo_read(inv_fd, (char *) data_u_buf, BUFSIZE);

		/* Create new data */
		memcpy(data_u2_buf, data_u_buf, data_u_sz); //old data
		memcpy(data_u2_buf+data_u_sz, sbuf.data, sbuf.size); //append new data
		data_u2_sz=data_u_sz+sbuf.size;

		/* Seek to start and truncate */
		r=DatumGetInt32(DirectFunctionCall3(
							lo_lseek,
							Int32GetDatum(inv_fd),
							Int32GetDatum(0),
							Int32GetDatum(SEEK_SET)));
		r=DatumGetInt32(DirectFunctionCall2(
							lo_truncate,
							Int32GetDatum(inv_fd),
							Int32GetDatum(0)));



		//compress
		data_c2_sz=LZ4_compress (data_u2_buf, data_c2_buf, data_u2_sz); //returns the number of bytes written in destination buffer 

		//write header
		lobhdr_new.compression=1;
		lobhdr_new.datasize=data_u2_sz;
		//lo_write(inv_fd, &lobhdr_new,sizeof(struct lob_header));
		memcpy(charhdr,&lobhdr_new,sizeof(struct lob_header));
		lo_write(inv_fd, charhdr,sizeof(struct lob_header));

		//write data
		lo_write(inv_fd, data_u2_buf, data_u2_sz ) ;

	}

	elog(INFO, "olddata u size is %d",data_u_sz);
	elog(INFO, "olddata c size is %d",data_c_sz);


	/*****************/
	/* Cleanup	     */
	/*****************/

	//close LO
	DirectFunctionCall1(lo_close, Int32GetDatum(inv_fd));

	//destroy msgpack
	msgpack_sbuffer_destroy(&sbuf);

	pfree(charhdr);
	pfree(c_arg_value);
	pfree(c_arg_quality);


    PG_RETURN_VOID();
}

//copy a PG varchar to c string. (PG strings have a header and are not null-terminated)
void text2cstring(text* t, char **s)
{
    int32 sz = VARSIZE(t)-VARHDRSZ; //VARHDRSZ = 4 


	*s = palloc(sz+1);
	memcpy(*s,VARDATA(t),sz);
	(*s)[sz] = '\0';
}


