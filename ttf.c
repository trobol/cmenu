#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// linux specific headers
#include <endian.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
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

int read_ttf(const char* path); 


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
	uint16_t version_frac;
	uint16_t	fontRevision_int;
	uint16_t fontRevision_frac;	
	uint32_t	checkSumAdjustment;
	uint32_t	magicNumber;
	uint16_t	flags;
	uint16_t	unitsPerEm;
	int64_t	created;
	int64_t	modified;
	int16_t	xMin;
	int16_t	yMin;
	int16_t	xMax;
	int16_t	yMax;
	uint16_t	macStyle;	
	uint16_t	lowestRecPPEM;
	int16_t	fontDirectionHint;
	int16_t	indexToLocFormat;
	int16_t	glyphDataFormat;
} TableHEAD;

typedef struct {
	uint8_t* ptr;
	uint8_t* end;
} reader;


int read_maxp(uint8_t* fp, uint32_t length, TableMAXP* maxp);
int read_glyf(uint8_t* fp, uint32_t length);
int read_loca(uint8_t* fp, uint32_t length, uint32_t** offsets);


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

/*
#define read_memcpy(dst, size) if (fp + (size) > fend) return fread_err(#dst); memcpy(dst, fp, size); fp += size;
#define read_byte(var)  if (fp + 1 > fend) return fread_err(#var); (var) = *fp; fp++;
#define read_uint16(var) if (fp + 2 > fend) return fread_err(#var); (var) = be16toh(*((uint16_t*)fp)); fp += 2;
#define read_uint32(var) if (fp + 4 > fend) return fread_err(#var); (var) = be32toh(*((uint32_t*)fp)); fp += 4;
#define read_int32(var) if (fp + 4 > fend) return fread_err(#var); (var) = b
#define read_int16(var) 

*/
TableMAXP g_maxp;
TableHEAD g_head;

TableDirRecord find_required_record(const char* tag_str, TableDirRecord* records, uint32_t* tags, uint16_t count) {
	TableDirRecord table = find_table_record(tag_str, records, tags, count);
	if (table.length == 0) { printf("missing table %s\n", tag_str); exit(1); }
	return table;
}


int read_ttf(const char* path) {


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
		// we dont change endianess of tableTag because its basically byte[4]
		r->checksum = be32toh( r->checksum);
		r->offset = be32toh(r->offset);
		r->length = be32toh(r->length);
    }
    
	union {
		TableDirRecord records[8];
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
	} req_records;
	
    
   	TableDirRecord maxp = find_table_record("maxp", records, tags, td.numTables);
	TableDirRecord glyf = find_table_record("glyf", records, tags, td.numTables);
	TableDirRecord cmap = find_table_record("cmap", records, tags, td.numTables);
	TableDirRecord loca = find_table_record("loca", records, tags, td.numTables);
	TableDirRecord name = find_table_record("name", records, tags, td.numTables);
	TableDirRecord head = find_table_record("head", records, tags, td.numTables);
	TableDirRecord hhea = find_table_record("hhea", records, tags, td.numTables);
	TableDirRecord hmtx = find_table_record("hmtx", records, tags, td.numTables);


	int err; 
	err = read_maxp(filemem + maxp.offset, maxp.length, &g_maxp);
	if (err != 0) return err;

	printf("version: %hu.%hu\n", g_maxp.versionInt, g_maxp.versionFrac);

	printf("glyph count: %hu\n", g_maxp.numGlyphs);
	printf("max contours: %hu\n", g_maxp.maxContours);
	printf("max points: %hu\n", g_maxp.maxPoints);
	printf("max component depth: %hu\n", g_maxp.maxComponentDepth);

	uint32_t* loca_offsets;

	err = read_cmap(filemem + cmap.offset, cmap.length);
	if (err != 0) return err;

	err = read_loca(filemem + loca.offset, loca.length, &loca_offsets);
	if (err != 0) return err;

	err = read_glyf(filemem + glyf.offset, glyf.length);
	if (err != 0) return err;

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



typedef struct {
	uint8_t on_curve : 1;
	uint8_t short_x : 1;
	uint8_t short_y : 1;
	uint8_t repeat : 1;
	uint8_t x_same : 1;
	uint8_t y_same : 1;
	uint8_t reserved : 2;
} SimpleGlyfFlags;


int read_loca(uint8_t* fp, uint32_t length, uint32_t** offsets) {

}

int read_cmap(uint8_t* fp, uint32_t length) {
	reader rdr[1] = { (reader){fp, fp+length} };

	uint16_t cmap_version = read_uint16(rdr);
	uint16_t num_subtables = read_uint16(rdr);

	uint16_t plat_id;
	uint16_t plats_id;
	uint32_t offset = 0;
	uint8_t* end = num_subtables * (2 + 2 + 4);
	while(rdr->ptr < end) {
		plat_id = read_uint16(rdr);
		plats_id = read_uint16(rdr);
		offset = read_uint32(rdr);
		if (plat_id != 3) offset = 0; // microsoft
	}
	if (offset == 0) {
		puts("missing microsoft encoding");
		return -1;
	}

platform_selected:
	// do stuff 



}

int read_glyf(uint8_t* fp, uint32_t length) {
	reader rdr[1] = { (reader){fp, fp+length} };

	// maximum size of different buffers
	uint16_t max_contours_num = g_maxp.maxContours;
	uint16_t max_endpts_len = max_contours_num * 2; // 16 bits
	uint16_t max_intr_len = g_maxp.maxSizeOfInstructions;
	uint16_t max_flags_len = g_maxp.maxPoints * 2; // not sure if this is actually the max, this is if each point has a flag and a number to continue
	uint16_t max_coords_len = g_maxp.maxPoints * 2; // most is if each coords is 2 bytes
	uint8_t* buffer = (uint8_t*)malloc((size_t)max_intr_len + (size_t)max_endpts_len + (size_t)max_flags_len + (size_t)max_coords_len + (size_t)max_coords_len);
	
	uint16_t* endpts_buf = (uint16_t*)buffer;
	uint8_t* intr_buf = ((uint8_t*)endpts_buf) + max_endpts_len;
	uint8_t* flags_buf = intr_buf + max_intr_len;
	int16_t* xcoords_buf = (int16_t*)(flags_buf + max_flags_len);
	int16_t* ycoords_buf = (int16_t*)(((uint8_t*)xcoords_buf) + max_coords_len);


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
	
	uint16_t num_glyphs = g_maxp.numGlyphs;
	


	for (uint16_t i = 0; i < num_glyphs; i++) {
		printf("glyph #%hu\n", i);
	
		int16_t num_contours;
		uint16_t intr_len;
		

		int16_t xmin, ymin, xmax, ymax;

		num_contours = read_int16(rdr);
		xmin = read_int16(rdr);
		ymin = read_int16(rdr);
		xmax = read_int16(rdr);
		ymax = read_int16(rdr);

		if (num_contours < 0) { puts("glyph was a contour, font not supported\n"); return -1; }
		if (num_contours > max_contours_num) { puts("contour num was greater than reported max"); return -1; }


		read_memcpy(rdr, endpts_buf, num_contours * sizeof(uint16_t));
		for(int16_t i = 0; i < num_contours; i++) {
			endpts_buf[i] = be16toh(endpts_buf[i]);
			printf("	%hi\n", endpts_buf[i]);
		}

		uint16_t num_coords = endpts_buf[num_contours-1]+1;

		intr_len = read_uint16(rdr);
		if (intr_len > max_intr_len) { puts("intr len was greater than reported max"); return -1; }
		read_memcpy(rdr, intr_buf, intr_len);

		uint8_t flag_repeat = 0;
		for (uint8_t i = 0; i < num_coords; i++) {
			if (flag_repeat == 0) {
				flags_buf[i] = read_byte(rdr);
				if ( flags_buf[i] & 8 ) flag_repeat = read_byte(rdr);
			} else {
				if (i == 0) { puts("tried to repeat first flag"); return -1;}
				flags_buf[i] = flags_buf[i-1];
				flag_repeat--;
			}
		}

		printf("contours: %hu\n", num_contours);
		printf("coords: %hu\n", num_coords);

		{
			for(uint16_t i = 0; i < num_coords; i++) {
				uint8_t flags = flags_buf[i];
				int16_t x;
				if (flags & 2) { // x short?
					uint8_t x_short = read_byte(rdr);
					if (flags & 16) x = ((int16_t)x_short); // x same
					else x = -((int16_t)x_short);
				} else {
					if (flags & 16) x = 0; // x same
					else x = read_int16(rdr);
				}
				xcoords_buf[i] = x;
			}
		}

		{

		for(uint16_t i = 0; i < num_coords; i++) {
			uint8_t flags = flags_buf[i];
			int16_t y;
			if (flags & 4) { // y
				uint8_t y_short = read_byte(rdr);
				if (flags & 32) y = ((uint16_t)y_short);
				else y = - ((uint16_t)y_short);
			} else {
				if (flags & 32) y = 0;
				else y = read_int16(rdr);
				
			}
			ycoords_buf[i] = y;
		}
		}

		puts("points: ");

		for(int16_t i = 0; i < num_coords; i++) {
			printf("	%5hi %5hi\n", xcoords_buf[i], ycoords_buf[i]);
		}

	}

	return -1;
}