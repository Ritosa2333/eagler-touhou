#!/usr/bin/env node
import { createHash, randomUUID } from "node:crypto";
import { lstat, readFile, rename, rm, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { basename, dirname, isAbsolute, relative, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import { isDeepStrictEqual } from "node:util";

import { validateSiteManifest } from "../../lib/contracts/site-manifest.mjs";
import { RESOURCE_MODE_EXTERNAL, RESOURCE_MODE_HOSTED } from "../../lib/contracts/resource-mode.mjs";
import { normalizeProductSelection } from "../../lib/product-selection.mjs";
import { verifyReleaseManifest } from "../../lib/release-manifest.mjs";
import { verifyRuntimeRelease } from "../../lib/runtime-release.mjs";
import { run } from "../../host/lib/process.mjs";

const project = resolve(fileURLToPath(new URL("../..", import.meta.url)));

function parseArgs(values) {
  const args = Object.create(null);
  for (const value of values) {
    const split = value.indexOf("=");
    if (!value.startsWith("--") || split < 3) throw new Error(`invalid argument: ${value}`);
    args[value.slice(2, split)] = value.slice(split + 1);
  }
  const allowed = new Set([
    "source", "runtime-release", "output", "profile", "games", "test-build",
    "netplay-relay", "origin-migration",
  ]);
  const unsupported = Object.keys(args).find(key => !allowed.has(key));
  if (unsupported) throw new Error(`unsupported argument: --${unsupported}`);
  for (const key of ["source", "runtime-release", "output", "profile"]) {
    if (!args[key]) throw new Error(`missing --${key}=VALUE`);
  }
  if (!/^web-validation-/.test(args.profile)) {
    throw new Error("maintainer External recovery requires --profile=web-validation-*; formal Release remains owned by npm run release");
  }
  if (args["test-build"] !== undefined && !["0", "1"].includes(args["test-build"])) {
    throw new Error("--test-build must be 0 or 1");
  }
  return args;
}

function resolveRelay(value, inherited) {
  if (value == null || value === "" || value === "inherit") return inherited || null;
  if (["none", "disable"].includes(value)) return null;
  let url;
  try { url = new URL(value); }
  catch { throw new Error("--netplay-relay must be inherit, none/disable, or a valid ws:// / wss:// URL"); }
  if (!/^wss?:$/.test(url.protocol) || url.username || url.password || url.hash) {
    throw new Error("--netplay-relay must be a plain ws:// or wss:// URL without credentials or fragment");
  }
  return url.href;
}

function resolveOriginMigration(value, inherited) {
  if (value == null || value === "" || value === "inherit") return inherited || null;
  if (["none", "disable"].includes(value)) return null;
  if (value === "http-to-https") return { mode: "http-to-https" };
  throw new Error("--origin-migration must be inherit, none/disable, or http-to-https");
}

function packageGenerationView(entry) {
  const { runtime: _runtime, multiplayerRuntime: _multiplayerRuntime, ...rest } = entry;
  return rest;
}

function runtimeVersion(entry) {
  if (typeof entry?.runtime !== "string") return null;
  return new URL(entry.runtime, "https://eagler.invalid/").searchParams.get("v");
}

async function pathExists(path) {
  try {
    await lstat(path);
    return true;
  } catch (error) {
    if (error?.code === "ENOENT") return false;
    throw error;
  }
}

async function bestEffortCleanup(path, options) {
  try {
    await rm(path, options);
  } catch (error) {
    console.warn(`Recovery scratch cleanup failed: ${path}: ${error?.message || error}`);
  }
}

const args = parseArgs(process.argv.slice(2));
const source = resolve(args.source);
const runtimeRelease = resolve(args["runtime-release"]);
const output = resolve(args.output);
if (source === output) throw new Error("External output must differ from Hosted source");
const sourceRelativeOutput = relative(source, output);
if (sourceRelativeOutput && !sourceRelativeOutput.startsWith("..") && !isAbsolute(sourceRelativeOutput)) {
  throw new Error("External output must not be nested inside the Hosted source");
}
if (await pathExists(output)) {
  throw new Error("External output already exists; recovery derivation requires a new output directory");
}

const sourceRelease = await verifyReleaseManifest(source);
const sourceDeployment = JSON.parse(await readFile(resolve(source, "deployment.json"), "utf8"));
if (sourceDeployment?.format !== "eagler-touhou-deployment/1" || sourceDeployment.resourceMode !== RESOURCE_MODE_HOSTED) {
  throw new Error("--source must be an integrity-verified Hosted deployment");
}
const sourceManifest = validateSiteManifest(JSON.parse(await readFile(resolve(source, "site-manifest.json"), "utf8")));
if (sourceManifest.shared.resourceMode !== RESOURCE_MODE_HOSTED) {
  throw new Error("Hosted source manifest has the wrong resource mode");
}

const games = normalizeProductSelection(args.games || sourceDeployment.games?.join(","));
if (games.some(game => !sourceManifest.games[game])) {
  throw new Error("External game selection is missing from Hosted source");
}
const runtimeManifestBytes = await readFile(resolve(runtimeRelease, "runtime-release.json"));
const runtimeReleaseManifestSha256 = createHash("sha256").update(runtimeManifestBytes).digest("hex");
const runtimeManifest = await verifyRuntimeRelease(runtimeRelease);
for (const game of games) {
  if (runtimeManifest.games?.[game]?.dataLayout !== sourceManifest.games[game].gameData.layout) {
    throw new Error(`${game}: Runtime Release data layout does not match Hosted Package generation`);
  }
}

const relay = resolveRelay(args["netplay-relay"], sourceManifest.shared.netplayRelay);
const originMigration = resolveOriginMigration(args["origin-migration"], sourceManifest.shared.originMigration);
const featureConfig = resolve(tmpdir(), `eagler-touhou-maintainer-external-${randomUUID()}.json`);
const recoveryContext = resolve(tmpdir(), `eagler-touhou-external-recovery-${randomUUID()}.json`);
const candidate = resolve(dirname(output), `.${basename(output)}.recovery-${randomUUID()}`);
const requestedOverrides = {
  netplayRelay: args["netplay-relay"] ?? "inherit",
  originMigration: args["origin-migration"] ?? "inherit",
  testBuild: args["test-build"] ?? "0",
};

try {
  await writeFile(featureConfig, `${JSON.stringify({
    schema: "eagler-touhou/server-features/1",
    resourceMode: RESOURCE_MODE_EXTERNAL,
    games: Object.fromEntries(games.map(game => [game, {
      languages: (sourceManifest.games[game].languageOptions || []).map(language => language.id).filter(Boolean),
      thprac: !!sourceManifest.games[game].features?.thprac,
    }])),
    ...(relay ? { netplayRelay: relay } : {}),
    ...(sourceManifest.shared.gameDataFallback ? { gameDataFallback: sourceManifest.shared.gameDataFallback } : {}),
    ...(originMigration ? { originMigration } : {}),
  }, null, 2)}\n`);
  await writeFile(recoveryContext, `${JSON.stringify({
    schema: "eagler-touhou/external-recovery-request/1",
    requestedOverrides,
  }, null, 2)}\n`);

  await run(process.execPath, [
    resolve(project, "scripts/package-server.mjs"),
    `--output=${candidate}`,
    `--runtime-release=${runtimeRelease}`,
    `--site-manifest=${resolve(source, "site-manifest.json")}`,
    `--artwork-dir=${resolve(source, "assets")}`,
    `--feature-config=${featureConfig}`,
    `--external-recovery-context=${recoveryContext}`,
    `--games=${games.join(",")}`,
    `--profile=${args.profile}`,
    `--test-build=${args["test-build"] || "0"}`,
  ], { cwd: project });

  const candidateManifest = validateSiteManifest(JSON.parse(await readFile(resolve(candidate, "site-manifest.json"), "utf8")));
  if (candidateManifest.shared.resourceMode !== RESOURCE_MODE_EXTERNAL ||
      (candidateManifest.shared.netplayRelay || null) !== relay ||
      !isDeepStrictEqual(candidateManifest.shared.originMigration || null, originMigration) ||
      !isDeepStrictEqual(candidateManifest.shared.gameDataFallback || null, sourceManifest.shared.gameDataFallback || null)) {
    throw new Error("External deployment overrides were not materialized as requested");
  }

  for (const game of games) {
    const sourceEntry = sourceManifest.games[game];
    const candidateEntry = candidateManifest.games[game];
    if (!isDeepStrictEqual(packageGenerationView(candidateEntry), packageGenerationView(sourceEntry))) {
      throw new Error(`${game}: External Package-generation metadata diverges from Hosted source`);
    }
    const candidateRuntimeVersion = runtimeVersion(candidateEntry);
    for (const language of candidateEntry.languages || []) {
      if (language?.pack?.runtimeVersion && language.pack.runtimeVersion !== candidateRuntimeVersion) {
        throw new Error(`${game}: External Runtime version does not match retained language-pack generation`);
      }
    }
    const descriptorName = sourceEntry.package?.descriptor;
    if (!descriptorName) throw new Error(`${game}: Hosted source is missing Package Descriptor metadata`);
    const [sourceDescriptor, candidateDescriptor] = await Promise.all([
      readFile(resolve(source, descriptorName), "utf8"),
      readFile(resolve(candidate, descriptorName), "utf8"),
    ]);
    if (!isDeepStrictEqual(JSON.parse(candidateDescriptor), JSON.parse(sourceDescriptor))) {
      throw new Error(`${game}: External Package Descriptor diverges from Hosted source generation`);
    }
  }

  const candidateRelease = await verifyReleaseManifest(candidate);
  const expectedRecoveryProvenance = {
    schema: "eagler-touhou/external-recovery-context/1",
    sourceHostedReleaseId: sourceRelease.releaseId,
    runtimeReleaseManifestSha256,
    games,
    requestedOverrides,
    effectiveDeployment: {
      netplayRelay: relay,
      originMigration,
      testBuild: args["test-build"] === "1",
    },
  };
  if (candidateRelease.parameters?.runtimeBuildProvenance !== "verified-runtime-release" ||
      !isDeepStrictEqual(candidateRelease.parameters?.externalRecovery, expectedRecoveryProvenance)) {
    throw new Error("External recovery provenance was not generated from the verified recovery inputs");
  }
  await run(process.execPath, [resolve(project, "scripts/verify-server-build.mjs"), candidate], { cwd: project });
  await rename(candidate, output);
  const release = await verifyReleaseManifest(output);
  console.log(JSON.stringify({ output, releaseId: release.releaseId, games, netplayRelay: relay, originMigration }));
} finally {
  await bestEffortCleanup(featureConfig, { force: true });
  await bestEffortCleanup(recoveryContext, { force: true });
  await bestEffortCleanup(candidate, { recursive: true, force: true });
}
