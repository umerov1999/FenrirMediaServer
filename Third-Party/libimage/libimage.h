#pragma once

#include <iostream>
#include <Windows.h>

namespace LIB_IMAGE
{
	enum class win_image_type
	{
		WIN_IMAGE_JPG = 0,
		WIN_IMAGE_PNG
	};

	struct win_image_size
	{
		win_image_size()
		{
			size_x = -1;
			size_y = -1;
		}
		win_image_size(int tsize_x, int tsize_y)
		{
			size_x = tsize_x;
			size_y = tsize_y;
		}
		~win_image_size()
		{
			size_x = -1;
			size_y = -1;
		}
		int size_x;
		int size_y;
	};

	class win_image
	{
	public:
		static std::string decompress_gzip(const std::string& str);
		win_image()
		{
			links = new size_t[1];
			*links = 0;
			hPicture = NULL;
		}
		win_image(HBITMAP Picture, win_image_size size)
		{
			links = new size_t[1];
			*links = 0;
			hPicture = Picture;
			if (Picture)
				image_size = size;
		}
		win_image(win_image& image)
		{
			if (image.is_has_image())
			{
				links = image.links;
				(*links)++;
				image_size = image.image_size;
				hPicture = image.hPicture;
			}
			else {
				links = new size_t[1];
				*links = 0;
			}
		}
		~win_image()
		{
			if (*links == 0 && is_has_image())
			{
				DeleteObject(hPicture);
				hPicture = NULL;
			}
			if (*links == 0) {
				delete links;
			}
			else {
				(*links)--;
			}
		}
		bool is_has_image() const
		{
			return hPicture != NULL;
		}
		win_image_size get_size() const
		{
			return image_size;
		}
		win_image& resize(int X, int Y)
		{
			if (is_has_image())
			{
				hPicture = (HBITMAP)CopyImage(hPicture, IMAGE_BITMAP, X, Y, LR_COPYDELETEORG);
				image_size.size_x = X;
				image_size.size_y = Y;
			}
			return *this;
		}
		win_image& operator=(const win_image& image)
		{
			if (image.is_has_image())
			{
				if (*links == 0) {
					delete links;
					links = NULL;
					if (hPicture) {
						DeleteObject(hPicture);
						hPicture = NULL;
					}
				}
				else {
					(*links)--;
				}
				links = image.links;
				(*links)++;
				image_size = image.image_size;
				hPicture = image.hPicture;
			}
			return *this;
		}
		void clone_to_image(win_image& image)
		{
			if (is_has_image())
			{
				if (*image.links == 0) {
					if (image.hPicture) {
						DeleteObject(image.hPicture);
						image.hPicture = NULL;
					}
				}
				else {
					(*image.links)--;
				}

				image.image_size = image_size;
				image.hPicture = (HBITMAP)CopyImage(hPicture, IMAGE_BITMAP, image_size.size_x, image_size.size_y, LR_COPYRETURNORG);
			}
		}
		win_image& make_RGB(HWND hwnd, DWORD Color);

		HBITMAP get_hBitmap(bool tnot_released = false)
		{
			if (tnot_released)
				*links = LONG_MAX;
			return hPicture;
		}

	private:
		size_t* links;
		HBITMAP hPicture;
		win_image_size image_size;
	};

	static inline void swap_rgb(unsigned char* rgb_buffer, int len)
	{
		unsigned char tmp = 0;
		int i = 0;
		for (i = 0; i < len; i += 3)
		{
			tmp = *(rgb_buffer + i);
			*(rgb_buffer + i) = *(rgb_buffer + i + 2);
			*(rgb_buffer + i + 2) = tmp;
		}
	}

	static inline void swap_rgba(unsigned char* rgba_buffer, int len)
	{
		unsigned char tmp = 0;
		int i = 0;
		for (i = 0; i < len; i += 4)
		{
			tmp = *(rgba_buffer + i);
			*(rgba_buffer + i) = *(rgba_buffer + i + 2);
			*(rgba_buffer + i + 2) = tmp;
		}
	}

	win_image PrepareImageFromBufferByType(HWND hwnd, const void* pDataBuffer, int nBufferSize, win_image_type Type, bool show_error_bitmap = true);
	win_image PrepareImageFromBufferAutoType(HWND hwnd, const void* pDataBuffer, int nBufferSize, bool show_error_bitmap = true);
	win_image PrepareImageFromSVG(HWND hwnd, int targetWidth, int targetHeight, const void* pDataBuffer, int nBufferSize, uint32_t bgColor = 0xffffffff);
	HBITMAP PrepareImageFromSVGResourceToIcon(HWND hwnd, int Res, int padding = 16, uint32_t bgColor = 0xffffffff);
	win_image PrepareImageFromSVGResource(HWND hwnd, int Res, int padding = 16, uint32_t bgColor = 0xffffffff);
	std::string GetDataFromResourceUtil(std::wstring ResType, int Res, HMODULE hMod = GetModuleHandleW(NULL));

	bool bmp2jpeg(HBITMAP hBmp, HDC hDC, int quality, BYTE*& buf_jpeg, size_t& buf_jpeg_size);
	bool bmp2png(HBITMAP hBmp, HDC hDC, BYTE*& buf_png, size_t& buf_png_size);
	//OnlyTest
	void CreateBMPFile(LPCTSTR pszFile, HBITMAP hBMP);
}