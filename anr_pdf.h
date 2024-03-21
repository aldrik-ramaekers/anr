/*
	anr_pdf.h - v0.1 - public domain pdf writer - Aldrik Ramaekers

	This is a single-header-file library for writing pdf files.

	Do this:
		#ifdef ANR_PDF_IMPLEMENTATION
	before you include this file in *one* C or C++ file to create the implementation.

	QUICK NOTES:
		Primarily of interest to developers making word processors.
		This libray does not do any layout calculations / text wrapping for you.
		Objects are displayed exactly where you put them!

	LICENSE
		See end of file for license information.
	
*/
#ifndef INCLUDE_ANR_PDF_H
#define INCLUDE_ANR_PDF_H

// 	DOCUMENTATION
//	This library follows the pdf 1.7 ISO 32000-1 standard
//	https://opensource.adobe.com/dc-acrobat-sdk-docs/pdfstandards/PDF32000_2008.pdf
//
//	Coordinates & font size are denoted in user space units. (inch / 72).
//	When positioning objects, xy (0, 0) is on the bottomleft of the page.
//
//	Dates follow ASN.1 format. see chapter 7.9.4. (YYYYMMDDHHmmSSOHH'mm)
//
//	IMPLEMENTED
//		Text (fonts/sizes/colors/spacing)
//		Bookmarks (see chapter 12.3.3)
//		Primitives (lines/cubic bezier)
//		Document properties
//
//	UNIMPLEMENTED
//		Primitives (rectangles/polygons/)
//		Text rotation
//		Tables (using primitives)
//		Images
//		Password encryption (See chapter 7.6.1)
//		Vertical text
//		UTF8 text/fonts
//		Page labels (See chapter 12.4.2)
//		Thumbnail images (See chapter 12.3.4)
//		Interactive elements: Text fields, radio buttons

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <inttypes.h>

#ifndef ANRPDF_ASSERT
#include <assert.h>
#define ANRPDF_ASSERT(x) assert(x)
#endif

#ifndef ANR_PDF_MAX_PAGES
#define ANR_PDF_MAX_PAGES 200
#endif

#ifndef ANR_PDF_MAX_BOOKMARKS
#define ANR_PDF_MAX_BOOKMARKS 200
#endif

#ifndef ANR_PDF_MAX_OBJECTS_PER_PAGE
#define ANR_PDF_MAX_OBJECTS_PER_PAGE 1000
#endif

#ifndef ANRPDFDEF
#ifdef ANR_PDF_STATIC
#define ANRPDFDEF static
#else
#define ANRPDFDEF extern
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef uint64_t anr_pdf_id;

typedef struct 
{
	float x,y;
} anr_pdf_vecf;

typedef struct 
{
	float x,y,w,h;
} anr_pdf_recf;

#define ANR_PDF_REC(__x,__y,__w,__h) (anr_pdf_recf){.x = __x, .y = __y,.w = __w, .h = __h}

typedef struct 
{
	anr_pdf_id id;
	uint64_t offset_in_body;
} anr_pdf_ref;


typedef struct
{
	float r,g,b; // colors between 0.0 and 1.0
} anr_pdf_color;

#define ANR_PDF_RGB(_r,_g,_b) (anr_pdf_color){_r, _g, _b}

// Convert inches to user space units.
#define ANR_INCH_TO_USU(_inches) _inches*72

typedef enum {
	ANR_PDF_PAGE_SIZE_LETTER,
	ANR_PDF_PAGE_SIZE_A0,
	ANR_PDF_PAGE_SIZE_A1,
	ANR_PDF_PAGE_SIZE_A2,
	ANR_PDF_PAGE_SIZE_A3,
	ANR_PDF_PAGE_SIZE_A4,
	ANR_PDF_PAGE_SIZE_A5,
	ANR_PDF_PAGE_SIZE_A6,

	ANR_PDF_PAGE_COUNT,
} anr_pdf_page_size;

typedef struct 
{
	anr_pdf_ref ref;
	anr_pdf_recf rec;
} anr_pdf_obj;

typedef struct 
{
	anr_pdf_ref ref;
	anr_pdf_page_size size;
} anr_pdf_page;

typedef struct
{
	uint64_t parent_index; // if no parent = -1
	uint32_t children_count;
	uint64_t index;
	const char* text; // needs to be valid untill end of document.
	anr_pdf_obj item_on_page;
	anr_pdf_page page;
	uint64_t prev_index; // prev item on same level. first item = -1
	uint64_t next_index; // next item on same level. last item  = -1
	uint64_t first_child_index; // if no children = -1
	uint64_t last_child_index; // if no children = -1
	uint32_t depth;
} anr_pdf_bookmark;

// Size = inches * 72
anr_pdf_vecf __anr_pdf_page_sizes[ANR_PDF_PAGE_COUNT] = 
{
	{.x = ANR_INCH_TO_USU(08.5), 	.y = ANR_INCH_TO_USU(11.0)}, // Letter
	{.x = ANR_INCH_TO_USU(33.1), 	.y = ANR_INCH_TO_USU(46.8)}, // A0
	{.x = ANR_INCH_TO_USU(23.4), 	.y = ANR_INCH_TO_USU(33.1)}, // A1
	{.x = ANR_INCH_TO_USU(16.5), 	.y = ANR_INCH_TO_USU(23.4)}, // A2

	{.x = ANR_INCH_TO_USU(11.7), 	.y = ANR_INCH_TO_USU(16.5)}, // A3
	{.x = ANR_INCH_TO_USU(8.3), 	.y = ANR_INCH_TO_USU(11.7)}, // A4
	{.x = ANR_INCH_TO_USU(5.8), 	.y = ANR_INCH_TO_USU(8.3)}, // A5
	{.x = ANR_INCH_TO_USU(4.1), 	.y = ANR_INCH_TO_USU(5.8)}, // A6
};

// See Table 105, 74.
// Parameters for text state and color.
typedef struct
{
	float char_space; // default 0
	float word_space; // default 0
	float horizontal_scale; // default 100
	float leading; // default 0
	uint16_t font_size; // default to 12
	anr_pdf_ref font; // default to document font
	int render_mode; // default 0 (see table 106)
	float rise; // default 0, can be negative
	anr_pdf_color color; // default black
} anr_pdf_td;

typedef enum
{
	ANR_PDF_LINECAP_BUTT = 0,
	ANR_PDF_LINECAP_ROUNDED = 1,
	ANR_PDF_LINECAP_SQUARE = 2,
} anr_pdf_linecap_style;

typedef enum
{
	ANR_PDF_LINEJOIN_MITER = 0,
	ANR_PDF_LINEJOIN_ROUND = 1,
	ANR_PDF_LINEJOIN_BEVEL = 2,
} anr_pdf_linejoin_style;

typedef enum
{
	ANR_PDF_RENDER_INTENT_ABSOLUTE_COLORIMETRIC
} anr_pdf_render_intent;

// See Table 52
// Parameters for all graphics.
typedef struct
{
	anr_pdf_linecap_style line_cap; // default 0
	int line_width; // default 0 = smallest possible line depending on device.
	anr_pdf_linejoin_style line_join; // default 0
	float miter_limit; // default 10, only applicable when line_join = ANR_PDF_LINEJOIN_MITER
	int dash_pattern[2]; // default = empty = solid line
	anr_pdf_color color; // default black
	// @Unimplemented: automatic stroke adjustment, op SA, see table 58
	// @Unimplemented: option to fill path
} anr_pdf_gfx;

#define ANR_PDF_TD_DEFAULT anr_pdf_td_default()
#define ANR_PDF_GFX_DEFAULT anr_pdf_gfx_default()

typedef struct
{
	// Main data buffer
	char* body_buffer;
	uint64_t body_write_cursor;
	uint32_t buf_size;
	uint64_t next_obj_id;

	// Xref data
	struct  {
		char* buffer;
		uint64_t write_cursor;
		uint32_t buf_size;
	} xref;

	// Current page data
	struct {
		char is_written;
		anr_pdf_page_size size;
		anr_pdf_ref objects[ANR_PDF_MAX_OBJECTS_PER_PAGE];
		uint64_t objects_count;
	} page;

	// list of pages
	anr_pdf_ref pages[ANR_PDF_MAX_PAGES];
	uint64_t page_count;

	// Standard objects
	anr_pdf_ref pagetree_ref;
	anr_pdf_ref catalog_ref;
	anr_pdf_ref doc_info_dic_ref; // Can be empty

	// Bookmark list
	anr_pdf_bookmark bookmarks[ANR_PDF_MAX_BOOKMARKS];
	uint64_t bookmark_count;

	// Fonts
	anr_pdf_ref default_font_ref;
	anr_pdf_ref default_font_italic_ref;
	anr_pdf_ref default_font_bold_ref;
	anr_pdf_ref default_font_italic_bold_ref;

} anr_pdf;




// === DOCUMENT OPERATIONS === 
ANRPDFDEF anr_pdf 			anr_pdf_document_begin(uint32_t buf_size);
ANRPDFDEF void 				anr_pdf_document_end(anr_pdf* pdf);
ANRPDFDEF void 				anr_pdf_write_to_file(anr_pdf* pdf, const char* path);
// item_on_page optional, parent optional.
ANRPDFDEF anr_pdf_bookmark 	anr_pdf_document_add_bookmark(anr_pdf* pdf, anr_pdf_page page, anr_pdf_obj* item_on_page, 
								anr_pdf_bookmark* parent, const char* text); 
// Add document information to the pdf. See chapter 14.3.3
// Dates follow ASN.1 format. see chapter 7.9.4. (YYYYMMDDHHmmSSOHH'mm)
ANRPDFDEF void 				anr_pdf_document_add_information_dictionary(anr_pdf* pdf, char* title, 
								char* author, char* subject, char* keywords, char* creator, 
								char* producer, char* creation_date, char* mod_date);

// === PAGE OPERATIONS ===
ANRPDFDEF void 			anr_pdf_page_begin(anr_pdf* pdf, anr_pdf_page_size size);
ANRPDFDEF anr_pdf_page 	anr_pdf_page_end(anr_pdf* pdf);
ANRPDFDEF anr_pdf_vecf	anr_pdf_page_get_size(anr_pdf_page_size size); // Returns the size of the page in user space units. (inches * 72)

// === OBJECT OPERATIONS === 
ANRPDFDEF anr_pdf_obj 	anr_pdf_add_text(anr_pdf* pdf, const char* text, float x, float y, anr_pdf_td info);
ANRPDFDEF anr_pdf_obj 	anr_pdf_add_line(anr_pdf* pdf, anr_pdf_vecf* data, uint32_t data_length, anr_pdf_gfx gfx);
ANRPDFDEF anr_pdf_obj 	anr_pdf_add_cubic_bezier(anr_pdf* pdf, anr_pdf_vecf* data, uint32_t data_length, anr_pdf_gfx gfx);

// === DEFAULT CONFIGS === 
ANRPDFDEF anr_pdf_td	anr_pdf_td_default();
ANRPDFDEF anr_pdf_gfx	anr_pdf_gfx_default();

////   end header file   /////////////////////////////////////////////////////
#endif // End of INCLUDE_ANR_PDF_H

#ifdef ANR_PDF_IMPLEMENTATION

static FILE* anr__pdf_fopen(const char* filename, const char* mode)
{
	FILE* f;	
	f = fopen(filename, mode);
	return f;
}

static void anr__pdf_fclose(FILE* file)
{
	fflush(file);
	fclose(file);
}

static anr_pdf_obj anr__pdf_emptyobj()
{
	return (anr_pdf_obj){0};
}

static anr_pdf_ref anr__pdf_emptyref()
{
	return (anr_pdf_ref){0};
}

static char anr__pdf_ref_valid(anr_pdf_ref ref)
{
	return ref.id > 0;
}

static uint64_t anr__pdf_append_printf(anr_pdf* pdf, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    char formatted_str[300];
	vsnprintf(formatted_str, 300, fmt, ap);

	uint64_t length = strlen(formatted_str);
	memcpy(pdf->body_buffer + pdf->body_write_cursor, formatted_str, (uint64_t)length);
	uint64_t result = pdf->body_write_cursor;
	pdf->body_write_cursor += length;

    va_end(ap);
    return result;
}

static uint64_t anr__pdf_append_str_idref(anr_pdf* pdf, const char* bytes, anr_pdf_ref ref)
{
	char formatted_str[300];
	sprintf(formatted_str, bytes, ref.id);

	uint64_t length = strlen(formatted_str);
	memcpy(pdf->body_buffer + pdf->body_write_cursor, formatted_str, (uint64_t)length);
	uint64_t result = pdf->body_write_cursor;
	pdf->body_write_cursor += length;
	return result;
}

static uint64_t anr__pdf_append_str(anr_pdf* pdf, const char* bytes)
{
	uint64_t length = strlen(bytes);
	memcpy(pdf->body_buffer + pdf->body_write_cursor, bytes, (uint64_t)length);
	uint64_t result = pdf->body_write_cursor;
	pdf->body_write_cursor += length;
	return result;
}

static uint64_t anr__pdf_append_bytes(anr_pdf* pdf, const char* bytes, uint64_t size)
{
	memcpy(pdf->body_buffer + pdf->body_write_cursor, bytes, size);
	uint64_t result = pdf->body_write_cursor;
	pdf->body_write_cursor += size;
	return result;
}

static anr_pdf_id anr__peak_next_id(anr_pdf* pdf)
{
	return pdf->next_obj_id;
}

static anr_pdf_ref anr__pdf_begin_obj(anr_pdf* pdf)
{
	anr_pdf_id id = pdf->next_obj_id++;
	char xref_entry[20];
	memcpy(xref_entry, "0000000000 00000 n \n", 20);

	char offset_str[11];
	sprintf(offset_str, "%" PRId64 , pdf->body_write_cursor);
	memcpy(xref_entry + 10 - strlen(offset_str), offset_str, strlen(offset_str));

	memcpy(pdf->xref.buffer + pdf->xref.write_cursor, xref_entry, 20);
	pdf->xref.write_cursor += 20;

	char newobj_entry[30];
	sprintf(newobj_entry, "\n%" PRId64 " 0 obj", id);
	anr__pdf_append_str(pdf, newobj_entry);

	return (anr_pdf_ref){.id = id, .offset_in_body = pdf->body_write_cursor};
}

static anr_pdf_obj anr__pdf_begin_content_obj(anr_pdf* pdf, anr_pdf_recf rec)
{
	ANRPDF_ASSERT(!pdf->page.is_written);
	ANRPDF_ASSERT(pdf->page.objects_count < ANR_PDF_MAX_OBJECTS_PER_PAGE);
	anr_pdf_ref ref = anr__pdf_begin_obj(pdf);
	anr_pdf_obj objref = {.ref = ref, .rec = rec};
	pdf->page.objects[pdf->page.objects_count++] = ref;
	return objref;
}

static void anr__append_xref_table(anr_pdf* pdf) 
{
	// Cross reference table
	uint64_t refxref = anr__pdf_append_str(pdf, "\nxref");
	anr__pdf_append_printf(pdf, "\n0 %d", pdf->next_obj_id);
	anr__pdf_append_str(pdf, "\n0000000000 65535 f \n");
	anr__pdf_append_bytes(pdf, pdf->xref.buffer, pdf->xref.write_cursor);

	// Trailer
	anr__pdf_append_str(pdf, "trailer\n<<");
	anr__pdf_append_printf(pdf,  "/Size %d\n", pdf->next_obj_id);
	anr__pdf_append_str_idref(pdf, "/Root %d 0 R", pdf->catalog_ref);
	if (anr__pdf_ref_valid(pdf->doc_info_dic_ref)) {
		anr__pdf_append_str_idref(pdf, "/Info %d 0 R", pdf->doc_info_dic_ref);
	}
	anr__pdf_append_str(pdf, ">>\nstartxref");
	anr__pdf_append_printf(pdf, "\n%d", refxref);
}

static anr_pdf_ref anr__append_outlines(anr_pdf* pdf)
{
	anr_pdf_id id_offset = anr__peak_next_id(pdf); // ids for bookmarks will be index+id_offset

	anr_pdf_ref first_bookmark = anr__pdf_emptyref();
	anr_pdf_ref last_bookmark = anr__pdf_emptyref();
	for (int i = 0; i < pdf->bookmark_count; i++)
	{
		anr_pdf_bookmark current_bookmark = pdf->bookmarks[i];

		anr_pdf_ref ref = anr__pdf_begin_obj(pdf);
		if (i == 0) first_bookmark = ref;
		last_bookmark = ref;
		anr__pdf_append_printf(pdf, "\n<<\n/Title (%s)", current_bookmark.text);
		if (current_bookmark.parent_index != -1)
			anr__pdf_append_printf(pdf, "\n/Parent %d 0 R", id_offset + current_bookmark.parent_index);
		if (current_bookmark.prev_index != -1)
			anr__pdf_append_printf(pdf, "\n/Prev %d 0 R", id_offset + current_bookmark.prev_index);
		if (current_bookmark.next_index != -1)
			anr__pdf_append_printf(pdf, "\n/Next %d 0 R", id_offset + current_bookmark.next_index);
		if (current_bookmark.first_child_index != -1)
			anr__pdf_append_printf(pdf, "\n/First %d 0 R", id_offset + current_bookmark.first_child_index);
		if (current_bookmark.last_child_index != -1)
			anr__pdf_append_printf(pdf, "\n/Last %d 0 R", id_offset + current_bookmark.last_child_index);
		if (current_bookmark.children_count != -1)
			anr__pdf_append_printf(pdf, "\n/Count %d", current_bookmark.children_count);

		float offset_bottom = anr_pdf_page_get_size(current_bookmark.page.size).y + 2;
		if (anr__pdf_ref_valid(current_bookmark.item_on_page.ref)) {
			offset_bottom = current_bookmark.item_on_page.rec.y;
		}
		anr__pdf_append_printf(pdf, "\n/Dest [%d 0 R /XYZ 0 %.2f 0]", current_bookmark.page.ref.id, offset_bottom);

		// @Unimplemented "C" color key (see Table 153)
		// @Unimplemented "F" font flag (see Table 153)

		anr__pdf_append_str(pdf, "\n>>");
		anr__pdf_append_str(pdf, "\nendobj");
	}
	
	anr_pdf_ref outlineref = anr__pdf_begin_obj(pdf);
	anr__pdf_append_str(pdf, "\n<</Type /Outlines");
	anr__pdf_append_printf(pdf, "\n/Count %d", pdf->bookmark_count);
	if (anr__pdf_ref_valid(first_bookmark)) {
		anr__pdf_append_str_idref(pdf, "\n/First %d 0 R", first_bookmark);
		anr__pdf_append_str_idref(pdf, "\n/Last %d 0 R", last_bookmark);
	}
	anr__pdf_append_str(pdf, ">>");
	anr__pdf_append_str(pdf, "\nendobj");

	return outlineref;
}

// Document catalog, see 7.7.2
static void anr__append_document_catalog(anr_pdf* pdf) 
{
	// Outlines
	anr_pdf_ref outlineref = anr__append_outlines(pdf);

	// Page tree
	anr_pdf_ref treeref = anr__pdf_begin_obj(pdf);
	anr__pdf_append_str(pdf, "\n<</Type /Pages");

	anr__pdf_append_str(pdf, "\n/Kids [");
	for (uint64_t i = 0; i < pdf->page_count; i++)
		anr__pdf_append_str_idref(pdf, "%d 0 R\n", pdf->pages[i]);
	anr__pdf_append_str(pdf, "]");

	anr__pdf_append_printf(pdf, "\n/Count %d", pdf->page_count);
	anr__pdf_append_str(pdf, ">>");
	anr__pdf_append_str(pdf, "\nendobj");

	pdf->pagetree_ref = treeref;

	// Catalog
	anr_pdf_ref docref = anr__pdf_begin_obj(pdf);
	anr__pdf_append_str(pdf, "\n<</Type /Catalog");
	anr__pdf_append_str_idref(pdf, "\n/Outlines %d 0 R", outlineref);
	// @Unimplemented: pagemode (see Table 28)
	anr__pdf_append_str_idref(pdf, "\n/Pages %d 0 R", treeref);
	anr__pdf_append_str(pdf, ">>");
	anr__pdf_append_str(pdf, "\nendobj");

	pdf->catalog_ref = docref;
}

static void anr__create_default_font(anr_pdf* pdf) {
	#define LOAD_FONT(_name, _container) { \
		anr_pdf_ref fontref = anr__pdf_begin_obj(pdf); \
		anr__pdf_append_str(pdf, "\n<< /Type /Font"); \
		anr__pdf_append_str(pdf, "\n/Subtype /Type1"); \
		anr__pdf_append_str_idref(pdf, "\n/Name /F%d", fontref); \
		anr__pdf_append_str(pdf, "\n/BaseFont /"_name); \
		anr__pdf_append_str(pdf, "\n/Encoding /MacRomanEncoding"); \
		anr__pdf_append_str(pdf, ">>"); \
		anr__pdf_append_str(pdf, "\nendobj"); \
		_container = fontref; \
	}

	LOAD_FONT("Times-Roman", pdf->default_font_ref);
	LOAD_FONT("Times-Italic", pdf->default_font_italic_ref);
	LOAD_FONT("Times-Bold", pdf->default_font_bold_ref);
	LOAD_FONT("Times-BoldItalic", pdf->default_font_italic_bold_ref);
}

anr_pdf anr_pdf_document_begin(uint32_t buf_size)
{
	anr_pdf pdf = {0};
	pdf.body_buffer = malloc(buf_size);
	pdf.buf_size = buf_size;
	pdf.body_write_cursor = 0;
	pdf.next_obj_id = 1;
	pdf.doc_info_dic_ref = anr__pdf_emptyref();

	pdf.xref.buffer = malloc(buf_size);
	pdf.xref.write_cursor = 0;
	pdf.xref.buf_size = buf_size;

	pdf.page.is_written = 1;

	anr__pdf_append_str(&pdf, "%PDF-1.7");
	anr__create_default_font(&pdf);

	return pdf;
}

void anr_pdf_document_end(anr_pdf* pdf)
{
	anr__append_document_catalog(pdf);
	anr__append_xref_table(pdf);
	anr__pdf_append_str(pdf, "\n%%EOF\n");
}

void anr_pdf_write_to_file(anr_pdf* pdf, const char* path)
{
	FILE* file = anr__pdf_fopen(path, "wb");
	fwrite(pdf->body_buffer, 1, pdf->body_write_cursor, file);
	anr__pdf_fclose(file);
}

void anr_pdf_page_begin(anr_pdf* pdf, anr_pdf_page_size size)
{
	ANRPDF_ASSERT(pdf->page.is_written);
	ANRPDF_ASSERT(pdf->page_count < ANR_PDF_MAX_PAGES);
	pdf->page.is_written = 0;
	pdf->page.objects_count = 0;
	pdf->page.size = size;
}

anr_pdf_page anr_pdf_page_end(anr_pdf* pdf)
{
	ANRPDF_ASSERT(!pdf->page.is_written);

	anr_pdf_ref procsetref = anr__pdf_begin_obj(pdf);
	anr__pdf_append_str(pdf, "\n[/PDF /Text /ImageB /ImageC /ImageI]");
	anr__pdf_append_str(pdf, "\nendobj");

	anr_pdf_ref pageref = anr__pdf_begin_obj(pdf);
	anr__pdf_append_str(pdf, "\n<</Type /Page");
	anr__pdf_append_str_idref(pdf, "\n/Resources <</ProcSet %d 0 R", procsetref);

	// Import all default fonts in page.
	anr__pdf_append_str(pdf, "\n/Font <<");
	anr__pdf_append_printf(pdf, "\n/F%d %d 0 R", pdf->default_font_ref.id, pdf->default_font_ref.id);
	anr__pdf_append_printf(pdf, "\n/F%d %d 0 R", pdf->default_font_italic_ref.id, pdf->default_font_italic_ref.id);
	anr__pdf_append_printf(pdf, "\n/F%d %d 0 R", pdf->default_font_bold_ref.id, pdf->default_font_bold_ref.id);
	anr__pdf_append_printf(pdf, "\n/F%d %d 0 R", pdf->default_font_italic_bold_ref.id, pdf->default_font_italic_bold_ref.id);
	anr__pdf_append_str(pdf, "\n>>>>");

	//anr__pdf_append_str(pdf, "\n/Parent 0 0 R"); // this field is required according to spec. Yikes! @Unimplemented
	
	anr_pdf_vecf page_size = anr_pdf_page_get_size(pdf->page.size);
	anr__pdf_append_printf(pdf, "\n/MediaBox [0 0 %.3f %.3f]", page_size.x, page_size.y);

	anr__pdf_append_str(pdf, "\n/Contents [\n");
	for (uint64_t i = 0; i < pdf->page.objects_count; i++)
		anr__pdf_append_str_idref(pdf, "%d 0 R\n", pdf->page.objects[i]);
	anr__pdf_append_str(pdf, "]>>");
	anr__pdf_append_str(pdf, "\nendobj");

	pdf->pages[pdf->page_count++] = pageref;
	pdf->page.is_written = 1;

	return (anr_pdf_page){.ref = pageref, .size = pdf->page.size};
}

anr_pdf_td anr_pdf_td_default()
{
	return (anr_pdf_td){0.0f,0.0f,100.0f,0.0f,12,anr__pdf_emptyref(),0, 0.0f, (anr_pdf_color){0.0f,0.0f,0.0f}};
}

anr_pdf_gfx anr_pdf_gfx_default()
{
	return (anr_pdf_gfx){.line_cap = ANR_PDF_LINECAP_BUTT, .line_width = 0, .line_join = ANR_PDF_LINEJOIN_MITER, 
		.miter_limit = 10, .dash_pattern = {0}, .color = ANR_PDF_RGB(0.0f,0.0f,0.0f)};
}

anr_pdf_obj anr_pdf_add_cubic_bezier(anr_pdf* pdf, anr_pdf_vecf* data, uint32_t data_length, anr_pdf_gfx gfx) 
{
	ANRPDF_ASSERT(data_length >= 3);
	ANRPDF_ASSERT((data_length - 3) % 2 == 0);

	// Calculate bounds
	anr_pdf_recf rec = {0};
	for (uint32_t i = 0; i < data_length; i++)
	{
		anr_pdf_vecf point = data[i];
		if (point.x < rec.x) rec.x = point.x;
		if (point.y > rec.y) rec.y = point.y;
		if (point.x > rec.w) rec.w = point.x;
		if (point.y < rec.h) rec.h = point.y;
	}
	rec.w = rec.w - rec.x;
	rec.h = rec.y - rec.h;

	uint64_t stream_length = 0;
	anr_pdf_obj obj_ref = anr__pdf_begin_content_obj(pdf, rec);
	anr_pdf_id streamlen_id = anr__peak_next_id(pdf); // next object is stream length
	anr__pdf_append_str_idref(pdf, "\n<< /Length %d 0 R >>", (anr_pdf_ref){.id=streamlen_id});
	anr__pdf_append_str(pdf, "\nstream");

	uint64_t write_start = anr__pdf_append_printf(pdf, "\n%d J", gfx.line_cap);
	anr__pdf_append_printf(pdf, "\n%d w", gfx.line_width);
	anr__pdf_append_printf(pdf, "\n%d j", gfx.line_join);
	anr__pdf_append_printf(pdf, "\n%.3f M", gfx.miter_limit);
	if (gfx.dash_pattern[0] > 0 && gfx.dash_pattern[1] > 0) {
		anr__pdf_append_printf(pdf, "\n[%d %d] 0 d", gfx.dash_pattern[0], gfx.dash_pattern[1]);
	}
	anr__pdf_append_printf(pdf, "\n%.1f %.1f %.1f RG", gfx.color.r, gfx.color.g, gfx.color.b);

	anr__pdf_append_printf(pdf, "\n%.3f %.3f m", data[0].x, data[0].y);
	anr__pdf_append_printf(pdf, "\n%.3f %.3f %.3f %.3f %.3f %.3f c", 
									data[0].x, data[0].y, data[1].x, data[1].y, data[2].x, data[2].y);

	for (uint32_t i = 3; i < data_length; i+=2)
	{
		anr_pdf_vecf point1 = data[i];
		anr_pdf_vecf point2 = data[i+1];
		anr__pdf_append_printf(pdf, "\n%.3f %.3f %.3f %.3f v", point1.x, point1.y, point2.x, point2.y);
	}

	anr__pdf_append_printf(pdf, "\nS");
	anr__pdf_append_printf(pdf, "\nn");

	// draw line here

	uint64_t write_end = anr__pdf_append_str(pdf, "\nendstream");
	anr__pdf_append_str(pdf, "\nendobj");

	stream_length = write_end - write_start;

	// Object containing stream length.
	anr__pdf_begin_obj(pdf);
	anr__pdf_append_printf(pdf, "\n%d", stream_length);
	anr__pdf_append_str(pdf, "\nendobj");

	return obj_ref;
}

anr_pdf_obj anr_pdf_add_line(anr_pdf* pdf, anr_pdf_vecf* data, uint32_t data_length, anr_pdf_gfx gfx) 
{
	ANRPDF_ASSERT(data_length > 0);

	// Calculate bounds
	anr_pdf_recf rec = {0};
	for (uint32_t i = 0; i < data_length; i++)
	{
		anr_pdf_vecf point = data[i];
		if (point.x < rec.x) rec.x = point.x;
		if (point.y > rec.y) rec.y = point.y;
		if (point.x > rec.w) rec.w = point.x;
		if (point.y < rec.h) rec.h = point.y;
	}
	rec.w = rec.w - rec.x;
	rec.h = rec.y - rec.h;

	uint64_t stream_length = 0;
	anr_pdf_obj obj_ref = anr__pdf_begin_content_obj(pdf, rec);
	anr_pdf_id streamlen_id = anr__peak_next_id(pdf); // next object is stream length
	anr__pdf_append_str_idref(pdf, "\n<< /Length %d 0 R >>", (anr_pdf_ref){.id=streamlen_id});
	anr__pdf_append_str(pdf, "\nstream");

	uint64_t write_start = anr__pdf_append_printf(pdf, "\n%.3f %.3f m", data[0].x, data[0].y);
	anr__pdf_append_printf(pdf, "\n%d J", gfx.line_cap);
	anr__pdf_append_printf(pdf, "\n%d w", gfx.line_width);
	anr__pdf_append_printf(pdf, "\n%d j", gfx.line_join);
	anr__pdf_append_printf(pdf, "\n%.3f M", gfx.miter_limit);
	if (gfx.dash_pattern[0] > 0 && gfx.dash_pattern[1] > 0) {
		anr__pdf_append_printf(pdf, "\n[%d %d] 0 d", gfx.dash_pattern[0], gfx.dash_pattern[1]);
	}
	anr__pdf_append_printf(pdf, "\n%.1f %.1f %.1f RG", gfx.color.r, gfx.color.g, gfx.color.b);

	for (uint32_t i = 1; i < data_length; i++)
	{
		anr_pdf_vecf point = data[i];
		anr__pdf_append_printf(pdf, "\n%.3f %.3f l", point.x, point.y);
	}

	anr__pdf_append_printf(pdf, "\nS");
	anr__pdf_append_printf(pdf, "\nn");

	// draw line here

	uint64_t write_end = anr__pdf_append_str(pdf, "\nendstream");
	anr__pdf_append_str(pdf, "\nendobj");

	stream_length = write_end - write_start;

	// Object containing stream length.
	anr__pdf_begin_obj(pdf);
	anr__pdf_append_printf(pdf, "\n%d", stream_length);
	anr__pdf_append_str(pdf, "\nendobj");

	return obj_ref;
}

anr_pdf_obj anr_pdf_add_text(anr_pdf* pdf, const char* text, float x, float y, anr_pdf_td info) 
{
	uint64_t stream_length = 0;
	anr_pdf_obj obj_ref = anr__pdf_begin_content_obj(pdf, ANR_PDF_REC(x, y + info.font_size, 0.0f, (float)info.font_size));
	anr_pdf_id streamlen_id = anr__peak_next_id(pdf); // next object is stream length
	anr__pdf_append_str_idref(pdf, "\n<< /Length %d 0 R >>", (anr_pdf_ref){.id=streamlen_id});
	anr__pdf_append_str(pdf, "\nstream");

	uint64_t write_start = anr__pdf_append_str(pdf, "\nBT");

	// Use default regular font if no font given.
	if (!anr__pdf_ref_valid(info.font)) {
		info.font = pdf->default_font_ref;
	}

	anr__pdf_append_printf(pdf, "\n/F%d %d Tf", info.font.id, info.font_size);
	anr__pdf_append_printf(pdf, "\n%.1f %.1f %.1f rg", info.color.r, info.color.g, info.color.b);
	anr__pdf_append_printf(pdf, "\n%.2f Tc", info.char_space);
	anr__pdf_append_printf(pdf, "\n%.2f Tw", info.word_space);
	anr__pdf_append_printf(pdf, "\n%.2f Tz", info.horizontal_scale);
	anr__pdf_append_printf(pdf, "\n%.2f TL", info.leading);
	anr__pdf_append_printf(pdf, "\n%.2f Ts", info.rise);
	anr__pdf_append_printf(pdf, "\n%d Tr", info.render_mode);
	anr__pdf_append_printf(pdf, "\n%.3f %.3f Td", x, y);
	
	anr__pdf_append_str(pdf, "\nT* (");
	anr__pdf_append_str(pdf, text);
	anr__pdf_append_str(pdf, "\n) Tj");
	anr__pdf_append_str(pdf, "\nET");

	uint64_t write_end = anr__pdf_append_str(pdf, "\nendstream");
	anr__pdf_append_str(pdf, "\nendobj");

	stream_length = write_end - write_start;

	// Object containing stream length.
	anr__pdf_begin_obj(pdf);
	anr__pdf_append_printf(pdf, "\n%d", stream_length);
	anr__pdf_append_str(pdf, "\nendobj");

	return obj_ref;
}

 void anr_pdf_document_add_information_dictionary(anr_pdf* pdf, char* title, 
						char* author, char* subject, char* keywords, char* creator, 
						char* producer, char* creation_date, char* mod_date)
{
	// @Unimplemented parameter: Trapped (see Table 317)

	#define ADD_ENTRY(__entry, __tag) \
		if (__entry) {\
			anr__pdf_append_str(pdf, "/"__tag" ("); \
			anr__pdf_append_str(pdf, __entry); \
			anr__pdf_append_str(pdf, ")\n"); }

	pdf->doc_info_dic_ref = anr__pdf_begin_obj(pdf);
	anr__pdf_append_str(pdf, "<<");
	ADD_ENTRY(title, "Title");
	ADD_ENTRY(author, "Author");
	ADD_ENTRY(subject, "Subject");
	ADD_ENTRY(keywords, "Keywords");
	ADD_ENTRY(creator, "Creator");
	ADD_ENTRY(producer, "Producer");
	ADD_ENTRY(creation_date, "CreationDate");
	ADD_ENTRY(mod_date, "ModDate");

	anr__pdf_append_str(pdf, ">>");
	anr__pdf_append_str(pdf, "\nendobj");
}

anr_pdf_vecf anr_pdf_page_get_size(anr_pdf_page_size size) {
	return __anr_pdf_page_sizes[size];
}

anr_pdf_bookmark anr_pdf_document_add_bookmark(anr_pdf* pdf, anr_pdf_page page, anr_pdf_obj* item_on_page, 
								anr_pdf_bookmark* parent, const char* text)
{
	ANRPDF_ASSERT(pdf->bookmark_count < ANR_PDF_MAX_BOOKMARKS);

	uint64_t new_index = pdf->bookmark_count;
	uint32_t depth = 0;

	uint64_t prev_last_child = -1;

	// update parent refs.
	if (parent) {
		if (pdf->bookmarks[parent->index].children_count == 0) {
			pdf->bookmarks[parent->index].first_child_index = new_index;
			pdf->bookmarks[parent->index].last_child_index = new_index;
		}
		else {
			prev_last_child = pdf->bookmarks[parent->index].last_child_index;
			pdf->bookmarks[prev_last_child].next_index = new_index;
			pdf->bookmarks[parent->index].last_child_index = new_index;
		}
		pdf->bookmarks[parent->index].children_count++;

		depth = parent->depth+1;
	}
	else 
	{
		// update prev and next refs for topmost items.
		for (int i = pdf->bookmark_count-1; i >= 0; i--)
		{
			anr_pdf_bookmark existing_bookmark = pdf->bookmarks[i];
			if (existing_bookmark.depth != depth) continue;

			prev_last_child = pdf->bookmarks[i].index;
			pdf->bookmarks[i].next_index = new_index;
			break;
		}
	}


	anr_pdf_bookmark bookmark = {
		.item_on_page = item_on_page == NULL ? anr__pdf_emptyobj() : *item_on_page, 
		.index = new_index, 
		.text = text, 
		.parent_index = parent == NULL ? -1 : parent->index, 
		.first_child_index = -1,
		.last_child_index = -1,
		.prev_index = prev_last_child,
		.next_index = -1,
		.children_count = 0,
		.depth = depth,
		.page = page,
	};
	pdf->bookmarks[pdf->bookmark_count++] = bookmark;

	return bookmark;
}

#ifdef __cplusplus
}
#endif
#endif

/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2024 Aldrik Ramaekers
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/