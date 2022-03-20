#include <iostream>
#include <utility>

#include "serializable_string_dict.h"

using namespace std;

pair<shared_ptr<char>,size_t> test_serialize() {
	SerializableStringDict sdict("root");

	sdict["haha"] = "hehe";
	sdict["hihi"] = "huhu";
	sdict["yes"]["yes"] = "yes!!!";
	sdict["no"]["no"] = "no!!!";
	sdict["good"]["great"] = "fantastic";
	sdict["black"] = "lily";

	printf("<Before serialization>\n");
	sdict.print();
	return make_pair(sdict.serialize(), sdict.serialized_length());
}

void test_deserialize(std::shared_ptr<char> data, size_t datalen) {
	std::shared_ptr<SerializableStringDict> des;

	if (!(des = SerializableStringDict::deserialize_data(data.get(), datalen))) {
		printf("Failed to deserialize\n");
	}
	else {
		printf("<After serialization>\n");
		des.get()->print();
	}
}

int main(int argc, char** argv){
	auto p = test_serialize();
	test_deserialize(p.first, p.second);
	return 0;
}