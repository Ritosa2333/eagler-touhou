import test from "node:test";
import assert from "node:assert/strict";
import {mkdtemp, writeFile, rm} from "node:fs/promises";
import {tmpdir} from "node:os";
import {join} from "node:path";
import {createHash} from "node:crypto";
import {readNativeTrace} from "../tools/parity/native-trace.mjs";

async function traceFile(body, check) {
  const directory = await mkdtemp(join(tmpdir(), "th19-native-trace-"));
  try { const path = join(directory, "trace.jsonl"); await writeFile(path, body); await check(path); }
  finally { await rm(directory, {recursive: true, force: true}); }
}
test("native trace streams large UTF-8 rows without changing order or source hash", async () => {
  const rows = [{frame: 1, note: "魔理沙".repeat(30000)}, {frame: 1, rng: [7, 8]}, {frame: 2}];
  const body = rows.map(JSON.stringify).join("\r\n") + "\r\n";
  await traceFile(body, async path => {
    const result = await readNativeTrace(path);
    assert.deepEqual(result.rows, rows);
    assert.equal(result.sha256, createHash("sha256").update(body).digest("hex"));
    assert.equal(result.incompleteTail, false);
  });
});
test("native trace accepts complete final JSON and marks only an incomplete final row", async () => {
  await traceFile('{"frame":1}\n{"frame":2}', async path => {
    assert.deepEqual((await readNativeTrace(path)).rows, [{frame: 1}, {frame: 2}]);
  });
  await traceFile('{"frame":1}\n{"frame":', async path => {
    const result = await readNativeTrace(path);
    assert.deepEqual(result.rows, [{frame: 1}]); assert.equal(result.incompleteTail, true);
  });
});
test("native trace rejects malformed rows before the final unterminated tail", async () => {
  await traceFile('{"frame":1}\ninvalid\n{"frame":2}\n', async path => {
    await assert.rejects(readNativeTrace(path), SyntaxError);
  });
});
