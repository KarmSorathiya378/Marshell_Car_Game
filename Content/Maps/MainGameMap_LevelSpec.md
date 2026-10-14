# VELOCITY X - MainGameMap Level Design Specification

## 1. Level Design Executive Summary
- **Map Name**: `MainGameMap`
- **Asset Package**: `/Game/Maps/MainGameMap`
- **Associated GameMode**: `BP_VELOCITYXGameMode` / `AVELOCITYXGameMode`
- **Course Type**: High-speed multi-lane highway sprint circuit
- **Total Track Length**: **7.5 Kilometers** (750,000 Unreal Units)
- **Track Layout**: 6-lane divided interstate (3 lanes Northbound +X direction, 3 lanes Southbound -X direction)
- **Lane Width**: 375 cm (3.75 m per lane)
- **Median Width**: 200 cm (2.0 m Jersey barrier divider)
- **Shoulders**: 150 cm left emergency lane, 300 cm right breakdown lane

---

## 2. Race Course Milestones & Sector Layout

```
[Start Gantry & Grid] -> [Sector 1 Split] -> [Sector 2 Split] -> [Sector 3 Split] -> [Sector 4 Split] -> [Finish Line]
   X = 1,000 uu             X = 150,000 uu       X = 300,000 uu       X = 450,000 uu       X = 600,000 uu       X = 750,000 uu
    (0.01 km)                  (1.50 km)            (3.00 km)            (4.50 km)            (6.00 km)            (7.50 km)
```

### 2.1 Starting Grid & Starting Line (X = 1,000 cm)
- **Start Gantry**: 37m truss structure with F1-style 5-stage red countdown lights and green launch signal.
- **Starting Line Trigger** ([`BP_StartLineTrigger`](file:///D:/karm/car_game/Content/Blueprints/Triggers/BP_StartLineTrigger.COPY)):
  - Spans all northbound carriageway lanes (18m width, 10m height).
  - Starts the official race clock via `AVELOCITYXGameMode::StartTimer()`.
  - Dispatches `OnStartLineCrossed` interface call.
- **Starting Grid Slots** ([`BP_VehicleSpawnPoint`](file:///D:/karm/car_game/Content/Blueprints/Vehicle/BP_VehicleSpawnPoint.COPY)):
  - 8 staggered starting slots (14m spacing between rows).
  - **Pole Position (Slot 1)**: `X = 1,000 cm, Y = 662.5 cm, Z = 38 cm` (Northbound Middle Lane).
  - **Slot 2**: `X = 300 cm, Y = 287.5 cm, Z = 38 cm` (Northbound Fast Lane).
  - **Slot 3**: `X = -400 cm, Y = 662.5 cm, Z = 38 cm` (Northbound Middle Lane).
  - **Slot 4**: `X = -1,100 cm, Y = 287.5 cm, Z = 38 cm` (Northbound Fast Lane).

### 2.2 Sector Checkpoint Triggers ([`BP_CheckpointTrigger`](file:///D:/karm/car_game/Content/Blueprints/Triggers/BP_CheckpointTrigger.COPY))
1. **Checkpoint 01 (Sector 1 Split)**:
   - **Position**: `X = 150,000 cm` (1.5 km mark)
   - **Target Time**: `00:22.000`
   - **Features**: Overhead timing gantry with cyan laser split line; measures Sector 1 split time and speed trap.
2. **Checkpoint 02 (Sector 2 Split)**:
   - **Position**: `X = 300,000 cm` (3.0 km mark)
   - **Target Time**: `00:44.000`
   - **Features**: Measures mid-course split time; high-speed aero test section.
3. **Checkpoint 03 (Sector 3 Split)**:
   - **Position**: `X = 450,000 cm` (4.5 km mark)
   - **Target Time**: `01:06.000`
   - **Features**: Approaching sweeping curve; evaluates handling and corner exit speed.
4. **Checkpoint 04 (Sector 4 Split)**:
   - **Position**: `X = 600,000 cm` (6.0 km mark)
   - **Target Time**: `01:28.000`
   - **Features**: Final sector trap gate; prepares driver for maximum top speed sprint.

### 2.3 Finish Line (X = 750,000 cm / 7.5 km)
- **Finish Line Trigger** ([`BP_FinishLineTrigger`](file:///D:/karm/car_game/Content/Blueprints/Triggers/BP_FinishLineTrigger.COPY)):
  - Massive checkered arch banner with gold laser timing beam.
  - Halts race clock (`AVELOCITYXGameMode::PauseTimer()`).
  - Records total race duration and trap speed.
  - Updates and persists `BestLapTimes` and career stats in `UVELOCITYXGameInstance` (`savegame.json`).
  - Triggers victory celebration fireworks, crowd cheer, and fanfare audio.
  - Transitions game state to `EGameState::GameOver` after 3.5s celebration delay.

---

## 3. Vehicle Spawning Architecture
- **Master Vehicle Spawner** ([`BP_VehicleSpawner`](file:///D:/karm/car_game/Content/Blueprints/Vehicle/BP_VehicleSpawner.COPY)):
  - Placed at Pole Position coordinates (`X=1000, Y=662.5, Z=38`).
  - Reads player's selected vehicle archetype from `AVELOCITYXGameMode::GetSelectedCarType()`:
    - `ECarType::Supercar` -> Spawns `ASupercar` ("APEX GT-1")
    - `ECarType::TrackSupercar` -> Spawns `ATrackSupercar` ("VALKYRIE RS")
    - `ECarType::Hypercar` -> Spawns `AHypercar` ("NEBULA HYPER-V")
  - Automatically handles PlayerController possession, input mode activation, and camera boom alignment.

---

## 4. Recovery & Unstuck System
- **Corridor Recovery Zone** ([`BP_RecoveryZone`](file:///D:/karm/car_game/Content/Blueprints/Triggers/BP_RecoveryZone.COPY)):
  - Bilateral box triggers covering the outer highway shoulders and wilderness buffer.
  - When a car breaches highway containment, rolls, or crashes past guardrails, safely teleports the car onto the nearest northbound lane facing forward (+X) with zeroed angular momentum.
- **Vehicle Recovery Watchdog** ([`BP_VehicleRecoveryHandler`](file:///D:/karm/car_game/Content/Blueprints/Vehicle/BP_VehicleRecoveryHandler.COPY)):
  - Continuously monitors vehicle chassis orientation:
    - Inverted roll/pitch > 70° for > 2.5s -> Triggers auto-recovery.
    - Full throttle with < 4 km/h speed (pinned against obstacle) for > 3.5s -> Triggers auto-recovery.
    - Responds instantly to player manual reset input (`R` key / controller `Y` button).

---

## 5. Blueprint Interface Integration
All triggers interface directly through:
- [`BPI_RaceProgression`](file:///D:/karm/car_game/Content/Blueprints/Core/BPI_RaceProgression.COPY):
  - `OnStartLineCrossed(Vehicle, Timestamp)`
  - `OnCheckpointPassed(Index, DistanceKm, SplitTime, SpeedKPH, SpeedMPH, Vehicle)`
  - `OnFinishLineCrossed(Vehicle, TotalTime, TopSpeedKPH)`
- [`BPI_CheckpointListener`](file:///D:/karm/car_game/Content/Blueprints/Triggers/BPI_CheckpointListener.COPY):
  - Real-time split time, delta calculations, and speed trap broadcasts to `BP_VelocityHUD`.
