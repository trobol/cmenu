#include <stdio.h>
#include "ttf.h"
#include "sdf.h"


int main(int argc, char *argv[]) {
    if (argc < 2) { puts("missing path"); return 1; }


	TTF_Character* first_char;
	size_t buffer_len;

    read_ttf(argv[1], &first_char, &buffer_len);

	TTF_Character* select_char = first_char;
	while(select_char != NULL && select_char->character != 'A')
		select_char = select_char->next;
    
	if (select_char == NULL){
		puts("COULDNT SELECT CHARACTER");
		return 1;
	}

	const char* bmp_path = "out.bmp";
	sdf_shape(bmp_path, select_char->min_x, select_char->min_y, select_char->max_x, select_char->max_y,
				select_char->points, select_char->endpoints, select_char->endpoints_count);

    return 0;

    

}