#include "serializable_string_dict.h"

#include "dprintf.hpp"

using namespace std;

SerializableStringDict::SerializableStringDict(bool vld) : 
attribute(0), content(nullptr), contentlen(0), is_valid(vld) { }

SerializableStringDict::~SerializableStringDict() { }

SerializableStringDict::SerializableStringDict(const SerializableStringDict& rhs) :
	attribute(rhs.attribute), name_string(rhs.name_string), content(rhs.content),
	contentlen(rhs.contentlen), children(rhs.children), is_valid(rhs.is_valid) { }

string SerializableStringDict::name() {
	return name_string;
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
		children[key] = std::shared_ptr<SerializableStringDict>(new SerializableStringDict(true));
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

	attribute = *(unsigned int*)buf[0];
	namelen = *(unsigned int*)buf[4];
	contentlen = *(unsigned long long*)buf[8];

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
		total_length = HEADER_BYTESIZE + name_string.size();
		for (auto& p : children) {
			SerializableStringDict& child = *(p.second.get());
			total_length += child.serialized_length();
		}
	}
	else {
		total_length = HEADER_BYTESIZE + name_string.size() + contentlen;
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
	*(unsigned int*)(&buf[ATTRIBUTE_BYTESIZE]) = name_string.size();

	if (attribute & ATTRIBUTE_CONTAINER) {
		size_t offset = HEADER_BYTESIZE + name_string.size();
		size_t contentlen = 0;
		for (auto& p : children) {
			SerializableStringDict& child = *(p.second.get());
			size_t childlen = child.serialized_length();
			rec_serialize(buf + offset, childlen);
			offset += childlen;
			contentlen += childlen;
		}
		*(unsigned long long*)(&buf[ATTRIBUTE_BYTESIZE + NAMELENGTH_BYTESIZE]) = contentlen;
		memcpy(&buf[HEADER_BYTESIZE], name_string.c_str(), name_string.size());		
	}
	else {
		*(unsigned long long*)(&buf[ATTRIBUTE_BYTESIZE + NAMELENGTH_BYTESIZE]) = contentlen;
		memcpy(&buf[HEADER_BYTESIZE], name_string.c_str(), name_string.size());
		memcpy(&buf[HEADER_BYTESIZE + name_string.size()], content.get(), contentlen);
	}
}

// Only deserialize_data can call this
SerializableStringDict::SerializableStringDict(unsigned int attr, std::string& nm, char* cntnt, size_t cntntlen) :
	attribute(attr), name_string(nm), is_valid(true) {
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


/*
SSDReader::SSDReader() : 
	src(nullptr), srclen(0), offset(0), attributes(0), contentlen(0), content(nullptr) { }

SSDReader::~SSDReader(){ }

SSDReader::SSDReader(char* ibuf, size_t ibuflen, size_t ioffset) : 
	src(nullptr), srclen(0), offset(0), attributes(0), contentlen(0), content(nullptr) {
	unsigned int iattribute = 0;
	unsigned int inamelen = 0;
	unsigned long long icontentlen = 0;

	do{
		if(ioffset+8 > ibuflen) // attribute + name length
			break;

		iattribute = *(unsigned int*)(&ibuf[ioffset]);
		inamelen = *(unsigned int*)(&ibuf[ioffset+4]);
		if(inamelen == 0)
			break;

		if(ioffset+8+inamelen+8 > ibuflen)
			break;
		icontentlen = *(unsigned long long*)(&ibuf[ioffset+4+inamelen]);
		if(ioffset+8+inamelen+8+icontentlen > ibuflen)
			break;

		src = ibuf;
		srclen = ibuflen;
		offset = ioffset;
		attributes = iattribute;
		name = string(&src[offset+4],&src[offset+4+inamelen]);
		contentlen = icontentlen;
		content = &src[offset+4+4+name.size()+8];

		if(iattribute & IS_CONTAINER){
			size_t nxt_offset = offset+4+4+name.size()+8;
			while(nxt_offset < srclen){
				shared_ptr<SSDReader> new_child(new SSDReader(src,srclen,nxt_offset));
				if(!(*(new_child.get())))
					break;
				child[new_child.get()->name] = new_child;
				nxt_offset += new_child.get()->get_total_size();	
			}
		}
	} while(0);
}

SSDReader SSDReader::operator[](std::string& key){
	if(child.find(key) != child.end()){
		return *(child[key].get());
	}
	else{
		SSDReader empty;
		return empty;
	}
}

char* SSDReader::get_raw(){
	return content;
}

unsigned long long SSDReader::get_total_size(){
	return 4+4+name.size()+4+contentlen;
}

unsigned long long SSDReader::get_content_size(){
	return contentlen;
}

SSDCreator::SSDCreator(){
}

SSDCreator::~SSDCreator(){ }

unsigned long long SSDCreator::calc_total_len(){
	unsigned long long res = 0;
	if(attributes & IS_CONTAINER){
		for(auto& p:child){
			res += p.second.get()->calc_total_len();
		}
	}
	else{
		res += 4; // attributes
		res += 4; // name len
		res += name.size();
		res += 8; // content size
		res += content.size();
	}
	return res;
}

char* SSDCreator::serialize(){
	unsigned long long total_len = calc_total_len();
	char* sbuf = new char[total_len];
	serialize_rec(sbuf, 0);
	return sbuf;
}

void SSDCreator::serialize_rec(char* dst, size_t dstlen){
	size_t header_and_name = 4+4+name.size();

	*(unsigned int*)(&dst[0]) = attributes;
	*(unsigned int*)(&dst[4]) = name.size();
	memcpy(&dst[4+4], &name[0], name.size());

	if(!child.empty()){ // container
		unsigned long long x = header_and_name;
		for(auto& p:child){
			unsigned long long curlen = p.second.get()->calc_total_len();
			serialize_rec(dst+x, curlen);
			x += curlen;
		}
	}
	else{
		*(unsigned long long*)(&dst[header_and_name]) = content.size();
		memcpy(&dst[header_and_name+8], &content[0], content.size());
	}
}
*/