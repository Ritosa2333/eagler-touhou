#!/usr/bin/env node
import { randomUUID } from "node:crypto";
import { readFile, readdir, rename, rm, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { basename, dirname, relative, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import { normalizeProductSelection } from "../lib/product-selection.mjs";
import { validateHostManifest } from "../lib/contracts/host-manifest.mjs";
import { run } from "../host/lib/process.mjs";

const project = resolve(fileURLToPath(new URL("..", import.meta.url)));
const args = Object.fromEntries(process.argv.slice(2).map(value => {
  const split = value.indexOf("=");
  if (!value.startsWith("--") || split < 3) throw new Error(`invalid argument: ${value}`);
  return [value.slice(2, split), value.slice(split + 1)];
}));
const required = name => {
  if (!args[name]) throw new Error(`missing --${name}=PATH`);
  return resolve(args[name]);
};

const source = required("source");
const output = required("output");
const runtimeRelease = required("runtime-release");
const candidate = resolve(dirname(output), `.${basename(output)}.external-${randomUUID()}`);
const profile = String(args.profile || "");
if (!/^web-(?:validation|release)-/.test(profile)) throw new Error("external packaging requires an explicit web-validation-* or web-release-* --profile=NAME");
if (source === output) throw new Error("external output must differ from its hosted source");
if (args["test-build"] !== undefined && !["0", "1"].includes(args["test-build"])) throw new Error("--test-build must be 0 or 1");

await run(process.execPath, [resolve(project, "scripts/verify-server-build.mjs"), source], { cwd: project });
const sourceDeployment = JSON.parse(await readFile(resolve(source, "deployment.json"), "utf8"));
if (sourceDeployment.resourceMode !== "hosted") throw new Error("external source must be a verified hosted deployment");
const sourceManifest = validateHostManifest(JSON.parse(await readFile(resolve(source, "host-manifest.json"), "utf8")));
const games = normalizeProductSelection(args.games || sourceDeployment.games?.join(","));
if (games.some(game => !sourceManifest.games[game])) throw new Error("external selection is missing from the hosted source");
const runtimeManifest = JSON.parse(await readFile(resolve(runtimeRelease, "runtime-release.json"), "utf8"));
for (const game of games) {
  if (runtimeManifest.games?.[game]?.dataLayout !== sourceManifest.games[game].gameData.layout) {
    throw new Error(`${game}: Runtime Release data layout does not match the hosted Package source`);
  }
}

async function runtimeInventory(root) {
  const files = new Map();
  async function walk(directory) {
    for (const entry of await readdir(directory, { withFileTypes: true })) {
      const path = resolve(directory, entry.name);
      if (entry.isDirectory()) await walk(path);
      else if (entry.isFile()) files.set(relative(root, path).replaceAll("\\", "/"), await readFile(path));
      else throw new Error(`unsupported Runtime entry: ${path}`);
    }
  }
  for (const game of games) await walk(resolve(root, "runtime", game));
  return files;
}

async function assertMatchingRuntimeTrees(hostedRoot, externalRoot) {
  const [hostedFiles, externalFiles] = await Promise.all([
    runtimeInventory(hostedRoot),
    runtimeInventory(externalRoot),
  ]);
  if (JSON.stringify([...hostedFiles.keys()].sort()) !== JSON.stringify([...externalFiles.keys()].sort())) {
    throw new Error("external Runtime file set does not match its hosted source");
  }
  for (const [path, bytes] of hostedFiles) {
    if (!bytes.equals(externalFiles.get(path))) throw new Error(`external Runtime identity does not match hosted source: ${path}`);
  }
}

const featureConfig = resolve(tmpdir(), `eagler-touhou-external-features-${randomUUID()}.json`);
try {
  await writeFile(featureConfig, `${JSON.stringify({
    schema: "eagler-touhou/server-features/1",
    resourceMode: "external",
    games: Object.fromEntries(games.map(game => [game, {
      languages: (sourceManifest.games[game].languageOptions || []).map(language => language.id).filter(Boolean),
      thprac: !!sourceManifest.games[game].features?.thprac,
    }])),
    ...(sourceManifest.shared.netplayRelay ? { netplayRelay: sourceManifest.shared.netplayRelay } : {}),
    ...(sourceManifest.shared.originMigration ? { originMigration: sourceManifest.shared.originMigration } : {}),
  }, null, 2)}\n`);
  await run(process.execPath, [
    resolve(project, "scripts/package-server.mjs"),
    `--output=${candidate}`,
    `--runtime-release=${runtimeRelease}`,
    `--previous-site=${source}`,
    `--host-manifest=${resolve(source, "host-manifest.json")}`,
    `--artwork-dir=${resolve(source, "assets")}`,
    `--feature-config=${featureConfig}`,
    `--games=${games.join(",")}`,
    `--profile=${profile}`,
    `--test-build=${args["test-build"] || "0"}`,
  ], { cwd: project });
  await run(process.execPath, [resolve(project, "scripts/verify-server-build.mjs"), candidate], { cwd: project });
  await assertMatchingRuntimeTrees(source, candidate);
  await rm(output, { recursive: true, force: true });
  await rename(candidate, output);
} finally {
  await rm(featureConfig, { force: true });
  await rm(candidate, { recursive: true, force: true });
}
