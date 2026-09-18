/** L2 artifact contract. Builds a synthetic all-products Runtime Release and
 * proves exact-file-set, hash, path and original-resource leak rejection.
 * Does not prove any Runtime executes in a browser. */
import assert from "node:assert/strict";
import { mkdtemp, mkdir, readFile, rm, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join, resolve } from "node:path";
import { validateRuntimeReleaseManifest, verifyRuntimeRelease } from "../lib/runtime-release.mjs";
import { writeSyntheticRuntimeRelease } from "../tests/support/runtime-release-fixture.mjs";

const root = await mkdtemp(join(tmpdir(), "eagler-runtime-release-"));
const manifest = await writeSyntheticRuntimeRelease(root);
await verifyRuntimeRelease(root);

const staleShellPath = resolve(root, "runtime", "th08", "shell.mjs");
const staleShell = await readFile(staleShellPath, "utf8");
await writeFile(staleShellPath, staleShell.replaceAll("runtimeEpoch", "legacyRuntimeToken"));
await assert.rejects(() => verifyRuntimeRelease(root), /navigation epoch protocol contract is missing/);
await writeFile(staleShellPath, staleShell);

await writeFile(resolve(root, "runtime", "th06", "th06.data"), "private game data");
await assert.rejects(() => verifyRuntimeRelease(root), /unexpected=.*th06\.data/);
await rm(resolve(root, "runtime", "th06", "th06.data"));

await writeFile(resolve(root, "runtime", "th06", "eagler-hitbox.png"), "derived original artwork");
await assert.rejects(() => verifyRuntimeRelease(root), /unexpected=.*eagler-hitbox\.png/);
await rm(resolve(root, "runtime", "th06", "eagler-hitbox.png"));

const badPath = structuredClone(manifest);
badPath.games.th06.runtime.root = "C:\\private\\runtime";
assert.throws(() => validateRuntimeReleaseManifest(badPath), /invalid Runtime root/);

const badLayout = structuredClone(manifest);
badLayout.games.th07.dataLayout = `sha256-${"f".repeat(64)}`;
await writeFile(resolve(root, "runtime-release.json"), `${JSON.stringify(badLayout, null, 2)}\n`);
await assert.rejects(() => verifyRuntimeRelease(root), /DATA layout does not match Runtime JS/);

console.log("Runtime Release closed-set contract: PASS");
