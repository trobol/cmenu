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
    uint32_t tableTag;
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

TableDirRecord find_table_record(const char* tag_str, TableDirRecord* records, uint32_t* tags, uint16_t count);

typedef struct {
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
} TableMAXP;

typedef struct {
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
} TableHEAD;

typedef struct {
	uint8_t* ptr;
	uint8_t* end;
} reader;


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



int read_maxp(uint8_t* fp, uint32_t length, TableMAXP* maxp);
int read_glyf(uint8_t* fp, uint32_t length, uint32_t* offsets, TableMAXP maxp, TTF_Data* data);
int read_loca(uint8_t* fp, uint32_t length, uint32_t* offsets, uint16_t num_glyphs, int16_t indexToLocFormat);
int read_head(uint8_t* fp, uint32_t length, TableHEAD* head);

uint8_t* read_file(const char* path, uint64_t* length);
void free_file(long length);




uint8_t read_byte(reader* rdr) {
	if (rdr->ptr + 1 > rdr->end) exit(1);
	uint8_t a = rdr->ptr[0];
	rdr->ptr += 1;
	return a;
}

void read_memcpy(reader* rdr, void* dst, size_t size) {
	if (rdr->ptr + size > rdr->end) exit(1);
	
	memcpy(dst, rdr->ptr, size);
	rdr->ptr += size;
}



uint16_t read_uint16(reader* rdr) {
	if (rdr->ptr + 2 > rdr->end) exit(1);
	uint8_t a = rdr->ptr[0];
	uint8_t b = rdr->ptr[1];
	rdr->ptr += 2;
	return (a << 8) | b;
}

uint32_t read_uint32(reader* rdr) {
	if (rdr->ptr + 4 > rdr->end) exit(1);
	uint8_t a = rdr->ptr[0];
	uint8_t b = rdr->ptr[1];
	uint8_t c = rdr->ptr[2];
	uint8_t d = rdr->ptr[3];
	rdr->ptr += 4;
	return (a << 24) | (b << 16) | (c << 8) | d; 
}

int16_t read_int16(reader* rdr) {
	if (rdr->ptr + 2 > rdr->end) exit(1);
	uint8_t a = rdr->ptr[0];
	uint8_t b = rdr->ptr[1];
	rdr->ptr += 2;
	return (a << 8) | b;
}

int32_t read_int32(reader* rdr) {
	if (rdr->ptr + 4 > rdr->end) exit(1);
	uint8_t a = rdr->ptr[0];
	uint8_t b = rdr->ptr[1];
	uint8_t c = rdr->ptr[2];
	uint8_t d = rdr->ptr[3];
	rdr->ptr += 4;
	return (a << 24) | (b << 16) | (c << 8) | d; 
}

int32_t read_int64(reader* rdr) {
	if (rdr->ptr + 8 > rdr->end) exit(1);
	
	
	int64_t i = ((uint64_t)rdr->ptr[0] << 56) | ((uint64_t)rdr->ptr[1] << 48) | ((uint64_t)rdr->ptr[2] << 40) | ((uint64_t)rdr->ptr[3] << 32) | ((uint64_t)rdr->ptr[4] << 24) | ((uint64_t)rdr->ptr[5] << 16) | ((uint64_t)rdr->ptr[6] << 8) | (uint64_t)rdr->ptr[7]; 
	rdr->ptr += 8;
	return i;
}



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
TableHEAD g_head;


TableDirRecord find_required_record(const char* tag_str, TableDirRecord* records, uint32_t* tags, uint16_t count) {
	TableDirRecord table = find_table_record(tag_str, records, tags, count);
	if (table.length == 0) { printf("missing table %s\n", tag_str); exit(1); }
	return table;
}




int read_ttf(const char* path, TTF_Character** first_char, size_t* buffer_len) {


	long file_len;
	uint8_t* filemem = read_file(path, &file_len);
    if (filemem == NULL) { puts("unable to open file"); return 1;}

	uint8_t* fp = filemem;
	uint8_t* fend = fp + file_len;
	reader rdr[1] = { (reader){fp, fp+file_len} };

	TableDir td;
	read_memcpy(rdr, &td, sizeof(TableDir));

	td.sfntVersion 	 = be32toh(td.sfntVersion);
    td.numTables	 = be16toh(td.numTables);
    td.searchRange 	 = be16toh(td.searchRange); 
    td.entrySelector = be16toh(td.entrySelector);
    td.rangeShift 	 = be16toh(td.rangeShift);


    printf("sfnt: %x\n", td.sfntVersion);
    printf("num tables: %hu\n",td.numTables);
    printf("search range: %hu\n", td.searchRange);
    printf("entry selector: %hu\n", td.entrySelector);
    printf("range shift: %hu\n", td.rangeShift);

    TableDirRecord* records = (TableDirRecord*)malloc((td.numTables * sizeof(TableDirRecord)) + (td.numTables * sizeof(uint32_t)));
	uint32_t* tags = (uint32_t*)(records + td.numTables);
	
	read_memcpy(rdr, records, sizeof(TableDirRecord) * td.numTables);
	

	for(uint16_t i = 0; i < td.numTables; i++) {
		TableDirRecord* r = records+i;
		tags[i] = r->tableTag;
		
		printf("%.4s\n", &r->tableTag);
		// we dont change endianess of tableTag because its basically char[4]
		r->checksum = be32toh( r->checksum);
		r->offset = be32toh(r->offset);
		r->length = be32toh(r->length);
	}
    
   	TableDirRecord maxp = find_table_record("maxp", records, tags, td.numTables);
	TableDirRecord glyf = find_table_record("glyf", records, tags, td.numTables);
	TableDirRecord cmap = find_table_record("cmap", records, tags, td.numTables);
	TableDirRecord loca = find_table_record("loca", records, tags, td.numTables);
	TableDirRecord name = find_table_record("name", records, tags, td.numTables);
	TableDirRecord head = find_table_record("head", records, tags, td.numTables);
	TableDirRecord hhea = find_table_record("hhea", records, tags, td.numTables);
	TableDirRecord hmtx = find_table_record("hmtx", records, tags, td.numTables);

	TableMAXP table_maxp;
	TableHEAD table_head;
	int err; 
	err = read_maxp(filemem + maxp.offset, maxp.length, &table_maxp);
	if (err != 0) return err;

	printf("version: %hu.%hu\n", table_maxp.versionInt, table_maxp.versionFrac);

	printf("glyph count: %hu\n", table_maxp.numGlyphs);
	printf("max contours: %hu\n", table_maxp.maxContours);
	printf("max points: %hu\n", table_maxp.maxPoints);
	printf("max component depth: %hu\n", table_maxp.maxComponentDepth);

	

	err = read_head(filemem + head.offset, head.length, &table_head);
	if (err != 0) return err;

	printf("format: %hi\n", g_head.indexToLocFormat);
	//err = read_cmap(filemem + cmap.offset, cmap.length);
	//if (err != 0) return err;

	uint16_t num_offsets = table_maxp.numGlyphs + 1;
	uint32_t* loca_offsets = (uint32_t*)calloc(num_offsets, sizeof(uint32_t));
	
	err = read_loca(filemem + loca.offset, loca.length, loca_offsets, num_offsets, table_head.indexToLocFormat);
	if (err != 0) return err;

	TTF_Data ttf_data;

	err = read_glyf(filemem + glyf.offset, glyf.length, loca_offsets, table_maxp, &ttf_data);
	if (err != 0) return err;


	*first_char = ttf_data.characters;

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
    
	if (select_char == NULL){
		puts("COULDNT SELECT CHARACTER");
		return 1;
	}
	FILE* svgp = fopen("svg_path.svg", "w");

	fprintf(svgp, "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"20000\" transform=\"scale(1,-1)\" height=\"1500\"> <path stroke=\"black\" fill=\"none\" d=\"");

	TTF_Character* cur_char = select_char;
	int x_offset = 200;
	int i = 0;
	while( cur_char->next != NULL) {
		
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
				fprintf(svgp, "l -20 -20 m 0 40 l 40 -40 m 0 40 l -20 -20"); // draw an x
				contour_start = endpts_buf[contour_index]+1;
				contour_index++;
			}
		}

	
		x_offset += cur_char->max_x - cur_char->min_x + 20;
		cur_char = cur_char->next;
		i++;
	}
	

	fprintf(svgp, "\"/></svg>");

	fclose(svgp);
	
}

uint8_t* read_file(const char* path, uint64_t* length) {
	int fd = open(path, O_RDONLY);
	if (fd == -1) return NULL;
	struct stat sb;
	fstat(fd, &sb);
	
	*length = (uint64_t)sb.st_size;

	return mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
}

void free_file(long length) {

}


// if the record is not found, the record returned is all zero except the tag is set to the requested tag
TableDirRecord find_table_record(const char* tag_str, TableDirRecord* records, uint32_t* tags, uint16_t count) {
    uint32_t tag = *(uint32_t*)tag_str; // this is a bad idea
    
    for(uint16_t i = 0; i < count; i++)
        if (tags[i] == tag) return records[i];
    

    return (TableDirRecord){tag, 0, 0, 0};
}


int read_maxp(uint8_t* fp, uint32_t length, TableMAXP* maxp) {
	reader rdr[1] = { (reader){fp, fp+length} };

	
	read_memcpy(rdr, maxp, sizeof(TableMAXP));

	maxp->versionInt = be16toh(maxp->versionInt);
	maxp->versionFrac = be16toh(maxp->versionFrac);
    maxp->numGlyphs = be16toh(maxp->numGlyphs);
    maxp->maxPoints = be16toh(maxp->maxPoints);
    maxp->maxContours = be16toh(maxp->maxContours);
    maxp->maxComponentPoints = be16toh(maxp->maxComponentPoints);
    maxp->maxComponentContours = be16toh(maxp->maxComponentContours);
    maxp->maxZones = be16toh(maxp->maxZones);
    maxp->maxTwilightPoints = be16toh(maxp->maxTwilightPoints);
    maxp->maxStorage = be16toh(maxp->maxStorage);
    maxp->maxFunctionsDefs = be16toh(maxp->maxFunctionsDefs);
    maxp->maxInstructionDefs = be16toh(maxp->maxInstructionDefs);
    maxp->maxStackElements = be16toh(maxp->maxStackElements);
    maxp->maxSizeOfInstructions = be16toh(maxp->maxSizeOfInstructions);
    maxp->maxComponentElements = be16toh(maxp->maxComponentElements);
    maxp->maxComponentDepth = be16toh(maxp->maxComponentDepth);

	return 0;
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

	if (g_head.indexToLocFormat == 0) {
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

int read_cmap(uint8_t* fp, uint32_t length) {
	reader rdr[1] = { (reader){fp, fp+length} };

	uint16_t cmap_version = read_uint16(rdr);
	uint16_t num_subtables = read_uint16(rdr);

	uint16_t plat_id;
	uint16_t plats_id;
	uint32_t offset;

	for(uint16_t i = 0; i < num_subtables; i++) {
		plat_id = read_uint16(rdr);
		plats_id = read_uint16(rdr);
		offset = read_uint32(rdr);
		if (plat_id == 0) goto platform_selected; // microsoft platform
	}
	
	puts("cmap: missing microsoft encoding");
	return -1;
	

platform_selected:




	return 0;

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


int read_glyf(uint8_t* fp, uint32_t length, uint32_t* offsets, TableMAXP maxp, TTF_Data* data) {
	reader rdr[1] = { (reader){fp, fp+length} };
	
	uint8_t* fptr = fp;
	const uint8_t* fend = fptr + length;

	// maximum size of different buffers
	uint16_t max_contours_num = maxp.maxContours;
	uint16_t max_endpts_len = max_contours_num * 2; // 16 bits
	uint16_t max_intr_len = maxp.maxSizeOfInstructions;
	uint16_t max_flags_len = maxp.maxPoints * 2; // not sure if this is actually the max, this is if each point has a flag and a number to continue
	uint16_t max_coords_len = maxp.maxPoints * sizeof(int16_t);


	uint8_t* buffer = (uint8_t*)malloc((size_t)max_intr_len + (size_t)max_endpts_len + (size_t)max_flags_len + (size_t)max_coords_len + (size_t)max_coords_len);
	
	uint16_t* endpts_buf = (uint16_t*)buffer;
	uint8_t* intr_buf = ((uint8_t*)endpts_buf) + max_endpts_len;
	//uint8_t* flags_buf = intr_buf + max_intr_len;
	//vec2i* coords_buf = (vec2i*)(flags_buf + max_flags_len);

	if (maxp.maxPoints > 64) puts("ERROR: point coords will overflow buffer, add a dynamic buffer");

	int16_t coords_buf_x[64];
	int16_t coords_buf_y[64];
	uint8_t flags_buf[64];
	
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
	
	uint16_t num_glyphs = maxp.numGlyphs;

	uint32_t x = 0;

	// buffer of TTF_Characters and their data, type is uint8_t* to make pointer math simpler
	size_t buffer_size = 256;
	uint8_t* buffer_start = malloc(buffer_size); // alloc 256 bytes to start

	uint32_t non_empty_glyph_count = 0;
	
	size_t cur_offset = 0;
	for (uint16_t glyph_index = 0; glyph_index < num_glyphs; glyph_index++) {
		rdr->ptr = fp + offsets[glyph_index];

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
		if (num_contours < 1) { puts("glyph was a contour, character skipped\n"); continue; }
		//printf("  num contours: %hi", num_contours);
		
		non_empty_glyph_count++;
		read16(min_x);
		read16(min_y);
		read16(max_x);
		read16(max_y);

		
		
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
		cur_char->character = glyph_index-1;
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

		// TODO: this technically reads one two many
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

	return 0;
error_eof:
	puts("\nerr: glyph tried to read outside its bounds");

	
}


int read_head(uint8_t* fp, uint32_t length, TableHEAD* head) {
	reader rdr[1] = { (reader){fp, fp+length} };

	head->version_int = 		read_uint16(rdr);	
	head->version_frac = 		read_uint16(rdr); 	
	head->fontRevision_int = 	read_uint16(rdr);	
	head->fontRevision_frac = 	read_uint16(rdr); 	
	head->checkSumAdjustment = read_uint32(rdr);	
	head->magicNumber = 		read_uint32(rdr);	
	head->flags = 				read_uint16(rdr);	
	head->unitsPerEm = 		read_uint16(rdr);	
	head->created = 			read_int64(rdr);		
	head->modified = 			read_int64(rdr);		
	head->xMin = 				read_int16(rdr);		
	head->yMin = 				read_int16(rdr);		
	head->xMax = 				read_int16(rdr);		
	head->yMax = 				read_int16(rdr);		
	head->macStyle = 			read_uint16(rdr);	
	head->lowestRecPPEM = 		read_uint16(rdr);	
	head->fontDirectionHint = 	read_int16(rdr);		
	head->indexToLocFormat = 	read_int16(rdr);		
	head->glyphDataFormat = 	read_int16(rdr);		

	return 0;
}