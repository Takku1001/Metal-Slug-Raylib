#include "Game.h"
#include <iostream>

Game::Game() {
    currentState = MAIN_MENU;
    currentLevel = 0;

    score = 0;
    gameRunning = false;
    title = "METAL SLUG";
    subtitle = "PRESS ENTER TO START";
    menuBackground = {}; // Initialize empty texture
    backgroundMusic = {}; // Initialize empty music
    audioEnabled = false; // Start with audio disabled
    musicLoaded = false;  // No music loaded initially

    levelIntroTimer = 5.0f; // Changed from 3.0f to 5.0f
    showingLevelIntro = true;

    levelProgress = 0.0f;
    levelProgressSpeed = 1.0f/40.0f;

}

Game::~Game() {
    // Don't call Cleanup here - it should be called explicitly in Run()
    // Cleanup();
}
void Game::Initialize() {
    // Get monitor dimensions
    int monitorWidth = GetMonitorWidth(0);
    int monitorHeight = GetMonitorHeight(0);


    levelIntroTimer = 5.0f;
    levelIntroShown = false;

    // Set borderless window flag
    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_TOPMOST);

    // Create borderless window at full monitor size
    InitWindow(monitorWidth, monitorHeight, "METAL SLUG - OOP VERSION");

    //SetWindowState(FLAG_VSYNC_HINT);


    SetTargetFPS(60);
    // ... rest of your initialization
    // SAFE AUDIO INITIALIZATION
    audioEnabled = false;
    musicLoaded = false;

    // Try to initialize audio with multiple safety checks
    try {
        InitAudioDevice();

        // Wait a moment for audio device to fully initialize
        // This can help on some systems
        for (int i = 0; i < 1000000; i++) {} // Small delay

        // Check if audio device initialized properly
        if (IsAudioDeviceReady()) {
            std::cout << "Audio device initialized successfully" << std::endl;
            audioEnabled = true;

            // Try to load music
            backgroundMusic = LoadMusicStream("music.mp3");

            // Multiple checks for music validity
            if (backgroundMusic.stream.buffer != nullptr &&
                backgroundMusic.stream.sampleRate > 0 &&
                backgroundMusic.frameCount > 0) {

                std::cout << "Music loaded successfully" << std::endl;
                SetMusicVolume(backgroundMusic, 0.7f); // Lower volume to be safe
                PlayMusicStream(backgroundMusic);
                musicLoaded = true;
            }
            else {
                std::cout << "Warning: Music file could not be loaded properly" << std::endl;
                // Reset to prevent issues
                backgroundMusic = {};
            }
        }
        else {
            std::cout << "Warning: Audio device failed to initialize" << std::endl;
            audioEnabled = false;
        }
    }
    catch (...) {
        std::cout << "Warning: Exception during audio initialization - running without sound" << std::endl;
        audioEnabled = false;
        musicLoaded = false;
        backgroundMusic = {};
    }

    // Initialize game resources
    LoadResources();
    gameRunning = true;

    // Initialize player
    player.LoadTextures();
    player.Reset();

    // In Game::Initialize()
    for (int i = 0; i < MAX_LEVELS; i++) {
        levels[i].Initialize(i + 1, SCREEN_WIDTH, SCREEN_HEIGHT);
        levels[i].SetPlayerReference(&player);  // Add this line
    }







    // STARTING FROM LEVEL 

    //currentLevel = 3;
    //levels[currentLevel].Reset();
    //player.SetPosition({ 50, 850 });

}

void Game::LoadResources() {
    // Load menu background image
    menuBackground = LoadTexture("menu.png"); // Change this to your actual filename

    // If the image doesn't match screen dimensions, we'll scale it in DrawMainMenu()
}

void Game::Run() {
    while (!WindowShouldClose() && gameRunning) {
        Update();
        Render();
    }

    // Call cleanup explicitly before the destructor
    Cleanup();
}

void Game::Update() {
    // SAFE MUSIC UPDATE - multiple safety checks
    if (audioEnabled && musicLoaded && backgroundMusic.stream.buffer != nullptr) {
        try {
            UpdateMusicStream(backgroundMusic);
        }
        catch (...) {
            std::cout << "Warning: Music update failed - disabling music" << std::endl;
            musicLoaded = false;
        }
    }

    HandleInput();

    switch (currentState) {
    case MAIN_MENU:
        break;

    case PLAYING: {
        if (player.GetHealth() <= 0) {
            ChangeState(GAME_OVER);
            break;
        }

        // Only update game logic if not showing level intro
        if (!showingLevelIntro) {
            // Update level progress based on distance walked
            float distanceRequired = 3000.0f + (currentLevel * 300.0f); // Scales with level
            levelProgress = player.GetTotalDistanceWalked() / distanceRequired;

            // In Game::Update() case PLAYING:
            if (levelProgress >= 1.0f || (currentLevel == 4 && levels[currentLevel].IsLevelComplete())) {
                levelProgress = 0.0f;
                player.ResetDistanceWalked();

                if (currentLevel >= MAX_LEVELS - 1) {
                    ChangeState(VICTORY);
                }
                else {
                    currentLevel++;
                    showingLevelIntro = true;
                    levelIntroTimer = 5.0f;
                    levels[currentLevel].Reset();
                    player.SetPosition({ 50, 850 });
                }
            }

            // Update game world
            int bgx = levels[currentLevel].GetBackgroundX1();
            int bg2x = levels[currentLevel].GetBackgroundX2();
            player.Update(GetFrameTime(), bgx, bg2x, SCREEN_WIDTH, SCREEN_HEIGHT);
            levels[currentLevel].SetBackgroundX1(bgx);
            levels[currentLevel].SetBackgroundX2(bg2x);
            levels[currentLevel].Update(GetFrameTime(), player, score);
        }
        else {
            // Only update the countdown timer during intro
            levelIntroTimer -= GetFrameTime();
            if (levelIntroTimer <= 0) {
                showingLevelIntro = false;
            }
        }
        break;
    }

    case PAUSED:
        break;

    case GAME_OVER:
        break;

    case VICTORY:
        break;
    }
}

void Game::Render() {
    BeginDrawing();

    // Only clear background if we're not in a menu state
    if (currentState == PLAYING || currentState == PAUSED) {
        ClearBackground(BLACK);
    }
    else {
        // For menus, we want a full clear
        ClearBackground(BLACK);
    }

    switch (currentState) {
    case MAIN_MENU:
        DrawMainMenu();
        break;

    case PLAYING:
        DrawGame();
        break;

    case PAUSED:
        DrawGame();
        DrawPauseMenu();
        break;

    case GAME_OVER:
        DrawGameOver();
        break;

    case VICTORY:
        DrawVictory();
        break;
    }

    EndDrawing();
}


void Game::DrawUI() {
    // 1. Draw persistent HUD elements
    DrawText(TextFormat("Score: %d", score), 20, 20, 35, BLACK);
    DrawText(TextFormat("Health: %d", player.GetHealth()), 20, 70, 35, RED);

    // 2. Draw level progress bar (only when not showing level intro)
    if (!showingLevelIntro && currentLevel != 4) {
        // Progress bar background
        float barWidth = SCREEN_WIDTH * 0.6f;
        float barHeight = 15.0f;
        Vector2 barPos = { SCREEN_WIDTH / 2 - barWidth / 2, 10 };

        // Background
        DrawRectangleRec(
            { barPos.x, barPos.y, barWidth, barHeight },
            Fade(BLACK, 0.7f)
        );

        // Filled progress
        DrawRectangleRec(
            { barPos.x, barPos.y, barWidth * levelProgress, barHeight },
            Color{ 0, 200, 0, 255 }  // Bright green
        );

        // Border
        DrawRectangleLinesEx(
            { barPos.x, barPos.y, barWidth, barHeight },
            2.0f,
            WHITE
        );

        // Level indicator
        DrawText(
            TextFormat("Level %d", currentLevel + 1),
            barPos.x + barWidth + 15,
            barPos.y - 3,
            20,
            WHITE
        );
    }

    // 3. Draw level intro overlay (when showing countdown)
    if (showingLevelIntro) 
    {
        // Dark overlay
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.85f));

        // Level text
        const char* levelText = TextFormat("LEVEL %d", currentLevel + 1);
        int levelFontSize = 70;
        Vector2 levelTextSize = MeasureTextEx(GetFontDefault(), levelText, levelFontSize, 2);
        DrawTextEx(
            GetFontDefault(),
            levelText,
            { SCREEN_WIDTH / 2 - levelTextSize.x / 2, SCREEN_HEIGHT / 2 - levelTextSize.y - 40 },
            levelFontSize,
            2,
            GOLD
        );

        // Countdown text
        const char* countText = TextFormat("%d", (int)ceil(levelIntroTimer));
        int countFontSize = 120;
        Vector2 countTextSize = MeasureTextEx(GetFontDefault(), countText, countFontSize, 2);
        DrawTextEx(
            GetFontDefault(),
            countText,
            { SCREEN_WIDTH / 2 - countTextSize.x / 2, SCREEN_HEIGHT / 2 - 10 },
            countFontSize,
            2,
            WHITE
        );

        // Help text
        const char* helpText = "GET READY!";
        int helpFontSize = 30;
        Vector2 helpTextSize = MeasureTextEx(GetFontDefault(), helpText, helpFontSize, 2);
        DrawTextEx(
            GetFontDefault(),
            helpText,
            { SCREEN_WIDTH / 2 - helpTextSize.x / 2, SCREEN_HEIGHT / 2 + 150 },
            helpFontSize,
            2,
            SKYBLUE
        );
    }
}


void Game::HandleInput() {
    switch (currentState) {
    case MAIN_MENU:
        if (IsKeyPressed(KEY_ENTER)) {
            ChangeState(PLAYING);
            levelIntroTimer = 5.0f;  // Ensure countdown starts at 5
            showingLevelIntro = true;
        }
        break;

    case PLAYING: {
        if (player.GetHealth() <= 0) {
            ChangeState(GAME_OVER);
            break;
        }

        if (IsKeyPressed(KEY_S)) {
            player.Shoot();
        }

        // Update level progress
        if (!showingLevelIntro) {

            float distanceMoved = player.GetDistanceWalked();
            float totalDistanceRequired = 5000.0f;
            levelProgress = distanceMoved / totalDistanceRequired;

            if (levelProgress > 1.0f) {
                levelProgress = 1.0f;
                if (currentLevel >= MAX_LEVELS - 1) {
                    ChangeState(VICTORY);
                }
                else {
                    currentLevel++;
                    showingLevelIntro = true;
                    levelIntroTimer = 5.0f;  // Changed to 5.0f for consistent countdown
                    levels[currentLevel].Reset();
                    player.SetPosition({ 50, 850 });
                }
            }
        }
        else {
            levelIntroTimer -= GetFrameTime();
            if (levelIntroTimer <= 0) {
                showingLevelIntro = false;
            }
        }

        int bgx = levels[currentLevel].GetBackgroundX1();
        int bg2x = levels[currentLevel].GetBackgroundX2();
        player.Update(GetFrameTime(), bgx, bg2x, SCREEN_WIDTH, SCREEN_HEIGHT);
        levels[currentLevel].SetBackgroundX1(bgx);
        levels[currentLevel].SetBackgroundX2(bg2x);
        levels[currentLevel].Update(GetFrameTime(), player, score);
        break;
    }

    case PAUSED:
        if (IsKeyPressed(KEY_ESCAPE)) {
            ChangeState(PLAYING);
        }
        if (IsKeyPressed(KEY_Q)) {
            ChangeState(MAIN_MENU);
        }
        break;

    case GAME_OVER:
        if (IsKeyPressed(KEY_R)) {
            RestartGame();
        }
        if (IsKeyPressed(KEY_Q)) {
            ChangeState(MAIN_MENU);
        }
        break;

    case VICTORY:
        if (IsKeyPressed(KEY_R)) {
            RestartGame();
        }
        if (IsKeyPressed(KEY_Q)) {
            ChangeState(MAIN_MENU);
        }
        break;
    }
}

void Game::ChangeState(GameState newState) {
    currentState = newState;
}

void Game::NextLevel() {
    currentLevel++;
    if (currentLevel >= MAX_LEVELS) {
        ChangeState(VICTORY);
    }
    else {
        player.ResetDistanceWalked(); // Explicit reset
        player.SetPosition({ 50, 850 });
        levels[currentLevel].Reset();
        showingLevelIntro = true;
        levelIntroTimer = 5.0f;
        ChangeState(PLAYING);
    }
}

void Game::RestartGame() {
    currentLevel = 0;
    score = 0;
    player.Reset();
    player.ResetDistanceWalked(); // Explicit reset

    for (int i = 0; i < MAX_LEVELS; i++) {
        levels[i].Reset();
    }

    levelProgress = 0.0f;
    showingLevelIntro = true;
    levelIntroTimer = 5.0f;
    ChangeState(PLAYING);
}

void Game::ResetCurrentLevel() 
{
    levels[currentLevel].Reset();
    player.SetPosition({ 50, 850 }); // Changed from 650 to 850 to match tank level
    player.SetHealth(100);
}

void Game::DrawMainMenu() {
    // Draw background image scaled to fit screen
    if (menuBackground.id != 0) {
        // Method 1: Scale to fit exactly (may distort aspect ratio)
        DrawTexturePro(
            menuBackground,
            { 0, 0, (float)menuBackground.width, (float)menuBackground.height }, // Source rectangle
            { 0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT },                 // Destination rectangle
            { 0, 0 },                                                            // Origin
            0.0f,                                                                // Rotation
            WHITE                                                                // Tint
        );

        // Method 2: Alternative - Scale to fit while maintaining aspect ratio (uncomment if preferred)
        /*
        float scaleX = (float)SCREEN_WIDTH / menuBackground.width;
        float scaleY = (float)SCREEN_HEIGHT / menuBackground.height;
        float scale = (scaleX < scaleY) ? scaleX : scaleY; // Use smaller scale to fit within screen

        int drawWidth = (int)(menuBackground.width * scale);
        int drawHeight = (int)(menuBackground.height * scale);
        int offsetX = (SCREEN_WIDTH - drawWidth) / 2;
        int offsetY = (SCREEN_HEIGHT - drawHeight) / 2;

        DrawTexturePro(
            menuBackground,
            { 0, 0, (float)menuBackground.width, (float)menuBackground.height },
            { (float)offsetX, (float)offsetY, (float)drawWidth, (float)drawHeight },
            { 0, 0 },
            0.0f,
            WHITE
        );
        */
    }
    else {
        // Fallback: Draw a gradient background if image fails to load
        for (int i = 0; i < SCREEN_HEIGHT; i++) {
            float alpha = (float)i / SCREEN_HEIGHT;
            Color color = ColorLerp(DARKBLUE, BLACK, alpha);
            DrawLine(0, i, SCREEN_WIDTH, i, color);
        }
    }

    // Draw semi-transparent overlay for better text readability
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.3f));

    // Draw title with shadow effect
    int titleFontSize = 80;
    int titleWidth = MeasureText(title, titleFontSize);
    int titleX = SCREEN_WIDTH / 2 - titleWidth / 2;
    int titleY = SCREEN_HEIGHT / 2 - titleFontSize;

    // Draw shadow
    DrawText(title, titleX + 3, titleY + 3, titleFontSize, BLACK);
    // Draw main title
    DrawText(title, titleX, titleY, titleFontSize, RED);

    // Draw subtitle with shadow
    int subtitleFontSize = 40;
    int subtitleWidth = MeasureText(subtitle, subtitleFontSize);
    int subtitleX = SCREEN_WIDTH / 2 - subtitleWidth / 2;
    int subtitleY = SCREEN_HEIGHT / 2 + titleFontSize / 2;

    // Draw shadow
    DrawText(subtitle, subtitleX + 2, subtitleY + 2, subtitleFontSize, BLACK);
    // Draw main subtitle
    DrawText(subtitle, subtitleX, subtitleY, subtitleFontSize, WHITE);

    // Draw controls with background panel
    int controlsX = 50;
    int controlsY = SCREEN_HEIGHT - 200;
    int panelWidth = 400;
    int panelHeight = 180;

    // Semi-transparent panel for controls
    DrawRectangle(controlsX - 10, controlsY - 10, panelWidth, panelHeight, Fade(BLACK, 0.7f));
    DrawRectangleLines(controlsX - 10, controlsY - 10, panelWidth, panelHeight, WHITE);

    DrawText("CONTROLS:", controlsX, controlsY, 30, YELLOW);
    DrawText("WASD/Arrow Keys - Move", controlsX, controlsY + 40, 20, WHITE);
    DrawText("S - Shoot", controlsX, controlsY + 60, 20, WHITE);
    DrawText("Space/W/Up - Jump", controlsX, controlsY + 80, 20, WHITE);
    DrawText("ESC - Pause", controlsX, controlsY + 100, 20, WHITE);
}

void Game::DrawGame() {
    // Always draw the level background
    levels[currentLevel].Draw();

    // Only draw game objects if not showing level intro
    if (!showingLevelIntro) {
        levels[currentLevel].DrawEnemies();
        player.DrawBullets();
        player.Draw();
    }

    // Draw UI elements (always visible)
    DrawUI();
}


void Game::DrawPauseMenu() {
    // Semi-transparent overlay
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.7f));

    DrawText("PAUSED", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 50, 50, WHITE);
    DrawText("ESC - Resume", SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2, 20, WHITE);
    DrawText("Q - Main Menu", SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2 + 30, 20, WHITE);
}

void Game::DrawGameOver() {
    // Draw background image scaled to fit screen (no color tint)
    if (menuBackground.id != 0) {
        DrawTexturePro(
            menuBackground,
            { 0, 0, (float)menuBackground.width, (float)menuBackground.height }, // Source rectangle
            { 0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT },                 // Destination rectangle
            { 0, 0 },                                                            // Origin
            0.0f,                                                                // Rotation
            WHITE                                                                // Keep original colors
        );
    }
    else {
        // Fallback: Draw a gradient background if image fails to load
        for (int i = 0; i < SCREEN_HEIGHT; i++) {
            float alpha = (float)i / SCREEN_HEIGHT;
            Color color = ColorLerp(RED, BLACK, alpha);
            DrawLine(0, i, SCREEN_WIDTH, i, color);
        }
    }

    // Draw semi-transparent dark overlay (not red) for better text readability
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.4f));

    // Draw main content with shadows
    int gameOverY = SCREEN_HEIGHT / 2 - 50;

    // Game Over title with shadow
    DrawText("GAME OVER", SCREEN_WIDTH / 2 - 200 + 3, gameOverY + 3, 70, BLACK);
    DrawText("GAME OVER", SCREEN_WIDTH / 2 - 200, gameOverY, 70, RED);

    // Final Score with shadow
    int scoreY = SCREEN_HEIGHT / 2 + 20;
    DrawText(TextFormat("Final Score: %d", score), SCREEN_WIDTH / 2 - 100 + 2, scoreY + 2, 30, BLACK);
    DrawText(TextFormat("Final Score: %d", score), SCREEN_WIDTH / 2 - 100, scoreY, 30, WHITE);

    // Control instructions with background panel
    int panelX = SCREEN_WIDTH / 2 - 120;
    int panelY = SCREEN_HEIGHT / 2 + 50;
    int panelWidth = 240;
    int panelHeight = 80;

    // Semi-transparent panel for controls
    DrawRectangle(panelX, panelY, panelWidth, panelHeight, Fade(BLACK, 0.7f));
    DrawRectangleLines(panelX, panelY, panelWidth, panelHeight, RED); // Red border for game over theme

    DrawText("R - Restart", SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2 + 60, 25, GREEN);
    DrawText("Q - Main Menu", SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2 + 90, 25, YELLOW);
}

void Game::DrawLevelComplete() {
    DrawText("LEVEL COMPLETE!", SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 - 50, 50, GREEN);
    DrawText(TextFormat("Level %d Completed!", currentLevel + 1), SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2, 30, WHITE);
    DrawText(TextFormat("Score: %d", score), SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2 + 40, 25, YELLOW);
    DrawText("ENTER - Next Level", SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT / 2 + 80, 25, GREEN);
}

void Game::DrawVictory() {
    // Draw background image scaled to fit screen
    if (menuBackground.id != 0) {
        DrawTexturePro(
            menuBackground,
            { 0, 0, (float)menuBackground.width, (float)menuBackground.height }, // Source rectangle
            { 0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT },                 // Destination rectangle
            { 0, 0 },                                                            // Origin
            0.0f,                                                                // Rotation
            WHITE                                                                // Tint
        );
    }
    else {
        // Fallback: Draw a gradient background if image fails to load
        for (int i = 0; i < SCREEN_HEIGHT; i++) {
            float alpha = (float)i / SCREEN_HEIGHT;
            Color color = ColorLerp(GOLD, DARKGREEN, alpha);
            DrawLine(0, i, SCREEN_WIDTH, i, color);
        }
    }

    // Draw semi-transparent golden overlay for victory effect
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(GOLD, 0.2f));

    // Draw main content with shadows
    int victoryY = SCREEN_HEIGHT / 2 - 100;

    // Victory title with shadow
    DrawText("VICTORY!", SCREEN_WIDTH / 2 - 150 + 3, victoryY + 3, 70, BLACK);
    DrawText("VICTORY!", SCREEN_WIDTH / 2 - 150, victoryY, 70, GOLD);

    // Completion message with shadow
    int completionY = SCREEN_HEIGHT / 2 - 20;
    DrawText("You completed all levels!", SCREEN_WIDTH / 2 - 200 + 2, completionY + 2, 30, BLACK);
    DrawText("You completed all levels!", SCREEN_WIDTH / 2 - 200, completionY, 30, WHITE);

    // Final Score with shadow
    int scoreY = SCREEN_HEIGHT / 2 + 20;
    DrawText(TextFormat("Final Score: %d", score), SCREEN_WIDTH / 2 - 100 + 2, scoreY + 2, 30, BLACK);
    DrawText(TextFormat("Final Score: %d", score), SCREEN_WIDTH / 2 - 100, scoreY, 30, YELLOW);

    // Control instructions with background panel
    int panelX = SCREEN_WIDTH / 2 - 120;
    int panelY = SCREEN_HEIGHT / 2 + 50;
    int panelWidth = 240;
    int panelHeight = 80;

    // Semi-transparent panel for controls
    DrawRectangle(panelX, panelY, panelWidth, panelHeight, Fade(BLACK, 0.7f));
    DrawRectangleLines(panelX, panelY, panelWidth, panelHeight, WHITE);

    DrawText("R - Play Again", SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2 + 60, 25, GREEN);
    DrawText("Q - Main Menu", SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2 + 90, 25, YELLOW);
}

void Game::Cleanup() {
    std::cout << "Starting cleanup..." << std::endl;

    // SAFE AUDIO CLEANUP - multiple layers of protection
    if (audioEnabled) {
        try {
            if (musicLoaded && backgroundMusic.stream.buffer != nullptr) {
                std::cout << "Stopping music..." << std::endl;

                // Stop music first
                try {
                    StopMusicStream(backgroundMusic);
                    std::cout << "Music stopped successfully" << std::endl;
                }
                catch (...) {
                    std::cout << "Warning: Exception stopping music" << std::endl;
                }

                // Small delay to ensure stop completes
                for (int i = 0; i < 1000000; i++) {}

                // Unload music
                try {
                    UnloadMusicStream(backgroundMusic);
                    std::cout << "Music unloaded successfully" << std::endl;
                }
                catch (...) {
                    std::cout << "Warning: Exception unloading music" << std::endl;
                }

                musicLoaded = false;
            }

            // Close audio device
            try {
                if (IsAudioDeviceReady()) {
                    CloseAudioDevice();
                    std::cout << "Audio device closed successfully" << std::endl;
                }
            }
            catch (...) {
                std::cout << "Warning: Exception closing audio device" << std::endl;
            }

            audioEnabled = false;
        }
        catch (...) {
            std::cout << "Warning: Exception during audio cleanup" << std::endl;
        }
    }

    // Unload menu background safely
    try {
        if (menuBackground.id != 0) {
            UnloadTexture(menuBackground);
            std::cout << "Menu background unloaded successfully" << std::endl;
        }
    }
    catch (...) {
        std::cout << "Warning: Exception unloading menu background" << std::endl;
    }

    // Close window - this should always be last
    try {
        if (IsWindowReady()) {
            CloseWindow();
            std::cout << "Window closed successfully" << std::endl;
        }
    }
    catch (...) {
        std::cout << "Warning: Exception closing window" << std::endl;
    }

    std::cout << "Cleanup completed" << std::endl;
}