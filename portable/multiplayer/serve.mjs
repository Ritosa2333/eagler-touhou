import {readFileSync,writeFileSync,existsSync} from 'node:fs';
import {resolve} from 'node:path';
import {createPresentationLabServer,sha256} from '../../third_party/eagler-common/testkit/presentation-lab/server-core.mjs';
import {createRuntimeManifest} from '../package-eagler.mjs';

const root=resolve(import.meta.dirname,'../..');
const workspace=resolve(process.env.EAGLER_WORKSPACE||root+'/../..');
const fixture=process.env.TH08_MP_FIXTURES==='1';
const buildRoot=resolve(root,'th08_web/artifacts',fixture?'multiplayer-fixtures':'multiplayer');
const build=JSON.parse(readFileSync(resolve(buildRoot,'build.json')));
if(build.variant!==(fixture?'multiplayer-fixtures':'multiplayer'))throw Error('Build the requested TH08 multiplayer variant first');
const wasm=resolve(buildRoot,'th08-sdl.wasm');
if(sha256(readFileSync(wasm))!==build.sha256)throw Error('Stale multiplayer WASM');
const files=new Map([
  ['/','smoke.html'],['/smoke.mjs','smoke.mjs'],
].map(([url,name])=>[url,resolve(import.meta.dirname,name)]));
files.set('/th08-sdl.mjs',resolve(buildRoot,'th08-sdl.mjs'));
files.set('/th08-sdl.wasm',wasm);
const data=process.env.TH08_MP_DATA||resolve(workspace,'games/web-content/th08/th08.dat');
if(!existsSync(data))throw Error('Set TH08_MP_DATA to your retail th08.dat; diagnostic tests never bundle retail DATA');
files.set('/input/th08.dat',data);
files.set('/host.html',resolve(import.meta.dirname,'runtime-host.html'));
// The production shell binds admission to this build identity. Keep the local
// host lane on the same manifest contract as a packaged Runtime.
const hostManifest=resolve(buildRoot,'host-runtime-manifest.json');
writeFileSync(hostManifest,JSON.stringify(createRuntimeManifest({game:'th08',build,presentationLab:false,multiplayer:true})));
files.set('/runtime/manifest.json',hostManifest);
files.set('/runtime/resources.json',resolve(import.meta.dirname,'host-resources.json'));
files.set('/runtime/th08-sdl.mjs',resolve(buildRoot,'th08-sdl.mjs'));files.set('/runtime/th08-sdl.wasm',wasm);
for(const file of ['th08.html','shell.mjs','eagler-host.mjs','save-storage.mjs','multiplayer-host.mjs','practice.mjs','practice-config.mjs','practice-sections.mjs','midi-worklet.mjs'])
 files.set('/runtime/'+file,resolve(root,'th08_web/sdl-runtime',file));
files.set('/runtime/motion-replay.mjs',resolve(root,'portable/browser/motion-replay.mjs'));
files.set('/fonts/msgothic.ttc',process.env.TH08_MP_FONT||resolve(workspace,'th06-eagler/assets/msgothic.ttc'));
for(const name of ['blend.bin','cp932.bin'])files.set('/fonts/'+name,resolve(workspace,'th08-eagler/build-eagler/fonts',name));
for(const path of files.values())if(!existsSync(path))throw Error('Missing smoke resource: '+path);
const port=Number(process.env.PORT||(fixture?8140:8139));
const {server,start}=createPresentationLabServer({port,files,identity:{game:'th08',variant:build.variant,wasm:build.sha256,scope:fixture?'diagnostic native cooperation fixtures, no transport acceptance':'local native HUD smoke, no transport acceptance'}});
server.on('listening',()=>{
 console.log('TH08 multiplayer smoke http://127.0.0.1:'+port);
 process.send?.({type:'ready',port:server.address().port});
});
start();
