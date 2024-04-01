#define ANR_DATA_DEBUG
//#define ANR_DATA_FULL_TEST_REPORT
#define ANR_DATA_IMPLEMENTATION
#include "../anr_data.h"

#include <time.h>

#define TEST_LOOP 1
#if 1
#define HASH_LENGTH 50000
#define ADD_REMOVE_COUNT 200000
#else
#define HASH_LENGTH 2000
#define ADD_REMOVE_COUNT 50000
#endif

int intptr;
static int* rand_int()
{
	static int rr = 1;
	intptr = rr++;
	return &intptr;
}

void test_ds(anr_ds* list)
{
	int d = *rand_int();
	assert(ANR_DS_ADD(list, rand_int()) == 0);
	assert(ANR_DS_ADD(list, &d) == 1);
	assert(ANR_DS_ADD(list, rand_int()) == 2);
	assert(ANR_DS_ADD(list, rand_int()) == 3);

	
	assert(ANR_DS_FIND_AT(list, 1) != 0 && *(int*)ANR_DS_FIND_AT(list, 1) == d);
	assert(ANR_DS_FIND_AT(list, 4) == 0);
	assert(ANR_DS_LENGTH(list) == 4);

	//ANR_DS_PRINT(list);

	assert(ANR_DS_REMOVE_BY(list, ANR_DS_FIND_AT(list, 0)) == 1);
	assert(ANR_DS_REMOVE_BY(list, ANR_DS_FIND_AT(list, 2)) == 1);
	assert(ANR_DS_REMOVE_BY(list, ANR_DS_FIND_AT(list, 1)) == 1);
	//assert(ANR_DS_REMOVE_BY(list, rand_int()) == 0);

	//assert(*(int*)ANR_DS_FIND_AT(list, 0) == d);
	assert(ANR_DS_LENGTH(list) == 1);

	//ANR_DS_PRINT(list);

	ANR_DS_ADD(list, rand_int());
	ANR_DS_ADD(list, rand_int());
	ANR_DS_ADD(list, rand_int());
	ANR_DS_ADD(list, rand_int());

	assert(ANR_DS_LENGTH(list) == 5);

	//ANR_DS_PRINT(list);

	void* data = ANR_DS_FIND_AT(list,2);
	int32_t index = ANR_DS_FIND_BY(list, data);
	assert(index == 2);
	assert(ANR_DS_REMOVE_AT(list, index) == 1);
	//ANR_DS_PRINT(list);
	assert(ANR_DS_LENGTH(list) == 4);

	d = *rand_int();
	assert(ANR_DS_INSERT(list, 0, &d) == 1);
	//assert(ANR_DS_INSERT(list, 99, &d) == 0);

	assert(ANR_DS_LENGTH(list) == 5);
	//assert(*(int*)ANR_DS_FIND_AT(list, 0) == d);

	d = *rand_int();
	assert(ANR_DS_INSERT(list, 3, &d) == 1);

	//ANR_DS_PRINT(list);

	assert(ANR_DS_LENGTH(list) == 6);
	//assert(*(int*)ANR_DS_FIND_AT(list, 3) == d);

	ANR_ITERATE(iter, list)
	{
		#ifdef ANR_DATA_DEBUG
		//printf("#%d %p\n", iter.index, iter.data);
		#endif
	}

	int data7 = *rand_int();
	int data8 = *rand_int();
	int data9 = *rand_int();
	ANR_DS_ADD(list, rand_int());
	ANR_DS_INSERT(list, 7, &data7);
	ANR_DS_INSERT(list, 8, &data8);
	ANR_DS_INSERT(list, 9, &data9);
	assert(ANR_DS_LENGTH(list) == 10);
	int* found = (int*)ANR_DS_FIND_AT(list, 8);
	if (found) {
		ANR_DS_REMOVE_BY(list, ANR_DS_FIND_AT(list, 8));
	}
	

	found = (int*)ANR_DS_FIND_AT(list, 8);
	if (found) { // Only false for hashmaps.
		assert(*found == data9); 
	}

	//ANR_DS_PRINT(list);

	ANR_DS_FREE(list);
}


char* random_hash()
{
	char* rr = malloc(HASH_LENGTH+1);
	FILE* f;
	#if 0
	for (int i = 0; i < HASH_LENGTH; i++)
	{
		rr[i] = rand() % 10;
	}
	rr[HASH_LENGTH] = 0;

	f = fopen("hash.txt", "w+");
	fwrite(rr, 1, HASH_LENGTH, f);
	fclose(f);
	#endif

	f = fopen("hash.txt", "rw");
	fread(rr, 1, HASH_LENGTH, f);
	fclose(f);
	return rr;
}

void add_remove_test(anr_ds* ds)
{
	#ifdef ANR_DATA_FULL_TEST_REPORT
	clock_t t = clock();
	#endif
	for (uint32_t i = 0; i < ADD_REMOVE_COUNT; i++)
	{
		ANR_DS_INSERT(ds, i, rand_int());
	}
	#ifdef ANR_DATA_FULL_TEST_REPORT
	printf("-- add %.3fs\n", ((double)(clock() - t))/CLOCKS_PER_SEC);
	#endif

	#ifdef ANR_DATA_FULL_TEST_REPORT
	t = clock();
	#endif
	uint32_t rand_index = 5;
	for (uint32_t i = 0; i < ADD_REMOVE_COUNT; i++)
	{
		rand_index += 100;
		if (rand_index >= ANR_DS_LENGTH(ds)) rand_index = 0;
		ANR_DS_REMOVE_AT(ds, rand_index);
	}
	#ifdef ANR_DATA_FULL_TEST_REPORT
	printf("-- remove %.3fs\n", ((double)(clock() - t))/CLOCKS_PER_SEC); 
	#endif

	ANR_DS_FREE(ds);
}

void rand_test(anr_ds* ds, char* hash)
{
	for (uint32_t i = 0; i < HASH_LENGTH; i++)
	{
		uint8_t ch = hash[i];
		int rand_index = (rand() % (ANR_DS_LENGTH(ds)+1))-1;
		if (rand_index < 0) rand_index = 0;
		if (ch >= 0 && ch <= 5) ANR_DS_ADD(ds, rand_int());
		if (ch == 6) ANR_DS_INSERT(ds, rand_index, rand_int());
		if (ch == 7 || ch == 8) ANR_DS_REMOVE_AT(ds, rand_index);
		if (ch == 9 && ANR_DS_LENGTH(ds) > rand_index) {
			ANR_DS_REMOVE_BY(ds, ANR_DS_FIND_AT(ds, rand_index));
		}
	}

	ANR_DS_FREE(ds);
}


int main(int argc, char** argvv)
{
	anr_linked_list list = ANR_DS_LINKED_LIST(sizeof(int));
	test_ds((anr_ds*)&list);

	anr_array array = ANR_DS_ARRAY(sizeof(int), 1);
	test_ds((anr_ds*)&array);

	anr_hashmap hashmap = ANR_DS_HASHMAP(sizeof(int), 20);
	test_ds((anr_ds*)&hashmap);

	char* rand = random_hash();
	clock_t t = clock();

	for (int i = 0; i < TEST_LOOP; i++)
	{
		list = ANR_DS_LINKED_LIST(sizeof(int));
		rand_test((anr_ds*)&list, rand);
	}
	printf("linked list fuzzing 		%.3fs\n", ((double)(clock() - t))/CLOCKS_PER_SEC); 

	t = clock();
	for (int i = 0; i < TEST_LOOP; i++)
	{
		char* rand = random_hash();
		array = ANR_DS_ARRAY(sizeof(int), 5);
		rand_test((anr_ds*)&array, rand);
	}
	printf("array fuzzing 			%.3fs\n", ((double)(clock() - t))/CLOCKS_PER_SEC); 

	t = clock();
	for (int i = 0; i < TEST_LOOP; i++)
	{
		char* rand = random_hash();
		hashmap = ANR_DS_HASHMAP(sizeof(int), 20);
		rand_test((anr_ds*)&hashmap, rand);
	}
	printf("hashmap fuzzing 		%.3fs\n", ((double)(clock() - t))/CLOCKS_PER_SEC); 
	free(rand);

	t = clock();
	list = ANR_DS_LINKED_LIST(sizeof(int));
	add_remove_test((anr_ds*)&list);
	printf("linkedlist addremove 		%.3fs\n", ((double)(clock() - t))/CLOCKS_PER_SEC); 

	t = clock();
	array = ANR_DS_ARRAY(sizeof(int), ADD_REMOVE_COUNT);
	add_remove_test((anr_ds*)&array);
	printf("array addremove 		%.3fs\n", ((double)(clock() - t))/CLOCKS_PER_SEC); 

	t = clock();
	hashmap = ANR_DS_HASHMAP(sizeof(int), ADD_REMOVE_COUNT);
	add_remove_test((anr_ds*)&hashmap);

	printf("hashmap addremove 		%.3fs\n", ((double)(clock() - t))/CLOCKS_PER_SEC); 

	return 0;
}

/*
TEST RESULTS: 50000 hash length, 200000 add remove

[v0.4] i7 4770
linked list fuzzing             2.967s
array fuzzing                   0.017s
hashmap fuzzing                 0.347s
linkedlist addremove            0.541s
array addremove                 4.365s
hashmap addremove               0.022s

*/