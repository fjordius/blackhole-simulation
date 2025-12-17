

# 🕳️ Black Hole Simulation

A real-time 3D black hole simulation built in C++ using Raylib. Features gravitational lensing, accretion disk physics, relativistic jets, and spacetime curvature visualization.

![Black Hole](https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Raylib](https://img.shields.io/badge/Raylib-000000?style=for-the-badge&logo=raylib&logoColor=white)

## ✨ Features

- **Event Horizon** - The point of no return, rendered as a perfect black sphere
- **Accretion Disk** - 20,000 particles orbiting with temperature-based coloring (white-hot inner, red outer)
- **Einstein Ring** - Gravitationally lensed light forming a glowing ring around the black hole
- **Spacetime Grid** - Visualize how mass warps the fabric of spacetime
- **Gravity Field Lines** - Animated lines showing the pull of gravity
- **Relativistic Jets** - Twin plasma beams shooting from the poles
- **Photon Sphere** - Light orbiting at the critical radius
- **Doppler Beaming** - One side of the disk appears brighter due to relativistic motion
- **Infalling Matter** - Streams of gas being pulled into the black hole
- **Starfield** - 3,000 twinkling background stars with varied colors

## 🎮 Controls

| Key | Action |
|-----|--------|
| `W` / `S` | Move camera up / down |
| `A` / `D` | Rotate camera left / right |
| `Scroll` | Zoom in / out |
| `Space` | Toggle auto-rotate |
| `G` | Toggle spacetime grid |
| `F` | Toggle gravity field lines |
| `↑` / `↓` | Adjust rotation speed |
| `ESC` | Exit |

## 🛠️ Installation

### Prerequisites

- [MSYS2](https://www.msys2.org/) with MinGW-w64
- Raylib

### Setup (Windows)

1. Install MSYS2 from https://www.msys2.org/

2. Open **MSYS2 MINGW64** terminal and install the compiler and raylib:
```bash
pacman -S mingw-w64-x86_64-gcc
pacman -S mingw-w64-x86_64-raylib

    Add C:\msys64\mingw64\bin to your system PATH

    Clone this repo:

Bash

git clone https://github.com/fjordius/blackhole-simulation.git
cd blackhole-simulation

    Compile:

Bash

g++ -O3 -o blackhole.exe blackhole.cpp -lraylib -lopengl32 -lgdi32 -lwinmm

    Run:

Bash

./blackhole.exe

📸 Screenshots

Coming soon
🧪 The Physics

This simulation visualizes several real phenomena predicted by Einstein's General Relativity:

    Gravitational Lensing - Light bends around massive objects, creating the Einstein ring effect
    Spacetime Curvature - Mass tells spacetime how to curve; the grid shows this warping
    Accretion Disk Dynamics - Matter spirals inward, heating up as it falls (inner = hotter = whiter)
    Relativistic Jets - Magnetic fields can launch plasma perpendicular to the disk at near light speed
    Doppler Effect - The approaching side of the rotating disk appears brighter than the receding side

📝 License

MIT License - do whatever you want with it.
🙏 Credits

    Built with Raylib
    Inspired by the black hole from Interstellar (2014)
