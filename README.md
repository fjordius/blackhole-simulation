# Black Hole Simulation

A real-time 3D black hole simulation built in C++ using Raylib.

## Features

- Event Horizon - The point of no return
- Accretion Disk - 20,000 particles with temperature-based coloring
- Einstein Ring - Gravitationally lensed light ring
- Spacetime Grid - Visualize how mass warps spacetime
- Gravity Field Lines - Animated lines showing gravitational pull
- Relativistic Jets - Plasma beams from the poles
- Photon Sphere - Light orbiting at critical radius
- Doppler Beaming - Relativistic brightness effects
- Infalling Matter - Gas streams being pulled in
- Starfield - 3,000 twinkling stars

## Controls

- W/S - Move camera up/down
- A/D - Rotate camera
- Scroll - Zoom
- Space - Toggle auto-rotate
- G - Toggle spacetime grid
- F - Toggle field lines
- ESC - Exit

## Build

Requires MSYS2 with MinGW-w64 and Raylib installed.

Compile with:
g++ -O3 -o blackhole.exe blackhole.cpp -lraylib -lopengl32 -lgdi32 -lwinmm

## Credits

Built with Raylib. Inspired by Interstellar (2014).

## License

MIT
