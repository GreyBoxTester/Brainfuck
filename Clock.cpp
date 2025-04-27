#include "Clock.h"

Clock::Clock()
{
	lastTime = high_resolution_clock::now();
}

float Clock::restart()
{
	auto deltaTime = high_resolution_clock::now() - lastTime;
	lastTime += deltaTime;
	return (float)duration_cast<milliseconds>(deltaTime).count() * 0.001f;
}

float Clock::getElapsedTime()
{
	return (float)duration_cast<milliseconds>(high_resolution_clock::now() - lastTime).count() * 0.001f;
}
