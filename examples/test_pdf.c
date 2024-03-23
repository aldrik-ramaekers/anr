#define ANR_PDF_BUFFER_RESERVE 10000

#define ANR_PDF_IMPLEMENTATION
#include "../anr_pdf.h"

#include <math.h>

static anr_pdf_page create_page_1(anr_pdf* pdf)
{
	anr_pdf_page_begin(pdf, ANR_PDF_PAGE_SIZE_A4);

	anr_pdf_td info = anr_pdf_td_default(pdf);

	#define NEXT_LINE size.y -= 15;
	anr_pdf_vecf size = anr_pdf_page_get_size(ANR_PDF_PAGE_SIZE_A4);
	NEXT_LINE; anr_pdf_add_text(pdf, "Hello World!", 10, size.y, info);

	info.color = ANR_PDF_RGB(1.0, 0.5, 0.0);
	NEXT_LINE; anr_pdf_add_text(pdf, "Text with color", 10, size.y, info);
	info.color = ANR_PDF_RGB(0.0, 0.0, 0.0);

	info.char_space = 4.0f;
	NEXT_LINE; anr_pdf_add_text(pdf, "Text with character spacing", 10, size.y, info);
	info.char_space = 0.0f;

	info.word_space = 10.0f;
	NEXT_LINE; anr_pdf_add_text(pdf, "Text with word spacing", 10, size.y, info);
	info.word_space = 0.0f;

	info.horizontal_scale = 150.0f;
	NEXT_LINE; anr_pdf_add_text(pdf, "Text with big horizontal scale", 10, size.y, info);
	info.horizontal_scale = 0.0f;

	info.horizontal_scale = 50.0f;
	NEXT_LINE; anr_pdf_add_text(pdf, "Text with small horizontal scale", 10, size.y, info);
	info.horizontal_scale = 100.0f;

	info.leading = 10.0f;
	NEXT_LINE; anr_pdf_add_text(pdf, "Text with leading", 10, size.y, info);
	size.y -= 10;
	NEXT_LINE; 
	info.leading = 0.0f;

	char* funky_text = "Funky text that goes up and down";
	for (int i = 0; i < strlen(funky_text); i++)
	{
		char str[2];
		str[0] = funky_text[i];
		str[1] = 0;
		info.rise = sin(i)*2.0f;
		anr_pdf_add_text(pdf, str, 10 + i*8, size.y, info);
	}
	info.rise = 0.0f;

	info.render_mode = 1;
	NEXT_LINE; anr_pdf_add_text(pdf, "Text rendered in outline mode", 10, size.y, info);
	info.render_mode = 0;

	info.font_size = 24;
	NEXT_LINE;NEXT_LINE; anr_pdf_add_text(pdf, "This text is pretty big..", 10, size.y, info);
	info.font_size = 12;
	
	info.font = pdf->default_font_bold_ref;
	NEXT_LINE; anr_pdf_add_text(pdf, "This is bold text", 10, size.y, info);
	info.font = pdf->default_font_ref;

	info.font = pdf->default_font_italic_ref;
	NEXT_LINE; anr_pdf_add_text(pdf, "This is italic text", 10, size.y, info);
	info.font = pdf->default_font_ref;

	info.font = pdf->default_font_italic_bold_ref;
	NEXT_LINE; anr_pdf_obj bold_ref = anr_pdf_add_text(pdf, "This is bold italic text", 10, size.y, info);
	info.font = pdf->default_font_ref;

	anr_pdf_page_add_text_annotation(pdf, bold_ref, "This text has an annotation");
 	anr_pdf_page pageref = anr_pdf_page_end(pdf);

	anr_pdf_bookmark bm1 = anr_pdf_document_add_bookmark(pdf, pageref, NULL, NULL, "Chapter 1");
	anr_pdf_document_add_bookmark(pdf, pageref, &bold_ref, &bm1, "Chapter 1.1");


	return pageref;
}

static anr_pdf_page create_page_2(anr_pdf* pdf)
{
	anr_pdf_obj line_ref;
	anr_pdf_page_begin(pdf, ANR_PDF_PAGE_SIZE_A4);
	{
		anr_pdf_vecf size = anr_pdf_page_get_size(ANR_PDF_PAGE_SIZE_A4);
	
		{
			anr_pdf_gfx gfx = ANR_PDF_GFX_DEFAULT;
			gfx.line_cap = ANR_PDF_LINECAP_ROUNDED;
			gfx.line_width = 10;
			gfx.line_join = ANR_PDF_LINEJOIN_MITER;
			gfx.miter_limit = 1;
			gfx.color = ANR_PDF_RGB(1.0f, 0.0f, 0.0f);

			anr_pdf_vecf line_data[] = {
				{size.x - 10.0f, size.y - 10.0f},
				{size.x - 50.0f, size.y - 10.0f},
				{size.x - 70.0f, size.y - 70.0f},
				{size.x - 170.0f, size.y - 50.0f},
				{size.x - 300.0f, size.y - 20.0f},
				{size.x - 420.0f, size.y - 220.0f},
				{size.x - 380.0f, size.y - 180.0f},
				{size.x - 500.0f, size.y - 20.0f},
			};
			anr_pdf_add_polygon(pdf, line_data, 8, gfx);
		}

		{
			anr_pdf_gfx gfx = ANR_PDF_GFX_DEFAULT;
			gfx.line_cap = ANR_PDF_LINECAP_ROUNDED;
			gfx.line_width = 10;
			gfx.color = ANR_PDF_RGB(0.0f, 0.3f, 0.5f);

			anr_pdf_vecf line_data[] = {
				{10.0f, 10.0f},
				{50.0f, 10.0f},
				{70.0f, 70.0f},
				{170.0f, 50.0f},
				{300.0f, 20.0f},
				{420.0f, 220.0f},
				{380.0f, 180.0f},
				{500.0f, 20.0f},
			};
			line_ref = anr_pdf_add_polygon(pdf, line_data, 8, gfx);
		}

		{
			anr_pdf_gfx gfx = ANR_PDF_GFX_DEFAULT;
			gfx.line_cap = ANR_PDF_LINECAP_ROUNDED;
			gfx.line_width = 4;
			gfx.color = ANR_PDF_RGB(0.0f, 1.0f, 0.0f);

			anr_pdf_vecf line_data[] = {
				{100.0f, 100.0f}, {200.0f, 200.0f}, {300.0f, 100.0f},
				{400.0f,  20.0f}, {500.0f, 200.0f},
				{700.0f, 700.0f}, {300.0f, 300.0f},
			};
			anr_pdf_add_cubic_bezier(pdf, line_data, 7, gfx);
		}

		{
			anr_pdf_gfx gfx = ANR_PDF_GFX_DEFAULT;
			gfx.line_cap = ANR_PDF_LINECAP_ROUNDED;
			gfx.line_width = 10;
			gfx.color = ANR_PDF_RGB(1.0f, 1.0f, 0.0f);
			anr_pdf_add_line(pdf, (anr_pdf_vecf){350.0f, 700.0f}, (anr_pdf_vecf){350.0f, 100.0f}, gfx);
		}

		anr_pdf_add_text(pdf, "This page has some weird shapes...", 300, 500, ANR_PDF_TD_DEFAULT);
	}

	anr_pdf_page pageref = anr_pdf_page_end(pdf);

	anr_pdf_bookmark bm2 = anr_pdf_document_add_bookmark(pdf, pageref, NULL, NULL, "Chapter 2");
	anr_pdf_document_add_bookmark(pdf, pageref, &line_ref, &bm2, "Chapter 2.1");

	return pageref;
}

static anr_pdf_page create_page_3(anr_pdf* pdf)
{
	anr_pdf_page_begin(pdf, ANR_PDF_PAGE_SIZE_A4);
	anr_pdf_page pageref = anr_pdf_page_end(pdf);
	return pageref;
}


int main()
{
	anr_pdf* pdf = anr_pdf_document_begin(1000000);
	anr_pdf_document_add_information_dictionary(pdf, 
		"Simple text document", "Aldrik", "Cool Banana's", 
		"Text, Bananas", NULL, "anr_pdf Library", "20240318201500-00'00", NULL);

	create_page_1(pdf);
	create_page_2(pdf);
	create_page_3(pdf);

	anr_pdf_document_end(pdf);
	anr_pdf_write_to_file(pdf, "bin/test_pdf.pdf");

	anr_pdf_document_free(pdf);

	return 0;
}