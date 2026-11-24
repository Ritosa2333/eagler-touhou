// Compiles the author's full route selector with a TH128-only, receiver-side RTC seat tag.
import {createHash} from 'node:crypto';import {spawnSync} from 'node:child_process';import {join,resolve,dirname,relative,isAbsolute} from 'node:path';
import {existsSync,readFileSync,realpathSync,mkdirSync,mkdtempSync,copyFileSync,writeFileSync} from 'node:fs';import {fileURLToPath} from 'node:url';
const mp=resolve(import.meta.dirname,'..'),repo=resolve(mp,'../..'),unit='common/src/netplay/BrowserPeerTransport.cpp';
const sources=[unit,'common/include/eagler/netplay/BrowserPeerTransport.hpp','common/include/eagler/netplay/PeerTransport.hpp','ports/th128-mp/transport/include/netplay/NetplayTransportConfig.hpp','ports/th128-mp/transport/peer.cpp','ports/th128-mp/transport/coop-rtc-sender.patch'];
const api='malloc free th128_mp_open th128_mp_open_coop th128_mp_close th128_mp_state th128_mp_mode th128_mp_error th128_mp_buffered th128_mp_send th128_mp_poll th128_mp_spectator_begin th128_mp_spectator_send'.split(' ');
const flags=['--no-entry','-std=c++20','-O2','-DTH128_COOP_PEER=1','-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=web','-sFILESYSTEM=0','-sINITIAL_MEMORY=16777216','-sSTACK_SIZE=524288','-sEXPORTED_FUNCTIONS='+JSON.stringify(api.map(n=>'_'+n)),'-sEXPORTED_RUNTIME_METHODS=["HEAPU8","UTF8ToString"]'];
const sha=x=>createHash('sha256').update(x).digest('hex'),pin=data=>({bytes:data.length,sha256:sha(data)});
const names=['author-peer.mjs','author-peer.wasm'];const id=(input,artifacts)=>sha(JSON.stringify([input,...names.map(n=>[n,artifacts[n].bytes,artifacts[n].sha256])]));
function inputs(){return sha(JSON.stringify([flags,...sources.map(p=>[p,sha(readFileSync(join(repo,p)))])]));}
export function readCoopPeerBuild(dir){const receipt=JSON.parse(readFileSync(join(dir,'coop-peer-build.json'),'utf8'));
 if(receipt.schema!=='th128-coop-peer/2'||receipt.compiler!=='4.0.22'||receipt.inputs!==inputs())throw Error('Cooperative transport does not match its source inventory');
 const artifacts=Object.fromEntries(names.map(n=>[n,pin(readFileSync(join(dir,n)))]));
 if(names.some(n=>JSON.stringify(artifacts[n])!==JSON.stringify(receipt.artifacts[n]))||receipt.generation!==id(receipt.inputs,artifacts))throw Error('Cooperative transport artifact checksum differs');return receipt;
}
function run(exe,args,cwd){const ret=spawnSync(exe,args,{cwd,encoding:'utf8',windowsHide:true,timeout:120000});if(ret.status!==0)throw Error([ret.error,ret.stdout,ret.stderr].filter(Boolean).join('\n'));return ret.stdout;}
export function buildCoopPeer(out){out=resolve(out);const python=process.env.EMSDK_PYTHON,emcc=process.env.TH128_EMCC;if(!python||!emcc||!run(python,[emcc,'--version'],repo).includes('4.0.22'))throw Error('Emscripten 4.0.22 is required');
 const canonical=join(realpathSync(dirname(out)),out.slice(dirname(out).length+1));const rel=relative(repo,canonical);if(!isAbsolute(rel)&&(!rel.startsWith('..\\')&&!rel.startsWith('../')))throw Error('Use a private output outside the source tree');
 mkdirSync(out,{recursive:true});const stage=mkdtempSync(join(out,'compile-'));mkdirSync(dirname(join(stage,unit)),{recursive:true});const input=inputs();copyFileSync(join(repo,unit),join(stage,unit));
 run('git',['-c','core.autocrlf=false','apply','--ignore-space-change','--whitespace=nowarn',join(mp,'transport/coop-rtc-sender.patch')],stage);
 run(python,[emcc,join(stage,unit),join(mp,'transport/peer.cpp'),'-I'+join(repo,'common/include'),'-I'+join(mp,'transport/include'),...flags,'-o',join(stage,names[0])],stage);
 if(inputs()!==input)throw Error('Transport sources changed during compilation');const artifacts=Object.fromEntries(names.map(n=>[n,pin(readFileSync(join(stage,n)))]));
 const generation=id(input,artifacts),dst=join(out,generation);if(!existsSync(dst)){mkdirSync(dst);for(const n of names)copyFileSync(join(stage,n),join(dst,n));writeFileSync(join(dst,'coop-peer-build.json'),JSON.stringify({schema:'th128-coop-peer/2',compiler:'4.0.22',inputs:input,generation,artifacts},null,2)+'\n',{flag:'wx'});}return {dst,...readCoopPeerBuild(dst)};
}
if(process.argv[1]&&resolve(process.argv[1])===fileURLToPath(import.meta.url)){if(!process.argv[2])throw Error('Missing private output directory');console.log(JSON.stringify(buildCoopPeer(process.argv[2])));}
