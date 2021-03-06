#include <stdio.h>
#include "ttf.h"
#include "sdf.h"


int main(int argc, char *argv[]) {
    if (argc < 2) { puts("missing path"); return 1; }

	TTF_Character* first_char;
	size_t buffer_len;

    TTF_FontData* fdata = read_ttf(argv[1], "e⠝dbagfc");

	if (fdata == NULL) {
		puts("FAILED TO LOAD FONT");
		return 1;
	}

	uint32_t index = find_glyph_index(fdata, 'A');
	if (index == 0) {
		puts("COULDNT SELECT CHARACTER");
		return 1;
	}

	printf("%u\n", index);
	/*
	const char* bmp_path = "out.bmp";
	sdf_shape(bmp_path, select_char->min_x, select_char->min_y, select_char->max_x, select_char->max_y,
				select_char->points, select_char->endpoints, select_char->endpoints_count);
		*/
    return 0;

    

}