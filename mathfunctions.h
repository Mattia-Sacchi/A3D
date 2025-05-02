#ifndef _MATH_FUNCTIONS_H_
#define _MATH_FUNCTIONS_H_
#include <vector>

std::vector<float> generateGaussianKernel(int size, float sigma);
std::vector<float> generateGaussianEdgeKernel(int size, float power);
#endif
