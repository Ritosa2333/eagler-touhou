# vendored

SDL is pinned to the official SDL 3.4.12 release commit
`f87239e71e42da91ca317a12eefb82cfbf3393eb`. The former
`0a61b59af1eec0c89b06b13bd4fd0f2ecdc3aff6` audio-buffer fork commit is
not available from the configured official remote. TH07's Web audio buffer
request now lives in `src/SoundPlayer.cpp`, using
`SDL_HINT_AUDIO_DEVICE_SAMPLE_FRAMES`; no private SDL modification is required.

For whatever reason, Emscripten doesn't include SDL3_image in its ports. Therefore this is required for building with SDL3 on Emscripten. Also I figured I might as well build SDL and SDL_ttf as well, so those are included as well.
