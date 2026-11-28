import assert from "node:assert/strict";
import test from "node:test";

import { createMusicTransfer, isMusicPath } from "../../wwwroot/host/audio/music-transfer.mjs";
import { readRuntimeFile } from "../../wwwroot/runtime-files.mjs";

const OGG = Uint8Array.from([0x4f, 0x67, 0x67, 0x53, 1, 2, 3, 4]);
const BAD = Uint8Array.from([0, 1, 2, 3, 4, 5, 6, 7]);

function fixture() {
  const events = [];
  const files = new Map();
  const transfer = createMusicTransfer({
    emit(event, fields) {
      events.push({ event, ...fields });
    },
    putFile(path, data) {
      files.set(path, Uint8Array.from(data));
    },
    fetchImpl: async (url) => {
      if (String(url).includes("missing")) return { ok: false, arrayBuffer: async () => new ArrayBuffer(0) };
      if (String(url).includes("corrupt")) {
        return { ok: true, arrayBuffer: async () => BAD.buffer.slice(BAD.byteOffset, BAD.byteOffset + BAD.byteLength) };
      }
      return { ok: true, arrayBuffer: async () => OGG.buffer.slice(OGG.byteOffset, OGG.byteOffset + OGG.byteLength) };
    },
  });
  return { events, files, transfer };
}

test("BGM paths are music mounts", () => {
  assert.equal(isMusicPath("/BGM/OP.ogg"), true);
  assert.equal(isMusicPath("/Sound/se_ok00.wav"), false);
});

test("missing music soft-fails with music-error and keeps pending for retry", async () => {
  const { events, files, transfer } = fixture();
  const result = await transfer.install([[
    { url: "http://local/missing-OP.ogg", path: "/BGM/OP.ogg" },
    { url: "http://local/ok-Stage.ogg", path: "/BGM/Stage01.ogg" },
  ]]);
  assert.equal(result.failed, 1);
  assert.equal(result.installed, 1);
  assert.equal(transfer.pendingCount(), 1);
  assert.deepEqual([...files.keys()], ["/BGM/Stage01.ogg"]);
  assert.ok(events.some((row) => row.event === "music-incomplete" && row.failed === 1));
  assert.ok(events.some((row) => row.event === "transfer" && row.path === "/BGM/Stage01.ogg"));
});

test("corrupt music is rejected as a damaged resource", async () => {
  const { events, transfer } = fixture();
  const result = await transfer.install([[
    { url: "http://local/corrupt-OP.ogg", path: "/BGM/OP.ogg" },
  ]]);
  assert.equal(result.failed, 1);
  assert.equal(transfer.pendingCount(), 1);
  assert.ok(events.some((row) => row.event === "music-error" && row.failed === 1));
});

test("retry-music recovers a previously failed track", async () => {
  let attempt = 0;
  const events = [];
  const files = new Map();
  const transfer = createMusicTransfer({
    emit(event, fields) {
      events.push({ event, ...fields });
    },
    putFile(path, data) {
      files.set(path, Uint8Array.from(data));
    },
    fetchImpl: async () => {
      attempt += 1;
      if (attempt === 1) return { ok: false, arrayBuffer: async () => new ArrayBuffer(0) };
      return { ok: true, arrayBuffer: async () => OGG.buffer.slice(OGG.byteOffset, OGG.byteOffset + OGG.byteLength) };
    },
  });
  await transfer.install([[{ url: "http://local/OP.ogg", path: "/BGM/OP.ogg" }]]);
  assert.equal(transfer.pendingCount(), 1);
  const retried = await transfer.retry();
  assert.equal(retried.failed, 0);
  assert.equal(retried.installed, 1);
  assert.equal(transfer.pendingCount(), 0);
  assert.deepEqual([...files.get("/BGM/OP.ogg")], [...OGG]);
  assert.ok(events.some((row) => row.event === "music-complete"));
});

test("non-music resources still hard-fail the install", async () => {
  const { transfer } = fixture();
  await assert.rejects(
    transfer.install([[{ url: "http://local/missing-font", path: "/msgothic.ttc" }]]),
    /Missing resource/,
  );
});

test("none-mode clear drops pending music retries", async () => {
  const { transfer } = fixture();
  await transfer.install([[{ url: "http://local/missing-OP.ogg", path: "/BGM/OP.ogg" }]]);
  assert.equal(transfer.pendingCount(), 1);
  transfer.clear();
  assert.equal(transfer.pendingCount(), 0);
  const retried = await transfer.retry();
  assert.equal(retried.installed, 0);
  assert.equal(retried.failed, 0);
});

test("default putFile lands on the shared runtime map", async () => {
  const transfer = createMusicTransfer({
    emit() {},
    fetchImpl: async () => ({
      ok: true,
      arrayBuffer: async () => OGG.buffer.slice(OGG.byteOffset, OGG.byteOffset + OGG.byteLength),
    }),
  });
  await transfer.install([[{ url: "http://local/OP.ogg", path: "/BGM/OP.ogg" }]]);
  assert.deepEqual([...readRuntimeFile("/BGM/OP.ogg")], [...OGG]);
});
