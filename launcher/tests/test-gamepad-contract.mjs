import assert from "node:assert/strict";
import { existsSync } from "node:fs";
import { readFile } from "node:fs/promises";
import { PRODUCT_GAMES } from "../lib/contracts/product-catalog.mjs";
import { workspacePath } from "../lib/workspace-layout.mjs";
import { runNativeCpp } from "./support/native-cpp.mjs";

const runtimeBuilds = JSON.parse(await readFile(workspacePath("launcher", "config", "runtime-builds.json"), "utf8"));
const hostCandidates = [
  "src/Controller.cpp",
  "web/cpp/sdl/GameHost.cpp",
  "web/cpp/sdl/Application.cpp",
  "web/cpp/sdl/BrowserRuntime.cpp",
  "web/cpp/sdl/InputHost.cpp",
];
function codeOnly(source) {
  return source.replace(/\/\*[\s\S]*?\*\//g, "").replace(/\/\/[^\r\n]*/g, "");
}
const formalVariants = product => product.multiplayer ? ["normal", "multiplayer"] : ["normal"];
const expectedIds = Object.entries(PRODUCT_GAMES).flatMap(([game, product]) =>
  formalVariants(product).map(variant => `${game}-${variant}`)).sort();
const catalogIds = Object.entries(runtimeBuilds.games).flatMap(([game, definition]) =>
  Object.keys(definition.variants).filter(variant => variant === "normal" || variant === "multiplayer")
    .map(variant => `${game}-${variant}`)).sort();
assert.deepEqual(catalogIds, expectedIds,
  "runtime-builds and Product Catalog must expose the same formal gamepad variants");
assert.equal(expectedIds.length, 15, "the complete registered gamepad matrix currently contains 15 variants");

const variants = [];
for (const [game, product] of Object.entries(PRODUCT_GAMES)) {
  const definition = runtimeBuilds.games[game];
  assert.ok(definition, `${game}: missing Runtime build registration`);
  for (const variant of formalVariants(product)) {
    const build = definition.variants[variant];
    assert.ok(build, `${game}/${variant}: missing Runtime build variant`);
    const repository = build.workspaceRepository ?? definition.workspaceRepository;
    const matches = [];
    for (const path of hostCandidates) {
      const absolute = workspacePath(repository, ...path.split("/"));
      if (!existsSync(absolute)) continue;
      const source = await readFile(absolute, "utf8");
      if (/eagler::input::BrowserGamepadState\s*\(/.test(codeOnly(source))) matches.push({path, absolute, source});
    }
    assert.equal(matches.length, 1,
      `${game}/${variant}: expected one native input owner under ${repository}, found ${matches.map(item => item.path).join(", ") || "none"}`);
    const host = matches[0];
    variants.push({id:`${game}-${variant}`, game, variant, repository, path:host.path,
      source:host.source});
  }
}
assert.deepEqual(variants.map(target => target.id).sort(), expectedIds,
  "every formal Runtime variant must resolve to its configured source owner");

function assertNativeSampling(target, originalSource = target.source) {
  const source = codeOnly(originalSource);
  assert.match(source, /eagler::input::BrowserGamepadState\s*\(/,
    `${target.id}: native input must sample the Launcher gamepad bridge`);

  if (target.path === "src/Controller.cpp") {
    assert.match(source, /const\s+int\s+mapped\s*=\s*eagler::input::BrowserGamepadState\s*\(\)\s*;[\s\S]*?if\s*\(mapped\s*>=\s*0\)[\s\S]*?buttons\s*\|=/,
      `${target.id}: normalized Launcher actions must reach the title input state`);
    assert.match(source, /if\s*\(eagler::input::BrowserGamepadOwned\s*\(\)\)\s*return\s+g_ControllerData/,
      `${target.id}: Launcher-owned mappings must not be captured as physical SDL bindings`);
    return;
  }

  if (target.game === "th09") {
    assert.match(source, /launcher_gamepad\[seat\]\s*=\s*eagler::input::BrowserGamepadState\s*\(seat\)/,
      `${target.id}: per-seat Launcher actions must be sampled`);
    assert.match(source, /launcher_gamepad\[side\]\s*>=\s*0[\s\S]*?ApplyGamepadVirtualKeys\([^,]+,\s*launcher_gamepad\[side\]\)/,
      `${target.id}: TH09 gameplay must consume the normalized per-seat actions`);
    assert.match(source, /BrowserGamepadOwned\s*\(\)[\s\S]*?return\s+32/,
      `${target.id}: Launcher-owned mappings must not be recaptured as physical bindings`);
    return;
  }

  if (target.path === "web/cpp/sdl/InputHost.cpp") {
    assert.match(source, /launcher_gamepad\s*=\s*eagler::input::BrowserGamepadState\s*\(\)/,
      `${target.id}: native frame sampling must read the Launcher gamepad state`);
    assert.match(source, /ApplyGamepadVirtualKeys\([^,]+,\s*launcher_gamepad\)/,
      `${target.id}: virtual actions must reach the game input path`);
    assert.match(source, /ApplyGamepadScanKeys\([^,]+,\s*launcher_gamepad\)/,
      `${target.id}: scan-code actions must reach the game input path`);
    return;
  }

  assert.match(source, /const\s+int\s+mappedPad\s*=\s*eagler::input::BrowserGamepadState\s*\(\)/,
    `${target.id}: the sampled Launcher mapping must have a local owner`);
  assert.match(source, /ApplyGamepadVirtualKeys\([^,]+,\s*mappedPad\)/,
    `${target.id}: normalized actions must enter the title's keyboard path`);
  assert.match(source, /mappedPad\s*<\s*0/,
    `${target.id}: native SDL input must remain the fallback when Launcher mapping is unowned`);
}

for (const target of variants) assertNativeSampling(target);

// Keep the legacy SDL Gamepad boundary checks grouped by actual source owner.
const uniqueTargets = [...new Map(variants.map(target => [`${target.repository}/${target.path}`, target])).values()];
for (const target of uniqueTargets) {
  const source = codeOnly(target.source);
  if (target.game === "th06" || target.game === "th07") {
    assert.match(source, /SDL_GetGamepadButton/, `${target.repository}: normalized SDL Gamepad buttons are required`);
    assert.match(source, /SDL_GetGamepadAxis/, `${target.repository}: normalized SDL Gamepad axes are required`);
    continue;
  }
  if (["th08", "th09", "th10"].includes(target.game)) {
    assert.match(source, /SDL_GetGamepadButton/, `${target.repository}: normalized SDL Gamepad buttons are required`);
    assert.match(source, /SDL_GetGamepadAxis/, `${target.repository}: normalized SDL Gamepad axes are required`);
    assert.match(source, /SDL_OpenGamepad/, `${target.repository}: controllers must open through SDL Gamepad`);
    assert.match(source, /SDL_GetGamepads/, `${target.repository}: controllers must be discovered through SDL Gamepad`);
    assert.doesNotMatch(source, /SDL_(?:Open|Get|Close)Joystick|SDL_JoystickConnected|SDL_GetJoysticks/,
      `${target.repository}: raw SDL Joystick identity/button paths must not define game bindings`);
  }
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
const th07 = variants.find(target => target.game === "th07" && target.variant === "normal");
assert.deepEqual(normalizedSlots(codeOnly(th07.source), "g_DIToSDLButton[]"), expectedSlots,
  "TH07 canonical DirectInput-style logical slots changed unexpectedly");
for (const target of uniqueTargets.filter(target => ["th08", "th09", "th10"].includes(target.game))) {
  assert.deepEqual(normalizedSlots(codeOnly(target.source), "gamepad_slots[]"), expectedSlots,
    `${target.id}: KeyConfig logical slots must match the proven TH07 normalized mapping`);
}

for (const target of uniqueTargets.filter(target => ["th06", "th07"].includes(target.game))) {
  const options = codeOnly(await readFile(workspacePath(target.repository, "src", "EaglerOptions.hpp"), "utf8"));
  assert.match(options, /BrowserGamepadDirectionBits\s*\(\)/,
    `${target.repository}: mobile keyboard-like Gamepad D-pad fallback is missing`);
  assert.match(options, /keyboard\|\\bkb\\b/i,
    `${target.repository}: Gamepad D-pad fallback must be restricted to keyboard-like devices`);
  assert.match(options, /pad\.buttons\[12\].*pad\.buttons\[13\].*pad\.buttons\[14\].*pad\.buttons\[15\]/s,
    `${target.repository}: keyboard-like Gamepad fallback must use the standard D-pad buttons only`);
}

for (const target of uniqueTargets.filter(target => ["th08", "th09", "th10"].includes(target.game))) {
  const source = codeOnly(target.source);
  const symbol = target.game === "th08" ? "th08_keyboard_gamepad_dpad" : `${target.game}_keyboard_gamepad_dpad`;
  assert.match(source, new RegExp(symbol), `${target.id}: mobile keyboard-like Gamepad D-pad fallback is missing`);
  assert.match(source, /keyboard\|\\bkb\\b/i,
    `${target.id}: Gamepad D-pad fallback must be restricted to keyboard-like devices`);
  assert.match(source, /pad\.buttons\[12\].*pad\.buttons\[13\].*pad\.buttons\[14\].*pad\.buttons\[15\]/s,
    `${target.id}: keyboard-like Gamepad fallback must use the standard D-pad buttons only`);
  const shellPath = target.game === "th09" ? "web/sdl-runtime/managed.mjs" : "web/sdl-runtime/shell.mjs";
  const shell = codeOnly(await readFile(workspacePath(target.repository, ...shellPath.split("/")), "utf8") +
    "\n" + await readFile(workspacePath(target.repository, "web/sdl-runtime/directory-keyboard.mjs"), "utf8"));
  assert.match(shell, /function runtimeKeyboardCode\(\w+\)/,
    `${target.id}: hosted mobile keyboard messages need Runtime-side normalization`);
  assert.match(shell, /(?:message|event)\.key.*(?:message|event)\.keyCode/s,
    `${target.id}: Runtime keyboard normalization must preserve key/keyCode fallback`);
  assert.match(shell, /Unidentified/,
    `${target.id}: Runtime keyboard normalization must handle mobile Unidentified code events`);
}

// A missing production sampler must fail the same owner-path contract.
const th08Normal = variants.find(target => target.id === "th08-normal");
const disconnected = th08Normal.source.replace(
  "const int mappedPad=eagler::input::BrowserGamepadState();",
  "const int mappedPad=-1;",
);
assert.notEqual(disconnected, th08Normal.source, "negative control must disconnect the actual TH08 normal sampler");
assert.throws(() => assertNativeSampling(th08Normal, disconnected), /native input must sample the Launcher gamepad bridge/,
  "a production host with its Launcher sample removed must fail the native wiring contract");

await runNativeCpp(`#include <eagler/input/BrowserGamepad.hpp>
#include <array>
#include <cassert>
using namespace eagler::input;
int main(){
  constexpr unsigned vk[]={90,88,16,27,38,40,37,39,17,13,67,82};
  constexpr unsigned scans[]={44,45,42,1,200,208,203,205,29,28,46,19};
  assert(BrowserGamepadState()==-1);
  for(unsigned bit=0;bit<12;++bit){
    std::array<unsigned char,256> keys{}, scan{};
    ApplyGamepadVirtualKeys(keys,1<<bit); ApplyGamepadScanKeys(scan,1<<bit);
    for(unsigned i=0;i<256;++i){
      const bool alias=(bit==2&&i==160)||(bit==8&&i==162);
      assert(bool(keys[i])==(i==vk[bit]||alias));
      assert(bool(scan[i])==(i==scans[bit]));
    }
  }
  std::array<unsigned char,256> keys{};keys[90]=128;
  ApplyGamepadVirtualKeys(keys,-1);assert(keys[90]==128);
  ApplyGamepadVirtualKeys(keys,0);assert(keys[90]==128);
}`, {includes:[workspacePath("common", "include")]});

console.log(`All-game normalized gamepad contract: PASS (${variants.length} Runtime variants; SDL owners: ${uniqueTargets.filter(target => ["th06", "th07", "th08", "th09", "th10"].includes(target.game)).map(target => target.repository).join("/")}; shared native bridge helper and broken-wiring negative control passed)`);
