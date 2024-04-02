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
	ANR_SC_SIMPLE,
	ANR_SC_RAW,
} anr_sc_type;

ANRSCDEF void* anr_sc_deflate(void* data, uint32_t length, uint32_t* out_length);

#endif // INCLUDE_ANR_SC_H

#ifdef ANR_SC_IMPLEMENTATION

void* anr_sc_deflate(void* data, uint32_t length, uint32_t* out_length)
{
	uint32_t max_size = length*2 + 1;
	uint8_t* final_stream = malloc(max_size);
	uint32_t write_cursor = 0;

	final_stream[0] = ANR_SC_SIMPLE;
	write_cursor++;
	
	uint8_t* src = (uint8_t*)data;
	uint8_t cmp = 0;
	uint8_t count = 0;
	for (int i = 0; i < length; i++)
	{
		uint8_t val = src[i];
		if (val == cmp) count++;
		else {
			if (count > 0) {
				final_stream[write_cursor++] = count;
				final_stream[write_cursor++] = cmp;
			}
			cmp = val;
			count = 1;
		}
	}
	final_stream[write_cursor++] = count;
	final_stream[write_cursor++] = cmp;

	if (write_cursor > length) {
		memcpy(final_stream+1, data, length);
		write_cursor = length;
		final_stream[0] = ANR_SC_RAW;

	}

	*out_length = write_cursor;

	#ifdef ANR_SC_DEBUG
	printf("Compress: %d -> (%.1f%%)\n", length, length/(float)write_cursor*100.0f);
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