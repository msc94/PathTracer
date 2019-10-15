#include "utils.h"

#include <random>

float utils::randomFloat(float low, float high) {
    static thread_local std::random_device rd;
    static thread_local std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(low, high);
    return dis(gen);
}
