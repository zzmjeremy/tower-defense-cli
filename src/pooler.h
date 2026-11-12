//Copyright 2K Games, Engineering Graduate Program. All rights reserved. DO NOT redistribute.

#ifndef POOLER_H
#define POOLER_H

#include <memory>
#include <vector>

#include "enemy.h"

// Implement the following methods:
// Clear()
// SpawnEnemy()
// DespawnEnemy()
// GetActiveEnemies()

class Pooler {
   public:
    Pooler(size_t minimumPoolSize);
    void Clear();
    Enemy* SpawnEnemy(const Vector2D& position);
    void DespawnEnemy(Enemy* enemy);
    std::vector<Enemy*> GetActiveEnemies() const;
    size_t PoolSize() const;

   private:
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::vector<Enemy*> availableEnemies;
    size_t minimumPoolSize;

    Enemy* CreateEnemy();
};

#endif  // POOLER_H
