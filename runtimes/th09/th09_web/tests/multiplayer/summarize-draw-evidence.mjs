import assert from 'node:assert/strict';
import {readFileSync,writeFileSync,statSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
const root=fileURLToPath(new URL('../../',import.meta.url)),repo=resolve(root,'..');
const out=resolve(root,'artifacts/multiplayer-tests');
const read=p=>JSON.parse(readFileSync(p));
const sha=b=>createHash('sha256').update(b).digest('hex');
const report=name=>{const r=read(resolve(out,name+'-report.json'));assert.equal(r.passed,true,name);return r;};
const development=read(resolve(root,'artifacts/sdl3/build.json')),release=read(resolve(root,'artifacts/sdl-release/build.json'));
for(const [build,path] of [[development,'artifacts/sdl3/th09-presentation.wasm'],[release,'artifacts/sdl-release/th09.wasm']]){
 assert.equal(sha(readFileSync(resolve(root,path))),build.sha256);
 for(const [path,expected] of Object.entries(build.sources))assert.equal(sha(readFileSync(resolve(repo,path))),expected,'stale source '+path);
}
const diagnosis=report('pc-phase-baseline');
const diagnosticBaseline=read(resolve(root,'artifacts/pc-phase-baseline/build.json'));
assert.equal(diagnosis.wasm,diagnosticBaseline.sha256);
const names=['pc-draw-paired-a','pc-draw-paired-b','pc-draw-characters-2-4','pc-draw-characters-10-12'];
const paired=names.map(name=>({name,...report(name)}));
const comparison=paired.map(r=>{
 assert.equal(r.wasm,development.sha256);assert.equal(r.result.checkpoints,200);assert.equal(r.result.burst,8);
 assert.equal(r.result.fullStateEqual,true);assert.equal(r.result.framebuffersEqual,true);
 assert.equal(r.result.renderer,diagnosis.result.renderer);assert.doesNotMatch(r.result.renderer,/swiftshader|llvmpipe|microsoft basic render/i);
 const s=r.result.summary,fixed=r.result.capture.total+r.result.restore.total;
 assert.ok(s.optimized.draw.total<s.reference.draw.total,r.name+' Draw did not improve');
 assert.ok(s.optimized.total.total<s.reference.total.total,r.name+' replay did not improve');
 return {name:r.name,drawReductionPercent:100*(1-s.optimized.draw.total/s.reference.draw.total),replayReductionPercent:100*(1-s.optimized.total.total/s.reference.total.total),phaseAccountedCorrection:{referenceMs:s.reference.total.total+fixed,optimizedMs:s.optimized.total.total+fixed,reductionPercent:100*(1-(s.optimized.total.total+fixed)/(s.reference.total.total+fixed))}};
});
const gates={};
for(const name of ['peer-browser','draw-round-reset','draw-rtc','draw-relay','draw-rtc-release']){
 const r=report(name);assert.equal(r.wasm,name==='draw-rtc-release'?release.sha256:development.sha256);
 if(name==='draw-rtc'||name==='draw-relay'||name==='draw-rtc-release'){
  assert.equal(r.hardwareRequested,true);for(const renderer of r.result.renderers)assert.equal(renderer,diagnosis.result.renderer);
 }
 gates[name]=r;
}
const realtime= ['draw-rtc-full-a','draw-rtc-fast-b','draw-rtc-fast-b2','draw-rtc-full-a2'].map((name,i)=>{
 const r=report(name);assert.equal(r.wasm,development.sha256);assert.equal(r.fullSpriteGeometry,i===0||i===3);
 assert.equal(r.hardwareRequested,true);for(const renderer of r.result.renderers)assert.equal(renderer,diagnosis.result.renderer);
 return {name,...r};
});
const keyboardPrediction=report('keyboard-prediction');assert.equal(keyboardPrediction.rows.length,20);
const lifecycle=resolve(root,'artifacts/sdl3/browser/netplay/report.json');
assert.ok(statSync(lifecycle).mtimeMs>statSync(resolve(root,'artifacts/sdl3/build.json')).mtimeMs);
gates.lifecycle={...read(lifecycle),wasm:development.sha256};assert.equal(gates.lifecycle.passed,true);
for(const name of ['paused','localReplayAfterMatch','reconnected'])assert.ok(gates.lifecycle.states.some(s=>s.name===name));
assert.equal(sha(readFileSync(resolve(root,'build-eagler-multiplayer/th09.wasm'))),release.sha256);
assert.equal(release.exports.some(e=>e.name.startsWith('th09_probe_')),false);
const fixtures=Object.fromEntries(['draw-cost-browser.mjs','phase-cost-browser.mjs','transport-browser.mjs','world-browser.mjs','peer-browser.mjs','summarize-draw-evidence.mjs'].map(name=>[name,sha(readFileSync(resolve(root,'tests/multiplayer',name)))]));
const evidence={passed:true,scope:'Local paired PC Draw CPU improvement; realtime ABBA is mixed, no overall smoothness, deployment, phone or human latency claim',comparison,fixtures,builds:{diagnosticBaseline,development,release},diagnosis,paired,realtime,keyboardPrediction,gates};
writeFileSync(resolve(repo,'docs/multiplayer/rollback-draw-evidence.json'),JSON.stringify(evidence,null,2)+'\n');
console.log(JSON.stringify({passed:true,comparison,release:release.sha256}));
