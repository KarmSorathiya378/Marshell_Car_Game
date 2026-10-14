# VELOCITY / X

A native Unreal Engine 5.7 arcade time-attack game. The active project is this folder's **VelocityX.uproject**.

**Play:** double-click **../Play Velocity X.cmd**, then press **Enter**. Select a car with **1 / 2 / 3** before starting.

The Sunridge circuit includes three-lap checkpoint racing, boost, chase/hood cameras, saved best lap, generated engine audio, and adjustable graphics. All required materials, the map, and authored coupe meshes are local.

- [Controls and launch options](PLAY.md)
- [Project assessment and current scope](../PROJECT_ANALYSIS.md)
- [Original setup notes, retained for reference](LEGACY_SETUP.md)

## Development

Installed engine used here: E:/UE_5.7 (5.7.4).

Run Scripts/Play.ps1 -BuildOnly for an incremental editor build, or Scripts/Play.ps1 -Editor to open Unreal Editor. Pass -EngineRoot if the engine is installed elsewhere.

Run Scripts/Test.ps1 for Unreal automation and the runtime driving smoke test. Logs, reports, screenshots, and save data are under Saved; build-session logs are also available in ../Logs.

The native gameplay is in Source/VelocityX/Private/Drive. The older vehicle/UI code remains available, with build/reflection fixes, but the default game mode is DriveGameMode. Other project copies in the parent folder are not the active build.

The bootstrap script Scripts/create_drive_assets.py recreates materials, imports ArtSource/*.fbx, and writes the Sunridge map. The original Blender model and its generator are retained under ArtSource and Scripts.
