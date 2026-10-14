# Velocity X project assessment

## Original state

- Three project copies: VelocityX, VelocityX 5.7, and VelocityX 5.7 - 2. The first two contained identical C++ implementations. Loose root Source/Config trees were separate and had no root .uproject.
- No .umap or .uasset content existed. Configured /Game/Maps/EntryMap could not load; Blueprint UI and skeletal vehicle references had no assets.
- Default gameplay spawned a third-person character. Car-selection fallback also selected that character.
- The custom Chaos class computed RPM, tire slip, and suspension telemetry but did not drive a configured Chaos movement component. No skeletal mesh, wheel bones, or physics asset existed.
- Root world-generation functions were placeholders.
- Unreal build blockers included old build defaults, duplicate reflected camera enums, a macro-selected reflected parent, and Blueprint-exposed fixed arrays.
- The legacy controller bound Pause twice and routed driving inputs by reflection every frame, without supporting the custom vehicle's SetThrottle/SetBrake API.

## Working project and implementation

The canonical project is **VelocityX/VelocityX.uproject**, targeting the installed **E:/UE_5.7 (5.7.4)**. Other project copies are retained as reference.

The Drive native mode provides a self-contained arcade time attack: generated Sunridge circuit, authored coupe geometry, fixed-step driving, three tuning presets, brake/reverse, boost, three cameras, three ordered-checkpoint laps, saved best lap, start/pause/results HUD, gamepad support, recovery, and graphics presets. This is a new playable foundation within the existing Unreal project, not a completed realistic Chaos vehicle simulator.

The circuit uses instanced meshes for track and scenery, no per-tree ticking, simple collision, bounded 120 Hz driving updates, and DX11 defaults without Lumen or virtual shadow maps. Graphics quality is adjustable with F1. Frame-rate claims must be based on the rendered runtime tests rather than the 120 FPS cap.

Generated content is reproducible from Scripts/create_drive_assets.py. Original coupe source and FBX exports are in ArtSource; Scripts/create_car_mesh.py reproduces them with Blender.

## Remaining scope beyond this playable foundation

The circuit is a flat oval time attack. There are no opponents, multiplayer, progression, photorealistic licensed vehicles, suspension/rollover simulation, or authored cockpit interior. The generated engine sound is synthetic. These require additional gameplay/art work; this project should not be described as production-complete or photorealistic.

## Verification so far

- Development Editor C++ build: passed with Unreal 5.7.4 and the installed MSVC 14.51 compiler.
- Asset bootstrap: passed; Sunridge.umap, sixteen materials, and two authored car meshes saved.
- Direct Unreal tests: FDriveRaceTest and FDriveDynamicsTest passed with zero errors. The tests run in isolated runtime worlds, including through the DriveValidation commandlet.
- Build/cache setup: local compilation uses -NoUBA/-NoUBALocal. Shader temporary files and derived data are kept inside the project.

The headless FBX bootstrap selects Unreal's legacy importer to avoid a Slate assertion in the Interchange path. The supported importer toggle is documented by [Epic](https://dev.epicgames.com/documentation/en-us/unreal-engine/importing-assets-using-interchange-in-unreal-engine).

## Final rendered verification

- Rendered driving smoke test: passed and exited successfully; the possessed pawn accelerated and moved on Sunridge.
- Final start-screen and driving screenshots inspected at 1600 x 900: Saved/Screenshots/DriveMenu.png and DriveSmoke.png.
- Short offscreen Development Editor sample on NVIDIA RTX 3050 6 GB Laptop GPU: 76.9 FPS over 4.16 seconds at 1600 x 900. This includes startup/screenshot overhead and is not a sustained full-race benchmark.
- Final launcher build check: target up to date, succeeded in 0.85 seconds.
- First shader-cache generation was slow; subsequent launches reuse the project-local cache.

This is playable through the installed Unreal runtime/editor with -game. A separately packaged, redistributable Windows build has not been produced.
