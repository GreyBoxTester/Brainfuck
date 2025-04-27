#include "SearchFunctions.h"

template<size_t count>
static constexpr size_t repeatedOne = (repeatedOne<count - 1> << 8) | 1;
template<>
static constexpr size_t repeatedOne<0> = 0;

static constexpr size_t magicNum0 = repeatedOne<sizeof(size_t)>;
static constexpr size_t magicNum1 = magicNum0 << 7;

uint8_t* __cdecl searchLeft0(uint8_t* arr)
{
    for (; ((size_t)arr & (sizeof(size_t) - 1)) != 0; arr--)
    {
        if (*arr == 0) { return arr; }
    }

    if (*arr == 0) { return arr; }

    size_t* qwPtr = (size_t*)arr;
    qwPtr--;

    while (true)
    {
        size_t val = (*qwPtr - ((*qwPtr ^ magicNum0) & magicNum0)) & ((~(*qwPtr)) & magicNum1);
        if (val)
        {
            return ((uint8_t*)qwPtr) + (sizeof(size_t) - 1 - _lzcnt_u64(val) / 8);
        }

        qwPtr--;
    }
}

uint8_t* __cdecl searchRight0(uint8_t* arr)
{
    for (; ((size_t)arr & (sizeof(size_t) - 1)) != 0; arr++)
    {
        if (*arr == 0) { return arr; }
    }

    size_t* qwPtr = (size_t*)arr;

    while (true)
    {
        size_t val = (*qwPtr - ((*qwPtr ^ magicNum0) & magicNum0)) & ((~(*qwPtr)) & magicNum1);
        if (val)
        {
            return ((uint8_t*)qwPtr) + (_tzcnt_u64(val) / 8);
        }

        qwPtr++;
    }
}
