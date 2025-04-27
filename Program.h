#pragma once
#include <stdio.h>
#include "ExecutableMemory.h"
#include "SearchFunctions.h"

class Program
{
public:
	Program(ExecutableMemory& executableMem, size_t cellsCount);
	~Program();
	void run();
private:
	ExecutableMemory* executableMem;
	uint8_t* cells;
};
