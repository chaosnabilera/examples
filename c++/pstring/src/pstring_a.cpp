#include <cstring>

#include "pstring.h"

static std::shared_ptr<char> PEmptyPStringA(nullptr); 

static std::shared_ptr<char> EmptyPStringA() {
    if (PEmptyPStringA.get() == nullptr) {
        PEmptyPStringA = std::shared_ptr<char>(new char[1], std::default_delete<char[]>());
        PEmptyPStringA.get()[0] = '\0';
    }
    return PEmptyPStringA;
}

PStringA::PStringA() {
    clear();
}

PStringA::PStringA(const char* cstr) {
    size_t cstr_len = strlen(cstr);
    cBuf = std::shared_ptr<char>(new char[cstr_len + 1], std::default_delete<char[]>());
    memcpy(cBuf.get(), cstr, cstr_len);
    (cBuf.get())[cstr_len] = '\0';
    pStrLen = cstr_len;
}

PStringA::PStringA(const char* cstr, size_t cstr_len) {
    cBuf = std::shared_ptr<char>(new char[cstr_len + 1], std::default_delete<char[]>());
    memcpy(cBuf.get(), cstr, cstr_len);
    (cBuf.get())[cstr_len] = '\0';
    pStrLen = cstr_len;
}

PStringA::PStringA(const std::string& stlstr) {
    size_t stlstr_len = stlstr.length();
    cBuf = std::shared_ptr<char>(new char[stlstr_len + 1], std::default_delete<char[]>());
    memcpy(cBuf.get(), stlstr.c_str(), stlstr_len);
    (cBuf.get())[stlstr_len] = '\0';
    pStrLen = stlstr_len;
}

PStringA::PStringA(const PStringA& other){
    cBuf = other.cBuf;
    pStrLen = other.pStrLen;
}

PStringA::PStringA(std::shared_ptr<char> char_buf, size_t char_buf_len) {
    cBuf = char_buf;
    pStrLen = char_buf_len;
}

void PStringA::createNewSelfPlusOther(const char* cstr, size_t cstrlen, std::shared_ptr<char>* out_cbuf, size_t* out_pstrlen) {
    size_t new_pstrlen = 0;
    
    if (cstr == nullptr){
        *out_cbuf = cBuf;
        *out_pstrlen = pStrLen;
    }
    else {
        new_pstrlen = pStrLen + cstrlen;
        
        *out_cbuf = std::shared_ptr<char>(new char[new_pstrlen + 1], std::default_delete<char[]>());
        *out_pstrlen = new_pstrlen;
        
        memcpy(out_cbuf->get(), cBuf.get(), pStrLen);
        memcpy(out_cbuf->get() + pStrLen, cstr, cstrlen);
        (out_cbuf->get())[new_pstrlen] = '\0';
    }
}

PStringA::~PStringA() { }

PStringA& PStringA::operator=(const PStringA& other) {
    cBuf = other.cBuf;
    pStrLen = other.pStrLen;
    return *this;
}

PStringA PStringA::operator=(const char* cstr) {
    return PStringA(cstr);
}

PStringA PStringA::operator=(const std::string& std_str) {
    return PStringA(std_str);
}

PStringA PStringA::operator+(const PStringA& other){
    std::shared_ptr<char> new_cbuf;
    size_t new_pstrlen;
    
    createNewSelfPlusOther(other.cBuf.get(), other.pStrLen, &new_cbuf, &new_pstrlen);
    return PStringA(new_cbuf, new_pstrlen);
}

PStringA PStringA::operator+(const char* cstr) {
    std::shared_ptr<char> new_cbuf;
    size_t new_pstrlen;

    createNewSelfPlusOther(cstr, strlen(cstr), &new_cbuf, &new_pstrlen);
    return PStringA(new_cbuf, new_pstrlen);
}

PStringA PStringA::operator+(const std::string& stlstr) {
    std::shared_ptr<char> new_cbuf;
    size_t new_pstrlen;
    
    createNewSelfPlusOther(stlstr.c_str(), stlstr.length(), &new_cbuf, &new_pstrlen);
    return PStringA(new_cbuf, new_pstrlen);
}

PStringA& PStringA::operator+=(const PStringA& other) {
    std::shared_ptr<char> new_cbuf;
    size_t new_pstrlen;

    createNewSelfPlusOther(other.cBuf.get(), other.pStrLen, &new_cbuf, &new_pstrlen);
    
    cBuf = new_cbuf;
    pStrLen = new_pstrlen;
    return *this;
}

PStringA& PStringA::operator+=(const char* cstr) {
    std::shared_ptr<char> new_cbuf;
    size_t new_pstrlen;

    createNewSelfPlusOther(cstr, strlen(cstr), &new_cbuf, &new_pstrlen);

    cBuf = new_cbuf;
    pStrLen = new_pstrlen;
    return *this;
}

PStringA& PStringA::operator+=(const std::string& stlstr) {
    std::shared_ptr<char> new_cbuf;
    size_t new_pstrlen;

    createNewSelfPlusOther(stlstr.c_str(), stlstr.length(), &new_cbuf, &new_pstrlen);

    cBuf = new_cbuf;
    pStrLen = new_pstrlen;
    return *this;
}

bool PStringA::operator==(const PStringA& other) const {
    if (pStrLen != other.pStrLen)
        return false;
    else
        return (memcmp(cBuf.get(), other.cBuf.get(), pStrLen) == 0);
}

bool PStringA::operator==(const char* cstr) const {
    size_t cstr_len = strlen(cstr);
    
    if (cstr_len != pStrLen)
        return false;
    else
        return (memcmp(cBuf.get(), cstr, pStrLen) == 0);
}

bool PStringA::operator==(const std::string& stlstr) const {
    if (stlstr.length() != pStrLen)
        return false;
    else
        return (memcmp(cBuf.get(), stlstr.c_str(), pStrLen) == 0);
}

bool PStringA::operator!=(const PStringA& other) const {
    return !(*this == other);
}

bool PStringA::operator!=(const char* cstr) const {
    return !(*this == cstr);
}

bool PStringA::operator!=(const std::string& stlstr) const {
    return !(*this == stlstr);
}

bool PStringA::operator<(const PStringA& other) const {
    return (memcmp(cBuf.get(), other.cBuf.get(), pStrLen) < 0);
}

bool PStringA::operator<(const char* cstr) const {
    return (memcmp(cBuf.get(), cstr, pStrLen) < 0);
}

bool PStringA::operator<(const std::string& stlstr) const {
    return (memcmp(cBuf.get(), stlstr.c_str(), pStrLen) < 0);
}

bool PStringA::operator<=(const PStringA& other) const {
    return (memcmp(cBuf.get(), other.cBuf.get(), pStrLen) <= 0);
}

bool PStringA::operator<=(const char* cstr) const {
    return (memcmp(cBuf.get(), cstr, pStrLen) <= 0);
}

bool PStringA::operator<=(const std::string& stlstr) const {
    return (memcmp(cBuf.get(), stlstr.c_str(), pStrLen) <= 0);
}

bool PStringA::operator>(const PStringA& other) const {
    return (memcmp(cBuf.get(), other.cBuf.get(), pStrLen) > 0);
}

bool PStringA::operator>(const char* cstr) const {
    return (memcmp(cBuf.get(), cstr, pStrLen) > 0);
}

bool PStringA::operator>(const std::string& stlstr) const {
    return (memcmp(cBuf.get(), stlstr.c_str(), pStrLen) > 0);
}

bool PStringA::operator>=(const PStringA& other) const {
    return (memcmp(cBuf.get(), other.cBuf.get(), pStrLen) >= 0);
}

bool PStringA::operator>=(const char* cstr) const {
    return (memcmp(cBuf.get(), cstr, pStrLen) >= 0);
}

bool PStringA::operator>=(const std::string& stlstr) const {
    return (memcmp(cBuf.get(), stlstr.c_str(), pStrLen) >= 0);
}

char PStringA::operator[](size_t index) const {
    return cBuf.get()[index];
}

const char* PStringA::cStr() const {
    return cBuf.get();
}

std::string PStringA::stlStr() const {
    return std::string(cBuf.get(), pStrLen);
}

size_t PStringA::length() const {
    return pStrLen;
}

void PStringA::clear() {
    cBuf = EmptyPStringA();
    pStrLen = 0;
}

PStringA PStringA::substr(size_t index, size_t length) const {
    if(index >= pStrLen){
        return PStringA();
    }
    length = (index + length > pStrLen) ? pStrLen - index : length;
    return PStringA(cBuf.get() + index, length);
}

PStringA PStringA::substrFrom(size_t start_index) const {
    return substr(start_index, pStrLen - start_index);
}

PStringA PStringA::substrTo(size_t end_index) const {
    return substr(0, end_index);
}

PStringA PStringA::substrSlice(size_t start_index, size_t end_index) const {
    if (end_index <= start_index)
        return PStringA();
    
    return substr(start_index, end_index - start_index);
}

bool PStringA::findInternal(const char* cstr_find, size_t cstr_find_len, size_t find_from, size_t* out_index) const {
    if (find_from >= pStrLen) {
        return false;
    }
    
    if (cstr_find_len > pStrLen - find_from) {
        return false;
    }
    
    const char* found = strstr(cBuf.get() + find_from, cstr_find);
    
    if (found == nullptr) {
        return false;
    }

    if (out_index != nullptr) {
        *out_index = found - cBuf.get();
    }

    return true;
}

bool PStringA::find(const char* cstr_find, size_t find_from, size_t* out_index) const {
    return findInternal(cstr_find, strlen(cstr_find), find_from, out_index);
}

bool PStringA::find(const char* cstr_find, size_t* out_index) const {
    return findInternal(cstr_find, strlen(cstr_find), 0, out_index);
}

bool PStringA::find(const PStringA& pstr_find, size_t find_from, size_t* out_index) const {
    return findInternal(pstr_find.cBuf.get(), pstr_find.pStrLen, find_from, out_index);
}

bool PStringA::find(const PStringA& pstr_find, size_t* out_index) const {
    return findInternal(pstr_find.cBuf.get(), pstr_find.pStrLen, 0, out_index);
}

bool PStringA::find(const std::string& stlstr_find, size_t find_from, size_t* out_index) const {
    return findInternal(stlstr_find.c_str(), stlstr_find.length(), find_from, out_index);
}

bool PStringA::find(const std::string& stlstr_find, size_t* out_index) const {
    return findInternal(stlstr_find.c_str(), stlstr_find.length(), 0, out_index);
}

bool PStringA::rfindInternal(const char* cstr_find, size_t cstr_find_len, size_t rfind_from, size_t* out_index) const {
    bool result, match;
    size_t rbegin;
    
    if (rfind_from >= pStrLen) {
        return false;
    }
    
    if (cstr_find_len > rfind_from+1) {
        return false;
    }

    rbegin = (rfind_from < pStrLen - cstr_find_len) ? rfind_from : pStrLen - cstr_find_len;
    result = false;
    
    for (char* p = cBuf.get() + rbegin; p >= cBuf.get(); --p) {
        match = true;
        for (size_t i = 0; i < cstr_find_len; ++i) {
            if (cstr_find[i] != p[i]) {
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

bool PStringA::rfind(const char* cstr_find, size_t rfind_from, size_t* out_index) const {
    return rfindInternal(cstr_find, strlen(cstr_find), rfind_from, out_index);
}

bool PStringA::rfind(const char* cstr_find, size_t* out_index) const {
    return rfindInternal(cstr_find, strlen(cstr_find), pStrLen - 1, out_index);
}

bool PStringA::rfind(const PStringA& pstr_find, size_t rfind_from, size_t* out_index) const {
    return rfindInternal(pstr_find.cBuf.get(), pstr_find.pStrLen, rfind_from, out_index);
}

bool PStringA::rfind(const PStringA& pstr_find, size_t* out_index) const {
    return rfindInternal(pstr_find.cBuf.get(), pstr_find.pStrLen, pStrLen - 1, out_index);
}

bool PStringA::rfind(const std::string& stlstr_find, size_t rfind_from, size_t* out_index) const {
    return rfindInternal(stlstr_find.c_str(), stlstr_find.length(), rfind_from, out_index);
}

bool PStringA::rfind(const std::string& stlstr_find, size_t* out_index) const {
    return rfindInternal(stlstr_find.c_str(), stlstr_find.length(), pStrLen - 1, out_index);
}

void PStringA::findAllInternal(const char* cstr_find, size_t cstr_find_len, std::vector<size_t>* out_indices) const {
    size_t index;
    size_t find_from = 0;
    while (findInternal(cstr_find, cstr_find_len, find_from, &index)) {
        out_indices->push_back(index);
        find_from = index + cstr_find_len;
    }
}

void PStringA::findAll(const char* cstr_find, std::vector<size_t>* out_indices) const {
    size_t cstr_find_len = strlen(cstr_find);
    findAllInternal(cstr_find, cstr_find_len, out_indices);
}

void PStringA::findAll(const PStringA& pstr_find, std::vector<size_t>* out_indices) const {
    const char* cstr_find = pstr_find.cStr();
    size_t cstr_find_len = pstr_find.length();
    findAllInternal(cstr_find, cstr_find_len, out_indices);
}

void PStringA::findAll(const std::string& stlstr_find, std::vector<size_t>* out_indices) const {
    const char* cstr_find = stlstr_find.c_str();
    size_t cstr_find_len = stlstr_find.length();
    findAllInternal(cstr_find, cstr_find_len, out_indices);
}

size_t PStringA::countInternal(const char* cstr_find, size_t cstr_find_len) const {
    size_t count = 0;
    size_t index;
    size_t find_from = 0;
    while (findInternal(cstr_find, cstr_find_len, find_from, &index)) {
        ++count;
        find_from = index + cstr_find_len;
    }
    return count;
}

size_t PStringA::count(const PStringA& pstr_find) const {
    const char* cstr_find = pstr_find.cStr();
    size_t cstr_find_len = pstr_find.length();
    return countInternal(cstr_find, cstr_find_len);
}

size_t PStringA::count(const char* cstr_find) const {
    size_t cstr_find_len = strlen(cstr_find);
    return countInternal(cstr_find, cstr_find_len);
}

size_t PStringA::count(const std::string& stlstr_find) const {
    const char* cstr_find = stlstr_find.c_str();
    size_t cstr_find_len = stlstr_find.length();
    return countInternal(cstr_find, cstr_find_len);
}

bool PStringA::startsWithInternal(const char* cstr_find, size_t cstr_find_len) const {
    if (cstr_find_len > pStrLen) {
        return false;
    }
    return (memcmp(cstr_find, cBuf.get(), cstr_find_len) == 0);
}

bool PStringA::startsWith(const PStringA& pstr_find) const {
    const char* cstr_find = pstr_find.cStr();
    size_t cstr_find_len = pstr_find.length();
    return startsWithInternal(cstr_find, cstr_find_len);
}

bool PStringA::startsWith(const char* cstr_find) const {
    size_t cstr_find_len = strlen(cstr_find);
    return startsWithInternal(cstr_find, cstr_find_len);
}

bool PStringA::startsWith(const std::string& stlstr_find) const {
    const char* cstr_find = stlstr_find.c_str();
    size_t cstr_find_len = stlstr_find.length();
    return startsWithInternal(cstr_find, cstr_find_len);
}

bool PStringA::endsWithInternal(const char* cstr_find, size_t cstr_find_len) const {
    if (cstr_find_len > pStrLen) {
        return false;
    }
    return (memcmp(cstr_find, cBuf.get() + pStrLen - cstr_find_len, cstr_find_len) == 0);
}

bool PStringA::endsWith(const PStringA& pstr_find) const {
    const char* cstr_find = pstr_find.cStr();
    size_t cstr_find_len = pstr_find.length();
    return endsWithInternal(cstr_find, cstr_find_len);
}

bool PStringA::endsWith(const char* cstr_find) const {
    size_t cstr_find_len = strlen(cstr_find);
    return endsWithInternal(cstr_find, cstr_find_len);
}

bool PStringA::endsWith(const std::string& stlstr_find) const {
    const char* cstr_find = stlstr_find.c_str();
    size_t cstr_find_len = stlstr_find.length();
    return endsWithInternal(cstr_find, cstr_find_len);
}

PStringA PStringA::lower() const {
    std::shared_ptr<char> newbuf = std::shared_ptr<char>(new char[pStrLen + 1], std::default_delete<char[]>());
    char* src = cBuf.get();
    char* dst = newbuf.get();
    
    for(size_t i=0; i< pStrLen; ++i) {
        if (src[i] >= 'A' && src[i] <= 'Z') {
            dst[i] = src[i] + 32; // 'a'-'A' = 97 - 65 = 32
        }
        else {
            dst[i] = src[i];
        }
    }
    dst[pStrLen] = '\0';

    return PStringA(newbuf, pStrLen);
}

PStringA PStringA::upper() const {
    std::shared_ptr<char> newbuf = std::shared_ptr<char>(new char[pStrLen + 1], std::default_delete<char[]>());
    char* src = cBuf.get();
    char* dst = newbuf.get();

    for (size_t i = 0; i < pStrLen; ++i) {
        if (src[i] >= 'a' && src[i] <= 'z') {
            dst[i] = src[i] - 32; // 'A'-'Z' = 65 - 97 = -32
        }
        else {
            dst[i] = src[i];
        }
    }
    dst[pStrLen] = '\0';

    return PStringA(newbuf, pStrLen);
}

PStringA PStringA::strip() const { 
    const char* src = cBuf.get();
    // [start,end)
    size_t start = 0;
    size_t end = pStrLen;

    while (start < end && isspace(src[start])) {
        ++start;
    }
    while (end > start && isspace(src[end-1])) {
        --end;
    }
    
    return substr(start, end - start);
}

PStringA PStringA::stripLeft() const {
    const char* src = cBuf.get();
    size_t start = 0;

    while (start < pStrLen && isspace(src[start])) {
        ++start;
    }
    return substr(start, pStrLen - start);
}

PStringA PStringA::stripRight() const {
    const char* src = cBuf.get();
    size_t end = pStrLen;

    while (end > 0 && isspace(src[end-1])) {
        --end;
    }
    return substr(0, end);
}

void PStringA::splitInternal(const char* cstr_delimiter, size_t cstr_delimiter_len, std::vector<PStringA>* out_pstrs) const {
    size_t cur, search_end;
    size_t search_res;
    
    out_pstrs->clear();
    
    // handle special cases
    if (pStrLen == 0) {
        out_pstrs->push_back(PStringA());
    }
    if(cstr_delimiter_len == 0) {
        for (int i = 0; i < pStrLen; ++i) {
            out_pstrs->push_back(substr(i, 1));
        }
        return;
    }
    if (cstr_delimiter_len > pStrLen) {
        out_pstrs->push_back(*this);
    }

    // normal case
    //     * pStrLen > 0
    //     * 0 < cstr_delimiter_len <= pStrLen
    cur = 0;
    search_end = pStrLen - cstr_delimiter_len + 1;

    while (cur < search_end) {
        if (findInternal(cstr_delimiter, cstr_delimiter_len, cur, &search_res)) {
            out_pstrs->push_back(substr(cur, search_res - cur));
            cur = search_res + cstr_delimiter_len;
        }
        else {
            break;
        }
    }
    out_pstrs->push_back(substr(cur, pStrLen - cur));
}

void PStringA::split(const PStringA& pstr_delimiter, std::vector<PStringA>* out_pstrs) const {
    const char* cstr_delimiter = pstr_delimiter.cStr();
    size_t cstr_delimiter_len = pstr_delimiter.length();
    splitInternal(cstr_delimiter, cstr_delimiter_len, out_pstrs);
}

void PStringA::split(const char* cstr_delimiter, std::vector<PStringA>* out_pstrs) const {
    size_t cstr_delimiter_len = strlen(cstr_delimiter);
    splitInternal(cstr_delimiter, cstr_delimiter_len, out_pstrs);
}

void PStringA::split(const std::string& stlstr_delimiter, std::vector<PStringA>* out_pstrs) const {
    const char* cstr_delimiter = stlstr_delimiter.c_str();
    size_t cstr_delimiter_len = stlstr_delimiter.length();
    splitInternal(cstr_delimiter, cstr_delimiter_len, out_pstrs);
}

void PStringA::split(std::vector<PStringA>* out_pstrs) const {
    char* src = cBuf.get();
    size_t str_beg, cur;

    out_pstrs->clear();

    // special case
    if (pStrLen == 0) {
        out_pstrs->push_back(PStringA());
        return;
    }
    
    // normal case
    str_beg = 0;
    cur = 0;
    while(cur < pStrLen) {
        if (isspace(src[cur])) {
            if (str_beg == cur) {
                out_pstrs->push_back(PStringA());
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

PStringA PStringA::joinInternal(std::vector<PStringA>& pstrs, const char* cstr_delimiter, size_t cstr_delimiter_len) {
    size_t total_len = 0;
    size_t cur = 0;

    // special case
    if (pstrs.size() == 0) {
        return PStringA();
    }
    
    // normal case
    for (size_t i = 0; i < pstrs.size(); ++i) {
        total_len += pstrs[i].length();
    }
    total_len += (pstrs.size() - 1) * cstr_delimiter_len;

    std::shared_ptr<char> newbuf = std::shared_ptr<char>(new char[total_len + 1], std::default_delete<char[]>());
    char* dst = newbuf.get();
    for (size_t i = 0; i < pstrs.size(); ++i) {
        const char* src = pstrs[i].cStr();
        size_t len = pstrs[i].length();
        memcpy(dst + cur, src, len);
        cur += len;
        if (i < pstrs.size() - 1) {
            memcpy(dst + cur, cstr_delimiter, cstr_delimiter_len);
            cur += cstr_delimiter_len;
        }
    }
    dst[total_len] = '\0';
    
    return PStringA(newbuf, total_len);
}

PStringA PStringA::joinInternal(std::vector<char*>& cstrs, const char* cstr_delimiter, size_t cstr_delimiter_len) {
    std::vector<size_t> cstr_lens(cstrs.size(), 0);
    size_t total_len = 0;
    size_t cur = 0;

    // special case
    if (cstrs.size() == 0) {
        return PStringA();
    }
    
    // normal case
    for (size_t i = 0; i < cstrs.size(); ++i) {
        cstr_lens[i] = strlen(cstrs[i]);
        total_len += cstr_lens[i];
    }
    total_len += (cstrs.size() - 1) * cstr_delimiter_len;

    std::shared_ptr<char> newbuf = std::shared_ptr<char>(new char[total_len + 1], std::default_delete<char[]>());
    char* dst = newbuf.get();
    for (size_t i = 0; i < cstrs.size(); ++i) {
        const char* src = cstrs[i];
        size_t len = cstr_lens[i];
        memcpy(dst + cur, src, len);
        cur += len;
        if (i < cstrs.size() - 1) {
            memcpy(dst + cur, cstr_delimiter, cstr_delimiter_len);
            cur += cstr_delimiter_len;
        }
    }
    dst[total_len] = '\0';
    
    return PStringA(newbuf, total_len);
}

PStringA PStringA::joinInternal(std::vector<std::string>& stlstrs, const char* cstr_delimiter, size_t cstr_delimiter_len) {
    size_t total_len = 0;
    size_t cur = 0;
    
    // special case
    if (stlstrs.size() == 0) {
        return PStringA();
    }

    // normal case
    for (size_t i = 0; i < stlstrs.size(); ++i) {
        total_len += stlstrs[i].length();
    }
    total_len += (stlstrs.size() - 1) * cstr_delimiter_len;
    
    std::shared_ptr<char> newbuf = std::shared_ptr<char>(new char[total_len + 1], std::default_delete<char[]>());
    char* dst = newbuf.get();
    for (size_t i = 0; i < stlstrs.size(); ++i) {
        const char* src = stlstrs[i].c_str();
        size_t len = stlstrs[i].length();
        memcpy(dst + cur, src, len);
        cur += len;
        if (i < stlstrs.size() - 1) {
            memcpy(dst + cur, cstr_delimiter, cstr_delimiter_len);
            cur += cstr_delimiter_len;
        }
    }
    dst[total_len] = '\0';
        
    return PStringA(newbuf, total_len);
}

PStringA PStringA::join(std::vector<PStringA>& pstrs, const PStringA& pstr_delimiter) {
    return joinInternal(pstrs, pstr_delimiter.cStr(), pstr_delimiter.length());
}

PStringA PStringA::join(std::vector<PStringA>& pstrs, const char* cstr_delimiter) {
    return joinInternal(pstrs, cstr_delimiter, strlen(cstr_delimiter));
}

PStringA PStringA::join(std::vector<PStringA>& pstrs, const std::string& stlstr_delimiter) {
    return joinInternal(pstrs, stlstr_delimiter.c_str(), stlstr_delimiter.length());
}

PStringA PStringA::join(std::vector<char*>& cstrs, const PStringA& pstr_delimiter) {
    return joinInternal(cstrs, pstr_delimiter.cStr(), pstr_delimiter.length());
}

PStringA PStringA::join(std::vector<char*>& cstrs, const char* cstr_delimiter) {
    return joinInternal(cstrs, cstr_delimiter, strlen(cstr_delimiter));
}

PStringA PStringA::join(std::vector<char*>& cstrs, const std::string& stlstr_delimiter) {
    return joinInternal(cstrs, stlstr_delimiter.c_str(), stlstr_delimiter.length());
}

PStringA PStringA::join(std::vector<std::string>& pstrs, const PStringA& pstr_delimiter) {
    return joinInternal(pstrs, pstr_delimiter.cStr(), pstr_delimiter.length());
}

PStringA PStringA::join(std::vector<std::string>& pstrs, const char* cstr_delimiter) {
    return joinInternal(pstrs, cstr_delimiter, strlen(cstr_delimiter));
}

PStringA PStringA::join(std::vector<std::string>& pstrs, const std::string& stlstr_delimiter) {
    return joinInternal(pstrs, stlstr_delimiter.c_str(), stlstr_delimiter.length());
}

PStringA PStringA::replaceAllInternal(const char* cstr_find, size_t cstr_find_len, const char* cstr_replace, size_t cstr_replace_len) const {
    size_t total_len, cur, last, pos, len;
    char* dst;
    std::vector<size_t> positions;
    std::shared_ptr<char> newbuf;
    
    last = 0;
    while (findInternal(cstr_find, cstr_find_len, last, &pos)) {
        positions.push_back(pos);
        last = pos + cstr_find_len;
    }
    if (positions.size() == 0) {
        return *this;
    }

    total_len = 0;
    cur = 0;
    for (size_t i = 0; i < positions.size(); ++i) {
        total_len += positions[i] - cur;
        total_len += cstr_replace_len;
        cur = positions[i] + cstr_find_len;
    }
    total_len += length() - cur;
    
    newbuf = std::shared_ptr<char>(new char[total_len + 1], std::default_delete<char[]>());
    dst = newbuf.get();
    cur = 0;
    last = 0;
    for (size_t i = 0; i < positions.size(); ++i) {
        pos = positions[i];
        len = pos - last;
        memcpy(dst + cur, cStr() + last, len);
        cur += len;
        memcpy(dst + cur, cstr_replace, cstr_replace_len);
        cur += cstr_replace_len;
        last = pos + cstr_find_len;
    }
    len = length() - last;
    memcpy(dst + cur, cStr() + last, len);
    cur += len;
    dst[total_len] = '\0';
    
    return PStringA(newbuf, total_len);
}

PStringA PStringA::replaceAll(const PStringA& pstr_find, const PStringA& pstr_replace) const {
    return replaceAllInternal(pstr_find.cStr(), pstr_find.length(), pstr_replace.cStr(), pstr_replace.length());
}

PStringA PStringA::replaceAll(const PStringA& pstr_find, const char* cstr_replace) const {
    return replaceAllInternal(pstr_find.cStr(), pstr_find.length(), cstr_replace, strlen(cstr_replace));
}

PStringA PStringA::replaceAll(const PStringA& pstr_find, const std::string& stlstr_replace) const {
    return replaceAllInternal(pstr_find.cStr(), pstr_find.length(), stlstr_replace.c_str(), stlstr_replace.length());
}

PStringA PStringA::replaceAll(const char* cstr_find, const PStringA& pstr_replace) const {
    return replaceAllInternal(cstr_find, strlen(cstr_find), pstr_replace.cStr(), pstr_replace.length());
}

PStringA PStringA::replaceAll(const char* cstr_find, const char* cstr_replace) const {
    return replaceAllInternal(cstr_find, strlen(cstr_find), cstr_replace, strlen(cstr_replace));
}

PStringA PStringA::replaceAll(const char* cstr_find, const std::string& stlstr_replace) const {
    return replaceAllInternal(cstr_find, strlen(cstr_find), stlstr_replace.c_str(), stlstr_replace.length());
}

PStringA PStringA::replaceAll(const std::string& stlstr_find, const PStringA& pstr_replace) const {
    return replaceAllInternal(stlstr_find.c_str(), stlstr_find.length(), pstr_replace.cStr(), pstr_replace.length());
}

PStringA PStringA::replaceAll(const std::string& stlstr_find, const char* cstr_replace) const {
    return replaceAllInternal(stlstr_find.c_str(), stlstr_find.length(), cstr_replace, strlen(cstr_replace));
}

PStringA PStringA::replaceAll(const std::string& stlstr_find, const std::string& stlstr_replace) const {
    return replaceAllInternal(stlstr_find.c_str(), stlstr_find.length(), stlstr_replace.c_str(), stlstr_replace.length());
}

std::ostream& operator<<(std::ostream& os, const PStringA& pstr) {
    return os << pstr.cStr();
}