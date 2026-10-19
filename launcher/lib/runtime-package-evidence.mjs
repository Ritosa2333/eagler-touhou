import {createHash, randomUUID} from 'node:crypto';
import {lstatSync, mkdirSync, readFileSync, readdirSync, renameSync, writeFileSync} from 'node:fs';
import {basename, dirname, isAbsolute, relative, resolve} from 'node:path';
import {PRODUCT_GAMES} from './contracts/product-catalog.mjs';
import {runtimeBuildInputIdentity, runtimeBuildSourceRoot, expectedRuntimeBuildSources, expectedRuntimeBuildSubmodules,
  runtimeBuildPlanIdentity, runtimeBuildSourceFilesSha256} from './runtime-build-evidence.mjs';
import {workspaceRoot} from './workspace-layout.mjs';

export const RUNTIME_PACKAGE_BUILD_SCHEMA = 'eagler-touhou/runtime-package-build/1';
export const RUNTIME_PACKAGE_CONFIG_SCHEMA = 'eagler-touhou/runtime-packages/1';

const HASH = /^[a-f0-9]{64}$/i;
const PACKAGE_CONFIG = 'launcher/config/runtime-packages.json';
export const RUNTIME_PACKAGE_CONTROL_FILES = Object.freeze([
  'launcher/scripts/package-runtime.mjs',
  'launcher/lib/runtime-package-evidence.mjs',
  'launcher/config/runtime-packages.json',
  'launcher/config/runtime-builds.json',
  'launcher/config/workspace.json',
  'launcher/lib/runtime-build-evidence.mjs',
  'launcher/lib/runtime-build-dependencies.mjs',
  'launcher/lib/runtime-build-profiles.mjs',
  'launcher/lib/runtime-source-plan.mjs',
  'launcher/lib/source-layout.mjs',
  'launcher/lib/workspace-layout.mjs',
  'launcher/lib/contracts/product-catalog.mjs',
  'tools/testing/discovery.mjs',
]);

const hash = bytes => createHash('sha256').update(bytes).digest('hex');
const stablePathSort = (a, b) => a < b ? -1 : a > b ? 1 : 0;

function safeRelativePath(value, label) {
  if (typeof value !== 'string' || !value || value.startsWith('/') || value.includes('\\') || value.includes(':') || value.includes('\0'))
    throw new Error(`${label} must be a canonical relative path`);
  const parts = value.split('/');
  if (parts.some(part => !part || part === '.' || part === '..')) throw new Error(`${label} must be a canonical relative path`);
  return value;
}

function inside(base, path, label) {
  const rel = relative(base, path).replaceAll('\\', '/');
  if (!rel || rel === '..' || rel.startsWith('../') || isAbsolute(rel)) throw new Error(`${label} must remain inside ${base}`);
  return rel;
}

function fileIdentity(path, label) {
  const stat = lstatSync(path, {throwIfNoEntry: false});
  if (!stat?.isFile()) throw new Error(`${label} must be a regular file: ${path}`);
  const bytes = readFileSync(path);
  return {bytes: bytes.length, sha256: hash(bytes)};
}

function validatePackageDefinition(definition, game, variant) {
  const label = `Runtime package ${game}/${variant}`;
  if (!definition || typeof definition !== 'object' || Array.isArray(definition) || !['cmake', 'script'].includes(definition.kind))
    throw new Error(`${label} has an invalid kind`);
  safeRelativePath(definition.output, `${label} output`);
  if (definition.entry !== `${game}.html` || !/^[a-z0-9._-]+\.wasm$/i.test(definition.wasmFile || ''))
    throw new Error(`${label} has an invalid entry or WASM filename`);
  if (definition.kind === 'cmake') {
    const expected = [`${game}.html`, `${game}.js`, definition.wasmFile];
    if (!Array.isArray(definition.files) || JSON.stringify(definition.files) !== JSON.stringify(expected))
      throw new Error(`${label} CMake file set must contain HTML, generated JS, and WASM in that order`);
    if (definition.script != null || definition.args != null) throw new Error(`${label} CMake package must not declare a packager command`);
  } else {
    safeRelativePath(definition.script, `${label} script`);
    if (!Array.isArray(definition.args) || definition.args.some(arg => typeof arg !== 'string' || !arg || arg.includes('\0')))
      throw new Error(`${label} args must be a string array`);
    const controls = definition.controlFiles;
    if (!Array.isArray(controls) || !controls.includes(definition.script) || controls.some(path => !safeRelativePath(path, `${label} control`)))
      throw new Error(`${label} controlFiles must include its packager script`);
    if (definition.sharedControlFiles != null && (!Array.isArray(definition.sharedControlFiles) ||
        definition.sharedControlFiles.some(path => !safeRelativePath(path, `${label} shared control`))))
      throw new Error(`${label} sharedControlFiles must be canonical repository paths`);
    if (definition.files != null) throw new Error(`${label} script package must use its generated runtime-files inventory`);
  }
}

export function validateRuntimePackageConfig(config, buildConfig) {
  if (!config || config.schema !== RUNTIME_PACKAGE_CONFIG_SCHEMA || !config.games || typeof config.games !== 'object')
    throw new Error('invalid Runtime package config schema');
  const registered = Object.keys(PRODUCT_GAMES);
  if (JSON.stringify(Object.keys(config.games)) !== JSON.stringify(registered))
    throw new Error('Runtime package config games must match the Product Catalog order');
  for (const game of registered) {
    const packageVariants = config.games[game]?.variants;
    const buildVariants = buildConfig?.games?.[game]?.variants;
    const expectedVariants = ['normal', ...(PRODUCT_GAMES[game].multiplayerRuntime ? ['multiplayer'] : [])];
    if (!packageVariants || !buildVariants || JSON.stringify(Object.keys(packageVariants)) !== JSON.stringify(expectedVariants) ||
        expectedVariants.some(variant => !Object.hasOwn(buildVariants, variant)))
      throw new Error(`${game}: Runtime package variants must match its catalogued Runtime variants`);
    for (const [variant, definition] of Object.entries(packageVariants)) validatePackageDefinition(definition, game, variant);
  }
  return config;
}

export function loadRuntimePackageConfig(repository = workspaceRoot()) {
  const root = resolve(repository);
  const buildConfig = JSON.parse(readFileSync(resolve(root, 'launcher/config/runtime-builds.json'), 'utf8'));
  const config = JSON.parse(readFileSync(resolve(root, PACKAGE_CONFIG), 'utf8'));
  return validateRuntimePackageConfig(config, buildConfig);
}

export function runtimePackageSourceInputsIdentity(sourceFiles, requiredSubmodules = []) {
  if (!Array.isArray(sourceFiles) || !Array.isArray(requiredSubmodules)) throw new Error('Runtime package source inputs are invalid');
  const sourceFilesSha256 = runtimeBuildSourceFilesSha256(sourceFiles);
  const modules = [...requiredSubmodules].sort((a, b) => stablePathSort(a.path, b.path));
  return {
    sourceFilesSha256,
    sourceInputsSha256: hash(Buffer.from(JSON.stringify({requiredSubmodules: modules, sourceFilesSha256}))),
  };
}

/** Hash the closed file identity map shared by the package producer and acceptance reader. */
export function runtimePackageIdentity({game, variant, entry, files}) {
  if (!/^[a-z0-9]+$/.test(game || '') || !['normal', 'multiplayer'].includes(variant) ||
      typeof entry !== 'string' || !entry || entry.includes('/') || entry.includes('\\'))
    throw new Error('Runtime package identity requires a game, variant, and entry');
  if (!files || typeof files !== 'object' || Array.isArray(files)) throw new Error('Runtime package files must be an identity map');
  const entries = Object.entries(files).map(([path, identity]) => {
    safeRelativePath(path, 'Runtime package file path');
    if (!identity || !Number.isSafeInteger(identity.bytes) || identity.bytes < 0 || !HASH.test(identity.sha256 || ''))
      throw new Error(`invalid Runtime package file identity: ${path}`);
    return [path, {bytes: identity.bytes, sha256: identity.sha256.toLowerCase()}];
  }).filter(([path]) => path !== 'runtime-files.json' && path !== 'runtime-generation.json')
    .sort(([a], [b]) => stablePathSort(a, b));
  if (!entries.some(([path]) => path === entry)) throw new Error(`Runtime package identity is missing its entry: ${entry}`);
  if (!entries.length) throw new Error('Runtime package has no payload files');
  return hash(Buffer.from(JSON.stringify({
    schema: 'eagler-touhou/runtime-package-identity/1', game, variant, entry,
    files: Object.fromEntries(entries),
  })));
}

/** Validate the official inventory, the files it names, and the package directory's closed set. */
export function captureRuntimePackageFiles(directory, identity) {
  const root = resolve(directory);
  const inventoryPath = resolve(root, 'runtime-files.json');
  const inventoryStat = lstatSync(inventoryPath, {throwIfNoEntry: false});
  if (!inventoryStat?.isFile()) throw new Error(`Runtime inventory is missing or not a regular file: ${inventoryPath}`);
  const inventoryBytes = readFileSync(inventoryPath);
  const inventory = JSON.parse(inventoryBytes.toString('utf8'));
  if (inventory?.schema !== 'eagler-touhou/runtime-directory/1' || !inventory.files || typeof inventory.files !== 'object' || Array.isArray(inventory.files))
    throw new Error(`invalid Runtime package inventory: ${inventoryPath}`);
  if (!identity || inventory.game != null && inventory.game !== identity.game ||
      inventory.variant != null && inventory.variant !== identity.variant)
    throw new Error('Runtime package inventory identity mismatch');
  const files = {};
  for (const [name, expected] of Object.entries(inventory.files).sort(([a], [b]) => stablePathSort(a, b))) {
    safeRelativePath(name, 'Runtime inventory path');
    if (name === 'runtime-files.json') throw new Error('Runtime inventory cannot include itself');
    const path = resolve(root, name);
    inside(root, path, 'Runtime inventory path');
    const actual = fileIdentity(path, `Runtime inventory entry ${name}`);
    if (!expected || expected.bytes !== actual.bytes || String(expected.sha256).toLowerCase() !== actual.sha256)
      throw new Error(`Runtime package file identity mismatch: ${name}`);
    files[name] = actual;
  }
  if (!Object.hasOwn(files, 'manifest.json')) throw new Error('Runtime package inventory must include manifest.json');
  const actualPaths = [];
  function walk(directoryPath) {
    for (const item of readdirSync(directoryPath, {withFileTypes: true})) {
      const path = resolve(directoryPath, item.name);
      const name = inside(root, path, 'Runtime package entry');
      if (item.isSymbolicLink()) throw new Error(`Runtime package must not contain links: ${name}`);
      if (item.isDirectory()) walk(path);
      else if (item.isFile()) actualPaths.push(name);
      else throw new Error(`Runtime package contains a non-file entry: ${name}`);
    }
  }
  walk(root);
  const expectedPaths = [...Object.keys(files), 'runtime-files.json'].sort(stablePathSort);
  actualPaths.sort(stablePathSort);
  if (JSON.stringify(actualPaths) !== JSON.stringify(expectedPaths)) {
    const extra = actualPaths.filter(path => !expectedPaths.includes(path));
    const missing = expectedPaths.filter(path => !actualPaths.includes(path));
    throw new Error(`Runtime package is not closed${extra.length ? `; unexpected=${extra.join(',')}` : ''}${missing.length ? `; missing=${missing.join(',')}` : ''}`);
  }
  files['runtime-files.json'] = {bytes: inventoryBytes.length, sha256: hash(inventoryBytes)};
  return {files, packageSha256: runtimePackageIdentity({...identity, files}), inventory};
}

/** Require packaged executable code to be byte-identical to the receipt-bound compiler outputs. */
export function verifyRuntimePackageBuildOutputs(packageFiles, plan, sourceOutputs) {
  if (!packageFiles?.files || !sourceOutputs?.execution || !sourceOutputs?.loader)
    throw new Error(`${plan.game}/${plan.variant} source build output map is incomplete`);
  const expected=[
    [plan.wasmFile,sourceOutputs.execution],
    [basename(sourceOutputs.loader.path),sourceOutputs.loader],
    ...(plan.kind==='cmake'?[ [plan.entry,sourceOutputs.entry] ]:[]),
  ];
  for(const [name,output] of expected){
    if(!output||!packageFiles.files[name]||packageFiles.files[name].sha256!==output.sha256)
      throw new Error(`${plan.game}/${plan.variant}: packaged ${name===plan.wasmFile?'execution':name===basename(sourceOutputs.loader.path)?'loader':'entry'} is not the current compiler output`);
  }
  return true;
}

export function runtimePackageReceiptPath(game, variant, repository = workspaceRoot()) {
  if (!/^[a-z0-9]+$/.test(game) || !['normal', 'multiplayer'].includes(variant)) throw new Error('invalid Runtime package identity');
  return resolve(repository, 'artifacts/validation/runtime-packages', `${game}-${variant}.json`);
}

export function atomicWriteRuntimePackageReceipt(path, receipt) {
  const destination = resolve(path);
  mkdirSync(dirname(destination), {recursive: true});
  const temporary = `${destination}.${randomUUID()}.tmp`;
  writeFileSync(temporary, JSON.stringify(receipt, null, 2) + '\n', {flag: 'wx'});
  renameSync(temporary, destination);
}

export function resolveRuntimePackagePlan(game, variant, repository = workspaceRoot()) {
  const root = resolve(repository);
  const config = loadRuntimePackageConfig(root);
  const builds = JSON.parse(readFileSync(resolve(root, 'launcher/config/runtime-builds.json'), 'utf8'));
  const recipe = config.games[game]?.variants?.[variant];
  const buildEntry = builds.games[game], buildDefinition = buildEntry?.variants?.[variant];
  if (!recipe || !buildDefinition) throw new Error(`unknown Runtime package: ${game}/${variant}`);
  const owner = buildDefinition.workspaceRepository ?? buildEntry.workspaceRepository;
  const workspace = JSON.parse(readFileSync(resolve(root, 'launcher/config/workspace.json'), 'utf8'));
  const source = resolve(root, workspace.repositories?.[owner] || '');
  if (source === root || !source.startsWith(root + '\\') && !source.startsWith(root + '/')) throw new Error(`${game}/${variant} package owner is outside the workspace`);
  const output = resolve(source, recipe.output);
  inside(source, output, 'Runtime package output');
  const sourceBuild = {...buildEntry.sourceBuild, ...buildDefinition.sourceBuild};
  const sourceExecution = sourceBuild.kind === 'cmake'
    ? resolve(source, 'build/web', variant, `${game}.wasm`)
    : resolve(source, sourceBuild.execution || '');
  if (sourceExecution === source || !sourceExecution.startsWith(source + '\\') && !sourceExecution.startsWith(source + '/'))
    throw new Error(`${game}/${variant} source execution is outside the owner repository`);
  const script = recipe.kind === 'script' ? resolve(source, recipe.script) : null;
  if (script) inside(source, script, 'Runtime packager script');
  return {game, variant, owner, source, sourceExecution, kind: recipe.kind, output,
    entry: recipe.entry, wasmFile: recipe.wasmFile, files: recipe.files,
    script, args: recipe.args ?? [], controlFiles: recipe.controlFiles ?? [], sharedControlFiles: recipe.sharedControlFiles ?? [],
    recipe, sourceBuild};
}

export function captureRuntimePackageInputs(plan, repository = workspaceRoot()) {
  const root = resolve(repository);
  const buildPlanPath = resolve(root, 'artifacts/validation/source-builds', `${plan.game}-${plan.variant}.json`);
  const recordedPlan = JSON.parse(readFileSync(buildPlanPath, 'utf8'));
  const buildReceiptPath = resolve(root, 'artifacts/validation/source-builds', `${plan.game}-${plan.variant}.runtime.json`);
  const buildReceipt = JSON.parse(readFileSync(buildReceiptPath, 'utf8'));
  if(buildReceipt?.schema!=='eagler-touhou/runtime-source-build/3' ||
     buildReceipt.planSha256!==runtimeBuildPlanIdentity(recordedPlan))
    throw new Error('Recorded source build plan identity does not match its Runtime receipt');
  const expectedSource = runtimeBuildSourceRoot(plan.game, plan.variant, root);
  if (resolve(root, recordedPlan.source) !== resolve(root, expectedSource)) throw new Error('Source build plan owner does not match the current Runtime recipe');
  if (resolve(root, recordedPlan.execution) !== resolve(root, plan.sourceExecution)) throw new Error('Source build output does not match the formal package recipe');
  const identity = runtimeBuildInputIdentity(recordedPlan, root);
  const expectedSources = expectedRuntimeBuildSources(plan.game, plan.variant, root);
  const expectedSubmodules = expectedRuntimeBuildSubmodules(plan.game, plan.variant, root);
  if (JSON.stringify(recordedPlan.requiredSubmodules ?? []) !== JSON.stringify(expectedSubmodules) ||
      JSON.stringify(identity) !== JSON.stringify(recordedPlan.sources) ||
      JSON.stringify(identity.requiredSubmodules) !== JSON.stringify(expectedSubmodules)) {
    throw new Error('Recorded source build plan no longer matches current Runtime build inputs');
  }
  const sourceIdentity = runtimePackageSourceInputsIdentity(expectedSources, expectedSubmodules);
  return {...identity, ...sourceIdentity, packagerInputs: captureRuntimePackageControlInputs(plan, root), sourceFiles: expectedSources};
}

/** Capture the current production packager and its Launcher control inputs. */
export function captureRuntimePackageControlInputs(plan, repository = workspaceRoot()) {
  const root = resolve(repository);
  const controls = new Set([...RUNTIME_PACKAGE_CONTROL_FILES,
    ...plan.controlFiles.map(path => `${relative(root, plan.source).replaceAll('\\', '/')}/${path}`),
    ...plan.sharedControlFiles]);
  return [...controls].sort(stablePathSort).map(path => {
    safeRelativePath(path, 'Runtime packager input');
    const absolute = resolve(root, path);
    inside(root, absolute, 'Runtime packager input');
    return {path, sha256: fileIdentity(absolute, 'Runtime packager input').sha256};
  });
}

export function readRuntimeBuildReceipt(game, variant, repository = workspaceRoot()) {
  const root = resolve(repository);
  const path = resolve(root, 'artifacts/validation/source-builds', `${game}-${variant}.runtime.json`);
  const bytes = readFileSync(path);
  const receipt = JSON.parse(bytes.toString('utf8'));
  if (receipt?.game !== game || receipt?.variant !== variant || !HASH.test(receipt.executionSha256 || '') ||
      !Array.isArray(receipt.sourceFiles)) throw new Error(`invalid source Runtime receipt: ${path}`);
  return {path, bytes, receipt, sha256: hash(bytes)};
}

export function createRuntimePackageReceipt({game, variant, entry, packageSha256, files, sourceBuildReceiptSha256,
  sourceInputsSha256, packagerInputs}) {
  if (!HASH.test(packageSha256 || '') || !HASH.test(sourceBuildReceiptSha256 || '') || !HASH.test(sourceInputsSha256 || ''))
    throw new Error('Runtime package receipt hashes are invalid');
  if (!Object.hasOwn(files ?? {}, 'runtime-files.json') || !Object.hasOwn(files ?? {}, 'manifest.json'))
    throw new Error('Runtime package receipt must include its inventory and manifest');
  if (!Array.isArray(packagerInputs) || packagerInputs.some(item => !item || typeof item !== 'object' ||
      !HASH.test(item.sha256 || '') || !safeRelativePath(item.path, 'Runtime packager input')) ||
      new Set(packagerInputs.map(item => item.path)).size !== packagerInputs.length ||
      JSON.stringify(packagerInputs) !== JSON.stringify([...packagerInputs].sort((a, b) => stablePathSort(a.path, b.path))))
    throw new Error('Runtime package receipt packager inputs must be unique and sorted');
  const computed = runtimePackageIdentity({game, variant, entry, files});
  if (computed !== packageSha256) throw new Error('Runtime package identity does not match its closed file map');
  return {schema: RUNTIME_PACKAGE_BUILD_SCHEMA, game, variant, entry, packageSha256, files,
    sourceBuildReceiptSha256, sourceInputsSha256, packagerInputs};
}
