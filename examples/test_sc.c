#define ANR_SC_DEBUG
#define ANR_SC_IMPLEMENTATION
#include "../anr_sc.h"

#include <time.h>
#include <stdlib.h>

int big_matrix[] = {
	0,1,5,7,4,2,1,4,5,0xf,1,0,0,0,1,1,4,6,0xa,1,2,4,4,4,4,4,3,2,1,5,5,5,5,5,6,6,6,7,8,8,8,9,1,2,3,4,4,4,3,3,2,2,9,9,9,8,7,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

int* random_matrix_2d(int x, int y, int percentage) {
	srand(time(NULL));
	int* matrix = malloc(x * y * sizeof(int));

	for (int i = 0; i < x*y; i++) {
		if ((rand() % 100) >= percentage) matrix[i] = rand()%500;
		else matrix[i] = 0;
	}

	return matrix;
}

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
	uint32_t out;
	anr_sc_deflate(big_matrix, sizeof(big_matrix), &out);
	/*
	for (int i = 0; i < 50; i++) {
		int* random_2d_matrix = random_matrix_2d(20, 20, 100 - i);

		uint32_t out;
		anr_sc_deflate(random_2d_matrix, 20*20*sizeof(int), &out);
	}*/
	#endif


	return 0;
}