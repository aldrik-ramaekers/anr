//#define ANR_DATA_DEBUG
#define ANR_DATA_IMPLEMENTATION
#include "../anr_data.h"

static int* rand_int()
{
	int* da = malloc(sizeof(int));
	static int rr = 1;
	*da = rr++;
	return da;
}

void test_ds(anr_ds* list)
{
	int* d = rand_int();
	assert(ANR_DS_ADD(list, rand_int()) == 1);
	assert(ANR_DS_ADD(list, d) == 1);
	assert(ANR_DS_ADD(list, rand_int()) == 1);
	assert(ANR_DS_ADD(list, rand_int()) == 1);

	assert(ANR_DS_FIND_AT(list, 1) != 0 && *(int*)ANR_DS_FIND_AT(list, 1) == *d);
	assert(ANR_DS_FIND_AT(list, 4) == 0);
	assert(ANR_DS_LENGTH(list) == 4);

	ANR_DS_PRINT(list);

	assert(ANR_DS_REMOVE_BY(list, ANR_DS_FIND_AT(list, 0)) == 1);
	assert(ANR_DS_REMOVE_BY(list, ANR_DS_FIND_AT(list, 2)) == 1);
	assert(ANR_DS_REMOVE_BY(list, ANR_DS_FIND_AT(list, 1)) == 1);
	assert(ANR_DS_REMOVE_BY(list, rand_int()) == 0);

	assert(*(int*)ANR_DS_FIND_AT(list, 0) == *d);
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

	d = rand_int();
	assert(ANR_DS_INSERT(list, 0, d) == 1);
	assert(ANR_DS_INSERT(list, 99, d) == 0);

	assert(ANR_DS_LENGTH(list) == 5);
	assert(*(int*)ANR_DS_FIND_AT(list, 0) == *d);

	d = rand_int();
	assert(ANR_DS_INSERT(list, 3, d) == 1);

	ANR_DS_PRINT(list);

	assert(ANR_DS_LENGTH(list) == 6);
	assert(*(int*)ANR_DS_FIND_AT(list, 3) == *d);

	ANR_ITERATE(iter, list)
	{
		printf("#%d %p\n", iter.index, iter.data);
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
		int rand_index = rand() % (ANR_DS_LENGTH(ds)+1)-1;
		if (ch >= 0 && ch <= 5) ANR_DS_ADD(ds, rand_int());
		if (ch == 6) ANR_DS_INSERT(ds, rand_index, rand_int());
		if (ch == 7 || ch == 8) ANR_DS_REMOVE_AT(ds, rand_index);
		if (ch == 9 && ANR_DS_LENGTH(ds) > rand_index) {
			ANR_DS_REMOVE_BY(ds, ANR_DS_FIND_AT(ds, rand_index));
		}
	}
	ANR_DS_PRINT(ds);
}

int main(int argc, char** argvv)
{
	anr_linked_list list = ANR_DS_LINKED_LIST;
	test_ds((anr_ds*)&list);

	anr_array array = ANR_DS_ARRAY(sizeof(int), 1);
	test_ds((anr_ds*)&array);

	for (int i = 0; i < 10; i++)
	{
		char* rand = random_hash();

		list = ANR_DS_LINKED_LIST;
		rand_test((anr_ds*)&list, rand);

		array = ANR_DS_ARRAY(sizeof(int), 5);
		rand_test((anr_ds*)&array, rand);
		free(rand);
	}

	return 0;
}