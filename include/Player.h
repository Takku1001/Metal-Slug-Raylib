#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "Bullet.h"

class Player {
private:
    static const int MAX_BULLETS = 15;
    static const int MAX_ANIMATION_FRAMES = 23;
    static const int MAX_JUMP_FRAMES = 6;

    Texture2D bulletTexture;

    float distanceWalked;
    Vector2 lastPosition;

    float totalDistanceWalked;
    bool firstUpdate;

    Vector2 position;
    Texture2D walkingFrames[MAX_ANIMATION_FRAMES];
    Texture2D jumpFrames[MAX_JUMP_FRAMES];
    int health;
    bool isShooting;
    bool isMoving;
    bool isMovingRight;
    bool isMovingLeft;
    bool isJumping;
    bool isOnGround;
    int currentFrame;
    float updateTime;
    float runningTime;
    float jumpVelocity;
    float gravity;
    Bullet bullets[MAX_BULLETS];

    // Health bar variables

    float healthBarWidth = 60.0f;
    float healthBarHeight = 8.0f;
    float healthBarOffset = 25.0f;
    bool showHealthBar = true;

    Color healthBarFullColor;
    Color healthBarEmptyColor;

public:
    Player();
    ~Player();

    // Initialization
    void LoadTextures();
    void InitializeBullets();
    void Reset();

    float GetDistanceWalked() const { return distanceWalked; }


    float GetTotalDistanceWalked() const { return totalDistanceWalked; }
    void ResetDistanceWalked() {
        totalDistanceWalked = 0.0f;
        firstUpdate = true;
    }

    // Add to public section of Player class
    void SetHealthBarVisible(bool visible) { showHealthBar = visible; }
    bool IsHealthBarVisible() const { return showHealthBar; }

    // Updates
    void Update(float deltaTime, int& bgx, int& bg2x, int screenWidth, int screenHeight);
    void UpdateAnimation(float deltaTime);
    void UpdateMovement(int& bgx, int& bg2x, int screenWidth, int screenHeight);
    void UpdateBullets(int screenWidth);

    // Actions
    void Shoot();
    void Jump();
    void TakeDamage(int damage);

    // Rendering
    void Draw();
    void DrawBullets();
    void DrawHealthBar();  // New health bar drawing method

    // Getters
    Vector2 GetPosition() const { return position; }
    int GetHealth() const { return health; }
    Rectangle GetCollider() const;
    Bullet* GetBullets() { return bullets; }
    bool IsMoving() const { return isMoving; }
    bool IsMovingRight() const { return isMovingRight; }
    bool IsMovingLeft() const { return isMovingLeft; }

    // Setters
    void SetPosition(Vector2 pos) { position = pos; }
    void SetHealth(int hp) { health = hp; }

    // Health bar setters
    void SetHealthBarSize(float width, float height) {
        healthBarWidth = width;
        healthBarHeight = height;
    }
    void SetHealthBarOffset(float offset) { healthBarOffset = offset; }
    void SetHealthBarColors(Color full, Color empty) {
        healthBarFullColor = full;
        healthBarEmptyColor = empty;
    }
};

#endif