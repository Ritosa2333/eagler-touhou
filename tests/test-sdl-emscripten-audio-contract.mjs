import { readFile } from "node:fs/promises";

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
  if (!sdlAudio.includes("SDL_max(4096, SDL_GetDefaultSampleFramesFromFreq(device->spec.freq) * 2)")) {
    throw new Error(`${game}: SDL Emscripten A/B must use at least a 4096-frame ScriptProcessor block`);
  }
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
  minimumFrames: 4096,
  rejectedExperiment: "AudioWorklet",
}));
