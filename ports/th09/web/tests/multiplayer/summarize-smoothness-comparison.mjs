import assert from 'node:assert/strict';
import {readFileSync,writeFileSync,statSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {fileURLToPath} from 'node:url';
import {resolve} from 'node:path';

// Explicit input labels allow frozen originals to remain beside intermediate
// candidates. Argument order is A1 B1 B2 A2; only identical wire input is paired.
const [name,...labels]=process.argv.slice(2);assert.equal(labels.length,4,'output-name A1 B1 B2 A2');
for(const label of [name,...labels])assert.match(label,/^[\w-]+$/);
const out=fileURLToPath(new URL('../../artifacts/multiplayer-tests/',import.meta.url));
const sha=b=>createHash('sha256').update(b).digest('hex');
const reports=labels.map(label=>JSON.parse(readFileSync(resolve(out,label+'-report.json'))));
const modified=labels.map(label=>statSync(resolve(out,label+'-report.json')).mtimeMs);
assert.ok(modified.every((time,i)=>!i||time>=modified[i-1]),'Reports are not in retained A1/B1/B2/A2 execution order');
const reference=reports[0];
for(const r of reports){
 assert(r.passed&&r.sameFrame);assert.equal(r.parameters.inputMode,'tape');assert.equal(r.parameters.profile,false);
 assert.deepEqual(r.parameters,reference.parameters);assert.deepEqual(r.browsers,reference.browsers);
 assert.equal(r.presentationMeasurement,reference.presentationMeasurement);
 for(const key of ['harness','relay','shell'])assert.equal(r.identity[key],reference.identity[key],key);
 for(let side=0;side<2;++side){const e=r.results[side],ref=reference.results[side];
  assert.equal(e.route,'rtc');assert.equal(e.renderer,ref.renderer);assert.doesNotMatch(e.renderer,/swiftshader|llvmpipe|microsoft basic render/i);
  assert.equal(e.stats[0],r.parameters.limit);assert.equal(e.stats[1],r.parameters.limit);assert.equal(e.stats[4],r.parameters.limit);
  assert.deepEqual(e.measure.inputTape.slice(0,r.parameters.limit),ref.measure.inputTape.slice(0,r.parameters.limit));
  assert.equal(e.inputTapeSha256,ref.inputTapeSha256);assert.equal(e.hash,ref.hash);assert.deepEqual(e.audited,ref.audited);
 }
}
assert.equal(reports[0].identity.wasm,reports[3].identity.wasm);assert.equal(reports[1].identity.wasm,reports[2].identity.wasm);
const distribution=values=>{const v=values.toSorted((a,b)=>a-b);return {count:v.length,mean:v.reduce((a,b)=>a+b,0)/v.length,
 p50:v[Math.floor(v.length*.5)],p95:v[Math.floor(v.length*.95)],p99:v[Math.floor(v.length*.99)],max:v.at(-1),over25ms:v.filter(n=>n>25).length,over50ms:v.filter(n=>n>50).length};};
const aggregate=(runs,side)=>{
 const work=[],gaps=[];let forward=0,resim=0,elapsed=0,waits=0;
 for(const r of runs){const s=r.summary[side];forward+=s.lastFrame-s.firstFrame;resim+=s.resimulated;elapsed+=s.elapsedMs;waits+=s.waitCallbacks;
  const samples=r.results[side].measure.samples.filter(v=>v[3]>=180&&v[3]<r.parameters.limit);let previous;
  for(const sample of samples){work.push(sample[2]);if(!previous||sample[11]!==previous[11]){if(previous)gaps.push(sample[13]-previous[13]);previous=sample;}}
 }
 return {logicHz:forward*1000/elapsed,forwardFrames:forward,resimulatedFrames:resim,waitCallbacks:waits,
  callbackWorkMs:distribution(work),submittedRafGapMs:distribution(gaps),
  workMsPerForwardFrame:work.reduce((a,b)=>a+b,0)/forward,workMsPerSimulatedFrame:work.reduce((a,b)=>a+b,0)/(forward+resim)};
};
const endpoints=[0,1].map(side=>{
 const baseline=aggregate([reports[0],reports[3]],side),candidate=aggregate([reports[1],reports[2]],side);
 return {side,baseline,candidate,workReductionPercent:100*(1-candidate.workMsPerForwardFrame/baseline.workMsPerForwardFrame)};
});
const result={passed:true,scope:'Desktop RTC, exact wire-input workload and terminal world equality; aggregate measured work and renderer-submission RAF gaps, not physical scanout, remote geography or phone/human acceptance',
 parameters:reference.parameters,baseline:reference.identity.wasm,candidate:reports[1].identity.wasm,renderer:reference.results[0].renderer,
 inputTapeSha256:reference.results.map(r=>r.inputTapeSha256),endpoints,sources:labels.map(label=>({label,sha256:sha(readFileSync(resolve(out,label+'-report.json')))}))};
writeFileSync(resolve(out,name+'-comparison.json'),JSON.stringify(result,null,2)+'\n');console.log(JSON.stringify(result));
