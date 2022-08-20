#ifndef STROG_DISTRIBUTIONS_H
#define STROG_DISTRIBUTIONS_H

float mild_affinity();
float strong_affinity();
float strong_aversion(float degrees_of_freedom = 0.2f);
float mild_aversion(float degrees_of_freedom = 1.f);
float uniform_distribution_float(int min, int max);
int uniform_distribution_int(int min, int max);

#endif