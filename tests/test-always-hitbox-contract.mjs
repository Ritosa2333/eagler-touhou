import fs from "node:fs";
import path from "node:path";
import assert from "node:assert/strict";
import { PRODUCT_GAMES } from "../lib/contracts/product-catalog.mjs";

const root = path.resolve(import.meta.dirname, "..", "..");
const th06Options = fs.readFileSync(path.join(root, "th06-eagler/src/EaglerOptions.hpp"), "utf8");
const th06Player = fs.readFileSync(path.join(root, "th06-eagler/src/Player.cpp"), "utf8");
const player = fs.readFileSync(path.join(root, "th07-eagler/src/Player.cpp"), "utf8");
const th08Shell = fs.readFileSync(path.join(root, "th08-eagler/th08_web/sdl-runtime/shell.mjs"), "utf8");
const th08Host = fs.readFileSync(path.join(root, "th08-eagler/th08_web/cpp/sdl/GameHost.cpp"), "utf8");
const th08Form = fs.readFileSync(path.join(root, "th08-eagler/th08_web/cpp/game/PlayerForm.cpp"), "utf8");
const th08Scene = fs.readFileSync(path.join(root, "th08-eagler/th08_web/cpp/game/PlayerScene.hpp"), "utf8");
const th10Exports = fs.readFileSync(path.join(root, "th10-eagler/th10_web/cpp/platform/ApplicationExports.cpp"), "utf8");
const th10World = fs.readFileSync(path.join(root, "th10-eagler/th10_web/cpp/platform/WorldPlayer.cpp"), "utf8");
const coveredGames = new Set();

if (!th06Options.includes("AlwaysShowHitbox()") ||
    !th06Player.includes("EaglerOptions::AlwaysShowHitbox()") ||
    !th06Player.includes("DrawEaglerHitbox(")) {
  throw new Error("th06: required always-hitbox presentation is missing");
}
coveredGames.add("th06");

if (!player.includes("AnmVm g_EaglerHitboxVm")) {
  throw new Error("th07: always-hitbox visual must own a standalone render VM");
}
if (!player.includes("const Rng savedRng = g_Rng;") || !player.includes("g_Rng = savedRng;")) {
  throw new Error("th07: always-hitbox visual must preserve gameplay RNG around ANM execution");
}
if (/AlwaysShowHitbox\(\)[\s\S]{0,300}SpawnEffect\(24/.test(player)) {
  throw new Error("th07: always-show-hitbox must never allocate native EffectManager effect 24");
}
coveredGames.add("th07");

if (!th08Shell.includes("core.sdl_touch_display?.(options.alwaysHitbox?1:0)") ||
    !th08Host.includes('EX("sdl_touch_display")') ||
    !th08Form.includes("const bool show_hitbox=focused||always_hitbox") ||
    !th08Scene.includes("fixed_effect(22,p,2,0xffffffff)")) {
  throw new Error("th08: required always-hitbox must reuse the native focus marker without changing collision");
}
coveredGames.add("th08");

if (!th10Exports.includes('application_touch_display') ||
    !th10Exports.includes("always_hitbox=hitbox!=0") ||
    !th10World.includes("if(always_hitbox&&player.state==1)")) {
  throw new Error("th10: required always-hitbox presentation is missing");
}
coveredGames.add("th10");

assert.deepEqual([...coveredGames].sort(), Object.keys(PRODUCT_GAMES).sort(),
  "required always-hitbox verification must be updated when a formal game adapter is registered");
console.log(`All-game always-hitbox presentation contract: PASS (${Object.keys(PRODUCT_GAMES).map(game => game.toUpperCase()).join("/")})`);
