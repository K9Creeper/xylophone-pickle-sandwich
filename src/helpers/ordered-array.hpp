/// -----------------
/// ordered_array.hpp
/// @brief This file declares the template based Ordered_Array class.

#pragma once

#include <stdint.h>

template <typename type_t>
class Ordered_Array{
	protected:
		type_t* array = nullptr;

		uint32_t size = 0;
		uint32_t max_size = 0;
	public:
		void Place(void *address, uint32_t max_size);

		type_t Get(uint32_t i);
		void Remove(uint32_t i);

		uint32_t Get_Size()const;
		uint32_t Get_Max_Size()const;
};