# TH07 Eagler Runtime

This document is the Eagler-specific engineering guide for the `eagler` branch.
The root [`README.md`](README.md) remains the portable-project overview; hosted
protocol, Eagler behavior and product integration are documented here.

## Scope

TH07 runs as a game Runtime under the `eagler-touhou` Host. The Runtime owns
simulation, rendering, audio playback and its Emscripten filesystem. The Host
owns launcher/product UI, packages, Runtime lifecycle, save/replay tools,
touch UI and multiplayer lobby/signaling infrastructure.

The hosted Runtime protocol is `eagler-touhou/1` with game identifier `th07`.
The shell accepts hosted messages only from the same-origin parent frame.

## Web builds

For repository CI and source-only validation:

```sh
git submodule update --init --recursive

emcmake cmake -G Ninja --preset web-ci-normal
cmake --build --preset web-ci-normal --parallel

emcmake cmake -G Ninja --preset web-ci-netplay
cmake --build --preset web-ci-netplay --parallel
```

The presets build with `TH_EXTERNAL_ASSETS=ON`, so no original game resources
are required and the result is a compile/link artifact rather than a playable
asset bundle. CI rejects source-only output that contains `.dat` or `.data`
archives.

## Host / Runtime protocol

Every hosted message carries:

```text
protocol = "eagler-touhou/1"
game     = "th07"
command  = <command name>
request  = <optional request id>
```

### Host commands

| Command | Purpose |
| --- | --- |
| `configure` | Validate and install Runtime options and music configuration. |
| `resources` | Install package-owned Runtime resources. |
| `launch` | Start the configured Runtime once. |
| `keyboard` / `keyboard-clear` | Bridge browser keyboard input when SDL alone is insufficient. |
| `touch-controls` | Update fire/focus/bomb/escape, virtual stick and sensitivity. |
| `direct-touch` / `touch-cancel` | Forward/cancel normalized direct-touch input. |
| `thprac-mouse` | Forward practice UI mouse interaction. |
| `list` / `read` / `write` / `remove` / `sync` | Access Runtime-owned persistent files. |
| `retry-music` | Retry failed hosted audio resources. |

The Runtime emits startup, first-frame, renderer, frame/audio health, transfer,
music, error, practice-session and exit events for Host lifecycle management.

## Runtime options

Validated options are installed into `Module.eaglerOptions`. Product-facing
groups include:

- high-refresh presentation and optional 60 Hz presentation limiting;
- direct-touch and virtual-joystick movement, sensitivity, focus mode, bomb
  zone, double-tap bomb and related visibility helpers;
- OGG streaming/full decode plus hosted music-mode selection;
- live practice state using `eagler-touhou/thprac-session/1`;
- replay-viewer intent;
- 2/3-player multiplayer, spectators, deterministic seed/difficulty/loadouts,
  relay URL and Host-provided STUN/TURN ICE servers.

Debug/audit options exist for automated tests and are not separate product
features.

## Input and touch

SDL remains authoritative for normal keyboard/gamepad delivery. The browser
shell maintains a small fallback keyboard bitset for browser/WebView paths that
lose physical key events; the C++ input path merges it with SDL state.

Touch supports menu gestures and gameplay direct/virtual-stick movement. Live
Host controls can change fire/focus/bomb/escape and sensitivity without a
Runtime restart. Direct-touch coordinates are normalized through the current
canvas rectangle before entering the C++ touch bridge.

Web canvas focus changes are not application lifecycle changes. Real
backgrounding is handled by SDL/page lifecycle ownership; transient focus loss
only cancels active touch state so gameplay does not remain stuck.

## Persistent files

Normal persistence is `/savesth07`; multiplayer persistence is isolated at
`/savesth07-multiplayer`.

Host save/replay tooling operates through the explicit list/read/write/remove/
sync protocol rather than reaching into IndexedDB directly. Writes/removes are
synced before acknowledgement. The IDBFS restore path suppresses population-
generated `autoPersist` writes until restore finishes to avoid writing a
partial in-memory tree back over persistent storage.

## Practice integration

The hosted practice schema is `eagler-touhou/thprac-session/1` with
`game = "th07"`. C++ Runtime state, Host session state and the portable adapter
are one live owner and must be switched/cleared atomically at Practice, normal
Start and replay boundaries.

Legacy experimental portable/replay schemas are not part of the current live
Host protocol.

## Multiplayer implementation

TH07 supports 2- or 3-player browser sessions and spectators. The route is
selected before gameplay begins. WebRTC is preferred; signaling and emergency
WebSocket relay are Host-owned. The Runtime receives ICE configuration from the
Host and owns only game/session transport behavior.

The canonical shared relay/coturn implementation is in the sibling
`eagler-touhou` repository:

- `server/netplay-relay.mjs`
- `server/render-coturn-config.cjs`
- `server/coturn.env.example`
- `docs/SELF_HOSTING_REFERENCE.md`

TH07 tests may consume those Host-owned files, but this repository should not
carry a private duplicate server implementation.

The peer transport uses a pre-frame route barrier and buffers packets while the
route decision is still propagating. Mid-game transport migration is not part
of the current protocol.

## Shared tooling ownership

Common PBG/archive helpers used by Eagler packaging live in
`eagler-touhou/scripts/touhou_formats.py`. TH07 conversion scripts should reuse
that implementation rather than maintain a second PBG4 decompressor.

## Validation and test ownership

The fast CI gate for multiplayer/runtime core behavior is:

```sh
bash scripts/run-core-tests.sh
```

It compiles and executes the netplay protocol/session, rollback journal and
multiplayer input-lane tests. GitHub `Eagler Web CI` runs that core suite
alongside source-only `web-ci-normal` and `web-ci-netplay` Emscripten builds.

The broader `tests/` directory contains executable browser, replay, relay, TURN
and spectator behavior harnesses. Host-owned integration code and active
cross-repository source-contract audits are resolved from
`TH_EAGLER_HOST_ROOT`; cross-game checks use `TH_EAGLER_TH06_ROOT` for the TH06
checkout. These dependencies are explicit and are never discovered from a fixed
sibling-workspace layout. `scripts/convert_bgm_ogg.py` accepts the same Host
checkout through `--host-root` or `TH_EAGLER_HOST_ROOT`.

Static source-text checks live under `audit/source-contracts/`; historical
rebase evidence lives under `audit/feature-rebase/`. Neither audit directory is
runtime/gameplay acceptance.

## Repository boundaries

Do not commit original game archives, generated playable DATA, local build/test
output, browser profiles or accidental dirty submodule state. Shared Host
server/tooling code stays in `eagler-touhou`; this repository contains the TH07
Runtime and its game-specific integration/tests.
