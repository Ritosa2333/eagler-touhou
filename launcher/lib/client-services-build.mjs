import {existsSync} from 'node:fs';
import {mkdir, readFile, readdir, writeFile, rm, cp} from 'node:fs/promises';
import {spawn} from 'node:child_process';
import {isAbsolute, relative, resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {setTimeout as delay} from 'node:timers/promises';
import {createHash} from 'node:crypto';
import {resolvePublicOrRepositorySource} from './public-source.mjs';
import {resolveRuntimeGenerationWorkerSource} from './contracts-build.mjs';
import {localModuleClosure} from './browser-module-graph.mjs';
import {sharedContractInputs,copySharedContractOutputs} from './shared-contract-sources.mjs';
const project = resolve(fileURLToPath(new URL('..', import.meta.url)));
const output = resolve(project, '.cache/build/browser');
async function sources(directory) {
  const result = [];
  for (const entry of await readdir(directory, {withFileTypes:true})) {
    const path = resolve(directory, entry.name);
    if (entry.isDirectory()) result.push(...await sources(path));
    else if (entry.isFile() && entry.name.endsWith('.mts')) result.push(path);
  }
  return result.sort();
}
/** Pure contracts/services for Node acceptance and operator tools. No UI renderer. */
export async function ensureClientServicesBuild({force=false}={}) {
  if (!existsSync(resolve(project, 'src/contracts'))) return {built:false,mode:"prebuilt",sourceCount:0};
  const files = await sources(resolve(project,'src'));
  const expected = new Set(files.filter(path => !path.endsWith('.d.mts')).map(path =>
    'assets/' + relative(resolve(project, 'src'), path).replaceAll('\\', '/').replace(/\.mts$/, '.mjs')));
  const hash = createHash('sha256');
  for (const path of [...files, ...sharedContractInputs(project), fileURLToPath(import.meta.url), resolve(project,'lib/shared-contract-sources.mjs'), resolve(project,'tsconfig.launcher.json'), resolve(project,'package-lock.json')]) {
    hash.update(relative(project,path)); hash.update(await readFile(path));
  }
  const identity = hash.digest('hex'), state = resolve(project,'.cache/client-services-build.json');
  await mkdir(resolve(project,'.cache'),{recursive:true});
  const lock = resolve(project,'.cache/client-services-build.lock');
  const started = Date.now();
  for (;;) {try {await mkdir(lock);break;} catch(error) {if(error.code!=='EEXIST')throw error;if(Date.now()-started>180000)throw Error('Client service build is still locked');await delay(25);}}
  try {
    if (!force) {
      try {
        const previous = JSON.parse(await readFile(state,'utf8'));
        const complete = [...expected].every(path => existsSync(resolve(output,path)));
        const unchanged = previous.dependencies && (await Promise.all(previous.dependencies.map(async ([path,digest]) =>
          createHash('sha256').update(await readFile(resolvePublicOrRepositorySource(path))).digest('hex') === digest))).every(Boolean);
        if(previous.identity===identity && complete && unchanged) return {built:false,sourceCount:files.length};
      } catch {}
    }
    await new Promise((yes,no)=>{
      const child=spawn(process.execPath,[resolve(project,'node_modules/typescript/bin/tsc'),'-p','tsconfig.launcher.json','--pretty','false'],{cwd:project,stdio:'inherit'});
      child.once('error',no);child.once('exit',code=>code===0?yes():no(Error(`Client service compilation failed (${code})`)));
    });
    await copySharedContractOutputs(project,resolve(output,'assets'));
    const generated=[];
    async function collect(directory,prefix='') {
      for (const entry of await readdir(directory,{withFileTypes:true})) {
      const path=prefix+entry.name;
      if(entry.isDirectory()) await collect(resolve(directory,entry.name),path+'/');
        else if(entry.name.endsWith('.mjs')) {
          if(expected.has(path)) generated.push(path);
          else {
            const stale = resolve(directory,entry.name);
            const confined = relative(output,stale);
            if(isAbsolute(confined) || confined === '..' || confined.startsWith('../') || confined.startsWith('..\\')) throw Error('Stale service output escaped the build directory');
            await rm(stale);
          }
        }
      }
    }
    await collect(resolve(output,'assets'),'assets/');
    const modules=await localModuleClosure({root:project,entries:generated,allowBareImports:true,allowDynamicImports:true,
      resolveFile:path=>path.startsWith('assets/')?resolve(output,path):resolvePublicOrRepositorySource(path)});
    const dependencies = [];
    for(const path of modules) if(!path.startsWith('assets/')) {
      const target=resolve(output,path);await mkdir(resolve(target,'..'),{recursive:true});
      const source = resolvePublicOrRepositorySource(path);
      await cp(source,target);
      dependencies.push([path,createHash('sha256').update(await readFile(source)).digest('hex')]);
    }
    await writeFile(state,JSON.stringify({identity,dependencies})+'\n');
    return {built:true,sourceCount:files.length};
  } finally {await rm(lock,{recursive:true});}
}
export function isLauncherBuildOutputPath(path) {return /^assets\/(?:contracts|launcher)\//.test(path) && !/(?:^|\/)(?:app|lobby)\.mjs$/.test(path);}
export function isMappedBrowserPublicationPath(path) {return isLauncherBuildOutputPath(path);}
export function resolveBrowserPublicationSource(path) {
  if (/(?:^|\/)(?:app|lobby)\.mjs$/.test(path)) throw Error('Retired UI modules are not publication inputs');
  if (isLauncherBuildOutputPath(path) && existsSync(resolve(project,'src/contracts')) &&
      !existsSync(resolve(project,'src',path.slice('assets/'.length).replace(/\.mjs$/, '.mts')))) {
    throw Error('Service publication requires a current typed source: ' + path);
  }
  return isLauncherBuildOutputPath(path) && existsSync(resolve(project,'src/contracts'))
    ? resolve(output,path) : resolvePublicOrRepositorySource(path);
}
export {resolveRuntimeGenerationWorkerSource};
