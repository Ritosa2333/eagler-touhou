# Browser keyboard ownership

## Startup branding

`startup-branding.mjs` and `startup-wordmark.svg` own the TH06–TH09 startup
overlay. Generate the self-contained directory module or inline shell block
with `node tools/sync-startup-branding.mjs TARGET ...`; `--check` detects drift.
The glyph and yin-yang paths come from the original Eagler wordmark. Per-title
colors share optical bounds: right edge 628, logo top 112, build bottom 141 on
the 640×480 game canvas. No external font is required.

Adapters pass their build timestamp and mount the generated transparent pixels
before showing the startup picture. Each renderer draws the separate texture
after its startup background and before authored animations/covers. Original
game images and archives are never modified. TH06/TH07 stamp UTC at link time;
TH08/TH09 record the native build timestamp in their build/package metadata.

TH10/TH11/TH15/TH20 use `startup-developers.svg` (Georgia Italic serif paths and
a white/transparent yin-yang with a cutout eye), `startup-developers.mjs`, and `StartupBranding.hpp`.
Run `node tools/sync-startup-developers.mjs REPOSITORY ...` (or `--check`) to
generate each title's self-contained shell module and renderer helper. The
complete “EAGLER ☯ TOUHOU Developers” line is centered below the studio credit,
220 logical pixels wide, with no shadow. Build time is right-aligned at (628,472)
in the bottom-right corner of the full game canvas, UTC+8.
All signature lettering is stored as SVG paths: builds and browsers require no
Georgia font file, font package, or font download. The small build timestamp
uses the browser's local serif fallback without loading a font resource.
Native build metadata owns that timestamp. Retail images are not inputs to the
transparent texture generator. The native signature VM controls the credit's
RGB/alpha and draw lifetime; later scene layers still cover it. TH11/TH15 keep
their isolated signature ANMs ticking during the existing two-second startup
hold while menu preparation completes. This does not advance gameplay logic.
The helper restores renderer state and never reads back the game framebuffer.

`keyboard-owners.mjs` owns physical-key identity and release reconciliation.
The title adapter maps events to bits and decides pulse/lifecycle policy.
The Launcher has its own session-scoped protocol owner: it returns the original
DOWN identity when forwarding a release to an older or newer Runtime.

Known modifier sides and Arrow/Numpad aliases remain independent. An UP with no
physical code or location releases every matching logical owner. This can stop
the remaining physically held modifier on a keyboard that loses all identity;
it prevents an arbitrary owner from staying stuck indefinitely. Fresh DOWN
restores input. A repeat with no observed owner cannot restore cancelled input.

Standalone Emscripten shells embed generated source so older game pins do not
need a new external JavaScript dependency. Never edit the generated block:

```powershell
node tools/sync-browser-keyboard.mjs TH06-SHELL.html TH07-SHELL.html
node tools/sync-browser-keyboard.mjs --check TH06-SHELL.html TH07-SHELL.html
node tests/browser-keyboard-owners.test.mjs
```

`--check` is the cross-title drift gate; the behavior test also runs through
CTest when Node is available. Game-specific tests execute the actual shell
handlers, and Launcher browser integration checks confirmed per-seat input.

Directory Runtimes use `directory-keyboard.mjs` with the same physical owner
algorithm. It retains the resolved DOWN code, reconciles an ambiguous UP against
its actual evidence, merges native and hosted owners, and publishes only changed
codes. `clear()` advances a generation and prevents orphan repeats from rearming.
The title still owns its bindings and practice hotkeys. Updated C++ hosts use the
presence of `Module.resetBrowserKeyboard` to choose this browser owner instead of
OR-ing SDL's independent cached keyboard state. C++ lifecycle clears invoke that
hook and also reset SDL; older shells retain their SDL fallback.

The self-contained generated directory module is an explicit source dependency,
listed in the title packager and Launcher product catalog. Generate it with:

```powershell
node tools/sync-directory-keyboard.mjs PATH/TO/sdl-runtime/directory-keyboard.mjs
```

The generator also supports `--check`. TH08/TH09/TH10 follow-up changes on
2026-10-01 were reviewed as source only; builds and tests were not run at the
user's request. TH06/TH07 verification does not cover this directory bridge.
