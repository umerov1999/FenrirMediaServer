#include "jinclude.h"
#include "jpeglib.h"
#include "jerror.h"
#include "libimage.h"

#ifdef HAVE_UNSIGNED_CHAR
typedef unsigned char U_CHAR;
#define UCH(x)	((int) (x))
#else /* !HAVE_UNSIGNED_CHAR */
#ifdef CHAR_IS_UNSIGNED
typedef char U_CHAR;
#define UCH(x)	((int) (x))
#else
typedef char U_CHAR;
#define UCH(x)	((int) (x) & 0xFF)
#endif
#endif /* HAVE_UNSIGNED_CHAR */

typedef struct cjpeg_source_struct* cjpeg_source_ptr;

struct cjpeg_source_struct {
    JMETHOD(void, start_input, (j_compress_ptr cinfo,
        cjpeg_source_ptr sinfo));
    JMETHOD(JDIMENSION, get_pixel_rows, (j_compress_ptr cinfo,
        cjpeg_source_ptr sinfo));
    JMETHOD(void, finish_input, (j_compress_ptr cinfo,
        cjpeg_source_ptr sinfo));

    PBYTE input_buf;
    UINT read_offset;

    JSAMPARRAY buffer;
    JDIMENSION buffer_height;
};

/* Private version of data source object */

typedef struct _bmp_source_struct* bmp_source_ptr;

typedef struct _bmp_source_struct {
    struct cjpeg_source_struct pub; /* public fields */

    j_compress_ptr cinfo;		/* back link saves passing separate parm */

    JSAMPARRAY colormap;		/* BMP colormap (converted to my format) */

    jvirt_sarray_ptr whole_image;	/* Needed to reverse row order */
    JDIMENSION source_row;	/* Current source row number */
    JDIMENSION row_width;		/* Physical width of scanlines in file */

    int bits_per_pixel;		/* remembers 8- or 24-bit format */
} bmp_source_struct;


// JPEG related functions
int ReadOK(bmp_source_ptr, unsigned char*, int);
int read_byte(bmp_source_ptr);
void read_colormap(bmp_source_ptr, int, int);
JDIMENSION get_8bit_row(j_compress_ptr, cjpeg_source_ptr);
JDIMENSION get_16bit_row(j_compress_ptr, cjpeg_source_ptr);
JDIMENSION get_24bit_row(j_compress_ptr, cjpeg_source_ptr);
JDIMENSION get_32bit_row(j_compress_ptr, cjpeg_source_ptr);
JDIMENSION preload_image(j_compress_ptr, cjpeg_source_ptr);
void start_input_bmp(j_compress_ptr, cjpeg_source_ptr);
void finish_input_bmp(j_compress_ptr, cjpeg_source_ptr);
cjpeg_source_ptr jinit_read_bmp(j_compress_ptr);


/*
 * This function taken from the JPEG-8 example file rdbmp.c provided a
 * platform idenependant way to read files...
 * But, we "reading" from memory. So, return the current byte
 * in the buf and inc the pointer so it "feels" like an fopen read.
 */
int ReadOK(bmp_source_ptr sinfo, unsigned char* buffer, int len)
{
    memcpy(buffer, sinfo->pub.input_buf + sinfo->pub.read_offset, len);
    sinfo->pub.read_offset += len;
    return 1; // yeah, it always works cuz I say so..
}

/*
 * Like ReadOK, this would read from a file. But we aren't reading a file.
 * So, return the current byte in the buf and inc the pointer.
 * WARNING: I don't think this function is working. (My guess: read_offset++)
 * However, it just so happens since Windows 7 (and I think all the windows)
 * screenshots always return a 32 bit BMP, the code never calls this function.
 *
 */
int read_byte(bmp_source_ptr sinfo)
{
    return (int)(INT_PTR)sinfo->pub.input_buf + sinfo->pub.read_offset++;
}

/*
 * Since I think windows screenshot is always a 32bit BMP this function
 * will never be used, however, I am leaving it here in case there is a
 * version of windows that does return a 8bit indexed BMP. Once it is
 * confirmed that all windows use 32bit BMPs, I'll remove this.
 *
 * How does a BMP look you ask?
 * see: http://local.wasp.uwa.edu.au/~pbourke/dataformats/bitmaps/
 */
void read_colormap(bmp_source_ptr sinfo, int cmaplen, int mapentrysize)
{
    int i;

    switch (mapentrysize) {
    case 3:
        /* BGR format (occurs in OS/2 files) */
        for (i = 0; i < cmaplen; i++) {
            sinfo->colormap[2][i] = (JSAMPLE)read_byte(sinfo);
            sinfo->colormap[1][i] = (JSAMPLE)read_byte(sinfo);
            sinfo->colormap[0][i] = (JSAMPLE)read_byte(sinfo);
        }
        break;
    case 4:
        /* BGR0 format (occurs in MS Windows files) */
        for (i = 0; i < cmaplen; i++) {
            sinfo->colormap[2][i] = (JSAMPLE)read_byte(sinfo);
            sinfo->colormap[1][i] = (JSAMPLE)read_byte(sinfo);
            sinfo->colormap[0][i] = (JSAMPLE)read_byte(sinfo);
            (void)read_byte(sinfo);
        }
        break;
    default:
        return; //ERREXIT(sinfo->cinfo, JERR_BMP_BADCMAP);
        break;
    }
}

/*
 * Used to help convert 16 bit BMP
 * Taken from: http://bytes.com/topic/c/answers/552128-how-convert-16-bit-565-rgb-value-32-bit
 *
 * BUG: I haven't been able to figure out the correct format of the BMP in memory.
 *      Not sure if its 565 or 555. Nor am I sure if its rgb or bgr or what. Also
 *      I can't say I'm sure which order the two 8 bits that make up the unsigned
 *      short "a" should come in. As it is now, this will send back a valid JPEG.
 *      But, the colors won't be exact.
 */
unsigned long rgb16_to_rgb32(unsigned short a)
{
    /* 1. Extract the red, green and blue values */

    /* (555) from bbbb bggg ggrr rrr0 */
    unsigned long b = (a & 0xF800) >> 11;
    unsigned long g = (a & 0x07C0) >> 6;
    unsigned long r = (a & 0x003E) >> 1;

    /* (565) from rrrr rggg gggb bbbb */
    // unsigned long r = (a & 0xF800) >> 11;
    // unsigned long g = (a & 0x07E0) >> 5;
    // unsigned long b = (a & 0x001F);

    /* (555) from 0rrr rrgg gggb bbbb */
    // unsigned long r = (a & 0x7C00) >> 10;
    // unsigned long g = (a & 0x03E0) >> 5;
    // unsigned long b = (a & 0x001F);

    /* (555) from 0bbb bbgg gggr rrrr */
    //unsigned long b = (a & 0x7C00) >> 10;
    //unsigned long g = (a & 0x03E0) >> 5;
    //unsigned long r = (a & 0x001F);


    /* (555) from rrrr rggg ggbb bbb0 */
    //unsigned long r = (a & 0xF800) >> 11;
    //unsigned long g = (a & 0x07C0) >> 6;
    //unsigned long b = (a & 0x003E) >> 1;

    /* (565) from bbbb bggg gggr rrrr */
    //unsigned long b = (a & 0xF800) >> 11;
    //unsigned long g = (a & 0x07E0) >> 5;
    //unsigned long r = (a & 0x001F);

    /* 2. Convert them to 0-255 range:
    There is more than one way. You can just shift them left:
    to 00000000 rrrrr000 gggggg00 bbbbb000
    r <<= 3;
    g <<= 2;
    b <<= 3;
    But that means your image will be slightly dark and
    off-colour as white 0xFFFF will convert to F8,FC,F8
    So instead you can scale by multiply and divide: */
    r <<= 3;
    //g <<= 2; //(565)
    g <<= 3; //(555)
    b <<= 3;
    //r = r * 255 / 31;
    //g = g * 255 / 63; //(565)
    ////g = g * 255 / 31; //(555)
    //b = b * 255 / 31;
    /* This ensures 31/31 converts to 255/255 */

    /* 3. Construct your 32-bit format (this is 0RGB): */
    //return (r << 16) | (g << 8) | b;

    // This is 0RBG?? Yeah, it makes no sense to me either. 
    return (r << 16) | (b << 8) | g;

    /* Or for BGR0: */
    //return (r << 8) | (g << 16) | (b << 24);

}


/*
 * Read one row of pixels.
 * The image has been read into the whole_image array, but is otherwise
 * unprocessed.  We must read it out in top-to-bottom row order, and if
 * it is an 8-bit image, we must expand colormapped pixels to 24bit format.
 *
 * NOTE: Again, windows might only ever use 32bit BMP's making this function
 * useless. However, I'll leave it here until I can confirm that.
 *
 * NOTE: cjpeg_source_ptr sinfo is really a BMP ptr.
 */

JDIMENSION get_8bit_row(j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
/* This version is for reading 8-bit colormap indexes */
{
    bmp_source_ptr source = (bmp_source_ptr)sinfo;
    register JSAMPARRAY colormap = source->colormap;
    JSAMPARRAY image_ptr;
    register int t;
    register JSAMPROW inptr, outptr;
    register JDIMENSION col;

    /* Fetch next row from virtual array */
    source->source_row--;
    image_ptr = (*cinfo->mem->access_virt_sarray)
        ((j_common_ptr)cinfo, source->whole_image,
            source->source_row, (JDIMENSION)1, FALSE);

    /* Expand the colormap indexes to real data */
    inptr = image_ptr[0];
    outptr = source->pub.buffer[0];
    for (col = cinfo->image_width; col > 0; col--) {
        t = GETJSAMPLE(*inptr++);
        *outptr++ = colormap[0][t];	/* can omit GETJSAMPLE() safely */
        *outptr++ = colormap[1][t];
        *outptr++ = colormap[2][t];
    }

    return 1;
}

/*
 *
 * NOTE: Damn it, windows uses what ever the colors option is set to-
 *  High Color (16 bit)
 *  True Color (32 bit)
 *  Who the hell would use High Color? PDA's?
 *
 * NOTE: cjpeg_source_ptr sinfo is really a BMP ptr.
 *
 * Dev notes:
 * http://www.winehq.org/pipermail/wine-patches/2005-August/020010.html
 * http://www.cpp-home.com/tutorials/246_2.htm
 * http://bytes.com/topic/c/answers/552128-how-convert-16-bit-565-rgb-value-32-bit
 */

JDIMENSION get_16bit_row(j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
{
    bmp_source_ptr source = (bmp_source_ptr)sinfo;
    JSAMPARRAY image_ptr;
    register JSAMPROW inptr, outptr;
    register JDIMENSION col;
    unsigned long bit32_pix;
    char a, b;
    char* pix_ptr;
    /* Fetch next row from virtual array */
    source->source_row--;
    image_ptr = (*cinfo->mem->access_virt_sarray)
        ((j_common_ptr)cinfo, source->whole_image,
            source->source_row, (JDIMENSION)1, FALSE);

    /* Transfer data.  Note source values are in BGR order
     * (even though Microsoft's own documents say the opposite).
     */
    inptr = image_ptr[0];
    outptr = source->pub.buffer[0];
    for (col = cinfo->image_width; col > 0; col--) {
        // Need to pull 16 bits at a time. 
        a = *inptr++; // First 8
        b = *inptr++; // Second 8
        bit32_pix = rgb16_to_rgb32(a << 8 | b); //Send all 16bits to be converted
        pix_ptr = (char*)&bit32_pix;
        outptr[2] = *pix_ptr++;
        outptr[1] = *pix_ptr++;
        outptr[0] = *pix_ptr++;
        outptr += 3;
    }

    return 1;
}

/*
 *
 * NOTE: Again, windows might only ever use 32bit BMP's making this function
 * useless. However, I'll leave it here until I can confirm that.
 *
 * NOTE: cjpeg_source_ptr sinfo is really a BMP ptr.
 */

JDIMENSION get_24bit_row(j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
/* This version is for reading 24-bit pixels */
{
    bmp_source_ptr source = (bmp_source_ptr)sinfo;
    JSAMPARRAY image_ptr;
    register JSAMPROW inptr, outptr;
    register JDIMENSION col;

    /* Fetch next row from virtual array */
    source->source_row--;
    image_ptr = (*cinfo->mem->access_virt_sarray)
        ((j_common_ptr)cinfo, source->whole_image,
            source->source_row, (JDIMENSION)1, FALSE);

    /* Transfer data.  Note source values are in BGR order
     * (even though Microsoft's own documents say the opposite).
     */
    inptr = image_ptr[0];
    outptr = source->pub.buffer[0];
    for (col = cinfo->image_width; col > 0; col--) {
        outptr[2] = *inptr++;	/* can omit GETJSAMPLE() safely */
        outptr[1] = *inptr++;
        outptr[0] = *inptr++;
        outptr += 3;
    }

    return 1;
}

/*
 *
 * NOTE: Again, windows might only ever use 32bit BMP's making this function
 * useless. However, I'll leave it here until I can confirm that.
 *
 * NOTE: cjpeg_source_ptr sinfo is really a BMP ptr.
 */
JDIMENSION get_32bit_row(j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
/* This version is for reading 32-bit pixels */
{
    bmp_source_ptr source = (bmp_source_ptr)sinfo;
    JSAMPARRAY image_ptr;
    register JSAMPROW inptr, outptr;
    register JDIMENSION col;

    /* Fetch next row from virtual array */
    source->source_row--;
    image_ptr = (*cinfo->mem->access_virt_sarray)
        ((j_common_ptr)cinfo, source->whole_image,
            source->source_row, (JDIMENSION)1, FALSE);
    /* Transfer data.  Note source values are in BGR order
     * (even though Microsoft's own documents say the opposite).
     */
    inptr = image_ptr[0];
    outptr = source->pub.buffer[0];
    for (col = cinfo->image_width; col > 0; col--) {
        outptr[2] = *inptr++;	/* can omit GETJSAMPLE() safely */
        outptr[1] = *inptr++;
        outptr[0] = *inptr++;
        inptr++; // Skip the 4th bit (Alpha Channel)
        outptr += 3;
    }
    return 1;
}


/*
 * This method loads the image into whole_image during the first call on
 * get_pixel_rows.  The get_pixel_rows pointer is then adjusted to call
 * get_8bit_row, get_24bit_row or get_32bit_row on subsequent calls.
 * This will not copy the image header info. Just the raw image data.
 */
JDIMENSION preload_image(j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
{
    bmp_source_ptr source = (bmp_source_ptr)sinfo;
    //  register FILE *infile = source->pub.input_file;
    //  register int c;
    register JSAMPROW out_ptr;
    JSAMPARRAY image_ptr;
    JDIMENSION row;

    /* Read the data into a virtual array in input-file row order. */


    for (row = 0; row < cinfo->image_height; row++) {
        image_ptr = (*cinfo->mem->access_virt_sarray)
            ((j_common_ptr)cinfo, source->whole_image,
                row, (JDIMENSION)1, TRUE);
        out_ptr = image_ptr[0];

        // Copy the bmp data
        memcpy(out_ptr, source->pub.input_buf + source->pub.read_offset, source->row_width);
        source->pub.read_offset += source->row_width;
    }

    /* Set up to read from the virtual array in top-to-bottom order */
    switch (source->bits_per_pixel) {
    case 8:
        source->pub.get_pixel_rows = get_8bit_row;
        break;
    case 16:
        source->pub.get_pixel_rows = get_16bit_row;
        break;
    case 24:
        source->pub.get_pixel_rows = get_24bit_row;
        break;
    case 32:
        source->pub.get_pixel_rows = get_32bit_row;
        break;
    default:
        return 0; //ERREXIT(cinfo, JERR_BMP_BADDEPTH);
    }
    source->source_row = cinfo->image_height;

    /* And read the first row */
    return (*source->pub.get_pixel_rows) (cinfo, sinfo);
}


/*
 * Read the file header; return image size and component count.
 * A lot of this could might be safe to remove since we might
 * only ever be using 32bit Windows BMP images. UPDATE: or 16bit BMPs
 */
void start_input_bmp(j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
{
    bmp_source_ptr source = (bmp_source_ptr)sinfo;
    U_CHAR bmpfileheader[14];
    U_CHAR bmpinfoheader[64];
#define GET_2B(array,offset)  ((unsigned int) UCH(array[offset]) + \
			       (((unsigned int) UCH(array[offset+1])) << 8))
#define GET_4B(array,offset)  ((INT32) UCH(array[offset]) + \
			       (((INT32) UCH(array[offset+1])) << 8) + \
			       (((INT32) UCH(array[offset+2])) << 16) + \
			       (((INT32) UCH(array[offset+3])) << 24))
    INT32 bfOffBits;
    INT32 headerSize;
    INT32 biWidth;
    INT32 biHeight;
    unsigned int biPlanes;
    INT32 biCompression;
    INT32 biXPelsPerMeter, biYPelsPerMeter;
    INT32 biClrUsed = 0;
    int mapentrysize = 0;		/* 0 indicates no colormap */
    INT32 bPad;
    JDIMENSION row_width;

    /* Read and verify the bitmap file header */
    // Its a bitmap... I just made it.. But, if you findout otherwise
    // return without an error message.. Better than a crash I guess. 

    if (!ReadOK(source, bmpfileheader, 14))
        return; //ERREXIT(cinfo, JERR_INPUT_EOF);
    if (GET_2B(bmpfileheader, 0) != 0x4D42) /* 'BM' */
        return; //ERREXIT(cinfo, JERR_BMP_NOT);
    bfOffBits = (INT32)GET_4B(bmpfileheader, 10);
    /* We ignore the remaining fileheader fields */

    /* The infoheader might be 12 bytes (OS/2 1.x), 40 bytes (Windows),
     * or 64 bytes (OS/2 2.x).  Check the first 4 bytes to find out which.
     *
     * Read the first for bytes to figure out how big the header is.
     * The read the rest of the header (once we know its size)
     */
    if (!ReadOK(source, bmpinfoheader, 4))
        return; //ERREXIT(cinfo, JERR_INPUT_EOF);
    headerSize = (INT32)GET_4B(bmpinfoheader, 0);


    if (headerSize < 12 || headerSize > 64)
        return; //ERREXIT(cinfo, JERR_BMP_BADHEADER);
    if (!ReadOK(source, bmpinfoheader + 4, headerSize - 4)) // Read the rest of the header
        return; //ERREXIT(cinfo, JERR_INPUT_EOF);

    switch ((int)headerSize) {
    case 12:
        /* Decode OS/2 1.x header (Microsoft calls this a BITMAPCOREHEADER) */
        biWidth = (INT32)GET_2B(bmpinfoheader, 4);
        biHeight = (INT32)GET_2B(bmpinfoheader, 6);
        biPlanes = GET_2B(bmpinfoheader, 8);
        source->bits_per_pixel = (int)GET_2B(bmpinfoheader, 10);

        switch (source->bits_per_pixel) {
        case 8:			/* colormapped image */
            mapentrysize = 3;		/* OS/2 uses RGBTRIPLE colormap */
            //TRACEMS2(cinfo, 1, JTRC_BMP_OS2_MAPPED, (int) biWidth, (int) biHeight);
            break;
        case 24:			/* RGB image */
            //TRACEMS2(cinfo, 1, JTRC_BMP_OS2, (int) biWidth, (int) biHeight);
            break;
        default:
            //ERREXIT(cinfo, JERR_BMP_BADDEPTH);
            break;
        }
        break;
    case 40:  // When using "High color"
        biWidth = (INT32)GET_4B(bmpinfoheader, 4);
        biHeight = (INT32)GET_4B(bmpinfoheader, 8);
        biPlanes = GET_2B(bmpinfoheader, 12);
        source->bits_per_pixel = (int)GET_2B(bmpinfoheader, 14);
        biCompression = GET_4B(bmpinfoheader, 16);
        biXPelsPerMeter = GET_4B(bmpinfoheader, 24);
        biYPelsPerMeter = GET_4B(bmpinfoheader, 28);
        biClrUsed = GET_4B(bmpinfoheader, 32);

        if (biCompression != 0)
            return;

        if (biXPelsPerMeter > 0 && biYPelsPerMeter > 0) {
            /* Set JFIF density parameters from the BMP data */
            cinfo->X_density = (UINT16)(biXPelsPerMeter / 100); /* 100 cm per meter */
            cinfo->Y_density = (UINT16)(biYPelsPerMeter / 100);
            cinfo->density_unit = 2;	/* dots/cm */
        }
        break;
    case 64:  // This is the one we get on 32bit Windows 7 from GDI
        /* Decode Windows 3.x header (Microsoft calls this a BITMAPINFOHEADER) */
        /* or OS/2 2.x header, which has additional fields that we ignore */
        biWidth = GET_4B(bmpinfoheader, 4);
        biHeight = GET_4B(bmpinfoheader, 8);
        biPlanes = GET_2B(bmpinfoheader, 12);
        source->bits_per_pixel = (int)GET_2B(bmpinfoheader, 14);
        biCompression = GET_4B(bmpinfoheader, 16);
        biXPelsPerMeter = GET_4B(bmpinfoheader, 24);
        biYPelsPerMeter = GET_4B(bmpinfoheader, 28);
        biClrUsed = GET_4B(bmpinfoheader, 32);
        /* biSizeImage, biClrImportant fields are ignored */

        switch (source->bits_per_pixel) {
        case 8:			/* colormapped image */
            mapentrysize = 4;		/* Windows uses RGBQUAD colormap */
            // TRACEMS2(cinfo, 1, JTRC_BMP_MAPPED, (int) biWidth, (int) biHeight);
            break;
        case 24:			/* RGB image */
            // TRACEMS2(cinfo, 1, JTRC_BMP, (int) biWidth, (int) biHeight);
            break;
        case 32:			/* RGB image + Alpha Channel */
            // TRACEMS2(cinfo, 1, JTRC_BMP, (int) biWidth, (int) biHeight);
            break;
        default:
            return; //ERREXIT(cinfo, JERR_BMP_BADDEPTH);
            break;
        }
        if (biCompression != 0)
            return; //ERREXIT(cinfo, JERR_BMP_COMPRESSED);

        if (biXPelsPerMeter > 0 && biYPelsPerMeter > 0) {
            /* Set JFIF density parameters from the BMP data */
            cinfo->X_density = (UINT16)(biXPelsPerMeter / 100); /* 100 cm per meter */
            cinfo->Y_density = (UINT16)(biYPelsPerMeter / 100);
            cinfo->density_unit = 2;	/* dots/cm */
        }
        break;
    default:
        return; //ERREXIT(cinfo, JERR_BMP_BADHEADER);
        return;
    }

    if (biWidth <= 0 || biHeight <= 0)
        return; //ERREXIT(cinfo, JERR_BMP_EMPTY);
    if (biPlanes != 1)
        return; //ERREXIT(cinfo, JERR_BMP_BADPLANES);

    /* Compute distance to bitmap data --- will adjust for colormap below */
    bPad = bfOffBits - (headerSize + 14);

    /* Read the colormap, if any */
    if (mapentrysize > 0) {
        if (biClrUsed <= 0)
            biClrUsed = 256;		/* assume it's 256 */
        else if (biClrUsed > 256)
            return; //ERREXIT(cinfo, JERR_BMP_BADCMAP);
        /* Allocate space to store the colormap */
        source->colormap = (*cinfo->mem->alloc_sarray)
            ((j_common_ptr)cinfo, JPOOL_IMAGE,
                (JDIMENSION)biClrUsed, (JDIMENSION)3);
        /* and read it from the file */
        read_colormap(source, (int)biClrUsed, mapentrysize);
        /* account for size of colormap */
        bPad -= biClrUsed * mapentrysize;
    }

    /* Skip any remaining pad bytes */
    if (bPad < 0)			/* incorrect bfOffBits value? */
        return; //ERREXIT(cinfo, JERR_BMP_BADHEADER);
    // Not reading a file... so, just jump to the start..
    // No need to read_byte as an fseek hack.
    source->pub.read_offset = bfOffBits;
    //while (--bPad >= 0) {
    //  (void) read_byte(source);
    //}

    /* Compute row width in file, including padding to 4-byte boundary */
    if (source->bits_per_pixel == 16)
        row_width = (JDIMENSION)(biWidth * 2);
    else if (source->bits_per_pixel == 24)
        row_width = (JDIMENSION)(biWidth * 3);
    else if (source->bits_per_pixel == 32)
        row_width = (JDIMENSION)(biWidth * 4);
    else
        row_width = (JDIMENSION)biWidth;
    while ((row_width & 3) != 0) row_width++;
    source->row_width = row_width;

    /* Allocate space for inversion array, prepare for preload pass */
    source->whole_image = (*cinfo->mem->request_virt_sarray)
        ((j_common_ptr)cinfo, JPOOL_IMAGE, FALSE,
            row_width, (JDIMENSION)biHeight, (JDIMENSION)1);
    source->pub.get_pixel_rows = preload_image;

    /* Allocate one-row buffer for returned data */
    source->pub.buffer = (*cinfo->mem->alloc_sarray)
        ((j_common_ptr)cinfo, JPOOL_IMAGE,
            (JDIMENSION)(biWidth * 3), (JDIMENSION)1);
    source->pub.buffer_height = 1;

    cinfo->in_color_space = JCS_RGB;
    cinfo->input_components = 3;
    cinfo->data_precision = 8;
    cinfo->image_width = (JDIMENSION)biWidth;
    cinfo->image_height = (JDIMENSION)biHeight;

}


/*
 * Finish up at the end of the file.
 */

void finish_input_bmp(j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
{
    /* no work */
}


/*
 * The module selection routine for BMP format input.
 */
cjpeg_source_ptr jinit_read_bmp(j_compress_ptr cinfo)
{

    bmp_source_ptr source;
    /* Create module interface object */
    source = (bmp_source_ptr)
        (*cinfo->mem->alloc_small) ((j_common_ptr)cinfo, JPOOL_IMAGE,
            SIZEOF(bmp_source_struct));
    source->cinfo = cinfo;	/* make back link for subroutines */
    /* Fill in method ptrs, except get_pixel_rows which start_input sets */
    source->pub.start_input = start_input_bmp;
    source->pub.finish_input = finish_input_bmp;

    return (cjpeg_source_ptr)source;
}


/*
 * See: http://msdn.microsoft.com/en-us/library/dd145119%28VS.85%29.aspx
 * This function was copied from the MSDN example.
 * It was then modified to send the BMP data rather than save to disk
 * It was then modified to conver the BMP to JPEG and send
 * Now its realy big.
 */
bool LIB_IMAGE::bmp2jpeg(HBITMAP hBmp, HDC hDC, int quality, BYTE*& buf_jpeg, size_t& buf_jpeg_size)
{
    buf_jpeg = nullptr;
    buf_jpeg_size = 0;
    // data structures
    BITMAP bmp;
    PBITMAPINFO pbmi;
    WORD cClrBits;
    BITMAPFILEHEADER hdr;       // bitmap file-header 
    PBITMAPINFOHEADER pbih;     // bitmap info-header 
    LPBYTE lpBits;              // memory pointer 
    DWORD dwTotal;              // total count of bytes 
    DWORD cb;                   // incremental count of bytes 
    BYTE* hp;                   // byte pointer 
    DWORD s;
    BYTE* buf;

    // Convert to JPEG stuff
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cjpeg_source_ptr src_mgr;
    JDIMENSION num_scanlines;

    // Retrieve the bitmap's color format, width, and height. 
    if (!GetObject(hBmp, sizeof(BITMAP), (LPVOID)&bmp))
        // GetObject failed
        return false;

    // Convert the color format to a count of bits. 
    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
    if (cClrBits == 1)
        cClrBits = 1;
    else if (cClrBits <= 4)
        cClrBits = 4;
    else if (cClrBits <= 8)
        cClrBits = 8;
    else if (cClrBits <= 16)
        cClrBits = 16;
    else if (cClrBits <= 24)
        cClrBits = 24;
    else cClrBits = 32;

    // Allocate memory for the BITMAPINFO structure. (This structure 
    // contains a BITMAPINFOHEADER structure and an array of RGBQUAD 
    // data structures.) 
    if (cClrBits != 24)
        pbmi = (PBITMAPINFO)LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (size_t)(1 << cClrBits));

    // There is no RGBQUAD array for the 24-bit-per-pixel format. 
    else
        pbmi = (PBITMAPINFO)LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER));

    // Initialize the fields in the BITMAPINFO structure. 

    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pbmi->bmiHeader.biWidth = bmp.bmWidth;
    pbmi->bmiHeader.biHeight = bmp.bmHeight;
    pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
    pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;

    if (cClrBits < 24)
        pbmi->bmiHeader.biClrUsed = (1 << cClrBits);

    // If the bitmap is not compressed, set the BI_RGB flag. 
    pbmi->bmiHeader.biCompression = BI_RGB;

    // Compute the number of bytes in the array of color 
    // indices and store the result in biSizeImage. 
    pbmi->bmiHeader.biSizeImage = (pbmi->bmiHeader.biWidth + 7) / 8
        * pbmi->bmiHeader.biHeight * cClrBits;

    // Set biClrImportant to 0, indicating that all of the 
    // device colors are important. 
    pbmi->bmiHeader.biClrImportant = 0;


    pbih = (PBITMAPINFOHEADER)pbmi;
    lpBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);



    if (!lpBits) {
        // GlobalAlloc failed
        //printf("error: out of memory\n");
        return false;
    }

    // Retrieve the color table (RGBQUAD array) and the bits 
    // (array of palette indices) from the DIB. 
    if (!GetDIBits(hDC, hBmp, 0, (WORD)pbih->biHeight, lpBits, pbmi, DIB_RGB_COLORS)) {
        // GetDIBits failed
        //printf("error: GetDiBits failed\n");
        return false;
    }

    hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M" 
    // Compute the size of the entire file. 
    hdr.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) +
        pbih->biSize + pbih->biClrUsed
        * sizeof(RGBQUAD) + pbih->biSizeImage);
    hdr.bfReserved1 = 0;
    hdr.bfReserved2 = 0;

    // Compute the offset to the array of color indices. 
    hdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) +
        pbih->biSize + pbih->biClrUsed * sizeof(RGBQUAD);

    s = sizeof(BITMAPFILEHEADER);
    s = s + (sizeof(BITMAPINFOHEADER) + pbih->biClrUsed * sizeof(RGBQUAD));
    // Copy the array of color indices into the .BMP file. 
    dwTotal = cb = pbih->biSizeImage;
    hp = lpBits;

    s = s + ((int)cb);

    buf = (BYTE*)malloc(s * sizeof(BYTE));
    memcpy(buf, (LPVOID)&hdr, sizeof(BITMAPFILEHEADER));
    memcpy(buf + sizeof(BITMAPFILEHEADER), (LPVOID)pbih, sizeof(BITMAPINFOHEADER) + pbih->biClrUsed * sizeof(RGBQUAD));
    memcpy(buf + sizeof(BITMAPFILEHEADER) + (sizeof(BITMAPINFOHEADER) + pbih->biClrUsed * sizeof(RGBQUAD)), (LPSTR)hp, (int)cb);
    // Don't send it yet. Convert it to a JPEG.
    //packet_add_tlv_raw(resp, TLV_TYPE_DEV_SCREEN, buf, s);


    // JPEG conversion start here..'
    // buf is a pointer to a BMP in memory.

    /* Initialize JPEG parameters.
     * Much of this may be overridden later.
     * We need to provide some value for jpeg_set_defaults() to work.
     */

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    cinfo.in_color_space = JCS_RGB; /* arbitrary guess */
    jpeg_set_defaults(&cinfo);

    src_mgr = jinit_read_bmp(&cinfo); //Returns a cjpeg_source_ptr but is really bmp_source_ptr...

    src_mgr->input_buf = buf;
    src_mgr->read_offset = 0;
    /* Read the input file header to obtain file size & colorspace. */

    start_input_bmp(&cinfo, src_mgr);

    jpeg_default_colorspace(&cinfo);

    // TODO: accept options from the command line for grayscale and quality. 
    /* Go GRAYSCALE */
    //jpeg_set_colorspace(&cinfo, JCS_GRAYSCALE);
    /* Quality */
    jpeg_set_quality(&cinfo, quality, FALSE);

    // Write the compressed JPEG to memory: bug_jpeg
    jpeg_mem_dest(&cinfo, &buf_jpeg, &buf_jpeg_size);

    /* Start compressor */
    jpeg_start_compress(&cinfo, TRUE);

    /* Process data */
    while (cinfo.next_scanline < cinfo.image_height) {
        num_scanlines = (*src_mgr->get_pixel_rows) (&cinfo, src_mgr);
        (void)jpeg_write_scanlines(&cinfo, src_mgr->buffer, num_scanlines);
    }

    /* Finish compression and release memory */
    (*src_mgr->finish_input) (&cinfo, src_mgr);
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);

    // Free memory. 
    GlobalFree((HGLOBAL)lpBits);
    // This wasn't being free'ed before. Shouldn't you free all malloc calls? 
    free(buf);

    return true;
}