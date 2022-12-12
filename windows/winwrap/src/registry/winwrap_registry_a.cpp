#include "winwrap_registry.h"

#include "dprintf.hpp"

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

std::string sanitizePath(const std::string& reg_path) {
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

bool WinRegistryA::keyExist(const std::string& key) {
    bool result = false;
    HKEY root_key = NULL;
    std::string subkey = "";
    // KEY_READ = STANDARD_RIGHTS_READ | KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS | KEY_NOTIFY, so KEY_QUERY_VALUE is better
    DWORD sam_desired = (isWow6464) ? KEY_QUERY_VALUE | KEY_WOW64_64KEY : KEY_QUERY_VALUE | KEY_WOW64_32KEY;
    HKEY existing_key = NULL;
    LSTATUS open_result = ERROR_UNHANDLED_ERROR;

    do {
        if (!disasWinRegAbsPath(key, &root_key, &subkey)) {
            dprintf("[WinRegistryA::keyExist] disasWinRegAbsPath %s failed", key.c_str());
            break;
        }

        if ((open_result = RegOpenKeyExA(root_key, subkey.c_str(), 0, sam_desired, &existing_key)) != ERROR_SUCCESS) {
            dprintf("[WinRegistryA::keyExist] RegOpenKeyExA failed with LSTATUS:0x%08x", open_result);
            break;
        }

        result = true;
    } while (0);

    SAFE_REGCLOSEKEY(existing_key);

    return result;
}

bool WinRegistryA::enumKey(const std::string& key, std::vector<std::string>* out_keys, bool ignore_error, bool recursive) {
    bool result = false;
    HKEY root_key = NULL;
    HKEY target_key = NULL;
    std::string subkey = "";
    DWORD sam_desired = (isWow6464) ? KEY_ENUMERATE_SUB_KEYS | KEY_WOW64_64KEY : KEY_ENUMERATE_SUB_KEYS | KEY_WOW64_32KEY;
    LSTATUS lstatus = ERROR_UNHANDLED_ERROR;
    std::string prefix = "";

    do {
        if (!disasWinRegAbsPath(key, &root_key, &subkey)) {
            dprintf("[WinRegistryA::enumKey] disasWinRegAbsPath %s failed");
            break;
        }
        if ((lstatus = RegOpenKeyExA(root_key, subkey.c_str(), 0, sam_desired, &target_key)) != ERROR_SUCCESS) {
            dprintf("[WinRegistryA::enumKey] RegOpenKeyExA failed with LSTATUS:0x%08x", lstatus);
            break;
        }

        out_keys->clear();
        prefix = key + "\\";
        result = recEnumKey(target_key, prefix, out_keys, ignore_error, recursive);
    } while (0);

    SAFE_REGCLOSEKEY(target_key);

    return result;
}

bool WinRegistryA::recEnumKey(HKEY target_key, std::string prefix, std::vector<std::string>* out_keys, bool ignore_error, bool recursive) {
    bool result = false;
    char keyname_buf[256] = { 0 }; // https://learn.microsoft.com/en-us/windows/win32/sysinfo/registry-element-size-limits
    DWORD keyname_len = 0;
    LSTATUS lstatus = ERROR_UNHANDLED_ERROR;
    DWORD sam_desired = (isWow6464) ? KEY_ENUMERATE_SUB_KEYS | KEY_WOW64_64KEY : KEY_ENUMERATE_SUB_KEYS | KEY_WOW64_32KEY;
    HKEY next_target_key = NULL;
    std::string next_prefix = "";
    bool rec_result = false;

    result = true;

    for (int idx = 0; ; ++idx) {
        keyname_len = _countof(keyname_buf);
        if ((lstatus = RegEnumKeyExA(target_key, idx, keyname_buf, &keyname_len, NULL, NULL, NULL, NULL)) != ERROR_SUCCESS) {
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
        out_keys->push_back(prefix + keyname_buf);

        if (recursive) {
            rec_result = false;
            next_target_key = NULL;

            do {
                if ((lstatus = RegOpenKeyExA(target_key, keyname_buf, 0, sam_desired, &next_target_key)) != ERROR_SUCCESS) {
                    std::string fullpath = prefix + keyname_buf;
                    dprintf("[WinRegistryA::recEnumKey] Recursively opening subkey %s failed", fullpath.c_str());
                    break; // do-while
                }
                next_prefix = prefix + keyname_buf + "\\";
                rec_result = recEnumKey(next_target_key, next_prefix, out_keys, ignore_error, true);
                if (!rec_result) {
                    dprintf("[WinRegistryA::recEnumKey] Recursively calling recEnumKey %s failed", next_prefix.c_str());
                    break; // do-while
                }
            } while (0);

            SAFE_REGCLOSEKEY(next_target_key);
            if (!rec_result && !ignore_error) {
                result = false;
                break; // main loop
            }
        }
    }

    if (next_target_key != NULL) {
        SAFE_REGCLOSEKEY(next_target_key);
    }
    // target_key is not created by this function, so it DOES NOT close it
}

bool WinRegistryA::createKey(const std::string& key, bool* out_created_new) {
    bool result = false;
    HKEY root_key = NULL;
    std::string subkey;
    HKEY target_key = NULL;
    LSTATUS lstatus = ERROR_UNHANDLED_ERROR;
    // reduced privilege based on KEY_ALL_ACCESS. https://learn.microsoft.com/en-us/windows/win32/sysinfo/registry-key-security-and-access-rights
    DWORD sam_default = KEY_QUERY_VALUE | KEY_SET_VALUE | KEY_CREATE_SUB_KEY | KEY_ENUMERATE_SUB_KEYS;
    DWORD sam_desired = (isWow6464) ? sam_default | KEY_WOW64_64KEY : sam_default | KEY_WOW64_32KEY;
    DWORD disposition = 0;

    do {
        if (!disasWinRegAbsPath(key, &root_key, &subkey)) {
            dprintf("[WinRegistryA::createKey] disasWinRegAbsPath %s failed");
            break;
        }
        // REG_OPTION_NON_VOLATILE == 0 (default)
        if ((lstatus = RegCreateKeyExA(root_key, subkey.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, sam_desired, NULL, &target_key, &disposition)) != ERROR_SUCCESS) {
            dprintf("[WinRegistryA::createKey] RegCreateKeyExA failed");
            break;
        }

        if (out_created_new) {
            *out_created_new = (disposition == REG_CREATED_NEW_KEY);
        }
        result = true;
    } while (0);

    return result;
}

bool WinRegistryA::removeKey(const std::string& key, bool recursive) {
    bool result = false;
    HKEY root_key = NULL;
    std::string subkey;
    LSTATUS lstatus = ERROR_UNHANDLED_ERROR;
    DWORD sam_desired_existence = 0;
    DWORD sam_desired_delete = 0;
    HKEY target_key = NULL;
    size_t last_rslash = 0;
    std::string subkey_parent;
    std::string subsubkey;

    do {
        if (!disasWinRegAbsPath(key, &root_key, &subkey)) {
            dprintf("[WinRegistryA::removeKey] disasWinRegAbsPath %s failed");
            break;
        }
        if (subkey.length() == 0) {
            dprintf("[WinRegistryA::removeKey] Cannot remove root registry key : %s", key.c_str());
            break;
        }

        if (recursive) {
            sam_desired_existence = (isWow6464) ? KEY_QUERY_VALUE | KEY_WOW64_64KEY : KEY_QUERY_VALUE | KEY_WOW64_32KEY;
            sam_desired_delete = DELETE | KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE;
            sam_desired_delete = (isWow6464) ? sam_desired_delete | KEY_WOW64_64KEY : sam_desired_delete | KEY_WOW64_32KEY;

            // check existence first
            if ((lstatus = RegOpenKeyExA(root_key, subkey.c_str(), 0, sam_desired_existence, &target_key)) != ERROR_SUCCESS) {
                if (lstatus == ERROR_FILE_NOT_FOUND) {
                    dprintf("[WinRegistryA::removeKey] RegOpenKeyExA failed with ERROR_FILE_NOT_FOUND. Key doesn't seem to exist");
                    result = true; // already inexistent
                }
                else {
                    dprintf("[WinRegistryA::removeKey] RegOpenKeyExA failed with 0x%08x. Open error", lstatus);
                }
                break;
            }
            SAFE_REGCLOSEKEY(target_key);

            if ((last_rslash = subkey.rfind('\\')) != std::string::npos) {
                subkey_parent = subkey.substr(0, last_rslash);
                subsubkey = subkey.substr(last_rslash + 1);
                if ((lstatus = RegOpenKeyExA(root_key, subkey_parent.c_str(), 0, sam_desired_delete, &target_key)) != ERROR_SUCCESS) {
                    dprintf("[WinRegistryA::removeKey] RegOpenKeyExA failed with delete privilege : 0x%08x / %s", lstatus, key.c_str());
                    break;
                }
                if ((lstatus = RegDeleteTreeA(target_key, subsubkey.c_str())) != ERROR_SUCCESS) {
                    dprintf("[WinRegistryA::removeKey] RegDeleteTreeA failed : 0x%08x", lstatus);
                    break;
                }
            }
            else {
                if ((lstatus = RegOpenKeyExA(root_key, nullptr, 0, sam_desired_delete, &target_key)) != ERROR_SUCCESS) {
                    dprintf("[WinRegistryA::removeKey] RegOpenKeyExA failed with delete privilege : 0x%08x / %s", lstatus, key.c_str());
                    break;
                }
                if ((lstatus = RegDeleteTreeA(target_key, subkey.c_str())) != ERROR_SUCCESS) {
                    dprintf("[WinRegistryA::removeKey] RegDeleteTreeA failed : 0x%08x", lstatus);
                    break;
                }
            }
        }
        else {
            sam_desired_delete = (isWow6464) ? KEY_WOW64_64KEY : KEY_WOW64_32KEY;
            if ((lstatus = RegDeleteKeyExA(root_key, subkey.c_str(), sam_desired_delete, 0)) != ERROR_SUCCESS) {
                dprintf("[WinRegistryA::removeKey] RegDeleteKeyExA failed : 0x%08x", lstatus);
                break;
            }
        }
        result = true;

    } while (0);

    SAFE_REGCLOSEKEY(target_key);

    return result;
}

bool WinRegistryA::disasWinRegAbsPath(const std::string& key, HKEY* out_root_key, std::string* out_subkey) {
    HKEY hkey = NULL;
    size_t subkey_offset = 0;

    if (SGStringStartsWith(key, "HKCR")) {
        if (key[4] != '\0' && key[4] != '\\')
            return false;
        hkey = HKEY_CLASSES_ROOT;
        subkey_offset = 4;
    }
    else if (SGStringStartsWith(key, "HKEY_CLASSES_ROOT")) {
        if (key[17] != '\0' && key[17] != '\\')
            return false;
        hkey = HKEY_CLASSES_ROOT;
        subkey_offset = 17;
    }
    else if (SGStringStartsWith(key, "HKCU")) {
        if (key[4] != '\0' && key[4] != '\\')
            return false;
        hkey = HKEY_CURRENT_USER;
        subkey_offset = 4;
    }
    else if (SGStringStartsWith(key, "HKEY_CURRENT_USER")) {
        if (key[17] != '\0' && key[17] != '\\')
            return false;
        hkey = HKEY_CURRENT_USER;
        subkey_offset = 17;
    }
    else if (SGStringStartsWith(key, "HKLM")) {
        if (key[4] != '\0' && key[4] != '\\')
            return false;
        hkey = HKEY_LOCAL_MACHINE;
        subkey_offset = 4;
    }
    else if (SGStringStartsWith(key, "HKEY_LOCAL_MACHINE")) {
        if (key[18] != '\0' && key[18] != '\\')
            return false;
        hkey = HKEY_LOCAL_MACHINE;
        subkey_offset = 18;
    }
    else if (SGStringStartsWith(key, "HKU")) {
        if (key[3] != '\0' && key[3] != '\\')
            return false;
        hkey = HKEY_USERS;
        subkey_offset = 3;
    }
    else if (SGStringStartsWith(key, "HKEY_USERS")) {
        if (key[10] != '\0' && key[10] != '\\')
            return false;
        hkey = HKEY_USERS;
        subkey_offset = 10;
    }
    else if (SGStringStartsWith(key, "HKCC")) {
        if (key[4] != '\0' && key[4] != '\\')
            return false;
        hkey = HKEY_CURRENT_CONFIG;
        subkey_offset = 4;
    }
    else if (SGStringStartsWith(key, "HKEY_CURRENT_CONFIG")) {
        if (key[19] != '\0' && key[19] != '\\')
            return false;
        hkey = HKEY_CURRENT_CONFIG;
        subkey_offset = 19;
    }
    else {
        return false;
    }

    *out_root_key = hkey;
    while (key[subkey_offset] == '\\')
        ++subkey_offset;
    *out_subkey = key.substr(subkey_offset);

    return true;
}