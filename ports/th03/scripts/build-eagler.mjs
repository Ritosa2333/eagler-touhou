import {printPC98Inputs} from '../../../tools/build/pc98-inputs.mjs';
import {createHash} from 'node:crypto';
import {readFile,writeFile,mkdir,readdir,copyFile,stat,mkdtemp,rename,rm} from 'node:fs/promises';
import {resolve,relative,dirname} from 'node:path';
import {fileURLToPath} from 'node:url';
import {execFileSync} from 'node:child_process';
import {assertPortableRuntimeSources} from '../../../launcher/lib/runtime-generations.mjs';
import {DATA_SCHEMA,DATA_MAGIC,DATA_LAYOUT_SPEC} from '../web/managed-data.mjs';
const port = resolve(fileURLToPath(new URL('..', import.meta.url))), repo=resolve(port,'../..');
const args = Object.fromEntries(process.argv.slice(2).map(arg => { const at=arg.indexOf('='); return [arg.slice(2,at<0?undefined:at),at<0?true:arg.slice(at+1)]; }));
if(args['print-inputs']) await printPC98Inputs({port,game:'th03'});
const source = resolve(args.source||port), web=resolve(source,'web');
const sourceIdentity=args.source?null:JSON.parse(await readFile(resolve(port,'source-import.json'),'utf8'));
const privateInputs=new Set(sourceIdentity?.resources||[]);
const assets=resolve(args.assets||resolve(port,'.cache/source-resources'));
async function readInput(name){
  try{return await readFile(resolve(privateInputs.has(name)?assets:web,name));}
  catch(error){if(error.code==='ENOENT'&&privateInputs.has(name))throw Error('Missing private resource '+name+'; specify --assets=<standalone web directory>.');throw error;}
}
let revision=sourceIdentity?.commit??null;if(args.source){try{revision=execFileSync('git',['-c','safe.directory='+source.replaceAll('\\','/'),'rev-parse','HEAD'],{cwd:source,encoding:'utf8'}).trim();}catch{}}
const sha = bytes => createHash('sha256').update(bytes).digest('hex');
async function files(directory,prefix='') {
  const result=[];
  for(const item of await readdir(directory,{withFileTypes:true})) {
    if(item.isSymbolicLink()) throw Error('Source symlinks are not accepted');
    if(item.isDirectory()) result.push(...await files(resolve(directory,item.name),prefix+item.name+'/'));
    else if(item.isFile()) result.push(prefix+item.name);
  }
  return result.sort();
}
const names=[...new Set([...(await files(web)).filter(name=>!/^managed-[^/]+\.(?:mjs|js)$/.test(name)),...privateInputs])].sort().filter(name=>name!=='room-title-th05.js'&&!/(?:^|\/)(?:build|reports|baseline|screenshots|backups|tools|test-results|__pycache__)(?:\/|$)/.test(name) && !/\.(?:map|wat|cpuprofile|log|tmp|bak)$/.test(name));
const input=[]; for(const name of names) { const bytes=await readInput(name); input.push({path:name,bytes:bytes.length,sha256:sha(bytes)}); }
if(args['print-package-inputs']) { console.log(JSON.stringify({game:'th03',kind:'managed-import',inputs:input})); process.exit(0); }
const cache=resolve(port,'.cache');await mkdir(cache,{recursive:true});
const staging=await mkdtemp(resolve(cache,'build-')),output=resolve(staging,'build-eagler'),content=resolve(staging,'content');
try {
await mkdir(output,{recursive:true});await mkdir(content,{recursive:true});
const runtime=[], payload=[], entries=[];let offset=0;
for(const item of input) {
  const bytes=await readInput(item.path);
  if(/\.(?:js|mjs|css|html|wasm|json)$/.test(item.path)) {
    const target=resolve(output,item.path);await mkdir(dirname(target),{recursive:true});await writeFile(target,bytes);runtime.push(item.path);
  } else {
    if(!bytes.length) continue;
    entries.push({...item,offset});payload.push(bytes);offset+=bytes.length;
  }
}
function replaceOnce(text,from,to,label) { if(text.split(from).length!==2) throw Error('Upstream changed at '+label); return text.replace(from,to); }
const patch=async(name,edit)=>{const path=resolve(output,name);await writeFile(path,edit((await readFile(path,'utf8')).replaceAll('\r\n','\n')));};
const {adaptRuntime}=await import('./adapt-runtime.mjs');
await adaptRuntime({patch,replaceOnce,output,port,web,runtime,repo,files,readInput});
// Adapted sources have their own verified identity; keep upstream asset checks active.
for(const manifestPath of ['netplay/runtime.json','lockstep-runtime.json']) {
  let manifest;try{manifest=JSON.parse(await readFile(resolve(output,manifestPath),'utf8'));}catch(error){if(error.code==='ENOENT')continue;throw error;}
  for(const table of [manifest.files,manifest.assets])if(table)for(const name of Object.keys(table)) {
    if(name===manifestPath)throw Error('Self-referencing runtime hash');
    try{const bytes=await readFile(resolve(output,name));if(typeof table[name]==='string')table[name]=sha(bytes);}catch(error){if(error.code!=='ENOENT')throw error;if(!names.includes(name))delete table[name];}
  }
  await writeFile(resolve(output,manifestPath),JSON.stringify(manifest,null,2)+'\n');
}
const escape=text=>text.replaceAll('&','&amp;').replaceAll('<','&lt;').replaceAll('>','&gt;');
let notices='<!doctype html><html lang="en"><meta charset="utf-8"><title>TH03 runtime notices</title><body><h1>TH03 runtime sources and licenses</h1><p>Adapted from <a href="https://github.com/IxinorTyan/th03-netplay-lab">th03-netplay-lab</a>, revision '+escape(revision??'local working tree')+'.</p>';
for(const name of ['vendor/dosbox-x/NOTICE.md','vendor/dosbox-x/LICENSE','room-ui-assets/LICENSE.txt'])notices+='<h2>'+escape(name)+'</h2><pre style="white-space:pre-wrap">'+escape(await readFile(resolve(web,name),'utf8'))+'</pre>';
await writeFile(resolve(output,'NOTICE.html'),notices+'</body></html>');runtime.push('NOTICE.html');
const manifestBytes=Buffer.from(JSON.stringify({schema:DATA_SCHEMA,game:'th03',files:entries})),header=Buffer.alloc(12);header.write(DATA_MAGIC);header.writeUInt32LE(manifestBytes.length,8);
await writeFile(resolve(content,'th03.data'),Buffer.concat([header,manifestBytes,...payload]));
const sources=new Map();for(const name of [...new Set(runtime)].sort())sources.set(name,await readFile(resolve(output,name)));assertPortableRuntimeSources(sources);
const identities={};for(const name of [...new Set(runtime)].sort()){const bytes=await readFile(resolve(output,name));identities[name]={bytes:bytes.length,sha256:sha(bytes)};}
await writeFile(resolve(output,'manifest.json'),JSON.stringify({game:'th03',protocol:'eagler-touhou/1',features:{thprac:false,languages:false,focusHitbox:false},files:identities},null,2)+'\n');
const manifestFile=await readFile(resolve(output,'manifest.json'));identities['manifest.json']={bytes:manifestFile.length,sha256:sha(manifestFile)};
await writeFile(resolve(output,'runtime-files.json'),JSON.stringify({schema:'eagler-touhou/runtime-directory/1',files:identities},null,2)+'\n');
await writeFile(resolve(content,'source.json'),JSON.stringify({schema:'eagler-touhou/pc98-import/1',upstream:'https://github.com/IxinorTyan/th03-netplay-lab',revision,sourceInputs:input,dataLayout:'sha256-'+sha(JSON.stringify(DATA_LAYOUT_SPEC)),adaptation:'ports/th03/web'},null,2)+'\n');
const contentBytes=(await stat(resolve(content,'th03.data'))).size;
// A failed patch leaves the last successful export intact. Swap complete
// directories so removed upstream files cannot linger in a later generation.
const swaps=[];
try {
  for(const name of ['build-eagler','content']){
    const target=resolve(port,name),backup=resolve(staging,'previous-'+name);
    let previous=false;
    try{await rename(target,backup);previous=true;}catch(error){if(error.code!=='ENOENT')throw error;}
    const swap={target,backup,previous,installed:false};swaps.push(swap);
    await rename(resolve(staging,name),target);swap.installed=true;
  }
}catch(error){
  for(const swap of swaps.reverse()){
    if(swap.installed)await rename(swap.target,resolve(staging,'failed-'+relative(port,swap.target)));
    if(swap.previous)await rename(swap.backup,swap.target);
  }
  throw error;
}
console.log(JSON.stringify({game:'th03',runtime:'ports/th03/build-eagler',data:'ports/th03/content/th03.data',runtimeFiles:Object.keys(identities).length,contentFiles:entries.length,contentBytes}));
} finally {
  // staging is created by mkdtemp directly inside this port's ignored cache.
  if(dirname(staging)!==cache||!relative(cache,staging).startsWith('build-'))throw Error('Invalid build staging path');
  await rm(staging,{recursive:true,force:true});
}
