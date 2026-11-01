import {test} from 'vitest';
import assert from "node:assert/strict";
import { readFile } from "node:fs/promises";
import { resolve } from "node:path";
import { assemblePreloadData } from "../lib/preload-data-assembler.mjs";
import {workspacePath} from "../lib/workspace-layout.mjs";

test("test-preload-data-assembler", async () => {
/** Workspace integration: reconstruct current TH06/TH07 Emscripten DATA from
 * their declared source files and require exact bytes against known builds. */






for (const test of [
  { game: "th06", build: "build-web-eagler-thprac-test" },
  { game: "th07", build: "build-web-eagler-thprac" },
]) {
  const runtime = workspacePath(test.game, test.build, `${test.game}.js`);
  const expected = await readFile(workspacePath(test.game, test.build, `${test.game}.data`));
  const { data, layout } = await assemblePreloadData({
    game: test.game,
    runtimeScript: runtime,
    sourceDirectory: workspacePath(test.game, 'assets'),
  });
  assert.deepEqual(data, expected, `${test.game}: assembled DATA must exactly match the Emscripten build artifact`);
  assert.equal(data.length, layout.bytes);
}
console.log("Preload DATA assembly: PASS (TH06/TH07 exact bytes)");
});
