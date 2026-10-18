# eagler branch / SDL3 adapter

This branch targets eagler-touhou, using the Portable SDL3/GLES game core.
The upstream launcher and its combined DATA/music packaging are not an
acceptance target. Original game logic, renderer, simulation and audio
ownership remain native C++.

## Runtime boundary

- Parent: the existing same-origin `eagler-touhou/1` protocol.
- DATA: the parent's managed retail-memory lease, mounted as
  `/game/th10.dat`. Fonts are immutable shared Launcher resources, never part
  of the game DATA identity. Japanese uses shared `/msgothic.ttc`; Simplified
  Chinese uses shared `/unifont.otf`. The adapter keeps the same canonical
  shared-resource contract as TH06/TH07 and creates a runtime-local symlink at
  launch (`/fonts/msgothic.ttc` or `/fonts/simhei.ttf`) for the existing SDL
  binary, so the Runtime no longer packages duplicate font binaries. It only
  carries its small text-support tables (`blend.bin` and `codepages.bin`).
- Music: canonical `/bgm-ogg/th10_*.ogg`; stream and full decode are native.
  Existing game loop points, fades and SFX remain game-owned.
- The Launcher may initially configure `music: midi`, install local OGG
  directly through `FS.writeFile`, then set `Module.touhouMusicMode = ogg`.
  Launch reads the final mode; later writes notify the native pending source.
- Touch: direct drag, unlimited drag, fixed/free joystick, fire/focus and
  bomb/escape serial pulses. Joystick protocol axes use ±32767. Live snapshots
  do not reset the active gesture. Cancel, blur and mode changes release
  transient input. No new touch replay capability is advertised.
- Storage remains the existing language-separated IDBFS save namespace.

## Build and package

Set `EMSDK` to the pinned SDK and, if needed, `EM_CONFIG` to its configuration.

```powershell
node portable/build.mjs
$env:EAGLER_FONT_ROOT = 'PATH-TO-PRIVATE-SDL-NATIVE-FONTS'
node portable/package-eagler.mjs
node --test portable/test-eagler-host.mjs
node portable/check-eagler-touch.mjs
node portable/check-subpixel.mjs
```

`build-eagler/` is a closed directory Runtime consumed by the canonical
Launcher release assembler. Its `runtime-files.json` uses
`eagler-touhou/runtime-directory/1`; every file has size and SHA-256.
Packaging rejects stale compiled source and unexpected output files.
No retail DATA, OGG archive, original launcher or MIDI sound bank is included.

Use the canonical Launcher's `tests/browser/test-sdl-adapters.mjs` with a
private fixture configuration for local browser acceptance. Tests and builds
are separate from physical-device or public-deployment acceptance.
