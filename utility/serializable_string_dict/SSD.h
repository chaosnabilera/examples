#pragma once

#include <cstring>
#include <memory>
#include <map>
#include <string>
#include <vector>
#include <unordered_map>

#define IS_CONTAINER 1

class SSDReader{
public:
	SSDReader();
	SSDReader(char* ibuf, size_t ibuflen, size_t ioffset);
	~SSDReader();

	operator bool() const { return src != nullptr; };
	SSDReader operator[](std::string& key);
	
	char* get_raw();
	unsigned long long get_total_size();
	unsigned long long get_content_size();

private:
	char* src;
	size_t srclen;
	size_t offset;
	unsigned int attributes;
	std::string name;
	unsigned long long contentlen;
	char* content;
	std::unordered_map<std::string, std::shared_ptr<SSDReader>> child;
};

class SSDCreator{
public:
	SSDCreator();
	~SSDCreator();
	char* serialize();
	unsigned long long calc_total_len();
	unsigned int attributes;
	std::string name;
	std::vector<char> content;
	std::unordered_map<std::string, std::shared_ptr<SSDCreator>> child;
private:
	void serialize_rec(char* dst, size_t dstlen);
};