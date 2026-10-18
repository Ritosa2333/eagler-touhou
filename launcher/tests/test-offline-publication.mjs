// L2: synthetic content, unchanged publication then same-size tampering.
// Proves the publisher verifies bytes; does not prove gameplay or installation.
import assert from "node:assert/strict";
import { createHash } from "node:crypto";
import { spawnSync } from "node:child_process";
import { mkdtemp, readFile, rm, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join } from "node:path";
import { parsePackageZip } from "../package/package-zip.mjs";

const root = await mkdtemp(join(tmpdir(), "eagler-offline-publication-"));
try {
const payload = Buffer.from("content");
const identity = { bytes: payload.length, sha256: createHash("sha256").update(payload).digest("hex") };
const descriptor = {
  schema: "eagler-touhou/package/1",
  game: "th06",
  revision: "fixture",
  runtime: { type: "html", entry: "entry", playerProtocol: "eagler-touhou/player/1" },
  files: { entry: { source: "runtime.html", target: "/runtime.html", revision: "opaque", ...identity } },
  base: { files: ["entry"] },
  components: {},
};
await writeFile(join(root, "runtime.html"), payload);
const publish = () => spawnSync(process.execPath, [
  "scripts/package-offline-game.mjs",
  root,
  "th06",
  join(root, "out.zip"),
], { encoding: "utf8" });
async function saveDescriptor() {
  await writeFile(join(root, "th06.package.json"), JSON.stringify(descriptor));
}
await saveDescriptor();
let result = publish();
assert.equal(result.status, 0, result.stderr);
const parsed = await parsePackageZip(new Blob([await readFile(join(root, "out.zip"))]));
assert.equal(await parsed.files.get("entry").blob.text(), payload.toString());
await writeFile(join(root, "runtime.html"), "changed");
result = publish();
assert.equal(result.status, 0, result.stderr);
const changed = await parsePackageZip(new Blob([await readFile(join(root, "out.zip"))]));
assert.equal(await changed.files.get("entry").blob.text(), "changed");
assert.equal(changed.descriptor.files.entry.bytes, 7);
assert.equal(changed.descriptor.files.entry.sha256, createHash("sha256").update("changed").digest("hex"));
assert.notEqual(changed.descriptor.revision, descriptor.revision);
} finally {
  await rm(root, { recursive: true, force: true });
}
console.log("Offline publication input identity: PASS");
