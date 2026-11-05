import { createHash } from "node:crypto";
import { existsSync, readdirSync } from "node:fs";
import { readFile, writeFile } from "node:fs/promises";
import { extname, resolve } from "node:path";
import { run } from "./process.mjs";
import {ensureBunDependencies, usesBunLock} from './bun-environment.mjs';

// Publication verifies prebuilt JS/HTML with operator parsers; no React Router,
// TypeScript or other maintainer compiler is required by packaged operators.
const PREBUILT_REQUIRED = Object.freeze(["acorn", "fflate", "parse5", "workbox-build"]);
const SOURCE_REQUIRED = Object.freeze([...PREBUILT_REQUIRED, "typescript"]);
const MINIMUM_NODE_MAJOR = 22;
const STAMP_FILE = ".eagler-host-lock.json";

export function isSupportedNodeVersion(version = process.version) {
  const major = Number.parseInt(String(version).replace(/^v/i, "").split(".")[0], 10);
  return Number.isInteger(major) && major >= MINIMUM_NODE_MAJOR;
}

export function assertSupportedNode(version = process.version) {
  if (!isSupportedNodeVersion(version)) {
    throw new Error(`Node.js >= ${MINIMUM_NODE_MAJOR} is required for self-hosting; current version is ${version}`);
  }
}

function containsTypeScriptSource(directory) {
  if (!existsSync(directory)) return false;
  return readdirSync(directory, { withFileTypes: true }).some(entry => {
    const path = resolve(directory, entry.name);
    return entry.isDirectory()
      ? containsTypeScriptSource(path)
      : entry.isFile() && extname(entry.name) === ".mts";
  });
}

export function requiredNodeDependencies(projectRoot) {
  return containsTypeScriptSource(resolve(projectRoot, "src")) ? SOURCE_REQUIRED : PREBUILT_REQUIRED;
}

async function lockIdentity(projectRoot) {
  const path = resolve(projectRoot, "package-lock.json");
  const bytes = await readFile(path);
  return createHash("sha256").update(bytes).digest("hex");
}

export async function installedEnvironmentMatches(projectRoot, required, identity) {
  const stampPath = resolve(projectRoot, "node_modules", STAMP_FILE);
  let stamp, currentIdentity, manifestSha256;
  try {
    stamp = JSON.parse(await readFile(stampPath, "utf8"));
    currentIdentity = await lockIdentity(projectRoot);
    manifestSha256 = createHash('sha256').update(await readFile(resolve(projectRoot, 'package.json'))).digest('hex');
  }
  catch { return false; }
  if (stamp.schema !== 'eagler-touhou/node-environment/1' ||
      stamp.lockSha256 !== identity || currentIdentity !== identity ||
      stamp.manifestSha256 !== manifestSha256 ||
      stamp.nodeMajor !== Number.parseInt(process.versions.node, 10) ||
      JSON.stringify(stamp.required) !== JSON.stringify(required)) return false;

  let lock;
  try { lock = JSON.parse(await readFile(resolve(projectRoot, "package-lock.json"), "utf8")); }
  catch { return false; }
  for (const name of required) {
    try {
      const expected = lock.packages?.[`node_modules/${name}`]?.version;
      const actual = JSON.parse(await readFile(resolve(projectRoot, "node_modules", name, "package.json"), "utf8")).version;
      if (!expected || expected !== actual) return false;
    } catch { return false; }
  }
  return true;
}

async function writeEnvironmentStamp(projectRoot, required, identity) {
  const stamp = {
    schema: "eagler-touhou/node-environment/1",
    lockSha256: identity,
    manifestSha256: createHash('sha256').update(await readFile(resolve(projectRoot, 'package.json'))).digest('hex'),
    nodeMajor: Number.parseInt(process.versions.node, 10),
    required,
  };
  await writeFile(resolve(projectRoot, "node_modules", STAMP_FILE), `${JSON.stringify(stamp, null, 2)}\n`);
}

export async function ensureNodeDependencies(projectRoot) {
  if (usesBunLock(projectRoot)) return ensureBunDependencies(projectRoot, [...requiredNodeDependencies(projectRoot)]);
  assertSupportedNode();
  if (!existsSync(resolve(projectRoot, "package-lock.json"))) {
    throw new Error("package-lock.json is missing; refusing to install unpinned Node.js dependencies");
  }
  const required = [...requiredNodeDependencies(projectRoot)];
  const identity = await lockIdentity(projectRoot);
  const manifest = await readFile(resolve(projectRoot, 'package.json'), 'utf8');
  if (await installedEnvironmentMatches(projectRoot, required, identity)) return;

  console.log(`[Build] Refreshing locked Node.js dependencies (${required.join(", ")})`);
  // npm.cmd is a batch entrypoint and cannot be spawned directly on Windows.
  // The interpreter receives only this fixed command; the path stays in cwd.
  if (process.platform === 'win32') {
    await run(process.env.ComSpec || 'cmd.exe', ['/d', '/s', '/c', 'npm.cmd ci --ignore-scripts --no-audit --no-fund'], {cwd: projectRoot});
  } else {
    await run('npm', ['ci', '--ignore-scripts', '--no-audit', '--no-fund'], {cwd: projectRoot});
  }
  if (!await installedEnvironmentMatchesAfterInstall(projectRoot, required)) {
    throw new Error(`npm ci completed but the required dependency set is incomplete or mismatched: ${required.join(", ")}`);
  }
  if (await lockIdentity(projectRoot) !== identity) throw new Error('Node dependency lock changed during installation; retry with the current lockfile');
  if (await readFile(resolve(projectRoot, 'package.json'), 'utf8') !== manifest) throw new Error('Node dependency manifest changed during installation; retry with the current manifest');
  await writeEnvironmentStamp(projectRoot, required, identity);
}

async function installedEnvironmentMatchesAfterInstall(projectRoot, required) {
  let lock;
  try { lock = JSON.parse(await readFile(resolve(projectRoot, "package-lock.json"), "utf8")); }
  catch { return false; }
  for (const name of required) {
    try {
      const expected = lock.packages?.[`node_modules/${name}`]?.version;
      const actual = JSON.parse(await readFile(resolve(projectRoot, "node_modules", name, "package.json"), "utf8")).version;
      if (!expected || expected !== actual) return false;
    } catch { return false; }
  }
  return true;
}
