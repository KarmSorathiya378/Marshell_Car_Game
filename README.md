# Marshell Car Game: VelocityX

> **Developed and Produced by Marshell**  
> A high-octane native **Unreal Engine** arcade racing and time-attack game, featuring dual-mode playability (native Unreal Engine 5.3+ simulation & lightweight instant WebGL browser edition).

---

## Table of Contents
1. [About the Product: What Marshell Can Do](#about-the-product-what-marshell-can-do)
2. [Ways to Play](#ways-to-play)
   - [Method 1: Instant Browser Play](#method-1-instant-browser-play-no-unreal-engine-required)
   - [Method 2: Full Unreal Engine Experience](#method-2-full-unreal-engine-experience-high-fidelity)
3. [Unreal Engine Setup Guide](#unreal-engine-setup-guide)
   - [Prerequisites and Supported Versions](#prerequisites-and-supported-versions)
   - [Step-by-Step Installation and Launch](#step-by-step-installation-and-launch)
4. [Controls and Gameplay](#controls-and-gameplay)
   - [Vehicle Selection](#vehicle-selection)
   - [Keyboard and Gamepad Layout](#keyboard-and-gamepad-layout)
   - [Race Mechanics and Checkpoints](#race-mechanics-and-checkpoints)
5. [Code Architecture and Project Structure](#code-architecture-and-project-structure)
6. [Developer and Automation Tooling](#developer-and-automation-tooling)
7. [Credits and Author](#credits-and-author)

---

## About the Product: What Marshell Can Do

**Marshell** has designed and engineered this product as an end-to-end, high-performance racing experience. Here is everything this product delivers:

- **Native C++ Arcade Vehicle Physics**:
  Fixed-step 120 Hz driving dynamics calculation providing immediate responsiveness, smooth drift and handbrake friction, progressive acceleration, responsive reverse braking, and rechargeable nitrous boost.
- **Multiple Authorable Vehicle Classes**:
  Three pre-tuned race vehicle archetypes (Apex GT, Veloce R, and Titan X) offering distinct weight distribution, grip characteristics, and top speeds.
- **Dynamic Track Generation (Sunridge Circuit)**:
  Procedurally constructed oval circuit featuring road ribbons, barrier colliders, scenery props, and an ordered 16-checkpoint validation system to prevent cutting or reverse cheating.
- **Persistent Lap Tracking & Records**:
  Real-time session HUD, split times, and automated local disk serialization (`Saved/SaveGames/VelocityX_Record.sav`) for personal best lap records.
- **Dynamic Multi-Angle Camera System**:
  Real-time switching between Near Chase, Far Chase, Hood Camera, and Cockpit perspective.
- **Dual Engine Architecture**:
  Engineered with a modular architecture that enables both native AAA execution in Unreal Engine and instant lightweight WebGL simulation in any browser without installing game engines.
- **Automated Asset Bootstrapping & Testing**:
  Includes headless asset construction pipelines (`create_drive_assets.py`), procedural 3D model creators (`create_car_mesh.py`), and automated validation test suites (`FDriveRaceTest`, `FDriveDynamicsTest`).

---

## Ways to Play

The web edition now ports the **Sunridge circuit and racing rules** from the Unreal game: portable C++ driving runs in **WebAssembly**, and a browser renderer draws the original coupe and scenery assets. Players run independent races on their own devices. See [WEB_BUILD.md](WEB_BUILD.md) for setup, rebuilding, deployment, and differences from the Unreal renderer.

After analyzing the codebase, there are **two distinct ways** to play this game:

| Feature | Method 1: Web Edition (`VelocityX_Web`) | Method 2: Unreal Engine (`VelocityX`) |
| :--- | :--- | :--- |
| **Requirements** | Any modern web browser (Chrome, Edge, Firefox, Brave) | Unreal Engine 5.3+ installed |
| **Install Time** | 0 seconds (instant play) | Requires UE installation & build |
| **Physics / Graphics** | C++ WebAssembly physics + Three.js WebGL | Unreal Engine DX11/DX12 |
| **Best For** | Browser time attack with keyboard, gamepad or touch | Native Unreal experience |

---

## Method 1: Instant Browser Play (No Unreal Engine Required)

If you do not have Unreal Engine installed or want to test and play immediately:

1. From the repository root, run `npm run dev` with Node.js 24 installed.
2. Open `http://127.0.0.1:8000` (or visit the deployed website). WASM and asset loading require HTTP; do not double-click the HTML file.
3. Select a car and click **Start your engine**, or press **Enter**.
4. Drive with WASD/arrows, handbrake with Space, boost with Shift, and change cameras with C. Phones have touch controls. Pass all sixteen checkpoints in order for three laps.

### Deploy the browser edition to Vercel

Use the repository root as Vercel's **Root Directory** and **Other** as the framework preset. The root `vercel.json` runs `npm run check` before publishing only `VelocityX_Web`. Compiled WASM and assets are committed; Vercel needs no compiler or dependency installation. Commit and push the web files, C++ source, build script, tests, package files, and `vercel.json`, then redeploy.

If your Vercel project currently uses `VelocityX_Web` as its Root Directory, change it to the repository root (leave the Root Directory field empty) so Vercel can read `vercel.json`, `package.json`, and the tests. Remove any old dashboard overrides that skip the build step. The configured output remains `VelocityX_Web`.

Wait for **Ready**, select a car, and start the race. If WASM, meshes, or WebGL fail to initialize, the menu displays an error. Each browser runs the game locally; your PC does not need to remain on and no GPU streaming server is required. Rendering and barrier collision differ from Unreal; details are in [WEB_BUILD.md](WEB_BUILD.md).

### Automatic checks before publishing

Every push and pull request runs the **Game checks / Web game checks** GitHub Actions workflow. View its result in the repository's **Actions** tab or the pull request checks. To run the same checks locally, install Node.js 24 and run `npm run check` from the repository root.

The core checks validate WASM provenance, JavaScript syntax, mesh integrity, native dynamics and race-order regressions, timing, recovery, and complete three-lap races for every preset. GitHub Actions also rebuilds WASM with Emscripten 6.0.12 and runs actual desktop/mobile Chromium browser tests. Run those locally with `npm ci`, `npx playwright install chromium`, then `npm run test:browser`. The checks cover the browser port, not the Unreal runtime.

For each Git deployment, Vercel independently runs `npm run check` as its build step. A nonzero exit stops the deployment, keeping the previous production deployment live. GitHub Actions and Vercel run in parallel; Vercel gates publication on its core checks, not on the Actions/browser-test status. No Vercel token or GitHub deployment secret is required. Keep the GitHub repository connected to Vercel with `main` as its production branch.

Optional: add a GitHub branch ruleset for `main` requiring pull requests and the **Web game checks** status before merging. This also prevents unchecked changes from being merged; the Vercel build check protects deployments independently.

---

## Method 2: Full Unreal Engine Experience (High-Fidelity)

For the complete native experience created by Marshell with full vehicle physics, gamepad support, generated engine audio, and high-resolution rendering:

---

## Unreal Engine Setup Guide

### Prerequisites and Supported Versions

- **Unreal Engine Version**: **Unreal Engine 5.3 or newer** (compatible with UE 5.3, 5.4, 5.5, and 5.7+; default configuration targets UE 5.7).
- **Operating System**: Windows 10 / 11 (64-bit).
- **Compiler / Build Tools**: Visual Studio 2022 (Community or higher) with:
  - *Desktop development with C++*
  - *MSVC v143 - VS 2022 C++ x64/x86 build tools*
  - *Windows 10/11 SDK*
  - *.NET 8.0 SDK*

---

### Step-by-Step Installation and Launch

#### Step 1: Download and Install Unreal Engine
1. Download the **Epic Games Launcher** from [unrealengine.com](https://www.unrealengine.com/).
2. Open the Epic Games Launcher and select the **Unreal Engine** tab on the left.
3. Click **Library**, click the yellow **+** icon beside Engine Versions, and choose **Unreal Engine 5.3** or any newer version (such as 5.4, 5.5, or 5.7).
4. Click **Install** and wait for the download to complete.

#### Step 2: Clone or Download this Repository
```bash
git clone https://github.com/KarmSorathiya378/Marshell_Car_Game.git
cd Marshell_Car_Game
```

#### Step 3: Launch the Game (Choose One Option)

##### Option A: One-Click Launcher (Fastest)
Double-click the included batch launcher in the root folder:
- [Play Velocity X.cmd](Play%20Velocity%20X.cmd): Automatically compiles any changes and starts the game windowed at 1600x900.
> *Note:* If your Unreal Engine is installed in a custom location other than default `E:\UE_5.7`, run via PowerShell specifying your engine path:
> ```powershell
> powershell -ExecutionPolicy Bypass -File "VelocityX\Scripts\Play.ps1" -EngineRoot "C:\Program Files\Epic Games\UE_5.4"
> ```

##### Option B: Open via Unreal Editor
1. Double-click [Open Velocity X Editor.cmd](Open%20Velocity%20X%20Editor.cmd) (or pass `-Editor` to `VelocityX/Scripts/Play.ps1`).
2. Alternatively, right-click [VelocityX/VelocityX.uproject](VelocityX/VelocityX.uproject) and select **Switch Unreal Engine version...** to select your installed engine version (5.3+), then double-click the `.uproject` file.
3. When prompted to build missing modules, click **Yes**.
4. In the Content Browser, open `Content/Maps/Sunridge`.
   > **Note on Viewport**: Sunridge is a runtime-generated track; the viewport before pressing play is empty by design. Click the green **Play (PIE)** button in the editor toolbar to generate and drive the track.

##### Option C: Build from Visual Studio 2022
1. Right-click [VelocityX/VelocityX.uproject](VelocityX/VelocityX.uproject) and select **Generate Visual Studio project files**.
2. Open the generated `VelocityX.sln` in Visual Studio 2022.
3. Set the build configuration to **Development Editor** and platform to **Win64**.
4. Press `F5` to compile and launch in debug mode.

---

## Controls and Gameplay

### Vehicle Selection
On the start screen before beginning the race, press **1**, **2**, or **3** to switch car tuning presets:
- **1 — Apex GT**: Balanced acceleration, grip, and top speed.
- **2 — Veloce R**: Lightweight chassis with aggressive turning and high boost sensitivity.
- **3 — Titan X**: High-torque cruiser with maximum stability and heavy road adhesion.

### Keyboard and Gamepad Layout

| Action | Keyboard | Gamepad |
| :--- | :--- | :--- |
| **Accelerate** | `W` or `Up Arrow` | Right Trigger (`RT`) |
| **Brake / Reverse** | `S` or `Down Arrow` | Left Trigger (`LT`) |
| **Steer Left / Right** | `A` / `D` or Arrows | Left Thumbstick |
| **Handbrake Drift** | `Space` | Right Face Button (`B` / `Circle`) |
| **Nitrous Boost** | `Left Shift` | Left Face Button (`X` / `Square`) |
| **Cycle Camera** | `C` | Right Stick Click (`R3`) |
| **Recover to Track** | `R` | View / Select Button |
| **Start Race** | `Enter` | Bottom Face Button (`A` / `Cross`) |
| **Pause / Resume** | `Esc` or `P` | Menu / Start Button |
| **Restart Session** | `F5` | — |
| **Cycle Graphics Quality** | `F1` | — |
| **Quit Game** | `Alt + F4` | — |

### Race Mechanics and Checkpoints
- Complete **3 laps** around the Sunridge circuit.
- The circuit features **16 sequential checkpoints**. You must drive forward through each checkpoint in order; passing backward or cutting across the track will not register.
- The HUD minimap and distance indicators guide you to the active checkpoint.
- Your best lap time is automatically stored and compared across sessions.

---

## Code Architecture and Project Structure

```
Marshell_Car_Game/
├── VelocityX/                     # Active Unreal Engine project (UE 5.3+)
│   ├── VelocityX.uproject        # Unreal Engine Project descriptor
│   ├── Config/                   # Engine, Game, and Input mappings
│   ├── Content/                  # Maps (Sunridge), Meshes, Materials
│   ├── Source/                   # Native C++ source code
│   │   └── VelocityX/Private/Drive/
│   │       ├── DriveCar.cpp      # Vehicle arcade dynamics & input routing
│   │       ├── DriveCircuit.cpp  # Procedural circuit & checkpoint generator
│   │       ├── DriveHUD.cpp      # Speedometer, minimap, lap counter
│   │       └── DriveGame.h       # Race state, rules, checkpoint logic
│   ├── ArtSource/                # 3D models (Coupe, Glass, Track FBX/Blender)
│   └── Scripts/                  # Automated build and test scripts
├── VelocityX_Web/                 # Web Edition (Three.js WebGL game)
│   └── index.html                # Standalone playable browser version
├── Play Velocity X.cmd            # One-click native game launcher
├── Open Velocity X Editor.cmd     # One-click Unreal Editor launcher
├── PROJECT_ANALYSIS.md            # Technical analysis & architecture documentation
└── README.md                      # Project documentation
```

---

## Developer and Automation Tooling

- **Automated Test Suite**:
  Run unit and gameplay validation tests via PowerShell:
  ```powershell
  powershell -ExecutionPolicy Bypass -File "VelocityX\Scripts\Test.ps1"
  ```
- **Asset Generation**:
  Re-generate circuit materials and assets from script:
  ```powershell
  python "VelocityX\Scripts\create_drive_assets.py"
  ```
- **Vehicle Mesh Generation**:
  Rebuild procedural car models using Blender:
  ```powershell
  python "VelocityX\Scripts\create_car_mesh.py"
  ```

---

## Credits and Author

- **Producer & Creator**: **Marshell**
- **Repository**: [Marshell_Car_Game](https://github.com/KarmSorathiya378/Marshell_Car_Game.git)
