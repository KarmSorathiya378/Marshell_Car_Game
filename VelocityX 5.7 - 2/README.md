# VELOCITY X — Unreal Engine 5.3+ Project Setup

Welcome to the **VELOCITY X** project repository. This project is configured as a native C++ project for Unreal Engine 5.3+.

---

## 📁 Directory Structure

```
VelocityX/
├── VelocityX.uproject               # Unreal Engine project descriptor
├── README.md                        # Project & build documentation
├── Source/
│   ├── VelocityX.Target.cs          # Game target build rules
│   ├── VelocityXEditor.Target.cs    # Editor target build rules
│   └── VelocityX/
│       ├── VelocityX.Build.cs       # Module build rules (Core, CoreUObject, Engine, InputCore, UnrealEd, Slate, SlateCore, UMG)
│       ├── VelocityX.h              # Module header
│       ├── VelocityX.cpp            # Primary game module implementation
│       └── Private/
│           ├── VelocityXGameModeBase.h   # Starter GameMode header
│           ├── VelocityXGameModeBase.cpp # Starter GameMode implementation
│           ├── VelocityXCharacter.h      # Starter third-person character header
│           └── VelocityXCharacter.cpp    # Starter third-person character implementation
├── Config/
│   ├── DefaultEngine.ini            # Default engine settings & GameMode selection
│   ├── DefaultGame.ini              # Project metadata & title
│   └── DefaultInput.ini             # Axis and action input bindings
└── Content/
    ├── Maps/                        # Level maps and tracks
    ├── Blueprints/                  # Gameplay and character blueprints
    ├── Vehicle/                     # Vehicle meshes, physics assets & tuning
    └── UI/                          # HUD and UMG menu assets
```

---

## 🛠️ Prerequisites

1. **Unreal Engine 5.3 or newer** (Installed via Epic Games Launcher or built from source).
2. **Visual Studio 2022** (v17.4 or higher) with the following workloads:
   - **Game development with C++**
   - **MSVC v143 - VS 2022 C++ x64/x86 build tools**
   - **Windows 10/11 SDK (10.0.18362.0 or higher)**
   - **Unreal Engine installer integration** (optional, recommended)
3. **.NET 6.0 / .NET 8.0 SDK** (required by UnrealBuildTool in UE 5.3+).

---

## 🚀 How to Build and Import into Unreal Editor

### Option 1: Right-Click Generate Project Files (Recommended)

1. Open File Explorer and navigate to:
   ```
   D:\karm\car_game\VelocityX
   ```
2. Right-click on **`VelocityX.uproject`**.
3. Select **"Generate Visual Studio project files"**.
   - If prompted for Unreal Engine version, select **Unreal Engine 5.3**.
4. Open the generated **`VelocityX.sln`** file in Visual Studio 2022.
5. In Visual Studio:
   - Set Solution Configuration to **`Development Editor`**.
   - Set Solution Platform to **`Win64`**.
   - Right-click the **`VelocityX`** project in Solution Explorer and click **Build** (or press `Ctrl + Shift + B`).
6. Once the build completes successfully, double-click **`VelocityX.uproject`** to launch Unreal Editor.

---

### Option 2: Direct Unreal Editor Import

1. Launch **Unreal Engine 5.3+** from the Epic Games Launcher.
2. In the Unreal Project Browser, click **"Browse"** at the bottom right.
3. Browse to `D:\karm\car_game\VelocityX` and select **`VelocityX.uproject`**.
4. If Unreal Editor displays a message:
   > *"The following modules are missing or built with a different engine version: VelocityX. Would you like to rebuild them now?"*
5. Click **"Yes"**.
6. The editor will compile the C++ source files and open the project automatically.

---

### Option 3: Command Line Build via UnrealBuildTool (UBT)

Replace `<UE5_PATH>` with your Unreal Engine installation path (e.g., `C:\Program Files\Epic Games\UE_5.3`):

```powershell
# 1. Generate project files
& "<UE5_PATH>\Engine\Build\BatchFiles\GenerateProjectFiles.bat" -project="D:\karm\car_game\VelocityX\VelocityX.uproject" -game -engine

# 2. Build Development Editor
& "<UE5_PATH>\Engine\Build\BatchFiles\Build.bat" VelocityXEditor Win64 Development -Project="D:\karm\car_game\VelocityX\VelocityX.uproject" -WaitMutex -FromMsBuild

# 3. Launch Editor
& "<UE5_PATH>\Engine\Binaries\Win64\UnrealEditor.exe" "D:\karm\car_game\VelocityX\VelocityX.uproject"
```

---

## 🎮 Default Controls

The input mapping in `Config/DefaultInput.ini` includes:

| Action / Axis | Keyboard / Mouse | Gamepad |
| :--- | :--- | :--- |
| **MoveForward / Throttle** | `W` / `S` | Left Stick Y / Right Trigger |
| **MoveRight / Steering** | `A` / `D` | Left Stick X |
| **Jump / Handbrake** | `SpaceBar` | Face Button Bottom (`A` / `Cross`) |
| **Look / Turn** | Mouse `X` / `Y` | Right Stick `X` / `Y` |
| **Camera Switch** | `C` | Right Thumbstick Click |
| **Reset Vehicle** | `R` | Special Left (`Back` / `View`) |
| **Headlights** | `L` | D-Pad Up |
| **Pause** | `Escape` / `P` | Special Right (`Start` / `Menu`) |
