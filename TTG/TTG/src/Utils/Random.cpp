#include "Random.h"

std::random_device TTG::Random::rd;
std::mt19937 TTG::Random::gen(rd());
