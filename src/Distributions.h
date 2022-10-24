#ifndef STROG_DISTRIBUTIONS_H
#define STROG_DISTRIBUTIONS_H

#include <algorithm> // std::shuffle
#include <cstdlib>
#include <random>
#include<vector>

float mild_affinity();
float strong_affinity();
void shuffle_vector(std::vector<int>& vector);
float strong_aversion(float degrees_of_freedom = 0.2f);
float mild_aversion(float degrees_of_freedom = 1.f);
int uniform_distribution_int(int min, int max);

template <typename num_t>
float uniform_distribution_float(num_t min, num_t max)
{
	// static used for efficiency, so we only calculate this value once
	static constexpr double fraction{ 1.0 / (RAND_MAX + 1.0) };
	return min + static_cast<float>((max - min) * (std::rand() * fraction));
}

#endif