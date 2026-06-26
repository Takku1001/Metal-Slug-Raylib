#include "Bullet.h"

// Bullet.cpp
#include "Bullet.h"

Bullet::Bullet() {
    active = false;
    position = { 0, 0 };
    speed = 10.0f;
    texture = {};
    size = { 20, 8 };
    direction = { 1, 0 }; // Default to right
}

Bullet::Bullet(Vector2 startPos, float bulletSpeed, Texture2D bulletTexture) {
    active = true;
    position = startPos;
    speed = bulletSpeed;
    texture = bulletTexture;
    direction = { 1, 0 }; // Default to right
    if (texture.id != 0) {
        size = { (float)texture.width, (float)texture.height };
    }
    else {
        size = { 20, 8 };
    }
}

bool Bullet::IsOffScreen(int screenWidth, int screenHeight) {
    // Check all screen edges based on direction
    if (direction.x > 0 && position.x > screenWidth) return true; // Right moving
    if (direction.x < 0 && position.x < -size.x) return true;    // Left moving
    if (direction.y > 0 && position.y > screenHeight) return true; // Down moving
    if (direction.y < 0 && position.y < -size.y) return true;     // Up moving
    return false;
}


void Bullet::Update() 
{
    if (active) {
        position.x += speed * direction.x;
        position.y += speed * direction.y;
    }
}

bool Bullet::IsOffScreen(int screenWidth) {
    if (speed > 0) {
        return position.x > screenWidth;
    }
    else {
        return position.x < -size.x;
    }
}

void Bullet::Draw() {
    if (active) {
        if (texture.id != 0) {
            DrawTexture(texture, (int)position.x, (int)position.y, WHITE);
        }
        else {
            // Fallback rectangle if texture isn't loaded
            DrawRectangle((int)position.x, (int)position.y, (int)size.x, (int)size.y, RED);
        }
    }
}

Rectangle Bullet::GetCollider() const {
    return { position.x, position.y, size.x, size.y };
}

void Bullet::Fire(Vector2 startPos, float bulletSpeed, Texture2D bulletTexture, Vector2 dir)
{
    active = true;
    position = startPos;
    speed = bulletSpeed;
    this->texture = bulletTexture;
    direction = dir; // Set direction
    if (texture.id != 0) {
        size = { (float)texture.width, (float)texture.height };
    }
}