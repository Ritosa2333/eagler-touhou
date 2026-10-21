import {existsSync, readFileSync, readdirSync} from 'node:fs';
import {resolve, relative, sep} from 'node:path';
import {execFile} from 'node:child_process';
import {createHash} from 'node:crypto';
import {promisify} from 'node:util';
import {expectedRuntimeBuildSources, expectedRuntimeBuildSubmodules} from '../../launcher/lib/runtime-build-evidence.mjs';

export const root = resolve(import.meta.dirname, '../..');
export const INPUT_INSPECTION_TIMEOUT_MS = 30_000;
export const INPUT_INSPECTION_MAX_BUFFER_BYTES = 8*1024*1024;
const execFileAsync=promisify(execFile);
export const layout = JSON.parse(readFileSync(resolve(root, 'launcher/config/workspace.json'), 'utf8'));
export const builds = JSON.parse(readFileSync(resolve(root, 'launcher/config/runtime-builds.json'), 'utf8'));
export const slash = value => value.split(sep).join('/');
export function dependencyRevision(path, revision) {
  return {path,revision};
}

// Pair required module pins with the exact source files captured by the Runtime build inventory.
export function runtimeDependencyInputs(requiredSubmodules, sourceFiles, repository=root) {
  if (!Array.isArray(requiredSubmodules) || !Array.isArray(sourceFiles))
    throw new Error('Runtime dependency inputs require captured submodules and source files');
  const dependencies=[],inputFiles=new Set();
  for (const module of requiredSubmodules) {
    if (!module || typeof module.path!=='string' || typeof module.commit!=='string')
      throw new Error('Invalid required Runtime submodule identity');
    const prefix=`${module.path}/`;
    const files=sourceFiles.filter(file=>typeof file?.path==='string'&&file.path.startsWith(prefix));
    if (!files.length) throw new Error(`Required Runtime submodule has no captured source files: ${module.path}`);
    dependencies.push(dependencyRevision(module.path,module.commit));
    for (const file of files) inputFiles.add(resolve(repository,file.path));
  }
  return {dependencies,inputFiles:[...inputFiles]};
}

export function sourceDependencies(target) {
  if (target.recipe.kind!=='cmake') return {dependencies:[],inputFiles:[]};
  const requiredSubmodules=expectedRuntimeBuildSubmodules(target.game,target.variant,root);
  const sourceFiles=expectedRuntimeBuildSources(target.game,target.variant,root);
  return runtimeDependencyInputs(requiredSubmodules,sourceFiles,root);
}
export function targets({game, variant = 'both'} = {}) {
  if (game && !builds.games[game]) throw new Error(`Unknown game: ${game}`);
  if (!['normal', 'multiplayer', 'both'].includes(variant)) throw new Error(`Unknown variant: ${variant}`);
  const result = [];
  for (const [id, entry] of Object.entries(builds.games)) {
    if (game && id !== game) continue;
    for (const name of variant === 'both' ? ['normal', 'multiplayer'] : [variant]) {
      const definition = entry.variants[name];
      if (!definition) {if (game && variant !== 'both') throw new Error(`${game} has no ${variant} build`); continue;}
      result.push({game:id, variant:name, directory:layout.repositories[definition.workspaceRepository ?? entry.workspaceRepository],
        recipe:{...entry.sourceBuild, ...definition.sourceBuild}, definition});
    }
  }
  return result;
}

function headers(directory) {
  return readdirSync(directory, {withFileTypes:true}).flatMap(entry => {
    const path = resolve(directory, entry.name);
    return entry.isDirectory() ? headers(path) : /\.(?:h|hpp|inc)$/.test(entry.name) ? [path] : [];
  });
}

function commandEnvironment(environment) {
  const child = {...environment};
  // Input inspection is a standalone manifest query, not a nested debugger/test session.
  delete child.NODE_OPTIONS;
  delete child.NODE_TEST_CONTEXT;
  return child;
}

function capturedOutput(value) {
  if (value === undefined || value === null) return '<empty>';
  const text = Buffer.isBuffer(value) ? value.toString('utf8') : String(value);
  return JSON.stringify(text.length > 2000 ? `${text.slice(0,2000)}…[truncated]` : text);
}

export async function inspectInputs(target, {
  repositoryRoot=root, timeoutMs=INPUT_INSPECTION_TIMEOUT_MS, environment=process.env,
}={}) {
  const owner = resolve(repositoryRoot, target.directory);
  if (target.recipe.kind === 'cmake') {
    const cmake = readFileSync(resolve(owner, 'CMakeLists.txt'), 'utf8');
    const names = [...new Set([...cmake.matchAll(/\bsrc\/[A-Za-z0-9_./-]+\.(?:cpp|c)\b/g)].map(match => match[0]))].sort();
    const files = [...names.map(name => resolve(owner, name)), ...headers(resolve(owner, 'src'))];
    for (const path of files) if (!existsSync(path)) throw new Error(`Missing declared CMake source: ${path}`);
    return {schema:'eagler-touhou/runtime-inputs/1', directory:target.directory.split('/').at(-1),
      selection:'cmake-declaration-union', translationUnits:names, inputFiles:files,
      compilerFlags:Object.entries(target.definition.cache).map(([name, value]) => `${name}=${value}`),
      command:['node', 'tools/ports/check.mjs', `--game=${target.game}`, `--variant=${target.variant}`]};
  }
  const args = [...target.recipe.build];
  if (target.definition.features.thprac === true && target.recipe.practiceFlag) args.push(target.recipe.practiceFlag);
  const command = [process.execPath, ...args, '--print-inputs'];
  const commandEvidence=JSON.stringify(command),targetEvidence=`${target.game??'<unknown>'}/${target.variant??'<unknown>'} (${owner})`;
  if (!Number.isSafeInteger(timeoutMs)||timeoutMs<1)
    throw new Error(`Invalid input-inspection timeout for ${targetEvidence}: ${timeoutMs}`);
  const context=`target=${targetEvidence} command=${commandEvidence} timeoutMs=${timeoutMs}`;
  let source,elapsedMs;
  const started=performance.now();
  try {
    ({stdout:source}=await execFileAsync(command[0],command.slice(1),{cwd:owner,env:commandEnvironment(environment),encoding:'utf8',
      windowsHide:true,stdio:['ignore','pipe','pipe'],maxBuffer:INPUT_INSPECTION_MAX_BUFFER_BYTES,
      timeout:timeoutMs,killSignal:'SIGKILL'}));
    elapsedMs=Math.round(performance.now()-started);
  } catch(error) {
    elapsedMs=Math.round(performance.now()-started);
    const outputLimit=error.code==='ERR_CHILD_PROCESS_STDIO_MAXBUFFER';
    const timedOut=!outputLimit&&(error.code==='ETIMEDOUT'||(error.killed===true&&error.signal==='SIGKILL'));
    const failure=outputLimit?'exceeded the output limit':timedOut?'timed out':'failed';
    const exitCode=Number.isInteger(error.code)?error.code:error.status;
    throw new Error(`Runtime input inspection ${failure}: ${context} maxBufferBytes=${INPUT_INSPECTION_MAX_BUFFER_BYTES} `+
      `elapsedMs=${elapsedMs} childError=${error.code??'<none>'} exitCode=${exitCode??'<none>'} signal=${error.signal??'<none>'} `+
      `killed=${error.killed===true} `+
      `stdout=${capturedOutput(error.stdout)} stderr=${capturedOutput(error.stderr)}`,{cause:error});
  }
  let report;
  try { report=JSON.parse(source); }
  catch(error) {
    throw new Error(`Runtime input inspection returned invalid JSON: ${context} elapsedMs=${elapsedMs} exitCode=0 `+
      `stdout=${capturedOutput(source)}`,{cause:error});
  }
  if (report?.schema !== 'eagler-touhou/runtime-inputs/1'||!report.inputs||typeof report.inputs!=='object'||Array.isArray(report.inputs))
    throw new Error(`Unexpected runtime input manifest: ${context} elapsedMs=${elapsedMs} exitCode=0`);
  return {...report, selection:'build-entrypoint', inputFiles:Object.keys(report.inputs).map(name => resolve(owner, name)),
    command:['node', ...args, '--print-inputs']};
}

/** Match Git's text representation across CRLF/LF checkouts. */
export function sourceIdentity(files, extra = {}) {
  const hash = createHash('sha256').update(JSON.stringify(extra));
  for (const path of [...new Set(files)].sort((a, b) => slash(relative(root,a)).localeCompare(slash(relative(root,b)), 'en'))) {
    const name = slash(relative(root, path));
    const bytes=readFileSync(path);
    const binary=bytes.subarray(0,Math.min(bytes.length,8000)).includes(0);
    const content=binary?bytes:Buffer.from(bytes.toString('latin1').replaceAll('\r\n','\n'),'latin1');
    hash.update(name).update('\0').update(content).update('\0');
  }
  return hash.digest('hex');
}
