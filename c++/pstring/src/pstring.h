#pragma once
#ifndef __PSTRING_H__
#define __PSTRING_H__

/*
* Note : PString is immutable string class
*/

#include <memory>
#include <ostream>
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

    bool find(const PStringA& pstr_find, size_t find_from, size_t* out_index) const;
    bool find(const PStringA& pstr_find, size_t* out_index) const;
    bool find(const char* cstr_find, size_t find_from, size_t* out_index) const;
    bool find(const char* cstr_find, size_t* out_index) const;
    bool find(const std::string& stlstr_find, size_t find_from, size_t* out_index) const;
    bool find(const std::string& stlstr_find, size_t* out_index) const;

    // find starting at arr[p = rfind_from]
    // if we can't find it, --p
    bool rfind(const PStringA& pstr_find, size_t rfind_from, size_t* out_index) const;
    bool rfind(const PStringA& pstr_find, size_t* out_index) const;
    bool rfind(const char* cstr_find, size_t rfind_from, size_t* out_index) const;
    bool rfind(const char* cstr_find, size_t* out_index) const;
    bool rfind(const std::string& stlstr_find, size_t rfind_from, size_t* out_index) const;
    bool rfind(const std::string& stlstr_find, size_t* out_index) const;

    // Note : findAll is different from count
    //        "aaa".findAll("aa") = [0,1] ([a{a]a} - intersection at the middle is counted)
    void findAll(const PStringA& pstr_find, std::vector<size_t>* out_indices) const;
    void findAll(const char* cstr_find, std::vector<size_t>* out_indices) const;
    void findAll(const std::string& stlstr_find, std::vector<size_t>* out_indices) const;

    // Note : count is different from findAll
    //        "aaa".count("aa") = 1 ([aa]a - intersection is ignored)
    size_t count(const PStringA& pstr_find) const;
    size_t count(const char* cstr_find) const;
    size_t count(const std::string& stlstr_find) const;

    bool startsWith(const PStringA& pstr_find) const;
    bool startsWith(const char* cstr_find) const;
    bool startsWith(const std::string& stlstr_find) const;

    bool endsWith(const PStringA& pstr_find) const;
    bool endsWith(const char* cstr_find) const;
    bool endsWith(const std::string& stlstr_find) const;

    PStringA lower() const;
    PStringA upper() const;
    
    PStringA strip() const;
    PStringA stripLeft() const;
    PStringA stripRight() const;

    // out_pstrs will be cleared
    // if delimiter is empty, out_pstrs will contain string with only one element
    void split(const PStringA& pstr_delimiter, std::vector<PStringA>* out_pstrs) const;
    void split(const char* cstr_delimiter, std::vector<PStringA>* out_pstrs) const;
    void split(const std::string& stlstr_delimiter, std::vector<PStringA>* out_pstrs) const;
    // split with isspace : different from above (does not use splitInternal) 
    void split(std::vector<PStringA>* out_pstrs) const;

    static PStringA join(std::vector<PStringA>& pstrs, const PStringA& pstr_delimiter);
    static PStringA join(std::vector<PStringA>& pstrs, const char* cstr_delimiter);
    static PStringA join(std::vector<PStringA>& pstrs, const std::string& stlstr_delimiter);
    static PStringA join(std::vector<char*>& cstrs, const PStringA& pstr_delimiter);
    static PStringA join(std::vector<char*>& cstrs, const char* cstr_delimiter);
    static PStringA join(std::vector<char*>& cstrs, const std::string& stlstr_delimiter);
    static PStringA join(std::vector<std::string>& pstrs, const PStringA& pstr_delimiter);
    static PStringA join(std::vector<std::string>& pstrs, const char* cstr_delimiter);
    static PStringA join(std::vector<std::string>& pstrs, const std::string& stlstr_delimiter);

    PStringA replaceAll(const PStringA& pstr_find,      const PStringA&    pstr_replace) const;
    PStringA replaceAll(const PStringA& pstr_find,      const char*        cstr_replace) const;
    PStringA replaceAll(const PStringA& pstr_find,      const std::string& stlstr_replace) const;
    PStringA replaceAll(const char* cstr_find,          const PStringA&    pstr_replace) const;
    PStringA replaceAll(const char* cstr_find,          const char*        cstr_replace) const;
    PStringA replaceAll(const char* cstr_find,          const std::string& stlstr_replace) const;
    PStringA replaceAll(const std::string& stlstr_find, const PStringA&    pstr_replace) const;
    PStringA replaceAll(const std::string& stlstr_find, const char*        cstr_replace) const;
    PStringA replaceAll(const std::string& stlstr_find, const std::string& stlstr_replace) const;
    
private:
    PStringA(std::shared_ptr<char> char_buf, size_t char_buf_len);
    void createNewSelfPlusOther(const char* cstr, size_t cstrlen, std::shared_ptr<char>* out_cbuf, size_t* out_pstrlen);
    bool findInternal(const char* cstr_find, size_t cstr_find_len, size_t find_from, size_t* out_index) const;
    bool rfindInternal(const char* cstr_find, size_t cstr_find_len, size_t rfind_from, size_t* out_index) const;
    void findAllInternal(const char* cstr_find, size_t cstr_find_len, std::vector<size_t>* out_indices) const;
    size_t countInternal(const char* cstr_find, size_t cstr_find_len) const;
    bool startsWithInternal(const char* cstr_find, size_t cstr_find_len) const;
    bool endsWithInternal(const char* cstr_find, size_t cstr_find_len) const;
    void splitInternal(const char* cstr_delimiter, size_t cstr_delimiter_len, std::vector<PStringA>* out_pstrs) const;
    
    static PStringA joinInternal(std::vector<PStringA>& pstrs, const char* cstr_delimiter, size_t cstr_delimiter_len);
    static PStringA joinInternal(std::vector<char*>& cstrs, const char* cstr_delimiter, size_t cstr_delimiter_len);
    static PStringA joinInternal(std::vector<std::string>& stlstrs, const char* cstr_delimiter, size_t cstr_delimiter_len);

    PStringA replaceAllInternal(const char* cstr_find, size_t cstr_find_len, const char* cstr_replace, size_t cstr_replace_len) const;
    
    std::shared_ptr<char> cBuf;
    size_t pStrLen; // cBuf length = pStrLen + 1
};

std::ostream& operator<<(std::ostream& os, const PStringA& pstr);

#endif