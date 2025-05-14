/// -----------------
/// ordered-array.cpp
/// @brief This file defines core functions of the template based Ordered_Array class.

#include "ordered-array.hpp"

#include "memory.h"

template <typename type_t>
void Ordered_Array<type_t>::Place(void *address, uint32_t max_size)
{
	array = reinterpret_cast<type_t*>(address);

	memset(reinterpret_cast<unsigned char *>(address), 0, max_size * sizeof(type_t));
	
	size = 0;
	this->max_size = max_size;
}

template <typename type_t>
type_t Ordered_Array<type_t>::Get(uint32_t i)
{
	if(i >= this->max_size)
		return this->array[this->max_size - 1];
	return this->array[i];
}

template <typename type_t>
void Ordered_Array<type_t>::Remove(uint32_t i) {
    while (i < this->size - 1) {
        this->array[i] = this->array[i + 1];
        i++;
    }
    this->size--;
}

template <typename type_t>
uint32_t Ordered_Array<type_t>::Get_Size() const
{
	return size;
}

template <typename type_t>
uint32_t Ordered_Array<type_t>::Get_Max_Size() const
{
	return max_size;
}

template class Ordered_Array<void*>;