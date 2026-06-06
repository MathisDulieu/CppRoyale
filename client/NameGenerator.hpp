#pragma once
#include <string>
#include <vector>
#include <random>
#include <chrono>

class NameGenerator {
public:
    static std::string generate() {
        static const std::vector<std::string> adjectives = {
            "Angry", "Sleepy", "Grumpy", "Sneaky", "Clumsy",
            "Fluffy", "Tiny", "Mighty", "Wobbly", "Spicy",
            "Lazy", "Hyper", "Funky", "Gloomy", "Bouncy",
            "Chunky", "Dizzy", "Fuzzy", "Jumpy", "Nerdy"
        };

        static const std::vector<std::string> nouns = {
            "Goblin", "Panda", "Wizard", "Banana", "Ninja",
            "Potato", "Dragon", "Penguin", "Cactus", "Noodle",
            "Burrito", "Hamster", "Pigeon", "Waffle", "Muffin",
            "Raccoon", "Platypus", "Baguette", "Flamingo", "Sloth"
        };

        unsigned seed = static_cast<unsigned>(
            std::chrono::steady_clock::now().time_since_epoch().count()
        );
        std::default_random_engine rng(seed);

        std::uniform_int_distribution<int> adjDist(0, adjectives.size() - 1);
        std::uniform_int_distribution<int> nounDist(0, nouns.size() - 1);
        std::uniform_int_distribution<int> numDist(10, 9999);

        return adjectives[adjDist(rng)]
               + nouns[nounDist(rng)]
               + std::to_string(numDist(rng));
    }
};
