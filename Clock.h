#pragma once
#include <chrono>

using namespace std::chrono;

class Clock
{
public:
	Clock();
	float restart();
	float getElapsedTime();
private:
	std::chrono::time_point<high_resolution_clock> lastTime;
};
