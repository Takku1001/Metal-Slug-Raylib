#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include "Bullet.h"

enum EnemyType {
    TANK,
    UFO,
    ZOMBIE
};

class Enemy {
private:
    static const int MAX_ENEMY_BULLETS = 5;

    Texture2D bulletTexture;

    int damage;

    int level;
    float bulletSpeed;

    // UFO-specific movement variables
    EnemyType enemyType;
    float verticalSpeed;
    float verticalDirection;
    float minY, maxY;
    float hoverTimer;
    float hoverDuration;

    // Zombie-specific variables
    Vector2 targetPosition; // Player position for zombie targeting

    bool active;
    Vector2 position;
    float speed;
    Rectangle collider;
    Bullet bullets[MAX_ENEMY_BULLETS];
    float shootTimer;
    float shootInterval;
    Texture2D texture;
    int health;

public:
    Enemy();
    ~Enemy();

    // In the public section of Enemy class
    void SetBulletTexture(Texture2D tex) { bulletTexture = tex; }

    // Initialization
    void Initialize(Vector2 startPos, float enemySpeed, Texture2D enemyTexture);
    void InitializeUFO(Vector2 startPos, float enemySpeed, Texture2D enemyTexture, float minYPos, float maxYPos);
    void InitializeZombie(Vector2 startPos, float enemySpeed, Texture2D enemyTexture);
    void InitializeBullets();

    // Updates
    void Update(float deltaTime, int screenWidth);
    void UpdateTank(float deltaTime, int screenWidth);
    void UpdateUFO(float deltaTime, int screenWidth);
    void UpdateZombie(float deltaTime, int screenWidth);
    void UpdateBullets(float deltaTime);

    // Actions
    void Shoot();
    void ShootUFO(Vector2 playerPos);
    void ShootZombie(Vector2 playerPos);
    void TakeDamage(int damage);
    void Destroy() { active = false; }

    // Rendering
    void Draw();
    void DrawBullets();

    void SetLevel(int lvl) { level = lvl; }
    int GetLevel() const { return level; }
    void SetBulletSpeed(float speed) { bulletSpeed = speed; }
    float GetBulletSpeed() const { return bulletSpeed; }

    int GetDamage() const { return damage; }
    void SetDamage(int dmg) { damage = dmg; }

    // Type management
    void SetEnemyType(EnemyType type) { enemyType = type; }
    EnemyType GetEnemyType() const { return enemyType; }

    // Zombie-specific methods
    void SetTargetPosition(Vector2 target) { targetPosition = target; }
    Vector2 GetTargetPosition() const { return targetPosition; }

    // Getters
    bool IsActive() const { return active; }
    Vector2 GetPosition() const { return position; }
    Rectangle GetCollider() const { return collider; }
    Bullet* GetBullets() { return bullets; }
    int GetHealth() const { return health; }

    // Setters
    void SetActive(bool state) { active = state; }
    void SetPosition(Vector2 pos);
    void SetSpeed(float spd) { speed = spd; }
    void SetShootInterval(float interval) { shootInterval = interval; }
};

#endif