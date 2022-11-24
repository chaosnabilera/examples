#pragma once
#ifndef __PSTRING_H__
#define __PSTRING_H__

/*
* Note : PString is immutable string class
*/

#include <memory>
#include <string>
#include <vector>

class PStringA;
class PStringW;

class PStringA {
    
public:
    PStringA();
    PStringA(const char* cstr);
    PStringA(const char* cstr, size_t cstr_len);
    PStringA(const std::string& stlstr);
    PStringA(const PStringA& other);
    
    ~PStringA();
    
    PStringA& operator=(const PStringA& other);
    PStringA operator=(const char* cstr);
    PStringA operator=(const std::string& stlstr);

    PStringA operator+(const PStringA& other);
    PStringA operator+(const char* cstr);
    PStringA operator+(const std::string& stlstr);
    
    PStringA& operator+=(const PStringA& other);
    PStringA& operator+=(const char* cstr);
    PStringA& operator+=(const std::string& stlstr);
    
    bool operator==(const PStringA& other) const;
    bool operator==(const char* cstr) const;
    bool operator==(const std::string& stlstr) const;
    
    bool operator!=(const PStringA& other) const;
    bool operator!=(const char* cstr) const;
    bool operator!=(const std::string& stlstr) const;

    bool operator<(const PStringA& other) const;
    bool operator<(const char* cstr) const;
    bool operator<(const std::string& stlstr) const;

    bool operator<=(const PStringA& other) const;
    bool operator<=(const char* cstr) const;
    bool operator<=(const std::string& stlstr) const;

    bool operator>(const PStringA& other) const;
    bool operator>(const char* cstr) const;
    bool operator>(const std::string& stlstr) const;

    bool operator>=(const PStringA& other) const;
    bool operator>=(const char* cstr) const;
    bool operator>=(const std::string& stlstr) const;
    
    char operator[](size_t index) const;
    
    const char* cStr() const;
    std::string stlStr() const;
    
    size_t length() const;
    void clear();

    PStringA substr(size_t index, size_t length) const;
    PStringA substrFrom(size_t start_index) const;
    PStringA substrTo(size_t end_index) const;
    PStringA substrSlice(size_t start_index, size_t end_index) const;

    bool find(const char* cstr_to_find, size_t cstr_to_find_len, size_t find_from, size_t* out_index) const;
    bool find(const char* cstr_to_find, size_t find_from, size_t* out_index) const;
    bool find(const char* cstr_to_find, size_t* out_index) const;
    bool find(const PStringA& pstr_to_find, size_t find_from, size_t* out_index) const;
    bool find(const PStringA& pstr_to_find, size_t* out_index) const;
    bool find(const std::string& stlstr_to_find, size_t find_from, size_t* out_index) const;
    bool find(const std::string& stlstr_to_find, size_t* out_index) const;

    // find starting at arr[p = rfind_from]
    // if we can't find it, --p
    bool rfind(const char* cstr_to_find, size_t cstr_to_find_len, size_t rfind_from, size_t* out_index) const;
    bool rfind(const char* cstr_to_find, size_t rfind_from, size_t* out_index) const;
    bool rfind(const char* cstr_to_find, size_t* out_index) const;
    bool rfind(const PStringA& pstr_to_find, size_t rfind_from, size_t* out_index) const;
    bool rfind(const PStringA& pstr_to_find, size_t* out_index) const;
    bool rfind(const std::string& stlstr_to_find, size_t rfind_from, size_t* out_index) const;
    bool rfind(const std::string& stlstr_to_find, size_t* out_index) const;

    void findAll(const PStringA& pstr_to_find, std::vector<size_t>* out_indices) const;
    void findAll(const char* cstr_to_find, std::vector<size_t>* out_indices) const;
    void findAll(const std::string& stlstr_to_find, std::vector<size_t>* out_indices) const;

    size_t count(const PStringA& pstr_to_find) const;
    size_t count(const char* cstr_to_find) const;
    size_t count(const std::string& stlstr_to_find) const;

    bool startsWith(const PStringA& pstr_to_find) const;
    bool startsWith(const char* cstr_to_find) const;
    bool startsWith(const std::string& stlstr_to_find) const;

    bool endsWith(const PStringA& pstr_to_find) const;
    bool endsWith(const char* cstr_to_find) const;
    bool endsWith(const std::string& stlstr_to_find) const;

    PStringA lower() const;
    PStringA upper() const;
    
    PStringA strip() const;
    PStringA stripLeft() const;
    PStringA stripRight() const;

    void split(std::vector<PStringA>* out_pstrs, const PStringA& pstr_delimiter) const;
    void split(std::vector<PStringA>* out_pstrs, const char* cstr_delimiter) const;
    void split(std::vector<PStringA>* out_pstrs, const std::string& stlstr_delimiter) const;

    PStringA replaceAll(const PStringA& pstr_to_find,      const PStringA&    pstr_to_replace) const;
    PStringA replaceAll(const PStringA& pstr_to_find,      const char*        cstr_to_replace) const;
    PStringA replaceAll(const PStringA& pstr_to_find,      const std::string& stlstr_to_replace) const;
    PStringA replaceAll(const char* cstr_to_find,          const PStringA&    pstr_to_replace) const;
    PStringA replaceAll(const char* cstr_to_find,          const char*        cstr_to_replace) const;
    PStringA replaceAll(const char* cstr_to_find,          const std::string& stlstr_to_replace) const;
    PStringA replaceAll(const std::string& stlstr_to_find, const PStringA&    pstr_to_replace) const;
    PStringA replaceAll(const std::string& stlstr_to_find, const char*        cstr_to_replace) const;
    PStringA replaceAll(const std::string& stlstr_to_find, const std::string& stlstr_to_replace) const;

    static PStringA join(std::vector<PStringA>& pstrs,    const PStringA&    pstr_delimiter);
    static PStringA join(std::vector<PStringA>& pstrs,    const char*        cstr_delimiter);
    static PStringA join(std::vector<PStringA>& pstrs,    const std::string& stlstr_delimiter);
    static PStringA join(std::vector<char*>& cstrs,       const PStringA&    pstr_delimiter);
    static PStringA join(std::vector<char*>& cstrs,       const char*        cstr_delimiter);
    static PStringA join(std::vector<char*>& cstrs,       const std::string& stlstr_delimiter);
    static PStringA join(std::vector<std::string>& pstrs, const PStringA&    pstr_delimiter);
    static PStringA join(std::vector<std::string>& pstrs, const char*        cstr_delimiter);
    static PStringA join(std::vector<std::string>& pstrs, const std::string& stlstr_delimiter);
    
private:
    PStringA(std::shared_ptr<char> char_buf, size_t char_buf_len);
    void createNewSelfPlusOther(const char* cstr, size_t cstrlen, std::shared_ptr<char>* out_cbuf, size_t* out_pstrlen);
    
    std::shared_ptr<char> cBuf;
    size_t pStrLen; // cBuf length = pStrLen + 1
};

#endif