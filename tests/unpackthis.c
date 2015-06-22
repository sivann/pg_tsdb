#include <msgpack.h>
#include <stdio.h>
#include <assert.h>

void unpack(char const* buf, size_t len) {
    /* buf is allocated by client. */
    msgpack_unpacked result;
    size_t off = 0;
    msgpack_unpack_return ret;
    int i = 0;
    msgpack_unpacked_init(&result);
    ret = msgpack_unpack_next(&result, buf, len, &off);
    while (ret == MSGPACK_UNPACK_SUCCESS) {
        msgpack_object obj = result.data;

        /* Use obj. */
        printf("Object no %d:\n", i++);
        msgpack_object_print(stdout, obj);
        printf("\n");
        /* If you want to allocate something on the zone, you can use zone. */
        /* msgpack_zone* zone = result.zone; */
        /* The lifetime of the obj and the zone,  */

        ret = msgpack_unpack_next(&result, buf, len, &off);
    }
    msgpack_unpacked_destroy(&result);

    if (ret == MSGPACK_UNPACK_CONTINUE) {
        printf("All msgpack_object in the buffer is consumed.\n");
    }
    else if (ret == MSGPACK_UNPACK_PARSE_ERROR) {
        printf("The data in the buf is invalid format.\n");
    }
}

int main(int argc, char **argv) {

	FILE * fp, *fpw;
	long lSize;
	char *buffer;


	if (argc<2) {
		printf("Usage: unpackthis <file>\n");
		exit(1);
	}

	fp=fopen(argv[1],"r");
	if( !fp ) {
		perror(argv[1]);
		exit(1);
	};

	fseek( fp , 0L , SEEK_END);
	lSize = ftell( fp );
	rewind( fp );

	buffer = calloc( 1, lSize+1 );
	if( !buffer ) fclose(fp),perror("malloc"),exit(1);

	if( 1!=fread( buffer , lSize, 1 , fp)) {
		  perror("fread:");
		  exit(1);
	}
	fclose(fp);


    msgpack_sbuffer sbuf;
    msgpack_sbuffer_init(&sbuf);


    unpack(buffer, lSize);
    msgpack_sbuffer_destroy(&sbuf);

	free(buffer);
    return 0;
}

/* Output */

/*
Object no 1:
[1, true, "example"]
Object no 2:
"second"
Object no 3:
[42, false]
*/
