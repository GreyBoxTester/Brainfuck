#pragma once
#include <vector>
#include <string>
#include "Instruction.h"

class Parser
{
public:
	Parser();
	void parse(const std::string& str, std::vector<Instruction>* codeOut) const;
private:
	void pushRepeatable(std::vector<Instruction>* code, const Instruction& instruction) const;
};
