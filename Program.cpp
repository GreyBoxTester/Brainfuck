#include "Program.h"

typedef void(__cdecl* funcPtr)(
	uint8_t*, 
	int(__cdecl*)(int), 
	int(__cdecl*)(),
	uint8_t*(__cdecl*)(uint8_t*),
	uint8_t*(__cdecl*)(uint8_t*)
);

Program::Program(ExecutableMemory& executableMem, size_t cellsCount)
	: executableMem(&executableMem), cells(new uint8_t[cellsCount])
{
	memset(cells, 0, cellsCount);
}

Program::~Program()
{
	if (cells != nullptr) { delete[] cells; }
}

void Program::run()
{
	((funcPtr)executableMem->entryPoint())(cells, putchar, getchar, searchRight0, searchLeft0);
}
