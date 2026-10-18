// Validate and retain compact, portable evidence. Raw traces stay in artifacts;
// every report hash and all current build-source hashes are kept in the record.
import assert from 'node:assert/strict';
import {readFileSync,writeFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
const root=fileURLToPath(new URL('../../',import.meta.url)),workspace=resolve(root,'..');
const out=resolve(root,'artifacts/multiplayer-tests');
const sha=b=>createHash('sha256').update(b).digest('hex');
const json=path=>JSON.parse(readFileSync(path));
function build(directory,name){
 const b=json(resolve(root,directory,'build.json'));
 assert.equal(sha(readFileSync(resolve(root,directory,name))),b.sha256);
 for(const [path,digest] of Object.entries(b.sources))assert.equal(sha(readFileSync(resolve(workspace,path))),digest,`Build is stale: ${path}`);
 return b;
}
const diagnostic=build('artifacts/sdl-smooth-retry','th09-presentation.wasm');
const release=build('artifacts/sdl-smooth-release','th09.wasm');
assert.deepEqual(diagnostic.sources,release.sources,'Release is not built from the validated diagnostic sources');
assert.ok(!release.exports.some(e=>e.name.startsWith('th09_probe_')||e.name==='th09_title_open'));
const load=label=>{const path=resolve(out,label+'-report.json'),r=json(path);assert.ok(r.passed,label);return {label,sha256:sha(readFileSync(path)),data:r};};
const comparisonPath=resolve(out,'retry-tape-comparison.json'),comparison=json(comparisonPath);
assert.ok(comparison.passed);assert.equal(comparison.candidate,diagnostic.sha256);
const abba=comparison.sources.map(source=>{const r=load(source.label);assert.equal(r.sha256,source.sha256);return r;});
const smoke=['retry-rtc39-1','retry-rtc39-2'].map(load);
for(const {label,data:r} of smoke){
 assert.equal(r.identity.wasm,diagnostic.sha256,label);assert.equal(r.parameters.delay,39);assert.equal(r.parameters.jitter,5);
 assert.equal(r.parameters.slow,2);assert.equal(r.parameters.inputMode,'tape');assert.equal(r.parameters.profile,false);assert.ok(r.sameFrame);
 for(let side=0;side<2;++side){
  assert.deepEqual(r.results[side].audited,abba[0].data.results[side].audited);
  assert.equal(r.results[side].inputTapeSha256,comparison.inputTapeSha256[side]);
 }
}
const world=['retry-world-sparse','retry-world-dense','retry-world-round','retry-world-eiki'].map(load);
for(const {label,data:r} of world){assert.equal(r.wasm,diagnostic.sha256,label);assert.equal(r.result.bulletBoundary,1);assert.equal(r.result.checkpoints,270);}
const peer=['retry-peer-faults','retry-depth4','retry-depth8'].map(load);
for(const {label,data:r} of peer){
 assert.equal(r.wasm,diagnostic.sha256,label);assert.ok(r.result.framebuffersEqual);assert.ok(r.result.replayBytes>0);
 assert.equal(new Set(r.result.hashes).size,1);assert.deepEqual(r.result.audited[0],r.result.audited[1]);assert.deepEqual(r.result.audited[0],r.result.audited[2]);
 if(r.parameters.forceCorrection){
  assert.equal(r.parameters.limit,1800);assert.equal(r.parameters.inputTrace,'charged-strafe-alternating-focus');
  assert.ok(r.result.peakBullets.every(n=>n>=300));
  assert.ok(r.result.costs.every(c=>c.completeDeepCalls>=1500));
 }
}
const transports=['retry-transport-rtc','retry-transport-relay','retry-transport-release','retry-transport-release-relay'].map(load);
for(const {label,data:r} of transports){
 assert.equal(r.wasm,label.includes('release')?release.sha256:diagnostic.sha256,label);assert.ok(r.hardwareRequested);
 assert.deepEqual(r.result.routes,label.endsWith('relay')?['relay','relay','spectator']:['rtc','rtc','spectator']);
 if(!label.includes('release')){assert.ok(r.result.comparableFinalHashes);assert.equal(new Set(r.result.hashes).size,1);}
}
const components=['frame-schedule','session','dynamic-state','bullet-snapshot'].map(load);
for(const {label,data:r} of components)for(const collection of [r.sources,r.headers])for(const [path,digest] of Object.entries(collection))assert.equal(sha(readFileSync(path)),digest,`${label}: stale component`);
const runtime=resolve(root,'build-eagler-multiplayer'),files=json(resolve(runtime,'runtime-files.json')).files;
assert.equal(files['th09.wasm'].sha256,release.sha256);
for(const [path,entry] of Object.entries(files))assert.equal(sha(readFileSync(resolve(runtime,path))),entry.sha256);
const small=records=>records.map(({label,sha256,data:r})=>({label,sha256,wasm:r.wasm,scope:r.scope,parameters:r.parameters,
 result:r.result?{...r.result,samples:undefined}:undefined}));
const runtimeSources=['scripts/build-sdl.mjs','scripts/build-eagler.mjs','tests/multiplayer/smoothness-browser.mjs','tests/multiplayer/peer-browser.mjs','tests/multiplayer/world-browser.mjs','tests/multiplayer/transport-browser.mjs','tests/multiplayer/frame-schedule.cpp','tests/multiplayer/bullet-snapshot.cpp'];
const evidence={schema:'th09/rollback-smoothness-evidence/1',baseCommit:'bf60dbc',scope:'Local native-GPU desktop validation. Not physical scanout, remote Internet/TURN, phone, user dodging or acoustic acceptance. No push or deployment.',
 builds:{diagnostic:{wasm:diagnostic.sha256,bytes:diagnostic.bytes},release:{wasm:release.sha256,bytes:release.bytes,developmentExports:false}},sources:diagnostic.sources,
 harnesses:Object.fromEntries(runtimeSources.map(path=>[path,sha(readFileSync(resolve(root,path)))])),
 rtcComparison:{sha256:sha(readFileSync(comparisonPath)),...comparison},
 rtcNormal:smoke.map(({label,sha256,data:r})=>({label,sha256,identity:r.identity,parameters:r.parameters,summary:r.summary,hashes:r.results.map(e=>e.hash)})),
 world:small(world),peer:small(peer),transports:small(transports),components:components.map(({label,sha256})=>({label,sha256})),
 runtime:{localDirectory:'th09_web/build-eagler-multiplayer',files,version:json(resolve(runtime,'version.json'))}};
const destination=resolve(workspace,'docs/multiplayer/rollback-smoothness-evidence.json');
writeFileSync(destination,JSON.stringify(evidence,null,2)+'\n');
console.log(JSON.stringify({passed:true,destination,diagnostic:diagnostic.sha256,release:release.sha256,worldGates:world.length,peerGates:peer.length,transportGates:transports.length}));
