# Play the actual Unreal game in a browser

The native game is `VelocityX/VelocityX.uproject`: the Sunridge circuit, coupe mesh, car presets, laps, checkpoints, boost, audio, and Unreal HUD. `VelocityX_Web/index.html` is a separate simplified Three.js demo; it does not run this Unreal project.

Pixel Streaming runs the actual Unreal game on this PC and sends its video/audio to the browser. Browser input travels back to the running game. The project enables Epic's **Pixel Streaming 2** plugin and uses the matching **UE5.7** infrastructure.

## First test on this PC

Requirements: Unreal Engine 5.7 (default `E:\UE_5.7`), the existing C++ build tools, Node.js 24, Git, a compatible GPU, and an internet connection for the first dependency download.

1. Double-click **Play Unreal in Browser.cmd** in the repository root. The first run downloads and builds Epic's infrastructure, then builds and launches the Unreal game and signalling server. Later runs reuse the infrastructure.
2. Open **http://127.0.0.1:8080/player.html** in Chrome or Edge on this PC. Wait for Unreal to finish loading.
3. Click the connect/play overlay if shown, then click the video to focus input. This is the Unreal start screen; press **Enter** to start the race. Select **1 / 2 / 3** before starting, drive with **W/A/S/D**, use **Shift** to boost, **Space** to handbrake, **C** to change camera, and **P** to pause. Browser shortcuts can intercept some function keys.
4. Keep the PC and streaming processes running while playing.

For a custom Unreal installation:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File VelocityX/Scripts/Start-WebStreaming.ps1 -EngineRoot "C:\Program Files\Epic Games\UE_5.7"
```

To stop only the processes started by this launcher:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File VelocityX/Scripts/Stop-WebStreaming.ps1
```

The launcher records the processes and their start times in `.local/streaming/processes.json` to avoid stopping an unrelated process that reuses a process ID. Runtime logs are `.local/streaming/unreal.log`, `server.log`, and `server-error.log`. Infrastructure, dependencies, and these logs stay outside Git in `.local`. If port 8080 or 8888 is busy, pass another `-HttpPort` or `-StreamerPort` to the start script.

The local test uses the installed Unreal Editor's standalone `-game` mode with offscreen rendering. It does not require a packaged build. A production GPU host should use a packaged build instead of depending on the Editor installation.

## Put the same game on the public website

The Vercel deployment currently serves the separate Three.js demo. Launching a local stream does not automatically replace it or make this PC reachable from the public internet.

For a public Unreal player, the game must keep running on a GPU host. Provide a public HTTPS/WSS endpoint for the player/signalling service and configure WebRTC connectivity (including TURN when needed). The Vercel page can then link to or embed that player. A localhost address on Vercel would point to each visitor's own machine and will not connect them to this PC.

The first local test shares **one game instance**. Multiple browsers attached to that instance see the same game and may share control. Independent races for different players need separate Unreal sessions and session allocation. Hosting capacity, public networking, and the public player URL must be configured before replacing the deployed demo with the Unreal player.

References: [Epic's UE5.7 setup guide](https://dev.epicgames.com/documentation/en-us/unreal-engine/getting-started-with-pixel-streaming-in-unreal-engine?application_version=5.7), [Epic's streaming infrastructure](https://github.com/EpicGamesExt/PixelStreamingInfrastructure/tree/UE5.7).
