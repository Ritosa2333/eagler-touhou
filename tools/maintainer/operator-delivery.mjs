import { createHash } from "node:crypto";
import { readFile, readdir, writeFile } from "node:fs/promises";
import { isAbsolute, relative, resolve } from "node:path";

import { PRODUCT_GAMES } from "../../lib/contracts/product-catalog.mjs";
import { verifyReleaseManifest } from "../../lib/release-manifest.mjs";

export const OPERATOR_PROFILE_SCHEMA = "eagler-touhou/internal-operator-profile/1";
export const OPERATOR_MANIFEST_SCHEMA = "eagler-touhou/internal-operator-delivery/1";
const REQUIRED_OPERATOR_FILES = Object.freeze([
  "AGENTS.md",
  "README.md",
  "operator-profile.json",
  "verification-report.json",
  "site/host-manifest.json",
  "site/deployment.json",
  "site/release-manifest.json",
  "site/checksums.txt",
  "services/README.md",
  "services/configure-deployed-site.mjs",
  "services/nginx/eagler-touhou-site.conf.example",
  "services/nginx/nginx.migration-window.conf.example",
  "services/nginx/nginx.final-https.conf.example",
  "services/relay/package.json",
  "services/relay/package-lock.json",
  "services/relay/relay.env.example",
  "services/relay/eagler-netplay-relay.service",
  "services/relay/server/netplay-relay.mjs",
  "services/relay/server/room-probe-policy.mjs",
  "services/relay/server/spectator-frame.mjs",
  "services/turn/render-coturn-config.cjs",
  "services/turn/turn.env.example",
  "services/verify/verify-origin-cutover.mjs",
  "services/verify/verify-hsts-cutover.mjs",
  "services/verify/verify-public-relay-fallback.mjs",
  "services/verify/verify-public-targeted-relay.mjs",
]);

export const sha256 = bytes => createHash("sha256").update(bytes).digest("hex");

function sameList(actual, expected) {
  return Array.isArray(actual) && Array.isArray(expected) &&
    actual.length === expected.length && actual.every((value, index) => value === expected[index]);
}

export function validateOperatorProfile(profile) {
  if (!profile || profile.schema !== OPERATOR_PROFILE_SCHEMA || profile.resourceMode !== "hosted" ||
      typeof profile.requireGameDataFallback !== "boolean" ||
      !["absent", "configured"].includes(profile.netplayRelay) ||
      ![null, "http-to-https"].includes(profile.originMigration)) {
    throw new Error("invalid internal Operator profile");
  }
  const games = Object.keys(PRODUCT_GAMES);
  if (!profile.games || !sameList(Object.keys(profile.games), games)) {
    throw new Error("internal Operator profile must contain every registered game in canonical order");
  }
  for (const game of games) {
    const entry = profile.games[game];
    if (!entry || !Array.isArray(entry.languages) || !entry.languages.length || entry.languages[0] !== "ja" ||
        new Set(entry.languages).size !== entry.languages.length ||
        !Array.isArray(entry.music) || !entry.music.length || new Set(entry.music).size !== entry.music.length ||
        typeof entry.thprac !== "boolean" || !Array.isArray(entry.runtimeVariants) ||
        !entry.runtimeVariants.length || entry.runtimeVariants.some(value => !["normal", "multiplayer"].includes(value))) {
      throw new Error(`invalid internal Operator game profile: ${game}`);
    }
  }
  return profile;
}

export function assertHostedSiteMatchesOperatorProfile(manifest, profileInput) {
  const profile = validateOperatorProfile(profileInput);
  if (manifest?.schema !== "eagler-touhou/host-manifest/1" || manifest.shared?.resourceMode !== profile.resourceMode) {
    throw new Error("Operator input is not a Hosted Eagler Touhou site");
  }
  const fallback = manifest.shared?.gameDataFallback;
  if (profile.requireGameDataFallback && (!fallback || !/^https:\/\//.test(fallback.url || ""))) {
    throw new Error("Operator profile requires an HTTPS game-data fallback");
  }
  const hasRelay = typeof manifest.shared?.netplayRelay === "string" && manifest.shared.netplayRelay.length > 0;
  if ((profile.netplayRelay === "configured") !== hasRelay) {
    throw new Error(`Operator profile requires Netplay Relay to be ${profile.netplayRelay}`);
  }
  const actualMigration = manifest.shared?.originMigration?.mode ?? null;
  if (actualMigration !== profile.originMigration) {
    throw new Error(`Operator origin migration mismatch: expected ${profile.originMigration ?? "absent"}, got ${actualMigration ?? "absent"}`);
  }
  for (const [game, expected] of Object.entries(profile.games)) {
    const actual = manifest.games?.[game];
    const languages = actual?.languageOptions?.map(entry => entry?.id);
    const music = Object.keys(actual?.music || {});
    if (!sameList(languages, expected.languages)) {
      throw new Error(`${game}: Operator language profile mismatch; expected ${expected.languages.join(",")}, got ${(languages || []).join(",")}`);
    }
    if (!sameList(music, expected.music)) {
      throw new Error(`${game}: Operator music profile mismatch; expected ${expected.music.join(",")}, got ${music.join(",")}`);
    }
    if (actual?.features?.thprac !== expected.thprac) {
      throw new Error(`${game}: Operator thprac profile mismatch`);
    }
    if (!actual?.package?.descriptor) throw new Error(`${game}: Operator Hosted Package descriptor missing`);
    if (expected.runtimeVariants.includes("normal") && typeof actual.runtime !== "string") {
      throw new Error(`${game}: Operator normal Runtime missing`);
    }
    if (expected.runtimeVariants.includes("multiplayer") && typeof actual.multiplayerRuntime !== "string") {
      throw new Error(`${game}: Operator multiplayer Runtime missing`);
    }
  }
  return profile;
}

function safeFile(root, name) {
  if (typeof name !== "string" || !name || name.includes("\\") || isAbsolute(name) ||
      name.split("/").some(part => !part || part === "." || part === "..")) {
    throw new Error(`unsafe Operator path: ${name}`);
  }
  const path = resolve(root, name);
  if (relative(root, path).startsWith("..")) throw new Error(`Operator path escapes root: ${name}`);
  return path;
}

export async function inventoryOperatorFiles(root, excluded = new Set()) {
  const files = [];
  async function visit(directory) {
    for (const entry of (await readdir(directory, { withFileTypes: true })).sort((a, b) => a.name.localeCompare(b.name, "en"))) {
      const path = resolve(directory, entry.name);
      const name = relative(root, path).replaceAll("\\", "/");
      if (excluded.has(name)) continue;
      if (entry.isSymbolicLink()) throw new Error(`Operator delivery must not contain symlinks: ${name}`);
      if (entry.isDirectory()) await visit(path);
      else if (entry.isFile()) {
        const bytes = await readFile(path);
        files.push({ path: name, bytes: bytes.length, sha256: sha256(bytes) });
      } else throw new Error(`unsupported Operator entry: ${name}`);
    }
  }
  await visit(resolve(root));
  return files.sort((left, right) => left.path.localeCompare(right.path, "en"));
}

export async function writeOperatorIntegrity(root, { formalReleaseId, siteReleaseId, profile }) {
  const profileBytes = await readFile(resolve(root, "operator-profile.json"));
  const files = await inventoryOperatorFiles(root, new Set(["operator-manifest.json", "operator-checksums.txt"]));
  const identity = {
    formalReleaseId,
    siteReleaseId,
    profileSha256: sha256(profileBytes),
    files,
  };
  const manifest = {
    schema: OPERATOR_MANIFEST_SCHEMA,
    operatorId: `sha256-${sha256(JSON.stringify(identity))}`,
    ...identity,
    capabilities: validateOperatorProfile(profile),
  };
  const manifestBytes = Buffer.from(`${JSON.stringify(manifest, null, 2)}\n`);
  await writeFile(resolve(root, "operator-manifest.json"), manifestBytes);
  await writeFile(resolve(root, "operator-checksums.txt"), `${[
    ...files.map(file => `${file.sha256}  ${file.path}`),
    `${sha256(manifestBytes)}  operator-manifest.json`,
  ].join("\n")}\n`);
  return manifest;
}

export async function verifyOperatorDelivery(root) {
  const target = resolve(root);
  const manifestBytes = await readFile(resolve(target, "operator-manifest.json"));
  const manifest = JSON.parse(manifestBytes);
  const profileBytes = await readFile(resolve(target, "operator-profile.json"));
  const profile = validateOperatorProfile(JSON.parse(profileBytes));
  const { schema, operatorId, capabilities, ...identity } = manifest;
  if (schema !== OPERATOR_MANIFEST_SCHEMA || operatorId !== `sha256-${sha256(JSON.stringify(identity))}` ||
      JSON.stringify(capabilities) !== JSON.stringify(profile) || identity.profileSha256 !== sha256(profileBytes)) {
    throw new Error("Operator manifest identity mismatch");
  }
  const files = await inventoryOperatorFiles(target, new Set(["operator-manifest.json", "operator-checksums.txt"]));
  if (JSON.stringify(files) !== JSON.stringify(identity.files)) throw new Error("Operator file inventory mismatch");
  const paths = new Set(files.map(file => file.path));
  for (const required of REQUIRED_OPERATOR_FILES) {
    if (!paths.has(required)) throw new Error(`Operator required file missing: ${required}`);
  }
  const expectedChecksums = `${[
    ...files.map(file => `${file.sha256}  ${file.path}`),
    `${sha256(manifestBytes)}  operator-manifest.json`,
  ].join("\n")}\n`;
  if (await readFile(resolve(target, "operator-checksums.txt"), "utf8") !== expectedChecksums) {
    throw new Error("Operator checksum list mismatch");
  }
  const siteManifest = JSON.parse(await readFile(resolve(target, "site", "host-manifest.json"), "utf8"));
  assertHostedSiteMatchesOperatorProfile(siteManifest, profile);
  const siteRelease = await verifyReleaseManifest(resolve(target, "site"));
  if (siteRelease.releaseId !== identity.siteReleaseId) throw new Error("Operator Hosted site release identity mismatch");
  const report = JSON.parse(await readFile(resolve(target, "verification-report.json"), "utf8"));
  if (report?.completion?.["BUILD-VERIFIED"] !== "yes" || report.completion?.["STRUCTURE-VERIFIED"] !== "yes") {
    throw new Error("Operator source Release lacks build/structure verification");
  }
  return { operatorId, formalReleaseId: identity.formalReleaseId, siteReleaseId: identity.siteReleaseId, profile };
}
