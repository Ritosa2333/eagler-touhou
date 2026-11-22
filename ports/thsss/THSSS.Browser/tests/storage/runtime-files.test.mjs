import assert from "node:assert/strict";
import test from "node:test";

import { installRemoteResources, readRuntimeFile } from "../../wwwroot/runtime-files.mjs";

test("configure music resources land on their mount path", async () => {
  const fetchImpl = async (url) => ({
    ok: url.endsWith("OP.ogg"),
    arrayBuffer: async () => Uint8Array.from([1, 2, 3, 4]).buffer,
  });
  const count = await installRemoteResources([
    [{ url: "http://local/OP.ogg", path: "/BGM/OP.ogg" }],
    [{ url: "http://local/missing.ogg", path: "not-absolute" }],
  ], fetchImpl);
  assert.equal(count, 1);
  assert.deepEqual([...readRuntimeFile("/BGM/OP.ogg")], [1, 2, 3, 4]);
});

test("a missing music file fails the install", async () => {
  const fetchImpl = async () => ({ ok: false, arrayBuffer: async () => new ArrayBuffer(0) });
  await assert.rejects(
    installRemoteResources([[{ url: "http://local/OP.ogg", path: "/BGM/OP.ogg" }]], fetchImpl),
    /Missing resource/,
  );
});
