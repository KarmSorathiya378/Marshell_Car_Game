# Play Velocity X

Double-click **../Play Velocity X.cmd**. The launcher uses Unreal 5.7 at E:/UE_5.7 and builds incrementally when needed.

Press **Enter** to begin the three-lap Sunridge time attack. Before starting, choose **1 / 2 / 3** for Apex GT, Veloce R, or Titan X.

| Control | Action |
|---|---|
| W / Up | Accelerate |
| S / Down | Brake, then reverse |
| A / D or arrows | Steer |
| Space | Handbrake |
| Left Shift | Boost; recharges when released |
| C | Near chase / far chase / hood camera |
| R | Recover at last valid checkpoint |
| Esc / P | Pause or resume |
| F5 | Restart session |
| F1 | Cycle graphics presets |
| Alt+F4 | Quit |

Gamepad: right/left triggers accelerate/brake, left stick steers, bottom face button starts, right face button handbrakes, left face button boosts, right stick click changes camera, View recovers, Menu pauses.

Drive forward through all sixteen checkpoints each lap. Skipping or crossing backward does not advance the race. The marker on the minimap shows the next checkpoint. Best lap is saved in Saved/SaveGames.

To open the editor: powershell -NoProfile -ExecutionPolicy Bypass -File Scripts/Play.ps1 -Editor

For a different installation path, pass -EngineRoot to Scripts/Play.ps1. This project targets UE 5.7.

See ../PROJECT_ANALYSIS.md for the original blockers and current scope.

The circuit is generated at runtime. In Unreal Editor, press Play to see and drive the track.

## Step-by-step startup

1. Open D:\karm\car_game in File Explorer.
2. Double-click Play Velocity X.cmd to launch directly into the game using Unreal 5.7. Allow the build and loading to finish.
3. Click the game window, then press Enter to start. Use W/A/S/D to drive.

## Black editor viewport

Sunridge is a runtime-generated circuit: its track, lighting, and car appear when Play begins. An empty or black editor viewport before Play is expected with this implementation.

1. Double-click ../Open Velocity X Editor.cmd. This opens the repaired VelocityX project with its local shader/cache settings.
2. In the Content Browser, open Content/Maps/Sunridge if it is not already open.
3. Use the Play dropdown to select Selected Viewport or New Editor Window (PIE), then click the green Play button. Do not select Simulate.
4. Click inside the running game and press Enter.

Use VelocityX/VelocityX.uproject. The folders VelocityX 5.7 and VelocityX 5.7 - 2 are older copies, not the repaired project.

If the running game itself stays black after loading finishes, capture that window and check Saved/Logs/VelocityX.log. A black editor viewport before Play is different from a black running game.
