# 🎮 KirbyLand Video Game
## 🌎 Can you survive a day as a Kirby?

> **Wake up. Find your stuff. Start the car. Survive the commute.**
>
> Welcome to **KirbyLand** — a 2D C++ adventure game where an ordinary day can quickly turn into a series of very unordinary challenges.

---

## ✨ About KirbyLand

**KirbyLand** is a 2D adventure/minigame-style video game built from scratch using **C++ and SDL2**.

The game follows Kirby through a chaotic day, with each part of the day becoming its own interactive scene or challenge.

Rather than following one gameplay mechanic throughout the entire game, KirbyLand combines:

- 🏃 Character movement
- 🔎 Item collection
- 💬 Character dialogue
- 🎬 Scripted cutscenes
- ⏱️ Timing challenges
- 🚗 Driving mechanics
- 🛣️ Procedurally drawn roads
- 🚧 Obstacle-dodging challenges
- 🌳 Procedurally generated scenery
- 🔀 Branching level paths

The goal is simple:

### **Can you survive a day as a Kirby?**

---

# 🗺️ Current Game Flow

The game currently progresses through several connected scenes:

```text
                    🎮 START
                       │
                       ▼
                🛏️ Bedroom
                       │
                       ▼
                🏠 Outside Home
                       │
                       ▼
                🚗 Start the Car
                       │
                       ▼
                 🛣️ Driving
                       │
                ┌──────┴──────┐
                ▼             ▼
          🏎️ E470        🚧 Gun Club Road
                │             │
                ▼             ▼
        Dodge Traffic    Dodge Construction
```

Your choice during the driving scene determines which challenge Kirby encounters next.

---

# 🎯 Current Levels

## 🛏️ Level 1 — The Bedroom

Kirby wakes up late and realizes there isn't much time before work.

Before leaving the house, the player must locate three missing items scattered throughout the bedroom:

- 📱 Phone
- 👜 Bag
- 💨 Vape

Kirby begins in the center of the bedroom while the opening message explains the objective.

After a short delay, movement unlocks and the player can explore the room.

Collect all three items and reach the exit to continue.

### Features

- Player movement
- Jumping
- Ducking
- Collision detection
- Randomized collectible placement
- Item counter
- Timed intro sequence
- Scene transition system
- Fade effects

---

## 🏠 Level 2 — Outside the House

After escaping the bedroom, Kirby heads outside.

This scene introduces scripted character interactions and the game's dialogue system.

Kirby automatically walks toward Bob, triggering a conversation before continuing toward the car.

### Features

- 🎬 Automated character movement
- 💬 Multi-line dialogue
- 🗨️ Speech bubbles
- 👥 Multiple characters
- ⌨️ Player-controlled dialogue progression
- 🎞️ Cutscene sequencing
- 🌑 Fade transition into the next scene

---

## 🔑 Level 3 — Start the Car

Kirby finally reaches the car...

Unfortunately, getting it started is its own challenge.

The player is presented with a timing-based minigame:

> **Hit the SPACE bar at the right time and start the car.**

A moving timing indicator travels across the meter. Pressing **SPACE** inside the correct timing zone increases the engine progress.

Miss the timing window?

No progress.

### Engine Progress

```text
0%  ─────────────────────────────── 100%
     CLICK
          SPUTTER
               TURNING OVER
                        ALMOST...
                              🚗💨 STARTED!
```

As the engine gets closer to starting, the car reacts more aggressively.

### Features

- ⏱️ Timing-based gameplay
- 🎯 Accuracy window
- 📊 Engine progress meter
- 🚘 Engine state feedback
- 📳 Screen/car shaking effects
- 🖼️ First-person car interior
- 🏁 Success state

---

# 🛣️ Level 4 — The Drive

Once the engine starts, the perspective changes to a **bird's-eye view**.

For the first few seconds, Kirby simply cruises down the highway.

Then...

### 🎮 Steering unlocks.

Use:

```text
←  LEFT

→  RIGHT
```

to control the car.

The road, lane markers, guardrails, trees, signs, rocks, bushes, and other scenery move toward the bottom of the screen to simulate forward motion.

Eventually, the highway splits.

```text
               ┌───────────────┐
               │     E470      │
               │       ↖       │
               └───────────────┘

                        🚗

               ┌───────────────┐
               │ GUN CLUB ROAD │
               │       ↗       │
               └───────────────┘
```

The player must choose a route.

### ⬅️ LEFT — E470

Fast-paced highway driving.

### ➡️ RIGHT — Gun Club Road

A rough construction-filled commute.

**Your decision determines the next level.**

### Features

- 🚗 Player-controlled steering
- 🛣️ Procedurally rendered highway
- 🌲 Scrolling roadside scenery
- 🪨 Rocks and vegetation
- 🪧 Road signs
- 🛡️ Guardrails
- 🛤️ Animated lane markings
- 🔀 Dynamic highway fork
- 🧭 Branching level selection

---

# 🏎️ Level 5A — E470

Choosing **E470** begins a 30-second high-speed traffic challenge.

Cars continuously appear ahead of Kirby and travel down the highway toward the player.

At first, traffic is manageable.

Then it begins getting worse...

```text
0 seconds                          30 seconds
│                                      │
▼                                      ▼

🚗          🚗 🚙         🚙 🚗 🚕 🚗 🚙
LIGHT  ───► MEDIUM ───►     HEAVY TRAFFIC
```

The amount of traffic increases as the timer approaches zero.

Traffic generation is designed around multiple lanes so that the game does not intentionally create an impossible wall. At least one escape route is kept available when a wave is generated.

### Objective

> **Survive 30 seconds of E470 traffic.**

### Features

- 🚙 Procedurally generated traffic
- 🎨 Multiple vehicle colors
- 🛣️ Three-lane highway
- 📈 Increasing difficulty
- ⏲️ 30-second survival timer
- 💥 Vehicle collision detection
- 🎯 Fair lane-based spawning
- 🌲 Moving roadside environment
- 📊 Hit counter
- 🏁 Completion screen

---

# 🚧 Level 5B — Gun Club Road

Maybe E470 didn't sound so great.

Unfortunately...

Neither is Gun Club Road.

Gun Club Road is a 30-second construction-zone challenge filled with equipment and obstacles.

Instead of normal highway traffic, Kirby must dodge:

- 🚜 Bulldozers
- 🚧 Traffic cones
- 🏗️ Construction pipes
- 🛢️ Roadside construction barrels

The road itself has also seen better days.

### Very, very much better days.

The environment includes:

- 🕳️ Potholes
- 🩹 Poor asphalt repairs
- 🛣️ Faded lane markings
- ⚡ Cracked pavement
- 🪨 Dirt piles
- 🚧 Construction equipment
- 🟧 Temporary traffic barriers

The number of hazards increases throughout the level.

### Objective

> **Survive 30 seconds of Gun Club Road construction.**

### Features

- 🚜 Multiple obstacle types
- 🕳️ Procedural potholes
- 🛣️ Damaged road rendering
- 🚧 Construction scenery
- 📈 Increasing difficulty
- 🎯 Lane-based obstacle generation
- 💥 Collision detection
- ⏲️ Survival timer
- 📊 Hit counter
- 🏁 Completion state

---

# 🎮 Controls

| Action | Keyboard |
|---|---|
| Move Left | `A` / `←` |
| Move Right | `D` / `→` |
| Jump | `SPACE` |
| Duck | `S` / `↓` |
| Advance Dialogue | `SPACE` |
| Timing Challenges | `SPACE` |
| Driving | `←` / `→` |

> Controls may change slightly depending on the current scene.

---

# 🧠 How the Game Works

KirbyLand uses a **scene-based architecture**.

Instead of putting the entire game inside one massive game loop, each level is separated into its own scene.

Every scene follows a similar lifecycle:

```cpp
load();
onEnter();
handleEvent();
update();
render();
```

This keeps each level independent while allowing the main game system to handle scene transitions.

Conceptually:

```text
                 ┌─────────────┐
                 │    Game     │
                 └──────┬──────┘
                        │
                  Current Scene
                        │
          ┌─────────────┼─────────────┐
          ▼             ▼             ▼
     handleEvent      update        render
          │             │             │
          └─────────────┴─────────────┘
                        │
                        ▼
                Request Next Scene
```

This architecture makes it much easier to continue adding new levels without turning the project into one enormous source file.

---

# 📁 Project Structure

```text
KirbyLand/
│
├── 📁 assets/
│   └── 📁 images/
│       ├── 📁 backgrounds/
│       │   ├── first_screen
│       │   ├── kirby_background.png
│       │   ├── kirby_home.png
│       │   └── car_scene.png
│       │
│       ├── 📁 kirby_player/
│       │   ├── kirby_pajamas.png
│       │   └── kirby_golf.png
│       │
│       ├── 📁 other_characters/
│       │   └── angry_bob.png
│       │
│       └── 📁 items/
│           ├── vape.png
│           ├── phone.png
│           └── purse.png
│
├── 📁 include/
│   │
│   ├── 📁 core/
│   │   ├── Game.hpp
│   │   ├── Scene.hpp
│   │   └── SceneType.hpp
│   │
│   ├── 📁 entities/
│   │   ├── Player.hpp
│   │   └── CollectibleItem.hpp
│   │
│   ├── 📁 graphics/
│   │   ├── Texture.hpp
│   │   └── TextRenderer.hpp
│   │
│   └── 📁 scenes/
│       ├── StartScene.hpp
│       ├── BedroomScene.hpp
│       ├── HomeScene.hpp
│       ├── CarScene.hpp
│       ├── DrivingScene.hpp
│       ├── E470Scene.hpp
│       └── GunClubScene.hpp
│
├── 📁 src/
│   │
│   ├── main.cpp
│   │
│   ├── 📁 core/
│   │   └── Game.cpp
│   │
│   ├── 📁 entities/
│   │   ├── Player.cpp
│   │   └── CollectibleItem.cpp
│   │
│   ├── 📁 graphics/
│   │   ├── Texture.cpp
│   │   └── TextRenderer.cpp
│   │
│   └── 📁 scenes/
│       ├── StartScene.cpp
│       ├── BedroomScene.cpp
│       ├── HomeScene.cpp
│       ├── CarScene.cpp
│       ├── DrivingScene.cpp
│       ├── E470Scene.cpp
│       └── GunClubScene.cpp
│
├── Makefile
└── README.md
```

---

# 🧩 Code Architecture

## 🎮 `Game`

`Game` acts as the central controller.

It is responsible for:

- SDL initialization
- Window creation
- Renderer creation
- Main game loop
- Event handling
- Scene ownership
- Scene switching
- Shared game systems

The individual levels don't need to know how the entire game works.

They only need to know when they want to move to another scene.

---

## 🎬 `Scene`

`Scene` provides the common interface used by every level.

This gives scenes a predictable structure:

```cpp
bool load();
void onEnter();
void handleEvent(const SDL_Event& event);
void update();
void render();
```

Scenes can then request a transition when their objective has been completed.

---

## 🧍 `Player`

The `Player` system handles the reusable player functionality used in character-based scenes.

This includes mechanics such as:

- Movement
- Jumping
- Ducking
- Position
- Rendering
- Character direction

---

## 💎 `CollectibleItem`

Collectibles are represented separately from the player and scene logic.

This allows scenes such as the bedroom to create multiple collectible objects while sharing the same underlying behavior.

---

## 🖼️ `Texture`

The `Texture` class handles loading and rendering image assets.

Keeping texture management separate prevents every scene from having to duplicate SDL image-loading code.

It also handles transparency so character sprites can render correctly over the game environment.

---

## ✍️ `TextRenderer`

`TextRenderer` provides reusable text and dialogue rendering.

It is used for:

- Objective messages
- Counters
- Dialogue
- Speech bubbles
- HUD information
- Level status messages

This keeps font management and text rendering centralized.

---

# 🛣️ Procedural Driving System

One of the larger systems currently implemented in KirbyLand is the driving environment.

Rather than relying entirely on large background images, the driving scenes generate much of the environment directly through SDL.

The illusion of movement is created by keeping Kirby's car near the bottom of the screen while moving the world toward the player.

```text
          ↓ TREE
          ↓ CAR
          ↓ SIGN
          ↓ LANE MARKING

            🚗
         PLAYER CAR
```

This technique is used for:

- Lane markings
- Road surfaces
- Trees
- Rocks
- Signs
- Guardrails
- Traffic
- Construction equipment
- Potholes
- Road damage

It allows the environment to continue indefinitely without requiring an enormous background texture.

---

# 🔀 Branching Levels

KirbyLand supports branching gameplay.

The driving transition currently provides the first major route decision:

```text
                     Driving
                        │
               ┌────────┴────────┐
               │                 │
               ▼                 ▼
             E470          Gun Club Road
               │                 │
         Highway Traffic    Construction
           Challenge          Challenge
```

Because scene transitions use `SceneType`, additional branches can be added without redesigning the entire game.

This opens the door for future decisions to affect Kirby's day in different ways.

---

# 🛠️ Built With

### Language

![C++](https://img.shields.io/badge/C++-17%2B-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)

### Game / Multimedia Libraries

![SDL2](https://img.shields.io/badge/SDL2-Game%20Framework-0B5F8A?style=for-the-badge)

![SDL_image](https://img.shields.io/badge/SDL2_image-Textures-4C8CBF?style=for-the-badge)

![SDL_ttf](https://img.shields.io/badge/SDL2_ttf-Text-6A5ACD?style=for-the-badge)

### Development

![VS Code](https://img.shields.io/badge/VS%20Code-Editor-007ACC?style=for-the-badge&logo=visualstudiocode&logoColor=white)

![Linux](https://img.shields.io/badge/Linux-WSL-FCC624?style=for-the-badge&logo=linux&logoColor=black)

![Git](https://img.shields.io/badge/Git-Version%20Control-F05032?style=for-the-badge&logo=git&logoColor=white)

---

# ⚙️ Requirements

To build KirbyLand, you will need:

- C++ compiler with C++17 support
- GNU Make
- SDL2
- SDL2_image
- SDL2_ttf

On Ubuntu / WSL:

```bash
sudo apt update

sudo apt install \
    build-essential \
    libsdl2-dev \
    libsdl2-image-dev \
    libsdl2-ttf-dev
```

---

# 🚀 Build & Run

Clone the repository:

```bash
git clone <YOUR-GITHUB-REPOSITORY-URL>
cd KirbyLand
```

Compile:

```bash
make
```

Run the complete game:

```bash
make run
```

Or:

```bash
./kirby_land
```

---

# 🧪 Scene Debugging

KirbyLand includes a useful scene launcher for development.

Instead of replaying every previous level whenever a new scene needs testing, the game can launch directly into a selected scene.

For example:

```bash
make run SCENE=bedroom
```

Or:

```bash
make run SCENE=car
```

Driving transition:

```bash
make run SCENE=driving
```

E470:

```bash
make run SCENE=e470
```

Gun Club Road:

```bash
make run SCENE=gunclub
```

This makes individual levels much faster to develop and debug.

---

# 🧹 Clean Build

To remove previously compiled files:

```bash
make clean
```

Then rebuild:

```bash
make
```

For a completely fresh test:

```bash
make clean
make
make run
```

---

# 🌟 Current Features

- [x] 🎮 Start screen
- [x] 🧍 Player movement
- [x] 🦘 Jumping
- [x] ⬇️ Ducking
- [x] 🖼️ Transparent sprites
- [x] 🗺️ Multiple scenes
- [x] 🎬 Scene transitions
- [x] 🌑 Screen fades
- [x] 🔎 Collectibles
- [x] 🎲 Random item placement
- [x] 💥 Collision detection
- [x] 💬 Dialogue system
- [x] 🗨️ Speech bubbles
- [x] 👥 Multiple characters
- [x] ⏱️ Timing minigame
- [x] 🚗 Driving mechanics
- [x] 🛣️ Procedural roads
- [x] 🌲 Procedural roadside scenery
- [x] 🔀 Branching routes
- [x] 🚙 Procedural traffic
- [x] 📈 Increasing level difficulty
- [x] 🚧 Construction hazards
- [x] 🕳️ Damaged road effects
- [x] ⏲️ Timed survival challenges
- [x] 🧪 Individual scene debugging

---

# 🔮 Future Ideas

KirbyLand is still actively being developed.

Some possible additions include:

- [ ] ❤️ Health system
- [ ] 🎵 Music and sound effects
- [ ] 🔊 Engine / vehicle sounds
- [ ] 💥 Better collision animations
- [ ] 📳 Collision screen shake
- [ ] 🕳️ Interactive potholes
- [ ] 🏆 Level scoring
- [ ] ⭐ Performance ratings
- [ ] 💾 Save system
- [ ] ⏸️ Pause menu
- [ ] 🎮 Controller support
- [ ] 🧍 Additional characters
- [ ] 🗺️ More locations
- [ ] 🎯 More minigames
- [ ] 🔀 Additional branching decisions
- [ ] 🏁 Final destination / ending sequence

And, of course...

### More chaos in Kirby's day.

---

# 💡 Development Philosophy

KirbyLand started with a relatively simple idea:

> **Build a game around a day in Kirby's life.**

As development continued, the project evolved into an experiment in combining several different gameplay systems inside one C++ game.

The codebase is structured around reusable systems rather than putting every mechanic into one file.

Each new scene can introduce a completely different challenge while still sharing the same:

- Game loop
- Rendering system
- Scene architecture
- Text system
- Entity system
- Transition system

That makes KirbyLand both a game and an ongoing exploration of **C++, SDL2, game architecture, procedural rendering, collision systems, state management, and interactive design.**

---

# 🤝 Contributing

KirbyLand is currently a personal game project, but suggestions and ideas are always welcome.

If contributing to the repository:

1. Create a new branch.
2. Make your changes.
3. Test the affected scenes.
4. Make sure the project builds successfully.
5. Submit a pull request with a description of the changes.

Example:

```bash
git checkout -b feature/new-level
```

Then:

```bash
git add .
git commit -m "Add new KirbyLand level"
git push
```

---

# 🐛 Found a Bug?

Kirby has enough problems already.

If you discover a bug, please include:

- The scene where it occurred
- What you were doing
- What you expected to happen
- What actually happened
- Any terminal/compiler output
- Steps to reproduce it

---

# 📸 Screenshots / Gameplay

> Screenshots and gameplay footage coming soon.

Suggested future README layout:

```text
┌──────────────────┐  ┌──────────────────┐
│                  │  │                  │
│    BEDROOM       │  │    CAR START     │
│    SCREENSHOT    │  │    SCREENSHOT    │
│                  │  │                  │
└──────────────────┘  └──────────────────┘

┌──────────────────┐  ┌──────────────────┐
│                  │  │                  │
│      E470        │  │    GUN CLUB      │
│    SCREENSHOT    │  │    SCREENSHOT    │
│                  │  │                  │
└──────────────────┘  └──────────────────┘
```

---

# 📜 License

This project is currently intended as a personal/educational game project.

If the repository becomes publicly distributed or open source, a formal license can be added here.

---

<div align="center">

# 🎮 KirbyLand

### *Can you survive a day as a Kirby?*

🛏️ **Wake Up** • 🔎 **Find Your Stuff** • 🚗 **Start the Car** • 🛣️ **Survive the Drive**

**Built with C++ & SDL2**

🚧 **Work in Progress** 🚧

</div>
