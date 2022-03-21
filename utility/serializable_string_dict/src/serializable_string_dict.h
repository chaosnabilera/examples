#pragma once

#include <cstring>
#include <memory>
#include <map>
#include <string>
#include <vector>
#include <unordered_map>

const unsigned int ATTRIBUTE_CONTAINER = (1 << 31);

const size_t ATTRIBUTE_BYTESIZE = sizeof(unsigned int);
const size_t NAMELENGTH_BYTESIZE = sizeof(unsigned int);
const size_t CONTENTLENGTH_BYTESIZE = sizeof(unsigned long long);
const size_t HEADER_BYTESIZE = ATTRIBUTE_BYTESIZE + NAMELENGTH_BYTESIZE + CONTENTLENGTH_BYTESIZE;

class SerializableStringDict;

struct ByteArr {
	char* arr;
	size_t arrlen;
};

class SerializableStringDict {
public:
	SerializableStringDict(std::string name = "", bool is_valid = false);
	~SerializableStringDict();
	SerializableStringDict(const SerializableStringDict& rhs);

	std::string name();
	char* raw_content();
	size_t content_bytelen();

	template<typename T>
	T convert_to() {
		T res;
		size_t copy_len = min(sizeof(T), contentlen);
		memset(&res, 0, sizeof(T));
		memcpy(&res, content.get(), copy_len);
		return res;
	}

	std::shared_ptr<SerializableStringDict> find_child(std::string& name);
	void add_child(std::shared_ptr<SerializableStringDict>& new_child);
	void set_content(char* src, size_t srclen);

	void print(int lv = 0);

	operator bool() const;

	// high level operator to get/set. may be slower than lower level api
	SerializableStringDict& operator[](char* key);
	SerializableStringDict& operator[](const char* key);
	SerializableStringDict& operator[](std::string key);
	void operator=(int rhs);
	void operator=(unsigned int rhs);
	void operator=(long long rhs);
	void operator=(unsigned long long rhs);
	void operator=(char* rhs);
	void operator=(const char* rhs);
	void operator=(std::string rhs);
	void operator=(ByteArr rhs);

	// serialization related
	size_t serialized_length();
	std::shared_ptr<char> serialize();
	static std::shared_ptr<SerializableStringDict> deserialize_data(char* buf, size_t buflen);

private:
	SerializableStringDict(unsigned int attribute, std::string& name, char* content = nullptr, size_t contentlen = 0);
	void rec_serialize(char* buf, size_t buflen);
	
	unsigned int attribute;
	std::string name_str;
	std::shared_ptr<char> content;
	size_t contentlen;
	std::unordered_map<std::string, std::shared_ptr<SerializableStringDict>> children;
	bool is_valid;
};