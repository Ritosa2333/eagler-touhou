#!/usr/bin/env node
// Local release owner. It composes existing builders and verifiers and never
// uploads files or changes a deployed server.
import { spawn } from "node:child_process";
import { createHash, randomUUID } from "node:crypto";
import { createReadStream, existsSync } from "node:fs";
import { cp, lstat, mkdir, readFile, readdir, rename, rm, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { basename, dirname, relative, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import { COMPLETION_REPORT_SCHEMA, validateCompletionReport } from "../lib/completion-report.mjs";
import {
  formalReleaseSourceOwners,
  normalizeFormalReleaseInput,
  RELEASE_PATH_INPUT_KEYS,
  RELEASE_PATH_MAP_INPUT_KEYS,
} from "../lib/release-plan.mjs";
import { verifyRuntimeRelease } from "../lib/runtime-release.mjs";
import { WORKSPACE_REPOSITORIES, workspacePath, workspaceRoot } from "../lib/workspace-layout.mjs";
import { sourceIdentity, writeReleaseManifest } from "../lib/release-manifest.mjs";
import { HOST_MANIFEST_FILE } from "../lib/contracts/host-manifest.mjs";
import { verifyReleaseBundle } from "../lib/release-bundle-verifier.mjs";

const project = resolve(fileURLToPath(new URL("..", import.meta.url)));
const workspace = workspaceRoot();
function usage() {
  console.log([
    "node scripts/release.mjs --input=release-input.json --output=NEW_DIRECTORY",
    "Input schema: eagler-touhou/release-input/1",
    "The prepare object uses tools/maintainer/assemble-site.ps1 parameter names.",
    "Full hosted inputs are required. This command performs no upload.",
  ].join("\n"));
}

function parseOptions(argv) {
  const result = {};
  for (const argument of argv) {
    const split = argument.indexOf("=");
    if (!argument.startsWith("--") || split < 3) throw new Error(`invalid argument: ${argument}`);
    result[argument.slice(2, split)] = argument.slice(split + 1);
  }
  return result;
}

const sha256 = bytes => createHash("sha256").update(bytes).digest("hex");
const candidateCompletion = Object.freeze({
  IMPLEMENTED: "yes",
  "BUILD-VERIFIED": "yes",
  "STRUCTURE-VERIFIED": "yes",
  "RUNTIME-VERIFIED": "pending",
  "SEMANTIC-VERIFIED": "pending",
  "HUMAN-ACCEPTED": "pending",
  RELEASED: "no",
});

async function runStep(report, name, command, args, environment = {}) {
  console.log(`release: ${name}`);
  const startedAt = Date.now();
  await new Promise((resolveStep, rejectStep) => {
    const child = spawn(command, args, {
      cwd: project,
      stdio: "inherit",
      shell: false,
      env: { ...process.env, ...environment },
    });
    child.once("error", rejectStep);
    child.once("exit", (code, signal) => code === 0
      ? resolveStep()
      : rejectStep(new Error(`release step failed: ${name} (${signal || code})`)));
  });
  report.push({ name, status: "PASS", elapsedMs: Date.now() - startedAt });
}

async function identifySources() {
  const result = {};
  for (const owner of formalReleaseSourceOwners()) {
    result[WORKSPACE_REPOSITORIES[owner]] = await sourceIdentity(workspacePath(owner));
  }
  return result;
}

async function identifyInputs(inputPath, prepare) {
  const files = [];
  async function visit(path, label) {
    const info = await lstat(path);
    if (info.isSymbolicLink()) throw new Error(`release input symlink must be resolved explicitly: ${label}`);
    if (info.isDirectory()) {
      for (const name of (await readdir(path)).sort()) await visit(resolve(path, name), `${label}/${name}`);
      return;
    }
    if (!info.isFile()) throw new Error(`unsupported release input: ${label}`);
    const hash = createHash("sha256");
    let bytes = 0;
    for await (const chunk of createReadStream(path)) {
      hash.update(chunk);
      bytes += chunk.length;
    }
    files.push({ path: label.replaceAll("\\", "/"), bytes, sha256: hash.digest("hex") });
  }
  for (const key of RELEASE_PATH_INPUT_KEYS) {
    if (prepare[key]) await visit(prepare[key], key);
  }
  for (const key of RELEASE_PATH_MAP_INPUT_KEYS) {
    for (const [game, path] of Object.entries(prepare[key] || {})) await visit(path, `${key}/${game}`);
  }
  await visit(inputPath, "release-input.json");
  return { scope: "explicit release input files", files, sha256: sha256(JSON.stringify(files)) };
}

async function inventory(directory, root = directory, files = []) {
  for (const entry of await readdir(directory, { withFileTypes: true })) {
    const path = resolve(directory, entry.name);
    if (entry.isDirectory()) await inventory(path, root, files);
    else {
      const bytes = await readFile(path);
      files.push({
        path: relative(root, path).replaceAll("\\", "/"),
        bytes: bytes.length,
        sha256: sha256(bytes),
      });
    }
  }
  return files;
}

if (process.argv.includes("--help")) {
  usage();
  process.exit(0);
}
const options = parseOptions(process.argv.slice(2));
if (!options.input || !options.output || Object.keys(options).some(key => !["input", "output"].includes(key))) {
  throw new Error("use --input=FILE --output=NEW_DIRECTORY (or --help)");
}

const inputPath = resolve(options.input);
const destination = resolve(options.output);
if (existsSync(destination)) throw new Error(`release output must not already exist: ${destination}`);
const input = JSON.parse(await readFile(inputPath, "utf8"));
const plan = normalizeFormalReleaseInput(input, {
  inputDirectory: dirname(inputPath),
  workspace,
  windir: process.env.WINDIR || "C:/Windows",
});
const releaseGames = plan.games;
const prepare = { ...plan.prepare };
for (const key of RELEASE_PATH_INPUT_KEYS) {
  if (!prepare[key]) continue;
  if (!existsSync(prepare[key])) throw new Error(`input path missing: ${key}`);
}
for (const key of RELEASE_PATH_MAP_INPUT_KEYS) {
  for (const [game, path] of Object.entries(prepare[key] || {})) {
    if (!existsSync(path)) throw new Error(`input path missing: ${key}.${game}`);
  }
}
await verifyRuntimeRelease(prepare.RuntimeRelease);

const features = JSON.parse(await readFile(prepare.FeatureConfig, "utf8"));
if ((features.resourceMode || "hosted") !== "hosted") {
  throw new Error("full release requires hosted input; external and import sites are derived from it");
}
if (!Array.isArray(prepare.Music) || !prepare.Music.length) {
  throw new Error("prepare.Music must explicitly list the requested modes");
}
if (input.gameDataFallback != null && (!/^https:\/\//.test(input.gameDataFallback.url || "") ||
    (input.gameDataFallback.hint != null && typeof input.gameDataFallback.hint !== "string"))) {
  throw new Error("gameDataFallback must contain an https:// url and optional string hint");
}

const operationId = `${Date.now()}-${randomUUID()}`;
const scratch = resolve(tmpdir(), "eagler-release-build", operationId);
const incompleteRoot = resolve(dirname(destination), ".release-incomplete");
const incompleteOutput = resolve(incompleteRoot, `${basename(destination)}-${operationId}`);
await mkdir(scratch, { recursive: true });
await mkdir(incompleteRoot, { recursive: true });
try {
const report = [];
const sourcesBefore = await identifySources();
const inputsBefore = await identifyInputs(inputPath, prepare);
prepare.OutputDirectory = resolve(scratch, "hosted-site");
prepare.PythonEnvironmentDirectory = resolve(scratch, "python-environment");
const parameterFile = resolve(scratch, "prepare.json");
const wrapper = resolve(scratch, "prepare.ps1");
await writeFile(parameterFile, JSON.stringify(prepare, null, 2));
await writeFile(wrapper, [
  "$ErrorActionPreference = 'Stop'",
  "$parameters = Get-Content -LiteralPath $args[0] -Raw | ConvertFrom-Json -AsHashtable",
  "& $args[1] @parameters",
  "",
].join("\n"));
await runStep(report, "hosted-build", "pwsh", [
  "-NoProfile", "-File", wrapper, parameterFile,
  resolve(project, "tools/maintainer/assemble-site.ps1"),
]);

const hosted = prepare.OutputDirectory;
await runStep(report, "hosted-verify", process.execPath, ["scripts/verify-server-build.mjs", hosted]);
const externalSite = resolve(scratch, "external-site");
await runStep(report, "external-build", process.execPath, [
  "scripts/package-external-site.mjs",
  `--source=${hosted}`,
  `--output=${externalSite}`,
  `--runtime-release=${prepare.RuntimeRelease}`,
  `--games=${releaseGames.join(",")}`,
  "--profile=web-release-external",
]);
const importConfig = resolve(scratch, "import-features.json");
await writeFile(importConfig, JSON.stringify({
  ...features,
  resourceMode: "import",
  ...(input.gameDataFallback ? { gameDataFallback: input.gameDataFallback } : {}),
}, null, 2));
const importSite = resolve(scratch, "import-site");
const importArgs = [
  "scripts/package-server.mjs",
  `--output=${importSite}`,
  `--feature-config=${importConfig}`,
  `--host-manifest=${resolve(hosted, HOST_MANIFEST_FILE)}`,
  `--runtime-release=${prepare.RuntimeRelease}`,
  `--artwork-dir=${resolve(hosted, "assets")}`,
  `--games=${releaseGames.join(",")}`,
  "--profile=web-release-import",
];
await runStep(report, "import-build", process.execPath, importArgs);
await runStep(report, "import-verify", process.execPath, ["scripts/verify-server-build.mjs", importSite]);

const sourcesAfter = await identifySources();
for (const repository of Object.keys(sourcesBefore)) {
  if (sourcesBefore[repository].sha256 !== sourcesAfter[repository].sha256) {
    throw new Error(`source changed during release: ${repository}; incomplete work retained under ${incompleteRoot}`);
  }
}
const inputsAfter = await identifyInputs(inputPath, prepare);
if (inputsBefore.sha256 !== inputsAfter.sha256) {
  throw new Error(`release inputs changed during release; incomplete work retained under ${incompleteRoot}`);
}

await mkdir(dirname(incompleteOutput), { recursive: true });
await mkdir(incompleteOutput);
await cp(hosted, resolve(incompleteOutput, "hosted-site"), { recursive: true });
await cp(externalSite, resolve(incompleteOutput, "external-site"), { recursive: true });
await cp(importSite, resolve(incompleteOutput, "import-site"), { recursive: true });
await cp(prepare.RuntimeRelease, resolve(incompleteOutput, "runtime-release"), { recursive: true });
await mkdir(resolve(incompleteOutput, "game-package"));
await mkdir(resolve(incompleteOutput, "offline-zip"));
for (const game of releaseGames) {
  const descriptorPath = resolve(hosted, `${game}.package.json`);
  const descriptor = JSON.parse(await readFile(descriptorPath, "utf8"));
  const packageRoot = resolve(incompleteOutput, "game-package", game);
  await mkdir(packageRoot);
  await cp(descriptorPath, resolve(packageRoot, "package.json"));
  for (const file of Object.values(descriptor.files)) {
    if (!file.source || file.source.includes("\\") || file.source.split("/").some(part => !part || part === "." || part === "..")) {
      throw new Error(`${game}: invalid Package source path`);
    }
    const target = resolve(packageRoot, file.source);
    await mkdir(dirname(target), { recursive: true });
    await cp(resolve(hosted, file.source), target);
  }
  await runStep(report, `offline-${game}`, process.execPath, [
    "scripts/package-offline-game.mjs", hosted, game,
    resolve(incompleteOutput, "offline-zip", `${game}.zip`),
  ]);
}

const completionReport = validateCompletionReport({
  schema: COMPLETION_REPORT_SCHEMA,
  steps: report,
  completion: candidateCompletion,
});
await writeFile(resolve(incompleteOutput, "verification-report.json"), `${JSON.stringify(completionReport, null, 2)}\n`);
await writeFile(resolve(incompleteOutput, "input-manifest.json"), `${JSON.stringify(inputsAfter, null, 2)}\n`);
const files = await inventory(incompleteOutput);
files.sort((left, right) => left.path.localeCompare(right.path, "en"));
await writeFile(resolve(incompleteOutput, "deployment.json"), `${JSON.stringify({
  format: "eagler-touhou-release-bundle/2",
  games: releaseGames,
  files,
}, null, 2)}\n`);
await writeReleaseManifest(incompleteOutput, {
  profile: "web-release",
  sources: sourcesAfter,
  parameters: {
    inputSha256: sha256(await readFile(inputPath)),
    runtimeBuildProvenance: "verified-runtime-release",
    music: prepare.Music,
    games: releaseGames,
  },
});
const manifest = await verifyReleaseBundle(incompleteOutput);
if (existsSync(destination)) {
  throw new Error("release destination appeared during build; verified bundle retained as incomplete output");
}
await rename(incompleteOutput, destination);
console.log(JSON.stringify({
  output: destination,
  releaseId: manifest.releaseId,
  completion: candidateCompletion,
}));
} finally {
  await rm(scratch, { recursive: true, force: true });
}
