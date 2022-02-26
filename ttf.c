#include <stdint.h>
#include <stdio.h>
#include <endian.h>
#include <stdlib.h>


/*
sequence of concatenated tables

*/

typedef struct {
    uint32_t tableTag;
    uint32_t checksum;
    uint32_t offset;
    uint32_t length;
} __attribute__((packed)) TableDirRecord;


typedef struct {
    uint32_t sfntVersion;
    uint16_t numTables;
    uint16_t searchRange;
    uint16_t entrySelector;
    uint16_t rangeShift;
} __attribute__((packed)) TableDir;

typedef struct {
    uint8_t* start, end;
    uint8_t* read;
} TableView;

#define swap16(bytes) (((bytes) >> 8) | ((bytes) << 8))
#define swap32(bytes) be32toh(bytes);


uint16_t read16(uint8_t* ptr, uint8_t* end);
uint32_t read32(uint8_t* ptr, uint8_t* end);
void read_section(uint8_t* ptr, uint8_t* end) {

}

int read_ttf(const char* path); 

#define read_uint16(var) 
#define read_uint32(var)

TableDirRecord* find_table_record(TableDirRecord* records, uint16_t count, const char* tag_str);

typedef struct {
    uint16_t version;
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


int read_ttf(const char* path) {



    FILE* fp = fopen(path, "r");

    if (fp == NULL) { puts("unable to open file"); return 1;}

    TableDir td;
    unsigned long num_read;

    num_read = fread(&td, sizeof(TableDir), 1, fp);
    if (num_read != 1) { puts("failed to read header"); return 1;}

    printf("sfnt: %x\n", be32toh(td.sfntVersion));
    printf("num tables: %hu\n", be16toh(td.numTables));
    printf("search range: %hu\n", be16toh(td.searchRange));
    printf("entry selector: %hu\n", be16toh(td.entrySelector));
    printf("range shift: %hu\n", be16toh(td.rangeShift));

    uint16_t num_records = be16toh(td.numTables);
    TableDirRecord* records = (TableDirRecord*)malloc(num_records * sizeof(TableDirRecord));

    num_read = fread(records, sizeof(TableDirRecord), num_records, fp);
    if (num_read != num_records) { puts("failed to read records"); return 1; }

    
    for(uint16_t i = 0; i < num_records; i++) {
        TableDirRecord r = records[i];

        uint32_t tag = be32toh(r.tableTag);
        printf("tag: %.4s\n", (char*)&r.tableTag);
        printf(" checksum: %u\n", be32toh(r.checksum));
        printf(" offset: %u\n", be32toh(r.offset));
        printf(" length: %u\n", be32toh(r.length));
    }
    
   
    /*
    TableDirRecord* maxp = find_table_record(records, num_records, "maxp");
    if (maxp == NULL) { puts("couldnt find maxp"); return 1; }

    
        printf("tag: %.4s\n", (char*)& maxp->tableTag);
        printf(" checksum: %u\n", be32toh( maxp->checksum));
        printf(" offset: %u\n", be32toh( maxp->offset));
        printf(" length: %u\n", be32toh( maxp->length));
        
    */
    /*


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


TableDirRecord* find_table_record(TableDirRecord* records, uint16_t count, const char* tag_str) {
    uint32_t tag = *(uint32_t*)tag_str; // this is a bad idea
    
    for(TableDirRecord* end = records + count; records < end; records++)
        if (records->tableTag == tag) return records;
    

    return NULL;
}