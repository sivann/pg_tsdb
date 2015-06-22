

#include <stdio.h>
#include <string.h>

#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>


#pragma pack(push, 1)
struct lob_header {
    uint8_t compression; //0:not compressed, 1: compressed
    uint16_t datasize; //size before compression. Needed for fast decompression using LZ4_decompress_fast
    uint16_t reserved2; //size before compression. Needed for fast decompression using LZ4_decompress_fast
};

#pragma pack(pop)

main() {

	int fd;
	int r,w,x;
	struct lob_header lh;

	fd=open("xx",O_WRONLY|O_CREAT);
	if (fd<0)
		perror("open");


	lh.compression=5;
	lh.datasize=15;
	lh.reserved2=0;

	w=write(fd,&lh,sizeof(struct lob_header));

	if (w<0)
		perror("write");

	printf("Written:%d\n",w);

	close(fd);


	printf("Size:%d\n",sizeof(struct lob_header));

}

// 05 0f 00 73 ff
