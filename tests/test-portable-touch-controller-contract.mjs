import assert from "node:assert/strict";
import { readFile } from "node:fs/promises";
import { workspacePath } from "../lib/workspace-layout.mjs";

const th08Path = workspacePath("th08", "portable", "input", "TouchController.hpp");
const th10Path = workspacePath("th10", "portable", "input", "TouchController.hpp");
const [th08, th10, th08Host, th10Host, th08GameHost, th10GameHost] = await Promise.all([
  readFile(th08Path),
  readFile(th10Path),
  readFile(workspacePath("th08", "th08_web", "sdl-runtime", "eagler-host.mjs"), "utf8"),
  readFile(workspacePath("th10", "th10_web", "sdl-runtime", "eagler-host.mjs"), "utf8"),
  readFile(workspacePath("th08", "th08_web", "cpp", "sdl", "GameHost.cpp"), "utf8"),
  readFile(workspacePath("th10", "th10_web", "cpp", "sdl", "GameHost.cpp"), "utf8"),
]);

assert.deepEqual(th08, th10, "TH08/TH10 portable TouchController implementations must remain byte-identical");
const source = th08.toString("utf8");
assert.ok(source.includes("motion_blocked"), "portable touch controller must preserve held movement ownership while movement is blocked");
assert.ok(source.includes("if(!s.ready&&dragging)motion_blocked=true;"), "deathbomb must block movement output without cancelling the held gesture");
assert.ok(source.includes("if(arrows)clear_motion();"), "physical/hosted arrow input must still take ownership from touch movement");
assert.ok(source.includes("const bool bomb=bomb_ticks>0;if(bomb_ticks>0)--bomb_ticks;"), "bomb pulse must be consumed while movement is blocked");
assert.ok(source.includes("out.keys[88]=bomb;"), "bomb must remain available during the deathbomb window");

for (const [game, host, nativeHost] of [["th08", th08Host, th08GameHost], ["th10", th10Host, th10GameHost]]) {
  assert.match(host, /Math\.max\(100,Math\.min\(300,sensitivity\)\)/,
    `${game}: Runtime option normalization must use the shared 100-300 percent sensitivity range`);
  assert.match(host, /sensitivity>=100&&sensitivity<=300/,
    `${game}: live touch snapshots must reject sensitivity outside the shared range`);
  assert.match(nativeHost, /std::clamp\(speed,1\.f,3\.f\)/,
    `${game}: native touch bridge must enforce the same 1.0-3.0 scale after percent conversion`);
}

console.log("Portable touch controller contract: PASS (TH08/TH10 identical, deathbomb-safe, sensitivity 100-300%)");
