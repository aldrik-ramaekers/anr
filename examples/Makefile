ifeq ($(OS),Windows_NT)
    EXTENSION := .exe
else
	EXTENSION :=
endif


data:
	gcc -g -Wall test_data.c -o bin/test_data$(EXTENSION)
	./bin/test_data$(EXTENSION)

pdf:
	rm bin/test_pdf.pdf || true
	gcc -g -Wall test_pdf.c -o bin/test_pdf$(EXTENSION)
	./bin/test_pdf$(EXTENSION)
	qpdf$(EXTENSION) --check bin/test_pdf.pdf