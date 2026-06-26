#ifndef LEVEL_H
#define LEVEL_H

#include "raylib.h"
#include "Enemy.h"
#include "Player.h"

class Level {
private:
    static const int MAX_ENEMIES = 10;
    static const int MAX_BOSS_BULLETS = 10;

    int levelNumber;
    Enemy enemies[MAX_ENEMIES];
    Texture2D background1;
    Texture2D background2;
    Texture2D enemyTexture;
    Texture2D ufoTexture; // Add UFO texture
    Texture2D zombieTexture; // Add Zombie texture

    // Boss-related variables
    Texture2D bossTexture;
    Texture2D bossBulletTexture;
    Texture2D bossDeathTexture;
    bool bossActive;
    int bossHealth;
    Vector2 bossPosition;
    float bossSpeed;
    float bossDirection;
    float bossShootTimer;
    float bossShootInterval;
    float bossMinY;
    float bossMaxY;
    Bullet bossBullets[MAX_BOSS_BULLETS];

    int bg1X, bg2X;
    float spawnTimer;
    float spawnInterval;
    int maxActiveEnemies;
    int enemiesSpawned;
    int enemiesToSpawn;

    const Player* playerRef;

    bool levelComplete;
    int screenWidth, screenHeight;

public:
    Level();
    ~Level();

    // Boss-related methods
    void InitializeBoss();
    void UpdateBoss(float deltaTime, Player& player, int& score);
    void BossShoot(Vector2 playerPos);
    void DrawBoss(const Player& player);
    bool IsBossDefeated() const { return bossActive && bossHealth <= 0; }

    void SetPlayerReference(const Player* player) { playerRef = player; }

    // Initialization
    void Initialize(int level, int scrWidth, int scrHeight);
    void LoadTextures();
    void Reset();

    // Updates
    void Update(float deltaTime, Player& player, int& score);
    void UpdateEnemies(float deltaTime, Player& player, int& score);
    void UpdateBackground(Player& player);
    void SpawnEnemy();

    // Collision Detection
    void CheckCollisions(Player& player, int& score);

    // Rendering
    void Draw();
    void DrawEnemies();

    // Getters
    bool IsLevelComplete() const { return levelComplete; }
    int GetLevelNumber() const { return levelNumber; }
    int GetBackgroundX1() const { return bg1X; }
    int GetBackgroundX2() const { return bg2X; }
    bool IsBossActive() const { return bossActive; }
    int GetBossHealth() const { return bossHealth; }

    // Setters
    void SetBackgroundX1(int x) { bg1X = x; }
    void SetBackgroundX2(int x) { bg2X = x; }
};

#endif