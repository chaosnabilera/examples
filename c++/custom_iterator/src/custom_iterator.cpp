/*
* https://www.internalpointers.com/post/writing-custom-iterators-modern-cpp
*/

#include "custom_iterator.h"

CustomClass::Iterator& CustomClass::Iterator::operator++()
{
	++iPtr;
	return *this;
}

CustomClass::Iterator CustomClass::Iterator::operator++(int)
{
	// advance the iterator and return the previous value
	Iterator tmp(*this);
	operator++();
	return tmp;
}

CustomClass::Iterator::reference CustomClass::Iterator::operator*() const
{
	return *iPtr;
}

CustomClass::Iterator::pointer CustomClass::Iterator::operator->() const
{
	return iPtr;
}

// friend functions are not member function!
bool operator==(const CustomClass::Iterator& lhs, const CustomClass::Iterator& rhs)
{
	return lhs.iPtr == rhs.iPtr;
}

bool operator!=(const CustomClass::Iterator& lhs, const CustomClass::Iterator& rhs)
{
	return lhs.iPtr != rhs.iPtr;
}


CustomClass::CustomClass(int size) {
	cData.resize(size);
	for (int i = 0; i < size; ++i)
		cData[i] = i;
}

CustomClass::Iterator CustomClass::begin()
{
	return Iterator(&cData[0]);
}

CustomClass::Iterator CustomClass::end()
{
	// using &cData[cData.size()] is undefined behavior and will crash
	return Iterator(&cData[0] + cData.size());
}