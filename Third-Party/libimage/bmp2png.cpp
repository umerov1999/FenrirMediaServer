#include <iostream>
#include "libimage.h"
#include "png.h"
using namespace std;
#define BitmapWidth(cx, bpp)  (((cx * bpp + 31) & ~31) >> 3)
class libpng_inmem_write_struct {
public:
    libpng_inmem_write_struct() {
        pngBfr = nullptr;
        pngSiz = 0;
    }
    BYTE* pngBfr;
    size_t pngSiz;
};

static void wrtBgPng(png_structp pngWrtPtr, png_bytep data, png_size_t length) {
    libpng_inmem_write_struct* p = (libpng_inmem_write_struct*)png_get_io_ptr(pngWrtPtr);
    p->pngBfr = (unsigned char*)realloc(p->pngBfr, p->pngSiz + length);
    if (!p->pngBfr) throw runtime_error("The PNG write memory did not correctly allocate.");
    memmove(p->pngBfr + p->pngSiz, data, length);
    p->pngSiz += length;
}

bool LIB_IMAGE::bmp2png(HBITMAP hBmp, HDC hDC, BYTE*& buf_png, size_t& buf_png_size) {
    buf_png = nullptr;
    buf_png_size = 0;
    BITMAP bmp;
    png_byte** row_pointers = nullptr;
    // Retrieve the bitmap's color format, width, and height. 
    if (!GetObject(hBmp, sizeof(BITMAP), (LPVOID)&bmp))
        // GetObject failed
        return false;
    WORD cClrBits = 0;
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
    bool has_alpha = cClrBits == 32;
    int color_type = has_alpha ? PNG_COLOR_TYPE_RGB_ALPHA : PNG_COLOR_TYPE_RGB;

    BITMAPINFO bitmap_info;
    memset(&bitmap_info, 0, sizeof(bitmap_info));
    bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
    bitmap_info.bmiHeader.biWidth = bmp.bmWidth;
    bitmap_info.bmiHeader.biHeight = bmp.bmHeight;
    bitmap_info.bmiHeader.biPlanes = 1;
    bitmap_info.bmiHeader.biBitCount = (has_alpha ? 4 : 3) * 8;
    bitmap_info.bmiHeader.biCompression = BI_RGB;
    bitmap_info.bmiHeader.biClrUsed = 0;
    bitmap_info.bmiHeader.biClrImportant = 0;
    unsigned char* bitmap_buffer = new unsigned char[BitmapWidth(bmp.bmWidth, (has_alpha ? 4 : 3) * 8) * bmp.bmHeight];
    if (!GetDIBits(hDC, hBmp, 0, bmp.bmHeight, bitmap_buffer, &bitmap_info, DIB_RGB_COLORS))
    {
        delete[] bitmap_buffer;
        return false;
    }

    libpng_inmem_write_struct p_io;

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) {
        delete[] bitmap_buffer;
        return false;
    }
    png_infop png_info;
    if (!(png_info = png_create_info_struct(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &png_info);
        delete[] bitmap_buffer;
        return false;
    }
    png_set_IHDR(png_ptr, png_info, bmp.bmWidth, bmp.bmHeight, 8, color_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    
    row_pointers = new png_byte*[bmp.bmHeight];
    for (unsigned int nRowNum = 0; nRowNum < bmp.bmHeight; nRowNum++) {
        row_pointers[bmp.bmHeight - nRowNum - 1] = bitmap_buffer + nRowNum * BitmapWidth(bmp.bmWidth, (has_alpha ? 4 : 3) * 8);
        if (has_alpha) {
            swap_rgba(row_pointers[bmp.bmHeight - nRowNum - 1], bmp.bmWidth * 4);
        }
        else {
            swap_rgb(row_pointers[bmp.bmHeight - nRowNum - 1], bmp.bmWidth * 3);
        }
    }
    png_init_io(png_ptr, (png_FILE_p)&p_io);
    png_set_rows(png_ptr, png_info, &row_pointers[0]);
    png_set_write_fn(png_ptr, &p_io, wrtBgPng, NULL);
    png_write_png(png_ptr, png_info, PNG_TRANSFORM_IDENTITY, NULL);
    delete[] row_pointers;
    delete[] bitmap_buffer;
    png_destroy_write_struct(&png_ptr, &png_info);
    buf_png = p_io.pngBfr;
    buf_png_size = p_io.pngSiz;
    return true;
}