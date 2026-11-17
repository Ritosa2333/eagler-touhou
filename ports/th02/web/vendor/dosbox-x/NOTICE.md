# DOSBox-X / js-dos runtime for TH02

The local runtime is copied from the workspace's `th04-coop-lab`
DOSBox-X implementation, pinned to `emulators@8.5.2` (2026-10-12).

- Package: https://www.npmjs.com/package/emulators/v/8.5.2
- Emulator source: https://github.com/js-dos/emulators/tree/6bbf0925d221a7304ea8e0dda1e89b8dc01569b2
- DOSBox-X source: https://github.com/js-dos/dosbox-x
- Original project: https://github.com/joncampbell123/dosbox-x
- GPL-2.0 license: `web/vendor/dosbox-x/LICENSE`.

`emulators.js` is an unchanged upstream asset. `dosbox-jspi.mjs` is the
TH04 JSPI factory/dispatcher adaptation. `wdosbox-x-fast.wasm` is the TH04
local core with held modifiers excluded from artificial timer repeat.
Its SHA-256 is recorded in `web/local-patch.json`. The exact transformation
and pinned source archive are retained in the sibling TH04 project:
`tools/fix_modifier_repeat.cjs`, `vendor-source/emulators-8.5.2.tgz` and
`vendor-source/emulators-8.5.2/`.

TH02 uses `web/dosbox-disk.js` to extract HUMAROKU from a verified copy of
the user's disk, mount it under internal DOS and run GAME.BAT via the
FASTBOOT.BAT wrapper, with the game's PMD driver and PC-98 FM board86.
`web/local.js` supplies local two/three-player inputs to the native patch.
Local two/three-player cooperation and network rooms both configure a verified
private WEBSTART.COM before boot, entering MAIN at the selected stage without
OP's difficulty/shot menus. Browser settings supply each seat's shot type and
initial resources. Multiplayer focus movement is fixed at 50%; network rooms
freeze the host-owned focus and targeting switches before synchronized play.
Direct multiplayer boot removes all six validated `zun zunsoft.com` logo
branches from the private FASTBOOT.BAT only. ZUN initialization/resident setup,
MMD, PMD selection and cleanup remain intact. The DOSBox-X `fastbioslogo`
setting removes the BIOS splash delay. The local status scanner uses the
core's conventional-RAM base instead of rotating through the entire WASM heap.
The interpreter uses Simple CPU and JSPI. Newer Edge/Chrome is required.

The network runtime now imports the pinned 2026-10-15 native scheduler core
from `th04-coop-lab/build/native-scheduler/runtime-tablet`. The source core
SHA-256 is `318af02f807f494613083c444a35c1abcde2823a9ad2c81a160e785d8d7d3948`.
Its DOSBox-X revision is `788fde99d3d451eab3621009186e5abe516f2b9c`.
`tools/disable_th04_idle.mjs` disables both TH04 guest-address gates.
`tools/build_th02_idle_candidate.mjs` adds TH02's guarded four-instruction
VSync wait fusion, preserving decoder instruction budgets and event boundaries.
`tools/build_network_runtime.py` then derives a new complete write-tracked core.
Native scheduling, guarded synchronous interpreter, memcmp and planar pixel
conversion are retained. TH02 uses its own disk/input/state adapter and an
independent `th02-rollback/6` protocol.
GPL sources, pinned toolchain, patch scripts and license remain in the sibling
TH04 development project; network deployment itself is self-contained here.
`web/netplay/runtime.json` records the actual deployed resource hashes.

TH02 multiplayer RGB art extends all three deployed cores (local JSPI,
plain rollback and tracked rollback). `tools/build_player_art_core.mjs` and
`tools/player-art.wat` add per-page sprite provenance at native GRCG writes
and apply garment/ghost RGB during scanout, before the native text layer.
`tools/build_player_art_runtime.py` regenerates write tracking for these stores.
The local JSPI wrapper also exposes its native exports for art initialization.
The original native palette and protected body pixels remain authoritative;
the provenance heap is restored with emulator rollback state. The source
baseline hashes and transformations are retained in the TH02 development
workspace under `build/art-baseline-20261016` and `tools/`.
`web/assets/th02-player-art.json` records all 33 source image hashes. All 10
body frames come from TH02's green-Genjii set in `th02-player-colors`; only
the three ghost PNGs come from `th05-four-player-colors/reimu`.
Each seat also packs a HUD ofuda frame extracted from TH02 MIKO16.BFT frame 4,
recolored with the same seat palette. `hudShot` records its source/image hashes
and the separated bullet/option layout. It uses native tiny-sprite rendering
with the same provenance tracking; no browser overlay is involved.
Actual native main/option projectiles and hit animations use the existing
per-shot owner records to tag both native blitters. Reserved 16-bit tags
60000..60002 select per-seat RGB palettes in the same art table. Shapes,
movement, collision, damage and scoring remain native; white/neutral details
and hardware palette fade endpoints are preserved. All provenance writes
participate in tracked rollback, including shared-shot-pool slot reuse.

Network play additionally applies validated, private in-memory MAIN edits to
observe PMD music commands and disable native song loading/playback. FM sound
effects still use the original driver. `tools/build_local_bgm_assets.py` extracts
the 15 original TH02 songs and renders looped Opus assets using the workspace's
PMD renderer (TH04 tool, or the same-source TH05 build) and ffmpeg/libopus.
`web/bgm/manifest.json` records source/audio hashes and loop points. Browser
Web Audio owns music playback time; confirmed game steps only select tracks
and change their fade/volume. Original source disks are never overwritten.

The original game assets are supplied by the user's disks; their rights
remain with their respective owners.

Standalone single player uses separate TH02 disks. Its 16-bit status, optional
focus/marker and touch hooks preserve all original resource/score/collision
routines. The small resident INT 60h bridge derives from the workspace TH05/TH04
PMD bridge: it masks music parts 0..14 while retaining FM effects part 15 and
original PMD timing. Browser Web Audio plays the existing TH02 Opus assets.
The bridge is installed only in the private running copy of GAME.BAT; original
source disks and cooperative native layouts are unchanged.
