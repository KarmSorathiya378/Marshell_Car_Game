# VELOCITY X — Input & Camera System Documentation

## Overview
This document specifies the complete Input and Camera Architecture for **VELOCITY X**, implemented for Unreal Engine 5 using the **Enhanced Input System** and integrated with **Zelda's `ABaseCar` / Chaos vehicle systems**, **UI state management**, and **vehicle telemetry**.

---

## 1. Class Architecture & Type Names

| Class / Type Name | Purpose | Location |
|---|---|---|
| `AVelocityPlayerController` / `AVELOCITYXPlayerController` | Main Player Controller handling Enhanced Input routing, context switching, vibration haptics, and reset logic | [`Source/Input/Public/VelocityPlayerController.h`](file:///D:/karm/car_game/Source/Input/Public/VelocityPlayerController.h) |
| `AVelocityCameraManager` / `AVELOCITYXCameraManager` | 4-Mode Dynamic Camera Manager with mesh socket tracking, speed FOV widening, and shake triggers | [`Source/Input/Public/VelocityCameraManager.h`](file:///D:/karm/car_game/Source/Input/Public/VelocityCameraManager.h) |
| `UVELOCITYXInputComponent` | Custom Enhanced Input Component with checked action binding templates | [`Source/Input/Public/VELOCITYXInputComponent.h`](file:///D:/karm/car_game/Source/Input/Public/VELOCITYXInputComponent.h) |
| `UVelocityInputConfig` | Data asset storing all Enhanced Input actions, mapping contexts, and priority settings | [`Source/Input/Public/VelocityInputConfig.h`](file:///D:/karm/car_game/Source/Input/Public/VelocityInputConfig.h) |
| `EVelocityCameraMode` / `ECameraMode` | Camera mode enum: `ChaseClose`, `ChaseFar`, `Hood`, `Cockpit`, `Top` | [`Source/Input/Public/VelocityInputConfig.h`](file:///D:/karm/car_game/Source/Input/Public/VelocityInputConfig.h) |
| `BP_CameraManager` | Blueprint Camera Manager asset definition | [`Config/Input/BP_CameraManager.COPY`](file:///D:/karm/car_game/Config/Input/BP_CameraManager.COPY) |
| `BP_VelocityPlayerController` | Blueprint Player Controller asset definition | [`Config/Input/BP_VelocityPlayerController.COPY`](file:///D:/karm/car_game/Config/Input/BP_VelocityPlayerController.COPY) |

---

## 2. Controls & Vehicle Input Routing

Directly routes analog and digital inputs to **Zelda's `ABaseCar`** (`ApplyThrottle`, `ApplyBrake`, `ApplySteering`, `ApplyHandbrake`, `ResetVehicle`, `ToggleHeadlights`):

| Action | Function | Keyboard | Gamepad | Input Type |
|---|---|---|---|---|
| **Throttle** | Accelerate car forward | `W` / `Up` | `Right Trigger (RT / R2)` | Analog `[0.0, 1.0]` |
| **Brake / Reverse** | Brake / Reverse gear | `S` / `Down` | `Left Trigger (LT / L2)` | Analog `[0.0, 1.0]` |
| **Steering** | Steer Left / Right | `A` / `D` or `Left` / `Right` | `Left Stick X` | Analog `[-1.0, 1.0]` (Curved) |
| **Handbrake** | Drift & emergency lock | `SpaceBar` | `A / Cross` or `B / Circle` | Digital (Boolean) |
| **Shift Up** | Manual transmission gear up | `E` | `Right Shoulder (RB / R1)` | Digital |
| **Shift Down** | Manual transmission gear down | `Q` | `Left Shoulder (LB / L1)` | Digital |
| **Cycle Camera** | ChaseClose → Hood → Cockpit → ChaseFar | `C` | `D-Pad Down` / `D-Pad Up` | Digital |
| **Quick Camera** | Quick select Chase / Cockpit | — | `D-Pad Right` (Chase) / `D-Pad Left` (Cockpit) | Digital |
| **Toggle Headlights** | Turn headlights On/Off | `L` | `X / Square` | Digital |
| **Reset Car** | Upright car if crashed or flipped | `R` | `Y / Triangle` | Digital |
| **Pause Menu** | Toggle Pause UI | `Escape` | `Start / Options / Menu` | Digital (`bExecuteWhenPaused = true`) |
| **Steering Assist** | Mouse steering assist | `Mouse X` | — | Axis1D with auto-centering |

---

## 3. Input Context Management (IMC Switching API)

Three distinct contexts prevent UI navigation and vehicle controls from overlapping:

1. `SetInputModeDriving()`:
   - Removes `IMC_Menu` and `IMC_Pause`.
   - Adds `IMC_Driving` (Priority `0`).
   - Sets `FInputModeGameOnly()`, hides mouse cursor.
2. `SetInputModeUI()`:
   - Removes `IMC_Driving`.
   - Adds `IMC_Menu` (Priority `10`).
   - Sets `FInputModeGameAndUI()`, shows mouse cursor, stops haptics.
3. `SetInputModePause()`:
   - Removes `IMC_Driving`.
   - Adds `IMC_Pause` (Priority `20`).
   - Sets `FInputModeGameAndUI()`, shows cursor, pauses game.
   - Broadcasts `OnPauseToggled(bool bIsPaused)` and notifies `AVELOCITYXGameMode::SetGameState(EGameState::Paused)`.

---

## 4. 4-Mode Dynamic Camera Manager

### 4.1 Modes & Mesh Sockets
The camera manager automatically checks whether target vehicle meshes feature standard sockets before falling back to calibrated offset presets:

1. **ChaseClose Camera** (`EVelocityCameraMode::ChaseClose`):
   - Socket: `"Socket_ChaseCamera"`
   - Fallback Offset: `(-490.0, 0.0, 150.0)`, LookAt: `(80.0, 0.0, 45.0)`.
   - Smooth lag damping (`11.0` pos, `9.0` rot).
2. **ChaseFar Camera** (`EVelocityCameraMode::ChaseFar`):
   - Socket: `"Socket_ChaseFar"`
   - Fallback Offset: `(-720.0, 0.0, 260.0)`, LookAt: `(120.0, 0.0, 60.0)`.
   - Wide-angle tracking with speed pullback up to `180` units.
3. **Hood Camera** (`EVelocityCameraMode::Hood`):
   - Socket: `"Socket_HoodCamera"`
   - Fallback Offset: `(135.0, 0.0, 75.0)`, LookAt: `(600.0, 0.0, 75.0)`.
   - Responsive forward driver view.
4. **Cockpit Camera** (`EVelocityCameraMode::Cockpit`):
   - Socket: `"Socket_CockpitCamera"`
   - Fallback Offset: `(-12.0, -34.0, 96.0)` (interior cabin driver seat).
   - Dynamic G-force inertia tilt (roll into turns, pitch on acceleration/braking).

### 4.2 Speed-Based FOV Scaling Hook
- Hook: `GetDynamicFOV() const` and `SetSpeedFOVRange(MinFOV, MaxFOV, MaxSpeedMPH)`.
- Smoothly widens FOV from `90.0°` at 0 MPH up to `118.0°` at 250+ MPH.
- Coupled with dynamic post-process motion blur scaling (`0.15` to `0.85`).

### 4.3 Camera Shake Triggers
- `TriggerCollisionShake(float ImpactMagnitude)`: Impact impulse on obstacle collisions.
- `TriggerHighSpeedWindShake(float WindIntensity)`: Procedural wind buffeting at extreme velocities.
- `TriggerEngineRedlineShake(float RPMPercent)`: Engine vibrations when approaching redline.

---

## 5. Gamepad Haptics & Vibration Hooks

Exposed on `AVelocityPlayerController`:
- `PlayTireSlipHaptics(float SlipRatio)`: Light high-frequency rumble indicating traction loss / drift.
- `PlayRedlineHaptics(float RPMNormalized)`: Engine redline shudder when RPM > 92%.
- `PlayHardBrakingABSHaptics(float BrakeIntensity)`: 40 Hz rapid pulsing simulating ABS activation during hard stops.
- `PlayCollisionHaptics(float ImpactForce)`: Heavy dual-motor impact shock on crashes.
- `PlayHandbrakeHaptics(float DeltaTime)`: Sustained vibration during active handbrake slides.

---

## 6. Settings & Config Integration

Configurable properties wired directly to `UVELOCITYXGameInstance` or Settings UI:
- `SteeringSensitivity` (Default `1.0`)
- `SteeringDeadzone` (Default `0.05`)
- `SteeringLinearity` (Default `1.25`, gamma exponential curve: `Sign(X) * Pow(Abs(Norm), Linearity)`)
- `TriggerDeadzone` (Default `0.02`)
- `ThrottleSensitivity` (Default `1.0`)
- `BrakeSensitivity` (Default `1.0`)
- `ApplyControlSettings(float InSteerSens, float InSteerDeadzone, float InSteerLinearity, float InTriggerDeadzone)`
