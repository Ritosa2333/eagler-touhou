import {test} from 'vitest';
import assert from "node:assert/strict";
import { execFileSync } from "node:child_process";
import { readFileSync } from "node:fs";
import { isAbsolute } from "node:path";
import { resolve, relative } from "node:path";
import { PRODUCT_GAMES } from "../lib/contracts/product-catalog.mjs";
import { projectConfigurationPath } from "../lib/source-layout.mjs";
import {
  WORKSPACE_REPOSITORIES,
  launcherRoot,
  projectRelativeWorkspacePath,
  workspacePath,
  workspaceRepositoryNames,
  workspaceRoot,
} from "../lib/workspace-layout.mjs";

test("test-workspace-layout", async () => {
/** L0/L3 module contract. Preconditions: repository-owned workspace config.
 * Mutation: none. Proves topology names resolve from one owner and may be
 * rooted elsewhere with EAGLER_WORKSPACE_ROOT. Does NOT prove sibling repos
 * are present or valid. */









const configuration = JSON.parse(readFileSync(new URL('../config/workspace.json', import.meta.url), 'utf8'));
assert.deepEqual(WORKSPACE_REPOSITORIES, configuration.repositories);
assert.deepEqual(workspaceRepositoryNames(['launcher', 'th06', 'th08', 'th09']),
  ['launcher', 'th06', 'th08', 'th09'].map(key => configuration.repositories[key]));
assert.notEqual(workspacePath('th08'), workspacePath('th08mp'));
assert.notEqual(workspacePath('th15'), workspacePath('th15mp'));
assert.ok(isAbsolute(workspacePath("th07", "resources", "shell.html")));
assert.equal(workspacePath("launcher"), launcherRoot());
for (const [game, suffix] of [
  ["th06", ["src", "FileSystem.cpp"]],
  ["th08", ["build-eagler"]],
  ["th10", ["build-eagler"]],
  ["th09", ["web", "build-eagler"]],
]) {
  assert.equal(resolve(launcherRoot(), projectRelativeWorkspacePath(game, ...suffix)),
    resolve(workspaceRoot(), configuration.repositories[game], ...suffix));
}
assert.throws(() => workspacePath("missing"), /unknown workspace repository/);
const listedProducts = JSON.parse(execFileSync(process.execPath, ["scripts/list-product-games.mjs"], {
  cwd: resolve(import.meta.dirname, ".."),
  encoding: "utf8",
}));
assert.deepEqual(listedProducts, Object.keys(PRODUCT_GAMES),
  "maintainer tooling must derive the game list from Product Catalog");
const powershellLayout = readFileSync(resolve(import.meta.dirname, "..", "tools", "maintainer", "lib", "workspace-layout.psm1"), "utf8");
assert.doesNotMatch(powershellLayout, /foreach\s*\(\$name\s+in\s+@\([^)]*th0\d/i,
  "PowerShell workspace layout must validate repository declarations generically instead of carrying a second game registry");
console.log(JSON.stringify({ workspaceLayout: "PASS", repositories: Object.keys(WORKSPACE_REPOSITORIES).length }));
});
