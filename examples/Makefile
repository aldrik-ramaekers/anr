ifeq ($(OS),Windows_NT)
    EXTENSION := .exe
else
	EXTENSION :=
endif


data:
	gcc -g -Wall test_data.c -o bin/test_data$(EXTENSION)
	./bin/test_data$(EXTENSION)