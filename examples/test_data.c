#define ANR_DATA_IMPLEMENTATION
#include "../anr_data.h"

void test_ds(anr_ds* list)
{
	int* d = malloc(sizeof(int));
	assert(ANR_DS_ADD(list, malloc(sizeof(int))) == 1);
	assert(ANR_DS_ADD(list, d) == 1);
	assert(ANR_DS_ADD(list, malloc(sizeof(int))) == 1);
	assert(ANR_DS_ADD(list, malloc(sizeof(int))) == 1);

	assert(ANR_DS_FIND_AT(list, 1) == d);
	assert(ANR_DS_FIND_AT(list, 4) == 0);
	assert(ANR_DS_LENGTH(list) == 4);

	ANR_DS_PRINT(list);

	assert(ANR_DS_REMOVE_BY(list, ANR_DS_FIND_AT(list, 0)) == 1);
	assert(ANR_DS_REMOVE_BY(list, ANR_DS_FIND_AT(list, 2)) == 1);
	assert(ANR_DS_REMOVE_BY(list, ANR_DS_FIND_AT(list, 1)) == 1);
	assert(ANR_DS_REMOVE_BY(list, (void*)0xFFFFFFFFF) == 0);

	assert(ANR_DS_FIND_AT(list, 0) == d);
	assert(ANR_DS_LENGTH(list) == 1);

	ANR_DS_PRINT(list);

	ANR_DS_ADD(list, malloc(sizeof(int)));
	ANR_DS_ADD(list, malloc(sizeof(int)));
	ANR_DS_ADD(list, malloc(sizeof(int)));
	ANR_DS_ADD(list, malloc(sizeof(int)));

	assert(ANR_DS_LENGTH(list) == 5);

	ANR_DS_PRINT(list);

	void* data = ANR_DS_FIND_AT(list,2);
	uint32_t index = ANR_DS_FIND_BY(list, data);
	assert(index == 2);
	assert(ANR_DS_REMOVE_AT(list, index) == 1);
	assert(ANR_DS_LENGTH(list) == 4);

	d = malloc(sizeof(int));
	assert(ANR_DS_INSERT(list, 0, d) == 1);
	assert(ANR_DS_INSERT(list, 99, d) == 0);

	assert(ANR_DS_LENGTH(list) == 5);
	assert(ANR_DS_FIND_AT(list, 0) == d);

	d = malloc(sizeof(int));
	assert(ANR_DS_INSERT(list, 3, d) == 1);

	assert(ANR_DS_LENGTH(list) == 6);
	assert(ANR_DS_FIND_AT(list, 3) == d);

	ANR_ITERATE(iter, list)
	{
		printf("#%d %p\n", iter.index, iter.data);
	}

	ANR_DS_PRINT(list);

	ANR_DS_FREE(list);
}

int main(int argc, char** argvv)
{
	anr_linked_list list = ANR_DS_LINKED_LIST;
	test_ds((anr_ds*)&list);

	return 0;
}