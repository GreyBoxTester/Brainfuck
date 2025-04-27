#pragma once
#include <vector>
#include "Instruction.h"

class Compiler
{
public:
	Compiler();
	unsigned int compile(const std::vector<Instruction>& instructionCode, std::vector<uint8_t>* machineCode);
private:
	void start(std::vector<uint8_t>* machineCode);
	void add(std::vector<uint8_t>* machineCode, uint8_t val);
	void mov(std::vector<uint8_t>* machineCode, size_t val);
	void set0(std::vector<uint8_t>* machineCode);
	void neg(std::vector<uint8_t>* machineCode);
	void addmul(std::vector<uint8_t>* machineCode, size_t multiplier, size_t offset);
	void srch0(std::vector<uint8_t>* machineCode, bool right);
	void inp(std::vector<uint8_t>* machineCode);
	void otp(std::vector<uint8_t>* machineCode);
	void loops(std::vector<uint8_t>* machineCode);
	void loope(std::vector<uint8_t>* machineCode);
	void end(std::vector<uint8_t>* machineCode);
private:
	template<typename T>
	void addVal(std::vector<uint8_t>* machineCode, T val) const;
	template<typename T>
	void addVal(std::vector<uint8_t>* machineCode, unsigned int index, T val) const;
private:
	std::vector<unsigned int> jumpAddresses;
};


template<typename T>
void Compiler::addVal(std::vector<uint8_t>* machineCode, T val) const
{
	machineCode->insert(machineCode->end(), (uint8_t*)&val, ((uint8_t*)&val) + sizeof(T));
}

template<typename T>
void Compiler::addVal(std::vector<uint8_t>* machineCode, unsigned int index, T val) const
{
	if (machineCode->size() - index < sizeof(T)) { return; }
	memcpy(&machineCode->at(index), &val, sizeof(T));
}
