import {selectedPortGames} from './support/port-selection.mjs';
/**
 * Required presentation-cadence coverage gate.
 *
 * The implementation is intentionally allowed to differ by Runtime family,
 * but every formal game must be registered in one verification lane. Adding a
 * Product Catalog entry without presentation verification makes this test fail
 * immediately instead of silently leaving the new adapter untested.
 */
import assert from "node:assert/strict";
import { access, readFile } from "node:fs/promises";
import { spawnSync } from "node:child_process";
import { PRODUCT_GAMES } from "../lib/contracts/product-catalog.mjs";
import { workspacePath } from "../lib/workspace-layout.mjs";
import { WORKSPACE_NODE_TESTS } from "./test-plan.mjs";
import { runNativeCpp } from "./support/native-cpp.mjs";

const verification = Object.freeze({
  th06: Object.freeze({ kind: "shared-preload", tests: Object.freeze([
    "tests/test-mobile-frame-pacing.mjs",
    "tests/test-native-frame-pacing.mjs",
  ]) }),
  th07: Object.freeze({ kind: "shared-preload", tests: Object.freeze([
    "tests/test-mobile-frame-pacing.mjs",
    "tests/test-native-frame-pacing.mjs",
  ]) }),
  th08: Object.freeze({ kind: "runtime", scripts: Object.freeze([
    "portable/check-high-refresh-contract.mjs",
    "portable/check-presentation-purity.mjs",
  ]) }),
  th10: Object.freeze({ kind: "runtime", scripts: Object.freeze([
    "portable/check-high-refresh-contract.mjs",
    "portable/check-presentation-purity.mjs",
  ]) }),
  th09: {kind:'native-clock', source:'web/cpp/sdl/Application.cpp'},
  th075: {kind:'native-clock', source:'web/cpp/sdl/BrowserRuntime.cpp'},
  th11: {kind:'native-clock', source:'web/cpp/sdl/Application.cpp'},
  th15: {kind:'native-clock', source:'web/cpp/sdl/BrowserRuntime.cpp'},
  th20: {kind:'native-clock', source:'web/cpp/sdl/ApplicationHost.cpp'},
});

assert.deepEqual(Object.keys(verification).sort(), Object.keys(PRODUCT_GAMES).sort(),
  "required presentation verification must be registered for every formal game adapter");

const scheduledWorkspaceTests = new Set(WORKSPACE_NODE_TESTS);
const selectedGames = selectedPortGames(Object.keys(verification));
for (const [game, lane] of Object.entries(verification).filter(([game]) => selectedGames.includes(game))) {
  const product = PRODUCT_GAMES[game];
  if (lane.kind === 'native-clock') {
    const host = await readFile(workspacePath(game, lane.source), 'utf8');
    assert.match(host, /cadence\.advance\(/, `${game}: browser loop must consume its cadence owner`);
    assert.match(host, /cadence\.reset\(/, `${game}: lifecycle must retire wall-clock debt`);
    if (game === 'th09') {
      await runNativeCpp(await readFile(workspacePath(game,'web/tests/multiplayer/frame-schedule.cpp'),'utf8'),
        {includes:[workspacePath(game,'web/tests/multiplayer')]});
    } else if (game === 'th075') {
      await runNativeCpp(`#include "FrameCadence.hpp"
#include <cassert>
#include <initializer_list>
int main(){for(unsigned hz:{60u,75u,90u,120u,144u,165u,240u}){
 touhou::sdl::FrameCadence clock;unsigned ticks=0;
 for(unsigned i=0;i<hz*10;++i)ticks+=clock.advance(1./hz);
 assert(ticks==600);
 }
 touhou::sdl::FrameCadence clock;
 assert(clock.advance(.1)==4);assert(clock.advance(0)==2);
 clock.reset();assert(clock.advance(0)==0);assert(clock.advance(-1)==0);
}`, {includes:[workspacePath(game,'portable/sdl')]});
    } else if (game === 'th20') {
      await runNativeCpp(await readFile(workspacePath(game,'portable/check-frame-cadence.cpp'),'utf8'),
        {includes:[workspacePath(game,'portable')]});
    } else {
      await runNativeCpp(`#include "PracticeCadence.hpp"
#include <cassert>
#include <initializer_list>
int main(){for(unsigned hz:{60u,75u,90u,120u,144u,165u,240u}){
 ${game}::PracticeCadence clock;unsigned ticks=0;
 for(unsigned i=0;i<hz*10;++i)ticks+=clock.advance(1./hz);
 assert(ticks==600);
 }
 ${game}::PracticeCadence clock;
 assert(clock.advance(.1)==4);assert(clock.advance(0)==2);
 clock.reset();assert(clock.advance(0)==0);assert(clock.advance(-1)==0);
 clock.period=1./120.;assert(clock.advance(1./60.)==2);
 clock.reset();assert(clock.advance(0)==0);
}`, {includes:[workspacePath(game,`web/cpp/game`)]});
    }
    continue;
  }
  if (lane.kind === "shared-preload") {
    assert.equal(product.dataProvider, "emscripten-preload",
      `${game}: shared preload presentation lane is only valid for emscripten-preload Runtimes`);
    for (const test of lane.tests) {
      assert.ok(scheduledWorkspaceTests.has(test), `${game}: required presentation gate is not scheduled: ${test}`);
    }
    continue;
  }

  assert.equal(product.runtimeFileLayout, "directory",
    `${game}: Runtime-owned presentation lane is expected for directory Runtimes`);
  for (const script of lane.scripts) {
    const path = workspacePath(game, ...script.split("/"));
    await access(path);
    const result = spawnSync(process.execPath, [path], {
      cwd: workspacePath(game),
      encoding: "utf8",
      windowsHide: true,
    });
    assert.equal(result.status, 0,
      `${game}: presentation verification failed: ${script}\n${result.stdout || ""}\n${result.stderr || ""}`);
  }
}

console.log(JSON.stringify({
  presentationContractCoverage: "PASS",
  scope: "source wiring and actual native clock components; no rendered-browser or physical-display claim",
  games: selectedGames,
  lanes: Object.fromEntries(Object.entries(verification).map(([game, lane]) => [game, lane.kind])),
}));
