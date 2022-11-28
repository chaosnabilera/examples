#include <cstring>

#include "pstring.h"

static std::shared_ptr<wchar_t> PEmptyPStringW(nullptr);

static std::shared_ptr<wchar_t> EmptyPStringW() {
    if (PEmptyPStringW.get() == nullptr) {
        PEmptyPStringW = std::shared_ptr<wchar_t>(new wchar_t[1], std::default_delete<wchar_t[]>());
        PEmptyPStringW.get()[0] = L'\0';
    }
    return PEmptyPStringW;
}

PStringW::PStringW() {
    clear();
}

PStringW::PStringW(const wchar_t* wstr) {
    size_t wstr_len = wcslen(wstr);
    cBuf = std::shared_ptr<wchar_t>(new wchar_t[wstr_len + 1], std::default_delete<wchar_t[]>());
    memcpy(cBuf.get(), wstr, wstr_len * sizeof(wchar_t));
    (cBuf.get())[wstr_len] = L'\0';
    pStrLen = wstr_len;
}

PStringW::PStringW(const wchar_t* wstr, size_t wstr_len) {
    cBuf = std::shared_ptr<wchar_t>(new wchar_t[wstr_len + 1], std::default_delete<wchar_t[]>());
    memcpy(cBuf.get(), wstr, wstr_len * sizeof(wchar_t));
    (cBuf.get())[wstr_len] = L'\0';
    pStrLen = wstr_len;
}

PStringW::PStringW(const std::wstring& stlwstr) {
    size_t stlwstr_len = stlwstr.length();
    cBuf = std::shared_ptr<wchar_t>(new wchar_t[stlwstr_len + 1], std::default_delete<wchar_t[]>());
    memcpy(cBuf.get(), stlwstr.c_str(), stlwstr_len * sizeof(wchar_t));
    (cBuf.get())[stlwstr_len] = L'\0';
    pStrLen = stlwstr_len;
}

PStringW::PStringW(const PStringW& other) {
    cBuf = other.cBuf;
    pStrLen = other.pStrLen;
}

PStringW::PStringW(std::shared_ptr<wchar_t> wchar_buf, size_t wchar_buf_len) {
    cBuf = wchar_buf;
    pStrLen = wchar_buf_len;
}

void PStringW::createNewSelfPlusOther(const wchar_t* wstr, size_t wstrlen, std::shared_ptr<wchar_t>* out_cbuf, size_t* out_pstrlen) {
    size_t new_pstrlen = 0;

    if (wstr == nullptr) {
        *out_cbuf = cBuf;
        *out_pstrlen = pStrLen;
    }
    else {
        new_pstrlen = pStrLen + wstrlen;

        *out_cbuf = std::shared_ptr<wchar_t>(new wchar_t[new_pstrlen + 1], std::default_delete<wchar_t[]>());
        *out_pstrlen = new_pstrlen;

        memcpy(out_cbuf->get(), cBuf.get(), pStrLen * sizeof(wchar_t));
        memcpy(out_cbuf->get() + pStrLen, wstr, wstrlen * sizeof(wchar_t));
        (out_cbuf->get())[new_pstrlen] = L'\0';
    }
}

PStringW::~PStringW() { }

PStringW& PStringW::operator=(const PStringW& other) {
    cBuf = other.cBuf;
    pStrLen = other.pStrLen;
    return *this;
}

PStringW PStringW::operator=(const wchar_t* wstr) {
    return PStringW(wstr);
}

PStringW PStringW::operator=(const std::wstring& stlwstr) {
    return PStringW(stlwstr);
}

PStringW PStringW::operator+(const PStringW& other) {
    std::shared_ptr<wchar_t> new_cbuf;
    size_t new_pstrlen;

    createNewSelfPlusOther(other.cBuf.get(), other.pStrLen, &new_cbuf, &new_pstrlen);
    return PStringW(new_cbuf, new_pstrlen);
}

PStringW PStringW::operator+(const wchar_t* wstr) {
    std::shared_ptr<wchar_t> new_cbuf;
    size_t new_pstrlen;

    createNewSelfPlusOther(wstr, wcslen(wstr), &new_cbuf, &new_pstrlen);
    return PStringW(new_cbuf, new_pstrlen);
}

PStringW PStringW::operator+(const std::wstring& stlwstr) {
    std::shared_ptr<wchar_t> new_cbuf;
    size_t new_pstrlen;

    createNewSelfPlusOther(stlwstr.c_str(), stlwstr.length(), &new_cbuf, &new_pstrlen);
    return PStringW(new_cbuf, new_pstrlen);
}

PStringW& PStringW::operator+=(const PStringW& other) {
    std::shared_ptr<wchar_t> new_cbuf;
    size_t new_pstrlen;

    createNewSelfPlusOther(other.cBuf.get(), other.pStrLen, &new_cbuf, &new_pstrlen);

    cBuf = new_cbuf;
    pStrLen = new_pstrlen;
    return *this;
}

PStringW& PStringW::operator+=(const wchar_t* wstr) {
    std::shared_ptr<wchar_t> new_cbuf;
    size_t new_pstrlen;

    createNewSelfPlusOther(wstr, wcslen(wstr), &new_cbuf, &new_pstrlen);

    cBuf = new_cbuf;
    pStrLen = new_pstrlen;
    return *this;
}

PStringW& PStringW::operator+=(const std::wstring& stlwstr) {
    std::shared_ptr<wchar_t> new_cbuf;
    size_t new_pstrlen;

    createNewSelfPlusOther(stlwstr.c_str(), stlwstr.length(), &new_cbuf, &new_pstrlen);

    cBuf = new_cbuf;
    pStrLen = new_pstrlen;
    return *this;
}

bool PStringW::operator==(const PStringW& other) const {
    if (pStrLen != other.pStrLen)
        return false;
    else
        return (memcmp(cBuf.get(), other.cBuf.get(), pStrLen * sizeof(wchar_t)) == 0);
}

bool PStringW::operator==(const wchar_t* wstr) const {
    size_t cstr_len = wcslen(wstr);

    if (cstr_len != pStrLen)
        return false;
    else
        return (memcmp(cBuf.get(), wstr, pStrLen * sizeof(wchar_t)) == 0);
}

bool PStringW::operator==(const std::wstring& stlwstr) const {
    if (stlwstr.length() != pStrLen)
        return false;
    else
        return (memcmp(cBuf.get(), stlwstr.c_str(), pStrLen * sizeof(wchar_t)) == 0);
}

bool PStringW::operator!=(const PStringW& other) const {
    return !(*this == other);
}

bool PStringW::operator!=(const wchar_t* wstr) const {
    return !(*this == wstr);
}

bool PStringW::operator!=(const std::wstring& stlwstr) const {
    return !(*this == stlwstr);
}

bool PStringW::operator<(const PStringW& other) const {
    return (memcmp(cBuf.get(), other.cBuf.get(), pStrLen * sizeof(wchar_t)) < 0);
}

bool PStringW::operator<(const wchar_t* wstr) const {
    return (memcmp(cBuf.get(), wstr, pStrLen * sizeof(wchar_t)) < 0);
}

bool PStringW::operator<(const std::wstring& stlwstr) const {
    return (memcmp(cBuf.get(), stlwstr.c_str(), pStrLen * sizeof(wchar_t)) < 0);
}

bool PStringW::operator<=(const PStringW& other) const {
    return (memcmp(cBuf.get(), other.cBuf.get(), pStrLen * sizeof(wchar_t)) <= 0);
}

bool PStringW::operator<=(const wchar_t* wstr) const {
    return (memcmp(cBuf.get(), wstr, pStrLen * sizeof(wchar_t)) <= 0);
}

bool PStringW::operator<=(const std::wstring& stlwstr) const {
    return (memcmp(cBuf.get(), stlwstr.c_str(), pStrLen * sizeof(wchar_t)) <= 0);
}

bool PStringW::operator>(const PStringW& other) const {
    return (memcmp(cBuf.get(), other.cBuf.get(), pStrLen * sizeof(wchar_t)) > 0);
}

bool PStringW::operator>(const wchar_t* wstr) const {
    return (memcmp(cBuf.get(), wstr, pStrLen * sizeof(wchar_t)) > 0);
}

bool PStringW::operator>(const std::wstring& stlwstr) const {
    return (memcmp(cBuf.get(), stlwstr.c_str(), pStrLen * sizeof(wchar_t)) > 0);
}

bool PStringW::operator>=(const PStringW& other) const {
    return (memcmp(cBuf.get(), other.cBuf.get(), pStrLen * sizeof(wchar_t)) >= 0);
}

bool PStringW::operator>=(const wchar_t* wstr) const {
    return (memcmp(cBuf.get(), wstr, pStrLen * sizeof(wchar_t)) >= 0);
}

bool PStringW::operator>=(const std::wstring& stlwstr) const {
    return (memcmp(cBuf.get(), stlwstr.c_str(), pStrLen * sizeof(wchar_t)) >= 0);
}

wchar_t PStringW::operator[](size_t index) const {
    return cBuf.get()[index];
}

const wchar_t* PStringW::cStr() const {
    return cBuf.get();
}

std::wstring PStringW::stlStr() const {
    return std::wstring(cBuf.get(), pStrLen);
}

size_t PStringW::length() const {
    return pStrLen;
}

void PStringW::clear() {
    cBuf = EmptyPStringW();
    pStrLen = 0;
}

PStringW PStringW::substr(size_t index, size_t length) const {
    if (index >= pStrLen) {
        return PStringW();
    }
    length = (index + length > pStrLen) ? pStrLen - index : length;
    return PStringW(cBuf.get() + index, length);
}

PStringW PStringW::substrFrom(size_t start_index) const {
    return substr(start_index, pStrLen - start_index);
}

PStringW PStringW::substrTo(size_t end_index) const {
    return substr(0, end_index);
}

PStringW PStringW::substrSlice(size_t start_index, size_t end_index) const {
    if (end_index <= start_index)
        return PStringW();

    return substr(start_index, end_index - start_index);
}

bool PStringW::findInternal(const wchar_t* wstr_find, size_t wstr_find_len, size_t find_from, size_t* out_index) const {
    if (find_from >= pStrLen) {
        return false;
    }

    if (wstr_find_len > pStrLen - find_from) {
        return false;
    }

    const wchar_t* found = wcsstr(cBuf.get() + find_from, wstr_find);

    if (found == nullptr) {
        return false;
    }

    if (out_index != nullptr) {
        *out_index = found - cBuf.get();
    }

    return true;
}

bool PStringW::find(const wchar_t* wstr_find, size_t find_from, size_t* out_index) const {
    return findInternal(wstr_find, wcslen(wstr_find), find_from, out_index);
}

bool PStringW::find(const wchar_t* wstr_find, size_t* out_index) const {
    return findInternal(wstr_find, wcslen(wstr_find), 0, out_index);
}

bool PStringW::find(const PStringW& pstr_find, size_t find_from, size_t* out_index) const {
    return findInternal(pstr_find.cBuf.get(), pstr_find.pStrLen, find_from, out_index);
}

bool PStringW::find(const PStringW& pstr_find, size_t* out_index) const {
    return findInternal(pstr_find.cBuf.get(), pstr_find.pStrLen, 0, out_index);
}

bool PStringW::find(const std::wstring& stlwstr_find, size_t find_from, size_t* out_index) const {
    return findInternal(stlwstr_find.c_str(), stlwstr_find.length(), find_from, out_index);
}

bool PStringW::find(const std::wstring& stlwstr_find, size_t* out_index) const {
    return findInternal(stlwstr_find.c_str(), stlwstr_find.length(), 0, out_index);
}

bool PStringW::rfindInternal(const wchar_t* wstr_find, size_t wstr_find_len, size_t rfind_from, size_t* out_index) const {
    bool result, match;
    size_t rbegin;

    if (rfind_from >= pStrLen) {
        return false;
    }

    if (wstr_find_len > rfind_from + 1) {
        return false;
    }

    rbegin = (rfind_from < pStrLen - wstr_find_len) ? rfind_from : pStrLen - wstr_find_len;
    result = false;

    for (wchar_t* p = cBuf.get() + rbegin; p >= cBuf.get(); --p) {
        match = true;
        for (size_t i = 0; i < wstr_find_len; ++i) {
            if (wstr_find[i] != p[i]) {
                match = false;
                break;
            }
        }
        if (match) {
            result = true;
            *out_index = p - cBuf.get();
            break;
        }
    }

    return result;
}

bool PStringW::rfind(const wchar_t* wstr_find, size_t rfind_from, size_t* out_index) const {
    return rfindInternal(wstr_find, wcslen(wstr_find), rfind_from, out_index);
}

bool PStringW::rfind(const wchar_t* wstr_find, size_t* out_index) const {
    return rfindInternal(wstr_find, wcslen(wstr_find), pStrLen - 1, out_index);
}

bool PStringW::rfind(const PStringW& pstr_find, size_t rfind_from, size_t* out_index) const {
    return rfindInternal(pstr_find.cBuf.get(), pstr_find.pStrLen, rfind_from, out_index);
}

bool PStringW::rfind(const PStringW& pstr_find, size_t* out_index) const {
    return rfindInternal(pstr_find.cBuf.get(), pstr_find.pStrLen, pStrLen - 1, out_index);
}

bool PStringW::rfind(const std::wstring& stlwstr_find, size_t rfind_from, size_t* out_index) const {
    return rfindInternal(stlwstr_find.c_str(), stlwstr_find.length(), rfind_from, out_index);
}

bool PStringW::rfind(const std::wstring& stlwstr_find, size_t* out_index) const {
    return rfindInternal(stlwstr_find.c_str(), stlwstr_find.length(), pStrLen - 1, out_index);
}

void PStringW::findAllInternal(const wchar_t* wstr_find, size_t wstr_find_len, std::vector<size_t>* out_indices) const {
    size_t index;
    size_t find_from = 0;
    while (findInternal(wstr_find, wstr_find_len, find_from, &index)) {
        out_indices->push_back(index);
        find_from = index + wstr_find_len;
    }
}

void PStringW::findAll(const wchar_t* wstr_find, std::vector<size_t>* out_indices) const {
    size_t wstr_find_len = wcslen(wstr_find);
    findAllInternal(wstr_find, wstr_find_len, out_indices);
}

void PStringW::findAll(const PStringW& pstr_find, std::vector<size_t>* out_indices) const {
    const wchar_t* wstr_find = pstr_find.cStr();
    size_t wstr_find_len = pstr_find.length();
    findAllInternal(wstr_find, wstr_find_len, out_indices);
}

void PStringW::findAll(const std::wstring& stlwstr_find, std::vector<size_t>* out_indices) const {
    const wchar_t* wstr_find = stlwstr_find.c_str();
    size_t wstr_find_len = stlwstr_find.length();
    findAllInternal(wstr_find, wstr_find_len, out_indices);
}

size_t PStringW::countInternal(const wchar_t* wstr_find, size_t wstr_find_len) const {
    size_t count = 0;
    size_t index;
    size_t find_from = 0;
    while (findInternal(wstr_find, wstr_find_len, find_from, &index)) {
        ++count;
        find_from = index + wstr_find_len;
    }
    return count;
}

size_t PStringW::count(const PStringW& pstr_find) const {
    const wchar_t* wstr_find = pstr_find.cStr();
    size_t wstr_find_len = pstr_find.length();
    return countInternal(wstr_find, wstr_find_len);
}

size_t PStringW::count(const wchar_t* wstr_find) const {
    size_t wstr_find_len = wcslen(wstr_find);
    return countInternal(wstr_find, wstr_find_len);
}

size_t PStringW::count(const std::wstring& stlwstr_find) const {
    const wchar_t* wstr_find = stlwstr_find.c_str();
    size_t wstr_find_len = stlwstr_find.length();
    return countInternal(wstr_find, wstr_find_len);
}

bool PStringW::startsWithInternal(const wchar_t* wstr_find, size_t wstr_find_len) const {
    if (wstr_find_len > pStrLen) {
        return false;
    }
    return (memcmp(wstr_find, cBuf.get(), wstr_find_len * sizeof(wchar_t)) == 0);
}

bool PStringW::startsWith(const PStringW& pstr_find) const {
    const wchar_t* wstr_find = pstr_find.cStr();
    size_t wstr_find_len = pstr_find.length();
    return startsWithInternal(wstr_find, wstr_find_len);
}

bool PStringW::startsWith(const wchar_t* wstr_find) const {
    size_t wstr_find_len = wcslen(wstr_find);
    return startsWithInternal(wstr_find, wstr_find_len);
}

bool PStringW::startsWith(const std::wstring& stlwstr_find) const {
    const wchar_t* wstr_find = stlwstr_find.c_str();
    size_t wstr_find_len = stlwstr_find.length();
    return startsWithInternal(wstr_find, wstr_find_len);
}

bool PStringW::endsWithInternal(const wchar_t* wstr_find, size_t wstr_find_len) const {
    if (wstr_find_len > pStrLen) {
        return false;
    }
    return (memcmp(wstr_find, cBuf.get() + pStrLen - wstr_find_len, wstr_find_len * sizeof(wchar_t)) == 0);
}

bool PStringW::endsWith(const PStringW& pstr_find) const {
    const wchar_t* wstr_find = pstr_find.cStr();
    size_t wstr_find_len = pstr_find.length();
    return endsWithInternal(wstr_find, wstr_find_len);
}

bool PStringW::endsWith(const wchar_t* wstr_find) const {
    size_t wstr_find_len = wcslen(wstr_find);
    return endsWithInternal(wstr_find, wstr_find_len);
}

bool PStringW::endsWith(const std::wstring& stlwstr_find) const {
    const wchar_t* wstr_find = stlwstr_find.c_str();
    size_t wstr_find_len = stlwstr_find.length();
    return endsWithInternal(wstr_find, wstr_find_len);
}

PStringW PStringW::lower() const {
    std::shared_ptr<wchar_t> newbuf = std::shared_ptr<wchar_t>(new wchar_t[pStrLen + 1], std::default_delete<wchar_t[]>());
    wchar_t* src = cBuf.get();
    wchar_t* dst = newbuf.get();

    for (size_t i = 0; i < pStrLen; ++i) {
        if (src[i] >= L'A' && src[i] <= L'Z') {
            dst[i] = src[i] + 32; // 'a'-'A' = 97 - 65 = 32
        }
        else {
            dst[i] = src[i];
        }
    }
    dst[pStrLen] = L'\0';

    return PStringW(newbuf, pStrLen);
}

PStringW PStringW::upper() const {
    std::shared_ptr<wchar_t> newbuf = std::shared_ptr<wchar_t>(new wchar_t[pStrLen + 1], std::default_delete<wchar_t[]>());
    wchar_t* src = cBuf.get();
    wchar_t* dst = newbuf.get();

    for (size_t i = 0; i < pStrLen; ++i) {
        if (src[i] >= 'a' && src[i] <= 'z') {
            dst[i] = src[i] - 32; // 'A'-'Z' = 65 - 97 = -32
        }
        else {
            dst[i] = src[i];
        }
    }
    dst[pStrLen] = L'\0';

    return PStringW(newbuf, pStrLen);
}

PStringW PStringW::strip() const {
    const wchar_t* src = cBuf.get();
    // [start,end)
    size_t start = 0;
    size_t end = pStrLen;

    while (start < end && isspace(src[start])) {
        ++start;
    }
    while (end > start && isspace(src[end - 1])) {
        --end;
    }

    return substr(start, end - start);
}

PStringW PStringW::stripLeft() const {
    const wchar_t* src = cBuf.get();
    size_t start = 0;

    while (start < pStrLen && isspace(src[start])) {
        ++start;
    }
    return substr(start, pStrLen - start);
}

PStringW PStringW::stripRight() const {
    const wchar_t* src = cBuf.get();
    size_t end = pStrLen;

    while (end > 0 && isspace(src[end - 1])) {
        --end;
    }
    return substr(0, end);
}

void PStringW::splitInternal(const wchar_t* wstr_delimiter, size_t wstr_delimiter_len, std::vector<PStringW>* out_pstrs) const {
    size_t cur, search_end;
    size_t search_res;

    out_pstrs->clear();

    // handle special cases
    if (pStrLen == 0) {
        out_pstrs->push_back(PStringW());
    }
    if (wstr_delimiter_len == 0) {
        for (int i = 0; i < pStrLen; ++i) {
            out_pstrs->push_back(substr(i, 1));
        }
        return;
    }
    if (wstr_delimiter_len > pStrLen) {
        out_pstrs->push_back(*this);
    }

    // normal case
    //     * pStrLen > 0
    //     * 0 < wstr_delimiter_len <= pStrLen
    cur = 0;
    search_end = pStrLen - wstr_delimiter_len + 1;

    while (cur < search_end) {
        if (findInternal(wstr_delimiter, wstr_delimiter_len, cur, &search_res)) {
            out_pstrs->push_back(substr(cur, search_res - cur));
            cur = search_res + wstr_delimiter_len;
        }
        else {
            break;
        }
    }
    out_pstrs->push_back(substr(cur, pStrLen - cur));
}

void PStringW::split(const PStringW& pstr_delimiter, std::vector<PStringW>* out_pstrs) const {
    const wchar_t* wstr_delimiter = pstr_delimiter.cStr();
    size_t wstr_delimiter_len = pstr_delimiter.length();
    splitInternal(wstr_delimiter, wstr_delimiter_len, out_pstrs);
}

void PStringW::split(const wchar_t* wstr_delimiter, std::vector<PStringW>* out_pstrs) const {
    size_t wstr_delimiter_len = wcslen(wstr_delimiter);
    splitInternal(wstr_delimiter, wstr_delimiter_len, out_pstrs);
}

void PStringW::split(const std::wstring& stlstr_delimiter, std::vector<PStringW>* out_pstrs) const {
    const wchar_t* wstr_delimiter = stlstr_delimiter.c_str();
    size_t wstr_delimiter_len = stlstr_delimiter.length();
    splitInternal(wstr_delimiter, wstr_delimiter_len, out_pstrs);
}

void PStringW::split(std::vector<PStringW>* out_pstrs) const {
    wchar_t* src = cBuf.get();
    size_t str_beg, cur;

    out_pstrs->clear();

    // special case
    if (pStrLen == 0) {
        out_pstrs->push_back(PStringW());
        return;
    }

    // normal case
    str_beg = 0;
    cur = 0;
    while (cur < pStrLen) {
        if (isspace(src[cur])) {
            if (str_beg == cur) {
                out_pstrs->push_back(PStringW());
            }
            else {
                out_pstrs->push_back(substr(str_beg, cur - str_beg));
            }
            ++cur;
            str_beg = cur;
        }
        else {
            ++cur;
        }
    }

    out_pstrs->push_back(substr(str_beg, pStrLen - str_beg));
    return;
}

PStringW PStringW::joinInternal(std::vector<PStringW>& pstrs, const wchar_t* wstr_delimiter, size_t wstr_delimiter_len) {
    size_t total_len = 0;
    size_t cur = 0;

    // special case
    if (pstrs.size() == 0) {
        return PStringW();
    }

    // normal case
    for (size_t i = 0; i < pstrs.size(); ++i) {
        total_len += pstrs[i].length();
    }
    total_len += (pstrs.size() - 1) * wstr_delimiter_len;

    std::shared_ptr<wchar_t> newbuf = std::shared_ptr<wchar_t>(new wchar_t[total_len + 1], std::default_delete<wchar_t[]>());
    wchar_t* dst = newbuf.get();
    for (size_t i = 0; i < pstrs.size(); ++i) {
        const wchar_t* src = pstrs[i].cStr();
        size_t len = pstrs[i].length();
        memcpy(dst + cur, src, len * sizeof(wchar_t));
        cur += len;
        if (i < pstrs.size() - 1) {
            memcpy(dst + cur, wstr_delimiter, wstr_delimiter_len * sizeof(wchar_t));
            cur += wstr_delimiter_len;
        }
    }
    dst[total_len] = L'\0';

    return PStringW(newbuf, total_len);
}

PStringW PStringW::joinInternal(std::vector<wchar_t*>& wstrs, const wchar_t* wstr_delimiter, size_t wstr_delimiter_len) {
    std::vector<size_t> cstr_lens(wstrs.size(), 0);
    size_t total_len = 0;
    size_t cur = 0;

    // special case
    if (wstrs.size() == 0) {
        return PStringW();
    }

    // normal case
    for (size_t i = 0; i < wstrs.size(); ++i) {
        cstr_lens[i] = wcslen(wstrs[i]);
        total_len += cstr_lens[i];
    }
    total_len += (wstrs.size() - 1) * wstr_delimiter_len;

    std::shared_ptr<wchar_t> newbuf = std::shared_ptr<wchar_t>(new wchar_t[total_len + 1], std::default_delete<wchar_t[]>());
    wchar_t* dst = newbuf.get();
    for (size_t i = 0; i < wstrs.size(); ++i) {
        const wchar_t* src = wstrs[i];
        size_t len = cstr_lens[i];
        memcpy(dst + cur, src, len * sizeof(wchar_t));
        cur += len;
        if (i < wstrs.size() - 1) {
            memcpy(dst + cur, wstr_delimiter, wstr_delimiter_len * sizeof(wchar_t));
            cur += wstr_delimiter_len;
        }
    }
    dst[total_len] = L'\0';

    return PStringW(newbuf, total_len);
}

PStringW PStringW::joinInternal(std::vector<std::wstring>& stlwstrs, const wchar_t* wstr_delimiter, size_t wstr_delimiter_len) {
    size_t total_len = 0;
    size_t cur = 0;

    // special case
    if (stlwstrs.size() == 0) {
        return PStringW();
    }

    // normal case
    for (size_t i = 0; i < stlwstrs.size(); ++i) {
        total_len += stlwstrs[i].length();
    }
    total_len += (stlwstrs.size() - 1) * wstr_delimiter_len;

    std::shared_ptr<wchar_t> newbuf = std::shared_ptr<wchar_t>(new wchar_t[total_len + 1], std::default_delete<wchar_t[]>());
    wchar_t* dst = newbuf.get();
    for (size_t i = 0; i < stlwstrs.size(); ++i) {
        const wchar_t* src = stlwstrs[i].c_str();
        size_t len = stlwstrs[i].length();
        memcpy(dst + cur, src, len * sizeof(wchar_t));
        cur += len;
        if (i < stlwstrs.size() - 1) {
            memcpy(dst + cur, wstr_delimiter, wstr_delimiter_len * sizeof(wchar_t));
            cur += wstr_delimiter_len;
        }
    }
    dst[total_len] = L'\0';

    return PStringW(newbuf, total_len);
}

PStringW PStringW::join(std::vector<PStringW>& pstrs, const PStringW& pstr_delimiter) {
    return joinInternal(pstrs, pstr_delimiter.cStr(), pstr_delimiter.length());
}

PStringW PStringW::join(std::vector<PStringW>& pstrs, const wchar_t* wstr_delimiter) {
    return joinInternal(pstrs, wstr_delimiter, wcslen(wstr_delimiter));
}

PStringW PStringW::join(std::vector<PStringW>& pstrs, const std::wstring& stlstr_delimiter) {
    return joinInternal(pstrs, stlstr_delimiter.c_str(), stlstr_delimiter.length());
}

PStringW PStringW::join(std::vector<wchar_t*>& wstrs, const PStringW& pstr_delimiter) {
    return joinInternal(wstrs, pstr_delimiter.cStr(), pstr_delimiter.length());
}

PStringW PStringW::join(std::vector<wchar_t*>& wstrs, const wchar_t* wstr_delimiter) {
    return joinInternal(wstrs, wstr_delimiter, wcslen(wstr_delimiter));
}

PStringW PStringW::join(std::vector<wchar_t*>& wstrs, const std::wstring& stlstr_delimiter) {
    return joinInternal(wstrs, stlstr_delimiter.c_str(), stlstr_delimiter.length());
}

PStringW PStringW::join(std::vector<std::wstring>& stlwstrs, const PStringW& pstr_delimiter) {
    return joinInternal(stlwstrs, pstr_delimiter.cStr(), pstr_delimiter.length());
}

PStringW PStringW::join(std::vector<std::wstring>& stlwstrs, const wchar_t* wstr_delimiter) {
    return joinInternal(stlwstrs, wstr_delimiter, wcslen(wstr_delimiter));
}

PStringW PStringW::join(std::vector<std::wstring>& stlwstrs, const std::wstring& stlstr_delimiter) {
    return joinInternal(stlwstrs, stlstr_delimiter.c_str(), stlstr_delimiter.length());
}

PStringW PStringW::replaceAllInternal(const wchar_t* wstr_find, size_t wstr_find_len, const wchar_t* cstr_replace, size_t cstr_replace_len) const {
    size_t total_len, cur, last, pos, len;
    wchar_t* dst;
    std::vector<size_t> positions;
    std::shared_ptr<wchar_t> newbuf;

    last = 0;
    while (findInternal(wstr_find, wstr_find_len, last, &pos)) {
        positions.push_back(pos);
        last = pos + wstr_find_len;
    }
    if (positions.size() == 0) {
        return *this;
    }

    total_len = 0;
    cur = 0;
    for (size_t i = 0; i < positions.size(); ++i) {
        total_len += positions[i] - cur;
        total_len += cstr_replace_len;
        cur = positions[i] + wstr_find_len;
    }
    total_len += length() - cur;

    newbuf = std::shared_ptr<wchar_t>(new wchar_t[total_len + 1], std::default_delete<wchar_t[]>());
    dst = newbuf.get();
    cur = 0;
    last = 0;
    for (size_t i = 0; i < positions.size(); ++i) {
        pos = positions[i];
        len = pos - last;
        memcpy(dst + cur, cStr() + last, len * sizeof(wchar_t));
        cur += len;
        memcpy(dst + cur, cstr_replace, cstr_replace_len * sizeof(wchar_t));
        cur += cstr_replace_len;
        last = pos + wstr_find_len;
    }
    len = length() - last;
    memcpy(dst + cur, cStr() + last, len * sizeof(wchar_t));
    cur += len;
    dst[total_len] = L'\0';

    return PStringW(newbuf, total_len);
}

PStringW PStringW::replaceAll(const PStringW& pstr_find, const PStringW& pstr_replace) const {
    return replaceAllInternal(pstr_find.cStr(), pstr_find.length(), pstr_replace.cStr(), pstr_replace.length());
}

PStringW PStringW::replaceAll(const PStringW& pstr_find, const wchar_t* cstr_replace) const {
    return replaceAllInternal(pstr_find.cStr(), pstr_find.length(), cstr_replace, wcslen(cstr_replace));
}

PStringW PStringW::replaceAll(const PStringW& pstr_find, const std::wstring& stlstr_replace) const {
    return replaceAllInternal(pstr_find.cStr(), pstr_find.length(), stlstr_replace.c_str(), stlstr_replace.length());
}

PStringW PStringW::replaceAll(const wchar_t* wstr_find, const PStringW& pstr_replace) const {
    return replaceAllInternal(wstr_find, wcslen(wstr_find), pstr_replace.cStr(), pstr_replace.length());
}

PStringW PStringW::replaceAll(const wchar_t* wstr_find, const wchar_t* cstr_replace) const {
    return replaceAllInternal(wstr_find, wcslen(wstr_find), cstr_replace, wcslen(cstr_replace));
}

PStringW PStringW::replaceAll(const wchar_t* wstr_find, const std::wstring& stlstr_replace) const {
    return replaceAllInternal(wstr_find, wcslen(wstr_find), stlstr_replace.c_str(), stlstr_replace.length());
}

PStringW PStringW::replaceAll(const std::wstring& stlwstr_find, const PStringW& pstr_replace) const {
    return replaceAllInternal(stlwstr_find.c_str(), stlwstr_find.length(), pstr_replace.cStr(), pstr_replace.length());
}

PStringW PStringW::replaceAll(const std::wstring& stlwstr_find, const wchar_t* cstr_replace) const {
    return replaceAllInternal(stlwstr_find.c_str(), stlwstr_find.length(), cstr_replace, wcslen(cstr_replace));
}

PStringW PStringW::replaceAll(const std::wstring& stlwstr_find, const std::wstring& stlstr_replace) const {
    return replaceAllInternal(stlwstr_find.c_str(), stlwstr_find.length(), stlstr_replace.c_str(), stlstr_replace.length());
}

std::wostream& operator<<(std::wostream& os, const PStringW& pstr) {
    return os << pstr.cStr();
}