#include <iostream>
#include <iomanip>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include "Parser.h"
#include "Optimizer.h"
#include "Compiler.h"
#include "ExecutableMemory.h"
#include "Program.h"
#include "ProgramInputParser.h"
#include "Clock.h"

#define CELL_COUNT 32768

void timeHandler(const std::tuple<std::string&, size_t&, bool&, bool&>& values, char** argv)
{
    std::get<3>(values) = true;
}

void helpHandler(const std::tuple<std::string&, size_t&, bool&, bool&>& values, char** argv)
{
    std::get<2>(values) = true;
}

void cellsHandler(const std::tuple<std::string&, size_t&, bool&, bool&>& values, char** argv)
{
    size_t cellCount = strtoull(argv[0], nullptr, 10);
    if (cellCount != 0) { std::get<1>(values) = cellCount; }
}

void defaultHandler(const std::tuple<std::string&, size_t&, bool&, bool&>& values, char* argv)
{
    std::string& fileName = std::get<0>(values);
    if (fileName.empty()) { fileName = argv; }
}

int main(int argc, char** argv)
{
    /*/

    std::string s;
    std::cin >> s;
    run(s, 10);
    
    /*/

    std::string filePath;
    size_t cellCount = CELL_COUNT;
    bool help = false;
    bool showTime = false;

    try
    {
        ProgramInputParser(
            {
                { "cells", { cellsHandler, 1 } },
                { "time", { timeHandler, 0 } },
                { "help", { helpHandler, 0 } }
            },
            defaultHandler,
            filePath, cellCount, help, showTime
        ).parse(argc, argv);
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return -1;
    }

    if (help)
    {
        std::cout <<
            "Usage: Brainfuck.exe [options] file\n"
            "Options:\n"
            "  -cells <count>\tselect cell count (default is " << CELL_COUNT << ")\n"
            "  -time\t\t\tdisblay running time\n"
            "  -help\t\t\tdisplay help\n";
        return 0;
    }

    if (filePath.empty())
    {
        std::cerr << "No input file\n";
        return -1;
    }

    std::ifstream fileStream(filePath);
    if (!fileStream)
    {
        std::cerr << "Couldn't open file " << filePath << '\n';
        return -1;
    }

    std::string code;
    std::string buf;
    while (!fileStream.eof())
    {
        std::getline(fileStream, buf);
        code += buf;
    }

    fileStream.close();

    try
    {
        std::vector<Instruction> instructionCode;
        Parser().parse(code, &instructionCode);
        Optimizer().optimize(&instructionCode);

        /*for (const Instruction& instruction : instructionCode)
        {
            switch (instruction.code)
            {
            case ADD: std::cout << "ADD " << (int64_t)instruction.data0 << '\n'; break;
            case MOV: std::cout << "MOV " << (int64_t)instruction.data0 << '\n'; break;
            case SET0: std::cout << "SET0\n"; break;
            case NEG: std::cout << "NEG\n"; break;
            case ADDMUL: std::cout << "ADDMUL " << (int64_t)instruction.data0 << ' ' << (int64_t)instruction.data1 << '\n'; break;
            case INP: std::cout << "INP\n"; break;
            case OTP: std::cout << "OTP\n"; break;
            case LOOPS: std::cout << "LOOPS\n"; break;
            case LOOPE: std::cout << "LOOPE\n"; break;
            }
        }

        std::cout << std::endl;*/

        std::vector<uint8_t> machineCode;
        unsigned int ep = Compiler().compile(instructionCode, &machineCode);
        instructionCode.clear();
        ExecutableMemory mem(machineCode.begin(), machineCode.end(), ep);
        Program prog(mem, cellCount);

        if (showTime)
        {
            Clock clock;
            prog.run();
            float t = clock.getElapsedTime();
            std::cout << "\nRunning time: " << t << " sec\n";
        }
        else
        {
            prog.run();
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return -1;
    }

    /**/

    return 0;
}

/*uint8_t num_input()
{
    unsigned int n;
    std::cin >> n;
    return (uint8_t)n;
}

void do_next_command(uint8_t *cells, int *current_cell, std::string *prg, size_t *index, bool *num_out)
{
    switch ((*prg)[*index])
    {
    case '+': cells[*current_cell]++; break;
    case '-': if (cells[*current_cell] > 0u) { cells[*current_cell]--; } break;
    case '>': if (*current_cell < CELLS_COUNT - 1) { (*current_cell)++; } break;
    case '<': if (*current_cell > 0u) { (*current_cell)--; } break;
    case 'L': *num_out = false; break;
    case 'N': *num_out = true; break;
    case '.':
        if (*num_out) { std::cout << "out: " << (int)cells[*current_cell] << '\n'; }
        else { std::cout << cells[*current_cell]; }
        break;
    case ',':
        std::cout << "input: ";
        if (*num_out) { cells[*current_cell] = num_input(); }
        else { std::cin >> cells[*current_cell]; }
        break;
    case '[':
        if (cells[*current_cell] == 0u)
        {
            for (; (*prg)[*index] != ']'; (*index)++);
            break;
        }

        (*index)++;
        size_t start_index = *index;

        while (cells[*current_cell] > 0u)
        {
            (*index) = start_index;
            for (; (*prg)[*index] != ']'; (*index)++)
            {
                do_next_command(cells, current_cell, prg, index, num_out);
            }
        }
    }
}*/
