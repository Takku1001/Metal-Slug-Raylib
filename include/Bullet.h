#ifndef BULLET_H
#define BULLET_H

#include "raylib.h"

class Bullet {
private:
    bool active;
    Vector2 position;
    float speed;
    Texture2D texture;
    Vector2 size;
    Vector2 direction;

public:
    Bullet();
    Bullet(Vector2 startPos, float bulletSpeed, Texture2D bulletTexture);

    // Updates
    void Update();
    bool IsOffScreen(int screenWidth);

    // Rendering
    void Draw();

    // Collision
    Rectangle GetCollider() const;

    // Getters
    bool IsActive() const { return active; }
    Vector2 GetPosition() const { return position; }

    // Setters
    void SetActive(bool state) { active = state; }
    void SetPosition(Vector2 pos) { position = pos; }
    void SetSpeed(float spd) { speed = spd; }
    void SetTexture(Texture2D tex) { texture = tex; }


    bool IsOffScreen(int screenWidth, int screenHeight);

    // Actions
    void Fire(Vector2 startPos, float bulletSpeed, Texture2D bulletTexture, Vector2 dir);  // Added Fire declaration
};

#endif