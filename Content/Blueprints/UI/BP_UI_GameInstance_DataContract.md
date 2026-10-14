# VELOCITY X - UI & GameInstance Integration Data Contract

## 1. Architecture Overview
This document specifies the bidirectional data contract, state lifecycle, and communication flow between **Unreal Motion Graphics (UMG)** UI widgets and the core C++ systems:
- `UVELOCITYXGameInstance`: Global persistent state, save/load system, audio/graphics scalability settings, career statistics.
- `AVELOCITYXGameMode`: State transitions, car selection validation, race lap stopwatch, and multicast delegates (`OnGameStateChanged`, `OnCarSelected`).
- `AVelocityPlayerController` / `AVELOCITYXPlayerController`: Player inputs, camera view mode, pause action (`ActionTogglePause`), reset car (`ActionResetCar`), controlled car access (`GetControlledCar()`).
- `ABaseCar`, `ATrackSupercar`, `AHypercar`: Real-time physics telemetry (`GetSpeedMPH()`, `GetSpeedKPH()`, `GetEngineRPM()`, `GetCurrentGear()`, `GetPerformanceStats()`, hybrid boost, and active DRS wing aero).

---

## 2. Widget-by-Widget Data Flow Matrix

| Widget Blueprint | Read From GameInstance | Written To GameInstance | Read From GameMode / Vehicle / Controller |
| :--- | :--- | :--- | :--- |
| **`BP_MainMenu`** | `CompletedRaces`, `TotalPlayTime`, `BestLapTimes[0..2]`, `MasterVolume` | None (reads for header career badge) | GameMode: binds to `OnGameStateChanged`<br>UVELOCITYXMainMenuWidget: binds to `OnPlayButtonClickedDelegate`, `OnSettingsButtonClickedDelegate`, `OnQuitButtonClickedDelegate` |
| **`BP_CarSelection`** | `BestLapTimes[0]` (Supercar), `BestLapTimes[1]` (TrackSupercar), `BestLapTimes[2]` (Hypercar) | None directly (delegates car choice to GameMode) | GameMode: `SelectCar(ECarType)`, `GetSelectedCarType()`, binds to `OnCarSelected`, `OnGameStateChanged`<br>Enums: `Supercar`, `TrackSupercar`, `Hypercar` |
| **`BP_VelocityHUD`** | `BestLapTimes[SelectedCarIndex]` | None (real-time read-only display) | Vehicle (`ABaseCar`): `GetSpeedKPH()`, `GetSpeedMPH()`, `GetEngineRPM()`, `GetCurrentGear()`, `GetPerformanceStats()`, `AreHeadlightsOn()`<br>Hypercar: `GetBoostChargePercent()`, `IsHybridBoostActive()`<br>TrackSupercar: `IsDRSActive()`, `IsRaceModeActive()`<br>GameMode: `GetElapsedTime()`<br>Controller: `GetCameraIndex()`, `ThrottleInput`, `BrakeInput` |
| **`BP_PauseMenu`** | None directly | None directly | GameMode: `GetElapsedTime()`, `GetSelectedCarType()`, calls `ResetTimer()`, calls `SetGameState(MainMenu)`<br>Controller: calls `ActionTogglePause()`, `ActionResetCar()` |
| **`BP_Settings`** | `GraphicsQuality` (`EGraphicsQuality`), `ShadowResolution`, `TextureQuality`, `bMotionBlur`, `bAntiAliasing`, `FieldOfView`, `MasterVolume`, `SFXVolume`, `MusicVolume` | Mutates all above properties, invokes `SaveGameData()` or `ApplyDefaultSettings()` | None |

---

## 3. Detailed Data Specifications

### 3.1 `BP_MainMenu`
- **C++ Base Class**: `UVELOCITYXMainMenuWidget` (derives from `UUserWidget`).
- **Delegates Bound**:
  - `OnPlayButtonClickedDelegate`: Triggers screen fade-out, sets GameMode state to `EGameState::Racing`, and launches the track level.
  - `OnSettingsButtonClickedDelegate`: Creates and displays `BP_Settings` modal.
  - `OnQuitButtonClickedDelegate`: Quits game application cleanly.
  - `OnCreditsButtonClickedDelegate`: Displays developer roster.
- **Data Ingest**:
  - `TotalPlayTime` (`float`): Rendered as formatted string `HHh MMm` in header profile badge.
  - `CompletedRaces` (`int`): Displayed as total career events completed.
  - `BestLapTimes[3]` (`int`): Displays global overall best time on the highway corridor.

### 3.2 `BP_CarSelection`
- **Enum Specification (`ECarType`)**:
  - `ECarType::Supercar`: "APEX GT-1" — 500 HP, 470 N*m, 320 km/h top speed, 3.5s 0-100.
  - `ECarType::TrackSupercar`: "VALKYRIE RS" — 720 HP, 800 N*m, 355 km/h top speed, 2.8s 0-100, active aero & DRS.
  - `ECarType::Hypercar`: "NEBULA HYPER-V" — 1500 HP, 1600 N*m, 420 km/h top speed, 2.4s 0-100, Quad-Turbo AWD catapult & Hybrid Boost.
- **Delegates & Functions**:
  - `AVELOCITYXGameMode::OnCarSelected(ECarType)`: Multicast event broadcasted when car is locked.
  - `AVELOCITYXGameMode::SelectCar(ECarType)`: Locks choice in GameMode.
  - `AVELOCITYXGameMode::GetSelectedCarType()`: Returns currently active car enum.

### 3.3 `BP_VelocityHUD`
- **Vehicle Telemetry Contract (`ABaseCar` via `AVelocityPlayerController::GetControlledCar`)**:
  - `GetSpeedKPH()` & `GetSpeedMPH()`: Feeds `M_UI_SpeedometerDial` dynamic material instance parameter `CurrentSpeedRatio = SpeedKPH / 420.0f` and digital speed readout.
  - `GetEngineRPM()`: Feeds `M_UI_RPMBar` dynamic material instance parameter `CurrentRPMRatio = RPM / 9000.0f`.
  - `Redline Trigger`: If `RPM >= VehicleStats.Engine.RedlineRPM`, triggers `IsRedlineActive = 1.0f` in shader and plays `Anim_RedlineWarningPulse` with audio beep.
  - `GetCurrentGear()`: Returns gear (-1 = Reverse, 0 = Neutral, 1..7 = Forward gears). Triggers `Anim_GearShift` scale pop and shift chime on shift.
  - `GetPerformanceStats()`: Returns model name, horsepower, torque, and top speed specs.
  - `AreHeadlightsOn()`: Updates headlight status badge.
- **Special Vehicle Perks**:
  - **Hypercar Cluster**:
    - `GetBoostChargePercent()`: Drives 0..1 progress bar for hybrid KERS battery charge.
    - `IsHybridBoostActive()`: Triggers electric boost pulse glow and status text.
  - **TrackSupercar Cluster**:
    - `IsDRSActive()`: Activates green glowing DRS badge when active rear wing is flattened for overtaking down straightaways.
- **Race Timer & Delta**:
  - `AVELOCITYXGameMode::GetElapsedTime()`: Precision race stopwatch formatted as `MM:SS.mmm`.

### 3.4 `BP_PauseMenu`
- **Suspension Contract**:
  - `Construct`: Invokes `UGameplayStatics::SetGamePaused(GetWorld(), true)` to pause physics and actors.
  - `Btn_Resume`: Sets unpaused, calls `AVelocityPlayerController::ActionTogglePause()`, restores game input mode.
  - `Btn_Restart`: Unpauses, calls `AVELOCITYXGameMode::ResetTimer()`, calls `AVelocityPlayerController::ActionResetCar()`.
  - `Btn_ExitToMenu`: Unpauses, sets GameMode state to `EGameState::MainMenu`, loads `MainMenu_Map`.

### 3.5 `BP_Settings`
- **Read & Write Contract**:
  - `GraphicsQuality` (`EGraphicsQuality`: Low=0, Medium=1, High=2, Ultra=3).
  - Audio Sliders: `MasterVolume`, `SFXVolume`, `MusicVolume`.
  - `ApplyDefaultSettings()`: Reverts settings to baseline hardware profile.
  - `SaveGameData()`: Flushes settings and career progress to `savegame.json`.

---

## 4. Save Game JSON Schema (`savegame.json`)
```json
{
  "Settings": {
    "GraphicsQuality": 2,
    "ShadowResolution": 2048,
    "TextureQuality": 2,
    "MotionBlur": true,
    "AntiAliasing": true,
    "FieldOfView": 90.0,
    "MasterVolume": 1.0,
    "SFXVolume": 1.0,
    "MusicVolume": 0.8
  },
  "Progress": {
    "TotalPlayTime": 9920.0,
    "CompletedRaces": 14,
    "BestLapTime0": 74382,
    "BestLapTime1": 72805,
    "BestLapTime2": 69914
  }
}
```
Lap times are persisted in milliseconds.
