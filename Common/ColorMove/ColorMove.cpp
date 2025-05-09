#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <list>
#include <iomanip>
#include <tchar.h>
using namespace std;

#define MAKE_RGB( r, g, b )		( ( ( 255 ) << 24 ) | ( ( b ) << 16 ) | ( ( g ) << 8 ) | ( r ) )

template <typename I> std::string n2hexstr(I w, size_t hex_len = sizeof(I) << 1) {
	static const char* digits = "0123456789ABCDEF";
	std::string rc(hex_len, '0');
	for (size_t i = 0, j = (hex_len - 1) * 4; i < hex_len; ++i, j -= 4)
		rc[i] = digits[(w >> j) & 0x0f];
	return rc;
}

static int parseColor(string colorString) {
	if (colorString[0] == '#') {
		long color = stol(colorString.substr(1), nullptr, 16);
		return (int)color;
	}
	throw runtime_error("Unknown color");
}

inline unsigned char clamp(float v)
{
	if (v < 0.f)
		return 0;
	if (v > 255.f)
		return 255;
	return (unsigned char)v;
}

inline float clampf(float v)
{
	if (v < 0.f)
		return 0.f;
	if (v > 1.f)
		return 1.f;
	return v;
}

class hsv_model {
public:
	hsv_model() {
		h = 0.f;
		s = 0.f;
		v = 0.f;
	}
	float h;
	float s;
	float v;
};

class rgb_model {
public:
	rgb_model() {
		r = 0;
		g = 0;
		b = 0;
	}
	rgb_model(int32_t color) {
#ifdef BIG_ENDIAN_COLORS
		r = (color & 0xff);
		g = ((color >> 8) & 0xff);
		b = ((color >> 16) & 0xff);
#else
		r = ((color >> 16) & 0xff);
		g = ((color >> 8) & 0xff);
		b = (color & 0xff);
#endif
	}
	rgb_model(float fR, float fG, float fB) {
		r = (unsigned char)(fR * 255) & 0xff;
		g = (unsigned char)(fG * 255) & 0xff;
		b = (unsigned char)(fB * 255) & 0xff;
	}
	void toFloat(float &fR, float &fG, float &fB) const {
		fR = (float)r / 255.0f;
		fG = (float)g / 255.0f;
		fB = (float)b / 255.0f;
	}
	int32_t toInt32() const {
#ifdef BIG_ENDIAN_COLORS
		return (int32_t)((r << 16) | (g << 8) | b);
#else
		return (int32_t)(r | (g << 8) | (b << 16));
#endif
	}
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

hsv_model rgb2hsv(const rgb_model& color)
{
	hsv_model         out;
	float      min, max, delta;

	min = color.r < color.g ? color.r : color.g;
	min = min < color.b ? min : color.b;

	max = color.r > color.g ? color.r : color.g;
	max = max > color.b ? max : color.b;

	out.v = max / 255.0f;
	delta = max - min;
	if (delta < 0.00001f)
	{
		out.s = 0;
		out.h = 0;
		return out;
	}
	if (max > 0.0f) {
		out.s = (delta / max);
	}
	else {
		out.s = 0.0f;
		out.h = NAN;
		return out;
	}
	if (color.r >= max)
		out.h = (color.g - color.b) / delta;
	else
		if (color.g >= max)
			out.h = 2.0f + (color.b - color.r) / delta;
		else
			out.h = 4.0f + (color.r - color.g) / delta;

	out.h *= 60.0f;

	if (out.h < 0.0)
		out.h += 360.0f;

	return out;
}

std::string change_hsv_c(const rgb_model &color, const float fHue, const float fSat, const float fVal)
{
	rgb_model out;
	const float cosA = fSat * cos(fHue * 3.14159265f / 180);
	const float sinA = fSat * sin(fHue * 3.14159265f / 180);

	const float aThird = 1.0f / 3.0f;
	const float rootThird = sqrtf(aThird);
	const float oneMinusCosA = (1.0f - cosA);
	const float aThirdOfOneMinusCosA = aThird * oneMinusCosA;
	const float rootThirdTimesSinA = rootThird * sinA;
	const float plus = aThirdOfOneMinusCosA + rootThirdTimesSinA;
	const float minus = aThirdOfOneMinusCosA - rootThirdTimesSinA;

	float matrix[3][3] = {
		{   cosA + oneMinusCosA / 3.0f  , minus                         , plus                          },
		{   plus                        , cosA + aThirdOfOneMinusCosA   , minus                         },
		{   minus                       , plus                          , cosA + aThirdOfOneMinusCosA   }
	};
	out.r = clamp((color.r * matrix[0][0] + color.g * matrix[0][1] + color.b * matrix[0][2]) * fVal);
	out.g = clamp((color.r * matrix[1][0] + color.g * matrix[1][1] + color.b * matrix[1][2]) * fVal);
	out.b = clamp((color.r * matrix[2][0] + color.g * matrix[2][1] + color.b * matrix[2][2]) * fVal);
	return "#" + n2hexstr(out.r) + n2hexstr(out.g) + n2hexstr(out.b);
}

void pickColor() {
	cout << "Enter hex colors: from, to" << endl;
	string ft, tt;
	cin >> ft >> tt;
	const int32_t from = parseColor(ft);
	const int32_t to = parseColor(tt);

	while (true) {
		string ct;
		cout << "Enter hex colors: target color" << endl;
		cin >> ct;
		const int32_t color = parseColor(ct);

		hsv_model frm = rgb2hsv(rgb_model(from));
		hsv_model tom = rgb2hsv(rgb_model(to));
		hsv_model clrr = rgb2hsv(rgb_model(color));
		std::cout << endl << change_hsv_c(rgb_model(color), tom.h - frm.h, clampf(clrr.s + (tom.s - frm.s)), clampf(clrr.v + (tom.v - frm.v))) << endl;
	}
}

class ColorsRef {
public:
	ColorsRef(string name, hsv_model from, hsv_model to, map<string, string> colors) {
		this->name = name;
		this->from = from;
		this->to = to;
		this->colors = colors;
	}
	string name;
	hsv_model from;
	hsv_model to;
	map<string, string> colors;
};

void material() {
	while (true) {
		cout << "Enter hex colors: colorPrimary, colorSecondary" << endl;
		string colorPrimary, colorSecondary;
		cin >> colorPrimary >> colorSecondary;

		list<ColorsRef> refs;

		refs.push_back(ColorsRef("Day Primary", rgb2hsv(rgb_model(parseColor("#6750A4"))), rgb2hsv(rgb_model(parseColor(colorPrimary))), {
			{"colorOnPrimary", "#FFFFFF"},
			{"colorPrimaryInverse", "#D0BCFF"},
			{"colorPrimaryContainer", "#EADDFF"},
			{"colorOnPrimaryContainer", "#21005D"}
			}));

		refs.push_back(ColorsRef("Day Secondary", rgb2hsv(rgb_model(parseColor("#625B71"))), rgb2hsv(rgb_model(parseColor(colorSecondary))), {
			{"colorOnSecondary", "#FFFFFF"},
			{"colorSecondaryContainer", "#E8DEF8"},
			{"colorOnSecondaryContainer", "#1D192B"}
			}));

		refs.push_back(ColorsRef("Night Primary", rgb2hsv(rgb_model(parseColor("#D0BCFF"))), rgb2hsv(rgb_model(parseColor(colorPrimary))), {
			{"colorOnPrimary", "#381E72"},
			{"colorPrimaryInverse", "#6750A4"},
			{"colorPrimaryContainer", "#4F378B"},
			{"colorOnPrimaryContainer", "#EADDFF"}
			}));

		refs.push_back(ColorsRef("Night Secondary", rgb2hsv(rgb_model(parseColor("#CCC2DC"))), rgb2hsv(rgb_model(parseColor(colorSecondary))), {
			{"colorOnSecondary", "#332D41"},
			{"colorSecondaryContainer", "#4A4458"},
			{"colorOnSecondaryContainer", "#E8DEF8"}
			}));

		for (auto& i : refs) {
			cout << endl << i.name << endl;

			for (auto& s : i.colors) {
				rgb_model colorRgb = rgb_model(parseColor(s.second));
				hsv_model colorHSV = rgb2hsv(colorRgb);
				std::cout << "<item name=\"" << s.first << "\">" << change_hsv_c(rgb_model(parseColor(s.second)), i.to.h - i.from.h, clampf(colorHSV.s + (i.to.s - i.from.s)), clampf(colorHSV.v + (i.to.v - i.from.v))) << "</item>" << endl;
			}
		}
		cout << endl;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	locale::global(locale("ru_RU.UTF-8"));
	cout << "ColorMove:" << endl;
	cout << "Copyright (c) Umerov Artem, 2025" << endl << endl;

	cout << "Select mode: 0 - pick color, 1 - generate Material You colors: " << endl;
	int md;
	cin >> md;
	if (md == 0) {
		pickColor();
	}
	else {
		material();
	}
}