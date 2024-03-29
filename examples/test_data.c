#define ANR_DATA_IMPLEMENTATION
#include "../anr_data.h"

void test_linked_list()
{
	anr_linked_list list = anr_linked_list_create();
	anr_linked_list_add(&list, malloc(sizeof(int)));
	anr_linked_list_add(&list, malloc(sizeof(int)));
	anr_linked_list_add(&list, malloc(sizeof(int)));
	anr_linked_list_add(&list, malloc(sizeof(int)));

	anr_linked_list_print(&list);

	anr_linked_list_remove_by(&list, anr_linked_list_find_at(&list, 0));
	anr_linked_list_remove_by(&list, anr_linked_list_find_at(&list, 2));
	anr_linked_list_remove_by(&list, anr_linked_list_find_at(&list, 1));

	anr_linked_list_print(&list);

	anr_linked_list_add(&list, malloc(sizeof(int)));
	anr_linked_list_add(&list, malloc(sizeof(int)));
	anr_linked_list_add(&list, malloc(sizeof(int)));
	anr_linked_list_add(&list, malloc(sizeof(int)));

	anr_linked_list_print(&list);

	void* data = anr_linked_list_find_at(&list,2);
	uint32_t index = anr_linked_list_find_by(&list, data);
	assert(index == 2);
	anr_linked_list_remove_at(&list, index);

	anr_linked_list_print(&list);

	anr_linked_list_free(&list);
}

int main(int argc, char** argvv)
{
	test_linked_list();

	return 0;
}