#ifndef STROG_DISTRIBUTIONS_H
#define STROG_DISTRIBUTIONS_H

#include <random>

std::mt19937 mt{ std::random_device{}() };

int uniform_distribution(int min, int max) 
{
	std::uniform_int_distribution uniform{ min, max };
	return uniform(mt);
}

#endif