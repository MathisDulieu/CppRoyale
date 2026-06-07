#include "MovementSystem.hpp"
#include <cmath>
#include <limits>

void MovementSystem::moveEntities(
    std::vector<std::unique_ptr<Entity> > &entities,
    std::vector<Tower> &towers,
    float tickDuration) {
    for (auto &entity: entities) {
        if (entity->isInCombat()) continue;

        Tower *targetTower = findClosestEnemyTower(towers,
                                                   entity->getX(),
                                                   entity->getY(),
                                                   entity->getOwnerId());
        if (!targetTower) continue;

        float deltaX = targetTower->getX() - entity->getX();
        float deltaY = targetTower->getY() - entity->getY();
        float length = std::sqrt(deltaX * deltaX + deltaY * deltaY);

        if (length <= 0.f) continue;

        float distancePerTick = entity->getSpeed() * tickDuration;
        entity->move((deltaX / length) * distancePerTick,
                     (deltaY / length) * distancePerTick);
    }
}

Tower *MovementSystem::findClosestEnemyTower(std::vector<Tower> &towers,
                                             float x, float y, uint8_t ownerId) {
    Tower *closest = nullptr;
    float closestDistance = std::numeric_limits<float>::max();

    for (auto &tower: towers) {
        if (!tower.isAlive()) continue;
        if (tower.getOwnerId() == ownerId) continue;

        float distance = distanceBetween(x, y, tower.getX(), tower.getY());
        if (distance < closestDistance) {
            closestDistance = distance;
            closest = &tower;
        }
    }
    return closest;
}

float MovementSystem::distanceBetween(float x1, float y1,
                                      float x2, float y2) const {
    float dx = x1 - x2;
    float dy = y1 - y2;
    return std::sqrt(dx * dx + dy * dy);
}
