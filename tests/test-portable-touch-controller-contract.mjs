import assert from "node:assert/strict";
import { readFile } from "node:fs/promises";
import { workspacePath } from "../lib/workspace-layout.mjs";

const th08Path = workspacePath("th08", "portable", "input", "TouchController.hpp");
const th10Path = workspacePath("th10", "portable", "input", "TouchController.hpp");
const [th08, th10] = await Promise.all([readFile(th08Path), readFile(th10Path)]);

assert.deepEqual(th08, th10, "TH08/TH10 portable TouchController implementations must remain byte-identical");
const source = th08.toString("utf8");
assert.ok(source.includes("motion_blocked"), "portable touch controller must preserve held movement ownership while movement is blocked");
assert.ok(source.includes("if(!s.ready&&dragging)motion_blocked=true;"), "deathbomb must block movement output without cancelling the held gesture");
assert.ok(source.includes("if(arrows)clear_motion();"), "physical/hosted arrow input must still take ownership from touch movement");
assert.ok(source.includes("const bool bomb=bomb_ticks>0;if(bomb_ticks>0)--bomb_ticks;"), "bomb pulse must be consumed while movement is blocked");
assert.ok(source.includes("out.keys[88]=bomb;"), "bomb must remain available during the deathbomb window");

console.log("Portable touch controller contract: PASS (TH08/TH10 identical, deathbomb-safe)");
