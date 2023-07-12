#pragma once

#include <string>
#include <iostream>
#include <cstring>
#include <cstdint>

#define MD5_BUF_SIZE 524288
#define MD5_F(x, y, z)   ((z) ^ ((x) & ((y) ^ (z))))
#define MD5_G(x, y, z)   ((y) ^ ((z) & ((x) ^ (y))))
#define MD5_H(x, y, z)   ((x) ^ (y) ^ (z))
#define MD5_I(x, y, z)   ((y) ^ ((x) | ~(z)))
#define MD5_STEP(f, a, b, c, d, x, t, s) \
		(a) += f((b), (c), (d)) + (x) + (t); \
		(a) = (((a) << (s)) | (((a) & 0xffffffff) >> (32 - (s)))); \
		(a) += (b);

#if defined(_WIN32) || defined(_WIN64) || defined(__ARM_NEON__) || defined(__aarch64__)
#define MD5_SET(n) \
			(*(uint32_t *)&ptr[(n) * 4])
#define MD5_GET(n) \
			MD5_SET(n)
#else
#define MD5_SET(n) \
			(ctx->block[(n)] = \
			(uint32_t)ptr[(n) * 4] | \
			((uint32_t)ptr[(n) * 4 + 1] << 8) | \
			((MD5_uint32_tu32)ptr[(n) * 4 + 2] << 16) | \
			((uint32_t)ptr[(n) * 4 + 3] << 24))
#define MD5_GET(n) \
			(ctx->block[(n)])
#endif

class MD5
{
public:
    MD5() {
		MD5_Init();
    }

	void MD5_Init() {
		md5_ctx.a = 0x67452301;
		md5_ctx.b = 0xefcdab89;
		md5_ctx.c = 0x98badcfe;
		md5_ctx.d = 0x10325476;

		md5_ctx.lo = 0;
		md5_ctx.hi = 0;
	}
	void MD5_Update(const void* data, unsigned long size) {
		uint32_t saved_lo;
		unsigned long used, free;

		saved_lo = md5_ctx.lo;
		if ((md5_ctx.lo = (saved_lo + size) & 0x1fffffff) < saved_lo)
			md5_ctx.hi++;
		md5_ctx.hi += size >> 29;
		used = saved_lo & 0x3f;

		if (used) {
			free = 64 - used;
			if (size < free) {
				memcpy(&md5_ctx.buffer[used], data, size);
				return;
			}

			memcpy(&md5_ctx.buffer[used], data, free);
			data = (unsigned char*)data + free;
			size -= free;
			body(&md5_ctx, md5_ctx.buffer, 64);
		}

		if (size >= 64) {
			data = body(&md5_ctx, data, size & ~(unsigned long)0x3f);
			size &= 0x3f;
		}

		memcpy(md5_ctx.buffer, data, size);
	}

	void MD5_Final(unsigned char* result) {
		unsigned long used, free;
		used = md5_ctx.lo & 0x3f;
		md5_ctx.buffer[used++] = 0x80;
		free = 64 - used;

		if (free < 8) {
			memset(&md5_ctx.buffer[used], 0, free);
			body(&md5_ctx, md5_ctx.buffer, 64);
			used = 0;
			free = 64;
		}

		memset(&md5_ctx.buffer[used], 0, free - 8);

		md5_ctx.lo <<= 3;
		md5_ctx.buffer[56] = md5_ctx.lo;
		md5_ctx.buffer[57] = md5_ctx.lo >> 8;
		md5_ctx.buffer[58] = md5_ctx.lo >> 16;
		md5_ctx.buffer[59] = md5_ctx.lo >> 24;
		md5_ctx.buffer[60] = md5_ctx.hi;
		md5_ctx.buffer[61] = md5_ctx.hi >> 8;
		md5_ctx.buffer[62] = md5_ctx.hi >> 16;
		md5_ctx.buffer[63] = md5_ctx.hi >> 24;
		body(&md5_ctx, md5_ctx.buffer, 64);
		result[0] = md5_ctx.a;
		result[1] = md5_ctx.a >> 8;
		result[2] = md5_ctx.a >> 16;
		result[3] = md5_ctx.a >> 24;
		result[4] = md5_ctx.b;
		result[5] = md5_ctx.b >> 8;
		result[6] = md5_ctx.b >> 16;
		result[7] = md5_ctx.b >> 24;
		result[8] = md5_ctx.c;
		result[9] = md5_ctx.c >> 8;
		result[10] = md5_ctx.c >> 16;
		result[11] = md5_ctx.c >> 24;
		result[12] = md5_ctx.d;
		result[13] = md5_ctx.d >> 8;
		result[14] = md5_ctx.d >> 16;
		result[15] = md5_ctx.d >> 24;
		memset(&md5_ctx, 0, sizeof(md5_ctx));
		MD5_Init();
	}

	static std::string md5(const void* dat, size_t len) {
		std::string res;
		unsigned char out[16];
		md5bin(dat, len, out);
		for (size_t i = 0; i < 16; ++i) {
			res.push_back(hb2hex(out[i] >> 4));
			res.push_back(hb2hex(out[i]));
		}
		return res;
	}

	static std::string md5(const std::string& dat) {
		return md5(dat.c_str(), dat.length());
	}

	static std::string md5file(std::FILE* file) {
		MD5 c;

		char* buff = new char[MD5_BUF_SIZE];
		unsigned char out[16];
		size_t len = 0;
		while ((len = std::fread(buff, sizeof(char), MD5_BUF_SIZE, file)) > 0) {
			c.MD5_Update(buff, (unsigned long)len);
		}
		delete[] buff;
		c.MD5_Final(out);

		std::string res;
		for (size_t i = 0; i < 16; ++i) {
			res.push_back(hb2hex(out[i] >> 4));
			res.push_back(hb2hex(out[i]));
		}
		return res;
	}

	static std::string md5file(const std::string& filename) {
		FILE* file = NULL;
		if (fopen_s(&file, filename.c_str(), "rb") != 0) {
			return "error";
		}
		if (!file) {
			return "error";
		}
		std::string res = md5file(file);
		fclose(file);
		return res;
	}

	static std::string md5file(const std::wstring& filename) {
		FILE* file = NULL;
		if (_wfopen_s(&file, filename.c_str(), L"rb") != 0) {
			return "error";
		}
		if (!file) {
			return "error";
		}
		std::string res = md5file(file);
		fclose(file);
		return res;
	}

private:
	struct MD5_CTX {
		uint32_t lo, hi;
		uint32_t a, b, c, d;
		unsigned char buffer[64];
		uint32_t block[16];
	};

	MD5_CTX md5_ctx;

	static char hb2hex(unsigned char hb) {
		hb = hb & 0xF;
		return hb < 10 ? '0' + hb : hb - 10 + 'a';
	}

	/* Return Calculated raw result(always little-endian), the size is always 16 */
	static void md5bin(const void* dat, size_t len, unsigned char out[16]) {
		MD5 c;
		c.MD5_Update(dat, (unsigned long)len);
		c.MD5_Final(out);
	}

	static const void* body(MD5_CTX* ctx, const void* data, unsigned long size) {
		const unsigned char* ptr;
		uint32_t a, b, c, d;
		uint32_t saved_a, saved_b, saved_c, saved_d;

		ptr = (const unsigned char*)data;

		a = ctx->a;
		b = ctx->b;
		c = ctx->c;
		d = ctx->d;

		do {
			saved_a = a;
			saved_b = b;
			saved_c = c;
			saved_d = d;

			MD5_STEP(MD5_F, a, b, c, d, MD5_SET(0), 0xd76aa478, 7)
				MD5_STEP(MD5_F, d, a, b, c, MD5_SET(1), 0xe8c7b756, 12)
				MD5_STEP(MD5_F, c, d, a, b, MD5_SET(2), 0x242070db, 17)
				MD5_STEP(MD5_F, b, c, d, a, MD5_SET(3), 0xc1bdceee, 22)
				MD5_STEP(MD5_F, a, b, c, d, MD5_SET(4), 0xf57c0faf, 7)
				MD5_STEP(MD5_F, d, a, b, c, MD5_SET(5), 0x4787c62a, 12)
				MD5_STEP(MD5_F, c, d, a, b, MD5_SET(6), 0xa8304613, 17)
				MD5_STEP(MD5_F, b, c, d, a, MD5_SET(7), 0xfd469501, 22)
				MD5_STEP(MD5_F, a, b, c, d, MD5_SET(8), 0x698098d8, 7)
				MD5_STEP(MD5_F, d, a, b, c, MD5_SET(9), 0x8b44f7af, 12)
				MD5_STEP(MD5_F, c, d, a, b, MD5_SET(10), 0xffff5bb1, 17)
				MD5_STEP(MD5_F, b, c, d, a, MD5_SET(11), 0x895cd7be, 22)
				MD5_STEP(MD5_F, a, b, c, d, MD5_SET(12), 0x6b901122, 7)
				MD5_STEP(MD5_F, d, a, b, c, MD5_SET(13), 0xfd987193, 12)
				MD5_STEP(MD5_F, c, d, a, b, MD5_SET(14), 0xa679438e, 17)
				MD5_STEP(MD5_F, b, c, d, a, MD5_SET(15), 0x49b40821, 22)
				MD5_STEP(MD5_G, a, b, c, d, MD5_GET(1), 0xf61e2562, 5)
				MD5_STEP(MD5_G, d, a, b, c, MD5_GET(6), 0xc040b340, 9)
				MD5_STEP(MD5_G, c, d, a, b, MD5_GET(11), 0x265e5a51, 14)
				MD5_STEP(MD5_G, b, c, d, a, MD5_GET(0), 0xe9b6c7aa, 20)
				MD5_STEP(MD5_G, a, b, c, d, MD5_GET(5), 0xd62f105d, 5)
				MD5_STEP(MD5_G, d, a, b, c, MD5_GET(10), 0x02441453, 9)
				MD5_STEP(MD5_G, c, d, a, b, MD5_GET(15), 0xd8a1e681, 14)
				MD5_STEP(MD5_G, b, c, d, a, MD5_GET(4), 0xe7d3fbc8, 20)
				MD5_STEP(MD5_G, a, b, c, d, MD5_GET(9), 0x21e1cde6, 5)
				MD5_STEP(MD5_G, d, a, b, c, MD5_GET(14), 0xc33707d6, 9)
				MD5_STEP(MD5_G, c, d, a, b, MD5_GET(3), 0xf4d50d87, 14)
				MD5_STEP(MD5_G, b, c, d, a, MD5_GET(8), 0x455a14ed, 20)
				MD5_STEP(MD5_G, a, b, c, d, MD5_GET(13), 0xa9e3e905, 5)
				MD5_STEP(MD5_G, d, a, b, c, MD5_GET(2), 0xfcefa3f8, 9)
				MD5_STEP(MD5_G, c, d, a, b, MD5_GET(7), 0x676f02d9, 14)
				MD5_STEP(MD5_G, b, c, d, a, MD5_GET(12), 0x8d2a4c8a, 20)
				MD5_STEP(MD5_H, a, b, c, d, MD5_GET(5), 0xfffa3942, 4)
				MD5_STEP(MD5_H, d, a, b, c, MD5_GET(8), 0x8771f681, 11)
				MD5_STEP(MD5_H, c, d, a, b, MD5_GET(11), 0x6d9d6122, 16)
				MD5_STEP(MD5_H, b, c, d, a, MD5_GET(14), 0xfde5380c, 23)
				MD5_STEP(MD5_H, a, b, c, d, MD5_GET(1), 0xa4beea44, 4)
				MD5_STEP(MD5_H, d, a, b, c, MD5_GET(4), 0x4bdecfa9, 11)
				MD5_STEP(MD5_H, c, d, a, b, MD5_GET(7), 0xf6bb4b60, 16)
				MD5_STEP(MD5_H, b, c, d, a, MD5_GET(10), 0xbebfbc70, 23)
				MD5_STEP(MD5_H, a, b, c, d, MD5_GET(13), 0x289b7ec6, 4)
				MD5_STEP(MD5_H, d, a, b, c, MD5_GET(0), 0xeaa127fa, 11)
				MD5_STEP(MD5_H, c, d, a, b, MD5_GET(3), 0xd4ef3085, 16)
				MD5_STEP(MD5_H, b, c, d, a, MD5_GET(6), 0x04881d05, 23)
				MD5_STEP(MD5_H, a, b, c, d, MD5_GET(9), 0xd9d4d039, 4)
				MD5_STEP(MD5_H, d, a, b, c, MD5_GET(12), 0xe6db99e5, 11)
				MD5_STEP(MD5_H, c, d, a, b, MD5_GET(15), 0x1fa27cf8, 16)
				MD5_STEP(MD5_H, b, c, d, a, MD5_GET(2), 0xc4ac5665, 23)
				MD5_STEP(MD5_I, a, b, c, d, MD5_GET(0), 0xf4292244, 6)
				MD5_STEP(MD5_I, d, a, b, c, MD5_GET(7), 0x432aff97, 10)
				MD5_STEP(MD5_I, c, d, a, b, MD5_GET(14), 0xab9423a7, 15)
				MD5_STEP(MD5_I, b, c, d, a, MD5_GET(5), 0xfc93a039, 21)
				MD5_STEP(MD5_I, a, b, c, d, MD5_GET(12), 0x655b59c3, 6)
				MD5_STEP(MD5_I, d, a, b, c, MD5_GET(3), 0x8f0ccc92, 10)
				MD5_STEP(MD5_I, c, d, a, b, MD5_GET(10), 0xffeff47d, 15)
				MD5_STEP(MD5_I, b, c, d, a, MD5_GET(1), 0x85845dd1, 21)
				MD5_STEP(MD5_I, a, b, c, d, MD5_GET(8), 0x6fa87e4f, 6)
				MD5_STEP(MD5_I, d, a, b, c, MD5_GET(15), 0xfe2ce6e0, 10)
				MD5_STEP(MD5_I, c, d, a, b, MD5_GET(6), 0xa3014314, 15)
				MD5_STEP(MD5_I, b, c, d, a, MD5_GET(13), 0x4e0811a1, 21)
				MD5_STEP(MD5_I, a, b, c, d, MD5_GET(4), 0xf7537e82, 6)
				MD5_STEP(MD5_I, d, a, b, c, MD5_GET(11), 0xbd3af235, 10)
				MD5_STEP(MD5_I, c, d, a, b, MD5_GET(2), 0x2ad7d2bb, 15)
				MD5_STEP(MD5_I, b, c, d, a, MD5_GET(9), 0xeb86d391, 21)

				a += saved_a;
			b += saved_b;
			c += saved_c;
			d += saved_d;

			ptr += 64;
		} while (size -= 64);

		ctx->a = a;
		ctx->b = b;
		ctx->c = c;
		ctx->d = d;

		return ptr;
	}
};