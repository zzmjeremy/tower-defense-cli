//Copyright 2K Games, Engineering Graduate Program. All rights reserved. DO NOT redistribute.

#include "pooler.h"

#include <algorithm>

Pooler::Pooler(size_t minimumPoolSize) : minimumPoolSize(minimumPoolSize) {
    enemies.reserve(minimumPoolSize);
    availableEnemies.reserve(minimumPoolSize);
    for (size_t i = 0; i < minimumPoolSize; ++i) {
        availableEnemies.push_back(CreateEnemy());
    }
}

void Pooler::Clear() {
    availableEnemies.clear();
    for (const auto& enemy : enemies) {
        enemy->Deactivate();
        availableEnemies.push_back(enemy.get());
    }
}

Enemy* Pooler::SpawnEnemy(const Vector2D& position) {
    if (availableEnemies.empty()) {
        // Reserve the free slot before creating an enemy, so a failed allocation
        // cannot leave an untracked object in the pool.
        availableEnemies.reserve(enemies.size() + 1);
        availableEnemies.push_back(CreateEnemy());
    }

    Enemy* enemy = availableEnemies.back();
    availableEnemies.pop_back();
    enemy->Activate(position);
    return enemy;
}

void Pooler::DespawnEnemy(Enemy* enemy) {
    if (enemy == nullptr) return;

    const auto owned = std::find_if(enemies.begin(), enemies.end(),
        [enemy](const auto& candidate) { return candidate.get() == enemy; });
    if (owned == enemies.end() || !enemy->IsActive()) return;

    // Ensure the free list has room before changing the enemy's state.
    availableEnemies.reserve(enemies.size());
    enemy->Deactivate();
    availableEnemies.push_back(enemy);
}

std::vector<Enemy*> Pooler::GetActiveEnemies() const {
    std::vector<Enemy*> active;
    active.reserve(enemies.size() - availableEnemies.size());
    for (const auto& enemy : enemies) {
        if (enemy->IsActive()) active.push_back(enemy.get());
    }
    return active;
}

size_t Pooler::PoolSize() const {
    return enemies.size();
}

Enemy* Pooler::CreateEnemy() {
    enemies.push_back(std::make_unique<Enemy>());
    return enemies.back().get();
}
