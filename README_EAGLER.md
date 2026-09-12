# TH06 Eagler Runtime

This document is the Eagler-specific engineering guide for the `eagler` branch.
The root [`README.md`](README.md) remains focused on the upstream portable
project; browser protocol, Host integration and Eagler-only behavior belong
here.

## Scope

This branch keeps the TH06 game Runtime separate from the `eagler-touhou`
Host. The Runtime owns game simulation, rendering, audio playback and its
Emscripten filesystem. The Host owns product UI, package selection, Runtime
lifecycle, save/replay tools, touch UI and multiplayer lobby/signaling.

The hosted Runtime protocol is `eagler-touhou/1` and the game identifier is
`th06`. Hosted messages are accepted only from the same-origin parent frame.

## Web builds

The normal playable Web build may use legally obtained game resources from the
ignored local asset directory. Do not commit or redistribute generated game
DATA bundles.

For source-only validation, the repository provides two Emscripten presets:

```sh
git submodule update --init --recursive

emcmake cmake -G Ninja --preset web-ci-normal
cmake --build --preset web-ci-normal --parallel

emcmake cmake -G Ninja --preset web-ci-netplay
cmake --build --preset web-ci-netplay --parallel
```

Both CI profiles use `TH_EXTERNAL_ASSETS=ON`; the ordinary profile disables
multiplayer while the netplay profile enables the multiplayer Runtime. The
GitHub workflow verifies that source-only CI does not emit `.dat` or `.data`
game archives.

## Host / Runtime protocol

Messages use an object with at least:

```text
protocol = "eagler-touhou/1"
game     = "th06"
command  = <command name>
request  = <optional request id>
```

Request-bearing commands receive a success/error reply. The shell validates
payload shape and bounds before installing Host state.

### Host commands

| Command | Purpose |
| --- | --- |
| `configure` | Install Runtime options, Runtime resources and music mode/resources before launch. |
| `resources` | Install additional package-owned Runtime resources. |
| `launch` | Start the game exactly once after configuration. |
| `keyboard` / `keyboard-clear` | Bridge browser keyboard state when SDL delivery is incomplete. |
| `touch-controls` | Update fire/focus/bomb/escape/virtual-stick state and touch sensitivity. |
| `direct-touch` / `touch-cancel` | Forward direct touch coordinates and lifecycle cancellation. |
| `thprac-mouse` | Forward practice-overlay mouse input in canvas coordinates. |
| `list` / `read` / `write` / `remove` / `sync` | Operate on the Runtime-owned persistent save tree. |
| `retry-music` | Retry failed hosted music resources. |

The Runtime emits lifecycle and health events including `first-frame`,
`runtime-info`, `frame-health`, `audio-health`, `exit`, `error`, `transfer`,
music completion/error events and `thprac-session` updates.

## Runtime options

`configure.options` is copied into `Module.eaglerOptions` after validation.
The product-facing option groups are:

- presentation: optional 60 Hz presentation cap;
- touch: enable state, direct/virtual-joystick movement, sensitivity, focus
  mode, unlimited/direct movement, bomb zone, double-tap bomb and visibility
  helpers;
- practice: `thpracEnabled`, locale and the live
  `eagler-touhou/thprac-session/1` session;
- audio: OGG stream/full-decode selection plus MIDI/WAV/OGG/none music mode;
- replay viewer intent;
- multiplayer: relay URL, local slot, 2/3-player count, spectator state,
  shared seed/difficulty/loadouts and server-provided ICE servers.

Debug harness flags are test-only and are not a product protocol extension.

## Input model

SDL remains the primary keyboard/gamepad path. The hosted shell mirrors a
small browser keyboard bitset for WebViews, IME paths and external keyboards
whose physical key events do not reliably reach SDL. C++ merges that bridge
with normal SDL input rather than replacing SDL ownership.

Touch supports direct movement and virtual-stick modes. The Host can update
live fire/focus/bomb/escape state without restarting the Runtime. Direct-touch
coordinates are normalized through the actual canvas rectangle before being
passed to the C++ touch bridge.

On Web, transient canvas/iframe focus loss is not treated as application
backgrounding. Real background lifecycle remains owned by SDL background
events and the shell/page lifecycle; touch state is cancelled on focus loss to
avoid stuck input.

## Persistent files

Normal Runtime persistence is mounted at `/savesth06`; the multiplayer Runtime
uses `/savesth06-multiplayer` so the two products do not share mutable save
state accidentally.

The Host accesses these files only through the protocol commands above. Writes
and removals are followed by an explicit filesystem sync. During IDBFS restore,
the shell suppresses `autoPersist` writes created by population itself so a
partial restored tree is not written back over IndexedDB before restore has
completed.

## Practice integration

The live Web practice contract uses exactly
`eagler-touhou/thprac-session/1` with `game = "th06"`. The Runtime, Host and
portable adapter represent one live practice owner and must transition that
state together at normal Start, Practice and replay lifecycle boundaries.

Historical experimental replay-sidecar schemas are not part of the live Host
protocol. New code should not add a second practice-session owner merely to
work around lifecycle bugs.

## Multiplayer implementation

The multiplayer build supports 2- or 3-player browser sessions plus spectators.
Simulation starts only after the transport route is decided. WebRTC is the
preferred peer path; the shared Host relay provides signaling and an emergency
WebSocket relay path. TURN/STUN configuration is delivered by the Host through
validated ICE server objects.

The shared lobby/signaling/relay service belongs to `eagler-touhou`, not this
Runtime repository. Runtime code owns deterministic game/session behavior and
the browser peer transport adapter; server deployment policy remains Host
infrastructure.

## Repository boundaries

Do not commit:

- original TH06 game archives or generated playable DATA bundles;
- local build directories, smoke-test output or browser profiles;
- dirty vendored-submodule working trees as accidental pointer changes;
- Host/server implementations that are already owned by `eagler-touhou`.

Changes to the hosted protocol should be coordinated with the Host contract and
covered by source/contract tests in both repositories.
