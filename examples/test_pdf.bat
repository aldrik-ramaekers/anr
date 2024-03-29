IF EXIST "bin\test_pdf.pdf" (
    del "bin\test_pdf.pdf"
)

gcc -g -Wall test_pdf.c -o bin/test_pdf.exe

@"bin/test_pdf.exe"

qpdf.exe --check bin/test_pdf.pdf