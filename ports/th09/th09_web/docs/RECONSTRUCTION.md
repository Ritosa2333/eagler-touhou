# TH09 1.50a reconstruction

Target: the local Japanese th09.exe, pinned in ../target.json.

Status (2026-09-20): playable authored C++ / SDL3 release candidate, compiled and tested.
Public deployment is recorded separately in artifacts/sdl-release/verification/deployment.json; do not infer live status from this document.

The production application uses native C++ game logic and the existing
portable SDL3 / WebGL2 platform, with CPU sprite vertices, ordered batching,
streaming-buffer replacement, native audio, local browser saves and touch input.
TH09-specific simulation, dual playfields, charge, attacks, AI, menus and replay
behavior are implemented in cpp/game, based on the local TH09 1.50a executable and resources.

Ghidra candidates under artifacts/cpp/analysis are analysis evidence only. They
are not compiled or presented as verified game logic. An x86 oracle is used only
for development comparisons against the pinned original executable.

Completion requires the complete game paths, runnable web build, original-behavior
comparisons, scene/render/audio/save/replay checks, and verification of deployment.
Finite tests do not constitute proof of every possible original state or device.

## Verified subsystem checkpoint

The following authored/adapted C++ modules are compiled into the development
comparison fixture. This list describes subsystem evidence, not whole-game completion.

| Subsystem | Original comparison |
| --- | --- |
| RNG, timers, device input | 25,600 / 9,408 / 16,385 comparisons |
| LZSS and resource crypt | 42 codec patterns; 240 crypt cases |
| PBGZ archive | All 256 resources, 176,902,249 decoded bytes compared byte-for-byte |
| Score/replay containers | Original score reader accepts new files; all 3 demo replays decode and re-encode exactly |
| Player movement | 8,192 frames, all 16 SHT files, both sides; effect/callback boundaries recorded |
| Charge and collision | 1,280 gauge; 18,000 box/circle/item; 8,192 rotated-box cases |
| Game math | 16,528 angle/rotation/Hermite comparisons |
| Animation execution | 12,638 synthetic comparisons; 1,818 real scripts and 3,663 sprites, 120,511 state comparisons |
| Simulation input capture | 10,000 frames, including unsigned counter wrap and original repeat behavior |
| Replay input playback | All 23,993 demo input frames, initialization, pause flags, FPS samples and accelerated dialogue callbacks |
| Replay recording | Original byte order across 4 allocation chunks, CPU input, pauses and 3 ending frames |
| ECL resources and variables | All 17 resources / 173 subroutines; 83,520 variable reader/writer checks |
| ECL execution | Arithmetic/control 3,456 frames; calls/4 parallel contexts 150; interpolation 2,016; movement 2,520 |
| ECL firing | 2,592 frames, 18 opcodes, exact emission payloads and periodic firing |
| Native ECL callbacks | All 12; 360 executions, including 8,576 bullet states on both playfields |
| Bullet patterns | 720 original creation comparisons, all 9 patterns, initial velocity and RNG |
| Bullet extra programs | 17,136 activation/update comparisons, including turns, acceleration, bounce, wrapping, delay and child emissions |
| Bullet manager | 720 frames of original manager update, including animation phases, attack/player collision boundaries and ordered draw lists |
| Bullet animation integration | All 23 original etama templates; 368 creations including pool exhaustion; 45 integrated frames; 82,730 full ANM state comparisons |
| Lasers | 615 creation/update comparisons, 48-slot exhaustion and collision windows; 59,040 full ANM states |
| ECL laser control | 384 frames; creation, reference lifetime, direction, position, length and stop/reset commands |
| Enemy animation selection | 2,592 frames; common/character animation sets, extra layers and movement poses |
| Enemy script state | 2,017 comparisons of life, timeout, drops, flags and animation controls; 480 additional arithmetic/RNG cases |
| Enemy timelines | 11,206 state comparisons and 435 spawn requests, all 8 original schedules plus waits/events/mirroring |
| Enemy creation | 707 comparisons of template initialization, both creation paths, inherited locals, common/character scripts, failure and 128-slot exhaustion |
| Enemy trail preparation | 224 original instruction/vertex UV/color comparisons |
| Cross-enemy/scene commands | 60 cases of original ECL execution, cross-Boss calls, child creation, cancellation and recorded scene outputs |
| Enemy defeat | 1,092 cases including chain rewards, actual charge-gauge updates, drops and 132 cross-field transfers |
| Combo rewards | 1,236 checks / 412 scenarios, score thresholds, resets, flushing and 399 transfers |
| Character attack meters | All 16 callbacks, 7,360 checks across ranks 0–22 and both sides, 18,528 attack requests |
| Character capture | All 16 shape predicates, 39,248 checks; 24,576 captured-spirit motion/integration frames |
| Enemy frame lifecycle | 3,552 original frame comparisons, including 480 integrated timeline frames; freeze, capture expiry, damage, death-script entry, targeting and ordered draw lists |
| Character attacks | All 27 attack callbacks, 256-slot allocation, lifecycle, limits and cleanup; 571 creations, more than 89,000 state comparisons, custom wave/field vertices and Eiki conversion in both bullet pools; full queue draw ordering is also compared |
| Attack areas | 1,069 creations and 3,600 collision/damage scenarios; allocation limits, delayed growth, reflection, interval damage and damage caps |
| Player shots | All 16 SHT resources; 21,200 comparisons, 12,240 firing calls, 1,920 damage queries and 1,280 draw passes, including native character callbacks and integrated attack areas |
| Shooting and charge controls | 24,576 comparisons across all 16 resources; both control modes, release thresholds, full-charge timeout, quick bombs and protection |
| Incoming player hazards | 4,480 registrations and 5,760 actual/predicted collision queries; first-hit order, pool reuse, invulnerability and tangent boundary exclusion |
| Player items | 24,300 comparisons, all four rewards, 2,442 cross-field transfer payloads; original loop-counter behavior, gravity, collection and drawing |
| Player life | 19,712 comparisons, all 16 characters; damage, Tewi automatic defence, fatal hit, recovery, entrance, invulnerability and shock |
| Computer opponent | 11,712 frames across all 61 CPU policies; actual hazard probes, evasion, charge release, target tracking, timers and RNG |
| Integrated player | 4,704 combined state comparisons and 192 draw passes; both sides and all 16 SHT resources, round reset, actual AI/motion/shots/areas/damage and combo timers |
| Effects | All 38 effects; 49,159 comparisons, 351 creations and 1,958 geometry/draw checks, including pool exhaustion, partial creation, reserved slots, recycling and cross-field transfer arrival |
| Match rules | Rank initialization, retries, score smoothing, extends and time rewards across all modes and difficulties; score accumulation retains the original units of ten points |
| Spell/Boss controller | All nine original variants, reentrant ECL announcements, level/statistics, counter-Boss flags, freeze windows and ordered banners |
| Dialogue | All 30 MSG resources, 862 scripts, 74,382 state checks and 2,048 weighted victory selections; text decoding, waiting, skipping and portrait order |
| Stage selection | 19,680 comparisons; all 293 route rows and 16 versus profiles, route weights, opponent reuse prevention and CPU retry policies |
| Match scene | 120 scenarios comparing front overlays, boss indicators, result caps, life-based retries, CPU policy and stage/game-over/ending transitions |
| Background scripts | All 15 STD resources; 49,584 comparisons, including model initialization, camera/Hermite curves, loops/labels/waits, sway, fog, overlays and freeze timing; final GPU rendering remains separate |
| Sprite drawing | 9,000 cases comparing original CPU vertices, pixel rounding, anchoring, tint, depth/blend selection and submitted batches |
| Graphics math and cameras | 10,904 comparisons of original scalar normalization, its inclusive tolerance boundary, matrix multiplication, rotations, camera matrices and projection |
| World sprites | 1,800 cases comparing world transforms, billboards, matrix/UV invalidation and vertex payloads |
| Background models | All 15 STD files, 2,400 draw passes, 12,824 world sprites, 14,886 billboards and 3,273 ribbons; culling, fog and the original shared projection input are retained |
| Background composition | 1,536 comparisons of clears, tint saturation, fog/depth changes, four model layers, overlays and Boss backgrounds on both fields |
| Animation resource ownership | All 59 ANM files / 870 texture entries; 3,636 original animation-start comparisons, cache reuse and cleanup after an injected allocation failure; GPU texture decoding is a boundary |
| Bullet and laser drawing | 256 comparisons of both fields, six ordered lists, both bullet pools and five phases; laser glow gates, positions, colors and retained state |
| Enemy drawing | 1,024 comparisons of the four enemy layers, attached sprites, trails, CPU vertices and ordered submissions |
| Integrated battle fields | All 16 characters across eight pairs / 12,000 frames, original ANM/SHT/ECL resources, shared RNG, shots, hazards, captures, cross-field attacks and forced Spell/Boss attacks |
| HUD | 1,204 state checks over 600 frames, all 63 HUD slots and two portraits, score/life/charge/survival notices, ordered drawing and four wipe triangles |
| Screen effects | All eight original modes, 3,197 state checks and 2,690 draws; fade/flash/release timing, pause gates, shake RNG, camera offsets and rectangles |
| Integrated match world | 7,470 frames over five Story/Extra/Versus scenarios, real world constructors, CPU and human input, dialogue, backgrounds, HUD, forced Boss attacks, ten round endings, retries and result transitions |
| Frame-timer boundary | 3,500 comparisons, including the inclusive 0.99 slowdown threshold |
| Texture pixels | All 870 original texture creation requests and 865 embedded uploads, including the four textures whose embedded dimensions differ from their ANM declarations |
| ASCII and score popups | 723 checks, 256 text entries, 220 popup creations, 100 update frames, complete glyph VMs, ordered drawing, digit order and proximity opacity |
| In-game menus | 4,756 checks across 70 scenarios, all pause/continue/match-end states, original animations, input delays, sound requests and transitions |

Comparison results and fixture hashes are in `artifacts/cpp/verification`.
The archive evidence is also retained in `reference/archive-manifest.json`.
`npm run cpp:build` builds the fixture; `npm test` runs routine comparisons.
`npm run test:archive-oracle` reruns all original archive decompression and takes
about 15 minutes on the development machine. Routine tests compare the complete
archive against hashes already established by that original execution.

## Numeric behavior

The original CRT starts at 53-bit precision, but creating its D3D device without
FPU_PRESERVE changes gameplay arithmetic to 24-bit precision, round-to-nearest.
The development oracle therefore uses FPCW 0x007f. The C++ game uses ordinary
single-precision arithmetic with contraction disabled, not an emulated x87
register stack. Trigonometric conversion points are verified separately.
The finite input ranges in these tests do not prove every possible float value.

## Integrated application checkpoint, 2026-09-20

- Complete routine suite: **91 passed, 0 failed**, 708.2 seconds; `artifacts/cpp/regression-final.log`. Core hash `6b5bdc1901adca81c67f4dcad74bf1a067cdba72267ff7b54027b4364324bf2c`.
- All three original demonstration streams: **17,108 unforced frames / 17,111 checks**, original constructor and update chain versus authored GameSession. RNG, scores, players, combo, scene, dialogue, background and HUD states compared.
- **28 Story/Extra character paths / 252 stage transitions / 28 endings**, 105,715 frames. Victories are explicitly forced for path coverage; this is not evidence of battle-outcome equivalence for every route. Continue, retry, saved replay stage selection and staff return are covered.
- Six freshly recorded C++ gameplay windows reproduce their own native-format replay state; the real browser Replay/save/name menus are also exercised. Browser replay scheduling accelerates original dialogue updates without dropping simulation ticks and matches single-tick execution at the same replay frame.
- PlayerRecords, GameConfiguration, all title/character/difficulty/options/key-config/ranking/replay/music screens, all ending/staff resources, native date format, demo interruption and Exit/restart are integrated.
- Native WAV effects use the original sound definitions and request priority; 19 OGG tracks retain source PCM frame counts and loop points. OGG is lossy. The platform uses miniaudio mixing into SDL3 audio streams.
- SDL_ttf prewarms Japanese dialogue, music comments, spells and ending glyphs. Text is encoded through the original CP932 table and color-blend table. Its rasterizer is not a byte-for-byte Windows GDI replacement.
- Browser imports, exports and IDBFS persistence use `/savesth09`; resource caching is separate and SHA-256 verified. An inactive Network UI cannot reset single-player progress. Resource downloading retries transient network errors up to three times, validates before caching, and waits for concurrent downloads to settle before a new start attempt. Injecting two consecutive 502 responses still completes loading. Original TH08 browser caches and saves survive the tested root Service Worker migration.
- Touch auto-fire generates actual press/release input; hold-charge, quick attack, focus, pause, menu gestures and relative movement use the same shared touch foundation as TH08/TH10, with TH09 rules.
- Network is a browser-specific two-player WebSocket room transport with 6-frame input lead, ordered input, bounded queues, room isolation and periodic state checks. Two independent browser contexts match simulation hashes, including pause/resume and a one-sided local save. Completed matches release synchronization before each user's separate replay save. Network is not wire-compatible with the original Windows protocol.
- Public serving is a strict manifest allowlist with ranges, ETags, COOP/COEP and CSP. Test entry points are not exported by the release build. Source/oracle files, the original executable, process logs, saves and tunnel tokens are not served.

## Rendering and performance

`GameWorld / Player / Enemy / BulletManager → AnmRenderer CPU vertices → ZunGraphics → GraphicsDevice → shared portable/sdl Renderer → SDL3 GLES / Emscripten WebGL2 → Canvas`.

No Direct3D DLL or Windows ABI is in this path. Original ANM pixel-format numbers are decoded at the asset boundary. Transparent draw order is retained. Batches replace GL buffer storage; fixed vertex layouts, render states and uniforms are cached. The shared renderer prewarms its 33 shader variants.

Stage resources decompress in 128 KiB steps; future route candidates are prepared without consuming RNG or invalidating current sprites. Each warm step parses one file or uploads one texture. On the development host hardware GPU, a nine-stage test measured 3.6–7.7 ms maximum transition handling, zero GPU pixel readbacks, zero bufferSubData updates and no new shader compilations after startup. Its forced stage ends and desktop GPU are identified in `artifacts/sdl3/browser/performance/report.json`. These measurements are not physical-phone performance claims.

## Verification limits and remaining external testing

The game is playable; all named game paths above are implemented. Finite state comparisons cannot prove every possible input/float or pixel-identical behavior on every driver. Full-length external Story/Extra replays beyond the shipped demos, original-versus-browser visual captures of every character/background and physical Android/iOS soak tests remain useful acceptance work. The current target retains `completeGame: false` rather than claiming absolute 1:1 proof.

Browser adaptations include touch input, WebSocket networking, IDBFS saves, SDL_ttf glyph rasterization, OGG music and browser-controlled fullscreen. Original game data and fonts remain separate assets. Generated decompiler candidates and the native oracle remain development evidence only.

## Rebuild and delivery

See `../README.md`. Keep `th09_web/cpp`, `sdl-runtime`, `scripts`, `tests`, `reference`, asset preparation inputs, pinned target, dependency lock and `../portable/sdl` plus `../portable/input`. Oracle dependencies currently use the sibling TH08 Unicorn package; the WASI/Emscripten SDK and browser test helper paths are documented. The self-contained playable release is `artifacts/sdl-release`; it includes its own Node server, room relay and ws package. It has no dependency on a running original executable or the development oracle.

## Public deployment verified

2026-09-20 19:10:59 Asia/Shanghai: version `9a02d9cc547747a3810c7b77`, Cloudflare origin `127.0.0.1:3007`. The public hostname downloaded all resources, entered Story battle, played audio and successfully upgraded the WebSocket room connection. The desktop browser in a mobile-sized viewport reported 60 FPS; physical-phone performance remains unmeasured. Public game errors: zero. Cloudflare analytics injection was blocked by the intentionally same-origin script policy and is logged separately. See `artifacts/sdl-release/verification/deployment.json`.
