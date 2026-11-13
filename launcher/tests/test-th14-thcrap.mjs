import assert from "node:assert/strict";
import { test } from "vitest";
import { createThcrapClient } from "../integrations/thcrap.mjs";
import { ThcrapRuntimeCompiler } from "../server/thcrap-compiler.mjs";

const root = "https://fixture.test/";
const mount = (path, bytes, kind = "jdiff") => ({
  game: "th14", path, mountPath: "/thcrap/" + path, kind,
  bytes: Buffer.isBuffer(bytes) ? bytes : Buffer.from(JSON.stringify(bytes))
});

test("TH14 selects full 1.00b assets and compiles Extra shot F plus the twelfth ending", async () => {
  const files = {
    "th14/st07f.msg.jdiff": 1, "th14/e12.msg.jdiff": 2, "th14/text.v1.00b.anm": 3,
    "th14/text.v1.00a.anm": 4, "th14/text.v0.01b.anm": 5, "th14/front.anm.jdiff": 6,
    "th14/stringlocs.v1.00b.js": 7
  };
  const client = createThcrapClient({repository: root, fetchImpl: async url => Response.json(
    new URL(url).pathname.endsWith("/patch.js") ? {id: "lang_en"} : files
  )});
  const pack = await client.resolveLanguage("lang_en", "th14");
  assert.deepEqual(pack.assets.map(asset => asset.path).sort(),
    Object.keys(files).filter(path => files[path] <= 3 || files[path] === 6).sort());

  const front = Buffer.alloc(88);
  front.writeUInt32LE(8);
  front.writeUInt16LE(1, 4);
  front.writeUInt32LE(68, 64);
  front.writeUInt32LE(33, 68);
  front.writeFloatLE(96, 80);
  const runner = {
    async extractArchiveEntry(archive, entry, version) {
      assert.equal(archive, "retail.dat");
      assert.equal(version, 14);
      return entry === "front.anm" ? front : Buffer.from(entry);
    },
    async dumpMessage(bytes, version) {
      assert.equal(version, 14);
      assert.equal(bytes.toString(), "st07f.msg");
      return Buffer.from("entry 0 (256)\n@34\n\t17;original\n@35\n\t32;1\n\t17;unchanged\n");
    },
    async dumpEnding(bytes, version) {
      assert.equal(version, 14);
      assert.equal(bytes.toString(), "e12.msg");
      return Buffer.from("entry 0 (256)\n@0\n\t3;original\n");
    },
    async compileMessage(bytes, version) { assert.equal(version, 14); return bytes; },
    async compileEnding(bytes, version) { assert.equal(version, 14); return bytes; }
  };
  const compiler = new ThcrapRuntimeCompiler({runner, archives: {th14: "retail.dat"}});
  const [message, ending, texture, hud] = await compiler.processPack([
    mount("th14/st07f.msg.jdiff", {0: {"34_0": {lines: ["Extra shot F"]}}}),
    mount("th14/e12.msg.jdiff", {0: {"0_0": {lines: ["Twelfth ending"]}}}),
    mount("th14/text.v1.00b.anm", Buffer.from("anm"), "binary"),
    mount("th14/front.anm.jdiff", {sprites: {33: [0, 0, 0, 0]}})
  ]);
  assert.equal(message.targetPath, "/thcrap/th14/st07f.msg");
  assert.match(message.bytes.toString(), /17;Extra shot F\n@35\n\t32;1\n\t17;unchanged/);
  assert.equal(ending.targetPath, "/thcrap/th14/e12.msg");
  assert.match(ending.bytes.toString(), /3;Twelfth ending/);
  assert.equal(texture.targetPath, "/thcrap/th14/text.anm");
  assert.equal(hud.targetPath, "/thcrap/th14/front.anm");
  assert.equal(front.readFloatLE(80), 96);
  assert.equal(hud.bytes.readFloatLE(80), 0);
});
