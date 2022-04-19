#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void bitmap1b_write(const char* path, uint8_t* data, uint32_t width, uint32_t height);
void sdf_shape(const char* path, int16_t min_x, int16_t min_y, int16_t max_x, int16_t max_y, int16_t* points, uint16_t* endpoints, uint32_t endpoints_count) {
	uint32_t width = max_x - min_x;
	uint32_t height = max_y - min_y;


	uint32_t num_pts = endpoints[endpoints_count-1]+1;
	uint16_t* points_buf = malloc(num_pts * 2);
	uint8_t* pixels = malloc(width * height);
	for (uint32_t pixel_y = 0; pixel_y < height; pixel_y++)
	for (uint32_t pixel_x = 0; pixel_x < width; pixel_x++) { // for every pixel
		
		int32_t p_x = pixel_x+min_x;
		int32_t p_y = height-pixel_y+min_y;
		
		uint16_t num_coords = endpoints[endpoints_count-1]+1;
		uint16_t contour_index = 0;
		float x, y;
		float perp_x, perp_y;
		float back_x, back_y;
		float len;

		int16_t* pts_itr = points;
		int16_t* pts_end = points + num_coords * 2;

		int32_t p0_x = 0;
		int32_t p0_y = 0;

		uint16_t index = 0;
		uint16_t first_pt = 0; // first point in contour

		int32_t closest2 = 9999999999;

		pixels[pixel_x + pixel_y * width] = 0;
		for (; pts_itr < pts_end; pts_itr += 2, index++) {

			
			
			int32_t d_x = pts_itr[0];
			int32_t d_y = pts_itr[1];
		
			/*
			{
					p0_x += d_x;
			p0_y += d_y;
				int32_t diff_x = (p_x - p0_x);
				int32_t diff_y = (p_y - p0_y);
				int32_t dist2 = diff_x * diff_x + diff_y * diff_y;
				if (dist2 < 10) {
					pixels[pixel_x + pixel_y * width] = 255;
					break;
				}
				continue;
			}
			*/
			
			if ( index == first_pt ) {
				p0_x += d_x;
				p0_y += d_y;
				first_pt = endpoints[contour_index]+1;
				contour_index++;
				continue;
			}
			

			int32_t x = (p_x - p0_x);
			int32_t y = (p_y - p0_y);


			int32_t top = x * d_x + y * d_y;
			int32_t btm = d_x * d_x + d_y * d_y;

			float t = (float)top/(float)btm;
			if ( t > 1.0) t = 1.0;
			if ( t < 0.0) t = 0.0;

			int32_t out_x = (float)p0_x + (float)d_x * t;
			int32_t out_y = (float)p0_y + (float)d_y * t;

			int32_t diff_x = (out_x - p_x);
			int32_t diff_y = (out_y - p_y);
			int32_t dist2 = diff_x * diff_x + diff_y * diff_y;



			//if (dist2 < 10) {
			//	pixels[pixel_x + pixel_y * width] = 255;
			//	break;
			//}

			if (dist2 < closest2) {
				closest2 = dist2;
			}

			p0_x += d_x;
			p0_y += d_y;
		}

		pixels[pixel_x + pixel_y * width] = 255 - (closest2 < 255 ? closest2 : 255);
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