import {projectCoopAudio} from './project-coop-audio.mjs';
import {cirnoPalettes} from '../runtime/coop-cirno-palettes.mjs';
import {readCoopPeerBuild} from './build-coop-peer.mjs';
import {createHash} from 'node:crypto';
import {createRequire} from 'node:module';
import {existsSync,lstatSync,mkdirSync,readFileSync,realpathSync,writeFileSync} from 'node:fs';
import {dirname,isAbsolute,join,relative,resolve,sep} from 'node:path';
import {fileURLToPath} from 'node:url';
import {parseArgs} from 'node:util';
import {matchesBuiltCore} from '../../th128/tools/build-runtime.mjs';
import {canonicalRuntimePayload,validateRuntimeGeneration} from '../../../launcher/src/contracts/runtime-generations.mts';

const repo=resolve(dirname(fileURLToPath(import.meta.url)),'../../..');
const normal=join(repo,'ports/th128'),mp=join(repo,'ports/th128-mp');
const sha=data=>createHash('sha256').update(data).digest('hex'),json=value=>Buffer.from(JSON.stringify(value,null,2)+'\n');
const pattern=/^(?:[a-zA-Z0-9_-][a-zA-Z0-9_.-]*\/)*[a-zA-Z0-9_-][a-zA-Z0-9_.-]*$/;
const read=(root,path)=>{
  if(!pattern.test(path))throw new Error('Invalid multiplayer Runtime file path: '+path);
  const name=join(root,path),bytes=readFileSync(name),local=relative(root,realpathSync(name));
  if(isAbsolute(local)||local.startsWith('..'+sep)||local==='..'||!lstatSync(name).isFile()||bytes.length===0||bytes.length>32*1024*1024)
    throw new Error('Invalid multiplayer Runtime input: '+path);
  return {name,bytes};
};
const licenseNames=['EAGLER-TOUCH-NOTICE.md','THPRAC-MIT.txt','Tweet-META-MIT.txt','eagler-touhou-launcher-GPL-3.0.txt','eagler-touhou-th12-MIT.txt','kikones34-MIT.txt'];

function collect({coreSite,transportSite,coopSite,parseModule=createRequire(join(repo,'launcher/package.json'))('acorn').parse}){
  const coreRoot=realpathSync(coreSite),peerRoot=realpathSync(transportSite),payload=new Map(),inputs=new Set();
  function file(root,path){const {name,bytes}=read(root,path);inputs.add(name);return bytes;}
  function emit(path,bytes){if(payload.has(path))throw new Error('Duplicate multiplayer Runtime file: '+path);payload.set(path,bytes);}
  function module(path){
    if(!/^(?:ports\/th128-mp\/(?:site|runtime)|ports\/th128\/site(?:\/runtime)?)\/[a-z0-9_.-]+\.mjs$/.test(path))
      throw new Error('Unsupported multiplayer Runtime dependency: '+path);
    if(payload.has(path))return;
    const original=file(repo,path),bytes=path==='ports/th128/site/runtime/sound.mjs'?Buffer.from(projectCoopAudio(original.toString('utf8'),parseModule)):original;emit(path,bytes);
    const parsed=parseModule(bytes.toString('utf8'),{ecmaVersion:'latest',sourceType:'module'});
    for(const statement of parsed.body){
      if(!['ImportDeclaration','ExportNamedDeclaration','ExportAllDeclaration'].includes(statement.type)||!statement.source)continue;
      const source=statement.source.value;
      if(typeof source!=='string'||!source.startsWith('.'))throw new Error('External package dependency in multiplayer Runtime');
      module(relative(repo,resolve(repo,dirname(path),source)).split(sep).join('/'));
    }
  }
  const html=file(mp,'site/th128.html').toString('utf8');
  if(!html.includes('src="./managed-entry.mjs"')||!html.includes('href="./managed.css"'))throw new Error('Managed Runtime shell is missing its entry');
  emit('th128.html',Buffer.from(html.replace('src="./managed-entry.mjs"','src="./ports/th128-mp/site/managed-entry.mjs"')
    .replace('href="./managed.css"','href="./ports/th128-mp/site/managed.css"')));
  module('ports/th128-mp/site/managed-entry.mjs');emit('ports/th128-mp/site/managed.css',file(mp,'site/managed.css'));
  const receipt=JSON.parse(file(coreRoot,'runtime/build.json')),coreGeneration=receipt.generation;
  const coreDir=`runtime/builds/${coreGeneration}`;
  const normalCore=receipt.privateCore===127||(!!coopSite&&matchesBuiltCore(receipt));
  if(receipt.game!=='th128'||!normalCore||!/^[a-f0-9]{64}$/.test(coreGeneration)||receipt.entry!==`${coreDir}/core.mjs`)
    throw new Error('TH128 multiplayer Runtime needs Core127 or a source-matched single-player core');
  const code={};
  function program(key,root,source,target,expected){
    const bytes=file(root,source),identity={path:target,bytes:bytes.length,sha256:sha(bytes)};
    if(identity.bytes!==expected.bytes||identity.sha256!==expected.sha256)throw new Error('TH128 multiplayer program differs: '+key);
    code[key]=identity;emit(target,bytes);
  }
  for(const [key,name] of [['core','core.mjs'],['wasm','core.wasm']])program(key,coreRoot,`${coreDir}/${name}`,`${coreDir}/${name}`,receipt.artifacts[name]);
  const profile='runtime/bullet-type9-profile.json',profileBytes=file(normal,'site/'+profile);
  code.type9={path:profile,bytes:profileBytes.length,sha256:sha(profileBytes)};emit(profile,profileBytes);
  const transport=coopSite?readCoopPeerBuild(peerRoot):null;
  if(transport){
    for(const [key,name] of [['peer','author-peer.mjs'],['peerWasm','author-peer.wasm']])
      program(key,peerRoot,name,`runtime/transport/${transport.generation}/${name}`,transport.artifacts[name]);
  }else{
  program('peer',peerRoot,'author-peer.mjs','runtime/transport/author-peer.mjs',{
    bytes:82423,sha256:'fe98f3061da44264de1dfc63059899fbfea46c2835456cf74700931d09d0232c',
  });
  program('peerWasm',peerRoot,'author-peer.wasm','runtime/transport/author-peer.wasm',{
    bytes:12617,sha256:'83b4dc05aa03f5c6f05cd8d90133d755bce75fbd2bf92f1a18d8cd7993e4f242',
  });
  }
  if(coopSite)for(const palette of cirnoPalettes){
    const path=`ports/th128-mp/site/palettes/${palette.patchFile}`,bytes=file(repo,path);
    if(bytes.length!==palette.bytes||sha(bytes)!==palette.sha256)throw Error('Cirno palette patch changed: '+palette.patchFile);
    emit(path,bytes);
  }
  let coopGeneration;
  if(coopSite){const base=realpathSync(coopSite),r=JSON.parse(file(base,'receipt.json'));
    if(r.schema!=='th128-coop-core/1'||![r.wasm,r.js,r.inputs].every(h=>typeof h==='string'&&/^[a-f0-9]{64}$/.test(h))||r.wasm===code.wasm.sha256)throw Error('Invalid co-op battle build receipt');
    coopGeneration=r.wasm;for(const [key,name,pin] of [['coopCore','coop-core.mjs',r.js],['coopWasm','coop-core.wasm',r.wasm]]){
      const bytes=file(base,name);if(sha(bytes)!==pin)throw Error('Co-op battle program differs: '+key);
      const path=`runtime/coop/${coopGeneration}/${name}`;code[key]={path,bytes:bytes.length,sha256:pin};emit(path,bytes);
    }
  }
  emit('runtime/program.json',json({schema:coopGeneration?'th128mp-program/2':'th128mp-program/1',coreGeneration,...(coopGeneration?{coopGeneration,peerGeneration:transport.generation}:{}),files:code}));
  emit('manifest.json',json({game:'th128',protocol:'eagler-touhou/1',entry:'th128.html',variant:'multiplayer',dataProvider:'retail-memory',
    dataLayout:receipt.packageDataLayout,features:{thprac:false,languages:false,focusHitbox:false}}));
  emit('licenses/notice.txt',file(normal,'NOTICE.md'));
  for(const name of licenseNames)emit('licenses/'+name,file(normal,'licenses/'+name));
  for(const [name,from] of [['animation-upstream','animation'],['ecl-upstream','ecl']])
    emit('licenses/'+name+'.txt',file(normal,`src/${from}/UPSTREAM_LICENSE.txt`));
  emit('licenses/multiplayer-notice.txt',Buffer.from('TH128 multiplayer uses the shared Eagler Touhou BrowserPeerTransport (GPL-3.0), linked independently of the game Core.\nCorresponding sources: common/src/netplay/BrowserPeerTransport.cpp, common/include/eagler/netplay and ports/th128-mp/transport.\nOriginal DAT, translations, music, and user files are not included in this program directory.\n'));
  const files=[...payload].sort(([a],[b])=>a<b?-1:a>b?1:0).map(([path,bytes])=>({path,bytes:bytes.length,sha256:sha(bytes)}));
  const generation=sha(canonicalRuntimePayload('th128.html',files));
  return {descriptor:validateRuntimeGeneration({generation,entry:'th128.html',files}),inputs:[...inputs].sort(),payload};
}

export function managedRuntimePlan(options){const {descriptor,inputs}=collect(options);return {descriptor,inputs};}
export function packageManagedRuntime({out,...options}){
  const root=realpathSync(out),{descriptor,payload}=collect(options),directory=join(root,descriptor.generation);
  if([repo,options.coreSite,options.transportSite].some(path=>resolve(path)===root)||existsSync(directory))throw new Error('Use an independent new multiplayer Runtime destination');
  mkdirSync(directory);
  for(const [path,bytes] of payload){
    const dst=join(directory,path);mkdirSync(dirname(dst),{recursive:true});writeFileSync(dst,bytes,{flag:'wx'});
    if(!readFileSync(dst).equals(bytes))throw new Error('Multiplayer packaged bytes differ: '+path);
  }
  const value={schema:'eagler-touhou/runtime-generation/1',...descriptor};
  writeFileSync(join(directory,'runtime-generation.json'),json(value),{flag:'wx'});
  return {directory,descriptor};
}

if(process.argv[1]&&resolve(process.argv[1])===fileURLToPath(import.meta.url)){
  const keys=['normal','transport','coop','output'];
  const {values}=parseArgs({options:Object.fromEntries(keys.map(k=>[k,{type:'string'}]))});
  for(const key of keys)if(!values[key])throw Error('Missing --'+key);
  const {directory,descriptor}=packageManagedRuntime({coreSite:values.normal,transportSite:values.transport,coopSite:values.coop,out:values.output});
  console.log(JSON.stringify({directory,generation:descriptor.generation,files:descriptor.files.length}));
}
