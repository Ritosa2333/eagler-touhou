import {readFileSync,existsSync} from 'node:fs';
import {resolve} from 'node:path';
import {createPresentationLabServer,sha256} from '../../third_party/eagler-common/testkit/presentation-lab/server-core.mjs';

const root=resolve(import.meta.dirname,'../..');
const workspace=resolve(process.env.EAGLER_WORKSPACE||root+'/../..');
const buildRoot=resolve(root,'th08_web/artifacts/multiplayer');
const build=JSON.parse(readFileSync(resolve(buildRoot,'build.json')));
if(build.variant!=='multiplayer')throw Error('Build --th08 --multiplayer first');
const wasm=resolve(buildRoot,'th08-sdl.wasm');
if(sha256(readFileSync(wasm))!==build.sha256)throw Error('Stale multiplayer WASM');
const files=new Map([
  ['/','smoke.html'],['/smoke.mjs','smoke.mjs'],
].map(([url,name])=>[url,resolve(import.meta.dirname,name)]));
files.set('/th08-sdl.mjs',resolve(buildRoot,'th08-sdl.mjs'));
files.set('/th08-sdl.wasm',wasm);
files.set('/input/th08.dat',process.env.TH08_MP_DATA||resolve(workspace,'games/web-content/th08/th08.dat'));
files.set('/fonts/msgothic.ttc',process.env.TH08_MP_FONT||resolve(workspace,'th06-eagler/assets/msgothic.ttc'));
for(const name of ['blend.bin','cp932.bin'])files.set('/fonts/'+name,resolve(workspace,'th08-eagler/build-eagler/fonts',name));
for(const path of files.values())if(!existsSync(path))throw Error('Missing smoke resource: '+path);
const port=Number(process.env.PORT||8139);
const {server,start}=createPresentationLabServer({port,files,identity:{game:'th08',variant:'multiplayer',wasm:build.sha256,scope:'local native HUD smoke, no transport acceptance'}});
server.on('listening',()=>console.log('TH08 multiplayer smoke http://127.0.0.1:'+port));
start();
