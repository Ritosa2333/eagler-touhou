import {test} from 'vitest';
import assert from "node:assert/strict";
import { mkdir, mkdtemp, rm, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { dirname, resolve } from "node:path";
import { loadSourceLayout, validateWorkspaceConfiguration } from "../lib/source-layout.mjs";

test("test-source-layout", async () => {
const root = await mkdtemp(resolve(tmpdir(), "eagler-source-layout-"));
const configuration = launcher => ({ schema: "eagler-touhou/workspace-layout/1", repositories: {
  launcher, th06: "th06-eagler", common: "eagler-common",
} });
async function config(path, value) {
  await mkdir(dirname(path), { recursive: true });
  await writeFile(path, JSON.stringify(value));
}
try {
  const legacy = resolve(root, "legacy"), web = resolve(legacy, "worktrees/current-main");
  await config(resolve(web, "config/workspace.json"), configuration("eagler-touhou"));
  await writeFile(resolve(legacy, "WORKSPACE.md"), "workspace");
  let layout = loadSourceLayout(web, { environment: {} });
  assert.equal(layout.root, legacy, "an existing worktree must resolve the workspace, not its worktrees directory");
  assert.equal(layout.path("th06"), resolve(legacy, "th06-eagler"));
  assert.equal(layout.path("launcher"), web);
  layout = loadSourceLayout(web, { environment: { EAGLER_WORKSPACE_ROOT: resolve(root, "override") } });
  assert.equal(layout.path("th06"), resolve(root, "override/th06-eagler"));
  assert.equal(layout.path("launcher"), web, "an external workspace does not replace the calling Launcher");

  const mono = resolve(root, "monorepo"), nested = resolve(mono, "launcher/web");
  await mkdir(nested, { recursive: true });
  await config(resolve(mono, "config/workspace.json"), configuration("launcher/web"));
  layout = loadSourceLayout(nested, { environment: {} });
  assert.equal(layout.projectRoot, mono);
  assert.equal(layout.path("common"), resolve(mono, "eagler-common"));
  assert.equal(layout.path("launcher", "app"), resolve(nested, "app"));
  assert.throws(() => layout.path("absent"), /unknown workspace repository/);

  const standalone = resolve(root, "operator-package");
  await config(resolve(standalone, "config/workspace.json"), configuration("eagler-touhou"));
  layout = loadSourceLayout(standalone, { environment: {} });
  assert.equal(layout.projectRoot, standalone, "standalone delivery must keep its own configuration");
  assert.equal(layout.root, root);
  for (const path of ["../other", "/absolute", "D:/outside", "a\\b", "a//b", ".", "a/../b"]) {
    assert.throws(() => validateWorkspaceConfiguration({ ...configuration("launcher"), repositories: { th06: path } }), /invalid workspace repository path/);
  }
} finally {
  assert.equal(dirname(root), resolve(tmpdir()));
  await rm(root, { recursive: true, force: true });
}
console.log("Source layout: PASS (worktree, explicit root, monorepo, standalone, invalid paths)");
});
