import {printPC98Inputs} from '../../../tools/build/pc98-inputs.mjs';
import {createHash} from 'node:crypto';
import {readFile,writeFile,mkdir,readdir,copyFile,stat,mkdtemp,rename,rm} from 'node:fs/promises';
import {resolve,relative,dirname} from 'node:path';
import {tmpdir} from 'node:os';
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
async function nativeMarkerInputs(){
  const meta=JSON.parse(await readFile(resolve(web,'native-patch.json'),'utf8'));
  const source=await readFile(resolve(privateInputs.has('native/main.exe')?assets:web,'native/main.exe'));
  if(createHash('sha256').update(source).digest('hex')!==meta.sha256)throw Error('TH03 native marker source identity mismatch');
  const bytes=Buffer.from(source), header=bytes.readUInt16LE(8)*16;
  const origin=header+meta.codeSegment*16+meta.hooks.find(h=>h.imageOffset===0x990b).target;
  const region=bytes.subarray(origin,origin+256);
  // Preserve the native 5-by-3 marker: its white center is symmetric around
  // the collision origin, including when the player crosses a byte edge.
  for(const before of ['bbf800','26899da000','bb7000d3cbb0ff']){
    const old=Buffer.from(before,'hex'), at=region.indexOf(old);
    if(at<0||region.indexOf(old,at+1)!==-1)throw Error('TH03 native marker instruction changed');
  }
  const sha=createHash('sha256').update(bytes).digest('hex');
  const soloMeta=JSON.parse(await readFile(resolve(web,'solo-assist.json'),'utf8'));
  const original=await readFile(resolve(privateInputs.has('native/original-main.exe')?assets:web,'native/original-main.exe'));
  if(createHash('sha256').update(original).digest('hex')!==soloMeta.originalSha256)throw Error('TH03 solo source identity mismatch');
  const temp=await mkdtemp(resolve(tmpdir(),'th03-native-marker-'));
  let payload;
  try{
    const out=resolve(temp,'solo.bin');
    execFileSync(process.env.NASM||'nasm',['-f','bin','patches/solo-marker.asm','-o',out],{cwd:port,stdio:'inherit'});
    payload=await readFile(out);
  }finally{await rm(temp,{recursive:true,force:true});}
  const entries=Array.from({length:6},(_,i)=>payload.readUInt16LE(i*2));
  const insert=0xe8f0, codeBase=0x96e0, tail=Buffer.from('ff207c9a5f5ec9c3','hex');
  const gap=(tail.length+payload.length+15)&~15, headerSize=original.readUInt16LE(8)*16;
  const hdr=Buffer.from(original.subarray(0,headerSize)),image=Buffer.from(original.subarray(headerSize));
  if(!image.subarray(insert,insert+tail.length).equals(tail))throw Error('TH03 solo native insertion anchor changed');
  const count=hdr.readUInt16LE(6),table=hdr.readUInt16LE(24),relocations=[];
  for(let i=0;i<count;i++){
    const at=hdr.readUInt16LE(table+i*4)+hdr.readUInt16LE(table+i*4+2)*16;relocations.push(at);
    const target=image.readUInt16LE(at);if(target*16>=insert)image.writeUInt16LE(target+gap/16,at);
    const moved=at+(at>=insert?gap:0);hdr.writeUInt16LE(moved%16,table+i*4);hdr.writeUInt16LE(Math.floor(moved/16),table+i*4+2);
  }
  for(const at of [14,22]){const segment=hdr.readUInt16LE(at);if(segment*16>=insert)hdr.writeUInt16LE(segment+gap/16,at);}
  const hooks=[[0x977e,'e8b70c',entries[0]],[0x972f,'e84600',entries[1]],[0xdaca,'e851fc',entries[3]],[0x9842,'e8602f',entries[4]],[0x990b,'e80a46',entries[5]]];
  for(const [at,expected,target] of hooks){
    if(image.subarray(at,at+3).toString('hex')!==expected||relocations.some(x=>x>=at-1&&x<at+3))throw Error('TH03 solo native hook changed');
    image[at]=0xe8;image.writeUInt16LE((codeBase+target-at-3)&0xffff,at+1);
  }
  const body=Buffer.concat([image.subarray(0,insert),tail,payload,Buffer.alloc(gap-tail.length-payload.length),image.subarray(insert)]);
  const length=hdr.length+body.length;hdr.writeUInt16LE(length%512,2);hdr.writeUInt16LE(Math.ceil(length/512),4);
  const soloBytes=Buffer.concat([hdr,body]);
  const newSoloMeta={...soloMeta,sha256:createHash('sha256').update(soloBytes).digest('hex'),bytes:soloBytes.length,insertSize:gap,dataSegment:0x1d56+gap/16,mailboxCsOffset:entries[2],hooks:hooks.map(([imageOffset,original,target])=>({imageOffset,original,target}))};
  return {bytes,meta:Buffer.from(JSON.stringify({...meta,sha256:sha,acceptedMainHashes:[...new Set([...meta.acceptedMainHashes,sha])]},null,2)+'\n'),soloBytes,soloMeta:Buffer.from(JSON.stringify(newSoloMeta,null,2)+'\n')};
}
let markerInputs;
async function readInput(name){
  if(['native/main.exe','native-patch.json','native/solo-main.exe','solo-assist.json'].includes(name)){
    markerInputs??=await nativeMarkerInputs();
    return name==='native/main.exe'?markerInputs.bytes:name==='native-patch.json'?markerInputs.meta:name==='native/solo-main.exe'?markerInputs.soloBytes:markerInputs.soloMeta;
  }
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
    try{const bytes=await readFile(resolve(output,name));if(typeof table[name]==='string')table[name]=sha(bytes);}catch(error){if(error.code!=='ENOENT')throw error;const packaged=input.find(item=>item.path===name);if(packaged&&typeof table[name]==='string')table[name]=packaged.sha256;else if(!packaged)delete table[name];}
  }
  await writeFile(resolve(output,manifestPath),JSON.stringify(manifest,null,2)+'\n');
}
const escape=text=>text.replaceAll('&','&amp;').replaceAll('<','&lt;').replaceAll('>','&gt;');
let notices='<!doctype html><html lang="en"><meta charset="utf-8"><title>TH03 runtime notices</title><body><h1>TH03 runtime sources and licenses</h1><p>Adapted from <a href="https://github.com/IxinorTyan/th03-netplay-lab">th03-netplay-lab</a>, revision '+escape(revision??'local working tree')+'.</p>';
for(const name of ['vendor/dosbox-x/NOTICE.md','vendor/dosbox-x/LICENSE','room-ui-assets/LICENSE.txt'])notices+='<h2>'+escape(name)+'</h2><pre style="white-space:pre-wrap">'+escape(await readFile(resolve(web,name),'utf8'))+'</pre>';
await writeFile(resolve(output,'NOTICE.html'),notices+'<h2>PMD player</h2><p><a href="pmd-player.UPSTREAM.txt">Source</a> · <a href="pmd-player.NOTICE.txt">Notice</a> · <a href="pmd-player.LICENSE.txt">License</a></p></body></html>');runtime.push('NOTICE.html');
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
