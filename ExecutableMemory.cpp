#include "ExecutableMemory.h"

ExecutableMemory::ExecutableMemory(unsigned int size, unsigned int entryPoint)
	: hMem(nullptr), memMap(nullptr), ep(entryPoint)
{
	hMem = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_EXECUTE_READWRITE, 0, size, NULL);
	if (hMem == nullptr) { return; }

	memMap = (uint8_t*)MapViewOfFile(hMem, FILE_MAP_ALL_ACCESS | FILE_MAP_EXECUTE, 0, 0, size);
	if (memMap == nullptr) { return; }
}

ExecutableMemory::ExecutableMemory(ExecutableMemory&& other) noexcept
	: hMem(other.hMem), memMap(other.memMap), ep(other.ep)
{
	other.hMem = nullptr;
	other.memMap = nullptr;
	other.ep = 0;
}

ExecutableMemory::~ExecutableMemory()
{
	if (memMap != nullptr) { UnmapViewOfFile(memMap); }
	if (hMem != nullptr) { CloseHandle(hMem); }
}

const uint8_t* ExecutableMemory::entryPoint() const
{
	return memMap + ep;
}

uint8_t* ExecutableMemory::data()
{
	return memMap;
}

const uint8_t* ExecutableMemory::data() const
{
	return memMap;
}

uint8_t& ExecutableMemory::operator[](unsigned int index)
{
	return memMap[index];
}

const uint8_t& ExecutableMemory::operator[](unsigned int index) const
{
	return memMap[index];
}
