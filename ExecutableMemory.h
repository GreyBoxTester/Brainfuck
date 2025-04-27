#pragma once
#include <stdint.h>
#include <Windows.h>
#include <memory>
#include "NonCopyable.h"

class ExecutableMemory : NonCopyable
{
public:
	ExecutableMemory(unsigned int size, unsigned int entryPoint = 0);
	template<typename It>
	ExecutableMemory(It begin, It end, unsigned int entryPoint = 0);
	ExecutableMemory(ExecutableMemory&& other) noexcept;
	~ExecutableMemory();
	const uint8_t* entryPoint() const;
	uint8_t* data();
	const uint8_t* data() const;
	uint8_t& operator[](unsigned int index);
	const uint8_t& operator[](unsigned int index) const;
private:
	HANDLE hMem;
	uint8_t* memMap;
	unsigned int ep;
};


template<typename It>
ExecutableMemory::ExecutableMemory(It begin, It end, unsigned int entryPoint)
	: ExecutableMemory((unsigned int)(end - begin), entryPoint)
{
	if (hMem == nullptr || memMap == nullptr) { return; }
	std::copy(begin, end, memMap);
}
