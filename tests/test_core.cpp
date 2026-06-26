// Headless core-logic tests for Metal-Slug-Raylib.
//
// These exercise the pure-logic methods of the game classes (bullet motion,
// off-screen detection, colliders, player health) directly against the real
// code. They deliberately avoid anything that needs an OpenGL context
// (texture loading, drawing), so they run without opening a window.

#include "Bullet.h"
#include "Player.h"

#include <cstdio>
#include <cmath>

static int g_failures = 0;
static int g_checks = 0;

static void check(bool cond, const char* what) {
    ++g_checks;
    if (cond) {
        std::printf("  [PASS] %s\n", what);
    } else {
        std::printf("  [FAIL] %s\n", what);
        ++g_failures;
    }
}

static bool approx(float a, float b) { return std::fabs(a - b) < 0.0001f; }

int main() {
    std::printf("== Bullet ==\n");
    {
        Bullet b;
        check(!b.IsActive(), "default bullet is inactive");
        b.Update();
        check(approx(b.GetPosition().x, 0.0f), "inactive bullet does not move on Update");
    }
    {
        Bullet b;
        Texture2D none = {};            // id == 0 -> uses fallback size {20,8}
        b.Fire({100.0f, 200.0f}, 5.0f, none, {1.0f, 0.0f});
        check(b.IsActive(), "fired bullet is active");
        check(approx(b.GetPosition().x, 100.0f), "fired bullet starts at given x");
        b.Update();
        check(approx(b.GetPosition().x, 105.0f), "rightward bullet advances by speed");
        check(approx(b.GetPosition().y, 200.0f), "rightward bullet keeps its y");
    }
    {
        Bullet b;
        Texture2D none = {};
        b.Fire({100.0f, 0.0f}, 8.0f, none, {-1.0f, 0.0f});
        b.Update();
        check(approx(b.GetPosition().x, 92.0f), "leftward bullet moves left");
    }
    {
        Bullet b;
        Texture2D none = {};
        b.Fire({0.0f, 0.0f}, 5.0f, none, {1.0f, 0.0f});
        b.SetPosition({1300.0f, 0.0f});
        check(b.IsOffScreen(1280, 720), "rightward bullet past right edge is off-screen");
        b.SetPosition({640.0f, 0.0f});
        check(!b.IsOffScreen(1280, 720), "rightward bullet inside screen is not off-screen");
    }
    {
        Bullet b;
        Texture2D none = {};
        b.Fire({700.0f, 0.0f}, 5.0f, none, {1.0f, 0.0f}); // speed > 0
        check(b.IsOffScreen(640), "single-arg IsOffScreen: positive speed past width");
        b.SetPosition({300.0f, 0.0f});
        check(!b.IsOffScreen(640), "single-arg IsOffScreen: inside width is on-screen");

        Rectangle r = b.GetCollider();
        check(approx(r.x, 300.0f) && approx(r.width, 20.0f) && approx(r.height, 8.0f),
              "collider matches position and fallback size");
    }

    std::printf("== Player ==\n");
    {
        Player p;
        check(p.GetHealth() == 100, "new player has 100 health");
        p.TakeDamage(30);
        check(p.GetHealth() == 70, "TakeDamage subtracts damage");
        p.TakeDamage(1000);
        check(p.GetHealth() == 0, "health clamps at 0, never negative");

        p.SetPosition({10.0f, 20.0f});
        Rectangle c = p.GetCollider();
        check(approx(c.x, 15.0f) && approx(c.y, 25.0f) && approx(c.width, 50.0f) && approx(c.height, 55.0f),
              "player collider is offset (+5,+5) with size 50x55");
    }

    std::printf("== Collision ==\n");
    {
        Player p;
        p.SetPosition({100.0f, 100.0f});      // collider {105,105,50,55}
        Bullet hit;
        Texture2D none = {};
        hit.Fire({120.0f, 120.0f}, 5.0f, none, {1.0f, 0.0f}); // overlaps player
        check(CheckCollisionRecs(p.GetCollider(), hit.GetCollider()),
              "overlapping bullet collides with player");

        Bullet miss;
        miss.Fire({900.0f, 900.0f}, 5.0f, none, {1.0f, 0.0f}); // far away
        check(!CheckCollisionRecs(p.GetCollider(), miss.GetCollider()),
              "distant bullet does not collide with player");
    }

    std::printf("\n%d checks, %d failures\n", g_checks, g_failures);
    return g_failures == 0 ? 0 : 1;
}
