# Audio Playbook

Status: active

## Purpose

Preserve each title's audio ownership and timing while adapting BGM, SFX,
optional MIDI and browser audio output. Backend modernization may replace the
PCM/device implementation; it must not silently replace track, loop, fade,
pause or game-state semantics.

## Applicability and authority

Applies to:

- the older TH06/TH07 game-owned SoundPlayer/streaming paths;
- the current TH08/TH10 C++ / SDL3 / miniaudio browser Runtimes;
- formal product music selection;
- OGG stream/full **decode policy**;
- optional MIDI transport where the product declares MIDI;
- audio regressions caused by scheduler, lifecycle or presentation changes.

One title's backend is reference evidence, not an ABI another title must copy.

<!-- knowledge-id: K-AUDIO-001 -->
## Normal design

Formal Runtime music selections are:

```text
ogg
midi
none
```

`stream` versus `full` is the separate `oggDecodeMode` option. It is not a
fourth/fifth music mode. Raw retail PCM/WAV/archive paths are source or Host
preparation mechanisms, not additional player-selectable modes.

The game remains the owner of track selection, command queues, looping, fade,
volume and SFX timing. The current directory Runtimes use this shape:

```text
title GameAudio/MIDI state
    ↓
miniaudio / title OGG source policy
    ↓
SDL3 browser audio output
```

For a MIDI-capable title:

```text
title SMF/MIDI owner
    ↓
title-owned timing / loop / fade
    ↓
Runtime MIDI event bridge → Launcher/browser synth
```

The browser bridge receives the final MIDI bytes; it does not reparse the MIDI file or become the owner of tempo, looping or fade semantics.

## Invariants

- Presentation FPS must not accelerate the audio scheduler or make logical BGM/SFX commands run more often.
- BGM and SFX remain distinct owners; BGM streaming metrics must not be used as SFX proof.
- OGG stream keeps the existing game-owned streaming state machine and decodes only the PCM range requested by the source.
- OGG full decode changes the PCM source policy, not track selection, loop, fade or command ownership.
- `none` disables BGM but must not disable SFX.
- MIDI timing uses actual elapsed time and preserves the title's authored event
  cadence rather than display-frame count.
- Restoring browser output after background/foreground must not restart the
  track, reset fade state or otherwise replace the title's existing music state.
- A source switch covers the whole old-source stop → new-source open/decode →
  new-source establishment lifecycle; exposing output during only part of that
  transition can repeat or overlap tracks.
- Browser `AudioContext`/synth state is evidence about the bridge, not proof
  that every device's gesture or background policy is accepted.
- Original audio files and user data remain external/private inputs; do not add them to source or public packages.

<!-- knowledge-id: K-AUDIO-002 -->
## Known pitfall: MIDI timer cadence

Older browser ports demonstrated that a portable timer stub can accidentally
tie MIDI progress to render cadence. When changing the main loop or presentation
ownership, check that the title's MIDI owner still advances from elapsed time
and is not driven by the number of display Draw calls.

## Known pitfall: OGG source policy

The OGG path should keep the title's game-audio owner authoritative. Stream mode
retains/feeds a decoder on demand; full mode may decode ahead, but both must feed
the same title-owned loop/read/reset contract. Do not replace this with an
independent browser-native track list/player.

## Superseded approaches

Browser-native track playback or a display-driven timer stub is not the product
design: the game remains the owner of track semantics and the Web lane supplies
decode/output/timing services. Treat `isPlaying` alone as an insufficient
audio result.

<!-- knowledge-id: K-AUDIO-003 -->
## Browser audio lifecycle gate

An `AudioContext` may remain `SUSPENDED` until a real user gesture. Verify
gesture/policy, context state, output progress and simulation progress as
separate observations; do not infer audio health from a visible canvas or
`isPlaying` flag. Current TH08/TH10 shells pause the game loop while recovering
foreground audio so browser output recovery cannot race ordinary simulation.

## Code anchors

Current directory-Runtime reference anchors (in the sibling TH08/TH10 Runtime
repositories):

- `th08_web/cpp/sdl/AudioHost.cpp` / `th10_web/cpp/sdl/AudioHost.cpp`:
  miniaudio/SDL3 output and title audio-device bridge;
- `th08_web/cpp/platform/GameAudioManager.*` and the corresponding TH10
  platform/game audio owners: title-side track/stream ownership;
- `th08_web/cpp/game/MidiPlayer.cpp`: TH08 MIDI semantics;
- `th08_web/sdl-runtime/shell.mjs`, `th10_web/sdl-runtime/shell.mjs` and
  `eagler-host.mjs`: configure mode, resource install and foreground audio
  recovery;
- `portable/build.mjs`: current C++ / SDL3 / Emscripten build identity.

TH06/TH07 keep their own title-specific audio owners; use them only when working
on those adapters.

## Verification

For an audio or scheduler change:

1. Build and run the static preview checks for the exact Web lane.
2. Test `none` and `ogg` separately, then test both `oggDecodeMode=stream`
   and `full`; test `midi` only for a product that declares MIDI.
3. Keep BGM and SFX metrics separate. Verify title BGM, a real SFX input and at least one Stage 1 transition where relevant.
4. For MIDI, record actual message categories and nonzero TinySynth/audio evidence, not only `isPlaying` or an open device.
5. Check loop/切曲/fade and background/foreground or gesture behavior when the changed code touches lifecycle.
6. Re-run Replay/storage checks if scheduler ownership changed, then label Edge, WebView, real-device and public evidence separately.

## Deliberately omitted claims

This playbook does not promote one browser/device run to universal mobile audio
acceptance or public deployment acceptance.
