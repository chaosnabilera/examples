#include <iostream>

#include "serializable_string_dict.h"

int main(int argc, char** argv){
	SerializableStringDict sdict;

	sdict["sparta"] = "this is sparta!";
	sdict["korea"] = "a country in asia!";
	sdict["testing"] = "hahahahahaha!";

	sdict["this"]["is"]["what?"] = "gogogogogo";

	return 0;
}