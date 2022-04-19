#include <stdio.h>
#include "ttf.h"
#include "sdf.h"


int main(int argc, char *argv[]) {
    if (argc < 2) { puts("missing path"); return 1; }


	TTF_Character* first_char;
	size_t buffer_len;

    read_ttf(argv[1], &first_char, &buffer_len);
    
	const char* bmp_path = "out.bmp";
	sdf_shape(bmp_path, first_char->min_x, first_char->min_y, first_char->max_x, first_char->max_y,
				first_char->points, first_char->endpoints, first_char->endpoints_count);

    return 0;

    

}