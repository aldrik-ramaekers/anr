#define ANR_SC_DEBUG
#define ANR_SC_IMPLEMENTATION
#include "../anr_sc.h"

#include <time.h>
#include <stdlib.h>

int* random_matrix_2d(int x, int y, int percentage) {
	srand(time(NULL));
	int* matrix = malloc(x * y * sizeof(int));

	for (int i = 0; i < x*y; i++) {
		if ((rand() % 100) >= percentage) matrix[i] = rand()%500;
		else matrix[i] = 0;
	}

	return matrix;
}

int main(int argc, char** argv)
{
	for (int i = 0; i < 100; i++) {
		int* random_2d_matrix = random_matrix_2d(20, 20, 100 - i);

		uint32_t out;
		anr_sc_deflate(random_2d_matrix, 20*20*sizeof(int), &out);
	}

	return 0;
}