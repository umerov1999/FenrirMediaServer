#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <cstring>
#include <iomanip>
#include <cstdint>

#define SHA3_BUF_SIZE 524288

#if defined(_MSC_VER)
#define SHA3_CONST(x) x
#else
#define SHA3_CONST(x) x##L
#endif

#define SHA3_USE_KECCAK_FLAG 0x80000000
#define SHA3_CW(x) ((x) & (~SHA3_USE_KECCAK_FLAG))

#ifndef SHA3_ROTL64
#define SHA3_ROTL64(x, y) (((x) << (y)) | ((x) >> (64 - (y))))
#endif

#define SHA3_KECCAK_SPONGE_WORDS (200 / sizeof(uint64_t))

class SHA3
{
public:
	SHA3(uint16_t bitSize) {
		SHA3_Init(bitSize);
	}

	void SHA3_Init(uint16_t bitSize)
	{
		memset(&ctx, 0, sizeof(ctx));
		ctx.digest_length = bitSize;
		ctx.capacityWords = 2 * bitSize / (8 * sizeof(uint64_t));
	}

	void SHA3_Update(const void* bufIn, size_t len)
	{
		size_t old_tail = (8 - ctx.byteIndex) & 7;

		size_t words;
		size_t tail;
		size_t i;

		const uint8_t* buf = (uint8_t*)bufIn;

		//SHA3_ASSERT(ctx->byteIndex < 8);
		//SHA3_ASSERT(ctx->wordIndex < sizeof(ctx->u.s) / sizeof(ctx->u.s[0]));

		if (len < old_tail) {
			while (len--)
				ctx.saved |=
				(uint64_t)(*(buf++)) << ((ctx.byteIndex++) * 8);
			//SHA3_ASSERT(ctx->byteIndex < 8);
			return;
		}

		if (old_tail) {
			len -= old_tail;
			while (old_tail--)
				ctx.saved |=
				(uint64_t)(*(buf++)) << ((ctx.byteIndex++) * 8);

			ctx.u.s[ctx.wordIndex] ^= ctx.saved;
			//SHA3_ASSERT(ctx->byteIndex == 8);
			ctx.byteIndex = 0;
			ctx.saved = 0;
			if (++ctx.wordIndex ==
				(SHA3_KECCAK_SPONGE_WORDS - SHA3_CW(ctx.capacityWords))) {
				keccakf(ctx.u.s);
				ctx.wordIndex = 0;
			}
		}

		//SHA3_ASSERT(ctx->byteIndex == 0);

		words = len / sizeof(uint64_t);
		tail = len - words * sizeof(uint64_t);

		for (i = 0; i < words; i++, buf += sizeof(uint64_t)) {
			const uint64_t t = (uint64_t)(buf[0]) |
				((uint64_t)(buf[1]) << 8 * 1) |
				((uint64_t)(buf[2]) << 8 * 2) |
				((uint64_t)(buf[3]) << 8 * 3) |
				((uint64_t)(buf[4]) << 8 * 4) |
				((uint64_t)(buf[5]) << 8 * 5) |
				((uint64_t)(buf[6]) << 8 * 6) |
				((uint64_t)(buf[7]) << 8 * 7);

			//SHA3_ASSERT(memcmp(&t, buf, 8) == 0);

			ctx.u.s[ctx.wordIndex] ^= t;
			if (++ctx.wordIndex ==
				(SHA3_KECCAK_SPONGE_WORDS - SHA3_CW(ctx.capacityWords))) {
				keccakf(ctx.u.s);
				ctx.wordIndex = 0;
			}
		}

		//SHA3_ASSERT(ctx->byteIndex == 0 && tail < 8);
		while (tail--) {
			ctx.saved |= (uint64_t)(*(buf++)) << ((ctx.byteIndex++) * 8);
		}
		//SHA3_ASSERT(ctx->byteIndex < 8);
	}

	std::string SHA3_Final()
	{
		uint64_t t;

		t = (uint64_t)(((uint64_t)(0x02 | (1 << 2))) <<
			((ctx.byteIndex) * 8));

		ctx.u.s[ctx.wordIndex] ^= ctx.saved ^ t;

		ctx.u.s[SHA3_KECCAK_SPONGE_WORDS - SHA3_CW(ctx.capacityWords) - 1] ^=
			SHA3_CONST(0x8000000000000000UL);
		keccakf(ctx.u.s);

		for (size_t i = 0; i < SHA3_KECCAK_SPONGE_WORDS; i++) {
			const uint32_t t1 = (uint32_t)ctx.u.s[i];
			const uint32_t t2 =
				(uint32_t)((ctx.u.s[i] >> 16) >> 16);
			ctx.u.sb[i * 8 + 0] = (uint8_t)(t1);
			ctx.u.sb[i * 8 + 1] = (uint8_t)(t1 >> 8);
			ctx.u.sb[i * 8 + 2] = (uint8_t)(t1 >> 16);
			ctx.u.sb[i * 8 + 3] = (uint8_t)(t1 >> 24);
			ctx.u.sb[i * 8 + 4] = (uint8_t)(t2);
			ctx.u.sb[i * 8 + 5] = (uint8_t)(t2 >> 8);
			ctx.u.sb[i * 8 + 6] = (uint8_t)(t2 >> 16);
			ctx.u.sb[i * 8 + 7] = (uint8_t)(t2 >> 24);
		}

		std::string result;
		result.resize(2 * (ctx.digest_length / 8));
		for (uint64_t i = 0; i < ctx.digest_length / 8; i++)
			sprintf_s(result.data() + i * 2, 3, "%02x", ctx.u.sb[i]);

		SHA3_Init(ctx.digest_length);
		return result;
	}
private:
	static inline const uint64_t keccakf_rndc[24] = {
	SHA3_CONST(0x0000000000000001UL), SHA3_CONST(0x0000000000008082UL),
	SHA3_CONST(0x800000000000808aUL), SHA3_CONST(0x8000000080008000UL),
	SHA3_CONST(0x000000000000808bUL), SHA3_CONST(0x0000000080000001UL),
	SHA3_CONST(0x8000000080008081UL), SHA3_CONST(0x8000000000008009UL),
	SHA3_CONST(0x000000000000008aUL), SHA3_CONST(0x0000000000000088UL),
	SHA3_CONST(0x0000000080008009UL), SHA3_CONST(0x000000008000000aUL),
	SHA3_CONST(0x000000008000808bUL), SHA3_CONST(0x800000000000008bUL),
	SHA3_CONST(0x8000000000008089UL), SHA3_CONST(0x8000000000008003UL),
	SHA3_CONST(0x8000000000008002UL), SHA3_CONST(0x8000000000000080UL),
	SHA3_CONST(0x000000000000800aUL), SHA3_CONST(0x800000008000000aUL),
	SHA3_CONST(0x8000000080008081UL), SHA3_CONST(0x8000000000008080UL),
	SHA3_CONST(0x0000000080000001UL), SHA3_CONST(0x8000000080008008UL)
	};

	static inline const uint32_t keccakf_rotc[24] = {
	1, 3, 6, 10, 15, 21, 28, 36, 45, 55, 2, 14, 27, 41, 56, 8, 25, 43, 62,
	18, 39, 61, 20, 44
	};

	static inline const uint32_t keccakf_piln[24] = {
		10, 7, 11, 17, 18, 3, 5, 16, 8, 21, 24, 4, 15, 23, 19, 13, 12, 2, 20,
		14, 22, 9, 6, 1
	};

	static void keccakf(uint64_t s[25])
	{
		int i, j, round;
		uint64_t t, bc[5];
#define KECCAK_ROUNDS 24
		for (round = 0; round < KECCAK_ROUNDS; round++) {
			for (i = 0; i < 5; i++)
				bc[i] =
				s[i] ^ s[i + 5] ^ s[i + 10] ^ s[i + 15] ^ s[i + 20];

			for (i = 0; i < 5; i++) {
				t = bc[(i + 4) % 5] ^ SHA3_ROTL64(bc[(i + 1) % 5], 1);
				for (j = 0; j < 25; j += 5)
					s[j + i] ^= t;
			}

			t = s[1];
			for (i = 0; i < 24; i++) {
				j = keccakf_piln[i];
				bc[0] = s[j];
				s[j] = SHA3_ROTL64(t, keccakf_rotc[i]);
				t = bc[0];
			}

			for (j = 0; j < 25; j += 5) {
				for (i = 0; i < 5; i++)
					bc[i] = s[j + i];
				for (i = 0; i < 5; i++)
					s[j + i] ^=
					(~bc[(i + 1) % 5]) & bc[(i + 2) % 5];
			}

			s[0] ^= keccakf_rndc[round];
		}
	}

	struct SHA3_CTX {
		uint64_t saved;
		uint64_t byteIndex;
		uint64_t wordIndex;
		uint64_t capacityWords;

		union {
			uint64_t s[SHA3_KECCAK_SPONGE_WORDS];
			uint8_t sb[SHA3_KECCAK_SPONGE_WORDS * 8];
		} u;

		uint16_t digest_length;
	};

	SHA3_CTX ctx;
};

class SHA3_512: public SHA3
{
public:
	SHA3_512() : SHA3(512) {

	}

	static std::string sha3(const void* dat, size_t len) {
		SHA3_512 hash;
		hash.SHA3_Update(dat, len);
		return hash.SHA3_Final();
	}

	static std::string sha3(const std::string& input) {
		return sha3(input.c_str(), input.length());
	}

	static std::string sha3_file(std::FILE* file) {
		SHA3_512 hash;

		char* buff = new char[SHA3_BUF_SIZE];
		size_t len = 0;
		size_t uuuu = 0;
		while ((len = std::fread(buff, sizeof(char), SHA3_BUF_SIZE, file)) > 0) {
			hash.SHA3_Update(buff, len);

			uuuu += len;
		}
		delete[] buff;
		return hash.SHA3_Final();
	}

	static std::string sha3_file(const std::wstring& filename) {
		FILE* file = NULL;
		if (_wfopen_s(&file, filename.c_str(), L"rb") != 0) {
			return "error";
		}
		if (!file) {
			return "error";
		}
		std::string res = sha3_file(file);
		std::fclose(file);
		return res;
	}
};

class SHA3_384 : public SHA3
{
public:
	SHA3_384() : SHA3(384) {

	}

	static std::string sha3(const void* dat, size_t len) {
		SHA3_384 hash;
		hash.SHA3_Update(dat, len);
		return hash.SHA3_Final();
	}

	static std::string sha3(const std::string& input) {
		return sha3(input.c_str(), input.length());
	}

	static std::string sha3_file(std::FILE* file) {
		SHA3_384 hash;

		char* buff = new char[SHA3_BUF_SIZE];
		size_t len = 0;
		size_t uuuu = 0;
		while ((len = std::fread(buff, sizeof(char), SHA3_BUF_SIZE, file)) > 0) {
			hash.SHA3_Update(buff, len);

			uuuu += len;
		}
		delete[] buff;
		return hash.SHA3_Final();
	}

	static std::string sha3_file(const std::wstring& filename) {
		FILE* file = NULL;
		if (_wfopen_s(&file, filename.c_str(), L"rb") != 0) {
			return "error";
		}
		if (!file) {
			return "error";
		}
		std::string res = sha3_file(file);
		std::fclose(file);
		return res;
	}
};

class SHA3_256 : public SHA3
{
public:
	SHA3_256() : SHA3(256) {

	}

	static std::string sha3(const void* dat, size_t len) {
		SHA3_256 hash;
		hash.SHA3_Update(dat, len);
		return hash.SHA3_Final();
	}

	static std::string sha3(const std::string& input) {
		return sha3(input.c_str(), input.length());
	}

	static std::string sha3_file(std::FILE* file) {
		SHA3_256 hash;

		char* buff = new char[SHA3_BUF_SIZE];
		size_t len = 0;
		size_t uuuu = 0;
		while ((len = std::fread(buff, sizeof(char), SHA3_BUF_SIZE, file)) > 0) {
			hash.SHA3_Update(buff, len);

			uuuu += len;
		}
		delete[] buff;
		return hash.SHA3_Final();
	}

	static std::string sha3_file(const std::wstring& filename) {
		FILE* file = NULL;
		if (_wfopen_s(&file, filename.c_str(), L"rb") != 0) {
			return "error";
		}
		if (!file) {
			return "error";
		}
		std::string res = sha3_file(file);
		std::fclose(file);
		return res;
	}
};