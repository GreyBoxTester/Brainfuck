#pragma once
#include <stdexcept>
#include <unordered_map>

template<typename... T>
struct CommandHandler
{
    CommandHandler(void (*func)(const std::tuple<T&...>&, char**), unsigned int argCount) : func(func), argCount(argCount) {}

    void (*func)(const std::tuple<T&...>&, char**);
    int argCount;
};


template<typename... T>
class ProgramInputParser
{
public:
	ProgramInputParser(std::unordered_map<std::string, CommandHandler<T...>> commandHandlers, void (*defaultHandler)(const std::tuple<T&...>&, char*), T&... values);
	void parse(int argc, char** argv) const;
private:
	std::unordered_map<std::string, CommandHandler<T...>> commandHandlers;
    void (*defaultHandler)(const std::tuple<T&...>&, char*);
    std::tuple<T&...> values;
};


template<typename... T>
ProgramInputParser<T...>::ProgramInputParser(std::unordered_map<std::string, CommandHandler<T...>> commandHandlers, void (*defaultHandler)(const std::tuple<T&...>&, char*), T&... values)
	: commandHandlers(std::move(commandHandlers)), defaultHandler(defaultHandler), values(values...)
{}

template<typename... T>
void ProgramInputParser<T...>::parse(int argc, char** argv) const
{
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            std::string s(argv[i] + 1);
            auto it = commandHandlers.find(s);
            if (it != commandHandlers.end())
            {
                const CommandHandler<T...>& handler = it->second;

                int iStart = i;
                i++;
                for (; i < argc; i++)
                {
                    int count = i - iStart;
                    if (count > handler.argCount || argv[i][0] == '-') { i--; break; }
                    else if (count == handler.argCount) { break; }
                }

                if (i - iStart < handler.argCount) { throw std::runtime_error(std::string("Too few arguments for command ") + argv[iStart]); }

                handler.func(values, argv + iStart + 1);
            }
            else
            {
                throw std::runtime_error(std::string("Unknown command: ") + argv[i]);
            }
        }
        else if (defaultHandler != nullptr)
        {
            defaultHandler(values, argv[i]);
        }
    }
}
