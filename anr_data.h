/*
anr_data.h - v0.1 - public domain data structures library

This is a single-header-file library that provides basic data structures

Do this:
	#ifdef ANR_DATA_IMPLEMENTATION
before you include this file in *one* C or C++ file to create the implementation.

LICENSE
	See end of file for license information.
	
*/
#ifndef INCLUDE_ANR_DATA_H
#define INCLUDE_ANR_DATA_H

#include <stdlib.h>
#include <stdio.h>

#ifndef ANRDATADEF
#ifdef ANR_DATA_STATIC
#define ANRDATADEF static
#else
#define ANRDATADEF extern
#endif
#endif

#include <inttypes.h>

#ifndef ANRDATA_ASSERT
#include <assert.h>
#define ANRDATA_ASSERT(x) assert(x)
#endif

typedef struct
{
	void* prev;
	void* next;
	void* data;
} anr_linked_list_node;

typedef struct
{
	anr_linked_list_node* first;
	anr_linked_list_node* last;
	uint32_t length;
} anr_linked_list;

/*
typedef struct
{
	void (*find_at)(void*,uint32_t);
} anr_ds;*/

#endif // INCLUDE_ANR_DATA_H

// === linked list ===
ANRDATADEF anr_linked_list 			anr_linked_list_create();
ANRDATADEF anr_linked_list_node* 	anr_linked_list_add(anr_linked_list* list, void* ptr);
ANRDATADEF void 					anr_linked_list_free(anr_linked_list* list);
ANRDATADEF void 					anr_linked_list_print(anr_linked_list* list);
ANRDATADEF void* 					anr_linked_list_find_at(anr_linked_list* list, uint32_t index);
ANRDATADEF uint32_t 				anr_linked_list_find_by(anr_linked_list* list, char* ptr);
ANRDATADEF uint8_t 					anr_linked_list_remove_by(anr_linked_list* list, void* ptr);
ANRDATADEF uint8_t 					anr_linked_list_remove_at(anr_linked_list* list, uint32_t index);

//anr_ds _ds_ll = { anr_linked_list_find_at };



#ifdef ANR_DATA_IMPLEMENTATION

void anr_linked_list_print(anr_linked_list* list)
{
	anr_linked_list_node* iter = list->first;
	uint32_t count = 0;
	printf("List %p has %d nodes, first: %p last: %p\n", list, list->length, list->first, list->last);
	while (iter)
	{
		printf("#%d %p prev: %p next: %p\n", count, iter, iter->prev, iter->next);
		iter = iter->next;
		count++;
	} 
}

void anr_linked_list_free(anr_linked_list* list)
{
	anr_linked_list_node* iter = list->last;
	anr_linked_list_node* last = iter;
	while (iter)
	{
		last = iter;
		free(iter->next);
		free(iter->data);
		iter = iter->prev;
	}
	free(last);
}

uint8_t anr_linked_list_remove_at(anr_linked_list* list, uint32_t index)
{
	anr_linked_list_node* iter = list->first;
	uint32_t count = 0;
	while (iter)
	{
		if (count == index) {
			if (iter == list->first) {
				list->first = iter->next;
			}
			if (iter == list->last) {
				list->last = iter->prev;
			}

			if (iter->prev) {
				((anr_linked_list_node*)(iter->prev))->next = iter->next;
			}
			if (iter->next) {
				((anr_linked_list_node*)(iter->next))->prev = iter->prev;
			}

			free(iter->data);
			free(iter);
			list->length--;
			return 1;
		}
		iter = iter->next;
		count++;
	}
	return 0;
}

uint8_t anr_linked_list_remove_by(anr_linked_list* list, void* ptr)
{
	anr_linked_list_node* iter = list->first;
	while (iter)
	{
		if (iter->data == ptr) {
			if (iter == list->first) {
				list->first = iter->next;
			}
			if (iter == list->last) {
				list->last = iter->prev;
			}

			if (iter->prev) {
				((anr_linked_list_node*)(iter->prev))->next = iter->next;
			}
			if (iter->next) {
				((anr_linked_list_node*)(iter->next))->prev = iter->prev;
			}

			free(iter->data);
			free(iter);
			list->length--;
			return 1;
		}
		iter = iter->next;
	}
	return 0;
}

uint32_t anr_linked_list_find_by(anr_linked_list* list, char* ptr)
{
	anr_linked_list_node* iter = list->first;
	uint32_t count = 0;
	while (iter)
	{
		if (iter->data == ptr) return count;
		count++;
		iter = iter->next;
	}
	return -1;
}

void* anr_linked_list_find_at(anr_linked_list* list, uint32_t index)
{
	anr_linked_list_node* iter = list->first;
	uint32_t count = 0;
	while (iter)
	{
		if (count == index) return iter->data;
		count++;
		iter = iter->next;
	}
	return 0;
}

anr_linked_list anr_linked_list_create()
{
	return (anr_linked_list){0};
}

anr_linked_list_node* anr_linked_list_add(anr_linked_list* list, void* ptr)
{
	anr_linked_list_node* iter = list->first;
	do
	{
		if (iter == NULL || iter->next == NULL) {
			anr_linked_list_node* node = malloc(sizeof(anr_linked_list_node));
			ANRDATA_ASSERT(node);
			node->data = ptr;
			node->prev = iter;
			node->next = NULL;
			iter == NULL ? (list->first = node) : (iter->next = node);
			list->last = node;
			list->length++;
			return node;
		}

		iter = iter->next;
	} while (iter);
	ANRDATA_ASSERT(0);
	return 0;
}

#endif // ANR_DATA_IMPLEMENTATION

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