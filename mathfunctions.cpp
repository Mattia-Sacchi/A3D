#include "mathfunctions.h" 
#include <vector>
#include <cmath>
#include <stdexcept>

std::vector < float> generateGaussianKernel(int size, float sigma) {
if(size % 2 == 0) {
	throw std::invalid_argument("Kernel size must be odd");
}

int halfSize = size / 2;
std::vector<std::vector<float>> kernel(size, std::vector<float>(size));
float sum = 0.0f;

const float PI = 3.14159265358979323846f;
float sigma2   = 2.0f * sigma * sigma;

for(int y = -halfSize; y <= halfSize; ++y) {
	for(int x = -halfSize; x <= halfSize; ++x) {
		float value                        = std::exp(-(x * x + y * y) / sigma2) / (PI * sigma2);
		kernel[y + halfSize][x + halfSize] = value;
		sum += value;
	}
}

// Normalize the kernel
for(int y = 0; y < size; ++y)
	for(int x = 0; x < size; ++x)
		kernel[y][x] /= sum;

std::vector<float> data;

for(int y = 0; y < size; ++y)
	for(int x = 0; x < size; ++x)
		data.push_back(kernel[y][x]);
return data;
}

std::vector<float> generateGaussianEdgeKernel(int size, float power) {
	if(size % 2 == 0) {
		throw std::invalid_argument("Size must be odd");
	}

	int halfSize = size / 2;
	std::vector<std::vector<float>> matrix(size, std::vector<float>(size));
	float maxDist = std::sqrt(2.0f) * halfSize; // Diagonal corner distance

	float sum = 0.0f;

	for(int y = -halfSize; y <= halfSize; ++y) {
		for(int x = -halfSize; x <= halfSize; ++x) {
			float dist                         = std::sqrt(x * x + y * y);
			float value                        = std::pow(dist / maxDist, power); // scale to [0,1] and raise to power
			matrix[y + halfSize][x + halfSize] = value;
			sum += value;
		}
	}

	// Optional normalization
	for(int y = 0; y < size; ++y)
		for(int x = 0; x < size; ++x)
			matrix[y][x] /= sum;

	std::vector<float> data;

	for(int y = 0; y < size; ++y)
		for(int x = 0; x < size; ++x)
			data.push_back(matrix[y][x]);
	return data;
}
