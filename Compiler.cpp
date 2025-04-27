#include "Compiler.h"

Compiler::Compiler() {}

unsigned int Compiler::compile(const std::vector<Instruction>& instructionCode, std::vector<uint8_t>* machineCode)
{
    /*start(machineCode);
    //mov rax, QWORD PTR [rbp+48]
    machineCode->insert(machineCode->end(), { 0x48, 0x8B, 0x45, 0x30 });
    end(machineCode);*/
#ifdef _WIN64
    jumpAddresses.clear();

    start(machineCode);

	for (const Instruction& instruction : instructionCode)
	{
        switch (instruction.code)
        {
        case ADD:
            add(machineCode, (uint8_t)instruction.data0);
            break;
        case MOV:
            mov(machineCode, instruction.data0);
            break;
        case SET0:
            set0(machineCode);
            break;
        case NEG:
            neg(machineCode);
            break;
        case ADDMUL:
            addmul(machineCode, instruction.data0, instruction.data1);
            break;
        case SRCH0:
            srch0(machineCode, instruction.data0 == SEARCH_RIGHT);
            break;
        case INP:
            inp(machineCode);
            break;
        case OTP:
            otp(machineCode);
            break;
        case LOOPS:
            loops(machineCode);
            break;
        case LOOPE:
            loope(machineCode);
            break;
        }
	}

    end(machineCode);
#endif

    return 0;
}

void Compiler::start(std::vector<uint8_t>* machineCode)
{
    //push rbp
    //mov rbp, rsp
    //push rdx
    //push r8
    //machineCode->insert(machineCode->end(), { 0x55, 0x48, 0x89, 0xE5, 0x52, 0x41, 0x50 });

    //push rbp
    //mov rbp, rsp
    //push rdx
    //push r8
    //push r9
    machineCode->insert(machineCode->end(), { 0x55, 0x48, 0x89, 0xE5, 0x52, 0x41, 0x50, 0x41, 0x51 });
}

void Compiler::add(std::vector<uint8_t>* machineCode, uint8_t val)
{
    //add BYTE PTR [rcx], data(byte)
    machineCode->insert(machineCode->end(), { 0x80, 0x01 });
    machineCode->push_back(val);
}

void Compiler::mov(std::vector<uint8_t>* machineCode, size_t val)
{
    if (val <= INT8_MAX)
    {
        //add rcx, data
        machineCode->insert(machineCode->end(), { 0x48, 0x83, 0xC1 });
        machineCode->push_back((uint8_t)val);
    }
    else if (-((short)val) <= INT8_MAX)
    {
        //sub rcx, data
        machineCode->insert(machineCode->end(), { 0x48, 0x83, 0xe9 });
        machineCode->push_back(-((int8_t)val));
    }
    else if (val <= INT_MAX)
    {
        //add rcx, data
        machineCode->insert(machineCode->end(), { 0x48, 0x81, 0xc1 });
        addVal<int>(machineCode, (int)val);
    }
    else if (-((int64_t)val) <= INT_MAX)
    {
        //sub rcx, data
        machineCode->insert(machineCode->end(), { 0x48, 0x81, 0xe9 });
        addVal<int>(machineCode, -((int)val));
    }
    else
    {
        //movabs rax, data
        //add rcx, rax
        machineCode->insert(machineCode->end(), { 0x48, 0xb8 });
        addVal<size_t>(machineCode, val);
        machineCode->insert(machineCode->end(), { 0x48, 0x01, 0xc1 });
    }
}

void Compiler::set0(std::vector<uint8_t>* machineCode)
{
    //mov BYTE PTR [rcx], 0
    machineCode->insert(machineCode->end(), { 0xc6, 0x01, 0x00 });
}

void Compiler::neg(std::vector<uint8_t>* machineCode)
{
    //neg BYTE PTR [rcx]
    machineCode->insert(machineCode->end(), { 0xf6, 0x19 });
}

void Compiler::addmul(std::vector<uint8_t>* machineCode, size_t multiplier, size_t offset)
{
    //mov al, BYTE PTR[rcx]
    machineCode->insert(machineCode->end(), { 0x8a, 0x01 });

    if (multiplier == (size_t)(-1))
    {
        //neg al
        machineCode->insert(machineCode->end(), { 0xf6, 0xd8 });
    }
    else if (multiplier != 1)
    {
        switch (__popcnt16((uint8_t)multiplier))
        {
        case 1:
            //shl al, shiftVal
            machineCode->insert(machineCode->end(), { 0xc0, 0xe0 });
            machineCode->push_back((uint8_t)15 - (uint8_t)__lzcnt16((uint8_t)multiplier));
            break;
        default:
            //mov dl, multiplier
            //mul dl
            machineCode->push_back(0xb2);
            machineCode->push_back((uint8_t)multiplier);
            machineCode->insert(machineCode->end(), { 0xf6, 0xe2 });
        }
    }

    if (offset <= INT8_MAX || -((short)offset) <= INT8_MAX)
    {
        //add BYTE PTR[rcx+offset], al
        //or
        //add BYTE PTR[rcx-offset], al
        machineCode->insert(machineCode->end(), { 0x00, 0x41 });
        machineCode->push_back((uint8_t)offset);
    }
    else if (offset <= INT_MAX || -((int64_t)offset) <= INT_MAX)
    {
        //add BYTE PTR[rcx+offset], al
        //or
        //add BYTE PTR[rcx-offset], al
        machineCode->insert(machineCode->end(), { 0x00, 0x81 });
        addVal<int>(machineCode, (int)offset);
    }
    else
    {
        //movabs rbx, offset
        //add BYTE PTR[rcx+rbx], al
        machineCode->insert(machineCode->end(), { 0x48, 0xbb });
        addVal<size_t>(machineCode, offset);
        machineCode->insert(machineCode->end(), { 0x00, 0x04, 0x19 });
    }
}

void Compiler::srch0(std::vector<uint8_t>* machineCode, bool right)
{
    //sub rsp, 32
    machineCode->insert(machineCode->end(), { 0x48, 0x83, 0xEC, 0x20 });

    if (right)
    {
        //call QWORD PTR [rbp-24]
        machineCode->insert(machineCode->end(), { 0xFF, 0x55, 0xE8 });
    }
    else
    {
        //call QWORD PTR [rbp+48]
        machineCode->insert(machineCode->end(), { 0xFF, 0x55, 0x30 });
    }

    //add rsp, 32
    //mov rcx, rax
    machineCode->insert(machineCode->end(), { 0x48, 0x83, 0xC4, 0x20, 0x48, 0x89, 0xC1 });
}

void Compiler::inp(std::vector<uint8_t>* machineCode)
{
    //push rcx
    //call QWORD PTR [rbp-16]
    //pop rcx
    //mov BYTE PTR [rcx], al
    //machineCode->insert(machineCode->end(), { 0x51, 0xFF, 0x55, 0xF0, 0x59, 0x88, 0x01 });

    //push rcx
    //sub rsp, 32
    //call QWORD PTR [rbp-16]
    //add rsp, 32
    //pop rcx
    //mov BYTE PTR [rcx], al
    machineCode->insert(machineCode->end(), { 0x51, 0x48, 0x83, 0xEC, 0x20, 0xFF, 0x55, 0xF0, 0x48, 0x83, 0xC4, 0x20, 0x59, 0x88, 0x01 });
}

void Compiler::otp(std::vector<uint8_t>* machineCode)
{
    //push rcx
    //sub rsp, 32
    //mov cl, BYTE PTR [rcx]
    //call QWORD PTR [rbp-8]
    //add rsp, 32
    //pop rcx
    machineCode->insert(machineCode->end(), { 0x51, 0x48, 0x83, 0xEC, 0x20, 0x8A, 0x09, 0xFF, 0x55, 0xF8, 0x48, 0x83, 0xC4, 0x20, 0x59 });
}

void Compiler::loops(std::vector<uint8_t>* machineCode)
{
    //jmp address
    machineCode->insert(machineCode->end(), { 0xe9, 0x00, 0x00, 0x00, 0x00 });
    jumpAddresses.push_back((unsigned int)machineCode->size());
}

void Compiler::loope(std::vector<uint8_t>* machineCode)
{
    //cmp BYTE PTR [rcx], 0
    //jne address
    unsigned int jumpAddress = jumpAddresses.back();
    jumpAddresses.pop_back();
    addVal<unsigned int>(machineCode, jumpAddress - sizeof(unsigned int), (unsigned int)(machineCode->size() - jumpAddress));
    machineCode->insert(machineCode->end(), { 0x80, 0x39, 0x00, 0x0F, 0x85 });
    addVal<unsigned int>(machineCode, (unsigned int)(jumpAddress - machineCode->size() - 4));
}

void Compiler::end(std::vector<uint8_t>* machineCode)
{
    //mov rsp, rbp
    //pop rbp
    //ret
    machineCode->insert(machineCode->end(), { 0x48, 0x89, 0xec, 0x5d, 0xc3 });
}
