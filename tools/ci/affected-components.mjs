import {readFile, appendFile} from 'node:fs/promises';
import {execFileSync} from 'node:child_process';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';

// These are production inputs to the Runtime source-build planner, recipes,
// and evidence. Keep test orchestration and helpers used only by source checks
// out of this set so they run policy tests without recompiling products.
const runtimeSourceBuildInputs = new Set([
  'launcher/scripts/build-runtime.mjs',
  'launcher/lib/runtime-source-plan.mjs',
  'launcher/lib/runtime-build-evidence.mjs',
  'launcher/lib/runtime-build-profiles.mjs',
  'launcher/lib/source-layout.mjs',
  'launcher/lib/workspace-layout.mjs',
  'launcher/lib/release-manifest.mjs',
  'launcher/lib/contracts/product-catalog.mjs',
  'launcher/config/runtime-builds.json',
  'launcher/config/workspace.json',
  'tools/testing/discovery.mjs',
  'tools/build/compiler-arguments.mjs',
  'tools/build/runtime-inputs.mjs',
  'tools/build/source-object.mjs',
]);

export function nodeBuildTargets(affected, workspace, builds) {
  return Object.entries(builds.games).flatMap(([game,entry])=>['normal','multiplayer'].flatMap(variant=>{
    const definition=entry.variants[variant];
    if (!definition || {...entry.sourceBuild,...definition.sourceBuild}.kind!=='node') return [];
    const directory=workspace.repositories[definition.workspaceRepository ?? entry.workspaceRepository];
    return affected.includes(directory) ? [{game,variant,directory}] : [];
  }));
}

export function affectedComponents(files, workspace, builds) {
  const runtimes = [...new Set(Object.entries(workspace.repositories)
    .filter(([key, value]) => value.startsWith('ports/')).map(([, value]) => value))];
  const services=Object.values(workspace.repositories).filter(value=>value.startsWith('services/'));
  const all = ['launcher', workspace.repositories.common, ...runtimes, ...services];
  if (files === null) return all.sort();
  const affected = new Set(['launcher']);
  if (files.some(file => runtimeSourceBuildInputs.has(file))) {
    // Return every declared Runtime source owner. nodeBuildTargets expands the
    // node recipes to its matrix, while the TH06/TH07 CMake Web matrices are
    // selected directly from affected owners. This preserves all 15 existing
    // product build variants without scheduling unrelated service suites.
    for (const entry of Object.values(builds.games)) for (const [variant, definition] of Object.entries(entry.variants)) {
      if (!definition) continue;
      affected.add(workspace.repositories[definition.workspaceRepository ?? entry.workspaceRepository]);
    }
  }
  // Common is a source dependency of both product builds and services. The
  // build planner/configuration and production build helpers above select all
  // Runtime owners, while test/CI tooling selects its validation lanes only.
  if (files.some(file => file.startsWith('common/'))) return all.sort();
  for (const directory of runtimes) if (files.some(file => file === directory || file.startsWith(directory + '/'))) affected.add(directory);
  for (const directory of services) if (files.some(file => file === directory || file.startsWith(directory + '/'))) affected.add(directory);
  // Ordinary and MP fixes require semantic review together, even when the
  // source branches were imported into two directories.
  for (const entry of Object.values(builds.games)) {
    const owners = [...new Set(Object.values(entry.variants).map(variant =>
      workspace.repositories[variant.workspaceRepository ?? entry.workspaceRepository]))];
    if (owners.some(owner => affected.has(owner))) owners.forEach(owner => affected.add(owner));
  }
  return [...affected].sort();
}

const hasPrefix = (files, prefixes) => files === null || files.some(file => prefixes.some(prefix => file.startsWith(prefix)));

/** Select source-validation lanes from their actual inputs instead of service or docs-only changes. */
export function ciJobCoverage(files) {
  const runtimeSourceBuildInputChanged = (files ?? []).some(file => runtimeSourceBuildInputs.has(file));
  const sourceContracts = runtimeSourceBuildInputChanged || hasPrefix(files, ['common/', 'tools/', 'tests/', '.github/', 'launcher/config/', 'ports/']);
  const portAdaptation = hasPrefix(files, ['common/', 'tools/', 'tests/', '.github/', 'launcher/config/', 'ports/',
    'launcher/app/', 'launcher/src/', 'launcher/lib/', 'launcher/scripts/build-runtime.mjs', 'launcher/tests/', 'docs/adaptation/checklist.json']) ||
    (files ?? []).some(file => file === 'launcher/package.json' || file === 'launcher/package-lock.json');
  return {sourceContracts, portAdaptation};
}

if (process.argv[1] && import.meta.url === pathToFileURL(resolve(process.argv[1])).href) {
  const root = resolve(import.meta.dirname, '../..');
  const workspace = JSON.parse(await readFile(resolve(root, 'launcher/config/workspace.json'), 'utf8'));
  const builds = JSON.parse(await readFile(resolve(root, 'launcher/config/runtime-builds.json'), 'utf8'));
  const base = process.env.EAGLER_CI_BASE || '';
  const files = /^[a-f0-9]{40,64}$/i.test(base) && !/^0+$/.test(base)
    // A whole-repository import exceeds Node's default 1 MiB stdout buffer.
    ? execFileSync('git', ['diff', '--name-only', '--no-renames', '-z', base, 'HEAD', '--'], {cwd: root, encoding: 'utf8', maxBuffer: 64 * 1024 * 1024}).split('\0').filter(Boolean)
    : null;
  const affected = affectedComponents(files, workspace, builds);
  const coverage = ciJobCoverage(files);
  const result = JSON.stringify(affected);
  if (process.env.GITHUB_OUTPUT) {
    await appendFile(process.env.GITHUB_OUTPUT, `affected=${result}\n`);
    await appendFile(process.env.GITHUB_OUTPUT, `node-builds=${JSON.stringify(nodeBuildTargets(affected,workspace,builds))}\n`);
    await appendFile(process.env.GITHUB_OUTPUT, `source-contracts=${coverage.sourceContracts}\nport-adaptation=${coverage.portAdaptation}\n`);
  }
  console.log(result);
}
