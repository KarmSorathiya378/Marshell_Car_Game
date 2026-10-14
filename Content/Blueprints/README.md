# VELOCITY X - Blueprints Architecture & Implementation Guide

This directory contains the complete Blueprint systems engineered for **VELOCITY X**. These Blueprints can be imported directly or recreated in the Unreal Editor using the provided node logic and parameter registries.

---

## 1. Complete Blueprint Registry & Asset Paths

```
Content/Blueprints/
├── GameMode/
│   └── BP_VELOCITYXGameMode.COPY        # Master Game Mode managing states, vehicle selection, and progression
├── Player/
│   ├── BP_VELOCITYXPlayerController.COPY# Master Player Controller with HUD binding, pause logic & state transitions
│   └── BP_VELOCITYXPlayerState.COPY     # Authoritative Player State tracking lap stopwatch, split times & arcade scoring
├── UI/
│   ├── BP_VelocityHUD.COPY              # Real-time telemetry racing HUD (Speedometer, Tachometer, Minimap, Timer)
│   ├── BP_PauseMenu.COPY                # Simulation suspended pause modal with settings & resume actions
│   ├── BP_RaceResults.COPY              # Post-race podium scoreboard modal with times, score & retry flow
│   ├── BP_MainMenu.COPY                 # Master game menu with career profile badge
│   ├── BP_CarSelection.COPY             # 3D vehicle showroom selection (Supercar, TrackSpec, Hypercar)
│   └── BP_Settings.COPY                 # Audio, graphics scalability & control preferences
├── Core/
│   ├── BP_VelocityLevel.COPY            # Level Script Actor orchestrating all world systems
│   └── BPI_RaceProgression.COPY         # Interface for StartLine, Checkpoint, and FinishLine events
├── Highway/
│   └── BP_HighwayGenerator.COPY         # Procedural 50km multi-lane highway generator (HISM)
├── Roadside/
│   ├── BP_HighwayStreetlight.COPY       # 12m dual-head LED highway light with IES profiles
│   ├── BP_OverheadGantrySign.COPY       # 34m multi-lane overhead steel truss gantry & VMS
│   ├── BP_EmergencyPhoneBooth.COPY      # Solar-powered roadside SOS call box with beacon
│   └── BP_RoadsideSpawner.COPY          # Procedural placement engine for roadside assets
├── Environment/
│   ├── BP_AtmosphericLightingSetup.COPY # Physically-calibrated sun, atmosphere, fog & post-processing
│   └── BP_ProceduralFoliageCluster.COPY # HISM forest corridor, rolling hills & mountain backdrops
└── Traffic/
    ├── BP_TrafficVehicleBase.COPY       # Autonomous civilian vehicle with raycast obstacle avoidance
    └── BP_TrafficManager.COPY           # Dynamic vehicle pooling and lane streaming system
```

---

## 2. Specific Engineering Values & Calibration Tables

### A. Road Specifications (Dual-Carriageway Highway)
* **Total Length**: `5,000,000.0 cm` = `50,000.0 m` = **50.0 km**
* **Number of Lanes**: 6 lanes total (3 lanes Northbound, 3 lanes Southbound)
* **Standard Lane Width**: `375.0 cm` (3.75 m)
* **Left Inner Shoulder**: `150.0 cm` (1.5 m)
* **Right Outer Emergency Shoulder**: `300.0 cm` (3.0 m)
* **Central Median Strip**: `200.0 cm` (2.0 m) with continuous New Jersey concrete barrier
* **Total Highway Cross-Section Width**:
  $$\text{Total Width} = ((150 + (3 \times 375) + 300) \times 2) + 200 = 3,350.0 \text{ cm} \approx 33.5 \text{ meters}$$
* **Highway Chunk Length**: `20,000.0 cm` (200.0 m) $\rightarrow$ 250 chunks over 50 km
* **Road Markings**:
  - Center line dividers: Dashed white, `300.0 cm` stripe, `900.0 cm` gap, `15.0 cm` width
  - Shoulder lines: Solid white, `20.0 cm` width
  - Median boundary lines: Solid yellow, `15.0 cm` width

### B. Roadside Infrastructure Spacing
* **Streetlight Poles**: Every `5,000.0 cm` (**50 meters**) $\rightarrow$ 1,000 poles over 50 km
* **Overhead Sign Gantries**: Every `250,000.0 cm` (**2.5 kilometers**) $\rightarrow$ 20 gantries over 50 km
* **Emergency SOS Phone Booths**: Every `150,000.0 cm` (**1.5 kilometers**) on outer shoulders $\rightarrow$ ~33 booths per side
* **Kilometer Milestones**: Every `100,000.0 cm` (**1.0 kilometer**)

### C. Physical Lighting & Atmospheric Parameters
* **Directional Light (Sun)**:
  - Intensity: `85,000.0 Lux` (direct sunlight)
  - Color Temperature: `5,600 K` (clean midday/afternoon solar spectrum)
  - Sun Pitch: `-26.0°`, Azimuth: `142.0°` (dramatic high-speed shadow casting)
  - Dynamic Shadow Distance: `45,000.0 cm` (450 meters) with 4 cascades
  - Contact Shadow Length: `0.15` (pin-sharp wheel contact shadows)
* **Sky Atmosphere**:
  - Rayleigh Scattering: `(0.058, 0.135, 0.331)`
  - Mie Scattering: `(0.00399, 0.00399, 0.00399)`
  - Mie Anisotropy: `0.82` (forward solar glare halo)
  - Aerial Perspective Scale: `1.2`
* **Sky Light**:
  - Intensity: `1.25 cd/m²` with real-time capture and Distance Field Ambient Occlusion (DFAO)
* **Exponential Height Fog**:
  - Fog Density: `0.0018`
  - Fog Height Falloff: `0.0008`
  - Volumetric Fog: Enabled, scattering distribution `0.45`, view distance `35,000.0 cm` (350 m)
* **Post-Processing (Unbound Volume)**:
  - Motion Blur: Amount `0.65`, Max `100%` (essential sensation for 300+ km/h)
  - Bloom: Convolution method, Intensity `0.75`, Threshold `1.15`
  - Auto Exposure: Histogram-based, Min EV100 `8.0`, Max EV100 `14.5`
  - Tonemapper: ACES standard, Contrast `1.12`, Saturation `1.06`, Vignette `0.28`

### D. Terrain Height & Procedural Nature Specifications
* **Road Margin Clearance**: `1,800.0 cm` (18.0 m clear zone from road centerline to prevent obstructions)
* **Forest Corridor Lateral Depth**: `45,000.0 cm` (450.0 m on both flanks)
* **Terrain Elevation Profile**:
  - Roadside drainage ditch: `-40.0 cm` dip at $0\text{ m} \le \text{dist} \le 4\text{ m}$ from shoulder
  - Rolling foothill rise: $Z = \Delta y \times 0.065 + (\Delta y^2 \times 0.000018)$
  - Longitudinal undulating hill waves: $Z_{\text{wave}} = 350 \sin(0.0004 X) + 800 \cos(0.00015 X)$ cm
* **Mountain Backdrop**:
  - Distance: `800,000.0 cm` (8.0 km) on Left flank, `1,000,000.0 cm` (10.0 km) on Right flank
  - Peak Altitude: `15,000.0 - 18,000.0 cm` (150m - 180m above sea level)
* **HLOD & Foliage Distance Culling**:
  - Grass: Cull Start `3,500 cm` (35m), Cull End `5,500 cm` (55m)
  - Bushes: Cull Start `12,000 cm` (120m), Cull End `25,000 cm` (250m)
  - Trees: Cull Start `35,000 cm` (350m), Max `75,000 cm` (750m) $\rightarrow$ seamlessly transitions to HLOD 16-angle Octahedral Imposters up to `500,000 cm` (5.0 km)

### E. Traffic Simulation Specifications
* **Active Vehicle Pool**: 35 concurrent vehicles in streaming bubble
* **Spawn Window**: `15,000 cm` to `45,000 cm` (150m to 450m) ahead of player
* **Despawn Threshold**: `> 25,000 cm` (250m) behind player
* **Lane Speed Distribution**:
  - Lane 1 (Slow / Trucks): `85.0 - 95.0 km/h`
  - Lane 2 (Cruising Cars): `110.0 - 125.0 km/h`
  - Lane 3 (High-Speed Overtaking): `140.0 - 165.0 km/h`
* **Forward Collision Avoidance**: Intelligent Driver Model (IDM) 45m forward raycast with dynamic braking and brake light activation
