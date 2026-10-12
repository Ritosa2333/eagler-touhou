# TH15 Legacy multiplayer experiment

Branch: `experiment/th15-multiplayer`, based on upstream TH15 `eagler`
`09affbe`. References: TH10 `eagler-mp` `288ea8d`, TH11 `eagler-mp`
`d62e3f1`, TH09 `eagler` `edb05b2`, common `th11-multiplayer` `e02347f`.
TH15 upstream tracking is `main` at
`fe113b6f9cbcb676b15dfd95cfd5916c4d5de95e`; the Eagler base is
`09affbe418f36dce9889f8294e89f1332f57f329`.
The matching launcher changes are on `experiment/th15-multiplayer-launcher`.
Its base is `eb27a88`. The network submodule extends common `e02347f` with
TH15 spectator timing identity, pinned at
`dd93c944ce2f2830e99be9c8dba7ff2ac645f548` on
`experiment/th15-multiplayer-common`. This dependency commit is local; publish
that common branch before sharing a remote checkout of the title branch.
Canonical TH15 and launcher checkouts have not been promoted or overwritten.

Two or three pilots independently select Reimu, Marisa, Sanae or Reisen.
Player lifecycle, shots, Bomb stock/fragments, lives and Power have stable
per-seat owners. Score, total/chapter Graze, point value, point-item counts,
collection statistics and chapter rewards use one team owner. Each pilot's
eligible graze contributes once, including multiple pilots grazing one bullet.
Chapter rewards are calculated once from the team statistics.
Life fragments form a team pool: three grant each pilot one life in Legacy;
Extra requires five for each of its first five Extends, as in retail TH15.
Each pilot remains capped at eight spare lives. A capped pilot still contributes
while a teammate has room; when everyone is capped, a fragment becomes only
the collecting pilot's Bomb fragment. Extends do not reactivate ghosts.
Whole-life and whole-Bomb pickups go to their collecting pilot.
The stage, enemies, projectiles and spell capture are shared.
Aimed emissions select the nearest operable pilot; ties use seat order.
Every participant simulates the same confirmed input rows at 60 Hz.
`eagler-common-netplay` owns transport, calibration, ACK repair, frame-zero
barriers, spectator publication and the input replay codec. Prediction and
rollback are disabled. No world snapshots are taken for network ticks.

The ordinary pinned common submodule is retained for the existing THPrac
build. The second submodule pins the network dependency separately. Both
submodules must be initialized after checkout:

```powershell
git submodule update --init --recursive
$env:TH15_EMSDK='C:/path/to/emsdk'
node th15_web/scripts/build-sdl-application.mjs --release --multiplayer
$env:EAGLER_FONT_ROOT='C:/path/to/measured/th15/fonts'
node portable/package-eagler.mjs --multiplayer
```

Multiplayer output is `build-eagler-multiplayer`, with product identity
`th15mp`. Ordinary output remains `build-eagler`. Saves are `/savesth15mp`
and replays are `replay/th15mp_01.rpy` through `th15mp_99.rpy`. Each recording
selects a free slot, then replaces slot 01 if all slots are occupied. Replays
carry the complete seat setup, seed, build identity and confirmed input rows.
Playback requires the same executable build and applies no second input delay.
Retail one-player replays are not accepted by the multiplayer viewer.
Gameplay ABI version 2 rejects the earlier per-seat economy recordings.

For shared launcher development, point `EAGLER_WORKSPACE_ROOT` at the repository
workspace and provide the multiplayer worktree at
`worktrees/th15-multiplayer` (a local junction may point at the experiment).
The matching launcher catalog declares the room and loadout policy. Its
existing directory, lobby, runtime preparation and exit flow remain shared.

```powershell
$env:TH15_TEST_ARCHIVE='C:/path/to/Japanese-1.00b/th15.dat'
node portable/multiplayer/native-test.mjs
node portable/multiplayer/network-test.mjs
node portable/multiplayer/module-test.mjs
$env:TH15_LAUNCHER_ROOT='C:/path/to/matching/eagler-touhou'
node portable/multiplayer/browser-test.mjs
```

The headless C++ fixture (WASM under Node) executes the production player, bomb, ECL, animation,
enemy, item, bullet and laser systems using retail DATA. Its presentation
endpoints are headless acknowledgements; it does not certify rendering.
It checks 3,600 retail gameplay ticks for seat-independent determinism, all four
characters, separate controls and bombs, pooled damage, nearest ECL aim and
item ownership, remote collisions, chapter rewards, next-stage resource
retention and all-player game-over. Network fixtures cover 2P/3P with D=0/2/9,
reordered holes, capture ownership, generation fencing, build mismatches,
all-seat recording and malformed replay rejection. The shared startup fixture
preserves TH08/10/11 timing bytes while adding TH15.
The browser fixture uses production WASM and shell, the actual shared relay,
retail DATA and measured fonts. Reports and screenshots stay in ignored
`th15_web/artifacts/sdl-multiplayer` directories. Retail DATA is not included.
The browser fixture enters real shared lobbies, checks the frame-zero barrier
and missing-input wait, admits a 2P spectator, then imports and plays both
2P/3P recordings through the offline viewer menu and hosted exit lifecycle.
Ordinary THPrac compiles/packages separately without multiplayer exports; its
existing typed-owner gameplay regression also passes.

This experiment implements Legacy only. Pointdevice retry/checkpoint files,
THPrac, ghost rescue, Power transfer, multiplayer drop multipliers and teammate
opacity adjustments are not implemented. Stock exhaustion disables that pilot;
the game-over menu opens after all pilots exhaust their stocks. Non-Reisen
bombs protect teammates for 180 frames; Reisen's shields remain personal.
All restart/continue/exit choices return to the lobby. Touch-free joystick
preferences use directional joystick inputs in this mode, so queued movement
never depends on a sender's earlier simulated position.

Acceptance still requires long full-stage/Extra play, remote-network and
physical touch-device checks. Local protocol or headless tests do not establish
those results.
