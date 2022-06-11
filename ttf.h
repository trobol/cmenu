#include <stdint.h>

/*
Should have internal structure that gets combined into one allocation

*/


/*
Font data structure



- array that maps UTF-8 codes to to character indices
- array of characters
- array of glyphs
- array of lines

*/

typedef struct TTF_Line {
    int start_x, start_y;
    int end_x, end_y;
} TTF_Line;

typedef struct TTF_Point {
	int16_t x, y;
} TTF_Point;


typedef struct TTF_Character TTF_Character;
struct TTF_Character {
    uint16_t character;
	int16_t min_x, min_y;
	int16_t max_x, max_y;
	uint16_t points_count;
	uint16_t endpoints_count;
	TTF_Point* points;
	uint16_t* endpoints;
};

typedef struct TTF_FontData TTF_FontData;

typedef struct TTF_CharLoader TTF_CharLoader;

typedef struct TTF_CharInfo {
	
} TTF_CharInfo;


TTF_FontData* ttf_load(const char* path, const char* alphabet);
uint32_t find_glyph_index(TTF_FontData* fdata, char character);

TTF_Character* ttf_char_new(TTF_FontData* fdata);
int ttf_load_character(TTF_FontData* fdata, struct TTF_Character* out_character, uint32_t char_code);