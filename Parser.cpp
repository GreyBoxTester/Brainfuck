#include "Parser.h"

Parser::Parser() {}

void Parser::parse(const std::string& str, std::vector<Instruction>* codeOut) const
{
    size_t bracketCount = 0;

    for (char c : str)
    {
        switch (c)
        {
        case '+': pushRepeatable(codeOut, { ADD, 1 }); break;
        case '-': pushRepeatable(codeOut, { ADD, (size_t)(-1) }); break;
        case '>': pushRepeatable(codeOut, { MOV, 1 }); break;
        case '<': pushRepeatable(codeOut, { MOV, (size_t)(-1) }); break;
        case ',': codeOut->emplace_back(INP); break;
        case '.': codeOut->emplace_back(OTP); break;
        case '[':
            bracketCount++;
            codeOut->emplace_back(LOOPS);
            break;
        case ']':
            if (bracketCount == 0) { throw std::exception("Too many closing brackets"); }
            bracketCount--;
            codeOut->emplace_back(LOOPE);
            break;
        }
    }

    if (bracketCount > 0) { throw std::exception("Too many opening brackets"); }
}

void Parser::pushRepeatable(std::vector<Instruction>* code, const Instruction& instruction) const
{
    if (!code->empty()) 
    { 
        if (code->back().code == instruction.code) 
        { 
            code->back().data0 += instruction.data0; 
            if (code->back().data0 == 0) { code->pop_back(); }
            return;
        } 
    }

    code->push_back(instruction);
}
