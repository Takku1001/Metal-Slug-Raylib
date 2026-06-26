#include "Player.h"
#include <iostream>

Player::Player() {
    position = { 50, 850 }; // Changed from 650 to 850 to match tank level (screenHeight/2 + 350)
    health = 100;
    isShooting = false;
    isMoving = false;
    isMovingRight = false;
    isMovingLeft = false;
    isJumping = false;
    isOnGround = true;
    currentFrame = 0;
    updateTime = 0.1f;
    runningTime = 0.0f;
    jumpVelocity = 0.0f;
    gravity = 1.0f;
    bulletTexture = {}; // Initialize empty texture

    totalDistanceWalked = 0.0f;
    firstUpdate = true;


    // Initialize all textures to empty
    for (int i = 0; i < MAX_ANIMATION_FRAMES; i++) {
        walkingFrames[i] = {};
    }
    for (int i = 0; i < MAX_JUMP_FRAMES; i++) {
        jumpFrames[i] = {};
    }

    InitializeBullets();
}

Player::~Player() {
    // Safe texture cleanup
    for (int i = 0; i < MAX_ANIMATION_FRAMES; i++) {
        if (walkingFrames[i].id != 0) {
            UnloadTexture(walkingFrames[i]);
            walkingFrames[i].id = 0;
        }
    }
    for (int i = 0; i < MAX_JUMP_FRAMES; i++) {
        if (jumpFrames[i].id != 0) {
            UnloadTexture(jumpFrames[i]);
            jumpFrames[i].id = 0;
        }
    }
    if (bulletTexture.id != 0) {
        UnloadTexture(bulletTexture);
        bulletTexture.id = 0;
    }
}

void Player::LoadTextures() {
    // Load walking animation frames
    for (int i = 0; i < MAX_ANIMATION_FRAMES; i++) {
        if (walkingFrames[i].id == 0) { // Only load if not already loaded
            walkingFrames[i] = LoadTexture(TextFormat("%d.png", i));
            if (walkingFrames[i].id != 0) {
                walkingFrames[i].height = 100;
                walkingFrames[i].width = 100;
            }
        }
    }

    // Load jump frames if available
    for (int i = 0; i < MAX_JUMP_FRAMES; i++) {
        if (jumpFrames[i].id == 0) { // Only load if not already loaded
            jumpFrames[i] = LoadTexture(TextFormat("jump%d.png", i));
            if (jumpFrames[i].id != 0) {
                jumpFrames[i].height = 100;
                jumpFrames[i].width = 100;
            }
        }
    }

    // Load bullet texture safely
    if (bulletTexture.id == 0) { // Only load if not already loaded
        bulletTexture = LoadTexture("bullet.png");
        if (bulletTexture.id != 0) {
            bulletTexture.width = 30;  // Adjust as needed
            bulletTexture.height = 10;
        }
    }
}

void Player::InitializeBullets() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        bullets[i] = Bullet();
        bullets[i].SetTexture(bulletTexture);
    }
}

void Player::Reset() {
    position = { 50, 850 }; // Changed from 650 to 850 to match tank level
    health = 100;
    isMoving = false;
    isMovingRight = false;
    isMovingLeft = false;
    isJumping = false;
    isOnGround = true;
    currentFrame = 0;
    runningTime = 0.0f;
    jumpVelocity = 0.0f;

    InitializeBullets();
}

void Player::Update(float deltaTime, int& bgx, int& bg2x, int screenWidth, int screenHeight) {
    UpdateMovement(bgx, bg2x, screenWidth, screenHeight);
    UpdateAnimation(deltaTime);
    UpdateBullets(screenWidth);

}

void Player::UpdateAnimation(float deltaTime)
{
    runningTime += deltaTime;
    if (runningTime >= updateTime) {
        runningTime = 0.0f;
        if (isMoving) {
            currentFrame++;
            if (currentFrame >= MAX_ANIMATION_FRAMES) {
                currentFrame = 0;
            }
        }
        else {
            currentFrame = 0; // Reset to idle frame when not moving
        }
    }
}

void Player::UpdateMovement(int& bgx, int& bg2x, int screenWidth, int screenHeight) {
    // Store position before movement
    Vector2 previousPosition = position;

    // Reset movement flags
    isMoving = false;
    isMovingRight = false;
    isMovingLeft = false;

    // Horizontal movement
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        if (position.x < screenWidth / 2) {
            position.x += 5.0f;
        }
        else {
            bgx -= 5;
            bg2x -= 5;
        }
        isMoving = true;
        isMovingRight = true;
    }
    else if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        if (position.x > 0) {
            position.x -= 5.0f;
            isMoving = true;
            isMovingLeft = true;
        }
        else if (bgx < 0) {
            bgx += 5;
            bg2x += 5;
            isMoving = true;
            isMovingLeft = true;
        }
    }

    // Calculate distance moved
    if (!firstUpdate) {
        // Only count distance when background is scrolling (player moving forward)
        if (isMovingRight && (bgx < 0 || bg2x < 0)) {
            totalDistanceWalked += fabs(position.x - previousPosition.x);
        }
    }
    else {
        firstUpdate = false;
    }
    lastPosition = position;

    // Jump
    if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && isOnGround) {
        Jump();
    }

    // Apply gravity and jumping physics
    if (!isOnGround) {
        position.y += jumpVelocity;
        jumpVelocity += gravity;

        // Ground collision - aligned with tank level
        float groundLevel = screenHeight / 2 + 350;
        if (position.y >= groundLevel) {
            position.y = groundLevel;
            isOnGround = true;
            isJumping = false;
            jumpVelocity = 0.0f;
        }
    }

    distanceWalked += fabs(position.x - lastPosition.x); // Only track horizontal movement
    lastPosition = position;

    // Boundary checks
    if (position.x < 0) {
        position.x = 0;
    }
    if (position.x > screenWidth - 100) {
        position.x = screenWidth - 100;
    }
}

void Player::UpdateBullets(int screenWidth) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].IsActive()) {
            bullets[i].Update();
            if (bullets[i].IsOffScreen(screenWidth)) {
                bullets[i].SetActive(false);
            }
        }
    }
}

void Player::Shoot() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].IsActive()) {
            Vector2 bulletPos = { position.x + 50, position.y + 40 };
            Vector2 direction = { 1, 0 }; // Default to right

            // If jumping, shoot upward at an angle
            if (isJumping) {
                direction = { 1, -0.5f }; // Upward at 45 degree angle
            }

            bullets[i].Fire(bulletPos, 10.0f, bulletTexture, direction);
            break;
        }
    }
}

void Player::Jump() {
    if (isOnGround) {
        isJumping = true;
        jumpVelocity = -15.0f;
        isOnGround = false;
    }
}

void Player::TakeDamage(int damage) {
    health -= damage;
    if (health < 0) {
        health = 0;
    }
}


void Player::DrawHealthBar() {
    if (!showHealthBar || health <= 0) return;

    Vector2 healthBarPos = {
        position.x + (50 - healthBarWidth) / 2,  // Center over player width
        position.y - healthBarOffset           // Position above player
    };

    // Background (empty health)
    DrawRectangleRec(
        { healthBarPos.x, healthBarPos.y, healthBarWidth, healthBarHeight },
        Fade(RED, 0.5f)
    );

    // Current health
    float currentWidth = (health / 100.0f) * healthBarWidth;
    DrawRectangleRec(
        { healthBarPos.x, healthBarPos.y, currentWidth, healthBarHeight },
        GREEN
    );

    // Border
    DrawRectangleLinesEx(
        { healthBarPos.x, healthBarPos.y, healthBarWidth, healthBarHeight },
        1.0f,
        BLACK
    );
}

void Player::Draw() {
    DrawHealthBar();  // Draw health bar first

    if (walkingFrames[0].id != 0) {
        if (isMoving && currentFrame < MAX_ANIMATION_FRAMES) {
            DrawTexture(walkingFrames[currentFrame], (int)position.x, (int)position.y, WHITE);
        }
        else {
            DrawTexture(walkingFrames[0], (int)position.x, (int)position.y, WHITE);
        }
    }
    else {
        DrawRectangle((int)position.x, (int)position.y, 50, 60, BLUE);
    }
}

void Player::DrawBullets() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        bullets[i].Draw();
    }
}

Rectangle Player::GetCollider() const {
    // Adjusted collision box to better match tank size
    return { position.x + 5, position.y + 5, 50, 55 };
}