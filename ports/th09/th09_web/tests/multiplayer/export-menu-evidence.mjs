// Keep portable evidence with immutable source, binary, fixture and report
// identities. Historical failed/invalid runs are retained, never blessed anew.
import assert from 'node:assert/strict';
import {readFileSync,writeFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
const root=fileURLToPath(new URL('../../',import.meta.url)),repo=resolve(root,'..');
const out=resolve(root,'artifacts/multiplayer-tests');
const read=path=>JSON.parse(readFileSync(path)),sha=bytes=>createHash('sha256').update(bytes).digest('hex');
const hashFile=path=>sha(readFileSync(path));
const load=label=>{const path=resolve(out,label+'-report.json'),data=read(path);assert.equal(data.passed,true,label);return {label,sha256:hashFile(path),data};};
function build(directory,name){
 const b=read(resolve(root,directory,'build.json'));
 assert.equal(hashFile(resolve(root,directory,name)),b.sha256);
 for(const [path,digest] of Object.entries(b.sources))assert.equal(hashFile(resolve(repo,path)),digest,'Stale source '+path);
 return b;
}
const diagnostic=build('artifacts/sdl-menu-frontier','th09-presentation.wasm');
const release=build('artifacts/sdl-menu-release','th09.wasm');
assert.deepEqual(diagnostic.sources,release.sources);
const releaseModule=new WebAssembly.Module(readFileSync(resolve(root,'artifacts/sdl-menu-release/th09.wasm')));
assert.ok(!WebAssembly.Module.exports(releaseModule).some(e=>e.name.startsWith('th09_probe_')||e.name==='th09_title_open'));
const abba=['menu-long-a1','menu-long-b1','menu-long-b2','menu-long-a2'].map(load);
const summaryPath=resolve(out,'menu-long-a1-summary.json'),summary=read(summaryPath);assert.ok(summary.passed);
assert.deepEqual(summary.runs.map(r=>r.label),abba.map(r=>r.label));
const first=abba[0].data,baseline=read(resolve(root,'artifacts/sdl-smooth-release/build.json'));
assert.equal(hashFile(resolve(root,'artifacts/sdl-smooth-release/th09.wasm')),baseline.sha256);
assert.equal(hashFile(resolve(root,'tests/multiplayer/smoothness-browser.mjs')),first.identity.harness,'Measured runner changed');
assert.equal(hashFile(resolve(root,'tests/multiplayer/audio-output-probe.mjs')),first.identity.audioProbe,'Measured audio probe changed');
assert.equal(hashFile(resolve(root,'sdl-runtime/shared-netplay.mjs')),first.identity.shell,'Measured session shell changed');
for(let i=0;i<abba.length;++i){
 const {label,sha256,data:r}=abba[i];assert.equal(summary.runs[i].sha256,sha256,label);
 assert.equal(r.identity.wasm,i===0||i===3?baseline.sha256:release.sha256,label);
 assert.deepEqual(r.parameters,first.parameters);assert.deepEqual(r.browsers,first.browsers);
 assert.equal(r.parameters.limit,7200);assert.equal(r.parameters.inputMode,'tape');assert.equal(r.parameters.release,true);
 assert.equal(r.parameters.audioOutput,true);assert.equal(r.parameters.profile,false);
 assert.equal(r.parameters.delay,39);assert.equal(r.parameters.jitter,5);assert.equal(r.parameters.slow,2);
 for(const key of ['harness','relay','shell','audioProbe'])assert.equal(r.identity[key],first.identity[key]);
 assert.equal(r.confirmedChecks,60);
 for(let side=0;side<2;++side){
  const x=r.results[side],expected=first.results[side];
  assert.equal(x.renderer,expected.renderer);assert.doesNotMatch(x.renderer,/swiftshader|llvmpipe|microsoft basic render/i);
  assert.equal(x.inputTapeSha256,expected.inputTapeSha256);
  assert.deepEqual(x.confirmedHashes.slice(0,60),expected.confirmedHashes.slice(0,60));
  assert.ok(x.audioOutput.blocks.length>1000&&x.audioOutput.nonzeroBlocks>1000&&!x.audioOutput.overflow);
  assert.ok(r.summary[side].scenes.includes(3),'Results not exercised');
 }
}
const action=load('menu-frontier-action-gate');assert.equal(action.data.wasm,diagnostic.sha256);
assert.equal(action.data.result.cases,11520);assert.equal(action.data.result.passed,1);assert.ok(action.data.result.restored);
const peers=['menu-pause-depth8','menu-result-retry-depth8'].map(load);
for(const {label,data:r} of peers){
 assert.equal(r.wasm,diagnostic.sha256,label);assert.equal(new Set(r.result.hashes).size,1);
 assert.deepEqual(r.result.audited[0],r.result.audited[1]);assert.deepEqual(r.result.audited[0],r.result.audited[2]);
 assert.ok(r.result.framebuffersEqual&&r.result.replayBytes>0);
}
assert.ok(peers[0].data.result.pauseCalls.every(n=>n>20));
assert.ok(peers[0].data.result.costs.every(c=>c.completeDeepCalls>=1500));
assert.equal(peers[1].data.result.frames,7200);assert.ok(peers[1].data.result.phases.every(p=>p.includes(3)));
const worlds=['menu-world-sparse','menu-world-dense','menu-world-round'].map(load);
for(const {label,data:r} of worlds){assert.equal(r.wasm,diagnostic.sha256,label);assert.equal(r.result.checkpoints,270);assert.equal(r.result.bulletBoundary,1);}
const components=['frame-schedule','session','dynamic-state','bullet-snapshot'].map(load);
for(const {label,data:r} of components)for(const inputs of [r.sources,r.headers])
 for(const [path,digest] of Object.entries(inputs))assert.equal(hashFile(path),digest,label+': stale component');
const transports=['menu-release-rtc','menu-release-relay'].map(load);
for(const {label,data:r} of transports){assert.equal(r.wasm,release.sha256,label);assert.ok(r.hardwareRequested);assert.deepEqual(r.result.routes,label.endsWith('relay')?['relay','relay','spectator']:['rtc','rtc','spectator']);}
const uiPath=resolve(root,'artifacts/sdl-menu-frontier/browser/netplay/report.json'),ui=read(uiPath);
assert.ok(ui.passed);assert.equal(ui.wasm,diagnostic.sha256);
for(const name of ['paused','retry','localReplayAfterMatch','reconnected'])assert.ok(ui.states.some(s=>s.name===name),'Missing UI lifecycle '+name);
const failedPath=resolve(out,'endurance-menu-candidate-1-failure.json'),failure=read(failedPath);
assert.match(failure.error,/AssertionError/);
for(const p of failure.partial)assert.deepEqual(p.confirmedHashes.slice(0,60),first.results[0].confirmedHashes.slice(0,60));
const files=read(resolve(root,'build-eagler-multiplayer/runtime-files.json')).files;
assert.equal(files['th09.wasm'].sha256,release.sha256);
for(const [path,entry] of Object.entries(files))assert.equal(hashFile(resolve(root,'build-eagler-multiplayer',path)),entry.sha256);
const compact=records=>records.map(({label,sha256,data:r})=>({label,sha256,...r,result:{...r.result,samples:undefined}}));
const evidence={schema:'th09/menu-smoothness-evidence/1',baseCommit:'ef772e0',
 scope:'Local desktop PC, loopback RTC sender impairment, native GPU and P2 CPU throttling. Not public Internet/TURN, physical scanout, phone or acoustic/human acceptance. No push/deployment.',
 builds:{baseline:{wasm:baseline.sha256},diagnostic:{wasm:diagnostic.sha256},release:{wasm:release.sha256,developmentExports:false}},sources:release.sources,
 harnesses:Object.fromEntries(['smoothness-browser.mjs','audio-output-probe.mjs','summarize-endurance.mjs','menu-barrier-browser.mjs','peer-browser.mjs','export-menu-evidence.mjs'].map(p=>[p,hashFile(resolve(root,'tests/multiplayer',p))])),
 performance:{sha256:hashFile(summaryPath),...summary},action:compact([action])[0],peers:compact(peers),worlds:compact(worlds),transports:compact(transports),
 components:components.map(({label,sha256})=>({label,sha256})),
 ui:{sha256:hashFile(uiPath),harnessSha256:hashFile(resolve(root,'tests/browser/netplay-check.mjs')),...ui},
 retainedInvalidRun:{label:'endurance-menu-candidate-1',sha256:hashFile(failedPath),error:failure.error,partialFrontiers:failure.partial.map(p=>p.stats),
  reason:'Invalid same-speculative-frame endpoint assertion. All 60 confirmed checks agree with peers and baseline. Replaced by fully-published endpoint plus complete confirmed-trace gates; this failed run is not counted in the passing A/B/B/A.'},
 runtime:{directory:'th09_web/build-eagler-multiplayer',version:read(resolve(root,'build-eagler-multiplayer/version.json')),files}};
const destination=resolve(repo,'docs/multiplayer/rollback-menu-evidence.json');
writeFileSync(destination,JSON.stringify(evidence,null,2)+'\n');console.log(JSON.stringify({passed:true,destination,diagnostic:diagnostic.sha256,release:release.sha256,pairedRuns:abba.length}));
