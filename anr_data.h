/*
anr_data.h - v0.1 - public domain data structures library

This is a single-header-file library that provides basic data structures

Do this:
	#ifdef ANR_DATA_IMPLEMENTATION
before you include this file in *one* C or C++ file to create the implementation.

The datastructures are almost completely interchangeable if you use the macros. See examples/test_data.c
important differences:
	- linked list finds data by comparing ptr's, array finds data by comparing memory.

LICENSE
	See end of file for license information.
	
*/
#ifndef INCLUDE_ANR_DATA_H
#define INCLUDE_ANR_DATA_H

#include <string.h>
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

typedef enum
{
	ANR_DS_LINKEDLIST = 0,
	ANR_DS_DYNAMIC_ARRAY = 1,
} anr_ds_type;

typedef struct
{
	void* prev;
	void* next;
	void* data;
} anr_linked_list_node;

typedef struct
{
	anr_ds_type ds_type;
	anr_linked_list_node* first;
	anr_linked_list_node* last;
	uint32_t length;
} anr_linked_list;

typedef struct
{
	anr_ds_type ds_type;
	void* data;
	uint32_t item_length;
	uint32_t reserve_size;
	uint32_t reserved;
	uint32_t length;
} anr_array;

typedef struct
{
	uint32_t index;
	void* data;
	union
	{
		struct
		{
			anr_linked_list_node* node;
		} ll;
	};
} anr_iter;

typedef union
{
	anr_linked_list ds_ll;
} anr_ds;

typedef struct
{
	uint8_t 	(*add)(void* list, void* ptr);
	void 		(*free)(void* list);
	void 		(*print)(void* list);
	void* 		(*find_at)(void*,uint32_t); // returns data
	uint32_t 	(*find_by)(void* list, char* ptr); // returns index, or -1 if not found
	uint8_t 	(*remove_at)(void* list, uint32_t index); // returns 1 on success, 0 on fail
	uint8_t 	(*remove_by)(void* list, void* ptr); // returns 1 on success, 0 on fail
	uint8_t 	(*insert)(void* list, uint32_t index, void* ptr); // returns 1 on success, 0 on fail
	uint32_t 	(*length)(void* list);
	anr_iter 	(*iter_start)(void* ll);
	uint8_t 	(*iter_next)(void* ll, anr_iter* iter); // returns 1 on success, 0 if no more items to iterate
} anr_ds_table;

typedef struct
{
	int type;
	anr_ds_table* ds;
} anr_ds_pair;

// === linked list ===
ANRDATADEF anr_linked_list 	anr_linked_list_create();
ANRDATADEF uint8_t 			anr_linked_list_add(void* list, void* ptr);
ANRDATADEF void 			anr_linked_list_free(void* list);
ANRDATADEF void 			anr_linked_list_print(void* list);
ANRDATADEF void* 			anr_linked_list_find_at(void* list, uint32_t index);
ANRDATADEF uint32_t 		anr_linked_list_find_by(void* list, char* ptr);
ANRDATADEF uint8_t 			anr_linked_list_remove_at(void* list, uint32_t index);
ANRDATADEF uint8_t 			anr_linked_list_remove_by(void* list, void* ptr);
ANRDATADEF uint8_t 			anr_linked_list_insert(void* list, uint32_t index, void* ptr);
ANRDATADEF uint32_t 		anr_linked_list_length(void* list);
ANRDATADEF anr_iter 		anr_linked_list_iter_start(void* list);
ANRDATADEF uint8_t 			anr_linked_list_iter_next(void* list, anr_iter* iter);

// === dynamic array ===
ANRDATADEF anr_array 	anr_array_create(uint32_t data_size, uint32_t reserve_count);
ANRDATADEF uint8_t 		anr_array_add(void* list, void* ptr);
ANRDATADEF void 		anr_array_free(void* list);
ANRDATADEF void 		anr_array_print(void* list);
ANRDATADEF void* 		anr_array_find_at(void* list, uint32_t index);
ANRDATADEF uint32_t 	anr_array_find_by(void* list, char* ptr);
ANRDATADEF uint8_t 		anr_array_remove_at(void* list, uint32_t index);
ANRDATADEF uint8_t 		anr_array_remove_by(void* list, void* ptr);
ANRDATADEF uint8_t 		anr_array_insert(void* list, uint32_t index, void* ptr);
ANRDATADEF uint32_t 	anr_array_length(void* list);
ANRDATADEF anr_iter 	anr_array_iter_start(void* list);
ANRDATADEF uint8_t 		anr_array_iter_next(void* list, anr_iter* iter);

anr_ds_table _ds_ll = 
{
	anr_linked_list_add,
	anr_linked_list_free,
	anr_linked_list_print,
	anr_linked_list_find_at,
	anr_linked_list_find_by,
	anr_linked_list_remove_at,
	anr_linked_list_remove_by,
	anr_linked_list_insert,
	anr_linked_list_length,
	anr_linked_list_iter_start,
	anr_linked_list_iter_next,
};

anr_ds_table _ds_array = 
{
	anr_array_add,
	anr_array_free,
	anr_array_print,
	anr_array_find_at,
	anr_array_find_by,
	anr_array_remove_at,
	anr_array_remove_by,
	anr_array_insert,
	anr_array_length,
	anr_array_iter_start,
	anr_array_iter_next,
};

anr_ds_pair _ds_arr[] = 
{
	{ANR_DS_LINKEDLIST, &_ds_ll},
	{ANR_DS_DYNAMIC_ARRAY, &_ds_array},
};

#define ANR_DS_ARRAY(_data_size, _reserve_count) anr_array_create(_data_size, _reserve_count)
#define ANR_DS_LINKED_LIST anr_linked_list_create()

#define ANR_DS_ADD(__ds, __ptr) (_ds_arr[(int)(((anr_ds*)__ds)->ds_ll.ds_type)]).ds->add((void*)__ds, __ptr)
#define ANR_DS_FREE(__ds) (_ds_arr[(int)(((anr_ds*)__ds)->ds_ll.ds_type)]).ds->free((void*)__ds)
#define ANR_DS_PRINT(__ds) (_ds_arr[(int)(((anr_ds*)__ds)->ds_ll.ds_type)]).ds->print((void*)__ds)
#define ANR_DS_FIND_AT(__ds, __index) (_ds_arr[(int)(((anr_ds*)__ds)->ds_ll.ds_type)]).ds->find_at((void*)__ds, __index)
#define ANR_DS_FIND_BY(__ds, __ptr) (_ds_arr[(int)(((anr_ds*)__ds)->ds_ll.ds_type)]).ds->find_by((void*)__ds, __ptr)
#define ANR_DS_REMOVE_BY(__ds, __ptr) (_ds_arr[(int)(((anr_ds*)__ds)->ds_ll.ds_type)]).ds->remove_by((void*)__ds, __ptr)
#define ANR_DS_REMOVE_AT(__ds, __index) (_ds_arr[(int)(((anr_ds*)__ds)->ds_ll.ds_type)]).ds->remove_at((void*)__ds, __index)
#define ANR_DS_INSERT(__ds, __index, __ptr) (_ds_arr[(int)(((anr_ds*)__ds)->ds_ll.ds_type)]).ds->insert((void*)__ds, __index, __ptr)
#define ANR_DS_LENGTH(__ds) (_ds_arr[(int)(((anr_ds*)__ds)->ds_ll.ds_type)]).ds->length((void*)__ds)
#define ANR_DS_ITER_START(__ds) (_ds_arr[(int)(((anr_ds*)__ds)->ds_ll.ds_type)]).ds->iter_start((void*)__ds)
#define ANR_DS_ITER_NEXT(__ds, __iter) (_ds_arr[(int)(((anr_ds*)__ds)->ds_ll.ds_type)]).ds->iter_next((void*)__ds, __iter)

#define ANR_ITERATE(__iter, __ds) \
	anr_iter __iter = ANR_DS_ITER_START((void*)__ds); \
	while (ANR_DS_ITER_NEXT((void*)__ds, &iter))

#endif // INCLUDE_ANR_DATA_H

#ifdef ANR_DATA_IMPLEMENTATION

#ifdef ANR_DATA_DEBUG
anr_linked_list curr_print = (anr_linked_list){ANR_DS_LINKEDLIST, 0, 0, 0};
anr_linked_list prev_print = (anr_linked_list){ANR_DS_LINKEDLIST, 0, 0, 0};

static void anr__print_diff()
{
	#define max(a,b) (((a) > (b)) ? (a) : (b))
	#define gotox(x) printf("\033[%dC", (x))
	#define moveup(y) printf("\033[%dA", y);
	
	int max_lines = max(prev_print.length, curr_print.length);
	for (int i = 0; i < max_lines; i++)
	{
		char* curr_line = (i < curr_print.length) ? ANR_DS_FIND_AT(&curr_print, i) : 0;
		if (curr_line)
		{
			printf("%s", curr_line);
			if (i < prev_print.length) moveup(1);
		}
		
		char* prev_line = (i < prev_print.length) ? ANR_DS_FIND_AT(&prev_print, i) : 0;
		if (prev_line)
		{
			gotox(100);
			int strl = strlen(prev_line);
			for (int x = 0; x < strl; x++) {
				char ch = prev_line[x];
				char is_same = (curr_line == NULL) ? 1 : ((x < strlen(curr_line) ? (ch == curr_line[x]) : 0));
				if (!is_same) printf("\033[0;31m%c\033[0m", ch);
				else printf("%c", ch);
			}
		}
	}
	printf("\n");

	// Next iteration.
	ANR_DS_FREE(&prev_print);
	prev_print = curr_print;
	curr_print = (anr_linked_list){ANR_DS_LINKEDLIST, 0, 0, 0};
}


void anr_linked_list_print(void* ds)
{
	
	anr_linked_list* list = ds;
	anr_linked_list_node* iter = list->first;
	uint32_t count = 0;

	char* buffer = malloc(200);
	snprintf(buffer, 200, "List %p has %d nodes, first: %p last: %p\n", list, list->length, list->first, list->last);
	ANR_DS_ADD(&curr_print, buffer);
	while (iter)
	{
		char* buffer = malloc(200);
		snprintf(buffer, 200, "#%d %p prev: %p next: %p\n", count, iter, iter->prev, iter->next);
		ANR_DS_ADD(&curr_print, buffer);
		iter = iter->next;
		count++;
	}
	anr__print_diff();
}
#else
void anr_linked_list_print(void* ds)
{
	(void)ds;
}
#endif

uint32_t anr_linked_list_length(void* ds)
{
	anr_linked_list* list = ds;
	return list->length;
}

anr_iter anr_linked_list_iter_start(void* ds)
{
	(void)ds;
	anr_iter iter;
	iter.ll.node = NULL;
	iter.index = -1;
	iter.data = NULL;
	return iter;
}

uint8_t anr_linked_list_iter_next(void* ds, anr_iter* iter)
{
	ANRDATA_ASSERT(ds);
	ANRDATA_ASSERT(iter);
	anr_linked_list* list = ds;
	if (iter->ll.node == NULL) iter->ll.node = list->first;
	else iter->ll.node = iter->ll.node->next;
	iter->index++;
	iter->data = iter->ll.node != NULL ? iter->ll.node->data : 0;
	return iter->ll.node != NULL;
}

void anr_linked_list_free(void* ds)
{
	ANRDATA_ASSERT(ds);
	anr_linked_list* list = ds;
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

uint8_t anr_linked_list_insert(void* ds, uint32_t index, void* ptr)
{
	ANRDATA_ASSERT(ds);
	anr_linked_list* list = ds;

	anr_linked_list_node* iter = list->first;
	uint32_t count = 0;
	while (iter)
	{
		if (count == index) {
			break;
		}
		iter = iter->next;
		count++;
	}

	if (!iter) return 0; // out of bounds.

	anr_linked_list_node* node = malloc(sizeof(anr_linked_list_node));
	ANRDATA_ASSERT(node);
	node->data = ptr;
	node->prev = iter->prev;
	node->next = iter;

	if (count == 0) {
		list->first = node;
	}
	if (count == list->length-1) {
		list->last = node;
	}

	if (node->prev) {
		((anr_linked_list_node*)(node->prev))->next = node;
	}
	if (node->next) {
		((anr_linked_list_node*)(node->next))->prev = node;
	}
	list->length++;

	return 1;
}

uint8_t anr_linked_list_remove_at(void* ds, uint32_t index)
{
	ANRDATA_ASSERT(ds);
	anr_linked_list* list = ds;
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

uint8_t anr_linked_list_remove_by(void* ds, void* ptr)
{
	ANRDATA_ASSERT(ds);
	ANRDATA_ASSERT(ptr);
	anr_linked_list* list = ds;
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

uint32_t anr_linked_list_find_by(void* ds, char* ptr)
{
	ANRDATA_ASSERT(ds);
	ANRDATA_ASSERT(ptr);
	anr_linked_list* list = ds;
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

void* anr_linked_list_find_at(void* ds, uint32_t index)
{
	ANRDATA_ASSERT(ds);
	anr_linked_list* list = ds;
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
	return (anr_linked_list){ANR_DS_LINKEDLIST, 0, 0, 0};
}

uint8_t anr_linked_list_add(void* ds, void* ptr)
{
	ANRDATA_ASSERT(ds);
	ANRDATA_ASSERT(ptr);
	anr_linked_list* list = ds;
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
			return 1;
		}

		iter = iter->next;
	} while (iter);
	
	ANRDATA_ASSERT(0);
	return 0;
}


anr_array anr_array_create(uint32_t data_size, uint32_t reserve_count)
{
	ANRDATA_ASSERT(data_size > 0);
	ANRDATA_ASSERT(reserve_count > 0);

	anr_array arr = (anr_array){ANR_DS_DYNAMIC_ARRAY, .data = 0, .item_length = data_size, .length = 0, .reserve_size = reserve_count, .reserved = 0};
	arr.reserved = data_size * reserve_count;
	arr.data = malloc(arr.reserved);
	ANRDATA_ASSERT(arr.data);

	return arr;
}

uint8_t anr_array_add(void* ds, void* ptr)
{
	ANRDATA_ASSERT(ds);
	ANRDATA_ASSERT(ptr);

	anr_array* arr = (anr_array*)ds;

	if (arr->length >= arr->reserved)
	{
		arr->reserved += arr->reserve_size*arr->item_length;
		arr->data = realloc(arr->data, arr->reserved);
		ANRDATA_ASSERT(arr->data);
	}

	memcpy(arr->data + (arr->length * arr->item_length), ptr, arr->item_length);
	arr->length++;

	return 1;
}
void anr_array_free(void* ds)
{
	ANRDATA_ASSERT(ds);

	anr_array* arr = (anr_array*)ds;
	free(arr->data);
}

#ifdef ANR_DATA_DEBUG
void anr_array_print(void* ds)
{
	ANRDATA_ASSERT(ds);

	anr_array* arr = ds;
	char* buffer = malloc(200);
	snprintf(buffer, 200, "array %p has %d items, %d reserved\n", arr, arr->length, arr->reserved);
	ANR_DS_ADD(&curr_print, buffer);
	for (int i = 0; i < arr->length; i++)
	{
		char* buffer = malloc(200);
		snprintf(buffer, 200, "#%d ", i);
		char* data = anr_array_find_at(ds, i);
		for (int x = 0; x < arr->item_length; x++) {
			snprintf(buffer+strlen(buffer), 200, "%x", *(char*)(data + x));
		}
		snprintf(buffer+strlen(buffer), 200, "\n");
		ANR_DS_ADD(&curr_print, buffer);
	}
	#ifdef ANR_DATA_DEBUG
	anr__print_diff();
	#else
	ANR_DS_FREE(&curr_print);
	curr_print = (anr_linked_list){ANR_DS_LINKEDLIST, 0, 0, 0};
	#endif
}
#else
void anr_array_print(void* ds)
{
	(void)ds;
}
#endif

void* anr_array_find_at(void* ds, uint32_t index)
{
	ANRDATA_ASSERT(ds);
	anr_array* arr = ds;
	if(index >= arr->length) return 0;

	return arr->data + (index * arr->item_length);
}
uint32_t anr_array_find_by(void* ds, char* ptr)
{
	ANRDATA_ASSERT(ds);
	ANRDATA_ASSERT(ptr);
	anr_array* arr = (anr_array*)ds;

	for (int i = 0; i < arr->length; i++)
	{
		void* data = anr_array_find_at(ds, i);
		if (memcmp(data, ptr, arr->item_length) == 0) {
			return i;
		}
	}

	return -1;
}
uint8_t anr_array_remove_at(void* ds, uint32_t index)
{
	ANRDATA_ASSERT(ds);
	anr_array* arr = (anr_array*)ds;
	if (index >= arr->length) return 0;
	if (index < 0) return 0;
	uint32_t mem_to_move = (arr->length - index - 1) * arr->item_length;
	uint32_t mem_to_overwrite = index * arr->item_length;
	uint32_t mem_to_copy = (index+1) * arr->item_length;
	memmove(arr->data + mem_to_overwrite, arr->data + mem_to_copy, mem_to_move);
	arr->length--;
	return 1;
}
uint8_t anr_array_remove_by(void* ds, void* ptr)
{
	uint32_t index = anr_array_find_by(ds, ptr);
	if (index == -1) return 0;

	anr_array* arr = (anr_array*)ds;

	if (index == arr->length-1) {
		arr->length--;
		return 1;
	}

	return anr_array_remove_at(ds, index);
}
uint8_t anr_array_insert(void* ds, uint32_t index, void* ptr)
{
	ANRDATA_ASSERT(ds);
	ANRDATA_ASSERT(ptr);
	anr_array* arr = (anr_array*)ds;
	if (index > arr->length) return 0;
	if (index < 0) return 0;

	if (arr->length >= arr->reserved)
	{
		arr->reserved += arr->reserve_size*arr->item_length;
		arr->data = realloc(arr->data, arr->reserved);
		ANRDATA_ASSERT(arr->data);
	}

	if (index == arr->length) return anr_array_add(ds, ptr);

	uint32_t mem_to_move = (arr->length - index) * arr->item_length;
	uint32_t mem_to_overwrite = (index+1) * arr->item_length;
	uint32_t mem_to_copy = (index) * arr->item_length;
	memmove(arr->data + mem_to_overwrite, arr->data + mem_to_copy, mem_to_move);
	memcpy(arr->data + index*arr->item_length, ptr, arr->item_length);
	arr->length++;
	return 1;
}
uint32_t anr_array_length(void* ds)
{
	ANRDATA_ASSERT(ds);
	anr_array* arr = (anr_array*)ds;
	return arr->length;
}
anr_iter anr_array_iter_start(void* ds)
{
	ANRDATA_ASSERT(ds);
	anr_iter iter;
	iter.index = -1;
	iter.data = NULL;
	return iter;
}
uint8_t anr_array_iter_next(void* ds, anr_iter* iter)
{
	ANRDATA_ASSERT(ds);
	iter->index++;
	iter->data = anr_array_find_at(ds, iter->index);
	return iter->data != NULL;
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