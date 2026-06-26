#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "Player.h"
#include "Level.h"

enum GameState {
    MAIN_MENU,
    PLAYING,
    PAUSED,
    GAME_OVER,
    LEVEL_COMPLETE,
    VICTORY
};

class Game {
private:
    static const int SCREEN_WIDTH = 1920;
    static const int SCREEN_HEIGHT = 1080;
    static const int MAX_LEVELS = 5;

    
    bool levelIntroShown;

    Music backgroundMusic;
    Texture2D menuBackground; // Add menu background texture
    bool audioEnabled; // Track if audio system is working
    bool musicLoaded;  // Track if music is loaded successfully


    float levelProgress = 0.0f;
    float levelProgressSpeed = 1.0f / 40.0f;  // Takes 40 seconds to fill (1.0 / desired seconds)
    float levelIntroTimer = 3.0f;
    bool showingLevelIntro = false;


    GameState currentState;
    Player player;
    Level levels[MAX_LEVELS];
    int currentLevel;
    int score;
    bool gameRunning;

    // Menu variables
    const char* title;
    const char* subtitle;

public:
    Game();
    ~Game();

    // Initialization
    void Initialize();
    void LoadResources();

    // Game Loop
    void Run();
    void Update();
    void Render();


    void DrawUI();

    // State Management
    void HandleInput();
    void ChangeState(GameState newState);
    void NextLevel();
    void RestartGame();
    void ResetCurrentLevel();

    // Rendering different states
    void DrawMainMenu();
    void DrawGame();
    void DrawPauseMenu();
    void DrawGameOver();
    void DrawLevelComplete();
    void DrawVictory();

    // Utility
    void Cleanup();

    // Getters
    bool IsRunning() const { return gameRunning; }
    GameState GetCurrentState() const { return currentState; }
    int GetCurrentLevel() const { return currentLevel; }
    int GetScore() const { return score; }
};

#endif