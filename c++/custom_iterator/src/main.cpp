#include <iostream>
#include "custom_iterator.h"

int main()
{
	CustomClass hey(20);
	
	for (auto i : hey) {
		printf("%d\n", i);
	}
}