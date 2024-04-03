/*
anr_sc.h - v0.1 - worst compression library you have ever seen.

Do this:
	#ifdef ANR_SC_IMPLEMENTATION
before you include this file in *one* C file to create the implementation.

*/

#ifndef INCLUDE_ANR_SC_H
#define INCLUDE_ANR_SC_H

#ifndef ANRSCDEF
#ifdef ANR_SC_STATIC
#define ANRSCDEF static
#else
#define ANRSCDEF extern
#endif
#endif

#ifdef ANR_SC_DEBUG
#include <stdio.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#ifndef ANRSC_ASSERT
#include <assert.h>
#define ANRSC_ASSERT(x) assert(x)
#endif

typedef enum
{
	ANR_SC_FLAG_SIMPLE,
	ANR_SC_FLAG_RAW,
} anr_sc_flag;

typedef enum
{
	ANR_SC_BLOCK_RAW = 0,
	ANR_SC_BLOCK_COMPRESSED = 1,
	ANR_SC_BLOCK_REPEAT = 2,
} anr_sc_block_type;

/*

file flag:
byte 0: 0 = compressed, 1 = raw

file blocks:
raw block: 2 - 63 bytes.
compressed block: 2 bytes.
2 pair repeat block: 5 bytes.

after byte 1:
bit 0-1 = 0 = raw, 1 = compressed 2 = repeated
bit 1-7 = count (max 63)

if raw: copy count number of bytes.
if compressed: repeat second byte count number of times.
if repeated: byte 1,3 = repeat count, byte 2,4 = repeat data, count = how many times to repeat cycle.

DEFLATE STEP 1: compress all bytes.
DEFLATE STEP 2: find repeating blocks.
*/

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0') 

ANRSCDEF void* anr_sc_deflate(void* data, uint32_t length, uint32_t* out_length);

#endif // INCLUDE_ANR_SC_H

#ifdef ANR_SC_IMPLEMENTATION

static uint32_t anr_sc_deflate_step_1(void* data, uint32_t length, uint8_t* buffer, uint32_t cursor)
{	
	uint8_t* src = (uint8_t*)data;
	uint8_t cmp = 0;
	uint8_t count = 0;
	for (int i = 0; i < length; i++)
	{
		uint8_t val = src[i];
		if (val == cmp) {
			count++;

			if (count <= 63) // max number per block.
				continue;
		}

		if (count > 0) {
			if (count > 63) count = 63;
			count |= (ANR_SC_BLOCK_COMPRESSED<<6);
			buffer[cursor++] = count;
			buffer[cursor++] = cmp;
		}
		cmp = val;
		count = 1;
	}
	// Finalize last block.
	count |= (ANR_SC_BLOCK_COMPRESSED<<6);
	buffer[cursor++] = count;
	buffer[cursor++] = cmp;

	return cursor;
}

static uint32_t anr_sc_deflate_step_2(void* data, uint32_t length, uint8_t* buffer, uint32_t cursor)
{
	ANRSC_ASSERT(cursor % 2 == 1);

	for (uint32_t i = 1; i < cursor; i+=2)
	{
		uint8_t count = buffer[i];
		//uint8_t val = buffer[i+1];
		uint8_t type = count>>6;
		//printf("%d %d\n", count&0x3F, val);
		ANRSC_ASSERT(type == ANR_SC_BLOCK_COMPRESSED);
		count &= 0x3F;


		uint8_t pair1_count = 0;
		uint8_t pair1_val = 0;
		uint8_t pair2_count = 0;
		uint8_t pair2_val = 0;
		uint8_t pair_count = 0; // need atleast 3 pairs for this to pay off.
		uint8_t finding_pairs = 1;
		for (int x = i; x < cursor; x+=4)
		{
			uint8_t p1_count = buffer[x]&0x3F;
			uint8_t p1_val = buffer[x+1];
			uint8_t p2_count = buffer[x+2]&0x3F;
			uint8_t p2_val = buffer[x+3];
			if (finding_pairs)
			{
				pair1_count = p1_count;
				pair1_val = p1_val;
				pair2_count = p2_count;
				pair2_val = p2_val;
				finding_pairs = 0;
				pair_count = 1;
				continue;
			}

			if (p1_count == pair1_count && p2_count == pair2_count && p1_val == pair1_val && p2_val == pair2_val && pair_count)
			{
				pair_count++;

				if (pair_count <= 63) // max number per block.
				{
					if (x+1 < cursor)
						continue;
				}
			}

			if (pair_count >= 3) { // Less than 3 pairs is better off raw.
				// Insert pair into current block.
				uint32_t saved_space = pair_count*4;
				buffer[i] = pair_count;
				buffer[i] |= (ANR_SC_BLOCK_REPEAT<<6);
				buffer[i+1] = pair1_count;
				buffer[i+2] = pair1_val;
				buffer[i+3] = pair2_count;
				buffer[i+4] = pair2_val;

				uint8_t* write_start = buffer+i+5;
				uint32_t read_offset = i+saved_space;
				memmove(write_start, buffer + read_offset, cursor - read_offset); // TODO dont do this..

				cursor -= saved_space;
				cursor += 5;

				//printf("Found pair! count: %d, saved: %d [%d %d %d %d]\n", pair_count, saved_space, pair1_count, pair1_val, pair2_count, pair2_val);

				i += 3;
			}
			break;
		}
		
		//printf("%d %d\n", buffer[i-12], buffer[i-10]);

		//printf("%d %d\n", count&0x3F, val);
	}

	return cursor;
}

void* anr_sc_deflate(void* data, uint32_t length, uint32_t* out_length)
{
	uint32_t max_size = length*3 + 1;
	uint8_t* final_stream = malloc(max_size);
	uint32_t write_cursor = 0;

	final_stream[0] = ANR_SC_FLAG_SIMPLE;
	write_cursor++;

	write_cursor = anr_sc_deflate_step_1(data, length, final_stream, write_cursor);
	printf("Step 1: Compress: %d -> %d (%.1f%%)", length, write_cursor, length/(float)write_cursor*100.0f);
	write_cursor = anr_sc_deflate_step_2(data, length, final_stream, write_cursor);
	printf(" Step 2: -> %d (%.1f%%)\n", write_cursor, length/(float)write_cursor*100.0f);

	//printf("%d %d "BYTE_TO_BINARY_PATTERN"\n", count, cmp, BYTE_TO_BINARY(count));
	//if (write_cursor > length) {
	//	memcpy(final_stream+1, data, length);
	//	write_cursor = length;
	//	final_stream[0] = ANR_SC_RAW;
	//
	//}

	*out_length = write_cursor;

	#ifdef ANR_SC_DEBUG

	#endif

	return final_stream;
}


#endif // ANR_SC_IMPLEMENTATION

/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2024 Aldrik Ramaekers
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/