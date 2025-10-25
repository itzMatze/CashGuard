#pragma once
#include <cstdint>
#include <limits>
#include <random>

uint64_t get_seed();

template<typename Precision = float>
class RandomGenerator
{
public:
	RandomGenerator(uint64_t seed = get_seed()) : distribution(std::uniform_real_distribution<Precision>(0.0f, 1.0f)), generator(std::mt19937(seed))
	{}

	float random_float(float lower_bound = 0.0f, float upper_bound = 1.0f)
	{
		return distribution(generator) * (upper_bound - lower_bound) + lower_bound;
	}

	int32_t random_int32(int32_t lower_bound = std::numeric_limits<int32_t>::min(), int32_t upper_bound = std::numeric_limits<int32_t>::max())
	{
		return random_float(lower_bound, upper_bound);
	}

	// only available with double precision
	int64_t random_int64(int64_t lower_bound = std::numeric_limits<int64_t>::min(), int64_t upper_bound = std::numeric_limits<int64_t>::max());
	double random_double(double lower_bound = 0.0f, double upper_bound = 1.0f);

private:
	std::uniform_real_distribution<Precision> distribution;
	std::mt19937 generator;
};

namespace rng
{
float random_float(float lower_bound = 0.0f, float upper_bound = 1.0f);
double random_double(double lower_bound = 0.0f, double upper_bound = 1.0f);

// generate random int in [lower_bound, upper_bound - 1]
int32_t random_int32(int32_t lower_bound = std::numeric_limits<int32_t>::min(), int32_t upper_bound = std::numeric_limits<int32_t>::max());
int64_t random_int64(int64_t lower_bound = std::numeric_limits<int64_t>::min(), int64_t upper_bound = std::numeric_limits<int64_t>::max());
} // namespace rng

