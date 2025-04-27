#pragma once
#include <vector>
#include <stack>
#include <list>
#include "Instruction.h"

class LoopInfo
{
public:
	LoopInfo(std::vector<Instruction>::iterator start, std::vector<Instruction>::iterator end, int64_t movCount = 0)
		: start(start), end(end), movCount(movCount)
	{}

public:
	std::vector<Instruction>::iterator start;
	std::vector<Instruction>::iterator end;
	int64_t movCount;
};

class Optimizer
{
public:
	Optimizer();
	void optimize(std::vector<Instruction>* instructionCode) const;
private:
	void makeLoopsInfo(std::vector<Instruction>* instructionCode, std::vector<LoopInfo>* loopsInfo) const;
	//void removeUnneccessaryBrackets(std::vector<Instruction>* instructionCode) const;

};
