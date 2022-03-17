#include <stdint.h>
#include <stdlib.h>
#include <sys/random.h>
#include <time.h>
#include <stdio.h>

enum SimpleGlyfFlags {
	SGLYFFLAGS_ONCURVE = 1,
	SGLYFFLAGS_SHORTX = 2,
	SGLYFFLAGS_SHORTY = 4,
	SGLYFFLAGS_REPEAT = 8,
	SGLYFFLAGS_SAMEX = 16,
	SGLYFFLAGS_SAMEY = 32,
};

const uint32_t NUM_COORDS = 10000;

typedef struct timespec timespec;

timespec diff(timespec start, timespec end);
timespec add(timespec start, timespec end);

void branched_read(int16_t* coords_buf, uint8_t* flags_buf, uint8_t* fptr) {
	for(uint32_t i = 0; i < NUM_COORDS; i++) {
		uint8_t flags = flags_buf[i];
		int16_t x;

		if (flags & SGLYFFLAGS_SHORTX) { // y
			uint8_t x_short = *(fptr++);
			if (flags & SGLYFFLAGS_SAMEX) x = ((uint16_t)x_short);
			else x = - ((uint16_t)x_short);
		} else {
			if (flags & SGLYFFLAGS_SAMEX) x = 0;
			else { x = (fptr[0] << 8) | fptr[1]; fptr += 2; }
			
		}
		coords_buf[i] = x;
	}
}

void branchless_read(int16_t* coords_buf, uint8_t* flags_buf, uint8_t* fptr) {
	uint32_t j = 0;
	for(uint32_t i = 0; i < NUM_COORDS; i++) {
		uint8_t flags = flags_buf[i];

		uint16_t sh = ~(((flags & SGLYFFLAGS_SHORTX) >> 1)-1);
		uint16_t n_sm = ((flags & SGLYFFLAGS_SAMEX) >> 4)-1;

		uint16_t x_0 = fptr[j] & ~sh & n_sm;
		uint16_t x_1 = fptr[j+1] & ~sh & n_sm;
		x_1 += (fptr[j] & sh ^ n_sm) + (1 & n_sm);

		j += 2;
		j -= sh & 1;
		j -= (~n_sm & ~sh) & 2;

		coords_buf[i] = x_0 << 8 | x_1;
	}
}

timespec test_branched(int16_t* coords_buf, uint8_t* flags_buf, uint8_t* fptr) {
	
	//getrandom(coord_buf, NUM_COORDS * sizeof(int16_t), 0);
	getrandom(flags_buf, NUM_COORDS * sizeof(uint8_t), 0);
	getrandom(fptr, NUM_COORDS * 2 * sizeof(uint8_t), 0);
	
	timespec time1, time2;
    
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
	branched_read(coords_buf, flags_buf, fptr);

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);

	for(uint32_t i = 0; i < NUM_COORDS; i+=1000) {
		//printf("%hi ", coords_buf[i]);
	}

	return diff(time1, time2);
}


timespec test_branchless(int16_t* coords_buf, uint8_t* flags_buf, uint8_t* fptr) {
	
	//getrandom(coord_buf, NUM_COORDS * sizeof(int16_t), 0);
	getrandom(flags_buf, NUM_COORDS * sizeof(uint8_t), 0);
	getrandom(fptr, NUM_COORDS * 2 * sizeof(uint8_t), 0);
	
	timespec time1, time2;
    
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
	branchless_read(coords_buf, flags_buf, fptr);

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);

	for(uint32_t i = 0; i < NUM_COORDS; i+=1000) {
		//printf("%hi ", coords_buf[i]);
	}

	return diff(time1, time2);
}

#define REPEAT_COUNT 100000

int main() {
	
	int16_t* coords_buf = (int16_t*)malloc(NUM_COORDS * sizeof(int16_t));
	uint8_t* flags_buf = (uint8_t*)malloc(NUM_COORDS * sizeof(uint8_t));
	uint8_t* fptr = (uint8_t*)malloc(NUM_COORDS * 2 * sizeof(uint8_t));


	timespec branched_times[REPEAT_COUNT];
	timespec branchless_times[REPEAT_COUNT];
	

	for(uint32_t i = 0; i < REPEAT_COUNT; i++) {
		branchless_times[i] = test_branchless(coords_buf, flags_buf, fptr);
	}

	for(uint32_t i = 0; i < REPEAT_COUNT; i++) {
		branched_times[i] = test_branched(coords_buf, flags_buf, fptr);
	}


	
	puts("\n\n");
	{
		timespec total = (timespec){0, 0};
		for(uint32_t i = 0; i < REPEAT_COUNT; i++) {
			total = add(total, branched_times[i]);
		}

		double t = (double)total.tv_sec + ((double)total.tv_nsec/1000000000.0);
		printf("branched: %f\n", t);
	}	
	{
		timespec total = (timespec){0, 0};
		for(uint32_t i = 0; i < REPEAT_COUNT; i++) {
			total = add(total, branchless_times[i]);
		}


		double t = (double)total.tv_sec + ((double)total.tv_nsec/1000000000.0);
		printf("branchless: %f\n", t);
	}
}

timespec diff(timespec start, timespec end)
{
    timespec temp;
    if ((end.tv_nsec-start.tv_nsec)<0) {
        temp.tv_sec = end.tv_sec-start.tv_sec-1;
        temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return temp;
}

timespec add(timespec start, timespec end)
{
    timespec temp;

	temp.tv_sec = start.tv_sec+end.tv_sec;
    temp.tv_nsec = start.tv_nsec+end.tv_nsec;
	if ( temp.tv_nsec >= 1000000000 ) {
		temp.tv_sec++;
        temp.tv_nsec -= 1000000000;
	}

    return temp;
}