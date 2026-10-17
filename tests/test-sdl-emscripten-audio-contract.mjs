import { readFile } from "node:fs/promises";
import assert from "node:assert/strict";
import { runNativeCpp } from "./support/native-cpp.mjs";

import { PRODUCT_GAMES } from "../lib/contracts/product-catalog.mjs";
import { workspacePath } from "../lib/workspace-layout.mjs";

const preloadGames = Object.entries(PRODUCT_GAMES)
  .filter(([, product]) => product.dataProvider === "emscripten-preload")
  .map(([game]) => game)
  .sort();

for (const game of preloadGames) {
  const sdlAudio = await readFile(workspacePath(game, "vendored", "SDL", "src", "audio", "emscripten", "SDL_emscriptenaudio.c"), "utf8");
  if (!sdlAudio.includes("scriptProcessorNode") || !sdlAudio.includes("setup a ScriptProcessorNode")) {
    throw new Error(`${game}: SDL Emscripten playback must retain the verified ScriptProcessor baseline`);
  }
  // The title explicitly requests 1024 frames; SDL's current ScriptProcessor
  // backend doubles that to 2048. Do not silently restore a 4096-frame latency
  // experiment merely to satisfy a historical source-text assertion.
  const audio = await readFile(workspacePath(game, "vendored/SDL/src/audio/SDL_audio.c"), "utf8");
  const defaults = audio.match(/int SDL_GetDefaultSampleFramesFromFreq\(const int freq\)\s*\{[\s\S]*?\n\}/)?.[0];
  const assignment = sdlAudio.match(/device->sample_frames\s*=\s*[^;]+;/)?.[0];
  assert.ok(defaults && assignment, `${game}: SDL sample-frame policy must be inspectable`);
  const sound = await readFile(workspacePath(game, "src/SoundPlayer.cpp"), "utf8");
  assert.match(sound, /SDL_HINT_AUDIO_DEVICE_SAMPLE_FRAMES,\s*"1024"/, `${game}: title callback-size request`);
  await runNativeCpp(`#include <cassert>
#include <cstdlib>
#define SDL_HINT_AUDIO_DEVICE_SAMPLE_FRAMES "frames"
static const char* hint=nullptr;
const char* SDL_GetHint(const char*){return hint;}
int SDL_atoi(const char* v){return std::atoi(v);}
${defaults}
struct Device{struct{int freq;}spec;int sample_frames;};
int main(){Device value{};auto* device=&value;
 for(const int freq:{22050,44100,48000,96000}){device->spec.freq=freq;hint="1024";${assignment}assert(device->sample_frames==2048);}
 hint=nullptr;assert(SDL_GetDefaultSampleFramesFromFreq(22050)==512);assert(SDL_GetDefaultSampleFramesFromFreq(48000)==1024);
}`.replace('#include <cassert>', '#include <cassert>\n#include <initializer_list>'));
  for (const forbidden of ["eaglerAudioWorkletReady", "new AudioWorkletNode", "workletDrain", "workletQueuedFrames"]) {
    if (sdlAudio.includes(forbidden)) {
      throw new Error(`${game}: rejected AudioWorklet experiment must not return (${forbidden})`);
    }
  }
}

console.log(JSON.stringify({
  sdlEmscriptenAudio: "PASS",
  games: preloadGames,
  backend: "ScriptProcessor",
  configuredFrames: 2048,
  rejectedExperiment: "AudioWorklet",
}));
