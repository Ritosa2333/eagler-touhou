# Streaming OGG music

`StreamingMusicPlayer` from `src/audio/streaming-music.mjs` accepts the existing
`MusicPlayer` constructor arguments (`context`, `readBytes`, `volume`) and has
the same `load`, `preload`, `play`, `pause`, `resume`, `stop`, `setVolume`, `fade`,
`source`, `gain`, `track`, `paused`, and offset/fade state interface. It also
exports a `MusicPlayer` alias. Pass `onError:error => fail(error)` to surface
errors raised after playback has begun; initial load/play failures reject their
promise. Without a callback, later errors are thrown asynchronously and retained
in `player.error`.

`load`/`preload` return encoded-byte/metadata records rather than AudioBuffers.
Compressed OGG bytes are retained in `buffers`; PCM is never cached per track.
The default codec module URL is resolved relative to this source module as
`../../site/runtime/audio/vorbis.mjs`; `decoderModuleURL` can override it.

Build with `node scripts/build-audio.mjs` using the existing Emscripten 4.0.22
installation (`EMSDK`, `EMCC_PYTHON_SCRIPT`, `EMSDK_PYTHON` are supported). The
script writes `site/runtime/audio/vorbis.mjs` and `site/runtime/audio/vorbis.wasm`. Both must
be included in the Runtime and served together, including inside a managed
Runtime. It keeps compiler cache/temp files under this project's `artifacts/`
and does not require global packages. The script only compiles; it does not run
the decoder, game, browser, syntax checks, or tests.

The Wasm wrapper opens stb_vorbis's memory pull API. It reads setup/length
metadata and the codec's bounded startup frame, then decodes only requested
sample ranges, with intrinsic Vorbis frame overlap. Neither `decodeAudioData`
nor stb_vorbis's whole-file decode API is called. Only one active music decoder
is retained. Its scratch PCM holds at most 16384 frames per channel. Scheduled
AudioBuffers contain 8192 frames each, with a queue capped at approximately two
seconds plus a block. At 44100 Hz stereo this is about 0.8 MiB of queued PCM plus
128 KiB of decoder output scratch and codec setup/overlap memory. The compressed
file has one Wasm copy while its decoder is open.

Loop boundaries are rounded to original-rate sample indices; blocks crossing a
boundary seek to `loopStart` and continue in the same AudioBuffer. This applies
to every indexed OGG, including the original ending's `th10_17.ogg`. Index rate,
channel count, loop bounds and premature EOF are checked explicitly. Playback
time is scheduled from integer frame counts on `AudioContext.currentTime`,
using timer/source-ended callbacks; rendering frames do not drive it.

Pause stores the sample position and fade's remaining duration, cancels queued
sources, and closes the decoder. Resume reopens the encoded stream and restores
that state. Request tokens discard stale asynchronous starts. The supplied
AudioContext belongs to the caller and is never closed by this player. Fade
gain automation uses the existing exponential amplitude curve to -50 dB and
then zero; muted playback remains logically active, as with the full player.

The finite queue can underrun if the main thread is blocked longer than the
queued audio. The player reports the first occurrence to the console, increments
`underruns`, and seeks forward according to the audio clock before refilling.
It cannot recover sound that was missed during the stall. This implementation
decodes on the main thread and depends on WebAssembly, dynamic ES modules, and
Web Audio. The upstream codec limitations (including no concatenated Vorbis
streams and unsupported pre-2004 floor 0) are listed in its source. Building is
not runtime or listening acceptance; no playback validation is implied.
