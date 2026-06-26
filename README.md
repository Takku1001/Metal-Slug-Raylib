# Metal-Slug-Raylib

A Metal Slug-style 2D run-and-gun shooter built in C++ with [raylib](https://www.raylib.com/). Fight through five themed levels (desert, jungle, city, space) against tanks, UFOs and zombies, then take on the end boss.

## Tech stack

- **C++14**
- **raylib 5.5** (graphics, input, audio)
- **CMake** build system
- **vcpkg** for dependency management

## Project structure

```
Metal-Slug-Raylib/
├── src/            # Implementation: main.cpp, Game, Player, Enemy, Level, Bullet
├── include/        # Class headers
├── assets/         # Sprites, parallax backgrounds, music
├── tests/          # Headless core-logic tests (no GPU/window needed)
└── CMakeLists.txt
```

## Prerequisites

- CMake 3.16+
- A C++14 compiler (MSVC, MinGW, or Clang)
- raylib 5.5 — easiest via vcpkg: `vcpkg install raylib`

## Build & run

```sh
# Configure (point CMAKE_TOOLCHAIN_FILE at your vcpkg install)
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=<vcpkg>/scripts/buildsystems/vcpkg.cmake

# Build
cmake --build build --config Debug

# Run (assets are copied next to the executable during the build)
cd build/Debug && ./metal_slug
```

## Tests

The core game logic (bullet motion, off-screen detection, colliders, player health)
is covered by a headless test harness that runs without opening a window:

```sh
ctest --test-dir build -C Debug
# or run the binary directly:
./build/Debug/core_tests
```

## Controls

| Action | Keys |
| ------ | ---- |
| Move   | `A` / `D` or `←` / `→` |
| Jump   | `W` / `↑` / `Space` |
| Shoot  | `S` |
| Start / confirm | `Enter` |
| Pause  | `Esc` |
| Restart | `R` |
| Quit   | `Q` |

## Author

Muhammad Rayyan Malik
