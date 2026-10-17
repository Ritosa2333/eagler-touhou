/** L0/L3 module contract. Preconditions: repository-owned workspace config.
 * Mutation: none. Proves topology names resolve from one owner and may be
 * rooted elsewhere with EAGLER_WORKSPACE_ROOT. Does NOT prove sibling repos
 * are present or valid. */
import assert from "node:assert/strict";
import { execFileSync } from "node:child_process";
import { readFileSync } from "node:fs";
import { isAbsolute } from "node:path";
import { resolve, relative } from "node:path";
import { PRODUCT_GAMES } from "../lib/contracts/product-catalog.mjs";
import {
  WORKSPACE_REPOSITORIES,
  launcherRoot,
  projectRelativeWorkspacePath,
  workspacePath,
  workspaceRepositoryNames,
  workspaceRoot,
} from "../lib/workspace-layout.mjs";

assert.deepEqual(Object.keys(WORKSPACE_REPOSITORIES), [
  "launcher", ...Object.keys(PRODUCT_GAMES), "thprac", "dependencies", "toolchains", "th11mp", "th15mp", "th10mp", "common",
]);
assert.deepEqual(workspaceRepositoryNames(["launcher", "th06", "th08", "th09"]), [
  "eagler-touhou", "th06-eagler", "th08-eagler", "th09-eagler",
]);
assert.equal(WORKSPACE_REPOSITORIES.th08, "th08-eagler");
assert.equal(WORKSPACE_REPOSITORIES.th10, "th10-eagler");
assert.equal(WORKSPACE_REPOSITORIES.th09, "th09-eagler");
assert.equal(WORKSPACE_REPOSITORIES.th11, "th11-eagler");
assert.equal(WORKSPACE_REPOSITORIES.th11mp, "worktrees/th11-multiplayer");
assert.equal(WORKSPACE_REPOSITORIES.th15, "th15-eagler");
assert.equal(WORKSPACE_REPOSITORIES.th15mp, "th15-eagler");
assert.equal(WORKSPACE_REPOSITORIES.th10mp, "worktrees/th10-multiplayer");
assert.equal(WORKSPACE_REPOSITORIES.common, "eagler-common");
assert.equal(WORKSPACE_REPOSITORIES.th20, "th20-eagler");
assert.ok(isAbsolute(workspacePath("th07", "resources", "shell.html")));
assert.equal(workspacePath("launcher"), launcherRoot());
for (const [game, directory, suffix] of [
  ["th06", "th06-eagler", ["src", "FileSystem.cpp"]],
  ["th08", "th08-eagler", ["build-eagler"]],
  ["th10", "th10-eagler", ["build-eagler"]],
  ["th09", "th09-eagler", ["th09_web", "build-eagler"]],
]) {
  assert.equal(resolve(launcherRoot(), projectRelativeWorkspacePath(game, ...suffix)),
    resolve(workspaceRoot(), directory, ...suffix));
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
