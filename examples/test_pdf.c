#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define ANR_PDF_BUFFER_RESERVE 100000000

#define ANR_PDF_IMPLEMENTATION
#include "../anr_pdf.h"

#include <math.h>

anr_pdf_obj bold_text_link;
anr_pdf_ref comic_sans;

static anr_pdf_page create_page_1(anr_pdf* pdf)
{
	anr_pdf_page_begin(pdf, ANR_PDF_PAGE_SIZE_A4);

	anr_pdf_txt_conf info = anr_pdf_txt_conf_default(pdf);
	info.font = comic_sans;

	#define NEXT_LINE size.y -= 15;
	anr_pdf_vecf size = anr_pdf_page_get_size(ANR_PDF_PAGE_SIZE_A4);
	NEXT_LINE; anr_pdf_add_text(pdf, "Hello world!", 10, size.y, info);

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
	info.angle = M_PI/-4.0f;
	NEXT_LINE;NEXT_LINE; anr_pdf_add_text(pdf, "This text is pretty big..", 300, size.y, info);
	info.angle = 0.0f;
	info.font_size = 12;
	
	info.font = pdf->default_font_bold_ref;
	NEXT_LINE; anr_pdf_add_text(pdf, "This is bold text", 10, size.y, info);
	info.font = pdf->default_font_ref;

	info.font = pdf->default_font_italic_ref;
	NEXT_LINE; anr_pdf_add_text(pdf, "This is italic text", 10, size.y, info);
	info.font = pdf->default_font_ref;

	info.font = pdf->default_font_italic_bold_ref;
	NEXT_LINE; bold_text_link = anr_pdf_add_text(pdf, "This is a link in static bold text", 10, size.y, info);
	info.font = pdf->default_font_ref;

 	anr_pdf_page pageref = anr_pdf_page_end(pdf);

	anr_pdf_bookmark bm1 = anr_pdf_document_add_bookmark(pdf, pageref, NULL, NULL, "Chapter 1");
	anr_pdf_document_add_bookmark(pdf, pageref, &bold_text_link, &bm1, "Chapter 1.1");

	anr_pdf_annot_cnf annot = ANR_PDF_ANNOT_CONF_DEFAULT;
	annot.color = ANR_PDF_RGB(1.0f, 0.0f, 0.0f);
	annot.posted_by = "John";
	anr_pdf_add_annotation_text(pdf, pageref, bold_text_link, "This text has an annotation", annot);

	return pageref;
}

static anr_pdf_page create_page_2(anr_pdf* pdf)
{
	anr_pdf_obj line_ref;
	anr_pdf_obj text_ref;
	anr_pdf_page_begin(pdf, ANR_PDF_PAGE_SIZE_A4);
	{
		anr_pdf_vecf size = anr_pdf_page_get_size(ANR_PDF_PAGE_SIZE_A4);
	
		{
			anr_pdf_gfx_conf gfx = ANR_PDF_GFX_CONF_DEFAULT;
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
			anr_pdf_gfx_conf gfx = ANR_PDF_GFX_CONF_DEFAULT;
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
			anr_pdf_gfx_conf gfx = ANR_PDF_GFX_CONF_DEFAULT;
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
			anr_pdf_gfx_conf gfx = ANR_PDF_GFX_CONF_DEFAULT;
			gfx.line_cap = ANR_PDF_LINECAP_ROUNDED;
			gfx.line_width = 10;
			gfx.color = ANR_PDF_RGB(1.0f, 1.0f, 0.0f);
			anr_pdf_add_line(pdf, (anr_pdf_vecf){350.0f, 700.0f}, (anr_pdf_vecf){350.0f, 100.0f}, gfx);
		}

		text_ref = anr_pdf_add_text(pdf, "This page has some weird shapes...", 300, 500, ANR_PDF_TXT_CONF_DEFAULT);

		{
			int w, h, bbs;
			unsigned char *data = stbi_load("greenland_grid_velo.bmp", &w, &h, &bbs, 3);	
			printf("Bmp greenland: %d %d %d\n", w, h, bbs);

			anr_pdf_img img = anr_pdf_embed_image(pdf, data, w*h*bbs, w, h, 8);
			anr_pdf_add_image(pdf, img, 0, 200, size.x/4, size.y/4);
		}

		{
			int w, h, bbs;
			unsigned char *data = stbi_load("spongebob.png", &w, &h, &bbs,3);
			printf("Bmp greenland: %d %d %d\n", w, h, bbs);

			anr_pdf_img img = anr_pdf_embed_image(pdf, data, w*h*3, w, h, 8);
			anr_pdf_add_image(pdf, img, 400, 200, size.x/4, size.y/4);
		}
	}

	anr_pdf_page pageref = anr_pdf_page_end(pdf);

	anr_pdf_annot_cnf annot = ANR_PDF_ANNOT_CONF_DEFAULT;
	annot.post_date = "20240323201500-00'00";
	annot.posted_by = "Aldrik";
	anr_pdf_annot root = anr_pdf_add_annotation_markup(pdf, pageref, text_ref, "This text is highlighted", ANR_PDF_ANNOTATION_MARKUP_HIGHLIGHT, annot);

	annot = ANR_PDF_ANNOT_CONF_DEFAULT;
	annot.posted_by = "Joe";
	annot.parent = root;
	anr_pdf_add_annotation_text(pdf, pageref, text_ref, "But I dont like the color..", annot);

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
	anr_pdf* pdf = anr_pdf_document_begin();
	anr_pdf_document_add_information_dictionary(pdf, 
		"Simple text document", "Aldrik", "Cool Banana's", 
		"Text, Bananas", NULL, "anr_pdf Library", "20240318201500-00'00", NULL);

	FILE* file = fopen("comic-sans.ttf", "rw");
	fseek(file, 0, SEEK_END);
	size_t ttf_size = ftell(file);
	printf("Comic sans: %d\n", (int)ttf_size);
	rewind(file);
	unsigned char* ttf_buffer = malloc(ttf_size);
	fread(ttf_buffer, 1, ttf_size, file);
	comic_sans = anr_pdf_embed_font(pdf, ttf_buffer, ttf_size);

	anr_pdf_page page1 = create_page_1(pdf);
	anr_pdf_page page2 = create_page_2(pdf);
	anr_pdf_page page3 = create_page_3(pdf);

	anr_pdf_add_annotation_link(pdf, page1, bold_text_link, page2, NULL, ANR_PDF_ANNOT_CONF_DEFAULT);

	anr_pdf_document_end(pdf);
	anr_pdf_write_to_file(pdf, "bin/test_pdf.pdf");

	anr_pdf_document_free(pdf);

	return 0;
}