/**
 * Required presentation-cadence coverage gate.
 *
 * The implementation is intentionally allowed to differ by Runtime family,
 * but every formal game must be registered in one verification lane. Adding a
 * Product Catalog entry without presentation verification makes this test fail
 * immediately instead of silently leaving the new adapter untested.
 */
import assert from "node:assert/strict";
import { access } from "node:fs/promises";
import { spawnSync } from "node:child_process";
import { PRODUCT_GAMES } from "../lib/contracts/product-catalog.mjs";
import { workspacePath } from "../lib/workspace-layout.mjs";
import { WORKSPACE_NODE_TESTS } from "./test-plan.mjs";

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
});

assert.deepEqual(Object.keys(verification).sort(), Object.keys(PRODUCT_GAMES).sort(),
  "required presentation verification must be registered for every formal game adapter");

const scheduledWorkspaceTests = new Set(WORKSPACE_NODE_TESTS);
for (const [game, lane] of Object.entries(verification)) {
  const product = PRODUCT_GAMES[game];
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
  games: Object.keys(verification),
  lanes: Object.fromEntries(Object.entries(verification).map(([game, lane]) => [game, lane.kind])),
}));
