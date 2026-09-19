import assert from "node:assert/strict";
import { readFile } from "node:fs/promises";
import { workspacePath } from "../lib/workspace-layout.mjs";

const th08Path = workspacePath("th08", "portable", "input", "TouchController.hpp");
const th10Path = workspacePath("th10", "portable", "input", "TouchController.hpp");
const [th08, th10, th08Motion, th10Motion, th08Host, th10Host, th08GameHost, th10GameHost,
  th08Runtime, th08Result, th08Replay, th10World, th10Result, th10Replay] = await Promise.all([
  readFile(th08Path),
  readFile(th10Path),
  readFile(workspacePath("th08", "portable", "input", "MotionTrack.hpp"), "utf8"),
  readFile(workspacePath("th10", "portable", "input", "MotionTrack.hpp"), "utf8"),
  readFile(workspacePath("th08", "th08_web", "sdl-runtime", "eagler-host.mjs"), "utf8"),
  readFile(workspacePath("th10", "th10_web", "sdl-runtime", "eagler-host.mjs"), "utf8"),
  readFile(workspacePath("th08", "th08_web", "cpp", "sdl", "GameHost.cpp"), "utf8"),
  readFile(workspacePath("th10", "th10_web", "cpp", "sdl", "GameHost.cpp"), "utf8"),
  readFile(workspacePath("th08", "th08_web", "cpp", "platform", "BrowserRuntime.cpp"), "utf8"),
  readFile(workspacePath("th08", "th08_web", "cpp", "game", "ResultView.cpp"), "utf8"),
  readFile(workspacePath("th08", "th08_web", "cpp", "game", "ReplayExport.cpp"), "utf8"),
  readFile(workspacePath("th10", "th10_web", "cpp", "platform", "WorldPlayer.cpp"), "utf8"),
  readFile(workspacePath("th10", "th10_web", "cpp", "game", "Results.cpp"), "utf8"),
  readFile(workspacePath("th10", "th10_web", "cpp", "game", "ReplaySave.cpp"), "utf8"),
]);

assert.deepEqual(th08, th10, "TH08/TH10 portable TouchController implementations must remain byte-identical");
const source = th08.toString("utf8");
assert.ok(source.includes("motion_blocked"), "portable touch controller must preserve held movement ownership while movement is blocked");
assert.ok(source.includes("if(!s.ready&&dragging)motion_blocked=true;"), "deathbomb must block movement output without cancelling the held gesture");
assert.ok(source.includes("if(arrows)clear_motion();"), "physical/hosted arrow input must still take ownership from touch movement");
assert.ok(source.includes("const bool bomb=bomb_ticks>0;if(bomb_ticks>0)--bomb_ticks;"), "bomb pulse must be consumed while movement is blocked");
assert.ok(source.includes("out.keys[88]=bomb;"), "bomb must remain available during the deathbomb window");

assert.equal(th08Motion, th10Motion, "TH08/TH10 MotionTrack implementations must remain byte-identical");
assert.ok(th08Motion.includes("cheat_movement_used=false"), "unlimited-touch cheat use needs a run-level marker");
assert.ok(th08Motion.includes("mark_cheat_movement(float x,float y){if(unlimited&&(x!=0||y!=0))cheat_movement_used=true;}"),
  "merely enabling unlimited touch must not mark a run; a non-zero consumed movement must do so");
assert.ok(th08Runtime.includes("motion.mark_cheat_movement(x,y);"), "TH08 must mark actual consumed unlimited movement");
assert.ok(th08Result.includes("context.cheat_movement_used?100.f"), "TH08 Result processing-drop rate must become 100% after cheat movement");
assert.ok(th08Replay.includes("context.cheat_movement_used?100.f"), "TH08 Replay processing-drop metadata must become 100% after cheat movement");
assert.ok(th10World.includes("w.motion.mark_cheat_movement(dx,dy);"), "TH10 must mark actual consumed unlimited movement");
assert.ok(th10Result.includes("env.cheat_movement_used&&*env.cheat_movement_used?100.f"), "TH10 Result processing-drop rate must become 100% after cheat movement");
assert.ok(th10Replay.includes("env.cheat_movement_used&&*env.cheat_movement_used?100.f"), "TH10 Replay processing-drop metadata must become 100% after cheat movement");

for (const [game, host, nativeHost] of [["th08", th08Host, th08GameHost], ["th10", th10Host, th10GameHost]]) {
  assert.match(host, /Math\.max\(100,Math\.min\(300,sensitivity\)\)/,
    `${game}: Runtime option normalization must use the shared 100-300 percent sensitivity range`);
  assert.match(host, /sensitivity>=100&&sensitivity<=300/,
    `${game}: live touch snapshots must reject sensitivity outside the shared range`);
  assert.match(nativeHost, /std::clamp\(speed,1\.f,3\.f\)/,
    `${game}: native touch bridge must enforce the same 1.0-3.0 scale after percent conversion`);
}

console.log("Portable touch controller contract: PASS (TH08/TH10 identical, deathbomb-safe, sensitivity 100-300%, cheat processing-drop semantics)");
