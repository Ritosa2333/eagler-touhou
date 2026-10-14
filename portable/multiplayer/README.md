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
Player lifecycle, shots, Bomb stock/fragments, lives/life fragments and Power have stable
per-seat owners. Score, total/chapter Graze, point value, point-item counts,
collection statistics and chapter rewards use one team owner. Each pilot's
eligible graze contributes once, including multiple pilots grazing one bullet.
Chapter score and point-value rewards are calculated once from the team statistics.
Life fragments and Extend progress are shared: each eligible event adds exactly
one fragment, including ghosts banking reserves. Chapter bonus thresholds are
1,250,000 for 2P and 1,500,000 for 3P by default. The room owner can set
500,000 to 5,000,000 in steps of 250,000 before starting. At 100% defeat rate,
the required Graze count is the threshold divided by 5,000. The value is
synchronized to players/spectators and retained in the Replay. Three fragments grant each uncapped pilot
one spare life in Legacy; Extra requires five for the first five team Extends,
then stops fragment-based Extends. Each pilot remains capped at eight spare
lives. Extends do not reactivate ghosts. See
[the current economy and teammate-action contract](../../docs/TH15_MULTIPLAYER_ECONOMY.md).
Whole-life and whole-Bomb pickups go to their collecting pilot.
Cooperative lives start and respawn with two Bombs per pilot (zero in the
Bomb-disabled challenge); fragments remain personal and survive respawn.
Stage survivors retain their earned stock. Retail single-player still uses
three base Bombs. Boss/midboss effective damage is scaled after native spell
and Bomb protection: 2 viable pilots use 3/4, 3 use 2/3, one uses native damage.
Temporary dying/respawning pilots remain viable; exhausted/ghost pilots do not.
Enemy HP, phase thresholds and ordinary-enemy damage stay in retail units.
A per-enemy fixed-denominator carry retains fractional damage as seats leave.
Each pilot's death/respawn reduces only their own damage before pooling.
Shared ECL counters and chapter/stage deaths include each committed pilot miss.
Spell startup tracks the Bomb owners that triggered its protection. Existing
Reisen shields do not protect a new spell; shields activated or hit during
the first 60 frames retain retail startup protection until that owner's
shield ends. Remote damaging Bombs and mixed Reisen teams use the same owner
tracking, so an unrelated older shield cannot prolong team startup reduction.
Non-Reisen teammate protection never shortens existing invulnerability.
Special near-player bullet rules consider every operable pilot and apply once
per bullet even when pilots overlap. References include current TH06/07 Eagler,
TH08 MP 0916231, TH10 MP 288ea8d and TH11 MP d62e3f1; title-specific damage caps,
Power values and spell formulas remain TH15's own.

The HUD repeats TH15's original life/Bomb icons, labels, fragment bars and
separators for each seat, following TH11's grouped presentation. All asset
sizes, the panel background and split Power typography retain the retail
appearance; the local seat number is highlighted. Shared point value and
chapter/total Graze appear once below the personal groups. Managed ANM widgets
and the original text queue consume no battle or visual RNG.
Small/large P and Full Power items prefer the nearest eligible collector who
still needs Power; eligibility includes the collector's own POC line, Bomb
attraction and pickup bounds. Owners are reconsidered while homing and after
death. Small/large P retain their authored types on spawn and in the field,
even when the whole team is full. The actual collector's own Power selects
the retail Power gain or full-Power score reward, credited once to team score;
P does not become a blue point item or increment point-item statistics.
ECL Power conditions read the minimum Power of the same
non-exhausted team, so a full primary pilot cannot suppress a teammate's P
drops. Full Power remains an F item and only fills its collector;
at full Power its retail bonus increases the shared point value once.
Cancel point-value items use normal collector eligibility rather than Power
priority. Point pickups use the shared value and the collecting character's
native POC/height calculation. Native drop quantities are retained.
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
Gameplay ABI version 16 rejects older incompatible economy generations.
Display-only local-hitbox filtering does not change that gameplay version.

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

Exhausted pilots remain visible as translucent original character sprites.
Their bounded automatic drift follows TH11, using an independent per-seat
random stream. Ghosts ignore movement, shot and Bomb input; death and ghost
states are excluded from bullets, all three active laser kinds and enemy
contact. They cannot graze, collect items, deal damage or become ECL targets.
The final-death transition clears options, focus and barrier effects.

This experiment implements Legacy only. Pointdevice retry/checkpoint files,
THPrac and multiplayer drop multipliers are not implemented. Ghost rescue,
life/Power transfer and teammate opacity follow the current economy contract.
Small P grants 0.02 in 2P and 0.03 in 3P; large P remains 1.00. Power gifts
require at least 2.00P and leave at least 1.00P. Stock exhaustion disables that
pilot; the game-over menu opens after all pilots exhaust their stocks.
Non-Reisen bombs give teammates 120 startup frames and refresh protection to
at least 40 frames during active updates. Reisen gives 30 frames on startup
and every shield-layer break, without sharing persistent shields.
Only the selected viewpoint's hitbox is drawn, including native fade-out.
Life rescue and transfer remain available during dialogue. Pauses and inactive
run transitions still block them. Following TH11, multiplayer failure/clear
menus offer only Return and Replay Save; Continue/Retry and their shortcuts
cannot reset one seat inside the current generation. P1 owns native menu
input, while dialogue input remains shared. Return ends the run in the lobby. Touch-free joystick
preferences use directional joystick inputs in this mode, so queued movement
never depends on a sender's earlier simulated position.

Acceptance still requires long full-stage/Extra play, remote-network and
physical touch-device checks. Local protocol or headless tests do not establish
those results.

Selected-view rendering also applies to player Graze rings/sparks and each
bullet's per-seat Graze tint/jitter. Shared Graze, bullet collision/rewards and
the authoritative aggregate remain unchanged. Normal clears run the native
ending and score/replay result owners on confirmed inputs before returning to
the lobby. Result inputs belong to P1; ending timing ignores local unlock
history. Native named saves write the complete multiplayer input archive.
