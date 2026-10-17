#!/usr/bin/env node
import { createHash } from "node:crypto";
import { readFile, writeFile } from "node:fs/promises";
import { resolve } from "node:path";

const hash = bytes => createHash("sha256").update(bytes).digest("hex");

async function refreshReleaseIntegrity(site, manifestBytes) {
  const deploymentPath = resolve(site, "deployment.json");
  const releaseManifestPath = resolve(site, "release-manifest.json");
  const checksumsPath = resolve(site, "checksums.txt");
  const deployment = JSON.parse(await readFile(deploymentPath, "utf8"));
  if (deployment?.format !== "eagler-touhou-deployment/1" || !Array.isArray(deployment.files)) {
    throw new Error("target deployment metadata is invalid");
  }
  const hostEntry = deployment.files.find(file => file?.path === "site-manifest.json");
  if (!hostEntry) throw new Error("site-manifest.json is missing from deployment inventory");
  hostEntry.bytes = manifestBytes.length;
  hostEntry.sha256 = hash(manifestBytes);
  const deploymentBytes = Buffer.from(`${JSON.stringify(deployment, null, 2)}\n`);
  await writeFile(deploymentPath, deploymentBytes);

  const previous = JSON.parse(await readFile(releaseManifestPath, "utf8"));
  if (previous?.schema !== "eagler-touhou/release-manifest/1" || !previous.profile ||
      !previous.sources || !Object.keys(previous.sources).length) {
    throw new Error("target release provenance is invalid");
  }
  const inventory = [...deployment.files].sort((left, right) => left.path.localeCompare(right.path, "en"));
  const identity = {
    profile: previous.profile,
    sources: previous.sources,
    parameters: previous.parameters || {},
    files: inventory,
    deploymentSha256: hash(deploymentBytes),
  };
  const release = {
    schema: "eagler-touhou/release-manifest/1",
    releaseId: `sha256-${hash(JSON.stringify(identity))}`,
    ...identity,
  };
  const releaseBytes = Buffer.from(`${JSON.stringify(release, null, 2)}\n`);
  await writeFile(releaseManifestPath, releaseBytes);
  const checksums = [
    ...inventory.map(file => `${file.sha256}  ${file.path}`),
    `${identity.deploymentSha256}  deployment.json`,
    `${hash(releaseBytes)}  release-manifest.json`,
  ];
  await writeFile(checksumsPath, `${checksums.join("\n")}\n`);
  return release.releaseId;
}

const options = Object.create(null);
for (const argument of process.argv.slice(2)) {
  const index = argument.indexOf("=");
  if (!argument.startsWith("--") || index < 3) throw new Error(`invalid argument: ${argument}`);
  options[argument.slice(2, index)] = argument.slice(index + 1);
}
const allowedKeys = new Set(["site", "relay", "origin-migration"]);
if (!options.site || Object.keys(options).some(key => !allowedKeys.has(key)) ||
    (!options.relay && !options["origin-migration"])) {
  throw new Error("usage: node services/configure-deployed-site.mjs --site=DEPLOYED_SITE " +
    "[--relay=wss://HOST/eagler-netplay/] [--origin-migration=enable|disable]");
}

let relay = null;
if (options.relay) {
  relay = new URL(options.relay);
  if (!['wss:', 'ws:'].includes(relay.protocol) || relay.username || relay.password || relay.hash) {
    throw new Error("relay must be a plain ws:// or wss:// URL without credentials or fragment");
  }
}
const originMigration = options["origin-migration"] || null;
if (originMigration && !["enable", "disable"].includes(originMigration)) {
  throw new Error("origin-migration must be enable or disable");
}

const manifestPath = resolve(options.site, "site-manifest.json");
const manifest = JSON.parse(await readFile(manifestPath, "utf8"));
if (manifest?.schema !== "eagler-touhou/site-manifest/1" || manifest?.shared?.resourceMode !== "hosted") {
  throw new Error("target is not an Eagler Touhou Hosted site");
}
if (relay) manifest.shared.netplayRelay = relay.href;
if (originMigration === "enable") manifest.shared.originMigration = { mode: "http-to-https" };
if (originMigration === "disable") delete manifest.shared.originMigration;
const manifestBytes = Buffer.from(`${JSON.stringify(manifest, null, 2)}\n`);
await writeFile(manifestPath, manifestBytes);
const releaseId = await refreshReleaseIntegrity(resolve(options.site), manifestBytes);
if (relay) console.log(`Configured deployed Relay: ${relay.href}`);
if (originMigration) console.log(`Origin migration: ${originMigration}`);
console.log(`Release integrity refreshed: ${releaseId}`);
