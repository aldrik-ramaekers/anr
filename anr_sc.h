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

ANRSCDEF uint8_t* anr_sc_deflate(uint8_t* data, uint32_t length, uint32_t* out_length);
ANRSCDEF uint8_t* anr_sc_inflate(uint8_t* data, uint32_t length, uint32_t* out_length);

#endif // INCLUDE_ANR_SC_H

#ifdef ANR_SC_IMPLEMENTATION

typedef struct
{
	uint8_t val;
	uint32_t count;
} anr_sc_val;

#define MAX_ENCODED_CHARS 16
#define TABLE_SIZE MAX_ENCODED_CHARS

int cmpfunc (const void * a, const void * b) {
   return ( (*(anr_sc_val*)a).count - (*(anr_sc_val*)b).count );
}

uint8_t* anr_sc_inflate(uint8_t* data, uint32_t length, uint32_t* out_length)
{
	anr_sc_val table[TABLE_SIZE];
	for (int i = 0; i < TABLE_SIZE; i++)
	{
		table[i].val = data[i];
	}

	uint8_t* inflated = malloc(length*8); // ballpark
	uint32_t inflated_cursor = 0;

	uint8_t current_block_table = 0;
	uint8_t block_table_index = 8;
	uint32_t read_cursor_offset = 0;
	for (int i = TABLE_SIZE; i < length;)
	{
		if (block_table_index == 8) {
			if (read_cursor_offset == 4) {
				read_cursor_offset = 0;
				i++;
			}

			current_block_table = data[i++];
			block_table_index = 0;
			continue;
		}

		uint8_t is_halve = (current_block_table >> block_table_index) & 0x1;
		if (read_cursor_offset == 0)
		{
			if (is_halve)
			{
				uint8_t table_index = (data[i] >> 4);
				inflated[inflated_cursor++] = table[table_index].val;
				read_cursor_offset = 4;
			}
			else
			{
				inflated[inflated_cursor++] = data[i];
				read_cursor_offset = 0;
				i++;
			}
		}
		else if (read_cursor_offset == 4)
		{
			if (is_halve)
			{
				uint8_t table_index = (data[i] & 0xF);
				inflated[inflated_cursor++] = table[table_index].val;

				read_cursor_offset = 0;
				i++;
			}
			else
			{
				uint8_t top = data[i] & 0xF;
				uint8_t bottom = data[i+1] & 0xF0;
				uint8_t val = (top << 4) | (bottom >> 4);
				inflated[inflated_cursor++] = val;

				read_cursor_offset = 4;
				i++;
			}
		}


		block_table_index++;
	}

	*out_length = inflated_cursor;
	return inflated;
}

uint8_t* anr_sc_deflate(uint8_t* data, uint32_t length, uint32_t* out_length)
{
	anr_sc_val values[256];
	for (uint32_t i = 0; i < 256; i++)
	{
		values[i].val = i;
		values[i].count = 0;
	}

	for (uint32_t i = 0; i < length; i++)
	{
		values[data[i]].count++;
	}
	qsort(values, 256, sizeof(anr_sc_val), cmpfunc);

	anr_sc_val table[TABLE_SIZE];
	for (int i = 0; i < 256; i++)
	{
		if (values[i].count == 0) continue;
		if (i < 256-MAX_ENCODED_CHARS) continue;
		table[256-i-1] = values[i];
	}
	
	// Find index of last indexable byte.
	uint32_t index_of_last_byte = 0;
	for (uint32_t i = 0; i < length; i++)
	{
		for (uint8_t x = 0; x < MAX_ENCODED_CHARS; x++) {
			if (data[i] == table[x].val) {
				index_of_last_byte = i;
			}
		}
	}
	
	// Write encoding table.
	uint32_t max_size = length + MAX_ENCODED_CHARS + (length/8);
	uint8_t* deflated = malloc(max_size);
	memset(deflated, 0, max_size);
	uint32_t deflated_cursor = 0;
	uint32_t deflate_cursor_offset = 0;
	for (int i = 0; i < MAX_ENCODED_CHARS; i++)
	{
		deflated[deflated_cursor++] = table[i].val;
	}

	uint8_t current_block_table = 0;
	uint32_t bits_in_block = 8;
	uint32_t current_block_index = 0;
	for (int i = 0; i < length; i++)
	{
		if (bits_in_block == 8) {
			bits_in_block = 0;
			current_block_table = 0;
			current_block_index = deflated_cursor;
			deflated_cursor++; // Reserved for block table.
		}

		uint8_t is_halve = 0;
		if (i == index_of_last_byte) {
			if (deflate_cursor_offset == 0) {
				goto skip_last_halfbyte;
			}
		}
			
		for (uint8_t x = 0; x < MAX_ENCODED_CHARS; x++) {
			if (data[i] == table[x].val) {
				is_halve = 1;

				if (deflate_cursor_offset == 0)
				{
					deflated[deflated_cursor] = x << 4;
					deflate_cursor_offset = 4;
				}
				else if (deflate_cursor_offset == 4)
				{
					deflated[deflated_cursor++] |= x;
					deflate_cursor_offset = 0;
				}
				break;
			}
		}

		
		skip_last_halfbyte:
		if (!is_halve)
		{
			if (deflate_cursor_offset == 0)
			{
				deflated[deflated_cursor++] = data[i];
			}
			else if (deflate_cursor_offset == 4)
			{
				uint8_t top = data[i] >> 4;
				uint8_t bottom = data[i] << 4;
				deflated[deflated_cursor++] |= top;
				deflated[deflated_cursor] |= bottom;
			}
		}

		current_block_table |= (is_halve << bits_in_block);
		bits_in_block++;
		if (bits_in_block == 8 || i+1>= length) {
			deflated[current_block_index] = current_block_table;

			if (deflate_cursor_offset == 4) {
				deflate_cursor_offset = 0;
				deflated_cursor++;
			}
		}
	}

	printf(" Deflated (%.1f%%)\n", (float)deflated_cursor/length*100.0f);

	*out_length = deflated_cursor;
	return deflated;
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