#include "winwrap_registry.h"

#include "dprintf.hpp"

DWORD WinRegistryValueW::toDWORD() {
    DWORD ret = 0;
    memcpy(&ret, data.get(), min(sizeof(DWORD), dataLen));
    return ret;
}

unsigned long long WinRegistryValueW::toQWORD() {
    unsigned long long ret = 0;
    memcpy(&ret, data.get(), min(sizeof(unsigned long long), dataLen));
    return ret;
}

std::wstring WinRegistryValueW::toWstring() {
    return std::wstring((WCHAR*)data.get(), ( dataLen / sizeof(WCHAR) ) );
}

std::vector<std::wstring> WinRegistryValueW::toWstringVector() {
    std::vector<std::wstring> ret;
    WCHAR* p = (WCHAR*)data.get();
    WCHAR* end = p + (dataLen / sizeof(WCHAR));
    while (p < end) {
        ret.push_back(p);
        p += wcslen(p) + 1;
    }
    return ret;
}

std::wstring WinRegistryValueW::descriptionString() {
    const size_t buflen = 0x1000;
    std::shared_ptr<WCHAR> buf = std::shared_ptr<WCHAR>(new WCHAR[buflen], std::default_delete<WCHAR[]>());

    if (type == REG_DWORD) {
        swprintf_s(buf.get(), buflen, L"REG_DWORD: %u(0x%08x)", *(DWORD*)data.get(), *(DWORD*)data.get());
    }
    else if (type == REG_QWORD) {
        swprintf_s(buf.get(), buflen, L"REG_QWORD: %llu(0x%016llx)", *(unsigned long long*)data.get(), *(unsigned long long*)data.get());
    }
    else if (type == REG_SZ) {
        swprintf_s(buf.get(), buflen, L"REG_SZ: %s", (WCHAR*)data.get());
    }
    else if (type == REG_EXPAND_SZ) {
        swprintf_s(buf.get(), buflen, L"REG_EXPAND_SZ: %s", (WCHAR*)data.get());
    }
    else if (type == REG_MULTI_SZ) {
        std::wstring combined;
        WCHAR* p = (WCHAR*)data.get();
        WCHAR* end = p + (dataLen / sizeof(WCHAR));
        while (p < end) {
            combined += p;
            p += wcslen(p) + 1;
            if (p < end)
                combined += L"; ";
        }
        swprintf_s(buf.get(), buflen, L"REG_MULTI_SZ: %s", combined.c_str());
    }

    return std::wstring(buf.get());
}

WinRegistryW::wow6464Key WinRegistryW::WOW6464KEY;
WinRegistryW::wow6432Key WinRegistryW::WOW6432KEY;

WinRegistryW::WinRegistryW() : isWow6464(true) {};
WinRegistryW::WinRegistryW(wow6464Key) : isWow6464(true) {};
WinRegistryW::WinRegistryW(wow6432Key) : isWow6464(false) {};

WinRegistryW::~WinRegistryW() {};

static bool sStringStartsWithW(const std::wstring& src, const std::wstring& prefix);

#define SAFE_REGCLOSEKEY(k) { if (k != NULL) { RegCloseKey(k); k = NULL; } }

bool sStringStartsWithW(const std::wstring& src, const std::wstring& prefix) {
    if (src.length() >= prefix.length()) {
        return (memcmp(src.c_str(), prefix.c_str(), prefix.length() * sizeof(WCHAR) ) == 0);
    }
    return false;
}

std::wstring WinRegistryW::sanitizePath(const std::wstring& reg_path) {
    std::vector<WCHAR> wchar_list;
    size_t idx = 0;

    for (idx = 0; idx < reg_path.length(); ++idx)
        if (reg_path[idx] != L'\\')
            break;

    while (idx < reg_path.length()) {
        if (reg_path[idx] == L'\\') {
            if (wchar_list.back() != L'\\') {
                wchar_list.push_back(L'\\');
            }
        }
        else {
            wchar_list.push_back(reg_path[idx]);
        }
        ++idx;
    }
    return std::wstring(wchar_list.begin(), wchar_list.end());
}

bool WinRegistryW::keyExist(const std::wstring& key_abspath) {
    bool result = false;
    HKEY hkey_exist = NULL;
    // KEY_READ = STANDARD_RIGHTS_READ | KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS | KEY_NOTIFY, so KEY_QUERY_VALUE is better
    DWORD sam_desired = (isWow6464) ? KEY_QUERY_VALUE | KEY_WOW64_64KEY : KEY_QUERY_VALUE | KEY_WOW64_32KEY;

    result = openKeyHandle(key_abspath, sam_desired, &hkey_exist);
    SAFE_REGCLOSEKEY(hkey_exist);

    return result;
}

bool WinRegistryW::enumKey(const std::wstring& key_abspath, std::vector<std::wstring>* out_subkey_abspath_list, bool ignore_error, bool recursive) {
    bool result = false;
    HKEY hkey_enum = NULL;
    std::wstring prefix;
    DWORD sam_desired = (isWow6464) ? KEY_ENUMERATE_SUB_KEYS | KEY_WOW64_64KEY : KEY_ENUMERATE_SUB_KEYS | KEY_WOW64_32KEY;

    if (openKeyHandle(key_abspath, sam_desired, &hkey_enum)) {
        out_subkey_abspath_list->clear();
        prefix = key_abspath + L"\\";
        result = recEnumKey(hkey_enum, prefix, out_subkey_abspath_list, ignore_error, recursive);
    }
    else {
        dprintf("[WinRegistryW::enumKey] openKeyHandle failed");
        result = false;
    }

    SAFE_REGCLOSEKEY(hkey_enum);

    return result;
}

bool WinRegistryW::createKey(const std::wstring& key_abspath, bool* out_created_new) {
    bool result = false;
    HKEY hkey_root = NULL;
    HKEY hkey_create = NULL;
    std::wstring subroot_path;
    // reduced privilege based on KEY_ALL_ACCESS. https://learn.microsoft.com/en-us/windows/win32/sysinfo/registry-key-security-and-access-rights
    DWORD sam_default = KEY_QUERY_VALUE | KEY_SET_VALUE | KEY_CREATE_SUB_KEY | KEY_ENUMERATE_SUB_KEYS;
    DWORD sam_desired = (isWow6464) ? sam_default | KEY_WOW64_64KEY : sam_default | KEY_WOW64_32KEY;
    DWORD disposition = 0;
    LSTATUS lstatus = ERROR_UNHANDLED_ERROR;

    do {
        if (!disasWinRegAbsPath(key_abspath, &hkey_root, &subroot_path)) {
            dprintf("[WinRegistryW::createKey] disasWinRegAbsPath %s failed");
            break;
        }
        // REG_OPTION_NON_VOLATILE == 0 (default)
        if ((lstatus = RegCreateKeyExW(hkey_root, subroot_path.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, sam_desired, NULL, &hkey_create, &disposition)) != ERROR_SUCCESS) {
            dprintf("[WinRegistryW::createKey] RegCreateKeyExW failed");
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

bool WinRegistryW::removeKey(const std::wstring& key_abspath, bool recursive) {
    bool result = false;
    HKEY hkey_root = NULL;
    HKEY hkey_target = NULL;
    std::wstring subroot_path;
    std::wstring subroot_parent;
    std::wstring key_name;
    size_t last_rslash = 0;
    DWORD sam_desired = 0;
    LSTATUS lstatus = ERROR_UNHANDLED_ERROR;

    do {
        if (!disasWinRegAbsPath(key_abspath, &hkey_root, &subroot_path)) {
            dprintf("[WinRegistryW::removeKey] disasWinRegAbsPath %s failed");
            break;
        }
        if (subroot_path.length() == 0) {
            dprintf("[WinRegistryW::removeKey] Cannot remove root registry key : %s", key_abspath.c_str());
            break;
        }

        if (recursive) {
            // check existence first
            sam_desired = (isWow6464) ? KEY_QUERY_VALUE | KEY_WOW64_64KEY : KEY_QUERY_VALUE | KEY_WOW64_32KEY;
            if ((lstatus = RegOpenKeyExW(hkey_root, subroot_path.c_str(), 0, sam_desired, &hkey_target)) != ERROR_SUCCESS) {
                if (lstatus == ERROR_FILE_NOT_FOUND) {
                    dprintf("[WinRegistryW::removeKey] RegOpenKeyExW failed with ERROR_FILE_NOT_FOUND. Key doesn't seem to exist");
                    result = true; // already inexistent
                }
                else {
                    dprintf("[WinRegistryW::removeKey] RegOpenKeyExW failed with 0x%08x. Open error", lstatus);
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
                if ((lstatus = RegOpenKeyExW(hkey_root, subroot_parent.c_str(), 0, sam_desired, &hkey_target)) != ERROR_SUCCESS) {
                    dprintf("[WinRegistryW::removeKey] RegOpenKeyExW failed with delete privilege : 0x%08x / %s", lstatus, key_abspath.c_str());
                    break;
                }
                if ((lstatus = RegDeleteTreeW(hkey_target, key_name.c_str())) != ERROR_SUCCESS) {
                    dprintf("[WinRegistryW::removeKey] RegDeleteTreeW failed : 0x%08x", lstatus);
                    break;
                }
            }
            else { // subroot_path == key_name
                if ((lstatus = RegOpenKeyExW(hkey_root, nullptr, 0, sam_desired, &hkey_target)) != ERROR_SUCCESS) {
                    dprintf("[WinRegistryW::removeKey] RegOpenKeyExW failed with delete privilege : 0x%08x / %s", lstatus, key_abspath.c_str());
                    break;
                }
                if ((lstatus = RegDeleteTreeW(hkey_target, subroot_path.c_str())) != ERROR_SUCCESS) {
                    dprintf("[WinRegistryW::removeKey] RegDeleteTreeW failed : 0x%08x", lstatus);
                    break;
                }
            }
        }
        else { // Only valid for key that does not have subkeys
            sam_desired = (isWow6464) ? KEY_WOW64_64KEY : KEY_WOW64_32KEY;
            if ((lstatus = RegDeleteKeyExW(hkey_root, subroot_path.c_str(), sam_desired, 0)) != ERROR_SUCCESS) {
                dprintf("[WinRegistryW::removeKey] RegDeleteKeyExW failed : 0x%08x", lstatus);
                break;
            }
        }
        result = true;

    } while (0);

    SAFE_REGCLOSEKEY(hkey_target);

    return result;
}

bool WinRegistryW::valueExist(const std::wstring& key_abspath, const std::wstring& value_name) {
    bool result = false;
    HKEY hkey_target = NULL;
    DWORD sam_desired = (isWow6464) ? KEY_QUERY_VALUE | KEY_WOW64_64KEY : KEY_QUERY_VALUE | KEY_WOW64_32KEY;
    LSTATUS lstatus = ERROR_UNHANDLED_ERROR;

    do {
        if (!openKeyHandle(key_abspath, sam_desired, &hkey_target)) {
            dprintf("[WinRegistryW::valueExist] openKeyHandle failed");
            break;
        }
        if ((lstatus = RegQueryValueExW(hkey_target, value_name.c_str(), NULL, NULL, NULL, NULL)) != ERROR_SUCCESS) {
            dprintf("[WinRegistryW::getValue] RegQueryValueExW failed : 0x%08x", lstatus);
            break;
        }
        result = true;
    } while (0);

    SAFE_REGCLOSEKEY(hkey_target);

    return result;
}

bool WinRegistryW::enumValue(const std::wstring& key_abspath, std::vector<WinRegistryValueW>* out_value_list) {
    const DWORD buf_vn_len = 16384; // https://learn.microsoft.com/en-us/windows/win32/sysinfo/registry-element-size-limits

    bool result = false;
    std::shared_ptr<WCHAR> buf_vn;
    std::shared_ptr<BYTE> buf_data;
    HKEY hkey_target = NULL;
    DWORD sam_desired = (isWow6464) ? KEY_QUERY_VALUE | KEY_WOW64_64KEY : KEY_QUERY_VALUE | KEY_WOW64_32KEY;
    DWORD vn_len = 0;
    DWORD valtype = 0;
    DWORD data_len = 0;
    LSTATUS lstatus = ERROR_UNHANDLED_ERROR;

    do {
        if (!openKeyHandle(key_abspath, sam_desired, &hkey_target)) {
            dprintf("[WinRegistryW::enumValue] openKeyHandle failed");
            break;
        }

        buf_vn = std::shared_ptr<WCHAR>(new WCHAR[buf_vn_len], std::default_delete<WCHAR[]>());
        out_value_list->clear();

        for (DWORD idx = 0; ; ++idx) {
            // 1. get the name of value
            vn_len = buf_vn_len;
            lstatus = RegEnumValueW(hkey_target, idx, buf_vn.get(), &vn_len, NULL, NULL, NULL, NULL);

            if (lstatus == ERROR_NO_MORE_ITEMS) {
                result = true;
                break;
            }
            else if (lstatus != ERROR_SUCCESS) {
                dprintf("[WinRegistryW::enumValue] RegEnumValueW failed : 0x%08x", lstatus);
                break;
            }

            // 2. get the data of value
            if ((lstatus = RegGetValueW(hkey_target, NULL, buf_vn.get(), RRF_RT_ANY, &valtype, NULL, &data_len)) != ERROR_SUCCESS) {
                dprintf("[WinRegistryW::enumValue] 1st RegGetValueW failed : 0x%08x", lstatus);
                break;
            }

            buf_data = std::shared_ptr<BYTE>(new BYTE[data_len], std::default_delete<BYTE[]>());

            if ((lstatus = RegGetValueW(hkey_target, NULL, buf_vn.get(), RRF_RT_ANY, &valtype, buf_data.get(), &data_len)) != ERROR_SUCCESS) {
                dprintf("[WinRegistryW::enumValue] 2nd RegGetValueW failed : 0x%08x", lstatus);
                break;
            }

            out_value_list->push_back({ valtype, buf_vn.get(), buf_data, data_len });
        }

        // result == true only if we met ERROR_NO_MORE_ITEMS
    } while (0);

    SAFE_REGCLOSEKEY(hkey_target);

    return result;
}

bool WinRegistryW::enumValueName(const std::wstring& key_abspath, std::vector<std::wstring>* out_value_name_list) {
    const DWORD buf_vn_len = 16384; // https://learn.microsoft.com/en-us/windows/win32/sysinfo/registry-element-size-limits

    bool result = false;
    std::shared_ptr<WCHAR> buf_vn;
    HKEY hkey_target = NULL;
    DWORD sam_desired = (isWow6464) ? KEY_QUERY_VALUE | KEY_WOW64_64KEY : KEY_QUERY_VALUE | KEY_WOW64_32KEY;
    DWORD vn_len = 0;
    LSTATUS lstatus = ERROR_UNHANDLED_ERROR;

    do {
        if (!openKeyHandle(key_abspath, sam_desired, &hkey_target)) {
            dprintf("[WinRegistryW::enumValue] openKeyHandle failed");
            break;
        }

        buf_vn = std::shared_ptr<WCHAR>(new WCHAR[buf_vn_len], std::default_delete<WCHAR[]>());
        out_value_name_list->clear();

        for (DWORD idx = 0; ; ++idx) {
            vn_len = buf_vn_len;
            lstatus = RegEnumValueW(hkey_target, idx, buf_vn.get(), &vn_len, NULL, NULL, NULL, NULL);

            if (lstatus == ERROR_NO_MORE_ITEMS) {
                result = true;
                break;
            }
            if (lstatus != ERROR_SUCCESS) {
                dprintf("[WinRegistryW::enumValueName] RegEnumValueW failed : 0x%08x", lstatus);
                break;
            }

            out_value_name_list->push_back(buf_vn.get());
        }

        // result == true only if we met ERROR_NO_MORE_ITEMS
    } while (0);

    SAFE_REGCLOSEKEY(hkey_target);

    return result;
}

bool WinRegistryW::setValue(const std::wstring& key_abspath, const std::wstring& value_name, DWORD value_type, const BYTE* data, DWORD data_len) {
    bool result = false;
    HKEY hkey_target = NULL;
    DWORD sam_desired = (isWow6464) ? KEY_SET_VALUE | KEY_WOW64_64KEY : KEY_SET_VALUE | KEY_WOW64_32KEY;
    LSTATUS lstatus = ERROR_UNHANDLED_ERROR;

    do {
        if (!openKeyHandle(key_abspath, sam_desired, &hkey_target)) {
            dprintf("[WinRegistryW::setValue] openKeyHandle failed");
            break;
        }

        if ((lstatus = RegSetValueExW(hkey_target, value_name.c_str(), 0, value_type, data, data_len)) != ERROR_SUCCESS) {
            dprintf("[WinRegistryW::setValue] RegSetValueExW failed : 0x%08x", lstatus);
            break;
        }

        result = true;
    } while (0);

    SAFE_REGCLOSEKEY(hkey_target);

    return result;
}

bool WinRegistryW::setValueDWORD(const std::wstring& key_abspath, const std::wstring& value_name, const DWORD value) {
    return setValue(key_abspath, value_name, REG_DWORD, (BYTE*)&value, sizeof(DWORD));
}

bool WinRegistryW::setValueQWORD(const std::wstring& key_abspath, const std::wstring& value_name, const unsigned long long value) {
    return setValue(key_abspath, value_name, REG_QWORD, (BYTE*)&value, sizeof(unsigned long long));
}

bool WinRegistryW::setValueSZ(const std::wstring& key_abspath, const std::wstring& value_name, const std::wstring& value) {
    return setValue(key_abspath, value_name, REG_SZ, (BYTE*)value.c_str(), (DWORD)((value.length() + 1) * sizeof(WCHAR)) ); // add null at end
}

bool WinRegistryW::setValueExpandSZ(const std::wstring& key_abspath, const std::wstring& value_name, const std::wstring& value) {
    // somehow expandsz requires 2 nulls....
    std::shared_ptr<WCHAR> expand_sz = std::shared_ptr<WCHAR>(new WCHAR[value.length()+2], std::default_delete<WCHAR[]>());
    memcpy(expand_sz.get(), value.c_str(), value.length() * sizeof(WCHAR));
    expand_sz.get()[value.length()] = L'\0';
    expand_sz.get()[value.length() + 1] = L'\0';

    return setValue(key_abspath, value_name, REG_EXPAND_SZ, (BYTE*)expand_sz.get(), (DWORD)((value.length() + 2) * sizeof(WCHAR)));
}

bool WinRegistryW::setValueMultiSZ(const std::wstring& key_abspath, const std::wstring& value_name, const std::vector<std::wstring>& value) {
    DWORD wchar_cnt = 0;
    DWORD wchar_cnt_offset = 0;
    std::shared_ptr<WCHAR> multi_sz;

    for (auto& s : value) {
        wchar_cnt += (DWORD)(s.length() + 1); // add null at end
    }
    ++wchar_cnt; // multi sz end with double null

    multi_sz = std::shared_ptr<WCHAR>(new WCHAR[wchar_cnt], std::default_delete<WCHAR[]>());

    for (auto& s : value) {
        memcpy(multi_sz.get() + wchar_cnt_offset, s.c_str(), (s.length() + 1) * sizeof(WCHAR) );
        wchar_cnt_offset += (DWORD)(s.length() + 1);
    }
    multi_sz.get()[wchar_cnt - 1] = L'\0';

    return setValue(key_abspath, value_name, REG_MULTI_SZ, (BYTE*)multi_sz.get(), wchar_cnt * sizeof(WCHAR));
}

bool WinRegistryW::getValue(const std::wstring& key_abspath, const std::wstring& value_name, WinRegistryValueW* out_value) {
    bool result = false;
    HKEY hkey_target = NULL;
    DWORD sam_desired = (isWow6464) ? KEY_QUERY_VALUE | KEY_WOW64_64KEY : KEY_QUERY_VALUE | KEY_WOW64_32KEY;
    DWORD data_len = 0;
    DWORD valtype = 0;
    std::shared_ptr<BYTE> buf_data;
    LSTATUS lstatus = ERROR_UNHANDLED_ERROR;

    do {
        if (!openKeyHandle(key_abspath, sam_desired, &hkey_target)) {
            dprintf("[WinRegistryW::getValue] openKeyHandle failed");
            break;
        }

        if ((lstatus = RegGetValueW(hkey_target, NULL, value_name.c_str(), RRF_RT_ANY, &valtype, NULL, &data_len)) != ERROR_SUCCESS) {
            dprintf("[WinRegistryW::getValue] 1st RegGetValueW did not return ERROR_SUCCESS : 0x%08x", ERROR_MORE_DATA, lstatus);
            break;
        }

        buf_data = std::shared_ptr<BYTE>(new BYTE[data_len], std::default_delete<BYTE[]>());

        if ((lstatus = RegGetValueW(hkey_target, NULL, value_name.c_str(), RRF_RT_ANY, &valtype, buf_data.get(), &data_len)) != ERROR_SUCCESS) {
            dprintf("[WinRegistryW::getValue] 2nd RegGetValueW did not return ERROR_SUCCESS : 0x%08x", lstatus);
            break;
        }

        *out_value = { valtype, value_name, buf_data, data_len };
        result = true;

    } while (0);

    SAFE_REGCLOSEKEY(hkey_target);

    return result;
}

bool WinRegistryW::removeValue(const std::wstring& key_abspath, const std::wstring& value_name) {
    bool result = false;
    HKEY hkey_target = NULL;
    DWORD sam_desired = (isWow6464) ? KEY_SET_VALUE | KEY_WOW64_64KEY : KEY_SET_VALUE | KEY_WOW64_32KEY;
    LSTATUS lstatus = ERROR_UNHANDLED_ERROR;

    do {
        if (!openKeyHandle(key_abspath, sam_desired, &hkey_target)) {
            dprintf("[WinRegistryW::deleteValue] openKeyHandle failed");
            break;
        }

        if ((lstatus = RegDeleteValueW(hkey_target, value_name.c_str())) != ERROR_SUCCESS) {
            dprintf("[WinRegistryW::deleteValue] RegDeleteValueW failed : 0x%08x", lstatus);
            break;
        }

        result = true;
    } while (0);

    SAFE_REGCLOSEKEY(hkey_target);

    return result;
}


bool WinRegistryW::disasWinRegAbsPath(const std::wstring& key_abspath, HKEY* out_hkey_root, std::wstring* out_subroot_path) {
    HKEY hkey = NULL;
    size_t subkey_offset = 0;

    if (sStringStartsWithW(key_abspath, L"HKCR")) {
        if (key_abspath[4] != L'\0' && key_abspath[4] != L'\\')
            return false;
        hkey = HKEY_CLASSES_ROOT;
        subkey_offset = 4;
    }
    else if (sStringStartsWithW(key_abspath, L"HKEY_CLASSES_ROOT")) {
        if (key_abspath[17] != L'\0' && key_abspath[17] != L'\\')
            return false;
        hkey = HKEY_CLASSES_ROOT;
        subkey_offset = 17;
    }
    else if (sStringStartsWithW(key_abspath, L"HKCU")) {
        if (key_abspath[4] != L'\0' && key_abspath[4] != L'\\')
            return false;
        hkey = HKEY_CURRENT_USER;
        subkey_offset = 4;
    }
    else if (sStringStartsWithW(key_abspath, L"HKEY_CURRENT_USER")) {
        if (key_abspath[17] != L'\0' && key_abspath[17] != L'\\')
            return false;
        hkey = HKEY_CURRENT_USER;
        subkey_offset = 17;
    }
    else if (sStringStartsWithW(key_abspath, L"HKLM")) {
        if (key_abspath[4] != L'\0' && key_abspath[4] != L'\\')
            return false;
        hkey = HKEY_LOCAL_MACHINE;
        subkey_offset = 4;
    }
    else if (sStringStartsWithW(key_abspath, L"HKEY_LOCAL_MACHINE")) {
        if (key_abspath[18] != L'\0' && key_abspath[18] != L'\\')
            return false;
        hkey = HKEY_LOCAL_MACHINE;
        subkey_offset = 18;
    }
    else if (sStringStartsWithW(key_abspath, L"HKU")) {
        if (key_abspath[3] != L'\0' && key_abspath[3] != L'\\')
            return false;
        hkey = HKEY_USERS;
        subkey_offset = 3;
    }
    else if (sStringStartsWithW(key_abspath, L"HKEY_USERS")) {
        if (key_abspath[10] != L'\0' && key_abspath[10] != L'\\')
            return false;
        hkey = HKEY_USERS;
        subkey_offset = 10;
    }
    else if (sStringStartsWithW(key_abspath, L"HKCC")) {
        if (key_abspath[4] != L'\0' && key_abspath[4] != L'\\')
            return false;
        hkey = HKEY_CURRENT_CONFIG;
        subkey_offset = 4;
    }
    else if (sStringStartsWithW(key_abspath, L"HKEY_CURRENT_CONFIG")) {
        if (key_abspath[19] != L'\0' && key_abspath[19] != L'\\')
            return false;
        hkey = HKEY_CURRENT_CONFIG;
        subkey_offset = 19;
    }
    else {
        return false;
    }

    *out_hkey_root = hkey;
    while (key_abspath[subkey_offset] == L'\\')
        ++subkey_offset;
    *out_subroot_path = key_abspath.substr(subkey_offset);

    return true;
}

bool WinRegistryW::recEnumKey(HKEY hkey, std::wstring prefix_path, std::vector<std::wstring>* out_subkey_abspath_list, bool ignore_error, bool recursive) {
    bool result = false;
    bool rec_result = false;
    WCHAR keyname_buf[256] = { 0 }; // https://learn.microsoft.com/en-us/windows/win32/sysinfo/registry-element-size-limits
    HKEY hkey_next = NULL;
    std::wstring next_prefix;
    DWORD sam_desired = (isWow6464) ? KEY_ENUMERATE_SUB_KEYS | KEY_WOW64_64KEY : KEY_ENUMERATE_SUB_KEYS | KEY_WOW64_32KEY;
    DWORD keyname_len = 0;
    LSTATUS lstatus = ERROR_UNHANDLED_ERROR;

    result = true;

    for (DWORD idx = 0; ; ++idx) {
        keyname_len = _countof(keyname_buf);
        if ((lstatus = RegEnumKeyExW(hkey, idx, keyname_buf, &keyname_len, NULL, NULL, NULL, NULL)) != ERROR_SUCCESS) {
            if (lstatus == ERROR_NO_MORE_ITEMS) {
                break;
            }
            dprintf("[WinRegistryW::recEnumKey] RegEnumKeyExW failed with error other than ERROR_NO_MORE_ITEMS(0x%08x) : 0x%08x", ERROR_NO_MORE_ITEMS, lstatus);
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
                if ((lstatus = RegOpenKeyExW(hkey, keyname_buf, 0, sam_desired, &hkey_next)) != ERROR_SUCCESS) {
                    std::wstring fullpath = prefix_path + keyname_buf;
                    dprintf("[WinRegistryW::recEnumKey] Recursively opening subkey %s failed", fullpath.c_str());
                    break; // do-while
                }
                next_prefix = prefix_path + keyname_buf + L"\\";
                rec_result = recEnumKey(hkey_next, next_prefix, out_subkey_abspath_list, ignore_error, true);
                if (!rec_result) {
                    dprintf("[WinRegistryW::recEnumKey] Recursively calling recEnumKey %s failed", next_prefix.c_str());
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
    return result;
}

bool WinRegistryW::openKeyHandle(const std::wstring& key_abspath, DWORD sam_desired, HKEY* out_key_handle) {
    bool result = false;
    HKEY hkey_root = NULL;
    std::wstring subkey;
    LSTATUS reg_result = ERROR_UNHANDLED_ERROR;

    do {
        if (!disasWinRegAbsPath(key_abspath, &hkey_root, &subkey)) {
            dprintf("[WinRegistryW::openKeyHandle] disasWinRegAbsPath %s failed");
            break;
        }
        if ((reg_result = RegOpenKeyExW(hkey_root, subkey.c_str(), 0, sam_desired, out_key_handle)) != ERROR_SUCCESS) {
            dprintf("[WinRegistryW::openKeyHandle] RegOpenKeyExW failed with LSTATUS:0x%08x", reg_result);
            break;
        }
        result = true;
    } while (0);

    return result;
}