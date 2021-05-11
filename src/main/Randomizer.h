#ifndef PONG_RANDOMIZER_H
#define PONG_RANDOMIZER_H

#include <glm/glm.hpp>

#include <chrono>
#include <ctime>
#include <random>

class Randomizer {
public:
    glm::vec2 randomDirection() {
        return glm::normalize(glm::vec2(
            randomPositiveOrNegative(),
            randomY() * randomPositiveOrNegative()));
    }

private:
    float random() {
        return floatDistro(defaultEngine);
    }

    float randomY() {
        return random() * 0.5 + 0.25;
    }

    float randomPositiveOrNegative() {
        return random() > 0.5 ? 1.0f : -1.0f;
    }

    std::default_random_engine defaultEngine {static_cast<std::uint32_t>(std::time(0))};
    std::uniform_real_distribution<float> floatDistro{0.0, 1.0};
};

#endif // PONG_RANDOMIZER_H
