// Assemble a runnable web release from authored C++ and original local assets.
import {readFileSync,writeFileSync,readdirSync,mkdirSync,copyFileSync,cpSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
import {execFileSync} from 'node:child_process';
const root=fileURLToPath(new URL('../',import.meta.url)),workspace=resolve(root,'..'),out=resolve(root,process.env.TH09_OUTPUT||'artifacts/sdl-release'),site=resolve(out,'site'),sha=b=>createHash('sha256').update(b).digest('hex');
const wasm=readFileSync(resolve(out,'th09.wasm')),loader=readFileSync(resolve(out,'th09.mjs')),build=JSON.parse(readFileSync(resolve(out,'build.json'),'utf8'));
if(build.sha256!==sha(wasm)||build.exports.some(e=>e.name.startsWith('th09_probe_')||e.name==='th09_title_open'))throw Error('Build release C++ first');
const runtime='/runtime/'+sha(Buffer.concat([wasm,loader])).slice(0,20),files={},resources=[];
function put(url,bytes,immutable=false){const name=url.slice(1);if(!name||name.includes('..'))throw Error('Invalid package path');const target=resolve(site,name);mkdirSync(resolve(target,'..'),{recursive:true});writeFileSync(target,bytes);files[url]={path:name,bytes:bytes.length,sha256:sha(bytes),immutable};}
function file(url,path,immutable=false){put(url,readFileSync(path),immutable);}
function resource(url,mount,path){file(url,path);resources.push({url,path:mount,bytes:files[url].bytes,sha256:files[url].sha256});}
put(runtime+'/th09.wasm',wasm,true);put(runtime+'/th09.mjs',loader,true);
for(const name of ['th09.html','style.css','keyboard.mjs','netplay.mjs','motion-replay.mjs'])file('/app/'+name,resolve(root,'sdl-runtime',name));
let shell=readFileSync(resolve(root,'sdl-runtime/shell.mjs'),'utf8').replace("from '/probe.mjs'",`from '${runtime}/th09.mjs'`);
shell+="\n// Replace a previous game's root worker without deleting its saves or caches.\nif('serviceWorker' in navigator)navigator.serviceWorker.register('/app-shell-sw.js',{scope:'/',updateViaCache:'none'}).then(r=>r.update()).catch(console.warn);\n";put('/app/shell.mjs',Buffer.from(shell));
files['/']={...files['/app/th09.html']};files['/index.html']={...files['/app/th09.html']};
resource('/th09.dat','/th09.dat',resolve(workspace,'[th09] 东方花映塚 (日文版)/th09.dat'));
for(const name of ['cp932.bin','blend.bin','msgothic.ttc'])resource('/fonts/'+name,'/fonts/'+name,resolve(root,'assets/sdl-native',name));
for(const name of readdirSync(resolve(root,'assets/sdl-native/music')).filter(n=>/^[a-z0-9_]+\.ogg$/.test(n)).sort())resource('/music/'+name,'/music/'+name,resolve(root,'assets/sdl-native/music',name));
put('/resources.json',Buffer.from(JSON.stringify(resources)));
file('/THIRD-PARTY-NOTICES.txt',resolve(root,'THIRD-PARTY-NOTICES.txt'));for(const name of readdirSync(resolve(root,'licenses')).sort())file('/licenses/'+name,resolve(root,'licenses',name));
put('/app-shell-sw.js',Buffer.from("// TH09 migration: network fetches, leave other game storage intact.\nself.addEventListener('install',event=>event.waitUntil(self.skipWaiting()));\nself.addEventListener('activate',event=>event.waitUntil(self.clients.claim()));\nself.addEventListener('fetch',()=>{});\n"));
const version=sha(Buffer.from(JSON.stringify(files))).slice(0,24);put('/version.json',Buffer.from(JSON.stringify({game:'th09',build:version})));
const manifest={game:'th09',version,sourceVersion:'Japanese 1.50a',execution:{kind:'cpp-sdl3',entry:'/app/th09.html',wasm:runtime+'/th09.wasm',loader:runtime+'/th09.mjs',sha256:sha(wasm),loaderSha256:sha(loader)},downloadBytes:resources.reduce((n,f)=>n+f.bytes,0)+wasm.length+loader.length,files};
writeFileSync(resolve(site,'manifest.json'),JSON.stringify(manifest,null,2)+'\n');mkdirSync(resolve(out,'scripts'),{recursive:true});copyFileSync(resolve(root,'scripts/release-server.mjs'),resolve(out,'scripts/serve.mjs'));copyFileSync(resolve(root,'scripts/netplay-relay.mjs'),resolve(out,'scripts/netplay-relay.mjs'));cpSync(resolve(root,'node_modules/ws'),resolve(out,'node_modules/ws'),{recursive:true});
writeFileSync(resolve(out,'package.json'),JSON.stringify({name:'th09-native-web',private:true,type:'module',scripts:{start:'node scripts/serve.mjs --port 3007'},dependencies:{ws:'8.21.3'}},null,2)+'\n');
copyFileSync(resolve(root,'README.md'),resolve(out,'README.md'));copyFileSync(resolve(root,'启动花映塚网页版.cmd'),resolve(out,'启动花映塚网页版.cmd'));
console.log(JSON.stringify({site,version,downloadBytes:manifest.downloadBytes,resources:resources.length,publicFiles:Object.keys(files).length,wasm:build.sha256}));
// A release always includes the shared launcher, including imports, touch and
// room controls. The standalone development shell is not the public entry.
execFileSync(process.execPath,[resolve(root,'scripts/package-launcher.mjs')],{env:{...process.env,TH09_OUTPUT:out},stdio:'inherit',windowsHide:true});
