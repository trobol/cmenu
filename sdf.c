#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void bitmap1b_write(const char* path, uint8_t* data, uint32_t width, uint32_t height);
void sdf_shape(const char* path, int16_t min_x, int16_t min_y, int16_t max_x, int16_t max_y, uint16_t* points, uint16_t* endpoints, uint32_t endpoints_count) {
	uint32_t width = max_x - min_x;
	uint32_t height = max_y - min_y;


	uint32_t num_pts = endpoints[endpoints_count-1]+1;
	uint16_t* points_buf = malloc(num_pts * 2);
	uint8_t* pixels = malloc(width * height);
	for (uint32_t y = 0; y < height; y++)
	for (uint32_t x = 0; x < width; x++) {
		
	




	}
	




	bitmap1b_write(path, pixels, width, height);

}


typedef struct __attribute__ ((packed)) BMP_Header {
	uint16_t id;
	uint32_t size;
	uint32_t unused;
	uint32_t offset;
} BMP_Header;

typedef struct __attribute__ ((packed)) DIB_Header {
	uint32_t bytes;
	int32_t width;
	int32_t height;
	uint16_t planes;
	uint16_t bits;
	uint32_t compression;
	uint32_t raw_size;
	uint32_t res_width;
	uint32_t res_height;
	uint32_t colors;
	uint32_t imp_colors;
} DIB_Header;


void write_bitmap_header(FILE* fp, uint32_t width, uint32_t height, uint16_t bits, uint32_t raw_size) {
	
	uint32_t header_bytes = sizeof(BMP_Header) + sizeof(DIB_Header);

	BMP_Header bmp_header = (BMP_Header) {
		.id=0x4D42,
		.size= header_bytes + raw_size,
		.unused = 0,
		.offset = header_bytes
	};

	DIB_Header dib_header = (DIB_Header){
		.bytes=sizeof(DIB_Header),
		.width=width,
		.height=-height,
		.planes=1,
		.bits=bits,
		.compression=0,
		.raw_size=raw_size,
		.res_width=2835,
		.res_height=2835,
		.colors=0,
		.imp_colors=0,
	};

	

	fwrite(&bmp_header, sizeof(bmp_header), 1, fp);
	fwrite(&dib_header, sizeof(DIB_Header), 1, fp);
}



void bitmap1b_write(const char* path, uint8_t* data, uint32_t width, uint32_t height) {
	FILE* fp;
	fp = fopen(path, "wb");


	uint32_t bmp_size = width * height;
	
	uint32_t bytes = 3;
	uint32_t data_line_size = width*bytes;
	uint32_t padding = (4 - (data_line_size % 4)) % 4;
	uint32_t line_size = data_line_size+padding;
	uint32_t raw_size = line_size*height;
	
	write_bitmap_header(fp, width, height, bytes*8, raw_size);


	uint8_t* buf = malloc(line_size);
	// write data in inverse order (thats how bitmaps work)
	for(uint32_t i = 0; i < height; i++) {
		uint8_t* buf_itr = buf;
		uint8_t* line_itr = data + (i * width);
		uint8_t* line_end = line_itr + width;
		while(line_itr < line_end) {
			buf_itr[0] = buf_itr[1] = buf_itr[2] = *line_itr;
			buf_itr += 3;
			line_itr++;
		}
		fwrite(buf, line_size, 1, fp);
	}

	free(buf);
	fclose(fp);
}