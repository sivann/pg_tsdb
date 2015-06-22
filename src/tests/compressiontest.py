#!/usr/bin/python
import csv
import json
import os
import os.path
import time
import msgpack
import sys
import datetime
import dateutil.parser
import dateutil.tz
import pytz
import zlib
import lz4

datadir="comprtest/"

#day,week,month,3months
rowlimit=[1,4,24,96,672,2880,8640]

for rlimit in rowlimit:
	print 'Processing with %s rows per file' % rlimit
	with open('ffd92b88-146b-11e2-8cd1-00163e856739.tsv', 'rb') as csvfile:
		reader = csv.reader(csvfile,delimiter='\t')
		rowno=0
		for row in reader:
			#parse row
			rowno+=1
			msid=row[1]
			mtime=row[2]
			rtime=row[3]
			mvalue=row[4]
			mqual=row[5]

			if rowno % 1000 == 0:
				print rowno, msid, 'rtime:',rtime, 'mtime:',mtime

			# parse mtime 
			mtime_timestamp=dateutil.parser.parse(mtime)
			mtime_month=mtime_timestamp.strftime("%Y-%m")
			mtime_day=mtime_timestamp.strftime("%Y-%m-%d")
			mtime_time=mtime_timestamp.strftime("%d%H%M%S")

			# parse rtime and convert to UTC (to avoid having +xxx tz notation)
			rtime_timestamp=dateutil.parser.parse(rtime)
			rtime_timestamp_utc=rtime_timestamp.astimezone(tz=pytz.utc)
			rtime_date=rtime_timestamp_utc.strftime("%Y%m%d%H%M%S")


			# Mtime time delta from mtime month start in seconds. Always positive.
			mtime_timestamp_epoch=mtime_timestamp.strftime('%s')
			mtime_month_epoch=datetime.datetime.strptime(mtime_month, '%Y-%m').strftime("%s")
			mtime_timediff_seconds=long(mtime_timestamp_epoch) - long(mtime_month_epoch)


			# Rtime time delta from mtime month start in seconds. Should be positive, but can be negative.
			rtime_timestamp_epoch=rtime_timestamp.strftime('%s')
			rtime_timediff_seconds=long(rtime_timestamp_epoch) - long(mtime_month_epoch)

			#row_dict={'m':msid,'v':mvalue,'r':rtime_date, 't':mtime_time,'q':mqual}
			row_dict={'r':rtime_timediff_seconds, 't':mtime_timediff_seconds, 'v':mvalue,'q':mqual}

			#print row_dict

			# read file
			#outfname=msid+'-'+mtime_month
			outfname=msid+'-'+str(rlimit)

			try:
				f=open(datadir+outfname,"r+")
			except IOError as e:
				# if not exists create file
				f=open(datadir+outfname,"w+")



			try:
				f1=open(datadir+outfname+'-uc',"r+")
			except IOError as e:
				# if not exists create file
				f1=open(datadir+outfname+'-uc',"w")


			f.seek(0)
			f1.seek(0)

			prev_data=f.read()
			if len(prev_data) > 2 :
				#prev_data=zlib.decompress(prev_data)
				prev_data=lz4.loads(prev_data)
				prev_data_unpacked=msgpack.unpackb(prev_data,use_list=True);
			else:
				prev_data_unpacked=list()

			#print 'Prev Unpacked:',prev_data_unpacked,'\n'
			#print 'New Unpacked:',prev_data_unpacked,'\n'
			prev_data_unpacked.append(row_dict)
			packed_new=msgpack.packb(prev_data_unpacked)
			#packed_new_z=zlib.compress(packed_new)
			packed_new_z=lz4.dumps(packed_new)

			datasize=sys.getsizeof(json.dumps(prev_data_unpacked))

			#print rowno,datasize,datasize/rowno,json.dumps(prev_data_unpacked)

			datasize_packed=sys.getsizeof(packed_new)
			datasize_packed_compressed=sys.getsizeof(packed_new_z)

			f.seek(0)
			f.write(packed_new_z)
			f.close()

			f1.seek(0)
			f1.write(json.dumps(prev_data_unpacked))
			f1.close()

			#if rowno == 10000000:
			#	sys.exit(0)
			if rowno == rlimit:
				print '*** Json:%d, Packed:%d, PackCompressed:%d, compr/json:%d '  \
				% (datasize, datasize_packed, datasize_packed_compressed, datasize/datasize_packed_compressed)
				print '*** mean Bytes/TR: Json:%d, Packed:%d, PackCompressed:%d \n'  \
				% (datasize/rlimit, datasize_packed/rlimit, datasize_packed_compressed/rlimit)
				break
	#csv_file.close()



#fout=open("","a+")
#foud.write(xxx)


