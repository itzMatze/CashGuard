#pragma once
#include <cstdint>
#include <limits>
#include <random>

uint32_t get_seed();

class RandomGenerator
{
public:
  RandomGenerator(uint32_t seed = get_seed());
  std::mt19937& get_generator();
  float random_float(float lower_bound = 0.0f, float upper_bound = 1.0f);
  int32_t random_int32(int32_t lower_bound = std::numeric_limits<int32_t>::min(), int32_t upper_bound = std::numeric_limits<int32_t>::max());

private:
  std::uniform_real_distribution<float> distribution;
  std::mt19937 generator;
};

namespace rng
{
float random_float(float lower_bound = 0.0f, float upper_bound = 1.0f);

// generate random int in [lower_bound, upper_bound - 1]
int32_t random_int32(int32_t lower_bound = std::numeric_limits<int32_t>::min(), int32_t upper_bound = std::numeric_limits<int32_t>::max());

// the instance is not thread safe
// this function should not be used most of the time
// instead create your own instance of the RandomGenerator
RandomGenerator& get_instance();
} // namespace rng

