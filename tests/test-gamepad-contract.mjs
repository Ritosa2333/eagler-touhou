import assert from "node:assert/strict";
import { readFile } from "node:fs/promises";
import { PRODUCT_GAMES } from "../lib/contracts/product-catalog.mjs";
import { workspacePath } from "../lib/workspace-layout.mjs";

// Gamepad is a required adapter capability. Physical device button indices are
// not a stable product contract, so every Runtime must normalize controllers
// before KeyConfig/gameplay consumes them.
const sources = {
  th06: await readFile(workspacePath("th06", "src", "Controller.cpp"), "utf8"),
  th07: await readFile(workspacePath("th07", "src", "Controller.cpp"), "utf8"),
  th08: await readFile(workspacePath("th08", "th08_web", "cpp", "sdl", "GameHost.cpp"), "utf8"),
  th10: await readFile(workspacePath("th10", "th10_web", "cpp", "sdl", "GameHost.cpp"), "utf8"),
};
const th08Runtime = await readFile(workspacePath("th08", "th08_web", "cpp", "platform", "BrowserRuntime.hpp"), "utf8");
const runtimeShells = {
  th06: await readFile(workspacePath("th06", "resources", "shell.html"), "utf8"),
  th07: await readFile(workspacePath("th07", "resources", "shell.html"), "utf8"),
  th08: await readFile(workspacePath("th08", "th08_web", "sdl-runtime", "shell.mjs"), "utf8"),
  th10: await readFile(workspacePath("th10", "th10_web", "sdl-runtime", "shell.mjs"), "utf8"),
};
assert.deepEqual(Object.keys(sources).sort(), Object.keys(PRODUCT_GAMES).sort(),
  "required gamepad verification must be updated when a formal game adapter is registered");

for (const game of Object.keys(sources)) {
  assert.match(sources[game], /SDL_GetGamepadButton/,
    `${game}: required physical gamepad button path is missing`);
  assert.match(sources[game], /SDL_GetGamepadAxis/,
    `${game}: required physical gamepad axis path is missing`);
}
for (const game of ["th08", "th10"]) {
  assert.match(sources[game], /SDL_OpenGamepad/,
    `${game}: controller opening must use SDL Gamepad normalization`);
  assert.match(sources[game], /SDL_GetGamepads/,
    `${game}: controller discovery must use SDL Gamepad normalization`);
  assert.doesNotMatch(sources[game], /SDL_(?:Open|Get|Close)Joystick|SDL_JoystickConnected|SDL_GetJoysticks/,
    `${game}: raw SDL Joystick identity/button paths must not define game bindings`);
}

const expectedSlots = [
  "SOUTH", "EAST", "WEST", "NORTH", "LEFT_SHOULDER", "RIGHT_SHOULDER",
  "BACK", "START", "LEFT_STICK", "RIGHT_STICK", "GUIDE",
];
function normalizedSlots(source, declaration) {
  const start = source.indexOf(declaration);
  assert.notEqual(start, -1, `missing normalized gamepad slot declaration: ${declaration}`);
  const body = source.slice(start, source.indexOf("};", start));
  return [...body.matchAll(/SDL_GAMEPAD_BUTTON_([A-Z_]+)/g)].map(match => match[1]);
}
assert.deepEqual(normalizedSlots(sources.th07, "g_DIToSDLButton[]"), expectedSlots,
  "TH07 canonical DirectInput-style logical slots changed unexpectedly");
for (const game of ["th08", "th10"]) {
  assert.deepEqual(normalizedSlots(sources[game], "gamepad_slots[]"), expectedSlots,
    `${game}: KeyConfig logical slots must match the proven TH07 normalized mapping`);
}

assert.match(th08Runtime, /InputController::bindings\(pad,app\.title\.context\.controller_state\)/,
  "TH08 KeyConfig must sample the same normalized controller snapshot as gameplay");

for (const game of ["th06", "th07"]) {
  const options = await readFile(workspacePath(game, "src", "EaglerOptions.hpp"), "utf8");
  assert.match(options, /BrowserGamepadDirectionBits\(\)/,
    `${game}: mobile keyboard-like Gamepad D-pad fallback is missing`);
  assert.match(options, /keyboard\|\\bkb\\b/i,
    `${game}: Gamepad D-pad fallback must be restricted to keyboard-like devices`);
  assert.match(options, /pad\.buttons\[12\].*pad\.buttons\[13\].*pad\.buttons\[14\].*pad\.buttons\[15\]/s,
    `${game}: keyboard-like Gamepad fallback must use the standard D-pad buttons only`);
}
for (const game of ["th08", "th10"]) {
  assert.match(sources[game], new RegExp(`${game}_keyboard_gamepad_dpad`),
    `${game}: mobile keyboard-like Gamepad D-pad fallback is missing`);
  assert.match(sources[game], /keyboard\|\\bkb\\b/i,
    `${game}: Gamepad D-pad fallback must be restricted to keyboard-like devices`);
  assert.match(sources[game], /pad\.buttons\[12\].*pad\.buttons\[13\].*pad\.buttons\[14\].*pad\.buttons\[15\]/s,
    `${game}: keyboard-like Gamepad fallback must use the standard D-pad buttons only`);
  assert.match(runtimeShells[game], /function runtimeKeyboardCode\(message\)/,
    `${game}: hosted mobile keyboard messages need Runtime-side normalization`);
  assert.match(runtimeShells[game], /message\.key.*message\.keyCode/s,
    `${game}: Runtime keyboard normalization must preserve key/keyCode fallback`);
  assert.match(runtimeShells[game], /Unidentified/,
    `${game}: Runtime keyboard normalization must handle mobile Unidentified code events`);
}

for (const game of ["th06", "th07"]) {
  assert.match(runtimeShells[game], /keyboardBitForEvent/,
    `${game}: mature hosted keyboard normalization must remain present`);
  assert.match(runtimeShells[game], /event\.key.*event\.keyCode/s,
    `${game}: hosted keyboard normalization must preserve key/keyCode fallback`);
}

console.log(`All-game normalized gamepad/KeyConfig contract: PASS (${Object.keys(PRODUCT_GAMES).map(game => game.toUpperCase()).join("/")})`);
