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
const dev=read(resolve(root,'artifacts/sdl3/build.json'));
const release=read(resolve(root,'artifacts/sdl-release/build.json'));
const baseline=read(resolve(root,'artifacts/pc-baseline/build.json'));
for(const [build,path] of [[dev,'artifacts/sdl3/th09-presentation.wasm'],[release,'artifacts/sdl-release/th09.wasm'],[baseline,'artifacts/pc-baseline/th09-presentation.wasm']])assert.equal(sha(readFileSync(resolve(root,path))),build.sha256);
for(const build of [dev,release])for(const [path,expected] of Object.entries(build.sources))assert.equal(sha(readFileSync(resolve(repo,path))),expected,'stale build source: '+path);
const names=['pc-baseline-a','pc-candidate-b','pc-baseline-a2','pc-candidate-b2'];
const runs=names.map(report);
for(let i=0;i<runs.length;++i){
 const r=runs[i];assert.equal(r.wasm,i%2?dev.sha256:baseline.sha256);assert.equal(r.hardwareRequested,true);
 assert.equal(r.browser,runs[0].browser);assert.equal(r.result.renderer,runs[0].result.renderer);
 assert.doesNotMatch(r.result.renderer,/swiftshader|llvmpipe|microsoft basic render/i);
 assert.deepEqual(r.result.audits,runs[0].result.audits,'cross-build gameplay mismatch');
 assert.equal(r.result.audits.length,200);assert.equal(r.result.bytes,runs[0].result.bytes);
}
const comparison={};
for(const metric of ['baseEightTicks','capture','restore','resimEightTicks','correction']){
 const a=(runs[0].result.summary[metric].total+runs[2].result.summary[metric].total)/2;
 const b=(runs[1].result.summary[metric].total+runs[3].result.summary[metric].total)/2;
 comparison[metric]={baselineMeanMs:a,candidateMeanMs:b,reductionPercent:100*(1-b/a)};
}
for(const candidate of [runs[1],runs[3]])for(const old of [runs[0],runs[2]]){
 assert.ok(candidate.result.summary.capture.total<old.result.summary.capture.total,'capture did not improve');
 assert.ok(candidate.result.summary.correction.total<old.result.summary.correction.total,'complete correction did not improve');
}
const gates={};
for(const name of ['pc-rtc-baseline','pc-rtc-candidate','pc-relay-candidate','pc-rtc-release','peer-browser','pc-round-reset']){
 const r=report(name);assert.equal(r.wasm,name==='pc-rtc-baseline'?baseline.sha256:name==='pc-rtc-release'?release.sha256:dev.sha256);
 if(name.startsWith('pc-rtc')||name==='pc-relay-candidate'){
  assert.equal(r.hardwareRequested,true);for(const renderer of r.result.renderers)assert.equal(renderer,runs[0].result.renderer);
 }
 gates[name]=r;
}
for(const name of ['session','dynamic-state'])gates[name]=report(name);
const lifecycle=resolve(root,'artifacts/sdl3/browser/netplay/report.json');
assert.ok(statSync(lifecycle).mtimeMs>statSync(resolve(root,'artifacts/sdl3/build.json')).mtimeMs,'stale lifecycle gate');
gates.lifecycle={...read(lifecycle),wasm:dev.sha256};assert.equal(gates.lifecycle.passed,true);
for(const name of ['paused','localReplayAfterMatch','reconnected'])assert.ok(gates.lifecycle.states.some(s=>s.name===name),'missing lifecycle '+name);
const serverLog=readFileSync(resolve(out,'pc-server.log'),'utf8');assert.match(serverLog,/(?:#|ℹ)\s+fail 0(?:\r?\n|$)/);gates.server={passed:true,logSha256:sha(serverLog)};
assert.equal(sha(readFileSync(resolve(root,'build-eagler-multiplayer/th09.wasm'))),release.sha256);
const fixtures=Object.fromEntries(['pc-cost-browser.mjs','transport-browser.mjs','world-browser.mjs','peer-browser.mjs','summarize-pc-evidence.mjs'].map(name=>[name,sha(readFileSync(resolve(root,'tests/multiplayer',name)))]));
const compactRuns=runs.map((r,i)=>({...r,label:names[i],result:{...r.result,audits:undefined,auditSha256:sha(JSON.stringify(r.result.audits))}}));
const evidence={passed:true,scope:'Local desktop PC optimization; no deployment or phone claim',comparison,fixtures,builds:{baseline,development:dev,release},runs:compactRuns,gates};
writeFileSync(resolve(repo,'docs/multiplayer/rollback-pc-evidence.json'),JSON.stringify(evidence,null,2)+'\n');
console.log(JSON.stringify({passed:true,comparison,development:dev.sha256,release:release.sha256}));
