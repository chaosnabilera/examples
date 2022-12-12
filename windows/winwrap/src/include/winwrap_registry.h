#pragma once
#ifndef __WINWRAP_REGISTRY_H__
#define __WINWRAP_REGISTRY_H__

#include <Windows.h>

#include <memory>
#include <string>
#include <vector>

enum WINREGISTRY_VALUETYPE {
    VT_BINARY,
    VT_DWORD,
    VT_DWORD_BIGENDIAN,
    VT_EXPAND_SZ,
    VT_LINK,
    VT_MULTI_SZ,
    VT_NONE,
    VT_QWORD,
    VT_QWORD_SZ
};

class WinRegistryA {
public:
    struct WinRegistryValueA {
        WINREGISTRY_VALUETYPE type;
        std::shared_ptr<BYTE> data;
        DWORD dataLen;
        
        DWORD toDWORD();
        unsigned long long toQWORD();
        std::string toString();
        std::vector<std::string> toStringVector();
    };
    
    static struct wow6464Key {} WOW6464KEY;
    static struct wow6432Key {} WOW6432KEY;
    
    WinRegistryA(); // == WinRegistryA(wow6464Key)
    explicit WinRegistryA(wow6464Key);
    explicit WinRegistryA(wow6432Key);
    ~WinRegistryA();

    std::string sanitizePath(const std::string& reg_path);
    
    // note that keyExist may give false negative if we don't have enough privilege
    bool keyExist(const std::string& key);
    // if ignore_error == true -> if error occurs at RegEnumKeyExA/RegOpenKeyExA for a subkey, just ignore it and continue
    //                 == false-> if error occurs return false immediately
    bool enumKey(const std::string& key, std::vector<std::string>* out_keys, bool ignore_error = true, bool recursive = false);
    bool createKey(const std::string& key, bool* out_created_new = nullptr);
    // note that removeKey may give false positive if we don't have enough privilege
    bool removeKey(const std::string& key, bool recursive);
    
    bool valueExist(const std::string& key, const std::string& value);
    bool enumValue(const std::string& key, std::vector<std::string>* out_values);
    bool setValue(const std::string& key, const std::string& value, WINREGISTRY_VALUETYPE value_type, const void* data, DWORD data_len);
    bool getValue(const std::string& key, const std::string& value, WinRegistryValueA* out_value);
    bool removeValue(const std::string& key, const std::string& value);

private:
    const bool isWow6464;
    bool disasWinRegAbsPath(const std::string& key, HKEY* out_root_key, std::string* out_subkey);
    bool recEnumKey(HKEY target_key, std::string prefix, std::vector<std::string>* out_keys, bool ignore_error, bool recursive);
};

#endif