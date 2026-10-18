/** L2 artifact contract. Preconditions: isolated generated candidate tree.
 * Mutation: creates a temporary bundle. Proves required release owners and report schema.
 * Does NOT prove Runtime contents, browser behavior or publication. */
import assert from "node:assert/strict";
import { createHash } from "node:crypto";
import { mkdtemp, mkdir, readFile, readdir, rm, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join, relative } from "node:path";
import { COMPLETION_FIELDS, COMPLETION_REPORT_SCHEMA } from "../lib/completion-report.mjs";
import { writeSyntheticRuntimeRelease } from "../tests/support/runtime-release-fixture.mjs";
import { writeReleaseManifest } from "../lib/release-manifest.mjs";
import { verifyReleaseBundle } from "../lib/release-bundle-verifier.mjs";
import { FORMAL_RELEASE_GAMES } from "../lib/release-plan.mjs";

const root = await mkdtemp(join(tmpdir(), "eagler-release-bundle-"));
try {
const games = [...FORMAL_RELEASE_GAMES];
for (const directory of ["hosted-site", "external-site", "import-site", "runtime-release", "game-package", "offline-zip"]) {
  await mkdir(join(root, directory));
}
await writeSyntheticRuntimeRelease(join(root, "runtime-release"));
for (const game of games) {
  await mkdir(join(root, "game-package", game));
  await writeFile(join(root, "game-package", game, "package.json"), "{}\n");
  await writeFile(join(root, "offline-zip", `${game}.zip`), game);
  await writeFile(join(root, "external-site", `${game}.package.json`), "{}\n");
}
await writeFile(join(root, "external-site", "release-catalog.json"), JSON.stringify({
  schema: "eagler-touhou/release-catalog/1",
  resourceMode: "external",
  games: Object.fromEntries(games.map(game => [game, { revision: "0".repeat(16), descriptor: `${game}.package.json` }])),
}));
await writeFile(join(root, "external-site", "deployment.json"), JSON.stringify({
  resourceMode: "external",
  files: games.map(game => ({ path: `${game}.package.json` })),
}));
const completion = Object.fromEntries(COMPLETION_FIELDS.map(field => [field, "pending"]));
completion.IMPLEMENTED = "yes";
completion["BUILD-VERIFIED"] = "yes";
completion["STRUCTURE-VERIFIED"] = "yes";
completion.RELEASED = "no";
await writeFile(join(root, "verification-report.json"), JSON.stringify({ schema: COMPLETION_REPORT_SCHEMA, steps: [], completion }));
await writeFile(join(root, "input-manifest.json"), "{}\n");
async function inventory(directory, files = []) {
  for (const entry of await readdir(directory, { withFileTypes: true })) {
    const path = join(directory, entry.name);
    if (entry.isDirectory()) await inventory(path, files);
    else {
      const bytes = await readFile(path);
      files.push({ path: relative(root, path).replaceAll("\\", "/"), bytes: bytes.length,
        sha256: createHash("sha256").update(bytes).digest("hex") });
    }
  }
  return files;
}
await writeFile(join(root, "deployment.json"), JSON.stringify({ format: "eagler-touhou-release-bundle/2", games, files: await inventory(root) }));
await writeReleaseManifest(root, {
  profile: "test",
  sources: { fixture: { revision: "test", sha256: "a".repeat(64) } },
  parameters: { games },
});
const result = await verifyReleaseBundle(root);
assert.match(result.releaseId, /^sha256-[a-f0-9]{64}$/);
assert.deepEqual(result.games, games);
assert.equal(result.completion.RELEASED, "no");
} finally {
  await rm(root, { recursive: true, force: true });
}
console.log("Release bundle L2: PASS");
