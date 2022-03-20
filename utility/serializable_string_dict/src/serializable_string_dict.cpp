#include "serializable_string_dict.h"

#include "dprintf.hpp"

using namespace std;

SerializableStringDict::SerializableStringDict(string nm, bool vld) : 
attribute(0), name_str(nm), content(nullptr), contentlen(0), is_valid(vld) { }

SerializableStringDict::~SerializableStringDict() { }

SerializableStringDict::SerializableStringDict(const SerializableStringDict& rhs) :
	attribute(rhs.attribute), name_str(rhs.name_str), content(rhs.content),
	contentlen(rhs.contentlen), children(rhs.children), is_valid(rhs.is_valid) { }

string SerializableStringDict::name() {
	return name_str;
}

char* SerializableStringDict::raw_content() {
	return content.get();
}

size_t SerializableStringDict::content_bytelen() {
	return contentlen;
}

shared_ptr<SerializableStringDict> SerializableStringDict::find_child(std::string& name) {
	shared_ptr<SerializableStringDict> empty(nullptr);
	if (children.find(name) == children.end())
		return empty;
	else
		return children[name];
}

void SerializableStringDict::add_child(std::shared_ptr<SerializableStringDict>& new_child) {
	children[new_child.get()->name()] = new_child;
}

void SerializableStringDict::set_content(char* src, size_t srclen) {
	// if it was a container, turn it into leaf node
	is_valid = true;
	attribute = 0;
	children.clear();
	// do setting
	content.reset();
	content = shared_ptr<char>(new char[srclen], default_delete<char[]>());
	contentlen = srclen;
	memcpy(content.get(), src, contentlen);
}

void SerializableStringDict::print(int lv) {
	for (int i = 0; i < lv; ++i)
		printf("\t");
	printf("[%s]", name_str.c_str());

	if (attribute & ATTRIBUTE_CONTAINER) {
		printf("[container : %zu items]\n", children.size());
		for (auto& p : children) {
			SerializableStringDict& child = *(p.second.get());
			child.print(lv + 1);
		}
	}
	else {
		shared_ptr<char> print_buf(new char[contentlen + 1], default_delete<char[]>());
		memcpy(print_buf.get(), content.get(), contentlen);
		print_buf.get()[contentlen] = '\0';

		printf("[content : %llu bytes]", contentlen);
		printf("[%s]\n", print_buf.get());
	}
}

SerializableStringDict::operator bool() const {
	return is_valid;
}

SerializableStringDict& SerializableStringDict::operator[](char* key) {
	string skey(key);
	return this->operator[](skey);
}

SerializableStringDict& SerializableStringDict::operator[](const char* key) {
	string skey(key);
	return this->operator[](skey);
}

SerializableStringDict& SerializableStringDict::operator[](std::string key) {
	if (!(attribute & ATTRIBUTE_CONTAINER)) {
		attribute = ATTRIBUTE_CONTAINER;
		content.reset();
		contentlen = 0;
	}

	if (children.find(key) == children.end()) {
		children[key] = std::shared_ptr<SerializableStringDict>(new SerializableStringDict(key, true));
	}

	return *(children[key].get());
}

void SerializableStringDict::operator=(int rhs) {
	set_content((char*) & rhs, sizeof(int));
}
void SerializableStringDict::operator=(unsigned int rhs) {
	set_content((char*)&rhs, sizeof(unsigned int));
}

void SerializableStringDict::operator=(long long rhs) {
	set_content((char*)&rhs, sizeof(long long));
}

void SerializableStringDict::operator=(unsigned long long rhs) {
	set_content((char*)&rhs, sizeof(unsigned long long));
}

void SerializableStringDict::operator=(char* rhs) {
	set_content(rhs, strlen(rhs));
}

void SerializableStringDict::operator=(const char* rhs) {
	set_content((char*)rhs, strlen(rhs));
}

void SerializableStringDict::operator=(string rhs) {
	set_content((char*)rhs.c_str(), rhs.size());
}

void SerializableStringDict::operator=(ByteArr rhs) {
	set_content(rhs.arr, rhs.arrlen);
}

// header: attribute(4) + name length(4) + content length (8)
// header + name + content

shared_ptr< SerializableStringDict> SerializableStringDict::deserialize_data(char* buf, size_t buflen) {
	unsigned int attribute = 0, namelen = 0;
	unsigned long long contentlen = 0;
	shared_ptr<SerializableStringDict> result(nullptr);
	shared_ptr<SerializableStringDict> empty(nullptr);

	if (buflen < HEADER_BYTESIZE) {
		dprintf("buflen is too small: %zu < %d", buflen, HEADER_BYTESIZE);
		return empty;
	}

	attribute = *(unsigned int*)(&buf[0]);
	namelen = *(unsigned int*)(&buf[ATTRIBUTE_BYTESIZE]);
	contentlen = *(unsigned long long*)(&buf[ATTRIBUTE_BYTESIZE+NAMELENGTH_BYTESIZE]);

	if (namelen == 0) {
		dprintf("nameless dict is not allowed");
		return empty;
	}

	if ((unsigned long long)buflen != (HEADER_BYTESIZE + namelen + contentlen)) {
		dprintf("buffer size does not match actual content");
		return empty;
	}

	string name(&buf[HEADER_BYTESIZE], &buf[HEADER_BYTESIZE + namelen]);

	if (attribute & ATTRIBUTE_CONTAINER) {
		result = shared_ptr<SerializableStringDict>(new SerializableStringDict(attribute, name));

		size_t offset = HEADER_BYTESIZE + namelen;
		bool valid = true;
		while (offset < buflen) {
			if (offset + HEADER_BYTESIZE > buflen) {
				dprintf("Child at offset : %zu is too short", offset);
				valid = false;
				break;
			}
			unsigned int child_namelen = *(unsigned int*)(&buf[offset + ATTRIBUTE_BYTESIZE]);
			unsigned long long child_contentlen = *(unsigned long long*)(&buf[offset + ATTRIBUTE_BYTESIZE + NAMELENGTH_BYTESIZE]);
			if (offset + HEADER_BYTESIZE + child_namelen + child_contentlen > buflen) {
				dprintf("Child at offset : %zu is too long", offset);
				valid = false;
				break;
			}
			auto child = deserialize_data(&buf[offset], HEADER_BYTESIZE + child_namelen + child_contentlen);

			result.get()->add_child(child);
			offset += (HEADER_BYTESIZE + child_namelen + child_contentlen);
		}

		if (!valid)
			result = empty;
	}
	else {
		result = shared_ptr<SerializableStringDict>(
			new SerializableStringDict(attribute, name, &buf[HEADER_BYTESIZE + namelen], contentlen));
	}

	return result;
}

size_t SerializableStringDict::serialized_length() {
	size_t total_length = 0;
	if (attribute & ATTRIBUTE_CONTAINER) {
		total_length = HEADER_BYTESIZE + name_str.size();
		for (auto& p : children) {
			SerializableStringDict& child = *(p.second.get());
			total_length += child.serialized_length();
		}
	}
	else {
		total_length = HEADER_BYTESIZE + name_str.size() + contentlen;
	}
	return total_length;
}

shared_ptr<char> SerializableStringDict::serialize() {
	size_t buflen = serialized_length();
	shared_ptr<char> result(new char[buflen], default_delete<char[]>());
	rec_serialize(result.get(), buflen);
	return result;
}

void SerializableStringDict::rec_serialize(char* buf, size_t buflen) {
	*(unsigned int*)(&buf[0]) = attribute;
	*(unsigned int*)(&buf[ATTRIBUTE_BYTESIZE]) = name_str.size();
	memcpy(&buf[HEADER_BYTESIZE], name_str.c_str(), name_str.size());

	if (attribute & ATTRIBUTE_CONTAINER) {
		size_t offset = HEADER_BYTESIZE + name_str.size();
		size_t contentlen = 0;
		for (auto& p : children) {
			SerializableStringDict& child = *(p.second.get());
			size_t childlen = child.serialized_length();
			child.rec_serialize(buf + offset, childlen);
			offset += childlen;
			contentlen += childlen;
		}
		*(unsigned long long*)(&buf[ATTRIBUTE_BYTESIZE + NAMELENGTH_BYTESIZE]) = contentlen;
	}
	else {
		*(unsigned long long*)(&buf[ATTRIBUTE_BYTESIZE + NAMELENGTH_BYTESIZE]) = contentlen;
		memcpy(&buf[HEADER_BYTESIZE + name_str.size()], content.get(), contentlen);
	}
}

// Only deserialize_data can call this
SerializableStringDict::SerializableStringDict(unsigned int attr, std::string& nm, char* cntnt, size_t cntntlen) :
	attribute(attr), name_str(nm), is_valid(true) {
	if (attribute & ATTRIBUTE_CONTAINER) {
		content = nullptr;
		contentlen = 0;
	}
	else {
		contentlen = cntntlen;
		content = shared_ptr<char>(new char[contentlen], default_delete<char[]>());
		memcpy(content.get(), cntnt, contentlen);
	}
}