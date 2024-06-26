/*
anr_pdf.h - v0.1 - public domain pdf writer

This is a single-header-file library for writing pdf files.

Do this:
	#ifdef ANR_PDF_IMPLEMENTATION
before you include this file in *one* C file to create the implementation.

QUICK NOTES:
	Primarily of interest to developers making word processors.
	This libray does not do any layout calculations / text wrapping for you.

LICENSE
	See end of file for license information.
	
*/
#ifndef INCLUDE_ANR_PDF_H
#define INCLUDE_ANR_PDF_H

// 	DOCUMENTATION
//	This library follows the pdf 1.7 ISO 32000-1 standard
//	https://opensource.adobe.com/dc-acrobat-sdk-docs/pdfstandards/PDF32000_2008.pdf
//
//	Coordinates & size are denoted in user space units. (inch / 72).
//	When positioning objects, xy (0, 0) is on the bottomleft of the page.
//
//	Dates follow ASN.1 format. see chapter 7.9.4. (YYYYMMDDHHmmSSOHH'mm)
//
//	ASCII text only.
//
//	IMPLEMENTED
//		Text (fonts/sizes/colors/spacing/rotation)
//		Primitives (lines/polygons/cubic beziers/rectangles)
//		Annotations (text/link/markup) + annotation threads
//		Page & Document labeling
//		Encoding (ASCIIHex)
//		Images (rgb)
//		TTF embedding
//		Bookmarks
//
//	UNIMPLEMENTED
//		Password encryption (See chapter 7.6.1)
//		Links

#include <inttypes.h>

#ifndef ANRPDF_ASSERT
#include <assert.h>
#define ANRPDF_ASSERT(x) assert(x)
#endif

#ifndef ANR_PDF_BUFFER_RESERVE
#define ANR_PDF_BUFFER_RESERVE 1000000
#endif

#ifndef ANR_PDF_MAX_PAGES
#define ANR_PDF_MAX_PAGES 2000
#endif


#ifndef ANR_PDF_MAX_CUSTOM_FONTS
#define ANR_PDF_MAX_CUSTOM_FONTS 50
#endif


#ifndef ANR_PDF_MAX_BOOKMARKS
#define ANR_PDF_MAX_BOOKMARKS 2000
#endif

#ifndef ANR_PDF_MAX_OBJECTS_PER_PAGE
#define ANR_PDF_MAX_OBJECTS_PER_PAGE 10000
#endif

#ifndef ANR_PDF_MAX_ANNOTATIONS_PER_PAGE
#define ANR_PDF_MAX_ANNOTATIONS_PER_PAGE 200
#endif


#ifndef ANRPDFDEF
#ifdef ANR_PDF_STATIC
#define ANRPDFDEF static
#else
#define ANRPDFDEF extern
#endif
#endif

#define ANR_PDF_PLACEHOLDER_REF "00000000"

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
	uint64_t parentoffset; // offset to parent reference.
	uint64_t annotoffset; // offset to annot array reference.
} anr_pdf_page;

typedef struct
{
	anr_pdf_ref ref;
	char id[7]; // ImXXXX
	uint32_t width;
	uint32_t height;
} anr_pdf_img;

typedef enum
{
	ANR_PDF_ANNOTATION_TEXT,
	ANR_PDF_ANNOTATION_LINK,
} anr_pdf_annot_type;

typedef struct 
{
	anr_pdf_ref ref;
	anr_pdf_page parent;
	anr_pdf_annot_type type;
} anr_pdf_annot;

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

typedef enum
{
	ANR_PDF_ALIGN_LEFT,
	ANR_PDF_ALIGN_CENTER,
	ANR_PDF_ALIGN_RIGHT,
} anr_pdf_align;

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

typedef enum
{
	ANR_PDF_TEXT_RENDERING_FILL = 0,
	ANR_PDF_TEXT_RENDERING_STROKE = 1,
	ANR_PDF_TEXT_RENDERING_STROKETHENFILL = 2,
} anr_pdf_text_rendering_mode;

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
	anr_pdf_text_rendering_mode render_mode; // default 0 (see table 106)
	float rise; // default 0, can be negative
	anr_pdf_color color; // default black
	float angle; // default 0
} anr_pdf_txt_conf;

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
	ANR_PDF_ANNOTATION_MARKUP_HIGHLIGHT,
	ANR_PDF_ANNOTATION_MARKUP_UNDERLINE,
	ANR_PDF_ANNOTATION_MARKUP_SQUIGGLY,
	ANR_PDF_ANNOTATION_MARKUP_STRIKEOUT,
} anr_pdf_annotation_markup_type;

// See Table 52
// Parameters for all graphics.
typedef struct
{
	anr_pdf_linecap_style line_cap; // default 0
	int line_width; // default 0 = smallest possible line depending on device.
	anr_pdf_linejoin_style line_join; // default 0
	float miter_limit; // default 10, only applicable when line_join = ANR_PDF_LINEJOIN_MITER, must be > 0
	int dash_pattern[2]; // default = empty = solid line
	anr_pdf_color color; // default black
	// @Unimplemented: automatic stroke adjustment, op SA, see table 58
	char fill;
} anr_pdf_gfx_conf;

// Parameters for annotations. all optional.
typedef struct
{
	char* posted_by; // default NULL
	char* post_date; // default NULL
	anr_pdf_color color; // default yellow
	anr_pdf_annot parent; // default none
} anr_pdf_annot_cnf;

#define ANR_PDF_TXT_CONF_DEFAULT anr_pdf_txt_conf_default()
#define ANR_PDF_GFX_CONF_DEFAULT anr_pdf_gfx_conf_conf_default()
#define ANR_PDF_ANNOT_CONF_DEFAULT anr_pdf_annot_conf_default()

typedef enum
{
	ANR_PDF_STREAM_ENCODE_NONE,
	ANR_PDF_STREAM_ENCODE_ASCIIHEX,
	// @Unimplemented: Base85 encoding
} anr_pdf_stream_encoding;

typedef struct
{
	// Main data buffer
	char* body_buffer;
	uint64_t body_write_cursor;
	uint32_t buf_size;
	uint64_t next_obj_id;

	// Stream encoding state
	anr_pdf_stream_encoding stream_encoding;
	char writing_to_stream;

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
		anr_pdf_img images[ANR_PDF_MAX_OBJECTS_PER_PAGE];
		uint32_t images_count;
	} page;

	// list of pages
	anr_pdf_page pages[ANR_PDF_MAX_PAGES];
	uint64_t page_count;

	anr_pdf_annot all_annotations[ANR_PDF_MAX_ANNOTATIONS_PER_PAGE*ANR_PDF_MAX_PAGES];
	uint64_t all_annotations_count;

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
	anr_pdf_ref custom_fonts[ANR_PDF_MAX_CUSTOM_FONTS];
	uint32_t custom_fonts_count;
} anr_pdf;


// === DOCUMENT OPERATIONS === 
ANRPDFDEF anr_pdf* 			anr_pdf_document_begin();
ANRPDFDEF void 				anr_pdf_document_end(anr_pdf* pdf);
ANRPDFDEF void 				anr_pdf_document_free(anr_pdf* pdf);
ANRPDFDEF void 				anr_pdf_write_to_file(anr_pdf* pdf, const char* path);
// item_on_page optional, parent optional.
ANRPDFDEF anr_pdf_bookmark 	anr_pdf_document_add_bookmark(anr_pdf* pdf, anr_pdf_page page, anr_pdf_obj* item_on_page, 
								anr_pdf_bookmark* parent, const char* text); 
// Add document information to the pdf-> See chapter 14.3.3
// Dates follow ASN.1 format. see chapter 7.9.4. (YYYYMMDDHHmmSSOHH'mm) @Unimplemented: create helper function for this
ANRPDFDEF void 				anr_pdf_document_add_information_dictionary(anr_pdf* pdf, char* title, 
								char* author, char* subject, char* keywords, char* creator, 
								char* producer, char* creation_date, char* mod_date);

// === PAGE OPERATIONS ===
ANRPDFDEF void 			anr_pdf_page_begin(anr_pdf* pdf, anr_pdf_page_size size);
ANRPDFDEF anr_pdf_page 	anr_pdf_page_end(anr_pdf* pdf);
ANRPDFDEF anr_pdf_vecf	anr_pdf_page_get_size(anr_pdf_page_size size); // Returns the size of the page in user space units. (inches * 72)

// === ANNOTATION OPERATIONS ===
ANRPDFDEF anr_pdf_annot	anr_pdf_add_annotation_markup(anr_pdf* pdf, anr_pdf_page page, anr_pdf_obj obj, char* text, anr_pdf_annotation_markup_type type, anr_pdf_annot_cnf data);
ANRPDFDEF anr_pdf_annot anr_pdf_add_annotation_text(anr_pdf* pdf, anr_pdf_page page, anr_pdf_obj obj, char* text, anr_pdf_annot_cnf data);
ANRPDFDEF anr_pdf_annot anr_pdf_add_annotation_link(anr_pdf* pdf, anr_pdf_page src_page, anr_pdf_obj src_obj, anr_pdf_page dest_page, anr_pdf_obj* dest_obj, anr_pdf_annot_cnf data);

// === OBJECT OPERATIONS (pdf native) ===
ANRPDFDEF anr_pdf_obj anr_pdf_add_text(anr_pdf* pdf, const char* text, float x, float y, anr_pdf_txt_conf info);
ANRPDFDEF anr_pdf_obj anr_pdf_add_line(anr_pdf* pdf, anr_pdf_vecf p1, anr_pdf_vecf p2, anr_pdf_gfx_conf gfx);
ANRPDFDEF anr_pdf_obj anr_pdf_add_polygon(anr_pdf* pdf, anr_pdf_vecf* data, uint32_t data_length, anr_pdf_gfx_conf gfx);
ANRPDFDEF anr_pdf_obj anr_pdf_add_cubic_bezier(anr_pdf* pdf, anr_pdf_vecf* data, uint32_t data_length, anr_pdf_gfx_conf gfx);
ANRPDFDEF anr_pdf_obj anr_pdf_add_image(anr_pdf* pdf, anr_pdf_img img, float x, float y, float w, float h);

// === OBJECT OPERATIONS (wrappers) ===
ANRPDFDEF anr_pdf_obj anr_pdf_add_page_label(anr_pdf* pdf, const char* text, anr_pdf_align align);
ANRPDFDEF anr_pdf_obj anr_pdf_add_table(anr_pdf* pdf, float* rows, uint32_t row_count, float* cols, uint32_t col_count, anr_pdf_color color);
ANRPDFDEF anr_pdf_obj anr_pdf_add_rectangle(anr_pdf* pdf, anr_pdf_vecf tl, anr_pdf_vecf br, char fill, anr_pdf_color color);

// === FILE EMBEDDING ===
	// Image data is assumed to be in rgb color space. 3 bytes per pixel.
ANRPDFDEF anr_pdf_img anr_pdf_embed_image(anr_pdf* pdf, unsigned char* data, uint32_t length, uint32_t width, uint32_t height, uint8_t bits_per_sample);
ANRPDFDEF anr_pdf_ref anr_pdf_embed_ttf(anr_pdf* pdf, unsigned char* data, uint32_t length);

// === DEFAULT CONFIGS === 
ANRPDFDEF anr_pdf_txt_conf	anr_pdf_txt_conf_default();
ANRPDFDEF anr_pdf_gfx_conf	anr_pdf_gfx_conf_conf_default();
ANRPDFDEF anr_pdf_annot_cnf	anr_pdf_annot_conf_default();

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

static char* anr__pdf_encode_asciihex(const char* src, char* dest, uint64_t src_size, uint64_t dest_size)
{
	for (uint64_t i = 0; i < src_size; i++) {
		dest[i*2]   = "0123456789ABCDEF"[src[i] >> 4];
		dest[i*2+1] = "0123456789ABCDEF"[src[i] & 0x0F];
	}
	return dest;
}

static uint64_t anr__pdf_append_bytes(anr_pdf* pdf, const char* bytes, uint64_t size)
{
	char* encoded_data = malloc(size*2 + 1); // asciihex uses at most size*2
	char* ptr = (char*)encoded_data;

	// Encode data
	if (pdf->writing_to_stream) {
		switch(pdf->stream_encoding) {
			case ANR_PDF_STREAM_ENCODE_NONE: ptr = (char*)bytes; break;
			case ANR_PDF_STREAM_ENCODE_ASCIIHEX: 
				ptr = anr__pdf_encode_asciihex(bytes, ptr, size, sizeof(encoded_data));				
				size = size*2;
			break;
		}
	}
	else {
		ptr = (char*)bytes;
	}

	// check buffer bounds
	while (pdf->body_write_cursor + size >= pdf->buf_size)
	{
		pdf->buf_size += ANR_PDF_BUFFER_RESERVE;
		pdf->body_buffer = realloc(pdf->body_buffer, pdf->buf_size);
	}

	memcpy(pdf->body_buffer + pdf->body_write_cursor, ptr, size);
	uint64_t result = pdf->body_write_cursor;
	pdf->body_write_cursor += size;
	free(encoded_data);
	return result;
}

static uint64_t anr__pdf_append_str(anr_pdf* pdf, const char* bytes)
{
	uint64_t length = strlen(bytes);
	return anr__pdf_append_bytes(pdf, bytes, length);
}

static uint64_t anr__pdf_append_str_idref(anr_pdf* pdf, const char* bytes, anr_pdf_ref ref)
{
	char formatted_str[300];
	sprintf(formatted_str, bytes, ref.id);

	uint64_t length = strlen(formatted_str);
	uint64_t result = anr__pdf_append_bytes(pdf, formatted_str, length);
	return result;
}

static uint64_t anr__pdf_append_printf(anr_pdf* pdf, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    char formatted_str[300];
	vsnprintf(formatted_str, 300, fmt, ap);

	uint64_t length = strlen(formatted_str);
	uint64_t result = anr__pdf_append_bytes(pdf, formatted_str, length);

    va_end(ap);
    return result;
}

static anr_pdf_id anr__peak_next_id(anr_pdf* pdf)
{
	return pdf->next_obj_id;
}

static anr_pdf_ref anr__pdf_begin_obj(anr_pdf* pdf)
{
	#define XREF_ENTRY_SIZE 20
	anr_pdf_id id = pdf->next_obj_id++;
	char xref_entry[XREF_ENTRY_SIZE];
	memcpy(xref_entry, "0000000000 00000 n \n", XREF_ENTRY_SIZE);

	char offset_str[11];
	sprintf(offset_str, "%" PRId64 , pdf->body_write_cursor);
	memcpy(xref_entry + 10 - strlen(offset_str), offset_str, strlen(offset_str));

	// check buffer bounds
	while (pdf->xref.write_cursor + XREF_ENTRY_SIZE >= pdf->xref.buf_size)
	{
		pdf->xref.buf_size += ANR_PDF_BUFFER_RESERVE;
		pdf->xref.buffer = realloc(pdf->xref.buffer, pdf->xref.buf_size);
	}

	memcpy(pdf->xref.buffer + pdf->xref.write_cursor, xref_entry, XREF_ENTRY_SIZE);
	pdf->xref.write_cursor += XREF_ENTRY_SIZE;

	char newobj_entry[30];
	sprintf(newobj_entry, "\n%" PRId64 " 0 obj", id);
	anr__pdf_append_str(pdf, newobj_entry);

	return (anr_pdf_ref){.id = id, .offset_in_body = pdf->body_write_cursor};
}

static uint64_t anr__pdf_end_content_obj(anr_pdf* pdf, uint64_t write_start)
{
	pdf->writing_to_stream = 0;

	// Some encodings have EOD sign
	switch(pdf->stream_encoding)
	{
		case ANR_PDF_STREAM_ENCODE_NONE: break;
		case ANR_PDF_STREAM_ENCODE_ASCIIHEX: anr__pdf_append_str(pdf, ">"); break;
	}

	uint64_t write_end = anr__pdf_append_str(pdf, "\nendstream");
	anr__pdf_append_str(pdf, "\nendobj");

	uint64_t stream_length = write_end - write_start;

	// Object containing stream length.
	anr__pdf_begin_obj(pdf);
	anr__pdf_append_printf(pdf, "\n%d", stream_length);
	anr__pdf_append_str(pdf, "\nendobj");
	return write_end;
}

static anr_pdf_obj anr__pdf_begin_content_obj(anr_pdf* pdf, anr_pdf_recf rec)
{
	ANRPDF_ASSERT(!pdf->page.is_written);
	ANRPDF_ASSERT(pdf->page.objects_count < ANR_PDF_MAX_OBJECTS_PER_PAGE);
	anr_pdf_ref ref = anr__pdf_begin_obj(pdf);
	anr_pdf_id streamlen_id = anr__peak_next_id(pdf); // next object is stream length
	anr__pdf_append_str_idref(pdf, "\n<< /Length %d 0 R", (anr_pdf_ref){.id=streamlen_id});
	switch(pdf->stream_encoding)
	{
		case ANR_PDF_STREAM_ENCODE_NONE: break;
		case ANR_PDF_STREAM_ENCODE_ASCIIHEX: anr__pdf_append_str(pdf, "\n/Filter /ASCIIHexDecode"); break;
	}

	anr__pdf_append_str(pdf, ">>\nstream\n");
	pdf->writing_to_stream = 1;
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
	anr__pdf_append_printf(pdf, "\n%d", refxref+1);
}

static anr_pdf_ref anr__pdf_append_outlines(anr_pdf* pdf)
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

static void anr__pdf_replace_placeholder_id(anr_pdf* pdf, uint64_t offset, anr_pdf_ref ref)
{
	// We write the pdf in one go but some objects need to reference eachother.. :(
	char idbuf[20];
	sprintf(idbuf, "%" PRId64, ref.id);
	int len = strlen(idbuf);
	memcpy(pdf->body_buffer + offset + (sizeof(ANR_PDF_PLACEHOLDER_REF)-1-len), idbuf, len);
}

// Document catalog, see 7.7.2
static void anr__pdf_append_document_catalog(anr_pdf* pdf) 
{
	// Outlines
	anr_pdf_ref outlineref = anr__pdf_append_outlines(pdf);

	// Page tree
	anr_pdf_ref treeref = anr__pdf_begin_obj(pdf);
	anr__pdf_append_str(pdf, "\n<</Type /Pages");

	anr__pdf_append_str(pdf, "\n/Kids [");
	for (uint64_t i = 0; i < pdf->page_count; i++)
		anr__pdf_append_str_idref(pdf, "%d 0 R\n", pdf->pages[i].ref);
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
	anr__pdf_append_str(pdf, "\n/PageLabels << /Nums [ 0 << /S /D >> ] >>"); // arabic numerals for page numbering.
	anr__pdf_append_str(pdf, ">>");
	anr__pdf_append_str(pdf, "\nendobj");

	// Pages need reference to page tree.
	for (uint64_t i = 0; i < pdf->page_count; i++)
	{
		anr__pdf_replace_placeholder_id(pdf, pdf->pages[i].parentoffset, treeref);
	}

	// Pages need reference to annotation array.
	for (uint64_t p = 0; p < pdf->page_count; p++)
	{
		anr_pdf_ref ref = anr__pdf_begin_obj(pdf);
		anr__pdf_append_str(pdf, "\n[");
		for (uint64_t i = 0; i < pdf->all_annotations_count; i++) {
			if (pdf->all_annotations[i].parent.ref.id != pdf->pages[p].ref.id) continue;
			anr__pdf_append_str_idref(pdf, "\n%d 0 R", pdf->all_annotations[i].ref);	
		}
		anr__pdf_append_str(pdf, "\n]");
		anr__pdf_append_str(pdf, "\nendobj");

		anr__pdf_replace_placeholder_id(pdf, pdf->pages[p].annotoffset, ref);
	}

	pdf->catalog_ref = docref;
}

static void anr__create_default_font(anr_pdf* pdf) {
	#define LOAD_FONT(_name, _container) { \
		anr_pdf_ref fontref = anr__pdf_begin_obj(pdf); \
		anr__pdf_append_str(pdf, "\n<< /Type /Font"); \
		anr__pdf_append_str(pdf, "\n/Subtype /Type1"); \
		anr__pdf_append_str_idref(pdf, "\n/Name /F%d", fontref); \
		anr__pdf_append_str(pdf, "\n/BaseFont /"_name); \
		anr__pdf_append_str(pdf, "\n/Encoding /WinAnsiEncoding"); \
		anr__pdf_append_str(pdf, ">>"); \
		anr__pdf_append_str(pdf, "\nendobj"); \
		_container = fontref; \
	}

	LOAD_FONT("Times-Roman", pdf->default_font_ref);
	LOAD_FONT("Times-Italic", pdf->default_font_italic_ref);
	LOAD_FONT("Times-Bold", pdf->default_font_bold_ref);
	LOAD_FONT("Times-BoldItalic", pdf->default_font_italic_bold_ref);
}

anr_pdf* anr_pdf_document_begin()
{
	anr_pdf* pdf = malloc(sizeof(anr_pdf));
	memset(pdf, 0, sizeof(anr_pdf));
	pdf->body_buffer = malloc(ANR_PDF_BUFFER_RESERVE);
	pdf->buf_size = ANR_PDF_BUFFER_RESERVE;
	pdf->body_write_cursor = 0;
	pdf->next_obj_id = 1;
	pdf->doc_info_dic_ref = anr__pdf_emptyref();
	pdf->stream_encoding = ANR_PDF_STREAM_ENCODE_NONE;
	pdf->writing_to_stream = 0;

	pdf->xref.buffer = malloc(ANR_PDF_BUFFER_RESERVE);
	pdf->xref.write_cursor = 0;
	pdf->xref.buf_size = ANR_PDF_BUFFER_RESERVE;

	pdf->page.is_written = 1;

	anr__pdf_append_str(pdf, "%PDF-1.7");
	anr__create_default_font(pdf);

	return pdf;
}

void anr_pdf_document_free(anr_pdf* pdf)
{
	free(pdf->body_buffer);
	free(pdf->xref.buffer);
	free(pdf);
}

void anr_pdf_document_end(anr_pdf* pdf)
{
	anr__pdf_append_document_catalog(pdf);
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
	pdf->page.images_count = 0;
	pdf->page.size = size;
	// @Unimplemented: page rotation
}

anr_pdf_page anr_pdf_page_end(anr_pdf* pdf)
{
	ANRPDF_ASSERT(!pdf->page.is_written);

	anr_pdf_ref procsetref = anr__pdf_begin_obj(pdf);
	anr__pdf_append_str(pdf, "\n[/PDF /Text /ImageB /ImageC /ImageI]");
	anr__pdf_append_str(pdf, "\nendobj");

	anr_pdf_ref extgstateref = anr__pdf_begin_obj(pdf);
	anr__pdf_append_str(pdf, "\n<< /Type /ExtGState /SA true >>");
	anr__pdf_append_str(pdf, "\nendobj");


	anr_pdf_ref pageref = anr__pdf_begin_obj(pdf);
	anr__pdf_append_str(pdf, "\n<</Type /Page");
	uint64_t offset = anr__pdf_append_str(pdf, "\n/Parent "ANR_PDF_PLACEHOLDER_REF" 0 R"); // ref is set when pagetree is apended..
	anr__pdf_append_str_idref(pdf, "\n/Resources <</ProcSet %d 0 R", procsetref);
	anr__pdf_append_str_idref(pdf, "\n/ExtGState <</GS1 %d 0 R >>", extgstateref);

	// Import all default fonts in page.
	anr__pdf_append_str(pdf, "\n/Font <<");
	anr__pdf_append_printf(pdf, "\n/F%d %d 0 R", pdf->default_font_ref.id, pdf->default_font_ref.id);
	anr__pdf_append_printf(pdf, "\n/F%d %d 0 R", pdf->default_font_italic_ref.id, pdf->default_font_italic_ref.id);
	anr__pdf_append_printf(pdf, "\n/F%d %d 0 R", pdf->default_font_bold_ref.id, pdf->default_font_bold_ref.id);
	anr__pdf_append_printf(pdf, "\n/F%d %d 0 R", pdf->default_font_italic_bold_ref.id, pdf->default_font_italic_bold_ref.id);
	for (uint32_t i = 0; i < pdf->custom_fonts_count; i++)
		anr__pdf_append_printf(pdf, "\n/F%d %d 0 R", pdf->custom_fonts[i].id, pdf->custom_fonts[i].id);
	anr__pdf_append_str(pdf, "\n>>");

	// Add all images to xobject array.
	if (pdf->page.images_count) {
		anr__pdf_append_str(pdf, "\n/XObject <<\n");
		for (uint64_t i = 0; i < pdf->page.images_count; i++)
			anr__pdf_append_printf(pdf, "/%s %" PRId64 " 0 R\n", pdf->page.images[i].id, pdf->page.images[i].ref.id);
		anr__pdf_append_str(pdf, "\n>>");
	}

	anr__pdf_append_str(pdf, "\n>>");

	anr_pdf_vecf page_size = anr_pdf_page_get_size(pdf->page.size);
	anr__pdf_append_printf(pdf, "\n/MediaBox [0 0 %.3f %.3f]", page_size.x, page_size.y);
	uint64_t annotoffset = anr__pdf_append_str(pdf, "\n/Annots "ANR_PDF_PLACEHOLDER_REF" 0 R"); // ref is set when pagetree is appended..

	// Add all objects to content array.
	anr__pdf_append_str(pdf, "\n/Contents [\n");
	for (uint64_t i = 0; i < pdf->page.objects_count; i++)
		anr__pdf_append_str_idref(pdf, "%d 0 R\n", pdf->page.objects[i]);
	anr__pdf_append_str(pdf, "]>>");

	anr__pdf_append_str(pdf, "\nendobj");


	anr_pdf_page page = (anr_pdf_page){.ref = pageref, .size = pdf->page.size, .parentoffset = offset+9, .annotoffset = annotoffset + 9};
	pdf->pages[pdf->page_count++] = page;
	pdf->page.is_written = 1;

	return page;
}

anr_pdf_txt_conf anr_pdf_txt_conf_default()
{
	return (anr_pdf_txt_conf){0.0f,0.0f,100.0f,0.0f,12,anr__pdf_emptyref(),ANR_PDF_TEXT_RENDERING_FILL, 0.0f, (anr_pdf_color){0.0f,0.0f,0.0f}, 0.0f};
}

anr_pdf_gfx_conf anr_pdf_gfx_conf_conf_default()
{
	return (anr_pdf_gfx_conf){.line_cap = ANR_PDF_LINECAP_BUTT, .line_width = 0, .line_join = ANR_PDF_LINEJOIN_MITER, 
		.miter_limit = 10, .dash_pattern = {0}, .color = ANR_PDF_RGB(0.0f,0.0f,0.0f), 0};
}

anr_pdf_annot_cnf anr_pdf_annot_conf_default()
{
	return (anr_pdf_annot_cnf){.color=ANR_PDF_RGB(1.0f, 1.0f, 0.0f), .parent = {.ref.id = 0}, .post_date = NULL, .posted_by = NULL};
}

anr_pdf_obj anr_pdf_add_cubic_bezier(anr_pdf* pdf, anr_pdf_vecf* data, uint32_t data_length, anr_pdf_gfx_conf gfx) 
{
	ANRPDF_ASSERT(data_length >= 3);
	ANRPDF_ASSERT((data_length - 3) % 2 == 0);
	ANRPDF_ASSERT(gfx.miter_limit > 0); // no warning given & gfx will not be displayed if 0

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

	anr_pdf_obj obj_ref = anr__pdf_begin_content_obj(pdf, rec);

	uint64_t write_start = anr__pdf_append_printf(pdf, "\n%d J", gfx.line_cap);
	anr__pdf_append_printf(pdf, "\n%d w", gfx.line_width);
	anr__pdf_append_printf(pdf, "\n%d j", gfx.line_join);
	anr__pdf_append_printf(pdf, "\n%.3f M", gfx.miter_limit);
	if (gfx.dash_pattern[0] > 0 && gfx.dash_pattern[1] > 0) {
		anr__pdf_append_printf(pdf, "\n[%d %d] 0 d", gfx.dash_pattern[0], gfx.dash_pattern[1]);
	}
	anr__pdf_append_printf(pdf, "\n%.1f %.1f %.1f %s", gfx.color.r, gfx.color.g, gfx.color.b, gfx.fill ? "rg" : "RG");

	anr__pdf_append_printf(pdf, "\n%.3f %.3f m", data[0].x, data[0].y);
	anr__pdf_append_printf(pdf, "\n%.3f %.3f %.3f %.3f %.3f %.3f c", 
									data[0].x, data[0].y, data[1].x, data[1].y, data[2].x, data[2].y);

	for (uint32_t i = 3; i < data_length; i+=2)
	{
		anr_pdf_vecf point1 = data[i];
		anr_pdf_vecf point2 = data[i+1];
		anr__pdf_append_printf(pdf, "\n%.3f %.3f %.3f %.3f v", point1.x, point1.y, point2.x, point2.y);
	}

	anr__pdf_append_printf(pdf, gfx.fill ? "\nf" : "\nS");
	anr__pdf_append_printf(pdf, "\nn");

	anr__pdf_end_content_obj(pdf, write_start);
	return obj_ref;
}

anr_pdf_obj anr_pdf_add_line(anr_pdf* pdf, anr_pdf_vecf p1, anr_pdf_vecf p2, anr_pdf_gfx_conf gfx)
{
	anr_pdf_vecf data[2] = {p1, p2};
	return anr_pdf_add_polygon(pdf, data, 2, gfx);
}

anr_pdf_obj anr_pdf_add_polygon(anr_pdf* pdf, anr_pdf_vecf* data, uint32_t data_length, anr_pdf_gfx_conf gfx) 
{
	ANRPDF_ASSERT(data_length > 0);
	ANRPDF_ASSERT(gfx.miter_limit > 0);

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

	anr_pdf_obj obj_ref = anr__pdf_begin_content_obj(pdf, rec);

	uint64_t write_start = anr__pdf_append_printf(pdf, "\n%.3f %.3f m", data[0].x, data[0].y);
	anr__pdf_append_printf(pdf, "\n%d J", gfx.line_cap);
	anr__pdf_append_printf(pdf, "\n%d w", gfx.line_width);
	anr__pdf_append_printf(pdf, "\n%d j", gfx.line_join);
	anr__pdf_append_printf(pdf, "\n%.3f M", gfx.miter_limit);
	if (gfx.dash_pattern[0] > 0 && gfx.dash_pattern[1] > 0) {
		anr__pdf_append_printf(pdf, "\n[%d %d] 0 d", gfx.dash_pattern[0], gfx.dash_pattern[1]);
	}
	anr__pdf_append_printf(pdf, "\n%.1f %.1f %.1f %s", gfx.color.r, gfx.color.g, gfx.color.b, gfx.fill ? "rg" : "RG");

	for (uint32_t i = 1; i < data_length; i++)
	{
		anr_pdf_vecf point = data[i];
		anr__pdf_append_printf(pdf, "\n%.3f %.3f l", point.x, point.y);
	}

	anr__pdf_append_printf(pdf, gfx.fill ? "\nf" : "\nS");
	anr__pdf_append_printf(pdf, "\nn");

	anr__pdf_end_content_obj(pdf, write_start);
	return obj_ref;
}

anr_pdf_obj anr_pdf_add_rectangle(anr_pdf* pdf, anr_pdf_vecf tl, anr_pdf_vecf br, char fill, anr_pdf_color color)
{
	anr_pdf_vecf header[] = { tl, (anr_pdf_vecf){br.x, tl.y}, br, (anr_pdf_vecf){tl.x, br.y}  };
	anr_pdf_gfx_conf conf = ANR_PDF_GFX_CONF_DEFAULT;
	conf.fill = fill;
	conf.color = color;
	return anr_pdf_add_polygon(pdf, header, 4, conf);
}

anr_pdf_obj anr_pdf_add_table(anr_pdf* pdf, float* rows, uint32_t row_count, float* cols, uint32_t col_count, anr_pdf_color color)
{
	ANRPDF_ASSERT(row_count > 1);
	ANRPDF_ASSERT(col_count > 1);

	float ystart = rows[0];
	float xstart = cols[0];
	float yend = rows[row_count-1];
	float xend = cols[col_count-1];

	// Header row
	anr_pdf_obj header = anr_pdf_add_rectangle(pdf, (anr_pdf_vecf){cols[0], rows[0]},  (anr_pdf_vecf){xend, rows[1]}, 1, color);

	for (int i = 0; i < row_count; i++)
	{
		anr_pdf_vecf v1 = {xstart, rows[i]};
		anr_pdf_vecf v2 = {xend, rows[i]};
		anr_pdf_add_line(pdf, v1, v2, ANR_PDF_GFX_CONF_DEFAULT);
	}

	for (int i = 0; i < col_count; i++)
	{
		anr_pdf_vecf v1 = {cols[i], ystart};
		anr_pdf_vecf v2 = {cols[i], yend};
		anr_pdf_add_line(pdf, v1, v2, ANR_PDF_GFX_CONF_DEFAULT);
	}

	return header;
}

anr_pdf_obj anr_pdf_add_page_label(anr_pdf* pdf, const char* text, anr_pdf_align align)
{	
	anr_pdf_vecf page_size = anr_pdf_page_get_size(pdf->page.size);
	float x = ANR_INCH_TO_USU(0.8), y = ANR_INCH_TO_USU(0.5);

	switch (align)
	{
	case ANR_PDF_ALIGN_LEFT: break;
	case ANR_PDF_ALIGN_CENTER: x = page_size.x/2;
		break;
	case ANR_PDF_ALIGN_RIGHT: x = page_size.x - ANR_INCH_TO_USU(0.8);
		break;
	}
	
	return anr_pdf_add_text(pdf, text, x, y, ANR_PDF_TXT_CONF_DEFAULT);
}

anr_pdf_obj anr_pdf_add_text(anr_pdf* pdf, const char* text, float x, float y, anr_pdf_txt_conf info) 
{
	uint64_t str_len = strlen(text); // @Unimplemented: we need to calculate string width somehow
	anr_pdf_obj obj_ref = anr__pdf_begin_content_obj(pdf, ANR_PDF_REC(x, y + info.font_size, str_len*5, (float)info.font_size));
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
	anr__pdf_append_printf(pdf, "\n%f %f %f %f %f %f Tm", cosf(info.angle), sinf(info.angle), -sinf(info.angle), cosf(info.angle), x, y);
	anr__pdf_append_printf(pdf, "\nT* (%s) Tj", text);
	anr__pdf_append_str(pdf, "\nET");

	anr__pdf_end_content_obj(pdf, write_start);
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

static void anr__pdf_add_optional_annotation_data(anr_pdf* pdf, anr_pdf_annot_cnf data)
{
	if (data.posted_by) {
		anr__pdf_append_printf(pdf, "\n/T (%s)", data.posted_by);
	}
	if (anr__pdf_ref_valid(data.parent.ref)) {
		anr__pdf_append_str(pdf, "\n/RT /R");
		anr__pdf_append_str_idref(pdf, "\n/IRT %d 0 R", data.parent.ref);
	}
	if (data.post_date) {
		anr__pdf_append_printf(pdf, "\n/M (D:%s)", data.post_date);
	}
	anr__pdf_append_printf(pdf, "\n/C [%.2f %.2f %.2f]", data.color.r, data.color.g, data.color.b);
}

anr_pdf_annot anr_pdf_add_annotation_link(anr_pdf* pdf, anr_pdf_page src_page, anr_pdf_obj src_obj, anr_pdf_page dest_page, anr_pdf_obj* dest_obj, anr_pdf_annot_cnf data)
{
	ANRPDF_ASSERT(pdf->all_annotations_count < ANR_PDF_MAX_ANNOTATIONS_PER_PAGE);
	anr_pdf_ref ref = anr__pdf_begin_obj(pdf);
	
	anr__pdf_append_str(pdf, "\n<</Type /Annot");
	anr__pdf_append_str(pdf, "\n/Subtype /Link");
	anr__pdf_append_printf(pdf, "\n/Rect [%.2f %.2f %.2f %.2f]", 
		src_obj.rec.x, src_obj.rec.y, src_obj.rec.x + src_obj.rec.w, src_obj.rec.y - src_obj.rec.h);
	if (dest_obj) {
		anr__pdf_append_printf(pdf, "\n/Dest [%d 0 R /XYZ %f %f null]", dest_page.ref.id, dest_obj->rec.x, dest_obj->rec.y);
	}
	else {
		anr_pdf_vecf psize = anr_pdf_page_get_size(dest_page.size);
		anr__pdf_append_printf(pdf, "\n/Dest [%d 0 R /XYZ %f %f null]", dest_page.ref.id, 0, psize.y);
	}
	anr__pdf_append_str(pdf, "\n/Border [0 0 0 0]");
	anr__pdf_add_optional_annotation_data(pdf, data);
	anr__pdf_append_str(pdf, ">>");
	anr__pdf_append_str(pdf, "\nendobj");

	anr_pdf_annot annot = {.ref = ref, .parent = src_page};
	pdf->all_annotations[pdf->all_annotations_count++] = annot;

	return annot;
}

anr_pdf_annot anr_pdf_add_annotation_markup(anr_pdf* pdf, anr_pdf_page page, anr_pdf_obj obj, char* text, anr_pdf_annotation_markup_type type, anr_pdf_annot_cnf data)
{
	ANRPDF_ASSERT(pdf->all_annotations_count < ANR_PDF_MAX_ANNOTATIONS_PER_PAGE);
	anr_pdf_ref ref = anr__pdf_begin_obj(pdf);
	
	anr__pdf_append_str(pdf, "\n<</Type /Annot");
	switch(type) {
		case ANR_PDF_ANNOTATION_MARKUP_HIGHLIGHT: anr__pdf_append_str(pdf, "\n/Subtype /Highlight"); break;
		case ANR_PDF_ANNOTATION_MARKUP_UNDERLINE: anr__pdf_append_str(pdf, "\n/Subtype /Underline"); break;
		case ANR_PDF_ANNOTATION_MARKUP_SQUIGGLY: anr__pdf_append_str(pdf, "\n/Subtype /Squiggly"); break;
		case ANR_PDF_ANNOTATION_MARKUP_STRIKEOUT: anr__pdf_append_str(pdf, "\n/Subtype /StrikeOut"); break;
	}	
	anr__pdf_append_printf(pdf, "\n/Rect [%.2f %.2f %.2f %.2f]", obj.rec.x, obj.rec.y, obj.rec.x + obj.rec.w, obj.rec.y - obj.rec.h);
	obj.rec.y -= 2;
	anr__pdf_append_printf(pdf, "\n/QuadPoints [%.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f]", 
		obj.rec.x, 
		obj.rec.y - obj.rec.h, 
		obj.rec.x + obj.rec.w,
		obj.rec.y - obj.rec.h, 
		obj.rec.x, 
		obj.rec.y,	
		obj.rec.x + obj.rec.w, 
		obj.rec.y);
	anr__pdf_add_optional_annotation_data(pdf, data);
	anr__pdf_append_printf(pdf, "\n/Contents(%s)", text);
	anr__pdf_append_str(pdf, ">>");
	anr__pdf_append_str(pdf, "\nendobj");

	anr_pdf_annot annot = {.ref = ref, .parent = page};
	pdf->all_annotations[pdf->all_annotations_count++] = annot;

	return annot;
}

anr_pdf_annot anr_pdf_add_annotation_text(anr_pdf* pdf, anr_pdf_page page, anr_pdf_obj obj, char* text, anr_pdf_annot_cnf data)
{
	ANRPDF_ASSERT(pdf->all_annotations_count < ANR_PDF_MAX_ANNOTATIONS_PER_PAGE);
	anr_pdf_ref ref = anr__pdf_begin_obj(pdf);
	
	anr__pdf_append_str(pdf, "\n<</Type /Annot");
	anr__pdf_append_str(pdf, "\n/Subtype /Text");
	anr__pdf_append_printf(pdf, "\n/Rect [%.2f %.2f %.2f %.2f]", obj.rec.x, obj.rec.y, obj.rec.x + obj.rec.w, obj.rec.y - obj.rec.h);
	anr__pdf_append_printf(pdf, "\n/Contents (%s)", text);
	anr__pdf_add_optional_annotation_data(pdf, data);
	anr__pdf_append_str(pdf, ">>");
	anr__pdf_append_str(pdf, "\nendobj");

	anr_pdf_annot annot = {.ref = ref, .parent = page};
	pdf->all_annotations[pdf->all_annotations_count++] = annot;

	return annot;
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

anr_pdf_img anr_pdf_embed_image(anr_pdf* pdf, unsigned char* data, uint32_t length, uint32_t width, uint32_t height, uint8_t bits_per_sample)
{
	ANRPDF_ASSERT(length == width*height*3);

	anr_pdf_ref ref = anr__pdf_begin_obj(pdf);
	
	anr__pdf_append_str(pdf, "\n<</Type /XObject");
	anr__pdf_append_str(pdf, "\n/Subtype /Image");
	anr__pdf_append_printf(pdf, "\n/Width %d", width);
	anr__pdf_append_printf(pdf, "\n/Height %d", height);
	anr__pdf_append_str(pdf, "\n/ColorSpace /DeviceRGB");
	anr__pdf_append_printf(pdf, "\n/BitsPerComponent %d", bits_per_sample);
	anr__pdf_append_str(pdf, "\n/Interpolate true");
	anr__pdf_append_printf(pdf, "\n/Length %d", length);
	anr__pdf_append_str(pdf, ">>");
	anr__pdf_append_str(pdf, "\nstream\n");
	anr__pdf_append_bytes(pdf, (char*)data, length);
	anr__pdf_append_str(pdf, "\nendstream");
	anr__pdf_append_str(pdf, "\nendobj");

	return (anr_pdf_img){.ref = ref, .width = width, .height = height};
}

anr_pdf_obj anr_pdf_add_image(anr_pdf* pdf, anr_pdf_img img, float x, float y, float w, float h)
{
	// add image to page resources.
	char id[10];
	snprintf(id, 10, "Im%d", pdf->page.images_count);
	memcpy(img.id, id, sizeof(img.id));
	pdf->page.images[pdf->page.images_count++] = img;

	anr_pdf_obj obj_ref = anr__pdf_begin_content_obj(pdf, ANR_PDF_REC(x, y, w, h));

	uint64_t write_start = anr__pdf_append_str(pdf, "q");
	anr__pdf_append_printf(pdf, "\n%f 0 0 %f %f %f cm", w, h, x, y); // Translate & scale
	anr__pdf_append_printf(pdf, "\n/%s Do", id);
	anr__pdf_append_str(pdf, "\nQ");

	anr__pdf_end_content_obj(pdf, write_start);
	return obj_ref;
}

////////////////////////////////////////////////////////////
// code stripped from stb_truetype.h by Sean Barrett
////////////////////////////////////////////////////////////

#define ttBYTE(p)     (* (stbtt_uint8 *) (p))
#define ttCHAR(p)     (* (stbtt_int8 *) (p))
#define ttFixed(p)    ttLONG(p)
typedef unsigned char   stbtt_uint8;
typedef signed   char   stbtt_int8;
typedef unsigned short  stbtt_uint16;
typedef signed   short  stbtt_int16;
typedef unsigned int    stbtt_uint32;
typedef signed   int    stbtt_int32;
static stbtt_uint16 ttUSHORT(stbtt_uint8 *p) { return p[0]*256 + p[1]; }
static stbtt_int16 ttSHORT(stbtt_uint8 *p)   { return p[0]*256 + p[1]; }
static stbtt_uint32 ttULONG(stbtt_uint8 *p)  { return (p[0]<<24) + (p[1]<<16) + (p[2]<<8) + p[3]; }
#define stbtt_tag4(p,c0,c1,c2,c3) ((p)[0] == (c0) && (p)[1] == (c1) && (p)[2] == (c2) && (p)[3] == (c3))
#define stbtt_tag(p,str)           stbtt_tag4(p,str[0],str[1],str[2],str[3])

int stbtt_FindGlyphIndex(unsigned char* data, uint32_t index_map, int unicode_codepoint)
{
   stbtt_uint16 format = ttUSHORT(data + index_map + 0);
   if (format == 0) { // apple byte encoding
      stbtt_int32 bytes = ttUSHORT(data + index_map + 2);
      if (unicode_codepoint < bytes-6)
         return ttBYTE(data + index_map + 6 + unicode_codepoint);
      return 0;
   } else if (format == 6) {
      stbtt_uint32 first = ttUSHORT(data + index_map + 6);
      stbtt_uint32 count = ttUSHORT(data + index_map + 8);
      if ((stbtt_uint32) unicode_codepoint >= first && (stbtt_uint32) unicode_codepoint < first+count)
         return ttUSHORT(data + index_map + 10 + (unicode_codepoint - first)*2);
      return 0;
   } else if (format == 2) {
      
      return 0;
   } else if (format == 4) { // standard mapping for windows fonts: binary search collection of ranges
      stbtt_uint16 segcount = ttUSHORT(data+index_map+6) >> 1;
      stbtt_uint16 searchRange = ttUSHORT(data+index_map+8) >> 1;
      stbtt_uint16 entrySelector = ttUSHORT(data+index_map+10);
      stbtt_uint16 rangeShift = ttUSHORT(data+index_map+12) >> 1;

      // do a binary search of the segments
      stbtt_uint32 endCount = index_map + 14;
      stbtt_uint32 search = endCount;

      if (unicode_codepoint > 0xffff)
         return 0;

      // they lie from endCount .. endCount + segCount
      // but searchRange is the nearest power of two, so...
      if (unicode_codepoint >= ttUSHORT(data + search + rangeShift*2))
         search += rangeShift*2;

      // now decrement to bias correctly to find smallest
      search -= 2;
      while (entrySelector) {
         stbtt_uint16 end;
         searchRange >>= 1;
         end = ttUSHORT(data + search + searchRange*2);
         if (unicode_codepoint > end)
            search += searchRange*2;
         --entrySelector;
      }
      search += 2;

      {
         stbtt_uint16 offset, start, last;
         stbtt_uint16 item = (stbtt_uint16) ((search - endCount) >> 1);

         start = ttUSHORT(data + index_map + 14 + segcount*2 + 2 + 2*item);
         last = ttUSHORT(data + endCount + 2*item);
         if (unicode_codepoint < start || unicode_codepoint > last)
            return 0;

         offset = ttUSHORT(data + index_map + 14 + segcount*6 + 2 + 2*item);
         if (offset == 0)
            return (stbtt_uint16) (unicode_codepoint + ttSHORT(data + index_map + 14 + segcount*4 + 2 + 2*item));

         return ttUSHORT(data + offset + (unicode_codepoint-start)*2 + index_map + 14 + segcount*6 + 2 + 2*item);
      }
   } else if (format == 12 || format == 13) {
      stbtt_uint32 ngroups = ttULONG(data+index_map+12);
      stbtt_int32 low,high;
      low = 0; high = (stbtt_int32)ngroups;
      // Binary search the right group.
      while (low < high) {
         stbtt_int32 mid = low + ((high-low) >> 1); // rounds down, so low <= mid < high
         stbtt_uint32 start_char = ttULONG(data+index_map+16+mid*12);
         stbtt_uint32 end_char = ttULONG(data+index_map+16+mid*12+4);
         if ((stbtt_uint32) unicode_codepoint < start_char)
            high = mid;
         else if ((stbtt_uint32) unicode_codepoint > end_char)
            low = mid+1;
         else {
            stbtt_uint32 start_glyph = ttULONG(data+index_map+16+mid*12+8);
            if (format == 12)
               return start_glyph + unicode_codepoint-start_char;
            else // format == 13
               return start_glyph;
         }
      }
      return 0; // not found
   }
   // @TODO
   return 0;
}
static stbtt_uint32 stbtt__find_table(unsigned char *data, stbtt_uint32 fontstart, const char *tag)
{
   stbtt_int32 num_tables = ttUSHORT(data+fontstart+4);
   stbtt_uint32 tabledir = fontstart + 12;
   stbtt_int32 i;
   for (i=0; i < num_tables; ++i) {
      stbtt_uint32 loc = tabledir + 16*i;
      if (stbtt_tag(data+loc+0, tag))
         return ttULONG(data+loc+8);
   }
   return 0;
}
void stbtt_GetGlyphHMetrics(unsigned char* data, uint32_t hmtx, int glyph_index, int *advanceWidth, int *leftSideBearing)
{
	uint32_t hhea = stbtt__find_table(data, 0, "hhea");
	stbtt_uint16 numOfLongHorMetrics = ttUSHORT(data+hhea + 34);
	if (glyph_index < numOfLongHorMetrics) {
		if (advanceWidth)     *advanceWidth    = ttSHORT(data + hmtx + 4*glyph_index);
		if (leftSideBearing)  *leftSideBearing = ttSHORT(data + hmtx + 4*glyph_index + 2);
	} else {
		if (advanceWidth)     *advanceWidth    = ttSHORT(data + hmtx + 4*(numOfLongHorMetrics-1));
		if (leftSideBearing)  *leftSideBearing = ttSHORT(data + hmtx + 4*numOfLongHorMetrics + 2*(glyph_index - numOfLongHorMetrics));
	}
}
void stbtt_GetCodepointHMetrics(unsigned char* data, uint32_t hmtx, uint32_t index_map, int codepoint, int *advanceWidth, int *leftSideBearing)
{
   stbtt_GetGlyphHMetrics(data, hmtx, stbtt_FindGlyphIndex(data,index_map,codepoint), advanceWidth, leftSideBearing);
}
uint32_t anr__pdf_get_ttf_codepoint_width(unsigned char* data, uint32_t codepoint)
{
	uint32_t hmtx = stbtt__find_table(data, 0, "hmtx");
	uint32_t cmap = stbtt__find_table(data, 0, "cmap");

	uint32_t numTables = ttUSHORT(data + cmap + 2);
	uint32_t index_map = 0;
	for (uint32_t i=0; i < numTables; ++i) {
		stbtt_uint32 encoding_record = cmap + 4 + 8 * i;
		// find an encoding we understand:
		switch(ttUSHORT(data+encoding_record)) {
			case 3:
				switch (ttUSHORT(data+encoding_record+2)) {
				case 1:
				case 10:
					index_map = cmap + ttULONG(data+encoding_record+4);
					break;
				}
				break;
			case 0:
				index_map = cmap + ttULONG(data+encoding_record+4);
				break;
		}
	}

	int32_t advance;
	int32_t lsb;
	stbtt_GetCodepointHMetrics(data, hmtx, index_map, codepoint, &advance, &lsb);
	return advance;
}

////////////////////////////////////////////////////////////
// end of code from stb_truetype.h
////////////////////////////////////////////////////////////

anr_pdf_ref anr_pdf_embed_ttf(anr_pdf* pdf, unsigned char* data, uint32_t length)
{
	ANRPDF_ASSERT(pdf->all_annotations_count < ANR_PDF_MAX_CUSTOM_FONTS);

	anr_pdf_ref ttf_ref = anr__pdf_begin_obj(pdf);
	anr__pdf_append_printf(pdf, "\n<</Length %d>>", length);
	anr__pdf_append_str(pdf, "\nstream\n");
	anr__pdf_append_bytes(pdf, (char*)data, length);
	anr__pdf_append_str(pdf, "\nendstream");
	anr__pdf_append_str(pdf, "\nendobj");

	anr_pdf_ref widths_ref = anr__pdf_begin_obj(pdf);
	anr__pdf_append_str(pdf, "\n[ ");
	{
		for (uint32_t i = 0; i < 0xFFFF; i++)
		{
			int advance = anr__pdf_get_ttf_codepoint_width(data, i);
			anr__pdf_append_printf(pdf, "%d ", advance);
		}
	}
	anr__pdf_append_str(pdf, "]\nendobj");

	// These values have been pulled from my behind.
	anr_pdf_ref descriptor_ref = anr__pdf_begin_obj(pdf);
	anr__pdf_append_str(pdf, "\n<</Type /FontDescriptor");
	anr__pdf_append_str_idref(pdf, "\n/FontName /F%d", descriptor_ref);
	anr__pdf_append_printf(pdf, "\n/Flags %d", 1 << 5); // nonsymbolic
	anr__pdf_append_str(pdf, "\n/FontBBox [-92.773438 -312.01172 1186.52344 1102.05078]"); 
	anr__pdf_append_str(pdf, "\n/MissingWidth 350"); 
	anr__pdf_append_str(pdf, "\n/ItalicAngle 0");
	anr__pdf_append_str(pdf, "\n/Ascent 1102.05078");
	anr__pdf_append_str(pdf, "\n/Descent -291.50391");
	anr__pdf_append_str(pdf, "\n/CapHeight 389.16016");
	anr__pdf_append_str(pdf, "\n/StemV 61.035156");
	anr__pdf_append_str_idref(pdf, "\n/FontFile2 %d 0 R", ttf_ref);
	anr__pdf_append_str(pdf, ">>");
	anr__pdf_append_str(pdf, "\nendobj");

	anr_pdf_ref ref = anr__pdf_begin_obj(pdf);
	anr__pdf_append_str(pdf, "\n<</Type /Font");
	anr__pdf_append_str(pdf, "\n/Subtype /TrueType");
	anr__pdf_append_str_idref(pdf, "\n/Name /F%d", ref);
	anr__pdf_append_str_idref(pdf, "\n/BaseFont /F%d", descriptor_ref);
	anr__pdf_append_str(pdf, "\n/FirstChar 0");
	anr__pdf_append_str(pdf, "\n/LastChar 255");
	anr__pdf_append_str_idref(pdf, "\n/Widths %d 0 R", widths_ref);
	anr__pdf_append_str(pdf, "\n/Encoding /WinAsciEncoding");
	anr__pdf_append_str_idref(pdf, "\n/FontDescriptor %d 0 R", descriptor_ref);
	anr__pdf_append_str(pdf, ">>");
	anr__pdf_append_str(pdf, "\nendobj");

	pdf->custom_fonts[pdf->custom_fonts_count++] = ref;

	return ref;
}

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