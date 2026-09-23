import {readFileSync,existsSync} from 'node:fs';
import {resolve} from 'node:path';
import {createPresentationLabServer,sha256} from '../../third_party/eagler-common/testkit/presentation-lab/server-core.mjs';
const root=resolve(import.meta.dirname,'../..');
const workspace=resolve(process.env.EAGLER_WORKSPACE||root+'/../..');
const buildRoot=resolve(root,'th10_web/artifacts/multiplayer');
const build=JSON.parse(readFileSync(resolve(buildRoot,'build.json')));
if(build.variant!=='multiplayer')throw Error('Build --multiplayer first');
const wasm=resolve(buildRoot,'th10-sdl.wasm');
if(sha256(readFileSync(wasm))!==build.sha256)throw Error('Stale multiplayer WASM');
const files=new Map([
 ['/','smoke.html'],['/smoke.mjs','smoke.mjs'],
].map(([url,name])=>[url,resolve(import.meta.dirname,name)]));
files.set('/th10-sdl.mjs',resolve(buildRoot,'th10-sdl.mjs'));
files.set('/th10-sdl.wasm',wasm);
files.set('/input/th10.dat',process.env.TH10_MP_DATA||resolve(workspace,'games/web-content/th10/th10.data'));
files.set('/fonts/msgothic.ttc',process.env.TH10_MP_FONT||resolve(workspace,'th06-eagler/assets/msgothic.ttc'));
for(const name of ['blend.bin','codepages.bin'])files.set('/fonts/'+name,resolve(workspace,'th10-eagler/build-eagler/fonts',name));
for(const path of files.values())if(!existsSync(path))throw Error('Missing smoke resource: '+path);
const port=Number(process.env.PORT||8138);
const {server,start}=createPresentationLabServer({port,files,identity:{game:'th10',variant:'multiplayer',wasm:build.sha256,scope:'local native gameplay smoke, no transport acceptance'}});
server.on('listening',()=>console.log('TH10 multiplayer smoke http://127.0.0.1:'+port));
start();
