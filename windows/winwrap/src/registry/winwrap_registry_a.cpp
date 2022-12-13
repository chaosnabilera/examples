#include "winwrap_registry.h"

#include "dprintf.hpp"

DWORD WinRegistryA::WinRegistryValueA::toDWORD() {
    DWORD ret = 0;
    memcpy(&ret, data.get(), min(sizeof(DWORD), dataLen));
    return ret;
}

unsigned long long WinRegistryA::WinRegistryValueA::toQWORD() {
    unsigned long long ret = 0;
    memcpy(&ret, data.get(), min(sizeof(DWORD), dataLen));
    return ret;
}

std::string WinRegistryA::WinRegistryValueA::toString() {
    return std::string((char*)data.get(), dataLen);
}

std::vector<std::string> WinRegistryA::WinRegistryValueA::toStringVector() {
    std::vector<std::string> ret;
    char* p = (char*)data.get();
    char* end = p + dataLen;
    while (p < end) {
        ret.push_back(p);
        p += strlen(p) + 1;
    }
    return ret;
}

WinRegistryA::wow6464Key WinRegistryA::WOW6464KEY;
WinRegistryA::wow6432Key WinRegistryA::WOW6432KEY;

WinRegistryA::WinRegistryA() : isWow6464(true) {};
WinRegistryA::WinRegistryA(wow6464Key) : isWow6464(true) {};
WinRegistryA::WinRegistryA(wow6432Key) : isWow6464(false) {};

WinRegistryA::~WinRegistryA() {};

static bool SGStringStartsWith(const std::string& src, const std::string& prefix);

#define SAFE_REGCLOSEKEY(k) { if (k != NULL) { RegCloseKey(k); k = NULL; } }

bool SGStringStartsWith(const std::string& src, const std::string& prefix) {
    if (src.length() >= prefix.length()) {
        return (memcmp(src.c_str(), prefix.c_str(), prefix.length()) == 0);
    }
    return false;
}

std::string WinRegistryA::sanitizePath(const std::string& reg_path) {
    std::vector<char> char_list;
    int idx = 0;

    for (idx = 0; idx < reg_path.length(); ++idx)
        if (reg_path[idx] != '\\')
            break;

    while (idx < reg_path.length()) {
        if (reg_path[idx] == '\\') {
            if (char_list.back() != '\\') {
                char_list.push_back('\\');
            }
        }
        else {
            char_list.push_back(reg_path[idx]);
        }
        ++idx;
    }
    return std::string(char_list.begin(), char_list.end());
}

bool WinRegistryA::keyExist(const std::string& key_abspath) {
    bool result = false;
    HKEY hkey_exist = NULL;
    // KEY_READ = STANDARD_RIGHTS_READ | KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS | KEY_NOTIFY, so KEY_QUERY_VALUE is better
    DWORD sam_desired = (isWow6464) ? KEY_QUERY_VALUE | KEY_WOW64_64KEY : KEY_QUERY_VALUE | KEY_WOW64_32KEY;
    
    result = openKeyHandle(key_abspath, sam_desired, &hkey_exist);
    SAFE_REGCLOSEKEY(hkey_exist);

    return result;
}

bool WinRegistryA::enumKey(const std::string& key_abspath, std::vector<std::string>* out_subkey_abspath_list, bool ignore_error, bool recursive) {
    bool result = false;
    HKEY hkey_enum = NULL;
    std::string prefix;
    DWORD sam_desired = (isWow6464) ? KEY_ENUMERATE_SUB_KEYS | KEY_WOW64_64KEY : KEY_ENUMERATE_SUB_KEYS | KEY_WOW64_32KEY;

    if (openKeyHandle(key_abspath, sam_desired, &hkey_enum)) {
        out_subkey_abspath_list->clear();
        prefix = key_abspath + "\\";
        result = recEnumKey(hkey_enum, prefix, out_subkey_abspath_list, ignore_error, recursive);
    }
    else {
        dprintf("[WinRegistryA::enumKey] openKeyHandle failed");
        result = false;
    }

    SAFE_REGCLOSEKEY(hkey_enum);

    return result;
}

bool WinRegistryA::createKey(const std::string& key_abspath, bool* out_created_new) {
    bool result = false;
    HKEY hkey_root = NULL;
    HKEY hkey_create = NULL;
    std::string subroot_path;
    // reduced privilege based on KEY_ALL_ACCESS. https://learn.microsoft.com/en-us/windows/win32/sysinfo/registry-key-security-and-access-rights
    DWORD sam_default = KEY_QUERY_VALUE | KEY_SET_VALUE | KEY_CREATE_SUB_KEY | KEY_ENUMERATE_SUB_KEYS;
    DWORD sam_desired = (isWow6464) ? sam_default | KEY_WOW64_64KEY : sam_default | KEY_WOW64_32KEY;
    DWORD disposition = 0;
    LSTATUS lstatus = ERROR_UNHANDLED_ERROR;

    do {
        if (!disasWinRegAbsPath(key_abspath, &hkey_root, &subroot_path)) {
            dprintf("[WinRegistryA::createKey] disasWinRegAbsPath %s failed");
            break;
        }
        // REG_OPTION_NON_VOLATILE == 0 (default)
        if ((lstatus = RegCreateKeyExA(hkey_root, subroot_path.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, sam_desired, NULL, &hkey_create, &disposition)) != ERROR_SUCCESS) {
            dprintf("[WinRegistryA::createKey] RegCreateKeyExA failed");
            break;
        }

        if (out_created_new) {
            *out_created_new = (disposition == REG_CREATED_NEW_KEY);
        }
        result = true;
    } while (0);

    SAFE_REGCLOSEKEY(hkey_create);

    return result;
}

bool WinRegistryA::removeKey(const std::string& key_abspath, bool recursive) {
    bool result = false;
    HKEY hkey_root = NULL;
    HKEY hkey_target = NULL;
    std::string subroot_path;
    std::string subroot_parent;
    std::string key_name;
    size_t last_rslash = 0;
    DWORD sam_desired = 0;
    LSTATUS lstatus = ERROR_UNHANDLED_ERROR;

    do {
        if (!disasWinRegAbsPath(key_abspath, &hkey_root, &subroot_path)) {
            dprintf("[WinRegistryA::removeKey] disasWinRegAbsPath %s failed");
            break;
        }
        if (subroot_path.length() == 0) {
            dprintf("[WinRegistryA::removeKey] Cannot remove root registry key : %s", key_abspath.c_str());
            break;
        }

        if (recursive) {
            // check existence first
            sam_desired = (isWow6464) ? KEY_QUERY_VALUE | KEY_WOW64_64KEY : KEY_QUERY_VALUE | KEY_WOW64_32KEY;
            if ((lstatus = RegOpenKeyExA(hkey_root, subroot_path.c_str(), 0, sam_desired, &hkey_target)) != ERROR_SUCCESS) {
                if (lstatus == ERROR_FILE_NOT_FOUND) {
                    dprintf("[WinRegistryA::removeKey] RegOpenKeyExA failed with ERROR_FILE_NOT_FOUND. Key doesn't seem to exist");
                    result = true; // already inexistent
                }
                else {
                    dprintf("[WinRegistryA::removeKey] RegOpenKeyExA failed with 0x%08x. Open error", lstatus);
                }
                break;
            }
            SAFE_REGCLOSEKEY(hkey_target);

            // Now re-open for removal
            if (isWow6464)
                sam_desired = KEY_WOW64_64KEY | DELETE | KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE;
            else
                sam_desired = KEY_WOW64_32KEY | DELETE | KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE;
            
            if ((last_rslash = subroot_path.rfind('\\')) != std::string::npos) {
                subroot_parent = subroot_path.substr(0, last_rslash);
                key_name = subroot_path.substr(last_rslash + 1);
                if ((lstatus = RegOpenKeyExA(hkey_root, subroot_parent.c_str(), 0, sam_desired, &hkey_target)) != ERROR_SUCCESS) {
                    dprintf("[WinRegistryA::removeKey] RegOpenKeyExA failed with delete privilege : 0x%08x / %s", lstatus, key_abspath.c_str());
                    break;
                }
                if ((lstatus = RegDeleteTreeA(hkey_target, key_name.c_str())) != ERROR_SUCCESS) {
                    dprintf("[WinRegistryA::removeKey] RegDeleteTreeA failed : 0x%08x", lstatus);
                    break;
                }
            }
            else { // subroot_path == key_name
                if ((lstatus = RegOpenKeyExA(hkey_root, nullptr, 0, sam_desired, &hkey_target)) != ERROR_SUCCESS) {
                    dprintf("[WinRegistryA::removeKey] RegOpenKeyExA failed with delete privilege : 0x%08x / %s", lstatus, key_abspath.c_str());
                    break;
                }
                if ((lstatus = RegDeleteTreeA(hkey_target, subroot_path.c_str())) != ERROR_SUCCESS) {
                    dprintf("[WinRegistryA::removeKey] RegDeleteTreeA failed : 0x%08x", lstatus);
                    break;
                }
            }
        }
        else { // Only valid for key that does not have subkeys
            sam_desired = (isWow6464) ? KEY_WOW64_64KEY : KEY_WOW64_32KEY;
            if ((lstatus = RegDeleteKeyExA(hkey_root, subroot_path.c_str(), sam_desired, 0)) != ERROR_SUCCESS) {
                dprintf("[WinRegistryA::removeKey] RegDeleteKeyExA failed : 0x%08x", lstatus);
                break;
            }
        }
        result = true;

    } while (0);

    SAFE_REGCLOSEKEY(hkey_target);

    return result;
}

bool WinRegistryA::valueExist(const std::string& key_abspath, const std::string& value_name) {
    bool result = false;
    HKEY hkey_target = NULL;
    DWORD sam_desired = (isWow6464) ? KEY_QUERY_VALUE | KEY_WOW64_64KEY : KEY_QUERY_VALUE | KEY_WOW64_32KEY;
    LSTATUS lstatus = ERROR_UNHANDLED_ERROR;
    
    do {
        if (!openKeyHandle(key_abspath, sam_desired, &hkey_target)) {
            dprintf("[WinRegistryA::valueExist] openKeyHandle failed");
            break;
        }
        if ((lstatus = RegQueryValueExA(hkey_target, value_name.c_str(), NULL, NULL, NULL, NULL)) != ERROR_SUCCESS) {
            dprintf("[WinRegistryA::getValue] RegQueryValueExA failed : 0x%08x", lstatus);
            break;
        }
        result = true;
    } while (0);

    SAFE_REGCLOSEKEY(hkey_target);

    return result;
}

bool WinRegistryA::enumValue(const std::string& key_abspath, std::vector<WinRegistryValueA>* out_value_list) {
    const DWORD buf_vn_len = 16384; // https://learn.microsoft.com/en-us/windows/win32/sysinfo/registry-element-size-limits

    bool result = false;
    std::shared_ptr<char> buf_vn;
    std::shared_ptr<BYTE> buf_data;
    std::shared_ptr<BYTE> buf_fixed;
    HKEY hkey_target = NULL;
    DWORD sam_desired = (isWow6464) ? KEY_QUERY_VALUE | KEY_WOW64_64KEY : KEY_QUERY_VALUE | KEY_WOW64_32KEY;
    DWORD vn_len = 0;
    DWORD valtype = 0;
    DWORD data_len = 0;
    LSTATUS lstatus = ERROR_UNHANDLED_ERROR;

    do {
        if (!openKeyHandle(key_abspath, sam_desired, &hkey_target)) {
            dprintf("[WinRegistryA::enumValue] openKeyHandle failed");
            break;
        }

        buf_vn = std::shared_ptr<char>(new char[buf_vn_len], std::default_delete<char[]>());
        out_value_list->clear();

        for (DWORD idx = 0; ; ++idx) {
            vn_len = 0;
            data_len = 0;

            lstatus = RegEnumValueA(hkey_target, idx, NULL, &vn_len, NULL, NULL, NULL, &data_len);

            if (lstatus == ERROR_NO_MORE_ITEMS) {
                result = true;
                break;
            }

            if (lstatus != ERROR_MORE_DATA) {
                dprintf("[WinRegistryA::enumValue] 1st call to RegEnumValueA did not return ERROR_MORE_DATA : 0x%08x", lstatus);
                break;
            }

            if (vn_len >= buf_vn_len) {
                dprintf("[WinRegistryA::enumValue] vn_len(%u) >= buf_vn_len(%u)", vn_len, buf_vn_len);
                break;
            }

            if (data_len > 0) {
                vn_len = buf_vn_len;
                buf_data = std::shared_ptr<BYTE>(new BYTE[data_len], std::default_delete<BYTE[]>());
                if ((lstatus = RegEnumValueA(hkey_target, idx, buf_vn.get(), &vn_len, NULL, &valtype, buf_data.get(), &data_len)) != ERROR_SUCCESS) {
                    dprintf("[WinRegistryA::enumValue] 2nd call to RegEnumValueA did not return ERROR_SUCCESS : 0x%08x", lstatus);
                    break;
                }
            }
            else { // special case
                vn_len = buf_vn_len;
                buf_data = std::shared_ptr<BYTE>(nullptr);
                if ((lstatus = RegEnumValueA(hkey_target, idx, buf_vn.get(), &vn_len, NULL, &valtype, NULL, NULL)) != ERROR_SUCCESS) {
                    dprintf("[WinRegistryA::enumValue] 2nd call (data_len == 0) to RegEnumValueA did not return ERROR_SUCCESS : 0x%08x", lstatus);
                    break;
                }
            }

            // not using realloc is kind of lazy but I don't want manual memory management...
            if ((valtype == REG_SZ || valtype == REG_EXPAND_SZ) && !(data_len >= 1 && buf_data.get()[data_len-1] == '\0')) {
                buf_fixed = std::shared_ptr<BYTE>(new BYTE[data_len + 1], std::default_delete<BYTE[]>());
                memcpy(buf_fixed.get(), buf_data.get(), data_len);
                buf_fixed.get()[data_len] = '\0';
                buf_data = buf_fixed;
                data_len += 1;
            }
            else if ((valtype == REG_MULTI_SZ) && !(data_len >= 2 && buf_data.get()[data_len - 2] == '\0' && buf_data.get()[data_len - 1] == '\0')) {
                buf_fixed = std::shared_ptr<BYTE>(new BYTE[data_len + 2], std::default_delete<BYTE[]>());
                memcpy(buf_fixed.get(), buf_data.get(), data_len);
                buf_fixed.get()[data_len]   = '\0';
                buf_fixed.get()[data_len+1] = '\0';
                buf_data = buf_fixed;
                data_len += 2;
            }

            out_value_list->push_back({valtype, buf_vn.get(), buf_data, data_len});
        }
        
        // result == true only if we met ERROR_NO_MORE_ITEMS
    } while (0);

    SAFE_REGCLOSEKEY(hkey_target);

    return result;
}

bool WinRegistryA::enumValueName(const std::string& key_abspath, std::vector<std::string>* out_value_name_list) {
    const DWORD buf_vn_len = 16384; // https://learn.microsoft.com/en-us/windows/win32/sysinfo/registry-element-size-limits

    bool result = false;
    std::shared_ptr<char> buf_vn;
    HKEY hkey_target = NULL;
    DWORD sam_desired = (isWow6464) ? KEY_QUERY_VALUE | KEY_WOW64_64KEY : KEY_QUERY_VALUE | KEY_WOW64_32KEY;
    DWORD vn_len = 0;
    LSTATUS lstatus = ERROR_UNHANDLED_ERROR;

    do {
        if (!openKeyHandle(key_abspath, sam_desired, &hkey_target)) {
            dprintf("[WinRegistryA::enumValue] openKeyHandle failed");
            break;
        }

        buf_vn = std::shared_ptr<char>(new char[buf_vn_len], std::default_delete<char[]>());
        out_value_name_list->clear();

        for (DWORD idx = 0; ; ++idx) {
            vn_len = buf_vn_len;
            lstatus = RegEnumValueA(hkey_target, idx, buf_vn.get(), &vn_len, NULL, NULL, NULL, NULL);

            if (lstatus == ERROR_NO_MORE_ITEMS) {
                result = true;
                break;
            }
            if (lstatus != ERROR_SUCCESS) {
                dprintf("[WinRegistryA::enumValueName] RegEnumValueA failed : 0x%08x", lstatus);
                break;
            }

            out_value_name_list->push_back(buf_vn.get());
        }

        // result == true only if we met ERROR_NO_MORE_ITEMS
    } while (0);

    SAFE_REGCLOSEKEY(hkey_target);

    return result;
}

bool WinRegistryA::disasWinRegAbsPath(const std::string& key_abspath, HKEY* out_hkey_root, std::string* out_subroot_path) {
    HKEY hkey = NULL;
    size_t subkey_offset = 0;

    if (SGStringStartsWith(key_abspath, "HKCR")) {
        if (key_abspath[4] != '\0' && key_abspath[4] != '\\')
            return false;
        hkey = HKEY_CLASSES_ROOT;
        subkey_offset = 4;
    }
    else if (SGStringStartsWith(key_abspath, "HKEY_CLASSES_ROOT")) {
        if (key_abspath[17] != '\0' && key_abspath[17] != '\\')
            return false;
        hkey = HKEY_CLASSES_ROOT;
        subkey_offset = 17;
    }
    else if (SGStringStartsWith(key_abspath, "HKCU")) {
        if (key_abspath[4] != '\0' && key_abspath[4] != '\\')
            return false;
        hkey = HKEY_CURRENT_USER;
        subkey_offset = 4;
    }
    else if (SGStringStartsWith(key_abspath, "HKEY_CURRENT_USER")) {
        if (key_abspath[17] != '\0' && key_abspath[17] != '\\')
            return false;
        hkey = HKEY_CURRENT_USER;
        subkey_offset = 17;
    }
    else if (SGStringStartsWith(key_abspath, "HKLM")) {
        if (key_abspath[4] != '\0' && key_abspath[4] != '\\')
            return false;
        hkey = HKEY_LOCAL_MACHINE;
        subkey_offset = 4;
    }
    else if (SGStringStartsWith(key_abspath, "HKEY_LOCAL_MACHINE")) {
        if (key_abspath[18] != '\0' && key_abspath[18] != '\\')
            return false;
        hkey = HKEY_LOCAL_MACHINE;
        subkey_offset = 18;
    }
    else if (SGStringStartsWith(key_abspath, "HKU")) {
        if (key_abspath[3] != '\0' && key_abspath[3] != '\\')
            return false;
        hkey = HKEY_USERS;
        subkey_offset = 3;
    }
    else if (SGStringStartsWith(key_abspath, "HKEY_USERS")) {
        if (key_abspath[10] != '\0' && key_abspath[10] != '\\')
            return false;
        hkey = HKEY_USERS;
        subkey_offset = 10;
    }
    else if (SGStringStartsWith(key_abspath, "HKCC")) {
        if (key_abspath[4] != '\0' && key_abspath[4] != '\\')
            return false;
        hkey = HKEY_CURRENT_CONFIG;
        subkey_offset = 4;
    }
    else if (SGStringStartsWith(key_abspath, "HKEY_CURRENT_CONFIG")) {
        if (key_abspath[19] != '\0' && key_abspath[19] != '\\')
            return false;
        hkey = HKEY_CURRENT_CONFIG;
        subkey_offset = 19;
    }
    else {
        return false;
    }

    *out_hkey_root = hkey;
    while (key_abspath[subkey_offset] == '\\')
        ++subkey_offset;
    *out_subroot_path = key_abspath.substr(subkey_offset);

    return true;
}

bool WinRegistryA::recEnumKey(HKEY hkey, std::string prefix_path, std::vector<std::string>* out_subkey_abspath_list, bool ignore_error, bool recursive) {
    bool result = false;
    bool rec_result = false;
    char keyname_buf[256] = { 0 }; // https://learn.microsoft.com/en-us/windows/win32/sysinfo/registry-element-size-limits
    HKEY hkey_next = NULL;
    std::string next_prefix;
    DWORD sam_desired = (isWow6464) ? KEY_ENUMERATE_SUB_KEYS | KEY_WOW64_64KEY : KEY_ENUMERATE_SUB_KEYS | KEY_WOW64_32KEY;
    DWORD keyname_len = 0;
    LSTATUS lstatus = ERROR_UNHANDLED_ERROR;

    result = true;

    for (DWORD idx = 0; ; ++idx) {
        keyname_len = _countof(keyname_buf);
        if ((lstatus = RegEnumKeyExA(hkey, idx, keyname_buf, &keyname_len, NULL, NULL, NULL, NULL)) != ERROR_SUCCESS) {
            if (lstatus == ERROR_NO_MORE_ITEMS) {
                break;
            }
            dprintf("[WinRegistryA::recEnumKey] RegEnumKeyExA failed with error other than ERROR_NO_MORE_ITEMS(0x%08x) : 0x%08x", ERROR_NO_MORE_ITEMS, lstatus);
            if (ignore_error) {
                continue;
            }
            else {
                result = false;
                break; // main loop
            }
        }
        keyname_buf[keyname_len] = 0;
        out_subkey_abspath_list->push_back(prefix_path + keyname_buf);

        if (recursive) {
            rec_result = false;
            hkey_next = NULL;

            do {
                if ((lstatus = RegOpenKeyExA(hkey, keyname_buf, 0, sam_desired, &hkey_next)) != ERROR_SUCCESS) {
                    std::string fullpath = prefix_path + keyname_buf;
                    dprintf("[WinRegistryA::recEnumKey] Recursively opening subkey %s failed", fullpath.c_str());
                    break; // do-while
                }
                next_prefix = prefix_path + keyname_buf + "\\";
                rec_result = recEnumKey(hkey_next, next_prefix, out_subkey_abspath_list, ignore_error, true);
                if (!rec_result) {
                    dprintf("[WinRegistryA::recEnumKey] Recursively calling recEnumKey %s failed", next_prefix.c_str());
                    break; // do-while
                }
            } while (0);

            SAFE_REGCLOSEKEY(hkey_next);
            
            if (!rec_result && !ignore_error) {
                result = false;
                break; // main loop
            }
        }
    }

    if (hkey_next != NULL) {
        SAFE_REGCLOSEKEY(hkey_next);
    }
    // target_key is not created by this function, so it DOES NOT close it
}

bool WinRegistryA::openKeyHandle(const std::string& key_abspath, DWORD sam_desired, HKEY* out_key_handle) {
    bool result = false;
    HKEY hkey_root = NULL;
    std::string subkey;
    LSTATUS reg_result = ERROR_UNHANDLED_ERROR;

    do {
        if (!disasWinRegAbsPath(key_abspath, &hkey_root, &subkey)) {
            dprintf("[WinRegistryA::openKeyHandle] disasWinRegAbsPath %s failed");
            break;
        }
        if ((reg_result = RegOpenKeyExA(hkey_root, subkey.c_str(), 0, sam_desired, out_key_handle)) != ERROR_SUCCESS) {
            dprintf("[WinRegistryA::openKeyHandle] RegOpenKeyExA failed with LSTATUS:0x%08x", reg_result);
            break;
        }
        result = true;
    } while (0);

    return result;
}