/** L2 artifact test. Preconditions: synthetic local files only. Mutations:
 * rewrite each fixture component in a fresh temporary directory. Proves:
 * identity, inventory and checksum tampering fails. Not source-build proof. */
import assert from "node:assert/strict";
import { createHash } from "node:crypto";
import { mkdtemp, readFile, rm, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join } from "node:path";
import { writeReleaseManifest, verifyReleaseManifest, fileSetIdentity } from "../lib/release-manifest.mjs";

const track = { bytes: 4, sha256: "a".repeat(64) };
const set = fileSetIdentity(["track.ogg"], [track]);
assert.deepEqual(set.sizes, [4]);
assert.notEqual(set.version, fileSetIdentity(["track.ogg"], [{ ...track, sha256: "b".repeat(64) }]).version);
assert.throws(() => fileSetIdentity(["track.ogg", "track.ogg"], [track, track]), /invalid content set/);
assert.throws(() => fileSetIdentity(["track.ogg"], [{ ...track, bytes: 0 }]), /invalid content identity/);

const root = await mkdtemp(join(tmpdir(), "eagler-release-manifest-"));
try {
const payload = Buffer.from("release fixture");
await writeFile(join(root, "runtime.wasm"), payload);
const deployment = {
  format: "eagler-touhou-deployment/1",
  releaseManifest: "release-manifest.json",
  files: [{
    path: "runtime.wasm",
    bytes: payload.length,
    sha256: createHash("sha256").update(payload).digest("hex"),
  }],
};
const deploymentText = JSON.stringify(deployment);
await writeFile(join(root, "deployment.json"), deploymentText);
const manifest = await writeReleaseManifest(root, {
  profile: "test",
  sources: { fixture: { revision: "synthetic", sha256: "fixture-only" } },
});
assert.equal((await verifyReleaseManifest(root)).releaseId, manifest.releaseId);
for (const [name, replacement, pattern] of [
  ["runtime.wasm", Buffer.from("tampered payload"), /release file mismatch/],
  ["deployment.json", `${deploymentText} `, /deployment identity mismatch/],
  ["checksums.txt", "", /checksum list mismatch/],
  ["release-manifest.json", JSON.stringify({ ...manifest, profile: "tampered" }), /manifest identity mismatch/],
]) {
  const original = await readFile(join(root, name));
  await writeFile(join(root, name), replacement);
  await assert.rejects(() => verifyReleaseManifest(root), pattern);
  await writeFile(join(root, name), original);
}
assert.equal((await verifyReleaseManifest(root)).releaseId, manifest.releaseId);
} finally {
  await rm(root, { recursive: true, force: true });
}
console.log("Release manifest L2: PASS (four tamper cases; fixture cleaned)");
