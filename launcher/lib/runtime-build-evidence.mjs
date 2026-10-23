import {execFileSync, spawn} from 'node:child_process';
import {createHash} from 'node:crypto';
import {lstatSync, readFileSync} from 'node:fs';
import {basename, dirname, extname, relative, resolve, isAbsolute} from 'node:path';
import {isNodeTest} from '../../tools/testing/discovery.mjs';
import {resolveRuntimeBuildSubmodules} from './runtime-build-dependencies.mjs';

const hash = bytes => createHash('sha256').update(bytes).digest('hex');
const compareRuntimeSourcePaths = (a,b) => a<b?-1:a>b?1:0;
export function canonicalRuntimeBuildSources(sourceFiles) {
  if(!Array.isArray(sourceFiles))throw new Error('Runtime source inventory must be an array');
  return [...sourceFiles].sort((a,b)=>compareRuntimeSourcePaths(a.path,b.path));
}
export function runtimeBuildSourceFilesSha256(sourceFiles) {
  return hash(JSON.stringify(canonicalRuntimeBuildSources(sourceFiles)));
}
function canonicalPlanValue(value) {
  if(Array.isArray(value))return value.map(canonicalPlanValue);
  if(value&&typeof value==='object')return Object.fromEntries(Object.keys(value).sort(compareRuntimeSourcePaths)
    .map(key=>[key,canonicalPlanValue(value[key])]));
  return value;
}
export function runtimeBuildPlanIdentity(plan) {
  if(!plan||typeof plan!=='object'||Array.isArray(plan))throw new Error('Runtime build plan must be an object');
  const {schema,sources,...inputs}=plan;
  return hash(JSON.stringify(canonicalPlanValue(inputs)));
}
export const RUNTIME_BUILD_CONTROL_FILES = Object.freeze([
  'launcher/scripts/build-runtime.mjs','launcher/lib/runtime-source-plan.mjs',
  'launcher/lib/runtime-build-evidence.mjs','launcher/lib/runtime-build-profiles.mjs',
  'launcher/lib/runtime-build-dependencies.mjs',
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

function normalizeSourceExclusions(value) {
  if(value==null)return [];
  if(!Array.isArray(value)||value.some(path=>typeof path!=='string'||!path||/[\\:\0\t\r\n]/.test(path)||
    path.startsWith('/')||path.split('/').some(part=>!part||part==='.'||part==='..')))
    throw new Error('Runtime source exclusions must be safe owner-relative paths');
  return [...new Set(value)].sort(compareRuntimeSourcePaths);
}
function isSourceExcluded(name, exclusions) {
  const normalized=name.replaceAll('\\','/');
  return exclusions.some(path=>normalized===path||normalized.startsWith(path+'/'));
}
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
  const requiredSubmodules=resolveRuntimeBuildSubmodules({source:plan.source,repository,
    declarations:plan.requiredSubmoduleDeclarations??[]});
  if(plan.requiredSubmodules&&JSON.stringify(plan.requiredSubmodules)!==JSON.stringify(requiredSubmodules))
    throw new Error('Required Runtime build submodules changed after the build plan was created');
  const sourceExclusions=runtimeSourceExclusions(plan,repository);
  return captureRuntimeBuildSourcesWithSubmodules(plan,repository,requiredSubmodules,sourceExclusions);
}

function runtimeSourceExclusions(plan,repository) {
  const declared=normalizeSourceExclusions(plan.sourceExclusions);
  if(plan.game&&plan.variant){
    const {source,recipe}=configuredRuntimeSource(plan.game,plan.variant,repository);
    if(resolve(source)!==resolve(plan.source))throw new Error('Runtime source exclusions do not match the configured source owner');
    const expected=normalizeSourceExclusions(recipe.sourceExclusions);
    if(JSON.stringify(declared)!==JSON.stringify(expected))
      throw new Error('Runtime source exclusions changed after the build plan was created');
    return expected;
  }
  return declared;
}

function captureRuntimeBuildSourcesWithSubmodules(plan,repository,requiredSubmodules,sourceExclusions) {
  const directories = [...new Set([plan.source, plan.common, plan.environment.EAGLER_COMMON_NETPLAY_ROOT,
    ...requiredSubmodules.map(module=>resolve(repository,module.path))])];
  const files = new Set();
  function collect(directory,exclusions=[]) {
    const indexed=execFileSync('git', ['ls-files','--stage','-z','--','.'],
      {cwd:directory, encoding:'utf8', windowsHide:true, maxBuffer:32*1024*1024}).split('\0').filter(Boolean);
    const entries=new Map();
    for(const record of indexed) {
      const tab=record.indexOf('\t');
      if(tab<0)throw new Error(`Invalid Git index entry while collecting Runtime inputs: ${directory}`);
      const [mode,,stage]=record.slice(0,tab).split(' ');
      const name=record.slice(tab+1),records=entries.get(name)??[];
      records.push({mode,stage});
      entries.set(name,records);
    }
    const names = execFileSync('git', ['ls-files','--cached','--others','--exclude-standard','-z','--','.'],
      {cwd:directory, encoding:'utf8', windowsHide:true, maxBuffer:32*1024*1024}).split('\0').filter(Boolean);
    for (const name of names) {
      if(isSourceExcluded(name,exclusions))continue;
      const indexedEntries=entries.get(name);
      if(indexedEntries?.length&&indexedEntries.every(entry=>entry.mode==='160000'))continue;
      if(indexedEntries&&(indexedEntries.length!==1||indexedEntries[0].stage!=='0'))
        throw new Error(`Unresolved Git index conflict in Runtime build input: ${resolve(directory,name)}`);
      if(!isBuildSourceEntry(name))continue;
      const path = resolve(directory, name), stat = lstatSync(path, {throwIfNoEntry:false});
      if (!stat) throw new Error(`Tracked or discovered Runtime build input is missing: ${path}`);
      if (stat.isFile()) files.add(path);
      else throw new Error(`Build input is not a regular source file: ${path}`);
    }
  }
  for (const directory of directories) {
    within(repository,directory);
    collect(directory,resolve(directory)===resolve(plan.source)?sourceExclusions:[]);
  }
  for (const name of [...RUNTIME_BUILD_CONTROL_FILES,...RUNTIME_BUILD_HELPER_FILES]) files.add(resolve(repository, name));
  const sourceFiles = canonicalRuntimeBuildSources([...files].map(path=>({path:within(repository, path), sha256:hash(readFileSync(path))})));
  if (!sourceFiles.some(file=>file.path.startsWith(within(repository, plan.source) + '/') && /\.(?:cpp|c)$/.test(file.path)))
    throw new Error('Build source receipt requires the selected owner native sources');
  return sourceFiles;
}

/** Compare only files and pinned modules consumed by the selected Runtime build. */
export function runtimeBuildInputIdentity(plan,repository) {
  repository=resolve(repository);
  const requiredSubmodules=resolveRuntimeBuildSubmodules({source:plan.source,repository,
    declarations:plan.requiredSubmoduleDeclarations??[]});
  if(plan.requiredSubmodules&&JSON.stringify(plan.requiredSubmodules)!==JSON.stringify(requiredSubmodules))
    throw new Error('Required Runtime build submodules changed after the build plan was created');
  const sourceExclusions=runtimeSourceExclusions(plan,repository);
  const sourceFiles=captureRuntimeBuildSourcesWithSubmodules(plan,repository,requiredSubmodules,sourceExclusions);
  return {requiredSubmodules,sourceExclusions,sourceFilesSha256:runtimeBuildSourceFilesSha256(sourceFiles)};
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
  const recipe={...entry.sourceBuild,...definition.sourceBuild};
  return {source,common,recipe,requiredSubmoduleDeclarations:recipe.requiredSubmodules??[]};
}

/** Resolve the source owner from the real build and workspace configuration. */
export function runtimeBuildSourceRoot(game,variant,repository) {
  const root=resolve(repository),{source}=configuredRuntimeSource(game,variant,root);
  return within(root,source);
}

/** Resolve the build outputs consumed by the formal package recipe without an SDK. */
export function expectedRuntimeBuildOutputs(game,variant,repository) {
  const root=resolve(repository),{source,recipe}=configuredRuntimeSource(game,variant,root);
  const execution=recipe.kind==='cmake'
    ? resolve(source,'build/web',variant,`${game}.wasm`)
    : resolve(source,recipe.execution??'');
  if(!recipe.execution&&recipe.kind!=='cmake')throw new Error(`execution output is not declared: ${game}/${variant}`);
  return runtimeBuildOutputPaths(game,recipe.kind,execution,root);
}

function runtimeBuildOutputPaths(game,kind,execution,repository) {
  const root=resolve(repository),executionPath=resolve(execution);
  if(!['cmake','node'].includes(kind))throw new Error(`Unsupported Runtime output recipe: ${kind}`);
  const executionName=basename(executionPath);
  if(extname(executionName)!=='.wasm')throw new Error(`Runtime execution output must be a WASM path: ${executionPath}`);
  const directory=dirname(executionPath),stem=executionName.slice(0,-'.wasm'.length);
  const outputs={execution:{path:within(root,executionPath)},
    loader:{path:within(root,resolve(directory,`${kind==='cmake'?game:stem}.${kind==='cmake'?'js':'mjs'}`))}};
  if(kind==='cmake')outputs.entry={path:within(root,resolve(directory,`${game}.html`))};
  else outputs.buildManifest={path:within(root,resolve(directory,'build.json'))};
  return outputs;
}

/** Read every configured code output; parent output directories may be approved junctions. */
export function captureRuntimeBuildOutputs(plan,repository) {
  const root=resolve(repository),{source,recipe}=configuredRuntimeSource(plan.game,plan.variant,root);
  if(resolve(plan.source)!==resolve(source))throw new Error('Runtime output source does not match its build recipe');
  const expected=expectedRuntimeBuildOutputs(plan.game,plan.variant,root);
  if(resolve(plan.execution)!==resolve(root,expected.execution.path))
    throw new Error('Runtime execution output does not match its build recipe');
  const paths=runtimeBuildOutputPaths(plan.game,recipe.kind,plan.execution,root);
  return Object.fromEntries(Object.entries(paths).map(([role,{path}])=>{
    const absolute=resolve(root,path),stat=lstatSync(absolute,{throwIfNoEntry:false});
    if(!stat?.isFile()||stat.isSymbolicLink())throw new Error(`Runtime build output must be a regular file: ${absolute}`);
    return [role,{path,sha256:hash(readFileSync(absolute))}];
  }));
}

/** Verify a v3 receipt's exact recipe output set and current bytes. */
export function verifyRuntimeBuildOutputs(receipt,game,variant,repository) {
  const root=resolve(repository);
  if(receipt?.schema!=='eagler-touhou/runtime-source-build/3')
    throw new Error(`Runtime package requires a source build version 3 receipt for ${game}/${variant}`);
  const expected=expectedRuntimeBuildOutputs(game,variant,root),actual=receipt.outputs;
  if(!actual||typeof actual!=='object'||Array.isArray(actual)||
     JSON.stringify(Object.keys(actual).sort())!==JSON.stringify(Object.keys(expected).sort()))
    throw new Error(`${game}/${variant} source receipt has an incomplete Runtime output map`);
  const digests={};
  for(const [role,expectedOutput] of Object.entries(expected)){
    const output=actual[role];
    if(!output||typeof output!=='object'||JSON.stringify(Object.keys(output).sort())!==JSON.stringify(['path','sha256'])||
       output.path!==expectedOutput.path||!/^[a-f0-9]{64}$/i.test(output.sha256||''))
      throw new Error(`${game}/${variant} source receipt has an invalid ${role} output mapping`);
    const absolute=resolve(root,expectedOutput.path),stat=lstatSync(absolute,{throwIfNoEntry:false});
    if(!stat?.isFile()||stat.isSymbolicLink())throw new Error(`Runtime build output must be a regular file: ${absolute}`);
    const sha256=hash(readFileSync(absolute));
    if(sha256!==output.sha256)throw new Error(`${game}/${variant} source Runtime output changed: ${expectedOutput.path}`);
    digests[role]={path:expectedOutput.path,sha256};
  }
  if(receipt.executionSha256!==digests.execution.sha256)
    throw new Error(`${game}/${variant} source receipt execution hash differs from its output map`);
  return digests;
}

/** Rebuild the complete expected source snapshot without requiring an SDK. */
export function expectedRuntimeBuildSources(game,variant,repository) {
  const root=resolve(repository),{source,common,recipe,requiredSubmoduleDeclarations}=configuredRuntimeSource(game,variant,root);
  const requiredSubmodules=resolveRuntimeBuildSubmodules({source,repository:root,declarations:requiredSubmoduleDeclarations});
  return captureRuntimeBuildSources({game,variant,source,common,requiredSubmoduleDeclarations,requiredSubmodules,sourceExclusions:recipe.sourceExclusions??[],
    environment:{EAGLER_COMMON_NETPLAY_ROOT:common}},root);
}

/** Resolve the exact required module pins expected by the selected build recipe. */
export function expectedRuntimeBuildSubmodules(game,variant,repository) {
  const root=resolve(repository),{source,requiredSubmoduleDeclarations}=configuredRuntimeSource(game,variant,root);
  return resolveRuntimeBuildSubmodules({source,repository:root,declarations:requiredSubmoduleDeclarations});
}

/** Bind the successful compiler output to the exact inputs observed before execution. */
export function runtimeBuildReceipt(plan, repository, observed) {
  const current = captureRuntimeBuildSources(plan, repository);
  if (JSON.stringify(current) !== JSON.stringify(observed)) throw new Error('Source changed during compilation; rebuild before recording Runtime evidence');
  const outputs=captureRuntimeBuildOutputs(plan,repository);
  const execution = readFileSync(resolve(repository,outputs.execution.path));
  if (execution.length < 8 || !execution.subarray(0,8).equals(Buffer.from([0,97,115,109,1,0,0,0])))
    throw new Error(`Compiler output is not WebAssembly: ${resolve(repository,outputs.execution.path)}`);
  const receipt={schema:'eagler-touhou/runtime-source-build/3',
    game:plan.game, variant:plan.variant,
    profile:plan.variant === 'multiplayer' ? 'multiplayer' : plan.variant === 'normal' ? 'production' : 'development',
    executionSha256:outputs.execution.sha256, planSha256:runtimeBuildPlanIdentity(plan),
    requiredSubmodules:plan.requiredSubmodules??[], outputs, sourceFiles:observed};
  return receipt;
}

function runtimeBuildNoopCompilerPlanIdentity(plan) {
  if (!plan || typeof plan !== 'object' || Array.isArray(plan)) return null;
  if (Array.isArray(plan.sourceExclusions) && plan.sourceExclusions.length) return null;
  const {schema, sources, ...inputs} = plan;
  if (Array.isArray(inputs.sourceExclusions) && inputs.sourceExclusions.length === 0) delete inputs.sourceExclusions;
  return hash(JSON.stringify(canonicalPlanValue(inputs)));
}
function runtimeBuildNoopCompileInputs(receipt) {
  if (!Array.isArray(receipt?.sourceFiles) || !receipt.sourceFiles.length || receipt.sourceFiles.some(file =>
    !file || typeof file.path !== 'string' || !/^[a-f0-9]{64}$/i.test(file.sha256 ?? ''))) return null;
  const ignored = new Set([...RUNTIME_BUILD_CONTROL_FILES, ...RUNTIME_BUILD_HELPER_FILES]);
  const files = canonicalRuntimeBuildSources(receipt.sourceFiles.filter(file => !ignored.has(file.path)));
  return files.length ? files : null;
}
function runtimeBuildNoopOutputs(receipt) {
  const outputs = receipt?.outputs;
  if (!outputs || typeof outputs !== 'object' || Array.isArray(outputs)) return null;
  const result = {};
  for (const role of Object.keys(outputs).sort(compareRuntimeSourcePaths)) {
    const output = outputs[role];
    if (!output || typeof output !== 'object' || Array.isArray(output) ||
      JSON.stringify(Object.keys(output).sort()) !== JSON.stringify(['path', 'sha256']) ||
      typeof output.path !== 'string' || !/^[a-f0-9]{64}$/i.test(output.sha256 ?? '')) return null;
    result[role] = {path: output.path, sha256: output.sha256};
  }
  return Object.keys(result).length ? result : null;
}
export function runtimeBuildCanReuseCmakeNoop({recipeKind,previousReceipt,previousPlan,currentReceipt,currentPlan}) {
  if (recipeKind !== 'cmake' || !previousReceipt || !previousPlan || !currentReceipt || !currentPlan) return false;
  if ((Array.isArray(previousPlan.sourceExclusions) && previousPlan.sourceExclusions.length) ||
      (Array.isArray(currentPlan.sourceExclusions) && currentPlan.sourceExclusions.length)) return false;
  const expectedProfile = plan => plan.variant === 'multiplayer' ? 'multiplayer' : plan.variant === 'normal' ? 'production' : 'development';
  if (currentReceipt.schema !== 'eagler-touhou/runtime-source-build/3' ||
      currentReceipt.game !== currentPlan.game || currentReceipt.variant !== currentPlan.variant ||
      currentReceipt.profile !== expectedProfile(currentPlan) ||
      previousReceipt.schema !== 'eagler-touhou/runtime-source-build/3' ||
      previousReceipt.schema !== currentReceipt.schema || previousReceipt.profile !== currentReceipt.profile ||
      previousReceipt.game !== currentReceipt.game || previousReceipt.variant !== currentReceipt.variant ||
      previousReceipt.game !== previousPlan.game || previousReceipt.variant !== previousPlan.variant ||
      previousReceipt.profile !== expectedProfile(previousPlan)) return false;
  if (previousReceipt.planSha256 !== runtimeBuildPlanIdentity(previousPlan) ||
      currentReceipt.planSha256 !== runtimeBuildPlanIdentity(currentPlan)) return false;
  const previousPlanIdentity = runtimeBuildNoopCompilerPlanIdentity(previousPlan);
  const currentPlanIdentity = runtimeBuildNoopCompilerPlanIdentity(currentPlan);
  if (!previousPlanIdentity || previousPlanIdentity !== currentPlanIdentity) return false;
  const previousInputs = runtimeBuildNoopCompileInputs(previousReceipt);
  const currentInputs = runtimeBuildNoopCompileInputs(currentReceipt);
  if (!previousInputs || !currentInputs || JSON.stringify(previousInputs) !== JSON.stringify(currentInputs)) return false;
  if (JSON.stringify(previousReceipt.requiredSubmodules ?? []) !== JSON.stringify(previousPlan.requiredSubmodules ?? []) ||
      JSON.stringify(currentReceipt.requiredSubmodules ?? []) !== JSON.stringify(currentPlan.requiredSubmodules ?? []) ||
      JSON.stringify(previousReceipt.requiredSubmodules ?? []) !== JSON.stringify(currentReceipt.requiredSubmodules ?? [])) return false;
  const previousOutputs = runtimeBuildNoopOutputs(previousReceipt);
  const currentOutputs = runtimeBuildNoopOutputs(currentReceipt);
  if (!previousOutputs || !currentOutputs || JSON.stringify(previousOutputs) !== JSON.stringify(currentOutputs)) return false;
  const expectedCmakeRoles = ['entry','execution','loader'];
  if (JSON.stringify(Object.keys(previousOutputs).sort(compareRuntimeSourcePaths)) !== JSON.stringify(expectedCmakeRoles) ||
      JSON.stringify(Object.keys(currentOutputs).sort(compareRuntimeSourcePaths)) !== JSON.stringify(expectedCmakeRoles)) return false;
  return previousReceipt.executionSha256 === currentReceipt.executionSha256 &&
    previousOutputs.execution?.sha256 === previousReceipt.executionSha256 &&
    currentOutputs.execution?.sha256 === currentReceipt.executionSha256;
}

function validCmakeNoopReuse({previousReceipt, previousPlan, currentReceipt, currentPlan, repository}) {
  if (configuredRuntimeSource(currentPlan.game, currentPlan.variant, repository).recipe.kind !== 'cmake') return false;
  return runtimeBuildCanReuseCmakeNoop({recipeKind:'cmake',previousReceipt,previousPlan,currentReceipt,currentPlan});
}

export async function executeRuntimeSourceBuild(plan, repository, {previousReceipt,previousPlan}={}) {
  if (!Array.isArray(plan.steps) || !plan.steps.length) throw new Error('Source build requires executable compiler steps');
  const observed = captureRuntimeBuildSources(plan, repository);
  const outputs=expectedRuntimeBuildOutputs(plan.game,plan.variant,repository);
  const identity = () => Object.fromEntries(Object.entries(outputs).map(([role,{path}])=>{
    const absolute=resolve(repository,path),stat=lstatSync(absolute,{throwIfNoEntry:false,bigint:true});
    if(!stat)return [role,null];
    if(!stat.isFile()||stat.isSymbolicLink())throw new Error(`Runtime build output must be a regular file: ${absolute}`);
    return [role,{sha256:hash(readFileSync(absolute)),inode:String(stat.ino),size:String(stat.size),mtimeNs:String(stat.mtimeNs),ctimeNs:String(stat.ctimeNs)}];
  }));
  const before=identity();
  let receiptBefore=null;
  try { receiptBefore=runtimeBuildReceipt(plan,repository,observed); }
  catch {}
  for (const step of plan.steps) await new Promise((accept, reject) => {
    const child = spawn(step.command, step.args, {cwd:step.cwd, shell:false, windowsHide:true,
      stdio:'inherit', env:{...process.env, ...plan.environment}});
    child.once('error', reject);
    child.once('exit', code=>code === 0 ? accept() : reject(new Error(`${plan.game}/${plan.variant} build failed (${code})`)));
  });
  const receipt=runtimeBuildReceipt(plan, repository, observed);
  const canonicalReceipt=value=>value&&Array.isArray(value.sourceFiles)
    ? {...value,sourceFiles:canonicalRuntimeBuildSources(value.sourceFiles)}:value;
  const after=identity();
  const refreshedExecution=JSON.stringify(before.execution)!==JSON.stringify(after.execution);
  const previousMatchesBefore=receiptBefore&&JSON.stringify(canonicalReceipt(previousReceipt))===JSON.stringify(canonicalReceipt(receiptBefore));
  const validStrictIncrementalReuse=previousMatchesBefore&&JSON.stringify(canonicalReceipt(receiptBefore))===JSON.stringify(receipt)&&
    previousPlan&&runtimeBuildPlanIdentity(previousPlan)===runtimeBuildPlanIdentity(plan);
  let validNodeManifestRebind=false;
  if(previousMatchesBefore&&previousPlan&&runtimeBuildPlanIdentity(previousPlan)===runtimeBuildPlanIdentity(plan)&&
     previousReceipt.planSha256===runtimeBuildPlanIdentity(previousPlan)&&
     configuredRuntimeSource(plan.game,plan.variant,repository).recipe.kind==='node'){
    const expectedRoles=Object.keys(expectedRuntimeBuildOutputs(plan.game,plan.variant,repository)).sort(compareRuntimeSourcePaths);
    const prior=canonicalReceipt(receiptBefore),final=canonicalReceipt(receipt);
    const priorRoles=Object.keys(prior.outputs??{}).sort(compareRuntimeSourcePaths);
    const finalRoles=Object.keys(final.outputs??{}).sort(compareRuntimeSourcePaths);
    const priorManifest=prior.outputs?.buildManifest,finalManifest=final.outputs?.buildManifest;
    if(JSON.stringify(priorRoles)===JSON.stringify(expectedRoles)&&JSON.stringify(finalRoles)===JSON.stringify(expectedRoles)&&
       priorManifest&&finalManifest&&priorManifest.path===finalManifest.path&&
       /^[a-f0-9]{64}$/i.test(priorManifest.sha256??'')&&/^[a-f0-9]{64}$/i.test(finalManifest.sha256??'')){
      final.outputs={...final.outputs,buildManifest:{...finalManifest,sha256:priorManifest.sha256}};
      validNodeManifestRebind=JSON.stringify(final)===JSON.stringify(prior);
    }
  }
  const validIncrementalReuse=validStrictIncrementalReuse || validNodeManifestRebind || (receiptBefore &&
    validCmakeNoopReuse({previousReceipt,previousPlan,currentReceipt:receipt,currentPlan:plan,repository}));
  if(!refreshedExecution&&!validIncrementalReuse)
    throw new Error('Compiler did not refresh code outputs (execution WASM) and no matching prior build receipt and plan exist');
  return receipt;
}
