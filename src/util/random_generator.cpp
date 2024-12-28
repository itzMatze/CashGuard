#include "util/random_generator.hpp"
#include <limits>

uint64_t get_seed()
{
  std::random_device rd;
  std::uniform_int_distribution<uint64_t> dis(0, std::numeric_limits<uint64_t>::max());
  return dis(rd);
}

template<> double RandomGenerator<double>::random_double(double lower_bound, double upper_bound)
{
	return distribution(generator) * (upper_bound - lower_bound) + lower_bound;
}

template<> int64_t RandomGenerator<double>::random_int64(int64_t lower_bound, int64_t upper_bound)
{
	return random_double(lower_bound, upper_bound);
}

namespace rng
{
RandomGenerator<double> rg(get_seed());

float random_float(float lower_bound, float upper_bound)
{
  return rg.random_float(lower_bound, upper_bound);
}

double random_double(double lower_bound, double upper_bound)
{
  return rg.random_double(lower_bound, upper_bound);
}

// generate random int in [lower_bound, upper_bound - 1]
int32_t random_int32(int32_t lower_bound, int32_t upper_bound)
{
  return rg.random_int32(lower_bound, upper_bound);
}

int64_t random_int64(int64_t lower_bound, int64_t upper_bound)
{
  return rg.random_int64(lower_bound, upper_bound);
}
} // namespace rng
