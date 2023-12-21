#include "Random.h"

std::random_device TTG::Random::m_rd;
std::mt19937 TTG::Random::m_gen(m_rd());
