#pragma once
#include <string>
#include "spdlog/spdlog.h"

#define CG_ASSERTIONS

#define CG_THROW(...) \
{ \
	spdlog::critical(__VA_ARGS__); \
	std::string s(__FILE__); \
	s.append(": "); \
	s.append(std::to_string(__LINE__)); \
	spdlog::throw_spdlog_ex(s); \
}

#if defined(CG_ASSERTIONS)
#define CG_ASSERT(X, ...) if (!(X)) CG_THROW(__VA_ARGS__);
#else
#define CG_ASSERT(X, ...) X
#endif

namespace cglog = spdlog;
