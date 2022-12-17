#pragma once
#ifndef __WINWRAP_REGISTRY_H__
#define __WINWRAP_REGISTRY_H__

#include <Windows.h>

#include <memory>
#include <string>
#include <vector>

struct WinRegistryValueA {
    DWORD type; // REG_DWORD, REG_SZ, REG_BINARY, etc.
    std::string name;
    std::shared_ptr<BYTE> data;
    DWORD dataLen;

    DWORD toDWORD();
    unsigned long long toQWORD();
    std::string toString();
    std::vector<std::string> toStringVector();
    
    std::string descriptionString();
};

class WinRegistryA {
public:
    static struct wow6464Key {} WOW6464KEY;
    static struct wow6432Key {} WOW6432KEY;
    
    WinRegistryA(); // == WinRegistryA(wow6464Key)
    explicit WinRegistryA(wow6464Key);
    explicit WinRegistryA(wow6432Key);
    ~WinRegistryA();

    std::string sanitizePath(const std::string& reg_path);
    
    // note that keyExist may give false negative if we don't have enough privilege
    bool keyExist(const std::string& key_abspath);
    // if ignore_error == true -> if error occurs at RegEnumKeyExA/RegOpenKeyExA for a subkey, just ignore it and continue
    //                 == false-> if error occurs return false immediately
    bool enumKey(const std::string& key_abspath, std::vector<std::string>* out_subkey_abspath_list, bool ignore_error = true, bool recursive = false);
    bool createKey(const std::string& key_abspath, bool* out_created_new = nullptr);
    // note that removeKey may give false positive if we don't have enough privilege
    bool removeKey(const std::string& key_abspath, bool recursive);
    
    bool valueExist(const std::string& key_abspath, const std::string& value_name);
    bool enumValue(const std::string& key_abspath, std::vector<WinRegistryValueA>* out_value_list);
    bool enumValueName(const std::string& key_abspath, std::vector<std::string>* out_value_name_list);
    // value_type = REG_DWORD, REG_SZ, REG_BINARY, etc.
    bool setValue(const std::string& key_abspath, const std::string& value_name, DWORD value_type, const BYTE* data, DWORD data_len);
    bool setValueDWORD(const std::string& key_abspath, const std::string& value_name, const DWORD value);
    bool setValueQWORD(const std::string& key_abspath, const std::string& value_name, const unsigned long long value);
    bool setValueSZ(const std::string& key_abspath, const std::string& value_name, const std::string& value);
    bool setValueExpandSZ(const std::string& key_abspath, const std::string& value_name, const std::string& value);
    bool setValueMultiSZ(const std::string& key_abspath, const std::string& value_name, const std::vector<std::string>& value);
    bool getValue(const std::string& key_abspath, const std::string& value_name, WinRegistryValueA* out_value);
    bool removeValue(const std::string& key_abspath, const std::string& value_name);

private:
    const bool isWow6464;
    bool disasWinRegAbsPath(const std::string& key_abspath, HKEY* out_hkey_root, std::string* out_subroot_path);
    bool recEnumKey(HKEY hkey, std::string prefix_path, std::vector<std::string>* out_subkey_abspath_list, bool ignore_error, bool recursive);
    bool openKeyHandle(const std::string& key_abspath, DWORD sam_desired, HKEY* out_key_handle);
};

#endif