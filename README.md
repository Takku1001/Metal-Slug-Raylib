# Metal-Slug-Raylib

A Metal Slug-style 2D run-and-gun shooter in C++ with [raylib](https://www.raylib.com/) — five themed levels (desert, jungle, city, space), tanks, UFOs, zombies, and an end boss.

**Stack:** C++14 · raylib 5.5 · CMake · vcpkg

## Structure

```
src/        # main.cpp, Game, Player, Enemy, Level, Bullet
include/    # class headers
assets/     # sprites, backgrounds, music
tests/      # headless core-logic tests
```

## Build & run

Requires CMake 3.16+, a C++14 compiler, and raylib (`vcpkg install raylib`).

```sh
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=<vcpkg>/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Debug
cd build/Debug && ./metal_slug      # assets are copied next to the binary
ctest --test-dir ../.. -C Debug     # run the tests
```

## Controls

Move `A`/`D` or `←`/`→` · Jump `W`/`↑`/`Space` · Shoot `S` · Start `Enter` · Pause `Esc` · Restart `R` · Quit `Q`

## Author

Muhammad Rayyan Malik
