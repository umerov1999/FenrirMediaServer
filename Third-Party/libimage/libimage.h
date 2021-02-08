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
		win_image()
		{
			not_released = false;
			hPicture = NULL;
		}
		win_image(HBITMAP Picture, win_image_size size)
		{
			not_released = false;
			hPicture = Picture;
			if (Picture)
				image_size = size;
		}
		win_image(win_image& image)
		{
			if (image.is_has_image())
			{
				image.not_released = true;
				image_size = image.image_size;
				hPicture = image.hPicture;
			}
		}
		~win_image()
		{
			if (not_released == false && is_has_image())
			{
				DeleteObject(hPicture);
				hPicture = NULL;
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
		win_image& operator=(win_image& image)
		{
			if (image.is_has_image())
			{
				if (hPicture)
					DeleteObject(hPicture);
				image.not_released = true;
				image_size = image.image_size;
				hPicture = image.hPicture;
			}
			return *this;
		}
		void clone_to_image(win_image& image)
		{
			if (is_has_image())
			{
				image.image_size = image_size;
				image.hPicture = (HBITMAP)CopyImage(hPicture, IMAGE_BITMAP, image_size.size_x, image_size.size_y, LR_COPYRETURNORG);
			}
		}
		win_image& make_RGB(HWND hwnd, DWORD Color);

		HBITMAP get_hBitmap(bool tnot_released = false)
		{
			if (tnot_released)
				not_released = true;
			return hPicture;
		}

	private:
		bool not_released;
		HBITMAP hPicture;
		win_image_size image_size;
	};

	win_image PrepareImageFromBufferByType(HWND hwnd, const void* pDataBuffer, int nBufferSize, win_image_type Type);
	win_image PrepareImageFromBufferAutoType(HWND hwnd, const void* pDataBuffer, int nBufferSize);
	std::string GetDataFromResourceUtil(std::wstring ResType, int Res, HMODULE hMod = GetModuleHandleW(NULL));
}