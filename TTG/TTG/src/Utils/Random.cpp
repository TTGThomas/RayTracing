#include "Random.h"

thread_local std::random_device TTG::Random::m_rd;
thread_local std::mt19937 TTG::Random::m_gen(m_rd());
