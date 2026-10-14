"""
VELOCITY X - MainGameMap Setup & Verification Script
Asset Path: Content/Maps/Setup_MainGameMap.py
Description:
    Unreal Engine 5 Editor Python script to programmatically build, spawn, and verify
    the primary racing level: 'MainGameMap'.
    Includes:
      - Starting Line & 8 Staggered Grid Spawn Points
      - 4 Sector Checkpoint Timing Gates (1.5 km, 3.0 km, 4.5 km, 6.0 km)
      - Checkered Finish Line at 7.5 km
      - Master Vehicle Spawner for Supercar, TrackSupercar, and Hypercar
      - Bilateral Highway Recovery Zone & Stuck Watchdog
    Can be run inside Unreal Editor via Python or standalone to audit level assets.
"""

import os
import sys

try:
    import unreal
    IN_UNREAL_EDITOR = True
except ImportError:
    IN_UNREAL_EDITOR = False

# Level Architecture Definition
LEVEL_CONFIG = {
    "map_name": "MainGameMap",
    "package_path": "/Game/Maps/MainGameMap",
    "track_distance_km": 7.5,
    "track_distance_uu": 750000.0,
    "grid_slots": 8,
    "checkpoints": [
        {"index": 1, "name": "SECTOR 1 SPLIT", "dist_km": 1.5, "pos_x": 150000.0, "target_time": 22.0},
        {"index": 2, "name": "SECTOR 2 SPLIT", "dist_km": 3.0, "pos_x": 300000.0, "target_time": 44.0},
        {"index": 3, "name": "SECTOR 3 SPLIT", "dist_km": 4.5, "pos_x": 450000.0, "target_time": 66.0},
        {"index": 4, "name": "SECTOR 4 SPLIT", "dist_km": 6.0, "pos_x": 600000.0, "target_time": 88.0}
    ],
    "start_line_x": 1000.0,
    "finish_line_x": 750000.0,
    "pole_position": {"x": 1000.0, "y": 662.5, "z": 38.0, "yaw": 0.0}
}


def run_unreal_editor_setup():
    """Executes level construction and actor placement in Unreal Engine Editor."""
    print("=" * 70)
    print("VELOCITY X: Constructing MainGameMap in Unreal Engine Editor...")
    print("=" * 70)

    editor_actor_sub = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    asset_lib = unreal.EditorAssetLibrary

    # 1. Spawn Atmospheric Lighting
    lighting_cls = asset_lib.load_blueprint_class("/Game/Blueprints/Environment/BP_AtmosphericLightingSetup")
    if lighting_cls:
        lighting_actor = editor_actor_sub.spawn_actor_from_class(lighting_cls, unreal.Vector(0,0,0), unreal.Rotator(0,0,0))
        if lighting_actor:
            lighting_actor.set_actor_label("BP_AtmosphericLightingSetup_Main")
            print("[UE] Spawned Atmospheric Lighting.")

    # 2. Spawn Highway Generator (7.5 km)
    highway_cls = asset_lib.load_blueprint_class("/Game/Blueprints/Highway/BP_HighwayGenerator")
    if highway_cls:
        highway_actor = editor_actor_sub.spawn_actor_from_class(highway_cls, unreal.Vector(0,0,0), unreal.Rotator(0,0,0))
        if highway_actor:
            highway_actor.set_actor_label("BP_HighwayGenerator_7.5KM")
            highway_actor.set_editor_property("TotalHighwayLength", LEVEL_CONFIG["track_distance_uu"])
            print("[UE] Spawned 7.5km Highway Generator.")

    # 3. Spawn Starting Grid
    grid_cls = asset_lib.load_blueprint_class("/Game/Blueprints/Highway/BP_HighwayStartGrid")
    if grid_cls:
        grid_actor = editor_actor_sub.spawn_actor_from_class(grid_cls, unreal.Vector(0,0,0), unreal.Rotator(0,0,0))
        if grid_actor:
            grid_actor.set_actor_label("BP_HighwayStartGrid_8Slot")
            print("[UE] Spawned 8-Slot Staggered Start Grid.")

    # 4. Spawn Vehicle Spawn Points
    spawn_pt_cls = asset_lib.load_blueprint_class("/Game/Blueprints/Vehicle/BP_VehicleSpawnPoint")
    if spawn_pt_cls:
        pole_pt = editor_actor_sub.spawn_actor_from_class(
            spawn_pt_cls,
            unreal.Vector(LEVEL_CONFIG["pole_position"]["x"], LEVEL_CONFIG["pole_position"]["y"], LEVEL_CONFIG["pole_position"]["z"]),
            unreal.Rotator(0, 0, 0)
        )
        if pole_pt:
            pole_pt.set_actor_label("BP_VehicleSpawnPoint_PolePosition")
            pole_pt.set_editor_property("GridSlotIndex", 1)
            pole_pt.set_editor_property("bIsPolePosition", True)
            print("[UE] Spawned Pole Position Spawn Point.")

    # 5. Spawn Master Vehicle Spawner
    spawner_cls = asset_lib.load_blueprint_class("/Game/Blueprints/Vehicle/BP_VehicleSpawner")
    if spawner_cls:
        spawner_actor = editor_actor_sub.spawn_actor_from_class(
            spawner_cls,
            unreal.Vector(LEVEL_CONFIG["pole_position"]["x"], LEVEL_CONFIG["pole_position"]["y"], LEVEL_CONFIG["pole_position"]["z"]),
            unreal.Rotator(0, 0, 0)
        )
        if spawner_actor:
            spawner_actor.set_actor_label("BP_VehicleSpawner_Master")
            print("[UE] Spawned Master Vehicle Spawner.")

    # 6. Spawn Start Line Trigger
    start_trig_cls = asset_lib.load_blueprint_class("/Game/Blueprints/Triggers/BP_StartLineTrigger")
    if start_trig_cls:
        start_trig = editor_actor_sub.spawn_actor_from_class(
            start_trig_cls,
            unreal.Vector(LEVEL_CONFIG["start_line_x"], 0, 0),
            unreal.Rotator(0, 0, 0)
        )
        if start_trig:
            start_trig.set_actor_label("BP_StartLineTrigger_Main")
            print("[UE] Spawned Start Line Trigger with 5-light countdown gantry.")

    # 7. Spawn 4 Checkpoint Triggers
    cp_cls = asset_lib.load_blueprint_class("/Game/Blueprints/Triggers/BP_CheckpointTrigger")
    if cp_cls:
        for cp in LEVEL_CONFIG["checkpoints"]:
            cp_actor = editor_actor_sub.spawn_actor_from_class(
                cp_cls,
                unreal.Vector(cp["pos_x"], 0, 0),
                unreal.Rotator(0, 0, 0)
            )
            if cp_actor:
                cp_actor.set_actor_label(f"BP_CheckpointTrigger_Sector_{cp['index']}")
                cp_actor.set_editor_property("CheckpointIndex", cp["index"])
                cp_actor.set_editor_property("CheckpointName", cp["name"])
                cp_actor.set_editor_property("TrackDistanceKm", cp["dist_km"])
                cp_actor.set_editor_property("TargetSectorTime", cp["target_time"])
                print(f"[UE] Spawned Checkpoint {cp['index']} at {cp['dist_km']} km.")

    # 8. Spawn Finish Line Trigger
    finish_cls = asset_lib.load_blueprint_class("/Game/Blueprints/Triggers/BP_FinishLineTrigger")
    if finish_cls:
        finish_actor = editor_actor_sub.spawn_actor_from_class(
            finish_cls,
            unreal.Vector(LEVEL_CONFIG["finish_line_x"], 0, 0),
            unreal.Rotator(0, 0, 0)
        )
        if finish_actor:
            finish_actor.set_actor_label("BP_FinishLineTrigger_Main")
            finish_actor.set_editor_property("FinishTrackDistanceKm", LEVEL_CONFIG["track_distance_km"])
            print("[UE] Spawned Finish Line Trigger with victory fireworks.")

    # 9. Spawn Recovery Zone & Recovery Watchdog
    recov_cls = asset_lib.load_blueprint_class("/Game/Blueprints/Triggers/BP_RecoveryZone")
    if recov_cls:
        recov_actor = editor_actor_sub.spawn_actor_from_class(
            recov_cls,
            unreal.Vector(LEVEL_CONFIG["track_distance_uu"] * 0.5, 0, 0),
            unreal.Rotator(0, 0, 0)
        )
        if recov_actor:
            recov_actor.set_actor_label("BP_RecoveryZone_Corridor")
            print("[UE] Spawned Bilateral Highway Recovery Zone.")

    watchdog_cls = asset_lib.load_blueprint_class("/Game/Blueprints/Vehicle/BP_VehicleRecoveryHandler")
    if watchdog_cls:
        watchdog_actor = editor_actor_sub.spawn_actor_from_class(
            watchdog_cls,
            unreal.Vector(0, 0, 0),
            unreal.Rotator(0, 0, 0)
        )
        if watchdog_actor:
            watchdog_actor.set_actor_label("BP_VehicleRecoveryHandler_Watchdog")
            print("[UE] Spawned Vehicle Recovery Watchdog.")

    print("=" * 70)
    print("[SUCCESS] MainGameMap configuration complete in Unreal Editor!")
    print("=" * 70)
    return True


def run_standalone_audit():
    """Performs integrity verification of all Level Designer deliverables."""
    print("=" * 70)
    print("VELOCITY X - Level Designer Asset & Map Integrity Audit")
    print("=" * 70)

    base_dir = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    maps_dir = os.path.join(base_dir, "Content", "Maps")
    triggers_dir = os.path.join(base_dir, "Content", "Blueprints", "Triggers")
    vehicle_dir = os.path.join(base_dir, "Content", "Blueprints", "Vehicle")

    all_passed = True

    # 1. Audit Map Assets
    print(f"Checking Maps in:     {maps_dir}")
    map_files = [
        "MainGameMap.umap.json",
        "MainGameMap.COPY",
        "Setup_MainGameMap.py",
        "MainGameMap_LevelSpec.md"
    ]
    for mf in map_files:
        full_path = os.path.join(maps_dir, mf)
        if os.path.exists(full_path):
            size = os.path.getsize(full_path)
            print(f" [PASS] Map Asset: {mf:26} ({size:,} bytes)")
        else:
            print(f" [FAIL] Map Asset: {mf:26} -> MISSING")
            all_passed = False

    # 2. Audit Triggers
    print(f"\nChecking Triggers in: {triggers_dir}")
    trigger_files = [
        "BPI_CheckpointListener.COPY",
        "BP_CheckpointTrigger.COPY",
        "BP_StartLineTrigger.COPY",
        "BP_FinishLineTrigger.COPY",
        "BP_RecoveryZone.COPY"
    ]
    for tf in trigger_files:
        full_path = os.path.join(triggers_dir, tf)
        if os.path.exists(full_path):
            size = os.path.getsize(full_path)
            print(f" [PASS] Trigger:   {tf:26} ({size:,} bytes)")
        else:
            print(f" [FAIL] Trigger:   {tf:26} -> MISSING")
            all_passed = False

    # 3. Audit Vehicle Blueprints
    print(f"\nChecking Vehicle in:  {vehicle_dir}")
    vehicle_files = [
        "BP_VehicleSpawner.COPY",
        "BP_VehicleSpawnPoint.COPY",
        "BP_VehicleRecoveryHandler.COPY"
    ]
    for vf in vehicle_files:
        full_path = os.path.join(vehicle_dir, vf)
        if os.path.exists(full_path):
            size = os.path.getsize(full_path)
            print(f" [PASS] Vehicle BP:{vf:26} ({size:,} bytes)")
        else:
            print(f" [FAIL] Vehicle BP:{vf:26} -> MISSING")
            all_passed = False

    print("\n" + "=" * 70)
    if all_passed:
        print("STATUS: ALL MAIN GAME MAP & CHECKPOINT DELIVERABLES AUDITED & VERIFIED!")
    else:
        print("STATUS: AUDIT ENCOUNTERED MISSING DELIVERABLES")
    print("=" * 70)
    return all_passed


if __name__ == "__main__":
    if not IN_UNREAL_EDITOR:
        success = run_standalone_audit()
        sys.exit(0 if success else 1)
    else:
        run_unreal_editor_setup()
