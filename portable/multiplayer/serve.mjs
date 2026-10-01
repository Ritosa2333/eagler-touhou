import {readFileSync,existsSync} from 'node:fs';
import {resolve} from 'node:path';
import {createPresentationLabServer,sha256} from '../../third_party/eagler-common/testkit/presentation-lab/server-core.mjs';
const root=resolve(import.meta.dirname,'../..');
const workspace=resolve(process.env.EAGLER_WORKSPACE||root+'/../..');
const profile=process.env.TH10_MP_PROFILE||'multiplayer';
if(!['multiplayer','multiplayer-fixtures'].includes(profile))throw Error('Invalid MP test profile');
const buildRoot=resolve(process.env.TH10_MP_BUILD_ROOT||resolve(root,'th10_web/artifacts',profile));
const fixtureRoot=resolve(process.env.TH10_MP_FIXTURE_ROOT||import.meta.dirname);
const build=JSON.parse(readFileSync(resolve(buildRoot,'build.json')));
if(build.variant!=='multiplayer')throw Error('Build --multiplayer first');
const wasm=resolve(buildRoot,'th10-sdl.wasm');
if(sha256(readFileSync(wasm))!==build.sha256)throw Error('Stale multiplayer WASM');
const files=new Map([
 ['/','smoke.html'],['/smoke.mjs','smoke.mjs'],['/performance-driver.mjs','performance-driver.mjs'],['/replay-robot.mjs','replay-robot.mjs'],
 ['/replay-audio-seek.mjs','replay-audio-seek.mjs'],
].map(([url,name])=>[url,resolve(fixtureRoot,name)]));
files.set('/th10-sdl.mjs',resolve(buildRoot,'th10-sdl.mjs'));
files.set('/th10-sdl.wasm',wasm);
files.set('/input/th10.dat',process.env.TH10_MP_DATA||resolve(workspace,'games/web-content/th10/th10.data'));
files.set('/fonts/msgothic.ttc',process.env.TH10_MP_FONT||resolve(workspace,'th06-eagler/assets/msgothic.ttc'));
for(const name of ['blend.bin','codepages.bin'])files.set('/fonts/'+name,resolve(workspace,'th10-eagler/build-eagler/fonts',name));
for(const path of files.values())if(!existsSync(path))throw Error('Missing smoke resource: '+path);
const port=Number(process.env.PORT||8138);
// /build.json is the common server's reserved identity endpoint; pass the
// source identity there rather than shadowing it with a file-map entry.
const {server,start}=createPresentationLabServer({port,files,identity:{game:'th10',variant:'multiplayer',
 profile,sha256:build.sha256,sourceDigest:build.sourceDigest,wasm:build.sha256,
 scope:'local native gameplay smoke, no transport acceptance'}});
server.on('listening',()=>{
 const actual=server.address().port;
 console.log('TH10 multiplayer smoke http://127.0.0.1:'+actual);
 if(process.send)process.send({type:'ready',port:actual,profile,sha256:build.sha256});
});
start();
