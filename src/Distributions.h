#ifndef STROG_DISTRIBUTIONS_H
#define STROG_DISTRIBUTIONS_H

int uniform_distribution_int(int min, int max);
float uniform_distribution_float(int min, int max);
float strong_aversion(float degrees_of_freedom = 0.2f);
float strong_affinity();

#endif