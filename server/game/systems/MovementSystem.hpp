#pragma once
#include <vector>
#include <memory>
#include "../Entity.hpp"
#include "../Tower.hpp"

class MovementSystem {
public:
    void moveEntities(std::vector<std::unique_ptr<Entity> > &entities,
                      std::vector<Tower> &towers,
                      float tickDuration);

private:
    Tower *findClosestEnemyTower(std::vector<Tower> &towers,
                                 float x, float y, uint8_t ownerId);

    float distanceBetween(float x1, float y1, float x2, float y2) const;
};
