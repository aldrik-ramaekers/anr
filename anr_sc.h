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
	ANR_SC_BLOCK_PATTERN = 2,
	ANR_SC_BLOCK_REFERENCE = 3,
} anr_sc_block_type;

/*

file flag:
byte 0: 0 = compressed, 1 = raw

file blocks:
raw block: 2 - 63 bytes.
compressed block: 2 bytes.
2 pair pattern block: 5 bytes.
reference block: 2 - n bytes.

after byte 1:
bit 0-1 = 0 = raw, 1 = compressed 2 = pattern, 3 = reference
bit 1-7 = count (max 63)

if raw: copy count number of bytes.
if compressed: repeat second byte count number of times.
if patterned: byte 1,3 = pattern count, byte 2,4 = pattern data, count = how many times to repeat cycle.

DEFLATE STEP 1: compress all bytes.
DEFLATE STEP 2: find pattern blocks.
DEFLATE STEP 3: make raw blocks.
DEFLATE STEP 4: find references.
*/

ANRSCDEF void* anr_sc_deflate(void* data, uint32_t length, uint32_t* out_length);

#endif // INCLUDE_ANR_SC_H

#ifdef ANR_SC_IMPLEMENTATION


static uint32_t anr_sc_remove_0(uint8_t* buffer, uint32_t cursor, uint8_t **buf)
{
	#define COPY_N_BYTES(_num) for (int t = 0; t < _num; t++) write_buffer[write_cursor++] = buffer[i+t];

	uint8_t* write_buffer = malloc(cursor);
	uint32_t write_cursor = 1;
	for (uint32_t i = 1; i < cursor;i+=0)
	{
		uint8_t count = buffer[i];
		uint8_t val = buffer[i+1];
		uint8_t type = count>>6;
		
		if (type == ANR_SC_BLOCK_PATTERN) {
			COPY_N_BYTES(5);
			i += 5;
		}
		else if (type == ANR_SC_BLOCK_RAW) {
			if (count == 0) { // Empty block.
				i++;
				continue;
			}
			count &= 0x3f;
			COPY_N_BYTES(count+1);
			i += count+1;
		}
		else if (type == ANR_SC_BLOCK_REFERENCE) {
			count &= 0x3f;
			COPY_N_BYTES(count+1);
			i += count+1;
		}
		else if (type == ANR_SC_BLOCK_COMPRESSED) {
			COPY_N_BYTES(2);
			i += 2;
		}
	}

	*buf = write_buffer;

	return write_cursor;
}

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
		//printf("\n%d %d", count&0x3F, val);
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

				if (pair_count < 63) // max number per block.
				{
					if (x+4 < cursor)
						continue;
				}
			}

			if (pair_count >= 3) { // Less than 3 pairs is better off raw.
				// Insert pair into current block.
				uint32_t saved_space = pair_count*4;
				buffer[i] = pair_count;
				buffer[i] |= (ANR_SC_BLOCK_PATTERN<<6);
				buffer[i+1] = pair1_count;
				buffer[i+2] = pair1_val;
				buffer[i+3] = pair2_count;
				buffer[i+4] = pair2_val;

				uint8_t* write_start = buffer+i+5;
				uint32_t read_offset = i+saved_space;
				memmove(write_start, buffer + read_offset, cursor - read_offset); // TODO dont do this..

				cursor -= saved_space;
				cursor += 5;

				//printf("\nFound pair! count: %d, saved: %d [%d %d %d %d]\n", pair_count, saved_space, pair1_count, pair1_val, pair2_count, pair2_val);

				i += 3;
			}
			break;
		}
	}

	return cursor;
}

static uint32_t anr_sc_deflate_step_3(uint8_t* buffer, uint32_t cursor, uint8_t** result_buffer)
{
	int singles_count = 0;
	uint32_t index_start = 0;
	for (uint32_t i = 1; i < cursor; i+=2)
	{
		uint8_t count = buffer[i];
		uint8_t type = count>>6;
		
		if (type == ANR_SC_BLOCK_PATTERN) {
			i += 3;
			goto append_raw;
		}

		ANRSC_ASSERT(type == ANR_SC_BLOCK_COMPRESSED);
		count &= 0x3F;


		if (count == 1) 
		{
			if (singles_count == 0) index_start = i;
			singles_count++;

			if (singles_count < 63 && i+2 < cursor) // max number per block.
				continue;
		}

append_raw:
		if (singles_count >= 2) // Need atleast 2 blocks for this to pay off.
		{
			for (int x = 0; x < singles_count; x++) {
				buffer[index_start+x+1] = buffer[index_start+(x*2+1)];
			}

			buffer[index_start] = singles_count;
			buffer[index_start] |= (ANR_SC_BLOCK_RAW<<6);

			uint32_t space_written = (singles_count+1);
			uint32_t space_saved = (singles_count*2)-space_written;
			uint32_t copy_from = index_start+space_written+space_saved;
			
			memset(buffer+index_start+space_written, 0, space_saved);
			i = copy_from-2;

			singles_count = 0;

		}

		singles_count = 0;
	}

	cursor = anr_sc_remove_0(buffer, cursor, result_buffer);
	free(buffer);
	return cursor;
}

static int32_t anr_sc_find_raw_match(uint8_t* buffer, uint32_t cursor, uint32_t cmp_index, uint32_t cmp_length, uint32_t upto)
{
	for (uint32_t i = 1; i < upto;i+=0)
	{
		uint8_t count = buffer[i];
		uint8_t type = count>>6;
		
		if (type == ANR_SC_BLOCK_PATTERN) {
			i += 5;
		}
		else if (type == ANR_SC_BLOCK_RAW) {
			count &= 0x3f;

			if (count > 3 && count == cmp_length && memcmp(buffer+i, buffer+cmp_index, count+1) == 0) {
				return i;
			}

			i += count+1;
		}
		else if (type == ANR_SC_BLOCK_COMPRESSED) {
			i += 2;
		}
		else if (type == ANR_SC_BLOCK_REFERENCE) {
			count &= 0x3f;
			i += count+1;
		}
	}

	return -1;
}

int is_big_endian(void)
{
    union {
        uint32_t i;
        char c[4];
    } e = { 0x01000000 };

    return e.c[0];
}

static uint32_t anr_sc_deflate_step_4(uint8_t* buffer, uint32_t cursor, uint8_t** result_buffer)
{
	for (uint32_t i = 1; i < cursor;i+=0)
	{
		uint8_t count = buffer[i];
		uint8_t val = buffer[i+1];
		uint8_t type = count>>6;
		
		if (type == ANR_SC_BLOCK_PATTERN) {
			i += 5;
		}
		else if (type == ANR_SC_BLOCK_RAW) {
			count &= 0x3f;

			uint32_t old_count = count + 1; // Length if this block.

			int32_t match = anr_sc_find_raw_match(buffer, cursor, i, count, i-1);
			uint8_t length_of_match = 0;
			if (match > 0)
			{
				if (val < 0x10000) {
					if (val < 0x100) length_of_match = 1;
					else length_of_match = 2;
				} else {
					if (val < 0x100000000L) length_of_match = 3;
					else length_of_match = 4;
				}

				uint32_t new_count = length_of_match+1;

				// Turn raw block into reference to matching block.
				buffer[i] = length_of_match;
				buffer[i] |= (ANR_SC_BLOCK_REFERENCE<<6);

				//printf ("big endian: %d", is_big_endian());
				memcpy(buffer+i+1, &match, length_of_match);
				memset(buffer+i+new_count, 0, old_count-new_count);
				i += old_count;
			}
			else
			{
				i += count+1;
			}
		}
		else if (type == ANR_SC_BLOCK_COMPRESSED) {
			i += 2;
		}
		else
		{
			ANRSC_ASSERT(0);
		}
	}

	cursor = anr_sc_remove_0(buffer, cursor, result_buffer);
	free(buffer);
	return cursor;
}

static void anr_sc_print(uint8_t* buffer, uint32_t cursor)
{
	for (uint32_t i = 1; i < cursor;i+=0)
	{
		uint8_t count = buffer[i];
		uint8_t val = buffer[i+1];
		uint8_t type = count>>6;
		
		if (type == ANR_SC_BLOCK_PATTERN) {
			printf("\npattern %d x [%d %d %d %d]", count&0x3f, buffer[i+1], buffer[i+2] - '0', buffer[i+3], buffer[i+4] - '0');
			i += 5;
		}
		else if (type == ANR_SC_BLOCK_RAW) {
			count &= 0x3f;
			printf("\nraw: %d > ", count);
			for (int x = 1; x <= count; x++) printf("%d", buffer[i+x] - '0');
			i += count+1;
		}
		else if (type == ANR_SC_BLOCK_REFERENCE) {
			count &= 0x3f;

			uint32_t value;
			memcpy(&value, buffer+i+1, count);

			printf("\nreference: length > %d, index > %d > ", count, value);

			uint8_t raw_count = buffer[value];
			for (int x = 1; x <= raw_count; x++) printf("%d", buffer[value+x] - '0');
			i += count+1;
		}
		else if (type == ANR_SC_BLOCK_COMPRESSED) {
			printf("\ncompressed: %d %d", count&0x3f, val - '0');
			i += 2;
		}
	}
}

void* anr_sc_deflate(void* data, uint32_t length, uint32_t* out_length)
{
	uint32_t max_size = length*3 + 1;
	uint8_t* final_stream = malloc(max_size);
	uint32_t write_cursor = 0;

	final_stream[0] = ANR_SC_FLAG_SIMPLE;
	write_cursor++;

	write_cursor = anr_sc_deflate_step_1(data, length, final_stream, write_cursor);
	printf(" Step 1: %d -> (%.1f%%)", length, (float)write_cursor/length*100.0f);
	write_cursor = anr_sc_deflate_step_2(data, length, final_stream, write_cursor);
	printf(" Step 2: -> (%.1f%%)", (float)write_cursor/length*100.0f);
	write_cursor = anr_sc_deflate_step_3(final_stream, write_cursor, &final_stream);
	printf(" Step 3: -> (%.1f%%)", (float)write_cursor/length*100.0f);
	write_cursor = anr_sc_deflate_step_4(final_stream, write_cursor, &final_stream);
	printf(" Step 4: -> %d (%.1f%%)\n", write_cursor, (float)write_cursor/length*100.0f);

	//anr_sc_print(final_stream, write_cursor);

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