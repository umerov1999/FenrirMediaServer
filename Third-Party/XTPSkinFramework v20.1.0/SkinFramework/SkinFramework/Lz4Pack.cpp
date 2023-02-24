#include <iostream>
#include <vector>
#include "../GraphicLibrary/lz4/lz4.h"
#include "../GraphicLibrary/lz4/lz4hc.h"
#include "WSTRUtils.h"
#include "Lz4Pack.h"
using namespace std;
using namespace ToolkitProWSTRUtils;
using namespace Lz4Pack;

template<typename T>
static inline bool decompressLz4(const void* compressed_buf, int compressed_size, vector<T>& uncompressed) {
    return LZ4_decompress_safe((const char*)compressed_buf, (char*)uncompressed.data(), compressed_size, (int)uncompressed.size() * sizeof(T)) == (int)(uncompressed.size() * sizeof(T));
}

static inline bool decompressLz4(const void* compressed_buf, const void* decompressed_buf, const DecomressorEntry& entry) {
    return LZ4_decompress_safe((const char*)compressed_buf + entry.offset, (char*)decompressed_buf, entry.compressedSize, entry.originalSize) == entry.originalSize;
}

bool Decompressor::read_headers() {
    if (!PackerHeader::check_header_size(size)) {
        return false;
    }
    PackerHeader hdr;
    memcpy(&hdr, buf, sizeof(PackerHeader));
    if (!hdr.is_valid_header(size)) {
        return false;
    }
    vector<PackerEntry> entries(hdr.count);
    int offset = sizeof(PackerHeader);
    if (!decompressLz4((char*)buf + offset, hdr.entries_compressed_size, entries)) {
        return false;
    }
    offset += hdr.entries_compressed_size;

    for (int i = 0; i < hdr.count; i++) {
        const PackerEntry& entry = entries[i];
        if (entry.isDir) {
            Dirs.push_back(UTF8_to_wchar(entry.file_name));
            continue;
        }
        if (offset + entry.compressed_size > size) {
            return false;
        }
        Files[UTF8_to_wchar(entry.file_name)] = DecomressorEntry(entry.original_size, entry.compressed_size, offset);
        offset += entry.compressed_size;
    }
    return true;
}

string Decompressor::operator[](const DecomressorEntry& entry) const {
    string Decompressed;
    Decompressed.resize(entry.originalSize);

    if (!decompressLz4((const char*)buf, (char*)Decompressed.data(), entry)) {
        return string();
    }
    return Decompressed;
}

bool Decompressor::has(const wstring& name_entry) const {
    return Files.exist(name_entry);
}

string Decompressor::operator[](const wstring& name_entry) const {
    const DecomressorEntry& entry = Files[name_entry];
    string Decompressed;
    Decompressed.resize(entry.originalSize);

    if (!decompressLz4((const char*)buf, (char*)Decompressed.data(), entry)) {
        return string();
    }
    return Decompressed;
}