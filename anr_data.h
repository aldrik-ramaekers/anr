/*
anr_data.h - v0.2 - public domain data structures library

This is a single-header-file library that provides basic data structures

Do this:
	#ifdef ANR_DATA_IMPLEMENTATION
before you include this file in *one* C or C++ file to create the implementation.

The data structures are completely interchangeable if you use the macros. See examples/test_data.c

LICENSE
	See end of file for license information.
	
*/
#ifndef INCLUDE_ANR_DATA_H
#define INCLUDE_ANR_DATA_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

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
	ANR_DS_HASHMAP = 2,
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
	uint32_t data_size;
} anr_linked_list;

typedef struct
{
	anr_ds_type ds_type;
	void* data;
	int32_t data_size;
	int32_t reserve_size;
	int32_t reserved;
	int32_t length;
} anr_array;

typedef struct
{
	uint32_t bucket_start;
	uint32_t length;
	void* data;
} anr_hashmap_bucket;

typedef struct
{
	anr_ds_type ds_type;
	uint32_t bucket_size;
	anr_array buckets;
	uint32_t data_size;
	uint32_t length;
} anr_hashmap;

typedef struct
{
	int32_t index;
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
	int32_t 	(*add)(void* ds, void* ptr); // returns index on success, -1 on fail
	void 		(*free)(void* ds);
	void 		(*print)(void* ds);
	void* 		(*find_at)(void*,uint32_t); // returns data
	uint32_t 	(*find_by)(void* ds, char* ptr); // returns index, or -1 if not found
	uint8_t 	(*remove_at)(void* ds, uint32_t index); // returns 1 on success, 0 on fail
	uint8_t 	(*remove_by)(void* ds, void* ptr); // returns 1 on success, 0 on fail
	uint8_t 	(*insert)(void* ds, uint32_t index, void* ptr); // returns 1 on success, 0 on fail
	uint32_t 	(*length)(void* ds);
	anr_iter 	(*iter_start)(void* ds);
	uint8_t 	(*iter_next)(void* ds, anr_iter* iter); // returns 1 on success, 0 if no more items to iterate
} anr_ds_table;

typedef struct
{
	int type;
	anr_ds_table* ds;
} anr_ds_pair;

// === linked list ===
ANRDATADEF anr_linked_list 	anr_linked_list_create(uint32_t data_size);
ANRDATADEF int32_t	 		anr_linked_list_add(void* ds, void* ptr);
ANRDATADEF void 			anr_linked_list_free(void* ds);
ANRDATADEF void 			anr_linked_list_print(void* ds);
ANRDATADEF void* 			anr_linked_list_find_at(void* ds, uint32_t index);
ANRDATADEF uint32_t 		anr_linked_list_find_by(void* ds, char* ptr);
ANRDATADEF uint8_t 			anr_linked_list_remove_at(void* ds, uint32_t index);
ANRDATADEF uint8_t 			anr_linked_list_remove_by(void* ds, void* ptr);
ANRDATADEF uint8_t 			anr_linked_list_insert(void* ds, uint32_t index, void* ptr);
ANRDATADEF uint32_t 		anr_linked_list_length(void* ds);
ANRDATADEF anr_iter 		anr_linked_list_iter_start(void* ds);
ANRDATADEF uint8_t 			anr_linked_list_iter_next(void* ds, anr_iter* iter);

// === dynamic array ===
ANRDATADEF anr_array 	anr_array_create(uint32_t data_size, uint32_t reserve_count);
ANRDATADEF int32_t 		anr_array_add(void* ds, void* ptr);
ANRDATADEF void 		anr_array_free(void* ds);
ANRDATADEF void 		anr_array_print(void* ds);
ANRDATADEF void* 		anr_array_find_at(void* ds, uint32_t index);
ANRDATADEF uint32_t 	anr_array_find_by(void* ds, char* ptr);
ANRDATADEF uint8_t 		anr_array_remove_at(void* ds, uint32_t index);
ANRDATADEF uint8_t 		anr_array_remove_by(void* ds, void* ptr);
ANRDATADEF uint8_t 		anr_array_insert(void* ds, uint32_t index, void* ptr);
ANRDATADEF uint32_t 	anr_array_length(void* ds);
ANRDATADEF anr_iter 	anr_array_iter_start(void* ds);
ANRDATADEF uint8_t 		anr_array_iter_next(void* ds, anr_iter* iter);

// === hashmap ===
ANRDATADEF anr_hashmap 	anr_hashmap_create(uint32_t data_size, uint32_t bucket_size);
ANRDATADEF int32_t	 	anr_hashmap_add(void* ds, void* ptr);
ANRDATADEF void 		anr_hashmap_free(void* ds);
ANRDATADEF void 		anr_hashmap_print(void* ds);
ANRDATADEF void* 		anr_hashmap_find_at(void* ds, uint32_t index);
ANRDATADEF uint32_t 	anr_hashmap_find_by(void* ds, char* ptr);
ANRDATADEF uint8_t 		anr_hashmap_remove_at(void* ds, uint32_t index);
ANRDATADEF uint8_t 		anr_hashmap_remove_by(void* ds, void* ptr);
ANRDATADEF uint8_t 		anr_hashmap_insert(void* ds, uint32_t index, void* ptr);
ANRDATADEF uint32_t 	anr_hashmap_length(void* ds);
ANRDATADEF anr_iter 	anr_hashmap_iter_start(void* ds);
ANRDATADEF uint8_t 		anr_hashmap_iter_next(void* ds, anr_iter* iter);

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

anr_ds_table _ds_hashmap = 
{
	anr_hashmap_add,
	anr_hashmap_free,
	anr_hashmap_print,
	anr_hashmap_find_at,
	anr_hashmap_find_by,
	anr_hashmap_remove_at,
	anr_hashmap_remove_by,
	anr_hashmap_insert,
	anr_hashmap_length,
	anr_hashmap_iter_start,
	anr_hashmap_iter_next,
};

anr_ds_pair _ds_arr[] = 
{
	{ANR_DS_LINKEDLIST, &_ds_ll},
	{ANR_DS_DYNAMIC_ARRAY, &_ds_array},
	{ANR_DS_HASHMAP, &_ds_hashmap},
};

#define ANR_DS_ARRAY(_data_size, _reserve_count) anr_array_create(_data_size, _reserve_count)
#define ANR_DS_LINKED_LIST(_data_size) anr_linked_list_create(_data_size)
#define ANR_DS_HASHMAP(_data_size, _bucket_size) anr_hashmap_create(_data_size, _bucket_size)

#define ANR_DS_ADD(__ds, __ptr) (_ds_arr[(int)(((anr_ds*)__ds)->ds_ll.ds_type)]).ds->add((void*)__ds, (void*)__ptr)
#define ANR_DS_FREE(__ds) (_ds_arr[(int)(((anr_ds*)__ds)->ds_ll.ds_type)]).ds->free((void*)__ds)
#define ANR_DS_PRINT(__ds) (_ds_arr[(int)(((anr_ds*)__ds)->ds_ll.ds_type)]).ds->print((void*)__ds)
#define ANR_DS_FIND_AT(__ds, __index) (_ds_arr[(int)(((anr_ds*)__ds)->ds_ll.ds_type)]).ds->find_at((void*)__ds, __index)
#define ANR_DS_FIND_BY(__ds, __ptr) (_ds_arr[(int)(((anr_ds*)__ds)->ds_ll.ds_type)]).ds->find_by((void*)__ds, (void*)__ptr)
#define ANR_DS_REMOVE_BY(__ds, __ptr) (_ds_arr[(int)(((anr_ds*)__ds)->ds_ll.ds_type)]).ds->remove_by((void*)__ds, (void*)__ptr)
#define ANR_DS_REMOVE_AT(__ds, __index) (_ds_arr[(int)(((anr_ds*)__ds)->ds_ll.ds_type)]).ds->remove_at((void*)__ds, __index)
#define ANR_DS_INSERT(__ds, __index, __ptr) (_ds_arr[(int)(((anr_ds*)__ds)->ds_ll.ds_type)]).ds->insert((void*)__ds, __index, (void*)__ptr)
#define ANR_DS_LENGTH(__ds) (_ds_arr[(int)(((anr_ds*)__ds)->ds_ll.ds_type)]).ds->length((void*)__ds)
#define ANR_DS_ITER_START(__ds) (_ds_arr[(int)(((anr_ds*)__ds)->ds_ll.ds_type)]).ds->iter_start((void*)__ds)
#define ANR_DS_ITER_NEXT(__ds, __iter) (_ds_arr[(int)(((anr_ds*)__ds)->ds_ll.ds_type)]).ds->iter_next((void*)__ds, __iter)

#define ANR_ITERATE(__iter, __ds) \
	anr_iter __iter = ANR_DS_ITER_START((void*)__ds); \
	while (ANR_DS_ITER_NEXT((void*)__ds, &__iter))

#endif // INCLUDE_ANR_DATA_H

#ifdef ANR_DATA_IMPLEMENTATION

#ifdef ANR_DATA_DEBUG
anr_linked_list curr_print = (anr_linked_list){ANR_DS_LINKEDLIST, 0, 0, 0, 200};
anr_linked_list prev_print = (anr_linked_list){ANR_DS_LINKEDLIST, 0, 0, 0, 200};

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
			gotox(85);
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
	curr_print = (anr_linked_list){ANR_DS_LINKEDLIST, 0, 0, 0, 200};
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
	iter->data = iter->ll.node != NULL ? ((uint8_t*)iter->ll.node)+offsetof(anr_linked_list_node, data) : 0;
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

	anr_linked_list_node* prev = NULL;
	anr_linked_list_node* next = NULL;

	if (index == list->length) {
		count = index;
		prev = list->last;
		next = NULL;
	}
	else {
		while (iter)
		{
			if (count == index) {
				break;
			}
			iter = iter->next;
			count++;
		}

		if (!iter) return 0; // out of bounds.

		prev = iter->prev;
		next = iter;
	}

	anr_linked_list_node* node = malloc(sizeof(anr_linked_list_node) + list->data_size - sizeof(void*));
	if (!node) return 0;
	memcpy(((uint8_t*)node)+offsetof(anr_linked_list_node, data), ptr, list->data_size);
	node->prev = prev;
	node->next = next;

	if (count == 0) {
		list->first = node;
	}
	if (count == list->length) {
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
	uint8_t search_backwards = index > (list->length/2);
	anr_linked_list_node* iter = search_backwards ? list->last : list->first;
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

			free(iter);
			list->length--;
			return 1;
		}
		iter = search_backwards ? iter->prev : iter->next;
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
		void* data = ((uint8_t*)iter)+offsetof(anr_linked_list_node, data);
		if (memcmp(data, ptr, list->data_size) == 0) {
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
		void* data = ((uint8_t*)iter)+offsetof(anr_linked_list_node, data);
		if (memcmp(data, ptr, list->data_size) == 0) {
			return count;
		}
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
		if (count == index) return ((uint8_t*)iter)+offsetof(anr_linked_list_node, data);
		count++;
		iter = iter->next;
	}
	return 0;
}

anr_linked_list anr_linked_list_create(uint32_t data_size)
{
	return (anr_linked_list){ANR_DS_LINKEDLIST, 0, 0, 0, data_size};
}

int32_t anr_linked_list_add(void* ds, void* ptr)
{
	ANRDATA_ASSERT(ds);
	ANRDATA_ASSERT(ptr);
	anr_linked_list* list = ds;
	anr_linked_list_node* iter = list->first;
	do
	{
		if (iter == NULL || iter->next == NULL) {
			anr_linked_list_node* node = malloc(sizeof(anr_linked_list_node) + list->data_size - sizeof(void*));
			if (!node) return -1;
			memcpy(((uint8_t*)node)+offsetof(anr_linked_list_node, data), ptr, list->data_size);
			node->prev = iter;
			node->next = NULL;
			iter == NULL ? (list->first = node) : (iter->next = node);
			list->last = node;
			list->length++;
			return list->length-1;
		}

		iter = iter->next;
	} while (iter);
	
	ANRDATA_ASSERT(0);
	return -1;
}

anr_array anr_array_create(uint32_t data_size, uint32_t reserve_count)
{
	ANRDATA_ASSERT(data_size > 0);
	ANRDATA_ASSERT(reserve_count > 0);

	anr_array arr = (anr_array){ANR_DS_DYNAMIC_ARRAY, .data = 0, .data_size = data_size, .length = 0, .reserve_size = reserve_count, .reserved = 0};
	arr.reserved = reserve_count;
	arr.data = malloc(arr.reserved*data_size);
	if (!arr.data) {
		arr.reserve_size = 1;
		arr.data = malloc(arr.reserved*data_size); // Try again with smallest possible size.
		ANRDATA_ASSERT(arr.data);
	}

	return arr;
}

int32_t anr_array_add(void* ds, void* ptr)
{
	ANRDATA_ASSERT(ds);
	ANRDATA_ASSERT(ptr);

	anr_array* arr = (anr_array*)ds;

	arr->length++;
	if (arr->reserved < arr->length)
	{
		arr->reserved += arr->reserve_size;
		void* b = realloc(arr->data, arr->reserved*arr->data_size);
		if (b) arr->data = b;
		else return -1;
	}

	memcpy(arr->data + ((arr->length-1) * arr->data_size), ptr, arr->data_size);

	return arr->length-1;
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
		for (int x = 0; x < arr->data_size; x++) {
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

	return arr->data + (index * arr->data_size);
}

uint32_t anr_array_find_by(void* ds, char* ptr)
{
	ANRDATA_ASSERT(ds);
	ANRDATA_ASSERT(ptr);
	anr_array* arr = (anr_array*)ds;

	for (int i = 0; i < arr->length; i++)
	{
		void* data = anr_array_find_at(ds, i);
		if (memcmp(data, ptr, arr->data_size) == 0) {
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
	uint32_t mem_to_move = (arr->length - index - 1) * arr->data_size;
	uint32_t mem_to_overwrite = index * arr->data_size;
	uint32_t mem_to_copy = (index+1) * arr->data_size;
	memmove(arr->data + mem_to_overwrite, arr->data + mem_to_copy, mem_to_move);
	arr->length--;

	if (arr->length < arr->reserved / 2) {
		arr->reserved /= 2;
		void* b = realloc(arr->data, arr->reserved*arr->data_size);
		if (b) arr->data = b;
	}
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
		arr->reserved += arr->reserve_size;
		void* b = realloc(arr->data, arr->reserved*arr->data_size);
		if (b) arr->data = b;
		else return 0;
	}

	if (index == arr->length) return anr_array_add(ds, ptr);

	uint32_t mem_to_move = (arr->length - index) * arr->data_size;
	uint32_t mem_to_overwrite = (index+1) * arr->data_size;
	uint32_t mem_to_copy = (index) * arr->data_size;
	memmove(arr->data + mem_to_overwrite, arr->data + mem_to_copy, mem_to_move);
	memcpy(arr->data + index*arr->data_size, ptr, arr->data_size);
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

anr_hashmap anr_hashmap_create(uint32_t data_size, uint32_t bucket_size)
{
	ANRDATA_ASSERT(data_size > 0);
	ANRDATA_ASSERT(bucket_size > 0);
	anr_hashmap hashmap = (anr_hashmap){.ds_type = ANR_DS_HASHMAP, .bucket_size = bucket_size, .data_size = data_size};
	hashmap.buckets = anr_array_create(sizeof(anr_hashmap_bucket), 1);
	return hashmap;
}

int32_t anr_hashmap_add(void* ds, void* ptr)
{
	ANRDATA_ASSERT(ds);
	ANRDATA_ASSERT(ptr);
	anr_hashmap* hashmap = (anr_hashmap*)ds;
	uint32_t item_size = hashmap->data_size + 1;

	uint32_t highest_bucket_start = 0;
	ANR_ITERATE(iter, &hashmap->buckets)
	{
		anr_hashmap_bucket* bb = (anr_hashmap_bucket*)iter.data;
		if (bb->bucket_start > highest_bucket_start) highest_bucket_start = bb->bucket_start;
		for (int i = 0; i < hashmap->bucket_size; i++)
		{
			char* data = bb->data + (i * item_size);
			if (data[0] == 1) continue;
			memcpy(data+1, ptr, hashmap->data_size);
			data[0] = 1;
			hashmap->length++;
			bb->length++;
			return bb->bucket_start + i;
		}
	}

	// All buckets are full, create new one.
	anr_hashmap_bucket new_bucket;
	new_bucket.bucket_start = highest_bucket_start;
	new_bucket.length = 0;
	uint32_t alloc_size = (hashmap->bucket_size * hashmap->data_size) + hashmap->bucket_size;
	new_bucket.data = malloc(alloc_size);
	if (!new_bucket.data) return -1;
	memset(new_bucket.data, 0, alloc_size);
	uint32_t bucket_index = anr_array_add(&hashmap->buckets, &new_bucket);
	anr_hashmap_bucket* bucket = anr_array_find_at(&hashmap->buckets, bucket_index);

	hashmap->length++;
	char* data = bucket->data + (0 * item_size);
	memcpy(data+1, ptr, hashmap->data_size);
	data[0] = 1;
	return new_bucket.bucket_start;
}

void anr_hashmap_free(void* ds)
{
	ANRDATA_ASSERT(ds);
	anr_hashmap* hashmap = (anr_hashmap*)ds;
	ANR_ITERATE(iter, &hashmap->buckets)
	{
		anr_hashmap_bucket* bb = (anr_hashmap_bucket*)iter.data;
		free(bb->data);
	}
	ANR_DS_FREE(&hashmap->buckets);
}

void anr_hashmap_print(void* ds)
{
	ANRDATA_ASSERT(ds);
	/*
	anr_hashmap* hashmap = (anr_hashmap*)ds;
	ANR_ITERATE(iter, &hashmap->buckets)
	{
		anr_hashmap_bucket* bb = (anr_hashmap_bucket*)iter.data;
		free(bb->data);
	}
	ANR_DS_FREE(&hashmap->buckets);
	*/
}

void* anr_hashmap_find_at(void* ds, uint32_t index)
{
	ANRDATA_ASSERT(ds);
	
	anr_hashmap* hashmap = (anr_hashmap*)ds;

	int bucket_start = (index / hashmap->bucket_size) * hashmap->bucket_size;
	int inner_index = index % hashmap->bucket_size;
	uint32_t item_size = hashmap->data_size + 1;

	ANR_ITERATE(iter, &hashmap->buckets)
	{
		anr_hashmap_bucket* bb = (anr_hashmap_bucket*)iter.data;
		if (bb->bucket_start == bucket_start) {
			char* data = bb->data + (inner_index * item_size);
			if (data[0] == 1) return data+1;
			
		}
	}
	return 0;
}

uint32_t anr_hashmap_find_by(void* ds, char* ptr)
{
	ANRDATA_ASSERT(ds);
	if (ptr == NULL) return -1;
	anr_hashmap* hashmap = (anr_hashmap*)ds;
	uint32_t item_size = hashmap->data_size + 1;

	ANR_ITERATE(iter, &hashmap->buckets)
	{
		anr_hashmap_bucket* bb = (anr_hashmap_bucket*)iter.data;
		for (int i = 0; i < hashmap->bucket_size; i++)
		{
			char* data = bb->data + (i * item_size);
			if (data[0] == 1) {
				if (memcmp(data+1, ptr, hashmap->data_size) == 0) return bb->bucket_start + i;
			}
		}
	}

	return -1;
}

uint8_t anr_hashmap_remove_at(void* ds, uint32_t index)
{
	ANRDATA_ASSERT(ds);
	anr_hashmap* hashmap = (anr_hashmap*)ds;

	int bucket_start = (index / hashmap->bucket_size) * hashmap->bucket_size;
	int inner_index = index % hashmap->bucket_size;

	ANR_ITERATE(iter, &hashmap->buckets)
	{
		anr_hashmap_bucket* bb = (anr_hashmap_bucket*)iter.data;
		if (bb->bucket_start == bucket_start) {
			uint32_t item_size = hashmap->data_size + 1;
			char* data = bb->data + (inner_index * item_size);
			if (data[0] == 1) {
				data[0] = 0;
				hashmap->length--;
				bb->length--;

				if (bb->length == 0) {
					ANR_DS_REMOVE_AT(&hashmap->buckets, iter.index);
				}
				return 1;
			}
		}
	}
	return 0;
}

uint8_t anr_hashmap_remove_by(void* ds, void* ptr)
{
	ANRDATA_ASSERT(ds);
	uint32_t index = anr_hashmap_find_by(ds, ptr);
	if (index == -1) return 0;
	anr_hashmap_remove_at(ds, index);
	return 1;
}

uint8_t anr_hashmap_insert(void* ds, uint32_t index, void* ptr)
{
	ANRDATA_ASSERT(ds);
	ANRDATA_ASSERT(ptr);
	anr_hashmap* hashmap = (anr_hashmap*)ds;

	int bucket_start = (index / hashmap->bucket_size) * hashmap->bucket_size;
	int inner_index = index % hashmap->bucket_size;

	anr_hashmap_bucket* bucket = NULL;
	ANR_ITERATE(iter, &hashmap->buckets)
	{
		anr_hashmap_bucket* bb = (anr_hashmap_bucket*)iter.data;
		if (bb->bucket_start == bucket_start) {
			bucket = bb;
			break;
		}
	}

	if (!bucket) {
		// Bucket does not exist yet. create one.
		anr_hashmap_bucket new_bucket;
		new_bucket.bucket_start = bucket_start;
		new_bucket.length = 0;
		uint32_t alloc_size = (hashmap->bucket_size * hashmap->data_size) + hashmap->bucket_size;
		new_bucket.data = malloc(alloc_size);
		if (!new_bucket.data) return 0;
		memset(new_bucket.data, 0, alloc_size);
		uint32_t bucket_index = anr_array_add(&hashmap->buckets, &new_bucket);
		bucket = anr_array_find_at(&hashmap->buckets, bucket_index);
	}

	uint32_t item_size = hashmap->data_size + 1;
	hashmap->length++;
	bucket->length++;
	char* data = bucket->data + (inner_index * item_size);
	data[0] = 1;
	memcpy(data+1, ptr, hashmap->data_size);
	return 1;
}

uint32_t anr_hashmap_length(void* ds)
{
	ANRDATA_ASSERT(ds);
	anr_hashmap* hashmap = (anr_hashmap*)ds;
	return hashmap->length;
}

anr_iter anr_hashmap_iter_start(void* ds)
{
	ANRDATA_ASSERT(ds);
	anr_iter iter;
	iter.data = NULL;
	iter.index = -1;
	return iter;
}

uint8_t anr_hashmap_iter_next(void* ds, anr_iter* iter)
{
	ANRDATA_ASSERT(ds);
	ANRDATA_ASSERT(iter);
	anr_hashmap* hashmap = (anr_hashmap*)ds;
	uint32_t item_size = hashmap->data_size + 1;

	ANR_ITERATE(iter2, &hashmap->buckets)
	{
		anr_hashmap_bucket* bb = (anr_hashmap_bucket*)iter2.data;
		for (int i = 0; i < hashmap->bucket_size; i++)
		{
			int32_t abs_index = bb->bucket_start + i;
			char* data = bb->data + (i * item_size);
			if (data[0] == 0) continue;
			if (abs_index <= iter->index) continue;

			iter->index = i;
			iter->data = data+1;
			return 1;
		}
	}
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