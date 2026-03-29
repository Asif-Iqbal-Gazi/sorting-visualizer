# Sorting Visualizer

Interactive sorting algorithm visualizer built with C++ and SDL3.

## Algorithms

- Bubble Sort

## Dependencies

- [SDL3](https://wiki.libsdl.org/SDL3/FrontPage)
- CMake 3.20+
- C++17

## Build

```bash
cmake -B build -DCMAKE_PREFIX_PATH=/opt/homebrew
cmake --build build
./build/sorting_visualizer
```

## Controls

| Key | Action |
|-----|--------|
| `SPACE` | Start sorting |
| `R` | Reshuffle and reset |
| `UP` / `DOWN` | Increase / decrease bar count (idle only) |
| `RIGHT` / `LEFT` | Increase / decrease speed |
| `ESC` | Quit |
