# Velocity X browser build

The public browser edition is a **standalone port of the Sunridge game**, not the old straight-road demo and not a Pixel Streaming player. Each visitor runs their own race locally. No Unreal installation, account, signalling server, GPU host, or backend is needed to play.

## What is reused

- `web-src/race.cpp` ports the native `DriveCar.cpp` arcade dynamics, car tuning, 120 Hz simulation, countdown, ordered checkpoints, recovery, best lap, and three-lap race rules to portable C++.
- Emscripten **6.0.12** compiles that code to the real `VelocityX_Web/wasm/velocityx.wasm` binary. The binary has no host imports; gameplay executes in WebAssembly, with no JavaScript physics fallback.
- `VelocityX_Web/app.js` renders the Sunridge oval, barriers, markings, desert scenery and pit structures using the layout and palette from `DriveCircuit.cpp` and `create_drive_assets.py`. It uses the same seeded random stream for scenery placement.
- The coupe body, glass and ridge geometry come from the original `VelocityX/ArtSource/VelocityCoupe.blend`. `scripts/export-web-meshes.py` exports them to browser geometry. Distant mountain meshes use a lower-detail version of the original mesh.
- The HUD, car selection, minimap, three cameras, boost, engine sound and local best record follow the native game. Touch driving controls support phones; keyboard and gamepad work on desktop.

This does **not** compile the Unreal Engine runtime itself to WASM. Three.js provides the browser renderer, so lighting, shadows, post-processing and materials differ from Unreal. The oval boundary constraint replaces Unreal's swept chassis collision. The road is a continuous ribbon to avoid overlapping segment artifacts. This is a playable browser port of the same circuit and mechanics, not a promise of identical engine rendering or collision behavior. The native Unreal source and authored meshes remain separate.

## Play locally

Install Node.js 24 and run:

```sh
npm run dev
```

Open **http://127.0.0.1:8000**. Do not double-click `index.html`: browsers need HTTP to fetch the WASM binary and mesh assets. Compiled WASM and meshes are committed, so playing or deploying does not require a compiler or Blender.

Select **1 / 2 / 3** or click a car, then click **Start your engine** or press **Enter**. Use **WASD / arrows** to drive, **Space** to handbrake, **Shift** to boost, **C** to change camera, **R** to recover, **P / Esc** to pause, **F5** to restart, and **F1** to change graphics quality. The toolbar offers pause, camera, sound and fullscreen. Touch devices show steer, brake, throttle, drift and boost controls. Losing focus pauses and clears held inputs.

Best times are stored only on the current browser/device. Storage and audio failures do not prevent play. Graphics performance depends on the visitor's device; touch devices and software renderers start at the Performance preset. Real phone hardware and browsers beyond Chromium still need broader testing.

## Rebuild after editing C++

On Windows:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/setup-wasm.ps1
npm run build:wasm
npm run check
```

On Linux/macOS, install and activate Emscripten 6.0.12 using the [official SDK](https://emscripten.org/docs/getting_started/downloads.html), then put `em++` on PATH or set `EMXX` to its path. Run the same npm commands. The build checks the compiler version and treats C++ warnings as errors.

Commit the C++ source, `velocityx.wasm`, and `wasm/manifest.json` together. The manifest records normalized source/build-recipe hashes and the compiled binary hash. Checks reject a stale or changed binary.

To re-export assets with Blender 5.2:

```powershell
& 'C:/Program Files/Blender Foundation/Blender 5.2/blender.exe' -b VelocityX/ArtSource/VelocityCoupe.blend --python scripts/export-web-meshes.py
```

The export only reads the original `.blend` file and writes web assets; it does not save changes to the authored file.

## Checks and deployment

`npm run check` uses Node built-ins: no dependency install, browser, compiler or Unreal runtime is required. It validates the binary, source hashes, local scripts and meshes, driving dynamics, race-order regressions, timing, recovery, and three complete races driven through the real WASM physics, one per preset.

For the browser suite:

```sh
npm ci
npx playwright install chromium
npm run test:browser
```

Desktop and mobile viewport tests exercise rendering, car selection, driving, boost, steering, pause/resume, cameras, recovery, saved best times, restricted storage, missing assets/WASM, and unavailable WebGL. They also check that gameplay makes no external requests. On Windows an existing Chromium browser can be used by setting `VX_BROWSER_PATH` to its executable. Mobile viewport tests are emulation, not real-device certification.

GitHub Actions installs the pinned compiler, rebuilds WASM, checks that it matches the committed artifacts, runs the core checks and then the browser suite. Failure artifacts contain screenshots/traces.

Vercel's **Root Directory** must be the repository root (empty in the dashboard), with **Other** as the framework preset. The committed `vercel.json` runs `npm run check` and publishes only `VelocityX_Web`. It serves the already compiled `.wasm` and mesh files; it does not download Emscripten or compile Unreal. Vercel's own core checks block a failing build. GitHub's browser suite runs independently; it is not a required Vercel deployment check unless separately configured in Vercel.

After a push to the linked production branch, verify the deployment is Ready and open the public site. The new menu says **SUNRIDGE MOTOR PARK / TIME ATTACK** and has three car options. `window.VelocityX.engine` reports `WebAssembly`; its read-only diagnostics expose the current race and render statistics.
