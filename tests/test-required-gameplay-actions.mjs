/** Cross-repository semantic input contract for Launcher-owned controls.
 * Existing titles may keep different native bitmasks/state machines, but the
 * shared Restart control depends on one stable behavior: R restarts the current
 * run while the pause UI owns gameplay input. */
import assert from "node:assert/strict";
import { readFile } from "node:fs/promises";
import { PRODUCT_GAMES } from "../lib/contracts/product-catalog.mjs";
import { workspacePath } from "../lib/workspace-layout.mjs";

const coveredGames = new Set();

const th06Ascii = await readFile(workspacePath("th06", "src", "AsciiManager.cpp"), "utf8");
assert.match(th06Ascii, /WAS_PRESSED\(TH_BUTTON_R\)[\s\S]{0,800}GAME_MENU_PAUSE_SELECTED_RESTART/,
  "th06: R must select the Pause restart action");
coveredGames.add("th06");

const th07Controller = await readFile(workspacePath("th07", "src", "Controller.cpp"), "utf8");
const th07Ascii = await readFile(workspacePath("th07", "src", "AsciiManager.cpp"), "utf8");
assert.match(th07Controller, /SDL_SCANCODE_R\s*,\s*TH_BUTTON_RESET/,
  "th07: R must map to the native Reset input bit");
assert.match(th07Ascii, /WAS_PRESSED_RAW\(TH_BUTTON_RESET\)/,
  "th07: Pause must consume the Reset input bit");
coveredGames.add("th07");

const th08Input = await readFile(workspacePath("th08", "th08_web", "cpp", "game", "InputController.cpp"), "utf8");
const th08Pause = await readFile(workspacePath("th08", "th08_web", "cpp", "game", "UiMenus.cpp"), "utf8");
assert.match(th08Input, /\{19\s*,\s*82\s*,\s*Reset\}/,
  "th08: R must map to the native Reset input bit");
assert.match(th08Pause, /restart\s*=\s*16384/,
  "th08: Pause restart must own the Reset input bit");
coveredGames.add("th08");

const th10Input = await readFile(workspacePath("th10", "th10_web", "cpp", "game", "InputDevices.cpp"), "utf8");
const th10Pause = await readFile(workspacePath("th10", "th10_web", "cpp", "game", "ResultsPause.cpp"), "utf8");
assert.match(th10Input, /\{0x52\s*,\s*0x4000\}/i,
  "th10: R must map to the native restart input bit");
assert.match(th10Pause, /pressed\s*&\s*0x4000/,
  "th10: Pause must consume the restart input bit");
coveredGames.add("th10");

assert.deepEqual([...coveredGames].sort(), Object.keys(PRODUCT_GAMES).sort(),
  "required restart-action verification must be updated when a formal game adapter is registered");
console.log(`Required gameplay actions: PASS (R -> pause restart for ${Object.keys(PRODUCT_GAMES).map(game => game.toUpperCase()).join("/")})`);
