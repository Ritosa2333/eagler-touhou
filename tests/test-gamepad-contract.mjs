import assert from "node:assert/strict";
import { readFile } from "node:fs/promises";
import { PRODUCT_GAMES } from "../lib/contracts/product-catalog.mjs";
import { workspacePath } from "../lib/workspace-layout.mjs";

// Gamepad is a required adapter capability, but each Runtime is free to use
// the platform path that matches its architecture. This contract proves that
// every formal game still has a physical-controller polling path; it does not
// require the implementations to be textually identical.
const sources = {
  th06: await readFile(workspacePath("th06", "src", "Controller.cpp"), "utf8"),
  th07: await readFile(workspacePath("th07", "src", "Controller.cpp"), "utf8"),
  th08: await readFile(workspacePath("th08", "th08_web", "cpp", "sdl", "GameHost.cpp"), "utf8"),
  th10: await readFile(workspacePath("th10", "th10_web", "cpp", "sdl", "GameHost.cpp"), "utf8"),
};
assert.deepEqual(Object.keys(sources).sort(), Object.keys(PRODUCT_GAMES).sort(),
  "required gamepad verification must be updated when a formal game adapter is registered");

for (const game of ["th06", "th07"]) {
  assert.match(sources[game], /SDL_GetGamepadButton/,
    `${game}: required physical gamepad button path is missing`);
  assert.match(sources[game], /SDL_GetGamepadAxis/,
    `${game}: required physical gamepad axis path is missing`);
}
for (const game of ["th08", "th10"]) {
  assert.match(sources[game], /SDL_GetJoystickButton/,
    `${game}: required physical controller button path is missing`);
  assert.match(sources[game], /SDL_GetJoystickAxis/,
    `${game}: required physical controller axis path is missing`);
}

console.log(`All-game physical gamepad/controller contract: PASS (${Object.keys(PRODUCT_GAMES).map(game => game.toUpperCase()).join("/")})`);
