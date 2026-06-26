#include "Level.h"
#include <iostream>

Level::Level() {
    levelNumber = 1;
    bg1X = 0;
    bg2X = 0;
    spawnTimer = 0.0f;
    spawnInterval = 3.0f;
    maxActiveEnemies = 2;
    enemiesSpawned = 0;
    enemiesToSpawn = 10;
    levelComplete = false;
    screenWidth = 1200;
    screenHeight = 1000;

    // Initialize textures to empty
    background1 = {};
    background2 = {};
    enemyTexture = {};
    ufoTexture = {};
    zombieTexture = {}; // Initialize zombie texture explicitly

    // Boss initialization
    bossActive = false;
    bossHealth = 1000;
    bossPosition = { 0, 0 };
    bossSpeed = 1.0f;
    bossDirection = 1.0f;
    bossShootTimer = 0.0f;
    bossShootInterval = 1.5f;
    bossMinY = 100.0f;
    bossMaxY = 400.0f;
}

Level::~Level() {
    // Safe cleanup - check if textures are valid before unloading
    if (background1.id != 0) {
        UnloadTexture(background1);
    }
    if (background2.id != 0) {
        UnloadTexture(background2);
    }
    if (enemyTexture.id != 0) {
        UnloadTexture(enemyTexture);
    }
    if (ufoTexture.id != 0) {
        UnloadTexture(ufoTexture);
    }
    if (zombieTexture.id != 0) {
        UnloadTexture(zombieTexture);
    }
    if (bossTexture.id != 0) {
        UnloadTexture(bossTexture);
    }
    if (bossBulletTexture.id != 0) {
        UnloadTexture(bossBulletTexture);
    }
    if (bossDeathTexture.id != 0) {
        UnloadTexture(bossDeathTexture);
    }
}

void Level::Initialize(int level, int scrWidth, int scrHeight) {
    levelNumber = level;
    screenWidth = scrWidth;
    screenHeight = scrHeight;

    // Reset level variables
    Reset();

    // Load textures for this level
    LoadTextures();

    // Set level-specific parameters
    switch (level) {
    case 1:
        maxActiveEnemies = 2;
        enemiesToSpawn = 10;
        spawnInterval = 3.0f;
        break;
    case 2:
        maxActiveEnemies = 4; // UFO level
        enemiesToSpawn = 20;
        spawnInterval = 2.0f;
        break;
    case 3:
        maxActiveEnemies = 5; // Mixed enemies
        enemiesToSpawn = 25;
        spawnInterval = 1.8f; // Faster spawn rate
        break;
    case 4:
        maxActiveEnemies = 8; // More zombies active at once
        enemiesToSpawn = 30; // Lots of zombies
        spawnInterval = 0.8f; // Very fast spawn rate
        break;
    case 5: // Boss level
        maxActiveEnemies = 3; // Fewer UFOs to focus on boss
        enemiesToSpawn = 15; // Continuous UFO spawns
        spawnInterval = 3.0f; // Slower spawn rate for boss level
        InitializeBoss(); // Initialize the boss
        break;
    }

    // Set enemy damage based on level
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].SetDamage(10 + (level * 5)); // Level 1: 15, Level 5: 35
    }
}

void Level::LoadTextures() {
    // Unload existing textures first if they exist
    if (background1.id != 0) UnloadTexture(background1);
    if (background2.id != 0) UnloadTexture(background2);
    if (enemyTexture.id != 0) UnloadTexture(enemyTexture);
    if (ufoTexture.id != 0) UnloadTexture(ufoTexture);

    // Load different backgrounds for each level
    switch (levelNumber) {
    case 1:
        background1 = LoadTexture("Map1.png");
        background2 = LoadTexture("Map2.png");
        break;
    case 2:
        background1 = LoadTexture("Desert1.png");
        background2 = LoadTexture("Desert2.png");
        break;
    case 3:
        background1 = LoadTexture("Jungle1.png");
        background2 = LoadTexture("Jungle2.png");
        break;
    case 4:
        background1 = LoadTexture("City1.png");
        background2 = LoadTexture("City2.png");
        break;
    case 5:
        background1 = LoadTexture("Space1.png");
        background2 = LoadTexture("Space2.png");
        break;
    default:
        // Fallback textures
        background1 = LoadTexture("Map1.png");
        background2 = LoadTexture("Map2.png");
        break;
    }

    // Set background dimensions
    if (background1.id != 0) {
        background1.height = screenHeight;
        background1.width = 4 * screenWidth;
    }
    if (background2.id != 0) {
        background2.height = screenHeight;
        background2.width = 4 * screenWidth;
    }

    // Load UFO texture for levels that use UFOs
    if (levelNumber == 2 || levelNumber == 3 || levelNumber == 5) {
        ufoTexture = LoadTexture("ufo.png");
        if (ufoTexture.id != 0) {
            ufoTexture.height = 50;
            ufoTexture.width = 80;
        }
    }

    // Load tank texture for levels that use tanks
    if (levelNumber != 5) { // Level 5 only has UFOs and boss
        enemyTexture = LoadTexture("EnemyTank.png");
        if (enemyTexture.id != 0) {
            enemyTexture.height = screenHeight / 4 - 150;
            enemyTexture.width = screenWidth / 4 - 150;
        }
    }
}

void Level::Reset() {
    bg1X = 0;
    bg2X = screenWidth * 4; // Make sure this matches the background width
    spawnTimer = 0.0f;
    enemiesSpawned = 0;
    levelComplete = false;

    // Deactivate all enemies
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].SetActive(false);
    }

    // Reset boss for level 5
    if (levelNumber == 5) {
        bossActive = true;  // Make sure boss is active
        bossHealth = 1000;
        bossPosition = { (float)(screenWidth - 350), 200.0f };
        bossShootTimer = 0.0f;
        bossDirection = 1.0f;

        // Initialize boss bullets
        for (int i = 0; i < MAX_BOSS_BULLETS; i++) {
            bossBullets[i].SetActive(false);
        }
    }
    else {
        bossActive = false;
        bossHealth = 1000;
        bossShootTimer = 0.0f;

        // Initialize boss bullets array even for non-boss levels
        for (int i = 0; i < MAX_BOSS_BULLETS; i++) {
            bossBullets[i].SetActive(false);
        }
    }
}

void Level::Update(float deltaTime, Player& player, int& score)
{
    UpdateBackground(player);
    UpdateEnemies(deltaTime, player, score);

    if (levelNumber == 5) {
        UpdateBoss(deltaTime, player, score);
        // Check all collisions for level 5 (including boss bullets vs player)
        CheckCollisions(player, score);

        // Check boss bullets vs player specifically
        Rectangle playerRect = player.GetCollider();
        for (int i = 0; i < MAX_BOSS_BULLETS; i++) {
            if (bossBullets[i].IsActive()) {
                Rectangle bulletRect = bossBullets[i].GetCollider();
                if (CheckCollisionRecs(bulletRect, playerRect)) {
                    bossBullets[i].SetActive(false);
                    player.TakeDamage(25); // Boss bullets do more damage
                    break;
                }
            }
        }

        levelComplete = IsBossDefeated();
    }
    else {
        CheckCollisions(player, score);

        // Original level complete logic for other levels
        if (enemiesSpawned >= enemiesToSpawn) {
            bool allEnemiesDefeated = true;
            for (int i = 0; i < MAX_ENEMIES; i++) {
                if (enemies[i].IsActive()) {
                    allEnemiesDefeated = false;
                    break;
                }
            }
            if (allEnemiesDefeated) {
                levelComplete = true;
            }
        }
    }
}

void Level::UpdateEnemies(float deltaTime, Player& player, int& score) {
    // Update existing enemies
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].IsActive()) {
            // Update zombie target position if it's a zombie
            if (enemies[i].GetEnemyType() == ZOMBIE) {
                enemies[i].SetTargetPosition(player.GetPosition());
            }
            enemies[i].Update(deltaTime, screenWidth);
        }
    }

    // Spawn new enemies
    spawnTimer += deltaTime;
    if (spawnTimer >= spawnInterval && enemiesSpawned < enemiesToSpawn) {
        SpawnEnemy();
        spawnTimer = 0.0f;
    }
}

void Level::UpdateBackground(Player& player) {
    // Player handles its own background movement through references
    // Just handle the background looping here

    int bgWidth = screenWidth * 4; // Background width

    // Loop backgrounds when they go off screen
    if (bg1X <= -bgWidth) {
        bg1X = bg2X + bgWidth;
    }
    if (bg2X <= -bgWidth) {
        bg2X = bg1X + bgWidth;
    }

    // Handle forward looping  
    if (bg1X >= bgWidth) {
        bg1X = bg2X - bgWidth;
    }
    if (bg2X >= bgWidth) {
        bg2X = bg1X - bgWidth;
    }
}

void Level::SpawnEnemy() {
    // Count active enemies
    int activeEnemies = 0;
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].IsActive()) {
            activeEnemies++;
        }
    }

    if (activeEnemies < maxActiveEnemies) {
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (!enemies[i].IsActive()) {
                if (levelNumber == 5) {
                    // In boss level, only spawn UFOs
                    Vector2 spawnPos = { (float)(screenWidth + 100), (float)GetRandomValue(50, 250) };
                    float enemySpeed = 2.0f; // Slower UFOs in boss level

                    enemies[i].InitializeUFO(spawnPos, enemySpeed, ufoTexture, 50.0f, 300.0f);
                    enemies[i].SetLevel(levelNumber);
                    enemies[i].SetDamage(15);
                    enemies[i].SetShootInterval(2.0f); // Slower shooting
                    enemies[i].SetBulletSpeed(6.0f);
                }
                else if (levelNumber == 4) {
                    // Spawn Zombie (Level 4 - Zombie horde)
                    Vector2 spawnPos = { (float)(screenWidth + 100), screenHeight / 2 + 350 };
                    float enemySpeed = 2.5f; // Decreased from 4.5f - slower zombies

                    // Check if zombie texture is loaded
                    if (zombieTexture.id == 0) {
                        std::cout << "ERROR: Zombie texture not loaded when trying to spawn!" << std::endl;
                        // Try to load it again
                        zombieTexture = LoadTexture("Zombie.png");
                    }

                    std::cout << "Spawning zombie with texture ID: " << zombieTexture.id << std::endl;

                    enemies[i].InitializeZombie(spawnPos, enemySpeed, zombieTexture);
                    enemies[i].SetLevel(levelNumber);
                    enemies[i].SetDamage(20); // High damage zombies
                    enemies[i].SetShootInterval(1.0f); // Fast shooting
                    enemies[i].SetBulletSpeed(4.0f); // Decreased from 7.0f - slower bullets

                    // Load and set zombie bullet texture
                    Texture2D zombieBullet = LoadTexture("Zombiebullet.png");
                    if (zombieBullet.id != 0) {
                        enemies[i].SetBulletTexture(zombieBullet);
                    }
                }
                else if (levelNumber == 2) {
                    // Spawn UFO (Level 2 - UFO only)
                    Vector2 spawnPos = { (float)(screenWidth + 100), (float)GetRandomValue(50, 250) };
                    float enemySpeed = 2.5f + (levelNumber * 0.5f);

                    enemies[i].InitializeUFO(spawnPos, enemySpeed, ufoTexture, 50.0f, 300.0f);
                    enemies[i].SetLevel(levelNumber);
                    enemies[i].SetDamage(12 + (levelNumber * 3));
                    enemies[i].SetShootInterval(1.5f);
                    enemies[i].SetBulletSpeed(8.0f);
                }
                else if (levelNumber == 3)
                {
                    // Level 3 - Mixed enemies (50% chance for each type)
                    if (GetRandomValue(0, 1) == 0) {
                        // Spawn Tank with regular bullet texture
                        Vector2 spawnPos = { (float)(screenWidth + 100), screenHeight / 2 + 350 };
                        float enemySpeed = 3.0f + (levelNumber * 0.75f);

                        enemies[i].Initialize(spawnPos, enemySpeed, enemyTexture);
                        enemies[i].SetLevel(levelNumber);
                        enemies[i].SetDamage(8 + (levelNumber * 4));
                        enemies[i].SetShootInterval(2.2f - (levelNumber * 0.15f));
                        enemies[i].SetBulletSpeed(8.0f);

                        // Make sure tank uses regular bullet texture
                        Texture2D tankBullet = LoadTexture("tankbullet.png");
                        if (tankBullet.id != 0) {
                            enemies[i].SetBulletTexture(tankBullet);
                        }
                    }
                    else {
                        // Spawn UFO with UFO bullet texture
                        Vector2 spawnPos = { (float)(screenWidth + 100), (float)GetRandomValue(50, 250) };
                        float enemySpeed = 2.5f + (levelNumber * 0.5f);

                        enemies[i].InitializeUFO(spawnPos, enemySpeed, ufoTexture, 50.0f, 300.0f);
                        enemies[i].SetLevel(levelNumber);
                        enemies[i].SetDamage(12 + (levelNumber * 3));
                        enemies[i].SetShootInterval(1.5f);
                        enemies[i].SetBulletSpeed(8.0f);
                    }
                }
                else {
                    // Spawn Tank (level 1)
                    Vector2 spawnPos = { (float)(screenWidth + 100), screenHeight / 2 + 350 };
                    float enemySpeed = 3.0f + (levelNumber * 0.75f);

                    enemies[i].Initialize(spawnPos, enemySpeed, enemyTexture);
                    enemies[i].SetLevel(levelNumber);
                    enemies[i].SetDamage(8 + (levelNumber * 4));
                    enemies[i].SetShootInterval(2.2f - (levelNumber * 0.15f));
                    enemies[i].SetBulletSpeed(8.0f);
                }

                enemiesSpawned++;
                break;
            }
        }
    }
}

void Level::CheckCollisions(Player& player, int& score) {
    Rectangle playerRect = player.GetCollider();
    Bullet* playerBullets = player.GetBullets();

    // Check collisions with regular enemies (including UFOs and Zombies)
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].IsActive()) continue;

        Rectangle enemyRect = enemies[i].GetCollider();

        // Check player-enemy collision
        if (CheckCollisionRecs(playerRect, enemyRect)) {
            player.TakeDamage(enemies[i].GetDamage());
            enemies[i].SetActive(false);
            continue;
        }

        // Check player bullets vs enemy
        for (int j = 0; j < 15; j++) { // MAX_BULLETS from Player class
            if (playerBullets[j].IsActive()) {
                Rectangle bulletRect = playerBullets[j].GetCollider();
                if (CheckCollisionRecs(bulletRect, enemyRect)) {
                    playerBullets[j].SetActive(false);
                    enemies[i].TakeDamage(50);
                    if (!enemies[i].IsActive()) {
                        // Different point values for different enemy types
                        if (enemies[i].GetEnemyType() == UFO) {
                            score += 150;
                        }
                        else if (enemies[i].GetEnemyType() == ZOMBIE) {
                            score += 75; // Less points but more enemies
                        }
                        else {
                            score += 100; // Tanks
                        }
                    }
                    break;
                }
            }
        }

        // Check enemy bullets vs player
        Bullet* enemyBullets = enemies[i].GetBullets();
        for (int j = 0; j < 5; j++) {
            if (enemyBullets[j].IsActive()) {
                Rectangle bulletRect = enemyBullets[j].GetCollider();
                if (CheckCollisionRecs(bulletRect, playerRect)) {
                    enemyBullets[j].SetActive(false);
                    player.TakeDamage(enemies[i].GetDamage());
                    break;
                }
            }
        }
    }
}

void Level::Draw() {
    // Draw backgrounds
    if (background1.id != 0) {
        DrawTexture(background1, bg1X, 0, WHITE);
    }
    else {
        DrawRectangle(bg1X, 0, screenWidth * 4, screenHeight, DARKBLUE);
    }

    if (background2.id != 0) {
        DrawTexture(background2, bg2X, 0, WHITE);
    }
    else {
        DrawRectangle(bg2X, 0, screenWidth * 4, screenHeight, DARKGREEN);
    }

    // Always draw boss for level 5 if active
    if (levelNumber == 5 && bossActive) {
        // Create a dummy player reference if needed for the DrawBoss call
        if (playerRef != nullptr) {
            DrawBoss(*playerRef);
        }
        else {
            // Fallback: draw boss without player reference
            if (bossHealth > 0) {
                if (bossTexture.id != 0) {
                    DrawTexture(bossTexture, (int)bossPosition.x, (int)bossPosition.y, WHITE);
                }
                else {
                    DrawRectangle((int)bossPosition.x, (int)bossPosition.y, 300, 200, RED);
                }

                // Draw health bar
                float healthPercent = (float)bossHealth / 1000.0f;
                int healthBarWidth = 400;
                int healthBarHeight = 25;
                int healthBarX = screenWidth / 2 - healthBarWidth / 2;
                int healthBarY = 50;

                DrawRectangle(healthBarX, healthBarY, healthBarWidth, healthBarHeight, Fade(BLACK, 0.7f));
                DrawRectangle(healthBarX, healthBarY, (int)(healthBarWidth * healthPercent), healthBarHeight, RED);
                DrawRectangleLines(healthBarX, healthBarY, healthBarWidth, healthBarHeight, WHITE);
                DrawText("BOSS", healthBarX - 60, healthBarY + 2, 20, WHITE);
                DrawText(TextFormat("%d/1000", bossHealth), healthBarX + healthBarWidth + 10, healthBarY + 2, 20, WHITE);
            }
        }
    }
}

void Level::DrawEnemies() {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].IsActive()) {
            enemies[i].Draw();
            enemies[i].DrawBullets();
        }
    }

    // Draw boss bullets for level 5
    if (levelNumber == 5) {
        for (int i = 0; i < MAX_BOSS_BULLETS; i++) {
            bossBullets[i].Draw();
        }
    }
}

void Level::InitializeBoss() {
    bossActive = true;
    bossHealth = 1000;
    bossPosition = { (float)(screenWidth - 350), 200.0f };
    bossSpeed = 1.0f;
    bossDirection = 1.0f;
    bossShootTimer = 0.0f;
    bossShootInterval = 1.5f;
    bossMinY = 100.0f;
    bossMaxY = 400.0f;

    // Load boss textures
    bossTexture = LoadTexture("Boss.png");
    bossBulletTexture = LoadTexture("Bossbullet.png");
    bossDeathTexture = LoadTexture("Bossdeath.png");

    if (bossTexture.id != 0) {
        bossTexture.width = 300;
        bossTexture.height = 200;
    }

    if (bossBulletTexture.id == 0) {
        // Fallback to regular bullet if boss bullet not found
        bossBulletTexture = LoadTexture("bullet.png");
    }

    // Initialize boss bullets
    for (int i = 0; i < MAX_BOSS_BULLETS; i++) {
        bossBullets[i] = Bullet();
        bossBullets[i].SetTexture(bossBulletTexture);
        bossBullets[i].SetActive(false);
    }
}

void Level::UpdateBoss(float deltaTime, Player& player, int& score) {
    if (!bossActive || bossHealth <= 0) return;

    // Boss movement - vertical oscillation
    bossPosition.y += bossSpeed * bossDirection;

    // Bounce between min and max Y
    if (bossPosition.y <= bossMinY) {
        bossPosition.y = bossMinY;
        bossDirection = 1.0f;
    }
    else if (bossPosition.y >= bossMaxY) {
        bossPosition.y = bossMaxY;
        bossDirection = -1.0f;
    }

    // Boss shooting
    bossShootTimer += deltaTime;
    if (bossShootTimer >= bossShootInterval) {
        bossShootTimer = 0.0f;
        BossShoot(player.GetPosition());
    }

    // Update boss bullets
    for (int i = 0; i < MAX_BOSS_BULLETS; i++) {
        if (bossBullets[i].IsActive()) {
            bossBullets[i].Update();
            // Remove bullets that go off screen
            if (bossBullets[i].GetPosition().x < -50 ||
                bossBullets[i].GetPosition().y < -50 ||
                bossBullets[i].GetPosition().y > screenHeight + 50) {
                bossBullets[i].SetActive(false);
            }
        }
    }

    // Check for player bullet collisions with boss
    Bullet* playerBullets = player.GetBullets();
    Rectangle bossRect = { bossPosition.x, bossPosition.y, 300.0f, 200.0f };

    for (int i = 0; i < 15; i++) { // MAX_BULLETS from Player class
        if (playerBullets[i].IsActive()) {
            Rectangle bulletRect = playerBullets[i].GetCollider();
            if (CheckCollisionRecs(bulletRect, bossRect)) {
                playerBullets[i].SetActive(false);
                bossHealth -= 50;
                score += 10; // Give points for hitting boss

                if (bossHealth <= 0) {
                    bossHealth = 0;
                    score += 1000; // Big bonus for defeating boss
                    levelComplete = true;
                }
                break;
            }
        }
    }

    // Check collisions between boss bullets and player
    CheckCollisions(player, score);
}

void Level::BossShoot(Vector2 playerPos) {
    // Find an inactive bullet
    for (int i = 0; i < MAX_BOSS_BULLETS; i++) {
        if (!bossBullets[i].IsActive()) {
            // Calculate direction toward player
            Vector2 bulletPos = { bossPosition.x, bossPosition.y + 100 }; // Center of boss
            Vector2 direction = { playerPos.x - bulletPos.x, playerPos.y - bulletPos.y };

            // Normalize direction
            float length = sqrt(direction.x * direction.x + direction.y * direction.y);
            if (length > 0) {
                direction.x /= length;
                direction.y /= length;
            }

            bossBullets[i].Fire(bulletPos, 8.0f, bossBulletTexture, direction);
            break;
        }
    }
}

void Level::DrawBoss(const Player& player) {
    if (!bossActive) return;

    if (bossHealth > 0) {
        // Draw normal boss
        if (bossTexture.id != 0) {
            DrawTexture(bossTexture, (int)bossPosition.x, (int)bossPosition.y, WHITE);
        }
        else {
            DrawRectangle((int)bossPosition.x, (int)bossPosition.y, 300, 200, RED);
        }

        // Draw boss health bar at fixed position at top of screen
        float healthPercent = (float)bossHealth / 1000.0f;
        int healthBarWidth = 400;
        int healthBarHeight = 25;
        int healthBarX = screenWidth / 2 - healthBarWidth / 2; // Center horizontally
        int healthBarY = 50; // Fixed position at top

        // Background (empty health)
        DrawRectangle(healthBarX, healthBarY, healthBarWidth, healthBarHeight, Fade(BLACK, 0.7f));

        // Current health
        DrawRectangle(healthBarX, healthBarY, (int)(healthBarWidth * healthPercent), healthBarHeight, RED);

        // Border
        DrawRectangleLines(healthBarX, healthBarY, healthBarWidth, healthBarHeight, WHITE);

        // Boss label
        DrawText("BOSS", healthBarX - 60, healthBarY + 2, 20, WHITE);

        // Health text
        DrawText(TextFormat("%d/1000", bossHealth), healthBarX + healthBarWidth + 10, healthBarY + 2, 20, WHITE);
    }
    else {
        // Draw death animation or explosion effect
        if (bossDeathTexture.id != 0) {
            DrawTexture(bossDeathTexture, (int)bossPosition.x, (int)bossPosition.y, WHITE);
        }
        else {
            // Simple explosion effect
            DrawRectangle((int)bossPosition.x, (int)bossPosition.y, 300, 200, ORANGE);
            DrawText("DESTROYED", (int)bossPosition.x + 80, (int)bossPosition.y + 90, 30, WHITE);
        }
    }
}