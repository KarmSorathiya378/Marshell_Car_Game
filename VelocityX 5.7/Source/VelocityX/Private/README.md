# VELOCITY X - Game Systems & Input Engineering Guide

This directory contains the core game systems, player controller, game mode, game state, and input configuration for **VELOCITY X**.

---

## 1. File Structure

```
VelocityX/Source/VelocityX/Private/
├── AVelocityXPlayerController.h / .cpp    # Player controller forwarders
├── VelocityXPlayerController.h / .cpp     # Master player controller (WASD, C, Space, L, R, Esc, Mouse Look)
├── AVelocityXGameModeBase.h / .cpp        # Game mode base forwarders
├── VelocityXGameModeBase.h / .cpp         # Game mode (State machine, car selection, pause system, session timer)
├── AVelocityXGameState.h / .cpp           # Game state forwarders
├── VelocityXGameState.h / .cpp            # Game state (Track timing, lap counters, persistent high scores)
├── DefaultInput.ini                       # Unreal Engine input bindings (WASD, Arrows, Space, C, L, R, Esc, Mouse)
├── VelocityXCharacter.h / .cpp            # Default character implementation
└── README.md                              # Systems engineering documentation
```

---

## 2. Systems Overview

### 1. `AVelocityXPlayerController`
* **Driving Input**:
  - `Throttle`: `W` / `Up Arrow` / `Gamepad Right Trigger` (Range `[0.0, 1.0]`)
  - `Brake / Reverse`: `S` / `Down Arrow` / `Gamepad Left Trigger` (Range `[0.0, 1.0]`)
  - `Steering`: `A` / `D` / `Left Arrow` / `Right Arrow` / `Gamepad Left Stick X` (Range `[-1.0, 1.0]`)
* **Action Bindings**:
  - `Handbrake`: `SpaceBar` / `Gamepad Face Button Bottom`
  - `Camera Toggle`: `C` / `Gamepad Right Stick Click` $\rightarrow$ Cycles `Chase` $\rightarrow$ `Hood` $\rightarrow$ `Cockpit` $\rightarrow$ `Chase`
  - `Headlights`: `L` / `Gamepad D-Pad Up` $\rightarrow$ Toggles headlight beams
  - `Reset Vehicle`: `R` / `Gamepad Special Left` $\rightarrow$ Resets vehicle upright, leveling roll/pitch (+35cm Z) and zeroing angular velocities
  - `Pause Menu`: `Escape` / `P` / `Gamepad Special Right` $\rightarrow$ Toggles pause menu, switches input mode between `GameOnly` and `GameAndUI`, toggles cursor
* **Cockpit Free Mouse Look**:
  - `Turn` (`MouseX`) and `LookUp` (`MouseY`) allow looking around inside the car when in Cockpit view (`EVelocityXCameraMode::Cockpit`).
  - Automatically smoothly re-centers when switching back to Chase or Hood views.
* **Pre-Race Vehicle Selection**:
  - `RequestCarSelection(EVelocityXCarType CarType)`: Selects Supercar, TrackSpec, or Hypercar.
  - `ConfirmCarSelectionAndStartRace()`: Spawns vehicle, possesses it, transitions state to `Racing`.
* **Universal Vehicle Routing**:
  - Interfaces natively with `ABaseCar`, `AVELOCITYXVehicle`, and generic pawns.

### 2. `AVelocityXGameModeBase`
* **Match State Management**:
  - State Enum: `CarSelection`, `Countdown`, `Racing`, `Paused`, `Finished`.
  - Dispatches `OnMatchStateChanged(EVelocityXMatchState NewState)`.
* **Car Selection**:
  - `SelectCar(EVelocityXCarType InCarType)`: Stores selected vehicle profile.
  - `SpawnAndPossessSelectedVehicle(APlayerController* PC)`: Spawns the chosen car class at PlayerStart and possesses it.
* **Pause / Unpause Logic**:
  - `TogglePauseGame()`, `PauseGame()`, `UnpauseGame()`.
  - Pauses world tick via `UGameplayStatics::SetGamePaused()`, manages input modes, sets mouse cursor visibility.
* **Session Racing Timer**:
  - Synchronized directly with `AVelocityXGameState`.
  - `StartRace()`, `EndRace()`, `ResetSessionTimer()`.
  - Formatted string representation (`MM:SS.mmm`).

### 3. `AVelocityXGameState`
* **Track Time**:
  - High-precision delta time accumulator (`CurrentTrackTime`, `CurrentLapTime`).
  - Best lap tracking and split time comparisons.
* **Lap Counter**:
  - `CurrentLap`, `TotalLaps`.
  - `CompleteLap(float LapTime)` triggers `OnLapCompleted` and checks for race finish.
* **High Score Storage**:
  - Stores `FVelocityXHighScore` records (PlayerName, RaceTime, BestLapTime, TopSpeedKPH, CarName, Timestamp).
  - Automatically sorts ascending by race time.
  - Top 10 persistent leaderboard slots.

---

## 3. Input Bindings Table (`DefaultInput.ini`)

| Action / Axis Name | Primary Key | Alternate Key | Gamepad / Mouse | Function / Description |
| :--- | :--- | :--- | :--- | :--- |
| **Throttle** | `W` | `Up Arrow` | `Gamepad_RightTriggerAxis` | Vehicle forward acceleration |
| **Brake** | `S` | `Down Arrow` | `Gamepad_LeftTriggerAxis` | Vehicle braking and reverse |
| **Steering** | `A` (-1.0), `D` (+1.0) | `Left` (-1.0), `Right` (+1.0) | `Gamepad_LeftX` | Vehicle steering |
| **Handbrake** | `SpaceBar` | — | `Gamepad_FaceButton_Bottom` | Rear-wheel handbrake lock |
| **Camera** | `C` | — | `Gamepad_RightThumbstick` | Cycle Chase / Hood / Cockpit |
| **Headlights** | `L` | — | `Gamepad_DPad_Up` | Toggle front headlight beams |
| **Reset** | `R` | — | `Gamepad_Special_Left` | Reset vehicle upright |
| **Pause** | `Escape` | `P` | `Gamepad_Special_Right` | Toggle pause menu |
| **Turn** | `MouseX` | — | — | Cockpit interior horizontal look |
| **LookUp** | `MouseY` | — | — | Cockpit interior vertical look |
