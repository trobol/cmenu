

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
	uint8_t end;
} TTF_Point;

typedef struct TTF_Character {
    char character;
	int min_x, min_y;
	int max_x, max_y;
	size_t points_count;
	TTF_Point points[];
} TTF_Character;



typedef struct TTF_FontData {
    TTF_Character* characters;
} TTF_FontData;


