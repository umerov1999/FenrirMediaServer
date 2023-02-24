#pragma once
#include <iostream>
#include "json.hpp"

static inline std::wstring getAppWithoutExtName() {
    TCHAR szFileName[MAX_PATH];
    GetModuleFileNameW(0, szFileName, MAX_PATH);

    std::wstring fname = szFileName;
    if (fname.empty()) {
        fname = L"AppNotDef";
    }
    size_t pos = fname.find_last_of(L"\\/");
    fname = (std::wstring::npos == pos)
        ? fname
        : fname.substr(pos + 1);
    pos = fname.find_last_of(L'.');
    return (std::wstring::npos == pos)
        ? fname
        : fname.substr(0, pos);
}
#define PREFS_NAME (getAppWithoutExtName() + L".prefs.json")

static inline bool writeJsonConfig(const std::wstring file_name, const nlohmann::json& cfg) {
    const char* UTF8START_PREF = "\xef\xbb\xbf";
    FILE* save = NULL;
    if (_wfopen_s(&save, file_name.c_str(), L"wb") != 0)
        return false;
    if (save == nullptr)
        return false;

    fwrite(UTF8START_PREF, 1, strlen(UTF8START_PREF), save);
    std::string sett = cfg.dump(4);
    fwrite(sett.c_str(), 1, sett.length(), save);
    fclose(save);
    return true;
}

static inline bool parseJsonConfig(const std::wstring file_name, nlohmann::json &cfg) {
    const char* UTF8START_PREF = "\xef\xbb\xbf";
    FILE* fl = NULL;
    if (_wfopen_s(&fl, file_name.c_str(), L"rb") == 0)
    {
        if (fl == nullptr) {
            return false;
        }
        std::string jsonS;
        char tto[3] = {};
        fread(tto, 1, 3, fl);
        int offbom = (memcmp(tto, UTF8START_PREF, 3) == 0 ? (int)strlen(UTF8START_PREF) : 0);
        fseek(fl, 0, SEEK_END);
        jsonS.resize(ftell(fl) - offbom + 1);
        fseek(fl, offbom, SEEK_SET);
        fread((char*)jsonS.data(), 1, jsonS.size() - 1, fl);
        fclose(fl);
        cfg = nlohmann::json::parse(jsonS);
    }
    else {
        return false;
    }
    return true;
}