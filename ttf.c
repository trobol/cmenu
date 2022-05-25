#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// linux specific headers
#include <endian.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "ttf.h"
/*
sequence of concatenated tables

*/


typedef struct {
	uint16_t format;
	uint32_t length;
	uint8_t data[];
} CMapTable;


struct TTF_FontData {
	// TABLE 'maxp'
	uint16_t versionInt;
	uint16_t versionFrac;
    uint16_t numGlyphs;
    uint16_t maxPoints;
    uint16_t maxContours;
    uint16_t maxComponentPoints;
    uint16_t maxComponentContours;
    uint16_t maxZones;
    uint16_t maxTwilightPoints;
    uint16_t maxStorage;
    uint16_t maxFunctionsDefs;
    uint16_t maxInstructionDefs;
    uint16_t maxStackElements;
    uint16_t maxSizeOfInstructions;
    uint16_t maxComponentElements;
    uint16_t maxComponentDepth;

	// TABLE 'head'
	uint16_t	version_int;
	uint16_t 	version_frac;
	uint16_t	fontRevision_int;
	uint16_t 	fontRevision_frac;	
	uint32_t	checkSumAdjustment;
	uint32_t	magicNumber;
	uint16_t	flags;
	uint16_t	unitsPerEm;
	int64_t		created;
	int64_t		modified;
	int16_t		xMin;
	int16_t		yMin;
	int16_t		xMax;
	int16_t		yMax;
	uint16_t	macStyle;	
	uint16_t	lowestRecPPEM;
	int16_t		fontDirectionHint;
	int16_t		indexToLocFormat;
	int16_t		glyphDataFormat;


	// Internal variables

	// TODO: make alphabet utf8 instead of using full 32 bits
	uint32_t* alphabet; // can be NULL
	uint32_t alphabet_len;


	CMapTable* cmap_table;
	// TODO: add optional alphabet

};

typedef struct {
    uint32_t tag;
    uint32_t checksum;
    uint32_t offset;
    uint32_t length;
} TableDirRecord;


typedef struct {
    uint32_t sfntVersion;
    uint16_t numTables;
    uint16_t searchRange;
    uint16_t entrySelector;
    uint16_t rangeShift;
} TableDir;

/*
uint16_t read16(uint8_t** fp, uint8_t* end);
uint32_t read32(uint8_t** fp, uint8_t* end);
*/

void read_section(uint8_t* ptr, uint8_t* end) {

}

int fread_err(const char* var_name) {
	printf("filed ended unexpectedly while trying to read '%s'\n", var_name);
	return -1;
}


typedef struct {
	int16_t x, y;
} vec2i;
typedef struct {
	// index of first point in simple glyph, start of complex 
	uint16_t body_start; 
	// number of points in a simple glyph, size in bytes of a complex glyph
	uint16_t body_size; 
	vec2i min;
	vec2i max;
} Glyph;

typedef struct {
	TTF_Character* characters;
	TTF_Character* characters_end;
	vec2i* points;
	uint16_t points_count;
} TTF_Data;

int validate_table(uint8_t* fp, const uint8_t* fend, TableDirRecord record);


int read_maxp(uint8_t* fp, uint32_t length, TTF_FontData* fdata);
int read_glyf(uint8_t* fp, uint32_t length, uint32_t* offsets, TTF_FontData* fdata, TTF_Data* data);
int read_loca(uint8_t* fp, uint32_t length, uint32_t* offsets, uint16_t num_glyphs, int16_t indexToLocFormat);
int read_head(uint8_t* fp, uint32_t length, TTF_FontData* fdata);

uint8_t* read_file(const char* path, uint64_t* length);
void free_file(long length);


#define MIN(a, b) ((a) < (b) ? (a) : (b)) 
#define MAX(a, b) ((a) > (b) ? (a) : (b)) 


#define readcpy(var, type) if (fptr + sizeof(type) > fend) goto error_eof; (var) = *(type*)fptr; fptr += sizeof(type);
#define read64(var) if (fptr + 8 > fend) goto error_eof; (var) = ((uint64_t)fptr[0] << 56) | ((uint64_t)fptr[1] << 48) | ((uint64_t)fptr[2] << 40) | ((uint64_t)fptr[3] << 32) | ((uint64_t)fptr[4] << 24) | ((uint64_t)fptr[5] << 16) | ((uint64_t)fptr[6] << 8) | (uint64_t)fptr[7]; fptr += 8;
#define read32(var) if (fptr + 4 > fend) goto error_eof; (var) = (fptr[0] << 24) | (fptr[1] << 16) | (fptr[2] << 8) | fptr[3]; fptr += 4;
#define read16(var) if (fptr + 2 > fend) goto error_eof; (var) = (fptr[0] << 8) | fptr[1]; fptr += 2;


/*
#define read_memcpy(dst, size) if (fp + (size) > fend) return fread_err(#dst); memcpy(dst, fp, size); fp += size;
#define read_byte(var)  if (fp + 1 > fend) return fread_err(#var); (var) = *fp; fp++;
#define read_uint16(var) if (fp + 2 > fend) return fread_err(#var); (var) = be16toh(*((uint16_t*)fp)); fp += 2;
#define read_uint32(var) if (fp + 4 > fend) return fread_err(#var); (var) = be32toh(*((uint32_t*)fp)); fp += 4;
#define read_int32(var) if (fp + 4 > fend) return fread_err(#var); (var) = b
#define read_int16(var) 

*/

typedef struct TableDirectory {
union {
	TableDirRecord required_records[8];
struct {
	TableDirRecord maxp;
	TableDirRecord glyf;
	TableDirRecord cmap;
	TableDirRecord loca;
	TableDirRecord name;
	TableDirRecord head;
	TableDirRecord hhea;
	TableDirRecord hmtx;
};
};
} TableDirectory;

#define MAKE_TAG(a,b,c,d) ((uint32_t) (d<<24) | (c<<16) | (b<<8) | (a) )

#define TAG_MAXP MAKE_TAG('m', 'a', 'x', 'p')
#define TAG_GLYF MAKE_TAG('g', 'l', 'y', 'f')
#define TAG_CMAP MAKE_TAG('c', 'm', 'a', 'p')
#define TAG_LOCA MAKE_TAG('l', 'o', 'c', 'a')
#define TAG_NAME MAKE_TAG('n', 'a', 'm', 'e')
#define TAG_HEAD MAKE_TAG('h', 'e', 'a', 'd')
#define TAG_HHEA MAKE_TAG('h', 'h', 'e', 'a')
#define TAG_HMTX MAKE_TAG('h', 'm', 't', 'x')

TableDirRecord* match_dir_tag(TableDirectory* dir, uint32_t tag) {
	switch(tag) {
		case TAG_MAXP:
			return &dir->maxp;
		case TAG_GLYF:
			return &dir->glyf;
		case TAG_CMAP:
			return &dir->cmap;
		case TAG_LOCA:
			return &dir->loca;
		case TAG_NAME:
			return &dir->name;
		case TAG_HEAD:
			return &dir->head;
		case TAG_HHEA:
			return &dir->hhea;
		case TAG_HMTX:
			return &dir->hmtx;
	}

	return NULL;
}

int read_directory(uint8_t* fptr, uint8_t* fend, TableDirectory* dir) {

	uint16_t numTables;
	fptr += 4; // sfntVersion
	read16(numTables);
	fptr += 2; // searchRange
	fptr += 2; // entrySelector
	fptr += 2; // rangeShift

	dir->maxp.tag = TAG_MAXP;
	dir->glyf.tag = TAG_GLYF;
	dir->cmap.tag = TAG_CMAP;
	dir->loca.tag = TAG_LOCA;
	dir->name.tag = TAG_NAME;
	dir->head.tag = TAG_HEAD;
	dir->hhea.tag = TAG_HHEA;
	dir->hmtx.tag = TAG_HMTX;

	for (uint16_t i = 0; i < numTables; i++) {
		uint32_t tag, checksum, offset, length;
		readcpy(tag, uint32_t); // reading raw bytes so no endian swap
		read32(checksum);
		read32(offset);
		read32(length);

		TableDirRecord* record = match_dir_tag(dir, tag);

		if (record != NULL) {
			record->tag = tag;
			record->checksum = checksum;
			record->offset = offset;
			record->length = length;
			
		}
	}
	return 0;
error_eof:

	return 1;
	
}

uint32_t find_glyph_index(TTF_FontData* fdata, char character) {
	
	uint16_t code = (uint16_t)character;
	uint16_t format = fdata->cmap_table->format;
	

	if (format == 4) {
		uint16_t* table = fdata->cmap_table->data;
		
		uint16_t segCountX2, searchRange, entrySelector, rangeShift;
		uint16_t* endCodeArray, *startCodeArray, *idRangeOffsetArray, *glyphIndexArray;
		int16_t* idDeltaArray;
		uint16_t segCount;

		segCountX2 = *table++;
		searchRange = *table++;
		entrySelector = *table++;
		rangeShift = *table++;

		segCount = segCountX2 / 2;

		printf("segCount: %hu\n", segCount);
		endCodeArray = table;
		table += segCount;

		table += 1; // reserved pad

		startCodeArray = table;
		table += segCount;

		idDeltaArray = (int16_t*)table;
		table += segCount;

		idRangeOffsetArray = table;
		table += segCount;

		glyphIndexArray = table;
		table += segCount;

		for (uint32_t i = 0; i < segCount; i++) {
			printf("<%6hu> <%6hu> %6hi %6hu\n", endCodeArray[i], startCodeArray[i], idDeltaArray[i] , idRangeOffsetArray[i] );
		}


		int segIdx = -1;
		for (int i = 0; i < segCount; i++) {
			if (endCodeArray[i] >= code) {
				segIdx = i;
				break;
			}
		}

		if (segIdx == -1) return 0;

		printf("segIdx: %i, ", segIdx);
		
		uint16_t startCode = startCodeArray[segIdx];
		int16_t idDelta = idDeltaArray[segIdx];
		uint16_t idRangeOffset = idRangeOffsetArray[segIdx];

		if (startCode > code) return 0;

		uint32_t index = (code - startCode) + (idRangeOffset / 2) + segIdx;

		uint32_t offset = idRangeOffset == 0 ? code : idRangeOffsetArray[index];

		if (offset == 0) return 0;
		printf("index: %u, idRangeOffset: %hu, idDelta: %hi\n", index, index, idDelta);

		return offset + idDelta;

	}
}

uint8_t utf8_code_len(uint8_t first_char);
inline uint8_t utf8_code_len(uint8_t first_char) {
	uint8_t size = 1;
	size += first_char >= 0b11000000;
	size += first_char >= 0b11100000;
	size += first_char >= 0b11110000;
	return size;
}

int create_alphabet(TTF_FontData* fontdata, const char* req_alphabet) {
	fontdata->alphabet = NULL;
	fontdata->alphabet_len = 0;

	if (req_alphabet == NULL) 
		return 0;

	// find string length and character count
	// check for any invalid unicode characters that would overrun end of string
	uint32_t char_count = 0;
	uint32_t req_len = 0;

	for(uint8_t* cptr = (uint8_t*)req_alphabet; *cptr != 0; char_count++) {
		uint8_t byte_count = utf8_code_len(*cptr);
		while(byte_count > 0) {
			if (cptr == 0) return -1;
			cptr++, req_len++, byte_count--;
		}
	}

	uint32_t* alphabet = malloc(char_count * sizeof(uint32_t));

	uint8_t* cptr = (uint8_t*)req_alphabet;
	uint8_t c;
	for(uint32_t i = 0; i < char_count; i++) {
		uint8_t clen = utf8_code_len(*cptr);
		uint32_t value = 0;
		while(clen-- > 0) {
			c = *(cptr++);
			value = (value << 8) | c;	
		}
		alphabet[i] = value;
	}


	// insertion sort
	for (int i = 0; i < char_count; i++)
	for (int j = i; j > 0 && alphabet[j] < alphabet[j-1]; j--) {
		uint32_t tmp = alphabet[j];
		alphabet[j] = alphabet[j-1];
		alphabet[j-1] = tmp;
		
	}

	/*
	char* test = malloc(req_len+1);
	char* tptr = test;
	for(int i = 0; i < char_count; i++) {
		char* c = (char*)(alphabet + i); 
		printf("%u: %hhi %hhi %hhi %hhi\n", alphabet[i], c[0], c[1], c[2], c[3]);
		for(int j = 3; j > -1; j--) {
			if (c[j] != 0) {
				*(tptr++) = c[j];
			}
		}
	}

	test[req_len] = 0;

	puts(test);
	*/
	
	fontdata->alphabet = alphabet;
	fontdata->alphabet_len = char_count;

	return 0;
}

TTF_FontData* read_ttf(const char* path, const char* requested_alphabet) {

	TTF_FontData* fdata = malloc(sizeof(TTF_FontData));

	long file_len;
	uint8_t* filemem = read_file(path, &file_len);
    if (filemem == NULL) { puts("unable to open file"); return NULL;}

	int err;

	err = create_alphabet(fdata, requested_alphabet);
	if (err != 0) return NULL;
	// sort alphabet

	uint8_t* fp = filemem;
	uint8_t* fptr = fp;
	uint8_t* fend = fp + file_len;
	

	TableDirectory dir;
	 
	err = read_directory(fptr, fend, &dir);
	if (err != 0) return NULL;


	
	err = read_maxp(filemem + dir.maxp.offset, dir.maxp.length, fdata);
	if (err != 0) return NULL;

	printf("version: %hu.%hu\n", fdata->versionInt, fdata->versionFrac);

	printf("glyph count: %hu\n", fdata->numGlyphs);
	printf("max contours: %hu\n", fdata->maxContours);
	printf("max points: %hu\n", fdata->maxPoints);
	printf("max component depth: %hu\n", fdata->maxComponentDepth);

	

	err = read_head(filemem + dir.head.offset, dir.head.length, fdata);
	if (err != 0) return NULL;

	printf("format: %hi\n", fdata->indexToLocFormat);
	err = read_cmap(filemem + dir.cmap.offset, dir.cmap.length, fdata);
	if (err != 0) return NULL;

	uint16_t num_offsets = fdata->numGlyphs + 1;
	uint32_t* loca_offsets = (uint32_t*)calloc(num_offsets, sizeof(uint32_t));
	
	err = read_loca(filemem + dir.loca.offset, dir.loca.length, loca_offsets, num_offsets, fdata->indexToLocFormat);
	if (err != 0) return NULL;

	TTF_Data ttf_data;

	err = read_glyf(filemem + dir.glyf.offset, dir.glyf.length, loca_offsets, fdata, &ttf_data);
	if (err != 0) return NULL;


	//*first_char = ttf_data.characters;

    /*	maxPoints



    'cmap' 	character to glyph mapping
    'glyf' 	glyph data
    'head' 	font header
    'hhea' 	horizontal header
    'hmtx' 	horizontal metrics
    'loca' 	index to location
    'maxp' 	maximum profile - memory amount
    'name' 	naming
    'post' 	PostScript - printer

    */

    // make buffer

    // for every section we want to read
    //   read section to buffer
    //   parse buffer into structures

	TTF_Character* select_char =  ttf_data.characters;
	while(select_char != NULL && select_char->character != 'A')
		select_char = select_char->next;
    
	//if (select_char == NULL){
	//	puts("COULDNT SELECT CHARACTER");
	//	return 1;
	//}
	FILE* svgp = fopen("svg_path.svg", "w");

	fprintf(svgp, "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"20000\" transform=\"scale(1,-1)\" height=\"1500\">");

	TTF_Character* cur_char = ttf_data.characters;
	int x_offset = 200;
	int i = 0;
	while( cur_char->next != NULL) {
		fprintf(svgp, "<!-- %hu -->", cur_char->character);
		fprintf(svgp, "<path stroke=\"black\" fill=\"none\" d=\"\n");
		fprintf(svgp, "M %i 100\n", x_offset);
		
		uint16_t num_coords = cur_char->points_count;
		TTF_Point* coords_buf = cur_char->points;
		uint16_t* endpts_buf = cur_char->endpoints;
		uint16_t contour_index = 0;
		float x, y;
		float perp_x, perp_y;
		float back_x, back_y;
		float len;

		uint16_t contour_start = 0;
		for(int16_t i = 0; i < num_coords; i++) {

			if (i == contour_start) {
				fprintf(svgp, "m %hi %hi\n", coords_buf[contour_start].x, coords_buf[contour_start].y);
				continue;
			}

			fprintf(svgp, "l %hi %hi\n", coords_buf[i].x, coords_buf[i].y);

			

			x = coords_buf[i].x;
			y = coords_buf[i].y;

			perp_x = y;
			perp_y = -x;
			len = sqrtf(perp_x * perp_x + perp_y * perp_y);
			perp_x = (perp_x / len) * 5;
			perp_y = (perp_y / len) * 5;

			back_x = (x / len) * 10;
			back_y = (y / len) * 10;


			fprintf(svgp, "l %f %f", -back_x + perp_x, -back_y + perp_y);
			fprintf(svgp, "m %f %f", perp_x * -2, perp_y * -2);
			fprintf(svgp, "l %f %f", back_x + perp_x, back_y + perp_y);
			
			
			if (i == endpts_buf[contour_index]) {
				fprintf(svgp, "l -20 -20 m 0 40 l 40 -40 m 0 40 l -20 -20"); // draw an x1
				contour_start = endpts_buf[contour_index]+1;
				contour_index++;
			}
		}

	
		x_offset += cur_char->max_x - cur_char->min_x + 20;
		cur_char = cur_char->next;
		i++;
		fprintf(svgp, "\"/>\n");
	}
	

	fprintf(svgp, "</svg>");

	fclose(svgp);



	return fdata;
	
}

uint8_t* read_file(const char* path, uint64_t* length) {
	int fd = open(path, O_RDONLY);
	if (fd == -1) return NULL;
	struct stat sb;
	fstat(fd, &sb);
	
	*length = (uint64_t)sb.st_size;

	return mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
}


int validate_table(uint8_t* fp, const uint8_t* fend, TableDirRecord record) {

	uint8_t* fptr = fp+record.offset;
	if (fptr > fend || fptr + record.length > fend)
		return 1;

	
	
/*
	// TODO: check checksum
	uint32_t calc_checksum = 0;
	uint32_t nLongs = (record.length + 3) / 4;
	while (nLongs-- > 0)
		calc_checksum += *fptr++;	

	if (calc_checksum != record.checksum)
		return 1;

*/
	return 0;
}


int read_maxp(uint8_t* fp, uint32_t length, TTF_FontData* fdata) {
	uint8_t* fptr = fp;
	const uint8_t* fend = fptr + length;

	read16(fdata->versionInt);
	read16(fdata->versionFrac);
    read16(fdata->numGlyphs);
    read16(fdata->maxPoints);
    read16(fdata->maxContours);
    read16(fdata->maxComponentPoints);
    read16(fdata->maxComponentContours);
    read16(fdata->maxZones);
    read16(fdata->maxTwilightPoints);
    read16(fdata->maxStorage);
    read16(fdata->maxFunctionsDefs);
    read16(fdata->maxInstructionDefs);
    read16(fdata->maxStackElements);
    read16(fdata->maxSizeOfInstructions);
    read16(fdata->maxComponentElements);
    read16(fdata->maxComponentDepth);

	return 0;

error_eof:
	return 1;
}


enum SimpleGlyfFlags {
	SGLYFFLAGS_ONCURVE = 1,
	SGLYFFLAGS_SHORTX = 2,
	SGLYFFLAGS_SHORTY = 4,
	SGLYFFLAGS_REPEAT = 8,
	SGLYFFLAGS_SAMEX = 16,
	SGLYFFLAGS_SAMEY = 32,
};

typedef struct {
	uint8_t on_curve : 1;
	uint8_t short_x : 1;
	uint8_t short_y : 1;
	uint8_t repeat : 1;
	uint8_t x_same : 1;
	uint8_t y_same : 1;
	uint8_t reserved : 2;
} SimpleGlyfFlags;


int read_loca(uint8_t* fp, uint32_t length, uint32_t* offsets, uint16_t num_offsets, int16_t indexToLocFormat) {
	uint8_t* fptr = fp;
	uint8_t* fend = fptr + length;

	if (indexToLocFormat == 0) {
		for (uint16_t i = 0; i < num_offsets; i++) {
			read16(offsets[i]);
			offsets[i] *= 2;
		}
	} else {
		for (uint16_t i = 0; i < num_offsets; i++) {
			read32(offsets[i]);
		}
	}

	return 0;

error_eof:
	puts("loca tried to read too much");
	return 1;
}

int read_cmap(uint8_t* fp, uint32_t length, TTF_FontData* fdata) {

	// select "best" table to use


	uint8_t* fptr = fp;
	const uint8_t* fend = fp+length;

	uint16_t cmap_version;
	uint16_t num_subtables;

	read16(cmap_version);
	read16(num_subtables);

	uint16_t plat_id; // Platform identifier
	uint16_t plats_id; // Platform-specific encoding identifier
	uint32_t offset; // Offset of the mapping table

	for(uint16_t i = 0; i < num_subtables; i++) {
		read16(plat_id);
		read16(plats_id);
		read32(offset);
		if (plat_id == 0) goto platform_selected; // microsoft platform
		
	}
	
	puts("cmap: missing microsoft encoding");
	return -1;
	

platform_selected:

	fptr = fp + offset;
	uint16_t table_format;
	read16(table_format);
	

	uint32_t table_len;
	uint32_t table_lang;
	uint16_t reserved;
	switch(table_format) {
		case 0:
		case 2:
		case 4:
		case 6:
			read16(table_len);
			read16(table_lang);
			table_len -= 6;
			break;
		case 8:
		case 10:
		case 12:
		case 13:
		case 14:
			read16(reserved);
			read32(table_len);
			read32(table_lang);
			table_len -= 12; 
			break;
		default:
			// unknown format
			return -1;
	}

	if (table_len == 0 || table_len > (fend-fptr))
		// reported table length is not valid
		return -1;

	printf("%u, %u, %u\n", table_format, table_len, table_lang);
	CMapTable* table = malloc(sizeof(CMapTable) + table_len);
	table->format = table_format;
	table->length = table_len;

	if (table_format == 0) {
		memcpy(table->data, fptr, 256);
		puts("loaded format 1");
	} else if (table_format == 4) {
		for(uint32_t i = 0; i < table_len; i+=2) {
			table->data[i+1] = *(fptr++);
			table->data[i] = *(fptr++);
		}
		puts("loaded format 4");
	}
	
	
	fdata->cmap_table = table;


 return 0;
error_eof:
	return 1;

}

// cross product: a x b
// a.x * b.y - a.y * b.x;

typedef struct { float x, y; } vec2;
typedef struct { vec2 o, d; } ray;

float vec2_cross(vec2 a, vec2 b) {
	return a.x * b.y - a.y * b.x;
}

vec2 vec2_sub(vec2 a, vec2 b) {
	return (vec2){a.x - b.x, a.y - b.y}; 
}

void intersect_lines() {

	ray q;
	ray p;

	// cross product of directions
	//float a = qdx * pdy - pdx * qdy;
	float a = vec2_cross(q.d, p.d);

	if (a == 0) {
		// lines do not cross
		// return
	}

	vec2 s = vec2_sub(q.o, p.o);
	

	float cross_sqd = vec2_cross(s, q.d);
	float cross_spd = vec2_cross(s, p.d);

	float t = cross_sqd / a;
	float u = cross_spd / a;

	
	
}

#define ARG_1_AND_2_ARE_WORDS           (1 << 0)
#define ARGS_ARE_XY_VALUES                      (1 << 1)
#define ROUND_XY_TO_GRID                        (1 << 2)
#define WE_HAVE_A_SCALE                         (1 << 3)
#define RESERVED                                        (1 << 4)
#define MORE_COMPONENTS                         (1 << 5)
#define WE_HAVE_AN_X_AND_Y_SCALE        (1 << 6)
#define WE_HAVE_A_TWO_BY_TWO            (1 << 7)
#define WE_HAVE_INSTRUCTIONS            (1 << 8)
#define USE_MY_METRICS                          (1 << 9)
#define OVERLAP_COMPOUND                        (1 << 10)
#define SCALED_COMPONENT_OFFSET         (1 << 11)
#define UNSCALED_COMPONENT_OFFSET       (1 << 12)


// TODO: I think I should make some sort of allocation management, it should be possible to predict the size of things?

int read_glyf(uint8_t* fp, uint32_t length, uint32_t* offsets, TTF_FontData* fdata, TTF_Data* data) {

	uint8_t* fptr = fp;
	const uint8_t* fend = fptr + length;

	// maximum size of different buffers
	uint16_t max_contours_num = fdata->maxContours;
	uint16_t max_endpts_len = max_contours_num * 2; // 16 bits
	uint16_t max_intr_len = fdata->maxSizeOfInstructions;
	uint16_t max_flags_len = fdata->maxPoints * 2; // not sure if this is actually the max, this is if each point has a flag and a number to continue
	uint16_t max_coords_len = fdata->maxPoints * sizeof(int16_t);


	uint8_t* buffer = (uint8_t*)malloc((size_t)max_intr_len + (size_t)max_endpts_len + (size_t)max_flags_len + (size_t)max_coords_len + (size_t)max_coords_len);
	
	uint16_t* endpts_buf = (uint16_t*)buffer;
	uint8_t* intr_buf = ((uint8_t*)endpts_buf) + max_endpts_len;
	//uint8_t* flags_buf = intr_buf + max_intr_len;
	//vec2i* coords_buf = (vec2i*)(flags_buf + max_flags_len);

	if (fdata->maxPoints > 128) puts("ERROR: point coords will overflow buffer, add a dynamic buffer");
	int16_t coords_buf_x[128];
	int16_t coords_buf_y[128];
	uint8_t flags_buf[128];
	
	// two types of glyphs: simple and compound
	// source: https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6glyf.html
	// Simple glyphs
	// 
	//	| Type			| Name					 | Description
	//  |---------------|------------------------|-------------------
	//	|uint16			| endPtsOfContours[n]	 | Array of last points of each contour; n is the number of contours; array entries are point indices
	//	|uint16			| instrLen				 | Total number of bytes needed for instructions
	//	|uint8			| instructions[instrLen] | Array of instructions for this glyph
	//	|uint8			| flags[variable]		 | Array of flags, the flags control how long the array of flags is (Table 16)
	//	|uint8 or int16	| xCoordinates[]		 | Array of x-coordinates; the first is relative to (0,0), others are relative to previous point
	//	|uint8 or int16	| yCoordinates[]		 | Array of y-coordinates; the first is relative to (0,0), others are relative to previous point
	//

	// Compound Glyphs:
	// for the time being I'm going to just call this an error
	
	uint16_t num_glyphs = fdata->numGlyphs;

	uint32_t x = 0;

	// buffer of TTF_Characters and their data, type is uint8_t* to make pointer math simpler
	size_t buffer_size = 256;
	uint8_t* buffer_start = malloc(buffer_size); // alloc 256 bytes to start

	uint32_t non_empty_glyph_count = 0;
	uint32_t compound_count = 0;
	uint32_t simple_count = 0;
	
	size_t cur_offset = 0;
	for (uint16_t glyph_index = 0; glyph_index < num_glyphs; glyph_index++) {
		
		uint32_t glyph_len = offsets[glyph_index+1] - offsets[glyph_index];
		
		if (glyph_len == 0) {
			printf("empty glyph %hu\n", glyph_index);
			continue;
		}
		
		uint8_t* fptr = fp + offsets[glyph_index];
		const uint8_t* fend = fp + length;

		//printf("glyph #%hu %i %i\n", i, offsets[i], offsets[i+1]);
	
		int16_t num_contours;
		uint16_t intr_len;
		

		int16_t min_x, min_y, max_x, max_y;

		read16(num_contours);
		read16(min_x);
		read16(min_y);
		read16(max_x);
		read16(max_y);

		if (num_contours < 1) { 
			compound_count++;
			//printf("%hu: compound \n", glyph_index);

			uint16_t flags, sub_glyph_index;
			do {
			read16(flags);
			read16(sub_glyph_index);

			//printf(" glyph_index: %hu\n", sub_glyph_index);
			// arg 1
			fptr++;
			if (flags & ARG_1_AND_2_ARE_WORDS) fptr++;

			// arg 2
			fptr++;
			if (flags & ARG_1_AND_2_ARE_WORDS) fptr++;

			if (flags & WE_HAVE_A_SCALE) {
				fptr += 2; // x and y
			}
			else if (flags & WE_HAVE_AN_X_AND_Y_SCALE)
			{
				fptr += 2; // x
				fptr += 2; // y
			}
			else if (flags & WE_HAVE_A_TWO_BY_TWO)
			{
				fptr += 2;
				fptr += 2;
				fptr += 2;
				fptr += 2;
			}

			} while (flags & MORE_COMPONENTS);
			continue; 
		}



		//printf("  num contours: %hi", num_contours);
		
		non_empty_glyph_count++;
		simple_count++;

		
		if (num_contours > max_contours_num) { printf("contour num was greater than reported max %hu %hu", num_contours, max_contours_num); return -1; }
	
		for(int16_t i = 0; i < num_contours; i++) {
			read16(endpts_buf[i]);
		}

		// pointers in TTF_Character are relative and will be fixed to be non-relative at the end of the function
		// 

		uint16_t read_num_pts =  endpts_buf[num_contours-1]+1; // index of last point + 1 = number of points in file
		uint16_t num_pts = read_num_pts + num_contours; // we are adding another point to the end of each contour for ease if use later
		
		// calculate size of character, and ensure there is enough room in buffer
		size_t char_size = sizeof(TTF_Character) + sizeof(TTF_Point) * num_pts + sizeof(uint16_t) * num_contours;
		size_t next_offset = cur_offset + char_size;
		if (next_offset > buffer_size) {
			while(next_offset > buffer_size)
				buffer_size *= 2;
			buffer_start = realloc(buffer_start, buffer_size);
		}

		TTF_Character* cur_char = (TTF_Character*)(buffer_start + cur_offset);
		cur_char->character = glyph_index;
		cur_char->min_x = min_x;
		cur_char->min_y = min_y;
		cur_char->max_x = max_x;
		cur_char->max_y = max_y;
		cur_char->points_count = num_pts;
		cur_char->endpoints_count = num_contours;
		// not going to bother setting ptrs now, will be set later once we are no longer resizing



		read16(intr_len);
		if (intr_len > max_intr_len) { puts("intr len was greater than reported max"); return -1; }
		//read_memcpy(rdr, intr_buf, intr_len);
		// TODO: do something with instructions
		fptr += intr_len; // skip instructions for now

		uint8_t flag_repeat = 0;
		for (uint8_t i = 0; i < read_num_pts; i++) {
			if (flag_repeat == 0) {
				flags_buf[i] = *(fptr++);
				if ( flags_buf[i] & SGLYFFLAGS_REPEAT ) flag_repeat = *(fptr++);
			} else {
				if (i == 0) { puts("tried to repeat first flag"); return -1;}
				flags_buf[i] = flags_buf[i-1];
				flag_repeat--;
			}
		}

		
	
		// these next two sections are obtuse
		// but in my defense they are much faster because they are branchless
		{
			uint16_t j = 0;
			for(uint16_t i = 0; i < read_num_pts; i++) {
				uint8_t flags = flags_buf[i];

				uint16_t sh = ~(((flags & SGLYFFLAGS_SHORTX) >> 1)-1); // every bit is 1 if short flag is set, zero otherwise
				uint16_t n_sm = ((flags & SGLYFFLAGS_SAMEX) >> 4)-1;   // every bit is 1 if same flag is NOT set, zero otherwise

				uint16_t x_0 = fptr[j] & ~sh & n_sm; // zero if not short and not same
				uint16_t x_1 = fptr[j+1] & ~sh & n_sm; // ^
				x_1 += (fptr[j] & sh ^ n_sm) + (1 & n_sm); // if short flag, xor with n_sm computes ones complement, add one if negative for twos complement
				
				j += 2;
				j -= sh & 1;
				j -= (~n_sm & ~sh) & 2;

				coords_buf_x[i] = x_0 << 8 | x_1;
				
			}
			fptr += j;
		}


		{
			uint16_t j = 0;
			for(uint16_t i = 0; i < read_num_pts; i++) {
				uint8_t flags = flags_buf[i];

				uint16_t sh = ~(((flags & SGLYFFLAGS_SHORTY) >> 2)-1);
				uint16_t n_sm = ((flags & SGLYFFLAGS_SAMEY) >> 5)-1;

				uint16_t y_0 = fptr[j] & ~sh & n_sm;
				uint16_t y_1 = fptr[j+1] & ~sh & n_sm;
				y_1 += (fptr[j] & sh ^ n_sm) + (1 & n_sm);

				j += 2;
				j -= sh & 1;
				j -= (~n_sm & ~sh) & 2;

				coords_buf_y[i] = y_0 << 8 | y_1;
			}
			fptr += j;
		}
		
		for (uint16_t i = 0; i < cur_char->endpoints_count; i++)
			cur_char->endpoints[i] = endpts_buf[i] + i+1;

		uint16_t* endpts_itr = cur_char->endpoints;
		vec2i start = (vec2i){coords_buf_x[0],  coords_buf_y[0]};
		vec2i end = (vec2i){0, 0};

		// INSERT ONE MORE LINE AT THE END
		TTF_Point* points = (TTF_Point*)(cur_char->endpoints + cur_char->endpoints_count);
		for (uint16_t in = 0, out = 0; out < num_pts; in++, out++) {
			
			int16_t x;
			int16_t y;

			if (in < read_num_pts) {
				x = coords_buf_x[in];
				y = coords_buf_y[in];
			}
		
			
			if (out == *endpts_itr) {
				
				int16_t new_pt_x = start.x - end.x;
				int16_t new_pt_y = start.y - end.y;
				points[out].x = new_pt_x;
				points[out].y = new_pt_y;
				endpts_itr++;
				out++;
				
				end.x = start.x;
				end.y = start.y;
				
				x -= new_pt_x;
				y -= new_pt_y;
				
				
				start.x += x;
				start.y += y;

				
				
				if (out >= num_pts)
					break;
				
			} else {

			}
			
	

			end.x += x;
			end.y += y;
			

			points[out].x = x; 
			points[out].y = y;

			
		}
		


		cur_offset = next_offset;
	}

	TTF_Character* cur_char = (TTF_Character*)buffer_start;
	for (int i = 0; i < non_empty_glyph_count; i++) {
		// endpoints are the memory right after character
		cur_char->points = (TTF_Point*)(cur_char->endpoints + cur_char->endpoints_count);
		cur_char->next = (TTF_Character*)(cur_char->points + cur_char->points_count);

		cur_char = cur_char->next;
	}

	cur_char->next = NULL;

	data->characters = (TTF_Character*)buffer_start;
	data->characters_end = (TTF_Character*)(buffer_start + buffer_size);

	printf("compound glyphs: %u\nsimple glyphs: %u\n", compound_count, simple_count);

	return 0;
error_eof:
	puts("\nerr: glyph tried to read outside its bounds");

	
}


int read_head(uint8_t* fp, uint32_t length, TTF_FontData* fdata) {
	uint8_t* fptr = fp;
	const uint8_t* fend = fptr + length;
	
	read16(fdata->version_int);
	read16(fdata->version_frac);
	read16(fdata->fontRevision_int);
	read16(fdata->fontRevision_frac);	
	read32(fdata->checkSumAdjustment);
	read32(fdata->magicNumber);
	read16(fdata->flags);
	read16(fdata->unitsPerEm);
	read64(fdata->created);
	read64(fdata->modified);
	read16(fdata->xMin);
	read16(fdata->yMin);
	read16(fdata->xMax);
	read16(fdata->yMax);
	read16(fdata->macStyle);	
	read16(fdata->lowestRecPPEM);
	read16(fdata->fontDirectionHint);
	read16(fdata->indexToLocFormat);
	read16(fdata->glyphDataFormat);

	return 0;
error_eof:
	return 1;
}