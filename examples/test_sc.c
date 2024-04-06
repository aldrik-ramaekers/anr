#define ANR_SC_DEBUG
#define ANR_SC_IMPLEMENTATION
#include "../anr_sc.h"

#include <time.h>
#include <stdlib.h>

void test_file(char* str)
{
	FILE* file = fopen(str, "rb");
	fseek(file, 0, SEEK_END);
	size_t iso_size = ftell(file);
	rewind(file);
	unsigned char* iso_buffer = malloc(iso_size);
	fread(iso_buffer, 1, iso_size, file);
	fclose(file);

	printf("%s:", str);
	uint32_t out;
	anr_sc_deflate(iso_buffer, iso_size, &out);
	free(iso_buffer);
}

int main(int argc, char** argv)
{ 
	//test_file("res/bible.txt");
	//test_file("res/cid2code.txt");
	test_file("res/small.txt");
	//test_file("res/cid2codesmall.txt");

	#if 0
	for (int i = 0; i < 20; i++) {
		char name[50];
		sprintf(name, "res/files/%d", i);

		test_file(name);
	}
	#endif


	return 0;
}