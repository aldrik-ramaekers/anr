//#define ANR_SC_DEBUG
#define ANR_SC_IMPLEMENTATION
#include "../anr_sc.h"

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0') 

void test_small()
{
	uint8_t buffer[] = { 0, 32, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
	//uint8_t buffer[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
	uint32_t out;
	uint8_t* compressed_data = anr_sc_deflate(buffer, sizeof(buffer), &out);

#if 1
	printf("\n\n");
	for (int i = 0; i < out; i++)
	{
		printf(BYTE_TO_BINARY_PATTERN " ", BYTE_TO_BINARY(compressed_data[i]));
	}
	printf("\n\n");

	uint8_t* decompressed_data = anr_sc_inflate(compressed_data, out, &out);

	printf("\n\n");
	for (int i = 0; i < out; i++)
	{
		printf(BYTE_TO_BINARY_PATTERN " ", BYTE_TO_BINARY(buffer[i]));
	}
	printf("\n");
	for (int i = 0; i < out; i++)
	{
		printf(BYTE_TO_BINARY_PATTERN " ", BYTE_TO_BINARY(decompressed_data[i]));
	}
	printf("\n\n");
#endif

	assert(out == (int)sizeof(buffer));
	assert(memcmp(buffer, decompressed_data, out) == 0);
}

void test_file(char* str)
{
	FILE* file = fopen(str, "rb");
	fseek(file, 0, SEEK_END);
	int iso_size = ftell(file);
	rewind(file);
	unsigned char* iso_buffer = malloc(iso_size);
	fread(iso_buffer, 1, iso_size, file);
	fclose(file);

	printf("%s:", str);
	uint32_t out;
	uint8_t* compressed_data = anr_sc_deflate(iso_buffer, iso_size, &out);
	uint8_t* decompressed_data = anr_sc_inflate(compressed_data, out, &out);

	assert(out == iso_size);
	assert(memcmp(iso_buffer, decompressed_data, out) == 0);
	
	free(compressed_data);
	free(decompressed_data);
	free(iso_buffer);
}

int main(int argc, char** argv)
{ 
	//test_small();

	test_file("res/bible.txt");
	test_file("res/cid2code.txt");
	test_file("res/small.txt");
	test_file("res/test.txt");

	#if 0
	test_file("test_data.c");
	test_file("test_pdf.c");
	test_file("test_sc.c");

	test_file("../anr_data.h");
	test_file("../anr_pdf.h");
	test_file("../anr_sc.h");
	#endif


	return 0;
}