import {execFileSync, spawn} from 'node:child_process';
import {createHash} from 'node:crypto';
import {lstatSync, readFileSync, existsSync, statSync} from 'node:fs';
import {relative, resolve, isAbsolute} from 'node:path';
import {isNodeTest} from '../../tools/testing/discovery.mjs';

const hash = bytes => createHash('sha256').update(bytes).digest('hex');
export const RUNTIME_BUILD_CONTROL_FILES = Object.freeze([
  'launcher/scripts/build-runtime.mjs','launcher/lib/runtime-source-plan.mjs',
  'launcher/lib/runtime-build-evidence.mjs','launcher/lib/runtime-build-profiles.mjs',
  'launcher/lib/source-layout.mjs','launcher/lib/workspace-layout.mjs','launcher/lib/release-manifest.mjs',
  'launcher/lib/contracts/product-catalog.mjs',
  'launcher/config/runtime-builds.json','launcher/config/workspace.json',
  'tools/testing/discovery.mjs',
]);
// These are imported by current production build entrypoints. Test modules,
// documentation and unrelated build utilities are deliberately excluded.
export const RUNTIME_BUILD_HELPER_FILES = Object.freeze([
  'tools/build/compiler-arguments.mjs','tools/build/runtime-inputs.mjs','tools/build/source-object.mjs',
]);
const within = (root, path) => {
  const name = relative(root, path).replaceAll('\\','/');
  if (!name || isAbsolute(name) || name === '..' || name.startsWith('../')) throw new Error(`Build input must be inside the current repository: ${path}`);
  return name;
};

function isBuildSourceEntry(name) {
  const normalized=name.replaceAll('\\','/');
  const parts=normalized.split('/');
  const basename=parts.at(-1);
  // Build documentation is not consumed by the compiler. Excluding it keeps
  // status/report edits from invalidating an otherwise reproducible build.
  if(parts.includes('docs')||/^(?:README|ADAPTATION)(?:\.|$)/i.test(basename)||
     /\.(?:md|markdown|rst|adoc)$/i.test(basename))return false;
  // The shared tooling predicate is the repository's canonical Node test
  // entry convention; also exclude the equivalent JS/TS spec convention.
  if(isNodeTest(normalized)||/\.(?:test|spec)\.(?:[cm]?js|mts|tsx?)$/i.test(basename))return false;
  return true;
}

/** Observe inputs before the real compiler starts, including initialized dependencies. */
export function captureRuntimeBuildSources(plan, repository) {
  const directories = [...new Set([plan.source, plan.common, plan.environment.EAGLER_COMMON_NETPLAY_ROOT])];
  const files = new Set();
  function collect(directory) {
    const names = execFileSync('git', ['ls-files','--cached','--others','--exclude-standard','-z','--','.'],
      {cwd:directory, encoding:'utf8', windowsHide:true, maxBuffer:32*1024*1024}).split('\0').filter(Boolean);
    for (const name of names) {
      if(!isBuildSourceEntry(name))continue;
      const path = resolve(directory, name), stat = lstatSync(path, {throwIfNoEntry:false});
      if (!stat) continue;
      if (stat.isDirectory() && existsSync(resolve(path, '.git'))) {collect(path); continue;}
      if (stat.isFile()) files.add(path);
      else throw new Error(`Build input is not a regular source file: ${path}`);
    }
  }
  for (const directory of directories) {within(repository, directory); collect(directory);}
  for (const name of [...RUNTIME_BUILD_CONTROL_FILES,...RUNTIME_BUILD_HELPER_FILES]) files.add(resolve(repository, name));
  const sourceFiles = [...files].map(path=>({path:within(repository, path), sha256:hash(readFileSync(path))}))
    .sort((a,b)=>a.path.localeCompare(b.path,'en'));
  if (!sourceFiles.some(file=>file.path.startsWith(within(repository, plan.source) + '/') && /\.(?:cpp|c)$/.test(file.path)))
    throw new Error('Build source receipt requires the selected owner native sources');
  return sourceFiles;
}

function configuredRuntimeSource(game,variant,repository) {
  if(!['normal','multiplayer'].includes(variant))throw new Error(`Unsupported Runtime build variant: ${variant}`);
  const builds=JSON.parse(readFileSync(resolve(repository,'launcher/config/runtime-builds.json'),'utf8'));
  const workspace=JSON.parse(readFileSync(resolve(repository,'launcher/config/workspace.json'),'utf8'));
  const entry=builds.games?.[game],definition=entry?.variants?.[variant];
  if(!entry||!definition)throw new Error(`Unknown Runtime build: ${game}/${variant}`);
  const owner=definition.workspaceRepository??entry.workspaceRepository;
  const sourceDirectory=workspace.repositories?.[owner],commonDirectory=workspace.repositories?.common;
  if(typeof sourceDirectory!=='string'||typeof commonDirectory!=='string')
    throw new Error(`Runtime build workspace does not resolve ${game}/${variant} and common`);
  const source=resolve(repository,sourceDirectory),common=resolve(repository,commonDirectory);
  within(repository,source);within(repository,common);
  return {source,common};
}

/** Resolve the source owner from the real build and workspace configuration. */
export function runtimeBuildSourceRoot(game,variant,repository) {
  const root=resolve(repository),{source}=configuredRuntimeSource(game,variant,root);
  return within(root,source);
}

/** Rebuild the complete expected source snapshot without requiring an SDK. */
export function expectedRuntimeBuildSources(game,variant,repository) {
  const root=resolve(repository),{source,common}=configuredRuntimeSource(game,variant,root);
  return captureRuntimeBuildSources({game,variant,source,common,
    environment:{EAGLER_COMMON_NETPLAY_ROOT:common}},root);
}

/** Bind the successful compiler output to the exact inputs observed before execution. */
export function runtimeBuildReceipt(plan, repository, observed) {
  const current = captureRuntimeBuildSources(plan, repository);
  if (JSON.stringify(current) !== JSON.stringify(observed)) throw new Error('Source changed during compilation; rebuild before recording Runtime evidence');
  const execution = readFileSync(plan.execution);
  if (execution.length < 8 || !execution.subarray(0,8).equals(Buffer.from([0,97,115,109,1,0,0,0])))
    throw new Error(`Compiler output is not WebAssembly: ${plan.execution}`);
  return {schema:'eagler-touhou/runtime-source-build/1', game:plan.game, variant:plan.variant,
    profile:plan.variant === 'multiplayer' ? 'multiplayer' : plan.variant === 'normal' ? 'production' : 'development',
    executionSha256:hash(execution), sourceFiles:observed};
}

export async function executeRuntimeSourceBuild(plan, repository, {previousReceipt,previousPlan}={}) {
  if (!Array.isArray(plan.steps) || !plan.steps.length) throw new Error('Source build requires executable compiler steps');
  const observed = captureRuntimeBuildSources(plan, repository);
  const identity = () => {
    const stat=statSync(plan.execution,{throwIfNoEntry:false,bigint:true});
    return stat ? [stat.ino,stat.size,stat.mtimeNs,stat.ctimeNs].join(':') : null;
  };
  const before=identity();
  for (const step of plan.steps) await new Promise((accept, reject) => {
    const child = spawn(step.command, step.args, {cwd:step.cwd, shell:false, windowsHide:true,
      stdio:'inherit', env:{...process.env, ...plan.environment}});
    child.once('error', reject);
    child.once('exit', code=>code === 0 ? accept() : reject(new Error(`${plan.game}/${plan.variant} build failed (${code})`)));
  });
  const receipt=runtimeBuildReceipt(plan, repository, observed);
  const planKey=value=>{
    if (!value) return null;
    const {schema,sources,...inputs}=value;
    return JSON.stringify(inputs);
  };
  if (before !== null && identity() === before && (JSON.stringify(previousReceipt) !== JSON.stringify(receipt) || planKey(previousPlan) !== planKey(plan)))
    throw new Error('Compiler did not refresh the execution output and no matching prior build receipt and plan exist');
  return receipt;
}
