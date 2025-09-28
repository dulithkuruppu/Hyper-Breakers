
# Hyper Breakers

A fast-paced 2D multiplayer combat platformer built from scratch in C using SDL2. Two players battle in dynamic arenas featuring physics-based movement, multiple weapon types, and environmental hazards.

## Project Overview

Hyper Breakers is a local multiplayer fighting game where two players compete on a single keyboard. Players navigate platform-based arenas, collect randomly spawning weapons, and engage in combat using projectile-based weapons with realistic trajectories. The game features a complete physics system, smooth animations, and comprehensive audio feedback.

## Key Features

- **Local Multiplayer Combat** - Two players battle on the same keyboard with separate control schemes
- **5 Unique Weapons** - Each with different damage, fire rate, ammunition, and special properties
- **Physics-Based Movement** - Acceleration, friction, gravity, and momentum create realistic character control
- **Dynamic Environments** - Moving platforms, jump pads, climbable ladders, and damaging hazards
- **Polished Audio-Visual Experience** - Character animations, weapon effects, and context-aware soundtrack
- **Multiple Game States** - Main menu, gameplay, pause screen, and game over screen with smooth transitions

## Demo Video

https://github.com/user-attachments/assets/8aa803c6-8009-4ef6-896a-ba2ff821b112

## How It Works

The game runs a 60 FPS game loop that processes input, updates game physics, checks collisions, and renders the current state. Players control characters that can run, jump, crouch, climb ladders, and shoot in any direction.

## Core Systems

**GameObject Architecture**: Every interactive element (platforms, hazards, bullets, weapons) is a GameObject with polymorphic behavior through function pointers, allowing different responses to collisions and updates without OOP.

**Physics Engine**: Implements gravity, collision detection with separated axis checking, and velocity-based movement with delta time for frame-independent physics.

**Weapon System**: Weapons spawn every 10 seconds on random platforms. Each weapon type has unique properties:
- Pistol: Balanced starter weapon
- Rifle: Rapid-fire with burst mode
- Shotgun: Multiple projectiles with spread
- Sniper: High damage, slow reload
- RPG: Maximum damage, single shot

**Audio System**: Multi-channel sound mixing allows concurrent sound effects (shooting, jumping, reloading) without cutoff, plus context-aware background music.

## Core Libraries Needed

The following SDL2 libraries are required to build and run Hyper Breakers:

1. **SDL2** (version 2.0.0 or higher)
   - Main library for window creation and input handling
2. **SDL2_image**
   - For loading PNG/JPG sprites
3. **SDL2_mixer**
   - For audio playback and sound mixing
4. **SDL2_ttf**
   - For rendering text and fonts

## Build and Run Instructions

1. **Clone the repository:**
   ```bash
   git clone [repository-url]
   cd hyper-breakers
   ```

2. **Compile the game:**
   ```bash
   make clean
   make
   ```

3. **Run the game:**
   ```bash
   ./hyperbreakers
   ```

## Controls

### Player 1 (Left side)

| Key | Action |
|-----|--------|
| A / D | Move left / right |
| W | Jump / Aim upward |
| S | Crouch / Aim downward / Drop through platforms |
| 1 | Hold to aim, release to shoot |
| 2 | Pick up weapon (while crouching) |

### Player 2 (Right side)

| Key | Action |
|-----|--------|
| ← / → | Move left / right |
| ↑ | Jump / Aim upward |
| ↓ | Crouch / Aim downward / Drop through platforms |
| N | Hold to aim, release to shoot |
| M | Pick up weapon (while crouching) |

### General Controls

- **P** - Pause/Resume game
- **ESC** - Exit game

## My Contributions

### Player Input and Control System
- **Designed and implemented the complete player input handling system** (handlePlayerInput() in player.c) that processes keyboard inputs for both players simultaneously
- **Developed physics-based movement mechanics** with acceleration and friction rather than instant velocity changes, creating realistic character momentum and weight
- **Implemented the crouch system** that modifies player hitbox dimensions and enables dropping through one-way platforms
- **Created the dual-purpose control scheme** where movement keys double as aiming controls when holding the shoot button

### Shooting and Combat Mechanics
- **Built the complete shooting system** including the hold-to-aim and release-to-fire mechanism that allows precise shot placement
- **Implemented trigonometric bullet trajectory calculations** that compute projectile paths based on player aim angle and facing direction
- **Developed the weapon differentiation system** where each gun type has unique damage values, ammunition counts, reload times, and special properties like shotgun spread
- **Created the weapon pickup mechanism** that detects nearby weapons when players crouch and press the pickup key

### Audio System Integration
- **Integrated comprehensive sound effects** throughout the gameplay, connecting player actions to corresponding audio feedback
- **Implemented context-aware music system** that intelligently pauses/resumes rather than restarting when transitioning between game states
- **Developed multi-channel audio management** preventing sound cutoff by assigning different sound types to separate mixing channels
- **Created the weapon-specific sound system** that plays appropriate audio based on equipped gun type

### Collision Detection and Physics
- **Implemented the separated axis collision checking** that prevents players from getting stuck in walls by checking X and Y collisions independently
- **Developed collision response behaviors** for different surface types including solid platforms, one-way platforms, and bounce pads
- **Created the ground detection system** that enables jumping only when players are on solid surfaces
- **Debugged complex edge cases** in collision detection including corner collisions and moving platform interactions

### Weapon and Item Systems
- **Contributed to the weapon spawning timer system** that generates new weapons every 10 seconds
- **Implemented collision avoidance algorithms** ensuring weapons spawn only in valid locations without overlapping walls
- **Developed the weapon stats application system** that updates player properties when picking up new weapons
- **Created weapon cleanup mechanisms** that remove old weapons when new ones spawn

### Memory Management and Optimization
- **Implemented proper memory cleanup** throughout player and weapon systems using custom free functions
- **Utilized Valgrind extensively** to identify and fix memory leaks, achieving zero leaks in final build
- **Optimized performance** to maintain stable 60 FPS even with multiple bullets and effects on screen

## Project Structure

```
hyper-breakers/
├── src/
│   ├── audio.c/h           # Sound and music management
│   ├── bar.c/h             # Health bar rendering
│   ├── gameLoop.c/h        # Main game loop and state management
│   ├── gameMode.c/h        # Game initialization
│   ├── genericDynamicList.c/h  # Dynamic array implementation
│   ├── level.c/h           # Level building and GameObjects
│   ├── menu.c/h            # UI and menu screens
│   ├── physics.c/h         # Collision detection system
│   ├── player.c/h          # Player controls and mechanics
│   ├── render.c/h          # Rendering pipeline
│   ├── sprites.c/h         # Animation system
│   ├── vector.c/h          # Vector mathematics
│   └── weapon.c/h          # Weapon spawning and stats
├── assets/
│   ├── sprites/            # Character and object graphics
│   ├── sounds/             # Sound effects and music
│   └── fonts/              # UI fonts
├── Makefile                # Build configuration
└── README.md              # This file
```

## Team

This project was developed as part of a 4-person team for the Imperial College London Computing Practical Module's C Project. Each member contributed to different core systems while collaborating on integration and debugging.
