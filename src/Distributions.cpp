#include <algorithm> // std::shuffle
#include <cstdlib>
#include <random>

#include "Distributions.h"

std::mt19937 mt{ std::random_device{}() };

namespace Beleif
{

	float true_random() { return uniform_distribution_float(0.00001f, 10.f); };

	// TODO: add true randomness and also indifference?
	float mild_aversion(float degrees_of_freedom)
	{
		static std::chi_squared_distribution<float> chi2{ degrees_of_freedom };
		return chi2(mt);
	}


	float mild_affinity() { return std::max(0.00001f, 10 - mild_aversion()); };


	float strong_aversion(float degrees_of_freedom)
	{
		static std::chi_squared_distribution<float> chi2{ degrees_of_freedom };
		return chi2(mt);
	}


	float strong_affinity() { return std::max(0.00001f, 10 - strong_aversion()); };
}


int uniform_distribution_int(int min, int max)
{
	static constexpr double fraction{ 1.0 / (RAND_MAX + 1.0) };
	return min + static_cast<int>((max - min + 1) * (std::rand() * fraction));
}


void shuffle_vector(std::vector<int>& vector)
{
	std::shuffle(vector.begin(), vector.end(), mt);
}
