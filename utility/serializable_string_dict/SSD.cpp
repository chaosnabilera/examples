#include "SSD.h"

using namespace std;

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