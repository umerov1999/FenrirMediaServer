#include "pch.h"
#include <iostream>
#include <vector>
#include "lz4.h"
#include "lz4hc.h"
#include "WSTRUtils.h"
#include "Lz4Pack.h"
using namespace std;
using namespace WSTRUtils;
using namespace Lz4Pack;

template<typename T>
static inline int compressLz4(const vector<T>& uncompressed, vector<char>& compressed) {
    int originalSize = (int)uncompressed.size() * sizeof(T);
    int CompressedBound = LZ4_compressBound((int)originalSize);
    compressed.resize(CompressedBound);
    return LZ4_compress_HC((const char*)uncompressed.data(), (char*)compressed.data(), (int)uncompressed.size() * sizeof(T), CompressedBound, LZ4HC_CLEVEL_MAX);
}

template<typename T>
static inline bool decompressLz4(const void* compressed_buf, int compressed_size, vector<T>& uncompressed) {
    return LZ4_decompress_safe((const char*)compressed_buf, (char*)uncompressed.data(), compressed_size, (int)uncompressed.size() * sizeof(T)) == uncompressed.size() * sizeof(T);
}

static inline bool decompressLz4(const void* compressed_buf, const void* decompressed_buf, const DecomressorEntry& entry) {
    return LZ4_decompress_safe((const char*)compressed_buf + entry.offset, (char*)decompressed_buf, entry.compressedSize, entry.originalSize) == entry.originalSize;
}

ComressorEntry::ComressorEntry(const vector<char>& uncompressed) {
    isDir = false;
    originalSize = (int)uncompressed.size();
    compressedSize = compressLz4(uncompressed, compressed);
    if (compressedSize <= 0) {
        compressedSize = -1;
    }
}

static wstring fixFilePathSlashes(const wstring& filePath)
{
    wstring ret = filePath;
    for (auto& i : ret)
    {
        if (i == L'\\')
            i = L'/';
    }
    return ret;
}

bool Compressor::add(const wstring& file_name, const vector<char>& uncompressed) {
    wstring flname = fixFilePathSlashes(file_name);
    if (flname.find_last_of(L'/') != wstring::npos)
        CheckAndMakeDirecories(flname.substr(0, flname.find_last_of(L'/')));
    string utf8FileName = wchar_to_UTF8(flname);
    if (utf8FileName.length() >= FILENAME_LEN || utf8FileName.length() <= 0) {
        return false;
    }
    if (wrap.exist(utf8FileName)) {
        return false;
    }
    ComressorEntry entry(uncompressed);
    if (entry.compressedSize <= 0) {
        return false;
    }
    wrap[utf8FileName] = entry;
    return true;
}

void Compressor::CheckAndMakeDirecories(const std::wstring& PathIgnoreFile)
{
    wstring PathIgnoreFl = PathIgnoreFile;
    wstring Temp;
    do
    {
        size_t Pos = PathIgnoreFl.find(L'/');
        if (Pos == wstring::npos)
        {
            if (Temp.length() > 0)
                Temp += (L"/" + PathIgnoreFl);
            else
                Temp += PathIgnoreFl;
            PathIgnoreFl.clear();
            if (!wrap.exist(wchar_to_UTF8(Temp)))
                addDir(Temp);
        }
        else
        {
            if (Temp.length() > 0)
                Temp += (L"/" + PathIgnoreFl.substr(0, Pos));
            else
                Temp = PathIgnoreFl.substr(0, Pos);
            PathIgnoreFl = PathIgnoreFl.substr(Pos + 1);

            if (!wrap.exist(wchar_to_UTF8(Temp)))
                addDir(Temp);
        }
    } while (!PathIgnoreFl.empty());
}

bool Compressor::addDir(const wstring& file_name) {
    wstring flname = fixFilePathSlashes(file_name);
    string utf8FileName = wchar_to_UTF8(flname);
    if (utf8FileName.length() >= FILENAME_LEN || utf8FileName.length() <= 0) {
        return false;
    }
    if (wrap.exist(utf8FileName)) {
        return false;
    }
    ComressorEntry entry;
    wrap[utf8FileName] = entry;
    return true;
}
bool Compressor::add(const wstring& file_name, const string& uncompressed) {
    vector<char> dt(uncompressed.size());
    memcpy(dt.data(), uncompressed.data(), uncompressed.length());
    return add(file_name, dt);
}
bool Compressor::build(const wstring& arch) {
    FILE* fl = NULL;
    if (_wfopen_s(&fl, arch.c_str(), L"wb") != 0) {
        return false;
    }
    if (!fl) {
        return false;
    }
    vector<PackerEntry> entries;
    vector<char> compressed_entries;
    
    for (auto& i : wrap) {
        PackerEntry entry;
        if (!i.get_value().isDir) {
            entry.compressed_size = i.get_value().compressedSize;
            entry.original_size = i.get_value().originalSize;
            entry.isDir = false;
        }
        else {
            entry.compressed_size = 0;
            entry.original_size = 0;
            entry.isDir = true;
        }
        memcpy(entry.file_name, i.get_key().c_str(), i.get_key().length());
        entries.push_back(entry);
    }
    int yy = compressLz4(entries, compressed_entries);
    if (yy <= 0) {
        fclose(fl);
        return false;
    }
    PackerHeader header((int)wrap.size(), yy);
    if (fwrite(&header, sizeof(PackerHeader), 1, fl) != 1) {
        fclose(fl);
        return false;
    }
    if (fwrite(compressed_entries.data(), 1, yy, fl) != yy) {
        fclose(fl);
        return false;
    }

    for (auto& i : wrap) {
        if (i.get_value().isDir)
            continue;
        if (fwrite(i.get_value().compressed.data(), 1, i.get_value().compressedSize, fl) != i.get_value().compressedSize) {
            fclose(fl);
            return false;
        }
    }
    fclose(fl);
    return true;
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

string Decompressor::operator[](const wstring& name_entry) const {
    const DecomressorEntry& entry = Files[name_entry];
    string Decompressed;
    Decompressed.resize(entry.originalSize);

    if (!decompressLz4((const char*)buf, (char*)Decompressed.data(), entry)) {
        return string();
    }
    return Decompressed;
}