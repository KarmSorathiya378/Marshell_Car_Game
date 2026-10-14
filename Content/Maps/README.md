# VELOCITY X - Level Design & Map Configuration

This directory contains the level assets, World Partition specifications, and setup scripts for the primary high-speed environment: **`VelocityHighway_50KM`**.

---

## 1. Level Design Overview

* **Map Name**: `VelocityHighway_50KM`
* **Target Game Mode**: `AVELOCITYXGameMode`
* **Default Pawn**: `AVELOCITYXVehicle`
* **Total Track Distance**: **50.0 Kilometers** ($5,000,000\text{ Unreal Units}$)
* **Coordinate System**:
  - **X Axis**: Straight highway longitudinal progression (0 to 5,000,000 cm)
  - **Y Axis**: Lateral span (Negative = Southbound carriageway & Left forest, Positive = Northbound carriageway & Right forest)
  - **Z Axis**: Elevation (Road deck at $Z=0$, hills rising to $Z=+1,200\text{ to }+8,000$, mountains at $Z=+15,000\text{ to }+18,000$)

---

## 2. Unreal Engine 5 World Partition & HLOD Architecture

To maintain **120 FPS** on PC/consoles across a massive 50km corridor without hitching or memory exhaustion:

1. **World Partition Grid Configuration**:
   - **Grid Cell Size**: `25,600 cm` (256 meters)
   - **Loading Range**: `150,000 cm` (1.5 km) around the player vehicle
   - Ensures only the relevant 3km section of high-detail physics and geometry is loaded into memory at any instant.

2. **Hierarchical Level of Detail (HLOD) Layers**:
   - **HLOD Layer 1 (`HLOD_Foliage_Trees`)**:
     * Cell Size: `51,200 cm` (512 meters)
     * Loading Range: `300,000 cm` (3.0 km)
     * Merges clusters of 20-50 trees into simplified proxy geometry with reduced draw calls.
   - **HLOD Layer 2 (`HLOD_Mountains_Distant`)**:
     * Cell Size: `204,800 cm` (2.048 km)
     * Loading Range: `2,500,000 cm` (25.0 km)
     * Extreme distant mountain cards, horizon silhouettes, and major terrain ridges.

3. **Large World Coordinates (LWC)**:
   - UE5 double-precision 64-bit coordinates natively prevent vehicle physics jitter or camera shaking at the 50km marker.

---

## 3. Actor Setup in Persistent Level

When opening `/Game/Maps/VelocityHighway_50KM.umap` in Unreal Editor, ensure the following core actors are present:

1. **`BP_VelocityLevel`** (Assigned as the Level Script Actor)
2. **`BP_AtmosphericLightingSetup`** (Placed at `[0, 0, 0]`)
3. **`BP_HighwayGenerator`** (Placed at `[0, 0, 0]`)
4. **`BP_RoadsideSpawner`** (Placed at `[0, 0, 0]`)
5. **`BP_TrafficManager`** (Placed at `[0, 0, 0]`)
6. **`PlayerStart`** (Placed at `[X=500, Y=387.5, Z=50]`, Northbound middle lane, facing X+)

---

## 4. One-Click Setup via Python Editor Utility

Run `Setup_VelocityHighway_Level.py` inside the Unreal Editor (Output Log -> Python or Tools -> Execute Python Script) to automatically construct and configure the map, spawn all actors, assign materials, and calibrate lighting parameters.
