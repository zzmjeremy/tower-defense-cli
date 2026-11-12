//Copyright 2K Games, Engineering Graduate Program. All rights reserved. DO NOT redistribute.

#ifndef TOWER_H
#define TOWER_H

#include <chrono>
#include <memory>
#include <vector>

#include "bullet.h"
#include "enemy.h"
#include "texture.h"
#include "vector2d.h"

// Implement the following methods:
// CalculateDistance
// FindClosestEnemy

class Tower {
   public:
    Vector2D position;
    std::chrono::steady_clock::time_point lastFireTime;
    Texture texture = Texture('T', Color::GREEN);

    Tower(int x, int y, int rateOfFire);

    void Update(const std::vector<Enemy*>& enemies, std::vector<Bullet>& bullets);

    bool GetAutoFire() const;
    void SetAutoFire(bool autoFire);
    void ManualFire(const std::vector<Enemy*>& enemies,
                    std::vector<Bullet>& bullets);

   private:
    int rateOfFire;
    bool bAutoFire;

    void FireAtEnemy(const std::vector<Enemy*>& enemies,
                     std::vector<Bullet>& bullets);
    void FireBulletAtEnemy(Enemy& enemy, std::vector<Bullet>& bullets);
    static long long CalculateDistance(const Vector2D& first,
                                       const Vector2D& second);
    Enemy* FindClosestEnemy(const std::vector<Enemy*>& enemies) const;
};

#endif  // TOWER_H
