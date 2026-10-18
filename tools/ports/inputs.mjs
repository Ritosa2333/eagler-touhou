import {existsSync, readFileSync, readdirSync} from 'node:fs';
import {resolve, relative, sep} from 'node:path';
import {execFileSync} from 'node:child_process';
import {createHash} from 'node:crypto';
import {sourceIdentity as gitSourceIdentity} from '../../launcher/lib/release-manifest.mjs';

export const root = resolve(import.meta.dirname, '../..');
export const layout = JSON.parse(readFileSync(resolve(root, 'launcher/config/workspace.json'), 'utf8'));
export const builds = JSON.parse(readFileSync(resolve(root, 'launcher/config/runtime-builds.json'), 'utf8'));
export const slash = value => value.split(sep).join('/');
export function dependencyRevision(path, revision, actual) {
  if (actual && actual.revision!==revision) throw new Error(`${path}: dependency checkout differs from its Git pin`);
  return {path,revision,...(actual?.dirty ? {workingTree:actual.sha256} : {})};
}
export async function sourceDependencies(target) {
  if (target.recipe.kind!=='cmake') return [];
  const rows=execFileSync('git',['ls-files','--stage','-z','--',target.directory],
    {cwd:root,encoding:'utf8',windowsHide:true}).split('\0').filter(row=>row.startsWith('160000 '));
  return Promise.all(rows.map(async row=>{
    const [meta,path]=row.split('\t'),revision=meta.split(' ')[1],directory=resolve(root,path);
    const actual=existsSync(resolve(directory,'.git')) ? await gitSourceIdentity(directory) : null;
    return dependencyRevision(path,revision,actual);
  }));
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

export function inspectInputs(target) {
  const owner = resolve(root, target.directory);
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
  const source = execFileSync(command[0], command.slice(1), {cwd:owner, encoding:'utf8', windowsHide:true, maxBuffer:8*1024*1024});
  const report = JSON.parse(source);
  if (report.schema !== 'eagler-touhou/runtime-inputs/1') throw new Error('Unexpected runtime input schema');
  return {...report, selection:'build-entrypoint', inputFiles:Object.keys(report.inputs).map(name => resolve(owner, name)),
    command:['node', ...args, '--print-inputs']};
}

/** Match Git's text representation across CRLF/LF checkouts. */
export function sourceIdentity(files, extra = {}) {
  const hash = createHash('sha256').update(JSON.stringify(extra));
  for (const path of [...new Set(files)].sort((a, b) => slash(relative(root,a)).localeCompare(slash(relative(root,b)), 'en'))) {
    const name = slash(relative(root, path));
    const text = readFileSync(path).toString('latin1').replaceAll('\r\n', '\n');
    hash.update(name).update('\0').update(text,'latin1').update('\0');
  }
  return hash.digest('hex');
}
