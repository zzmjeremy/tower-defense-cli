//Copyright 2K Games, Engineering Graduate Program. All rights reserved. DO NOT redistribute.

#include "tower.h"

#include <algorithm>
#include <cstdlib>
#include <limits>

Tower::Tower(int x, int y, int rateOfFire)
    : position({x, y}),
      rateOfFire(rateOfFire),
      lastFireTime(std::chrono::steady_clock::now()),
      bAutoFire(true)
    {}

void Tower::Update(const std::vector<Enemy*>& enemies,
                   std::vector<Bullet>& bullets) {
    if (bAutoFire) {
        FireAtEnemy(enemies, bullets);
    }
}

void Tower::FireAtEnemy(const std::vector<Enemy*>& enemies,
                        std::vector<Bullet>& bullets) {
    auto currentTime = std::chrono::steady_clock::now();
    auto timeElapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastFireTime);

    if (timeElapsed >= std::chrono::milliseconds(rateOfFire)) {
        Enemy* closestEnemy = FindClosestEnemy(enemies);
        if (closestEnemy == nullptr) return;

        if (bAutoFire) {
            long long distance = CalculateDistance(closestEnemy->GetPosition(), position);

            if (distance <= ENEMYRANGE) {
                FireBulletAtEnemy(*closestEnemy, bullets);
                lastFireTime = currentTime;
            }
        } else {
            FireBulletAtEnemy(*closestEnemy, bullets);
            lastFireTime = currentTime;
        }
    }
}

bool Tower::GetAutoFire() const { return bAutoFire; }

void Tower::SetAutoFire(bool autoFire) { bAutoFire = autoFire; }

void Tower::ManualFire(const std::vector<Enemy*>& enemies,
                       std::vector<Bullet>& bullets) {
    FireAtEnemy(enemies, bullets);
}

void Tower::FireBulletAtEnemy(Enemy& enemy, std::vector<Bullet>& bullets) {
    Vector2D direction = {0, 0};
    if (enemy.GetPosition().x > position.x) direction.x = 1;
    if (enemy.GetPosition().x < position.x) direction.x = -1;
    if (enemy.GetPosition().y > position.y) direction.y = 1;
    if (enemy.GetPosition().y < position.y) direction.y = -1;

    bullets.emplace_back(position, direction);
}

long long Tower::CalculateDistance(const Vector2D& first,
                                   const Vector2D& second) {
    // Bullets advance one cell on each axis per update, including diagonals.
    return std::max(std::llabs(static_cast<long long>(first.x) - second.x),
                    std::llabs(static_cast<long long>(first.y) - second.y));
}

Enemy* Tower::FindClosestEnemy(const std::vector<Enemy*>& enemies) const {
    Enemy* closest = nullptr;
    long long shortestDistance = std::numeric_limits<long long>::max();
    for (Enemy* enemy : enemies) {
        if (enemy == nullptr || !enemy->IsActive()) continue;
        const long long distance = CalculateDistance(enemy->GetPosition(), position);
        if (distance < shortestDistance) {
            shortestDistance = distance;
            closest = enemy;
        }
    }
    return closest;
}
