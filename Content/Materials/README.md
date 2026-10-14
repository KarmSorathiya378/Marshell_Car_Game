# VELOCITY X - Materials Architecture & Asset Guide

This directory contains the complete Master Materials and Material Functions engineered specifically for the 50km+ high-speed highway environment of **VELOCITY X**.

---

## 1. Directory Structure

```
Content/Materials/
├── Functions/
│   └── MF_WindAnimation.COPY           # Dual-frequency procedural vertex wind function
├── Road/
│   ├── M_Asphalt_Master.COPY           # Physical asphalt PBR master with tire groove wear & rain wetness
│   └── M_RoadMarking_Master.COPY       # Retroreflective glass bead thermoplastic markings
├── Barriers/
│   ├── M_Concrete_Barrier_Master.COPY  # New Jersey median barrier with dirt splash gradients
│   └── M_Guardrail_Galvanized_Master.COPY # Galvanized zinc W-beam steel guardrail with cat-eyes
├── Vegetation/
│   ├── M_Foliage_Leaves_TwoSided_Master.COPY # SSS Two-sided foliage shader with wind flex
│   ├── M_Foliage_Bark_Master.COPY      # Weathered bark with directional northern moss
│   ├── M_Foliage_Grass_Master.COPY     # High-speed roadside grass with tip-to-root gradient
│   └── M_Foliage_LOD_Imposter_Master.COPY # 16-angle octahedral imposter shader for distant trees
├── Terrain/
│   ├── M_Landscape_Master.COPY         # Slope-based auto-blend (Asphalt verge / Grass / Cliff rock)
│   └── M_Distant_Mountain_Master.COPY  # Aerial perspective depth-haze mountain backdrop
└── Lighting/
    └── M_LightEmission_Master.COPY     # Physical candela/nit emissive for highway lamps & gantries
```

---

## 2. Realistic Material Parameters & Physical Calibrations

| Material Asset | Base Color (sRGB / Hex) | Metallic | Roughness | Normal Intensity | Special Feature / Physics |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **M_Asphalt_Master** | `#2E2E30` (18% albedo) | `0.00` | `0.55 - 0.88` (Dry) <br>`0.04` (Wet) | Micro: `1.2`<br>Macro: `0.4` | Parallax Occlusion (2.0 cm), Tire groove polishing (-0.18 roughness), Puddle accumulation mask |
| **M_RoadMarking_Master** | `#F2F2EB` (White)<br>`#FFB814` (Yellow) | `0.00` | `0.38` (Dry)<br>`0.12` (Wet) | Micro: `0.8` | Retroreflective glass bead back-scatter (`pow(V·L, 8.0) * 3.5`), Procedural 3m/9m dash pattern |
| **M_Concrete_Barrier_Master**| `#9E9994` (Concrete)<br>`#473829` (Lower splash) | `0.00` | `0.82` (Dry)<br>`0.50` (Mud splash) | Mold: `0.8`<br>Chips: `1.2` | Height gradient splash up to 45 cm, horizontal slipform seams |
| **M_Guardrail_Galvanized** | `#C2C7CC` (Zinc) | `0.95` | `0.28` (Base)<br>`0.55` (Weathered) | Spangle: `0.6` | Galvanized crystalline zinc spangles, red/amber retroreflective cat-eye inserts |
| **M_Foliage_Leaves_TwoSided**| `#244714` (Chlorophyll) | `0.00` | `0.35` (Top)<br>`0.65` (Bottom) | Leaf vein: `1.0` | Two-Sided Foliage SSS (Subsurface color: `#477317`), Per-instance hue variation (±8%), Dual-freq wind |
| **M_Foliage_Bark_Master** | `#382E26` (Bark) | `0.00` | `0.88` (Bark)<br>`0.72` (Moss) | Bark furrow: `1.4`| Directional northern moss overlay driven by `WorldNormal.Y`, trunk wind flex |
| **M_Foliage_Grass_Master** | `#476B1F` (Tip)<br>`#1A290A` (Root) | `0.00` | `0.45` | Blade: `0.7` | PerInstanceFadeAmount dithering (35m - 55m cull), high-speed wind ripple |
| **M_Landscape_Master** | Multi-layer triplanar | `0.00` | `0.65 - 0.82` | Stratified: `1.1`| Auto slope blend (>35° = cliff granite, <35° = forest earth), mip-biasing for 120 FPS |
| **M_LightEmission_Master** | `#FFEBC4` (4200K LED)<br>`#FF8C0D` (Sodium) | `0.00` | N/A (Unlit) | N/A | 50,000 Nits calibrated luminance, distance-compensated bloom preservation |

---

## 3. How to Create and Assign in Unreal Editor

1. **Import / Create Master Materials**:
   - In Content Browser, navigate to `/Game/Materials/...`.
   - Right-click -> **Material** -> Name as specified above.
   - Double click to open, copy-paste or configure expressions matching the `.COPY` definitions.
2. **Create Material Instances**:
   - Right-click `M_Asphalt_Master` -> **Create Material Instance** -> Name `MI_Highway_Asphalt_Dry`.
   - Create another named `MI_Highway_Asphalt_Wet` (toggle `Rain_Wetness = 1.0`).
   - Right-click `M_RoadMarking_Master` -> Create `MI_LaneMarking_Dashed_White` (`bProceduralDashPattern = True`) and `MI_LaneMarking_Solid_Yellow` (`bUseYellowLine = True`).
3. **Assign to Highway Meshes**:
   - Assign `MI_Highway_Asphalt_Dry` to Road Deck Element 0.
   - Assign `MI_LaneMarking_Dashed_White` to Center Lane Markings.
   - Assign `M_Concrete_Barrier_Master` to Central Median Barrier.
   - Assign `M_Guardrail_Galvanized_Master` to Outer Shoulders.
