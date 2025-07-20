/// ---------------
/// ordered-array.c
/// @brief This file defines core functions of the ordered_array structure.

#include <ordered-array.h>

#include <memory.h>

void ordered_array_init(ordered_array_t* ordered_array, uint32_t max_size, ordered_array_is_less_than_handle_t handle){
    if(!ordered_array)
        return;

    ordered_array->array = NULL;

    ordered_array->size = 0;
    ordered_array->max_size = max_size;

    ordered_array->is_less_than_handle = handle;
}

void ordered_array_place(ordered_array_t* ordered_array, void* address){
    ordered_array->array = (type_t*)(address);

	memset((uint8_t*)address, 0, ordered_array->max_size * sizeof(type_t));
	
	ordered_array->size = 0;
}

bool ordered_array_insert(ordered_array_t* ordered_array, type_t item){
    if(!ordered_array || !ordered_array->is_less_than_handle)
        return false;
    
    uint32_t idx = 0;

    static type_t tmp1;
    static type_t tmp2;

    while(idx < ordered_array->size && ordered_array->is_less_than_handle(ordered_array->array[idx], item))
    {
        ++idx;
    }

    if(idx + 1 > ordered_array->max_size)
    {
        return false;
    }

    if (idx == ordered_array->size)
    {
        ordered_array->array[ordered_array->size] = item;

        ordered_array->size++;
    }
    else
    {
        tmp1 = ordered_array->array[idx];

        ordered_array->array[idx] = item;

        while (idx < ordered_array->size)
        {
            idx++;

            tmp2 = ordered_array->array[idx];

            ordered_array->array[idx] = tmp1;

            tmp1 = tmp2;
        }

        ordered_array->size++;
    }

    return true;
}

void ordered_array_remove(ordered_array_t* ordered_array, uint32_t index){
    while (index < ordered_array->size - 1) {
        ordered_array->array[index] = ordered_array->array[index + 1];
        index++;
    }
    ordered_array->size--;
}

type_t ordered_array_get(ordered_array_t* ordered_array, uint32_t index){
    if (index >= ordered_array->max_size) {
        return NULL;
    }
    
	return ordered_array->array[index];
}