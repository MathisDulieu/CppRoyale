#include "CombatSystem.hpp"
#include <limits>
#include <cmath>

void CombatSystem::resolveTroopCombat(
    std::vector<std::unique_ptr<Entity> > &entities,
    std::vector<Tower> &towers,
    float tickDuration) {
    for (auto &attacker: entities) {
        attacker->setInCombat(false);
        attacker->reduceCooldown(tickDuration);
    }

    for (auto &attacker: entities) {
        if (!attacker->isAlive()) continue;

        Entity *closestEnemy = nullptr;
        float closestDistance = std::numeric_limits<float>::max();

        for (auto &target: entities) {
            if (!target->isAlive()) continue;
            if (target->getOwnerId() == attacker->getOwnerId()) continue;

            float distance = distanceBetween(attacker->getX(), attacker->getY(),
                                             target->getX(), target->getY());
            if (distance < closestDistance) {
                closestDistance = distance;
                closestEnemy = target.get();
            }
        }

        if (closestEnemy && closestDistance <= attacker->getAttackRange()) {
            attacker->setInCombat(true);
            if (attacker->getCooldownTimer() <= 0.f) {
                closestEnemy->takeDamage(attacker->getAttackDamage());
                attacker->resetCooldown();
            }
            continue;
        }

        Tower *closestTower = findClosestEnemyTower(towers,
                                                    attacker->getX(),
                                                    attacker->getY(),
                                                    attacker->getOwnerId());
        if (closestTower) {
            float towerDistance = distanceBetween(attacker->getX(), attacker->getY(),
                                                  closestTower->getX(), closestTower->getY());
            if (towerDistance <= attacker->getAttackRange()) {
                attacker->setInCombat(true);
                if (attacker->getCooldownTimer() <= 0.f) {
                    closestTower->takeDamage(attacker->getAttackDamage());
                    attacker->resetCooldown();
                }
            }
        }
    }
}

void CombatSystem::resolveTowerCombat(
    std::vector<Tower> &towers,
    std::vector<std::unique_ptr<Entity> > &entities,
    float tickDuration) {
    for (auto &tower: towers) {
        if (!tower.isAlive()) continue;
        tower.reduceCooldown(tickDuration);

        Entity *closestEnemy = nullptr;
        float closestDistance = std::numeric_limits<float>::max();

        for (auto &entity: entities) {
            if (!entity->isAlive()) continue;
            if (entity->getOwnerId() == tower.getOwnerId()) continue;

            float distance = distanceBetween(tower.getX(), tower.getY(),
                                             entity->getX(), entity->getY());
            if (distance < closestDistance) {
                closestDistance = distance;
                closestEnemy = entity.get();
            }
        }

        if (closestEnemy && closestDistance <= tower.getAttackRange()) {
            if (tower.getCooldownTimer() <= 0.f) {
                closestEnemy->takeDamage(tower.getAttackDamage());
                tower.resetCooldown();
            }
        }
    }
}

Tower *CombatSystem::findClosestEnemyTower(std::vector<Tower> &towers,
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

float CombatSystem::distanceBetween(float x1, float y1,
                                    float x2, float y2) const {
    float dx = x1 - x2;
    float dy = y1 - y2;
    return std::sqrt(dx * dx + dy * dy);
}
