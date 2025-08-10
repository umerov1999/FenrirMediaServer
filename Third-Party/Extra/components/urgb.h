#pragma once
#include <afxwin.h>
#include <iostream>

class URGB {
public:
	URGB() {
		r = 0;
		g = 0;
		b = 0;
	}
	URGB(int red, int green, int blue) {
		r = (char)red;
		g = (char)green;
		b = (char)blue;
	}
	COLORREF operator()() {
		return RGB(r, g, b);
	}
	void operator=(const URGB& clr) {
		r = clr.r;
		g = clr.g;
		b = clr.b;
	}
	unsigned char r;
	unsigned char g;
	unsigned char b;
};