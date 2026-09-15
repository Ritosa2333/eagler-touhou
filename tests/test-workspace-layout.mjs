/** L0/L3 module contract. Preconditions: repository-owned workspace config.
 * Mutation: none. Proves topology names resolve from one owner and may be
 * rooted elsewhere with EAGLER_WORKSPACE_ROOT. Does NOT prove sibling repos
 * are present or valid. */
import assert from "node:assert/strict";
import { isAbsolute } from "node:path";
import {
  WORKSPACE_REPOSITORIES,
  launcherRoot,
  projectRelativeWorkspacePath,
  workspacePath,
  workspaceRepositoryNames,
} from "../lib/workspace-layout.mjs";

assert.deepEqual(Object.keys(WORKSPACE_REPOSITORIES), [
  "launcher", "th06", "th07", "th08", "th10", "thprac", "dependencies", "toolchains",
]);
assert.deepEqual(workspaceRepositoryNames(["launcher", "th06", "th08"]), [
  "eagler-touhou", "th06-eagler", "th08-eagler",
]);
assert.equal(WORKSPACE_REPOSITORIES.th08, "th08-eagler");
assert.equal(WORKSPACE_REPOSITORIES.th10, "th10-eagler");
assert.ok(isAbsolute(workspacePath("th07", "resources", "shell.html")));
assert.equal(workspacePath("launcher"), launcherRoot());
assert.equal(projectRelativeWorkspacePath("th06", "src", "FileSystem.cpp"), "../th06-eagler/src/FileSystem.cpp");
assert.equal(projectRelativeWorkspacePath("th08", "build-eagler"), "../th08-eagler/build-eagler");
assert.equal(projectRelativeWorkspacePath("th10", "build-eagler"), "../th10-eagler/build-eagler");
assert.throws(() => workspacePath("missing"), /unknown workspace repository/);
console.log(JSON.stringify({ workspaceLayout: "PASS", repositories: Object.keys(WORKSPACE_REPOSITORIES).length }));
