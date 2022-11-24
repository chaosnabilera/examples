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

bool PStringA::find(const char* cstr_to_find, size_t cstr_to_find_len, size_t find_from, size_t* out_index) const {
    if (find_from >= pStrLen) {
        return false;
    }
    
    if (cstr_to_find_len > pStrLen - find_from) {
        return false;
    }
    
    const char* found = strstr(cBuf.get() + find_from, cstr_to_find);
    
    if (found == nullptr) {
        return false;
    }

    if (out_index != nullptr) {
        *out_index = found - cBuf.get();
    }

    return true;
}

bool PStringA::find(const char* cstr_to_find, size_t find_from, size_t* out_index) const {
    return find(cstr_to_find, strlen(cstr_to_find), find_from, out_index);
}

bool PStringA::find(const char* cstr_to_find, size_t* out_index) const {
    return find(cstr_to_find, strlen(cstr_to_find), 0, out_index);
}

bool PStringA::find(const PStringA& pstr_to_find, size_t find_from, size_t* out_index) const {
    return find(pstr_to_find.cBuf.get(), pstr_to_find.pStrLen, find_from, out_index);
}

bool PStringA::find(const PStringA& pstr_to_find, size_t* out_index) const {
    return find(pstr_to_find.cBuf.get(), pstr_to_find.pStrLen, 0, out_index);
}

bool PStringA::find(const std::string& stlstr_to_find, size_t find_from, size_t* out_index) const {
    return find(stlstr_to_find.c_str(), stlstr_to_find.length(), find_from, out_index);
}

bool PStringA::find(const std::string& stlstr_to_find, size_t* out_index) const {
    return find(stlstr_to_find.c_str(), stlstr_to_find.length(), 0, out_index);
}

bool PStringA::rfind(const char* cstr_to_find, size_t cstr_to_find_len, size_t rfind_from, size_t* out_index) const {
    bool result, match;
    size_t rbegin;
    
    if (rfind_from >= pStrLen) {
        return false;
    }
    
    if (cstr_to_find_len > rfind_from+1) {
        return false;
    }

    rbegin = (rfind_from < pStrLen - cstr_to_find_len) ? rfind_from : pStrLen - cstr_to_find_len;
    result = false;
    
    for (char* p = cBuf.get() + rbegin; p >= cBuf.get(); --p) {
        match = true;
        for (size_t i = 0; i < cstr_to_find_len; ++i) {
            if (cstr_to_find[i] != p[i]) {
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

bool PStringA::rfind(const char* cstr_to_find, size_t rfind_from, size_t* out_index) const {
    return rfind(cstr_to_find, strlen(cstr_to_find), rfind_from, out_index);
}

bool PStringA::rfind(const char* cstr_to_find, size_t* out_index) const {
    return rfind(cstr_to_find, strlen(cstr_to_find), pStrLen - 1, out_index);
}

bool PStringA::rfind(const PStringA& pstr_to_find, size_t rfind_from, size_t* out_index) const {
    return rfind(pstr_to_find.cBuf.get(), pstr_to_find.pStrLen, rfind_from, out_index);
}

bool PStringA::rfind(const PStringA& pstr_to_find, size_t* out_index) const {
    return rfind(pstr_to_find.cBuf.get(), pstr_to_find.pStrLen, pStrLen - 1, out_index);
}

bool PStringA::rfind(const std::string& stlstr_to_find, size_t rfind_from, size_t* out_index) const {
    return rfind(stlstr_to_find.c_str(), stlstr_to_find.length(), rfind_from, out_index); 
}

bool PStringA::rfind(const std::string& stlstr_to_find, size_t* out_index) const {
    return rfind(stlstr_to_find.c_str(), stlstr_to_find.length(), pStrLen - 1, out_index);
}

