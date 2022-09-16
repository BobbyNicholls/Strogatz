#ifndef STROG_DISTRIBUTIONS_H
#define STROG_DISTRIBUTIONS_H

#include<vector>

float mild_affinity();
float strong_affinity();
void shuffle_vector(std::vector<int>& vector);
float strong_aversion(float degrees_of_freedom = 0.2f);
float mild_aversion(float degrees_of_freedom = 1.f);
float uniform_distribution_float(int min, int max);
int uniform_distribution_int(int min, int max);

#endif