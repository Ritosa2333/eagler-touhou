// ABBA comparison of retained, immutable-build phase reports. Rendering and
// full-state checks run outside the timed regions in phase-cost-browser.mjs.
import assert from 'node:assert/strict';
import {readFileSync,writeFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {fileURLToPath} from 'node:url';
import {resolve} from 'node:path';
const out=fileURLToPath(new URL('../../artifacts/multiplayer-tests/',import.meta.url));
const prefix=process.argv[2];assert.match(prefix||'',/^[\w-]+$/);
const labels=['a1','b1','b2','a2'].map(s=>prefix+'-'+s);
const sha=bytes=>createHash('sha256').update(bytes).digest('hex');
const reports=labels.map(label=>JSON.parse(readFileSync(resolve(out,label+'-report.json'))));
for(const r of reports){
 assert(r.passed&&r.hardwareRequested);
 assert.equal(r.browser,reports[0].browser);assert.equal(r.result.renderer,reports[0].result.renderer);
 assert.doesNotMatch(r.result.renderer,/swiftshader|llvmpipe|microsoft basic render/i);
 assert.deepEqual(r.result.audits,reports[0].result.audits,'Cross-build gameplay inventory mismatch');
}
assert.equal(reports[0].wasm,reports[3].wasm);assert.equal(reports[1].wasm,reports[2].wasm);
const distribution=values=>{const v=values.toSorted((a,b)=>a-b);return {count:v.length,p50:v[Math.floor(v.length*.5)],p95:v[Math.floor(v.length*.95)],p99:v[Math.floor(v.length*.99)],max:v.at(-1)};};
const metrics=Object.fromEntries(Object.keys(reports[0].result.summary).map(name=>{
 const baselineMeanMs=(reports[0].result.summary[name].total+reports[3].result.summary[name].total)/2;
 const candidateMeanMs=(reports[1].result.summary[name].total+reports[2].result.summary[name].total)/2;
 return [name,{baselineMeanMs,candidateMeanMs,reductionPercent:100*(1-candidateMeanMs/baselineMeanMs),
  baseline:distribution([...reports[0].result.samples[name],...reports[3].result.samples[name]]),
  candidate:distribution([...reports[1].result.samples[name],...reports[2].result.samples[name]])}];
}));
const result={passed:true,scope:'Two ABBA repetitions of the same PC 1P+CPU, single-checkpoint eight-tick fixture; not sustained ring/network or human acceptance',
 baseline:reports[0].wasm,candidate:reports[1].wasm,browser:reports[0].browser,renderer:reports[0].result.renderer,
 auditSha256:sha(JSON.stringify(reports[0].result.audits)),metrics,
 sources:labels.map(label=>({label,sha256:sha(readFileSync(resolve(out,label+'-report.json')))}))};
writeFileSync(resolve(out,prefix+'-comparison.json'),JSON.stringify(result,null,2)+'\n');
console.log(JSON.stringify(result));
