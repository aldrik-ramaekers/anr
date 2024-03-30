//#define ANR_DATA_DEBUG
#define ANR_DATA_IMPLEMENTATION
#include "../anr_data.h"

#include <time.h>

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

	ANR_DS_PRINT(list);

	assert(ANR_DS_REMOVE_BY(list, ANR_DS_FIND_AT(list, 0)) == 1);
	assert(ANR_DS_REMOVE_BY(list, ANR_DS_FIND_AT(list, 2)) == 1);
	assert(ANR_DS_REMOVE_BY(list, ANR_DS_FIND_AT(list, 1)) == 1);
	assert(ANR_DS_REMOVE_BY(list, rand_int()) == 0);

	//assert(*(int*)ANR_DS_FIND_AT(list, 0) == d);
	assert(ANR_DS_LENGTH(list) == 1);

	ANR_DS_PRINT(list);

	ANR_DS_ADD(list, rand_int());
	ANR_DS_ADD(list, rand_int());
	ANR_DS_ADD(list, rand_int());
	ANR_DS_ADD(list, rand_int());

	assert(ANR_DS_LENGTH(list) == 5);

	ANR_DS_PRINT(list);

	void* data = ANR_DS_FIND_AT(list,2);
	int32_t index = ANR_DS_FIND_BY(list, data);
	assert(index == 2);
	assert(ANR_DS_REMOVE_AT(list, index) == 1);
	ANR_DS_PRINT(list);
	assert(ANR_DS_LENGTH(list) == 4);

	d = *rand_int();
	assert(ANR_DS_INSERT(list, 0, &d) == 1);
	//assert(ANR_DS_INSERT(list, 99, &d) == 0);

	assert(ANR_DS_LENGTH(list) == 5);
	//assert(*(int*)ANR_DS_FIND_AT(list, 0) == d);

	d = *rand_int();
	assert(ANR_DS_INSERT(list, 3, &d) == 1);

	ANR_DS_PRINT(list);

	assert(ANR_DS_LENGTH(list) == 6);
	//assert(*(int*)ANR_DS_FIND_AT(list, 3) == d);

	ANR_ITERATE(iter, list)
	{
		#ifdef ANR_DATA_DEBUG
		printf("#%d %p\n", iter.index, iter.data);
		#endif
	}

	ANR_DS_PRINT(list);

	ANR_DS_FREE(list);
}

#define HASH_LENGTH 2048
char* random_hash()
{
	char* rr = malloc(HASH_LENGTH);
	for (int i = 0; i < HASH_LENGTH; i++)
	{
		rr[i] = rand() % 10;
	}
	return rr;
}

void rand_test(anr_ds* ds, char* hash)
{
	for (uint32_t i = 0; i < HASH_LENGTH; i++)
	{
		uint8_t ch = hash[i];
		int rand_index = (rand() % (ANR_DS_LENGTH(ds)+1))-1;
		if (ch >= 0 && ch <= 5) ANR_DS_ADD(ds, rand_int());
		if (ch == 6) ANR_DS_INSERT(ds, rand_index, rand_int());
		if (ch == 7 || ch == 8) ANR_DS_REMOVE_AT(ds, rand_index);
		if (ch == 9 && ANR_DS_LENGTH(ds) > rand_index) {
			ANR_DS_REMOVE_BY(ds, ANR_DS_FIND_AT(ds, rand_index));
		}
	}

	ANR_DS_FREE(ds);
}

#define TEST_LOOP 500
int main(int argc, char** argvv)
{
	anr_linked_list list = ANR_DS_LINKED_LIST(sizeof(int));
	test_ds((anr_ds*)&list);

	anr_array array = ANR_DS_ARRAY(sizeof(int), 1);
	test_ds((anr_ds*)&array);

	anr_hashmap hashmap = ANR_DS_HASHMAP(sizeof(int), 20);
	test_ds((anr_ds*)&hashmap);

	clock_t t = clock();
	for (int i = 0; i < TEST_LOOP; i++)
	{
		char* rand = random_hash();
		list = ANR_DS_LINKED_LIST(sizeof(int));
		rand_test((anr_ds*)&list, rand);
		free(rand);
	}
	printf("linked list took %fs\n", ((double)(clock() - t))/CLOCKS_PER_SEC); 

	t = clock();
	for (int i = 0; i < TEST_LOOP; i++)
	{
		char* rand = random_hash();
		array = ANR_DS_ARRAY(sizeof(int), 5);
		rand_test((anr_ds*)&array, rand);
		free(rand);
	}
	printf("array took %fs\n", ((double)(clock() - t))/CLOCKS_PER_SEC); 

	t = clock();
	for (int i = 0; i < TEST_LOOP; i++)
	{
		char* rand = random_hash();
		hashmap = ANR_DS_HASHMAP(sizeof(int), 20);
		rand_test((anr_ds*)&hashmap, rand);
		free(rand);
	}
	printf("hashmap took %fs\n", ((double)(clock() - t))/CLOCKS_PER_SEC); 

	return 0;
}