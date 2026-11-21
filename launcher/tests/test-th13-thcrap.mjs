import assert from "node:assert/strict";
import { test } from "vitest";
import { createThcrapClient } from "../integrations/thcrap.mjs";
import { ThcrapRuntimeCompiler } from "../server/thcrap-compiler.mjs";

const root = "https://fixture.test/";
const mount = (path, bytes, kind = "jdiff") => ({
  game: "th13", path, mountPath: "/thcrap/" + path, kind,
  bytes: Buffer.isBuffer(bytes) ? bytes : Buffer.from(JSON.stringify(bytes))
});

test("TH13 keeps Simplified Chinese dialogue, endings and named textures", async () => {
  const files = {
    "th13/st01a.msg.jdiff": 1,
    "th13/e01.msg.jdiff": 2,
    "th13/title/title01.png": 3,
    "th13/st01.ecl": 4,
    "th13/stringlocs.js": 5
  };
  const client = createThcrapClient({repository: root, fetchImpl: async url => Response.json(
    new URL(url).pathname.endsWith("/patch.js") ? {id: "lang_zh-hans"} : files
  )});
  const pack = await client.resolveLanguage("lang_zh-hans", "th13");
  assert.deepEqual(pack.assets.map(asset => asset.path).sort(), [
    "th13/e01.msg.jdiff", "th13/st01a.msg.jdiff", "th13/title/title01.png"
  ]);

  const runner = {
    async extractArchiveEntry(archive, entry, version) {
      assert.equal(archive, "th13.dat");
      assert.equal(version, 13);
      return Buffer.from(entry);
    },
    async dumpMessage(bytes, version) {
      assert.equal(version, 13);
      assert.equal(bytes.toString(), "st01a.msg");
      return Buffer.from("entry 0 (256)\n@10\n\t17;original\n");
    },
    async dumpEnding(bytes, version) {
      assert.equal(version, 13);
      assert.equal(bytes.toString(), "e01.msg");
      return Buffer.from("entry 0 (256)\n@0\n\t3;original\n");
    },
    async compileMessage(bytes, version) { assert.equal(version, 13); return bytes; },
    async compileEnding(bytes, version) { assert.equal(version, 13); return bytes; }
  };
  const compiler = new ThcrapRuntimeCompiler({runner, archives: {th13: "th13.dat"}});
  const [message, ending, texture] = await compiler.processPack([
    mount("th13/st01a.msg.jdiff", {0: {"10_0": {lines: ["简体对话"]}}}),
    mount("th13/e01.msg.jdiff", {0: {"0_0": {lines: ["简体结局"]}}}),
    mount("th13/title/title01.png", Buffer.from("png"), "image")
  ]);
  assert.equal(message.targetPath, "/thcrap/th13/st01a.msg");
  assert.match(message.bytes.toString(), /17;简体对话/);
  assert.equal(ending.targetPath, "/thcrap/th13/e01.msg");
  assert.match(ending.bytes.toString(), /3;简体结局/);
  assert.equal(texture.targetPath, "/thcrap/th13/title/title01.png");
  assert.equal(texture.bytes.toString(), "png");
});
