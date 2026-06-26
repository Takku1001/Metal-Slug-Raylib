#include "Enemy.h"
#include <cmath>

Enemy::Enemy() {
    active = false;
    position = { 0, 0 };
    speed = 3.0f;
    collider = { 0, 0, 100, 60 };
    shootTimer = 0.0f;
    shootInterval = 2.0f;
    health = 100;
    texture = {}; // Initialize empty texture
    bulletTexture = {}; // Initialize empty texture

    level = 1;  // Default level
    bulletSpeed = -8.0f;

    // UFO-specific initialization
    enemyType = TANK;
    verticalSpeed = 2.0f;
    verticalDirection = 1.0f;
    minY = 50.0f;
    maxY = 300.0f;
    hoverTimer = 0.0f;
    hoverDuration = 2.0f;

    // Zombie-specific initialization
    targetPosition = { 0, 0 };

    InitializeBullets();
}

Enemy::~Enemy() {
    // Safe texture cleanup
    if (texture.id != 0) {
        UnloadTexture(texture);
        texture.id = 0;
    }
    if (bulletTexture.id != 0) {
        UnloadTexture(bulletTexture);
        bulletTexture.id = 0;
    }
}

void Enemy::Initialize(Vector2 startPos, float enemySpeed, Texture2D enemyTexture) {
    active = true;
    position = startPos;
    speed = enemySpeed;
    texture = enemyTexture;
    collider = { position.x, position.y, 100, 60 };
    shootTimer = 0.0f;
    health = 100;
    enemyType = TANK;

    // Reset all bullets
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
        bullets[i].SetActive(false);
    }

    // Load bullet texture only if not already loaded
    if (bulletTexture.id == 0) {
        bulletTexture = LoadTexture("tankbullet.png");
        if (bulletTexture.id != 0) {
            bulletTexture.width = 30;
            bulletTexture.height = 10;
        }
    }

    // Set consistent bullet speed (double the enemy speed)
    bulletSpeed = -(speed * 1.5f * level);  // Negative for left direction

    // Initialize bullets with the correct speed
    InitializeBullets();
}

void Enemy::InitializeUFO(Vector2 startPos, float enemySpeed, Texture2D enemyTexture, float minYPos, float maxYPos) {
    active = true;
    position = startPos;
    speed = enemySpeed;
    texture = enemyTexture;
    collider = { position.x, position.y, 80, 50 }; // Smaller collider for UFO
    shootTimer = 0.0f;
    health = 75; // Less health than tanks
    enemyType = UFO;

    // UFO-specific setup
    minY = minYPos;
    maxY = maxYPos;
    verticalSpeed = 1.5f;
    verticalDirection = 1.0f;
    hoverTimer = 0.0f;
    hoverDuration = GetRandomValue(15, 35) / 10.0f; // Random hover time between 1.5-3.5 seconds

    // Reset all bullets
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
        bullets[i].SetActive(false);
    }

    // Load UFO bullet texture
    if (bulletTexture.id == 0) {
        bulletTexture = LoadTexture("ufobullet.png");
        if (bulletTexture.id != 0) {
            bulletTexture.width = 25;
            bulletTexture.height = 25;
        }
    }

    bulletSpeed = 8.0f + (level * 1.0f); // Positive for downward direction

    // Initialize bullets
    InitializeBullets();
}

void Enemy::InitializeZombie(Vector2 startPos, float enemySpeed, Texture2D enemyTexture) {
    active = true;
    position = startPos;
    speed = enemySpeed;
    texture = enemyTexture;
    collider = { position.x, position.y, 100, 120 }; // Updated collider to match smaller size
    shootTimer = 0.0f;
    health = 60; // Less health but more aggressive
    enemyType = ZOMBIE;

    // Zombie-specific setup
    targetPosition = { 0, 0 }; // Will be updated with player position

    // Reset all bullets
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
        bullets[i].SetActive(false);
    }

    // Load zombie bullet texture
    if (bulletTexture.id == 0) {
        bulletTexture = LoadTexture("Zombiebullet.png");
        if (bulletTexture.id != 0) {
            bulletTexture.width = 20;
            bulletTexture.height = 20;
        }
        else {
            // Fallback to regular bullet if zombie bullet not found
            
            bulletTexture = LoadTexture("bullet.png");
            if (bulletTexture.id != 0) {
                bulletTexture.width = 30;
                bulletTexture.height = 10;
            }
        }
    }

    bulletSpeed = 5.0f; // Decreased from 9.0f - slower zombie bullets

    // Initialize bullets
    InitializeBullets();
}

void Enemy::InitializeBullets() {
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
        bullets[i] = Bullet();
        bullets[i].SetTexture(bulletTexture);
        bullets[i].SetSpeed(bulletSpeed);
    }
}

void Enemy::Update(float deltaTime, int screenWidth) {
    if (!active) return;

    if (enemyType == UFO) {
        UpdateUFO(deltaTime, screenWidth);
    }
    else if (enemyType == ZOMBIE) {
        UpdateZombie(deltaTime, screenWidth);
    }
    else {
        UpdateTank(deltaTime, screenWidth);
    }

    // Update bullets
    UpdateBullets(deltaTime);
}

void Enemy::UpdateTank(float deltaTime, int screenWidth) {
    // Move enemy
    position.x -= speed;
    collider.x = position.x;

    // Remove enemy if off screen
    if (position.x < -100) {
        active = false;
        return;
    }

    // Update shooting timer
    shootTimer += deltaTime;
    if (shootTimer >= shootInterval) {
        Shoot();
        shootTimer = 0.0f;
    }
}

void Enemy::UpdateUFO(float deltaTime, int screenWidth) {
    // Horizontal movement (slower than tanks)
    position.x -= speed * 0.7f;

    // Remove UFO if off screen
    if (position.x < -100) {
        active = false;
        return;
    }

    // Vertical movement with hovering behavior
    hoverTimer += deltaTime;

    if (hoverTimer < hoverDuration) {
        // Hovering phase - slow vertical oscillation
        position.y += sin(hoverTimer * 3.0f) * 0.5f;
    }
    else {
        // Movement phase
        position.y += verticalSpeed * verticalDirection;

        // Bounce between min and max Y
        if (position.y <= minY) {
            position.y = minY;
            verticalDirection = 1.0f;
            hoverTimer = 0.0f; // Reset hover timer
            hoverDuration = GetRandomValue(10, 25) / 10.0f; // New random hover duration
        }
        else if (position.y >= maxY) {
            position.y = maxY;
            verticalDirection = -1.0f;
            hoverTimer = 0.0f; // Reset hover timer
            hoverDuration = GetRandomValue(10, 25) / 10.0f; // New random hover duration
        }
    }

    // Update collider position
    collider.x = position.x;
    collider.y = position.y;

    // Update shooting timer (UFOs shoot more frequently)
    shootTimer += deltaTime;
    if (shootTimer >= shootInterval * 0.8f) {
        Shoot(); // For now, use regular shoot - can be enhanced with player tracking
        shootTimer = 0.0f;
    }
}

void Enemy::UpdateZombie(float deltaTime, int screenWidth) {
    // Move zombie toward target (player)
    if (targetPosition.x != 0 || targetPosition.y != 0) {
        // Calculate direction toward player
        Vector2 direction = { targetPosition.x - position.x, targetPosition.y - position.y };
        float distance = sqrt(direction.x * direction.x + direction.y * direction.y);

        if (distance > 0) {
            // Normalize direction
            direction.x /= distance;
            direction.y /= distance;

            // Move toward player
            position.x += direction.x * speed;
            position.y += direction.y * speed * 0.3f; // Slower vertical movement
        }
    }
    else {
        // Default movement if no target
        position.x -= speed;
    }

    // Update collider position
    collider.x = position.x;
    collider.y = position.y;

    // Remove zombie if off screen
    if (position.x < -100) {
        active = false;
        return;
    }

    // Update shooting timer (zombies shoot frequently)
    shootTimer += deltaTime;
    if (shootTimer >= shootInterval) {
        if (targetPosition.x != 0 || targetPosition.y != 0) {
            ShootZombie(targetPosition);
        }
        else {
            Shoot(); // Default shoot if no target
        }
        shootTimer = 0.0f;
    }
}

void Enemy::UpdateBullets(float deltaTime) {
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
        if (bullets[i].IsActive()) {
            bullets[i].Update();
            // Check if bullet is off screen (bottom of screen)
            if (bullets[i].GetPosition().y > 1200) { // Assuming screen height
                bullets[i].SetActive(false);
            }
        }
    }
}

void Enemy::Shoot() {
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
        if (!bullets[i].IsActive()) {
            Vector2 bulletPos;
            float bulletSpeedToUse;
            Vector2 direction = { -1, 0 }; // Default to left for tanks

            if (enemyType == UFO) {
                // UFO shoots downward
                bulletPos = { position.x + 40, position.y + 50 };
                bulletSpeedToUse = bulletSpeed;
                direction = { 0, 1 }; // Down for UFOs
            }
            else if (enemyType == ZOMBIE) {
                // Zombie shoots left
                bulletPos = { position.x, position.y + 40 };
                bulletSpeedToUse = bulletSpeed;
                direction = { -1, 0 }; // Left for zombies
            }
            else {
                // Tank shoots left
                float yOffset = 19 - (level * 2);
                bulletPos = { position.x, position.y + yOffset };
                bulletSpeedToUse = bulletSpeed;
                direction = { -1, 0 }; // Explicitly left for tanks
            }

            bullets[i].Fire(bulletPos, bulletSpeedToUse, bulletTexture, direction);
            break;
        }
    }
}

void Enemy::ShootUFO(Vector2 playerPos) {
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
        if (!bullets[i].IsActive()) {
            Vector2 bulletPos = { position.x + 40, position.y + 50 };

            // Calculate direction toward player
            Vector2 direction = { playerPos.x - bulletPos.x, playerPos.y - bulletPos.y };
            float length = sqrt(direction.x * direction.x + direction.y * direction.y);

            if (length > 0) {
                direction.x /= length;
                direction.y /= length;

                // Set bullet velocity components (this would require modifying Bullet class)
                // For now, just shoot straight down
                bullets[i].Fire(bulletPos, 6.0f, bulletTexture, direction);
            }
            break;
        }
    }
}

void Enemy::ShootZombie(Vector2 playerPos) {
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
        if (!bullets[i].IsActive()) {
            Vector2 bulletPos = { position.x, position.y + 40 };

            // Calculate direction toward player
            Vector2 direction = { playerPos.x - bulletPos.x, playerPos.y - bulletPos.y };
            float length = sqrt(direction.x * direction.x + direction.y * direction.y);

            if (length > 0) {
                direction.x /= length;
                direction.y /= length;

                bullets[i].Fire(bulletPos, bulletSpeed, bulletTexture, direction);
            }
            break;
        }
    }
}

void Enemy::TakeDamage(int damage) {
    health -= damage;
    if (health <= 0) {
        active = false;
    }
}

void Enemy::Draw() {
    if (active) {
        if (texture.id != 0) {
            if (enemyType == ZOMBIE) {
                // Scale zombie texture to desired size
                DrawTexturePro(
                    texture,
                    { 0, 0, (float)texture.width, (float)texture.height }, // Source rectangle (full texture)
                    { position.x, position.y, 100, 120 },                   // Destination rectangle (smaller size)
                    { 0, 0 },                                                // Origin
                    0.0f,                                                    // Rotation
                    WHITE                                                    // Tint
                );
            }
            else {
                // Normal drawing for other enemies
                DrawTexture(texture, (int)position.x, (int)position.y, WHITE);
            }
        }
        else {
            // Fallback rectangle if texture isn't loaded
            if (enemyType == UFO) {
                DrawRectangle((int)position.x, (int)position.y, 80, 50, PURPLE);
            }
            else if (enemyType == ZOMBIE) {
                DrawRectangle((int)position.x, (int)position.y, 100, 120, GREEN); // Updated fallback size to match smaller zombie
                // Debug text
                DrawText("ZOMBIE", (int)position.x, (int)position.y - 20, 12, WHITE);
            }
            else {
                DrawRectangle((int)position.x, (int)position.y, 100, 60, RED);
            }
        }

        // Debug: Draw enemy type for troubleshooting
        if (enemyType == ZOMBIE) {
            DrawText(TextFormat("Z:%d", texture.id), (int)position.x, (int)position.y + 125, 10, YELLOW);
        }
    }
}

void Enemy::DrawBullets() {
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
        bullets[i].Draw();
    }
}

void Enemy::SetPosition(Vector2 pos) {
    position = pos;
    collider.x = position.x;
    collider.y = position.y;
}