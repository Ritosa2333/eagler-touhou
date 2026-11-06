// Freeze code and the CP932 map. Retail DATA, music and shared fonts stay in Package Store.
import {readFileSync,writeFileSync,mkdirSync,existsSync,readdirSync} from 'node:fs';
import {resolve,dirname,relative} from 'node:path';
import {createHash} from 'node:crypto';
const root=resolve(import.meta.dirname,'..'),game='th17';
const multiplayer=true;
const roomBenchmark=process.argv.includes('--room-benchmark');
if(roomBenchmark&&(!multiplayer||!process.env.EAGLER_RUNTIME_OUTPUT))throw Error('Room benchmark needs a dedicated multiplayer package output');
const out=process.env.EAGLER_RUNTIME_OUTPUT?resolve(process.env.EAGLER_RUNTIME_OUTPUT):resolve(root,multiplayer?'build-eagler-multiplayer':'build-eagler');
const buildRoot=resolve(root,'web',process.env.TH17_OUTPUT||(multiplayer?'artifacts/sdl-multiplayer':'artifacts/sdl-release'));
const hash=b=>createHash('sha256').update(b).digest('hex');
const build=JSON.parse(readFileSync(resolve(buildRoot,'build.json'),'utf8'));
if(!Number.isFinite(Date.parse(build.builtAt)))throw Error('Rebuild Runtime with build timestamp');
if(build.kind!==(roomBenchmark?'th17-room-benchmark':'th17-sdl3-application-release')||!build.completeGame)throw Error('TH17 runtime build kind mismatch');
if((build.features?.multiplayer===true)!==multiplayer)throw Error('TH17 Runtime variant mismatch');
for(const [path,expected] of Object.entries({...build.sources,...build.headers})){
 if(hash(readFileSync(resolve(root,path)))!==expected)throw Error('Rebuild changed source: '+path);
}
const wasm=readFileSync(resolve(buildRoot,'th17.wasm'));
if(hash(wasm)!==build.sha256)throw Error('Wasm build identity mismatch');
for(const {name} of WebAssembly.Module.exports(new WebAssembly.Module(wasm)))
 if(/^(?:application_|th17_probe|audit_|presentation_lab_)/.test(name)||(!roomBenchmark&&name.startsWith('th17_room_benchmark_')))throw Error('Diagnostic export in production: '+name);
const mappingPath='fonts/cp932.bin';
const browserHelpers=['local-replay.mjs','local-replay-native.mjs','spectator-playback.mjs'];
const names=['th17.html','manifest.json','gpu-timing.mjs','shell.mjs','managed.css','keyboard.mjs','directory-keyboard.mjs','th17.mjs','th17.wasm','resources.json',mappingPath,...browserHelpers];
const allowed=new Set([...names,'runtime-files.json']);
function walk(dir){return existsSync(dir)?readdirSync(dir,{withFileTypes:true}).flatMap(e=>e.isDirectory()?walk(resolve(dir,e.name)):[resolve(dir,e.name)]):[];}
for(const file of walk(out))if(!allowed.has(relative(out,file).replaceAll('\\','/')))throw Error('Unexpected Runtime file: '+file);
function put(name,bytes){const file=resolve(out,name);mkdirSync(dirname(file),{recursive:true});writeFileSync(file,bytes);}
const shellRoot=resolve(root,'web/sdl-runtime');
put('th17.html',readFileSync(resolve(shellRoot,'managed.html'),'utf8').replace('<head>','<head><meta name="eagler-data-provider" content="retail-memory">').replace('src="managed.mjs"','src="shell.mjs"'));
const version='1.0.14-eagler-'+build.sha256.slice(0,12);
const marker="/*TH17_BUILD_INFO*/{version:'development-incomplete',completeGame:false}";
const shell=readFileSync(resolve(shellRoot,'shell.mjs'),'utf8');
if(!shell.includes(marker))throw Error('Missing build-info marker');
put('shell.mjs',shell.replace(marker,JSON.stringify({version,completeGame:true})));
for(const name of ['gpu-timing.mjs','managed.css','keyboard.mjs','directory-keyboard.mjs'])put(name,readFileSync(resolve(shellRoot,name)));
for(const name of browserHelpers)put(name,readFileSync(resolve(root,'../../common/browser',name)));
const loader=readFileSync(resolve(buildRoot,'th17.mjs'));
put('th17.mjs',loader);put('th17.wasm',wasm);
const decoder=new TextDecoder('shift_jis'),mapping=Buffer.alloc(131072);
for(let code=0;code<65536;++code){
 const bytes=code<256?Uint8Array.of(code):Uint8Array.of(code>>8,code&255);
 const value=decoder.decode(bytes);
 mapping.writeUInt16LE(value.length===1&&value!=='�'?value.charCodeAt(0):0x30fb,code*2);
}
put(mappingPath,mapping);
const resources=[{path:'/'+mappingPath,url:'./'+mappingPath,bytes:mapping.length}];
put('resources.json',JSON.stringify({schema:'eagler-sdl-resources/1',game,resources},null,2)+'\n');
const features={thprac:build.features?.thprac===true,languages:build.features?.languages===true,focusHitbox:false,...(multiplayer?{multiplayer:true}:{}),...(roomBenchmark?{roomBenchmark:true}:{})};
put('manifest.json',JSON.stringify({game,protocol:'eagler-touhou/1',adapter:'sdl3-eagler',profile:roomBenchmark?'room-benchmark':multiplayer?'multiplayer':'production',...(multiplayer?{product:'th17mp',variant:'multiplayer'}:{}),builtAt:build.builtAt,version,features,music:['ogg-stream','ogg-full','none'],execution:{kind:build.kind,sha256:build.sha256,loaderSha256:hash(loader),architecture:build.architecture}},null,2)+'\n');
const files=Object.fromEntries(names.map(name=>{const bytes=readFileSync(resolve(out,name));return [name,{bytes:bytes.length,sha256:hash(bytes)}];}));
put('runtime-files.json',JSON.stringify({schema:'eagler-touhou/runtime-directory/1',game,files},null,2)+'\n');
console.log(JSON.stringify({out,version,files:names.length,wasm:build.sha256}));
