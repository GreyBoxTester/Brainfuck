#pragma once
#include <stdint.h>

#define ADD    0
#define MOV    1
#define SET0   2
#define NEG    3
#define ADDMUL 4
#define SRCH0  5
#define INP    6
#define OTP    7
#define LOOPS  8
#define LOOPE  9

//SRCH0
#define SEARCH_LEFT  0
#define SEARCH_RIGHT 1

class Instruction
{
public:
	Instruction(char code, size_t data0 = 0, size_t data1 = 0);
public:
	char code;
	size_t data0;
	size_t data1;
};