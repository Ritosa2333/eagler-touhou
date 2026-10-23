import {spawn} from 'node:child_process';
import {createHash} from 'node:crypto';
import {copyFileSync, existsSync, lstatSync, mkdirSync, readFileSync, readdirSync, writeFileSync} from 'node:fs';
import {dirname, resolve} from 'node:path';
import {workspaceRoot} from '../lib/workspace-layout.mjs';
import {
  atomicWriteRuntimePackageReceipt, captureRuntimePackageFiles, captureRuntimePackageInputs,
  createRuntimePackageReceipt, readRuntimeBuildReceipt, resolveRuntimePackagePlan,
  runtimePackageReceiptPath, runtimePackageSourceInputsIdentity, verifyRuntimePackageBuildOutputs,
} from '../lib/runtime-package-evidence.mjs';
import {runtimeBuildPlanIdentity, verifyRuntimeBuildOutputs} from '../lib/runtime-build-evidence.mjs';

const args = {};
for (const item of process.argv.slice(2)) {
  const match = item.match(/^--(game|variant)=(.+)$/);
  if (!match || Object.hasOwn(args, match[1])) throw new Error(`invalid argument: ${item}`);
  args[match[1]] = match[2];
}
if (!args.game || !args.variant) throw new Error('usage: node launcher/scripts/package-runtime.mjs --game=thXX --variant=normal|multiplayer');

const root = workspaceRoot();
const plan = resolveRuntimePackagePlan(args.game, args.variant, root);
const captureInputs = () => captureRuntimePackageInputs(plan, root);
const beforeInputs = captureInputs();
const buildEvidence = readRuntimeBuildReceipt(plan.game, plan.variant, root);
const sourceOutputs = verifyRuntimeBuildOutputs(buildEvidence.receipt, plan.game, plan.variant, root);
const inputIdentity = runtimePackageSourceInputsIdentity(buildEvidence.receipt.sourceFiles,
  buildEvidence.receipt.requiredSubmodules ?? []);
if (JSON.stringify(buildEvidence.receipt.sourceFiles) !== JSON.stringify(beforeInputs.sourceFiles) ||
    JSON.stringify(buildEvidence.receipt.requiredSubmodules ?? []) !== JSON.stringify(beforeInputs.requiredSubmodules ?? []) ||
    inputIdentity.sourceInputsSha256 !== beforeInputs.sourceInputsSha256) {
  throw new Error(`${plan.game}/${plan.variant} source build receipt is stale; rebuild the selected Runtime before packaging`);
}
const planReceiptPath = resolve(root, 'artifacts/validation/source-builds', `${plan.game}-${plan.variant}.json`);
const recordedPlan = JSON.parse(readFileSync(planReceiptPath, 'utf8'));
if (recordedPlan.game !== plan.game || recordedPlan.variant !== plan.variant ||
    buildEvidence.receipt.planSha256 !== runtimeBuildPlanIdentity(recordedPlan) ||
    recordedPlan.sources?.sourceFilesSha256 !== beforeInputs.sourceFilesSha256 ||
    JSON.stringify(recordedPlan.sources?.requiredSubmodules ?? []) !== JSON.stringify(beforeInputs.requiredSubmodules ?? [])) {
  throw new Error(`${plan.game}/${plan.variant} source build plan does not match its current source receipt`);
}

const compiledWasm = readFileSync(plan.sourceExecution);
const compiledWasmSha256 = createHash('sha256').update(compiledWasm).digest('hex');
if (compiledWasmSha256 !== buildEvidence.receipt.executionSha256 ||
    compiledWasmSha256 !== sourceOutputs.execution.sha256) {
  throw new Error(`${plan.game}/${plan.variant} compiler output does not match the current source build receipt`);
}

function ensureOutputIsAvailable(output, allowed = []) {
  if (!existsSync(output)) return;
  const stat = lstatSync(output);
  if (!stat.isDirectory()) throw new Error(`Runtime package output is not a directory: ${output}`);
  const actual = [];
  function walk(directory) {
    for (const entry of readdirSync(directory, {withFileTypes: true})) {
      const path = resolve(directory, entry.name);
      if (entry.isSymbolicLink()) throw new Error(`Runtime package output contains a link: ${path}`);
      if (entry.isDirectory()) walk(path);
      else if (entry.isFile()) actual.push(path.slice(output.length + 1).replaceAll('\\', '/'));
      else throw new Error(`Runtime package output contains a non-file entry: ${path}`);
    }
  }
  walk(output);
  const known = new Set(allowed);
  const unexpected = actual.filter(path => !known.has(path));
  if (unexpected.length) throw new Error(`Runtime package output contains unowned files: ${unexpected.join(', ')}`);
}

function packageCmakeOutput() {
  const output = plan.output;
  ensureOutputIsAvailable(output, [...plan.files, 'manifest.json', 'runtime-files.json']);
  mkdirSync(output, {recursive: true});
  const buildDirectory = dirname(plan.sourceExecution);
  for (const name of plan.files) {
    const source = resolve(buildDirectory, name);
    const stat = lstatSync(source, {throwIfNoEntry: false});
    if (!stat?.isFile()) throw new Error(`CMake Runtime output is missing: ${source}`);
    copyFileSync(source, resolve(output, name));
  }
  const identities = {};
  for (const name of plan.files) {
    const bytes = readFileSync(resolve(output, name));
    identities[name] = {bytes: bytes.length, sha256: createHash('sha256').update(bytes).digest('hex')};
  }
  if (identities[plan.wasmFile]?.sha256 !== buildEvidence.receipt.executionSha256)
    throw new Error('CMake Runtime WASM does not match the source build receipt');
  const manifest = {
    game: plan.game,
    variant: plan.variant,
    protocol: 'eagler-touhou/1',
    adapter: 'cmake-generated',
    profile: plan.variant === 'multiplayer' ? 'multiplayer' : 'production',
    features: recordedPlan.features,
    execution: {kind: 'cpp-cmake', sha256: buildEvidence.receipt.executionSha256},
  };
  writeFileSync(resolve(output, 'manifest.json'), JSON.stringify(manifest, null, 2) + '\n');
  const manifestBytes = readFileSync(resolve(output, 'manifest.json'));
  identities['manifest.json'] = {bytes: manifestBytes.length, sha256: createHash('sha256').update(manifestBytes).digest('hex')};
  writeFileSync(resolve(output, 'runtime-files.json'), JSON.stringify({
    schema: 'eagler-touhou/runtime-directory/1', game: plan.game, files: identities,
  }, null, 2) + '\n');
}

async function runOfficialPackager() {
  await new Promise((accept, reject) => {
    const child = spawn(process.execPath, [plan.script, ...plan.args], {
      cwd: plan.source,
      windowsHide: true,
      stdio: 'inherit',
      env: {...process.env, ...(plan.sourceBuild.environment??{}), EAGLER_RUNTIME_OUTPUT: plan.output},
    });
    child.once('error', reject);
    child.once('exit', code => code === 0 ? accept() : reject(new Error(`${plan.game}/${plan.variant} official packager failed (${code})`)));
  });
}

if (plan.kind === 'cmake') packageCmakeOutput();
else await runOfficialPackager();

const packageFiles = captureRuntimePackageFiles(plan.output, {
  game: plan.game, variant: plan.variant, entry: plan.entry,
});
if (!packageFiles.files[plan.entry] || !packageFiles.files[plan.wasmFile])
  throw new Error(`${plan.game}/${plan.variant} package is missing its configured entry or WASM`);
verifyRuntimePackageBuildOutputs(packageFiles,plan,sourceOutputs);
verifyRuntimeBuildOutputs(buildEvidence.receipt, plan.game, plan.variant, root);
const manifest = JSON.parse(readFileSync(resolve(plan.output, 'manifest.json'), 'utf8'));
if (manifest.game !== plan.game || manifest.variant != null && manifest.variant !== plan.variant)
  throw new Error(`${plan.game}/${plan.variant} package manifest identity mismatch`);
const afterInputs = captureInputs();
if (JSON.stringify({requiredSubmodules: beforeInputs.requiredSubmodules, sourceFilesSha256: beforeInputs.sourceFilesSha256,
  sourceInputsSha256: beforeInputs.sourceInputsSha256, packagerInputs: beforeInputs.packagerInputs}) !==
  JSON.stringify({requiredSubmodules: afterInputs.requiredSubmodules, sourceFilesSha256: afterInputs.sourceFilesSha256,
    sourceInputsSha256: afterInputs.sourceInputsSha256, packagerInputs: afterInputs.packagerInputs})) {
  throw new Error(`${plan.game}/${plan.variant} source or packager inputs changed while packaging; discard the package and retry`);
}
const receipt = createRuntimePackageReceipt({
  game: plan.game,
  variant: plan.variant,
  entry: plan.entry,
  packageSha256: packageFiles.packageSha256,
  files: packageFiles.files,
  sourceBuildReceiptSha256: buildEvidence.sha256,
  sourceInputsSha256: beforeInputs.sourceInputsSha256,
  packagerInputs: beforeInputs.packagerInputs,
});
const receiptPath = runtimePackageReceiptPath(plan.game, plan.variant, root);
atomicWriteRuntimePackageReceipt(receiptPath, receipt);
console.log(JSON.stringify({
  game: plan.game, variant: plan.variant, entry: plan.entry, output: plan.output,
  packageSha256: receipt.packageSha256, files: Object.keys(receipt.files).length,
  sourceBuildReceiptSha256: receipt.sourceBuildReceiptSha256, sourceInputsSha256: receipt.sourceInputsSha256,
  receipt: receiptPath,
}, null, 2));
