import assert from "node:assert/strict";
import { execFileSync } from "node:child_process";
import { mkdir, mkdtemp, rm, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { dirname, resolve } from "node:path";
import { sourceIdentity } from "../lib/release-manifest.mjs";

const root = await mkdtemp(resolve(tmpdir(), "eagler-source-identity-"));
const repo = resolve(root, "monorepo"), game = resolve(repo, "games/game");
const git = (...args) => execFileSync("git", ["-C", repo, ...args], { encoding: "utf8", windowsHide: true });
try {
  await mkdir(game, { recursive: true });
  await mkdir(resolve(repo, "games/other"), { recursive: true });
  git("init", "--quiet");
  await writeFile(resolve(game, "engine.cpp"), "original\n");
  await writeFile(resolve(repo, "games/other/engine.cpp"), "other\n");
  git("add", ".");
  git("-c", "user.name=Fixture", "-c", "user.email=fixture@example.invalid", "commit", "--quiet", "-m", "fixture");
  const clean = await sourceIdentity(game);
  assert.equal(clean.sourcePath, "games/game");
  assert.equal(clean.revision, git("rev-parse", "HEAD").trim());
  assert.equal(clean.dirty, false);
  assert.notEqual(clean.sha256, (await sourceIdentity(resolve(repo, "games/other"))).sha256);

  await writeFile(resolve(repo, "games/other/engine.cpp"), "unrelated dirty change\n");
  git("add", "games/other");
  await writeFile(resolve(repo, "games/other/new.cpp"), "unrelated new input\n");
  assert.deepEqual(await sourceIdentity(game), clean, "sibling changes must not become component build inputs");
  await writeFile(resolve(game, "engine.cpp"), "unstaged component change\n");
  const unstaged = await sourceIdentity(game);
  assert.equal(unstaged.dirty, true);
  assert.notEqual(unstaged.unstagedSha256, clean.unstagedSha256);
  git("add", "games/game");
  const staged = await sourceIdentity(game);
  assert.notEqual(staged.stagedSha256, clean.stagedSha256);
  await writeFile(resolve(game, "new.cpp"), "new input\n");
  const added = await sourceIdentity(game);
  assert.deepEqual(added.untracked.map(file => file.path), ["new.cpp"]);
  await writeFile(resolve(game, "new.cpp"), "different new input\n");
  assert.notEqual((await sourceIdentity(game)).sha256, added.sha256);

  const dependency = resolve(root, "dependency");
  await mkdir(dependency);
  execFileSync("git", ["-C", dependency, "init", "--quiet"]);
  await writeFile(resolve(dependency, "shared.hpp"), "dependency\n");
  execFileSync("git", ["-C", dependency, "add", "."]);
  execFileSync("git", ["-C", dependency, "-c", "user.name=Fixture", "-c", "user.email=fixture@example.invalid", "commit", "--quiet", "-m", "fixture"]);
  git("-c", "protocol.file.allow=always", "submodule", "add", "--quiet", dependency, "games/game/vendor/common");
  const withSubmodule = await sourceIdentity(game);
  assert.equal(withSubmodule.submodules[0].path, "vendor/common");
  assert.equal(withSubmodule.submodules[0].initialized, true);
  await writeFile(resolve(game, "vendor/common/shared.hpp"), "modified dependency\n");
  const dirtySubmodule = await sourceIdentity(game);
  assert.equal(dirtySubmodule.submodules[0].dirty, true);
  assert.notEqual(dirtySubmodule.sha256, withSubmodule.sha256);
  const whole = await sourceIdentity(repo);
  assert.equal(whole.sourcePath, undefined, "standalone repository identity keeps the existing shape");
} finally {
  assert.equal(dirname(root), resolve(tmpdir()));
  await rm(root, { recursive: true, force: true });
}
console.log("Source identity: PASS (nested component, dirty inputs, sibling isolation, submodule dependency)");
