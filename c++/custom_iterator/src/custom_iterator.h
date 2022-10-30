#pragma once
#ifndef __CUSTOM_ITERATOR_H__
#define __CUSTOM_ITERATOR_H__

#include <cstddef>
#include <iterator>
#include <vector>

class CustomClass {
public:
	class Iterator {
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = int;
		using difference_type = std::ptrdiff_t;
		using pointer = int*;
		using reference = int&;

		Iterator(int* ptr) : iPtr(ptr) {}

		Iterator& operator++();    // prefix increment
		Iterator operator++(int);  // postfix increment

		friend bool operator==(const Iterator& lhs, const Iterator& rhs);
		friend bool operator!=(const Iterator& lhs, const Iterator& rhs);

		reference operator*() const;
		pointer operator->() const;
		
	private:
		pointer iPtr;
	};

	CustomClass(int size);
	Iterator begin();
	Iterator end();
	
private:
	std::vector<int> cData;
};

#endif