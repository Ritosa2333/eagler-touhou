// Summarize already-retained component runs; never rerun tests or infer phone timing.
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {readFileSync, writeFileSync, existsSync} from 'node:fs';
import {resolve} from 'node:path';
const root=process.cwd(), evidenceRoot=resolve(root,'web/artifacts/multiplayer-tests');
const sha=b=>createHash('sha256').update(b).digest('hex');
const identity=p=>{const b=readFileSync(p);return {path:p,bytes:b.length,sha256:sha(b)};};
const readJson=p=>JSON.parse(readFileSync(p,'utf8'));
function invocation(label){
 const p=resolve(evidenceRoot,label+'-invocation.json'),v=readJson(p),log=resolve(evidenceRoot,label+'-process.log');
 assert.equal(v.status,0,label+' process failed');assert.equal(v.scriptSha256,v.scriptAfterSha256);
 assert.deepEqual(v.identityBefore,v.identityAfter);assert.equal(v.identityError,null);
 assert.equal(sha(readFileSync(log)),v.logSha256);
 return {identity:identity(p),record:v,log:identity(log),text:readFileSync(log,'utf8')};
}
function queue(label,mode){
 const run=invocation(label),records=run.text.split(/\r?\n/).flatMap(line=>{
  const first=line.indexOf('{');if(first<0)return [];
  const value=JSON.parse(line.slice(first));return [value];
 });
 const cases=records.filter(r=>r.case),sources=records.filter(r=>r.sourceStable===true);
 assert.equal(cases.length,21);assert.equal(sources.length,1);assert.equal(sources[0].mode,mode);
 assert.match(run.text,/tests 22/);assert.match(run.text,/pass 22/);assert.match(run.text,/fail 0/);
 for(const row of cases){assert.equal(row.mode,mode);assert.deepEqual(row.sourceIdentity,sources[0].sourceIdentity);}
 const goals=cases.flatMap(row=>Object.entries(row.goals).map(([name,passed])=>({case:row.case,name,passed})));
 if(mode==='candidate-goals')assert.ok(goals.every(row=>row.passed===true));
 return {invocation:run.identity,process:run.record,log:run.log,sources:sources[0],cases,
  goals:{total:goals.length,passed:goals.filter(x=>x.passed===true).length,failed:goals.filter(x=>x.passed!==true)}};
}
const baseline=queue('managed-touch-queue-b1','characterization'),candidate=queue('managed-touch-queue-c1','candidate-goals');
assert.equal(baseline.sources.sourceIdentity.test,candidate.sources.sourceIdentity.test);
assert.equal(baseline.sources.sourceIdentity.managed,'1f4ef1810d58c76c6fb85a1032257af5d49ab09f81de56a1114eae2dc7a76608');
assert.equal(candidate.sources.sourceIdentity.managed,'830f521ffa13c3be443c8bb277da510734c9625efcb224b1d1ed6ae3e7eee4f4');
assert.ok(baseline.goals.failed.length>0,'baseline must retain observed counterexamples');
const nativeRun=invocation('managed-touch-native-c1'),nativeResults=nativeRun.text.split(/\r?\n/).filter(s=>s.startsWith('{')).map(JSON.parse);
assert.equal(nativeResults.length,1);const nativeResult=nativeResults[0];
assert.equal(nativeResult.passed,true);assert.equal(nativeResult.groups,9);assert.equal(nativeResult.assertions,520);assert.equal(nativeResult.legacyVerify,0);
const nativeReport=resolve(evidenceRoot,'managed-touch-native-c1/touch-controller-report.json');
assert.equal(readJson(nativeReport).passed,true);
const output=resolve(root,'docs/multiplayer/rollback-managed-touch-component-evidence.json');
assert.equal(existsSync(output),false,'do not overwrite retained component evidence');
const result={schema:'th09-managed-touch-component-evidence/v1',createdAt:new Date().toISOString(),
 scope:'Actual managed listeners with controlled dependencies and stub native, plus production C++ TouchController component. Not real Launcher/device, elapsed input latency, whole game or FPS acceptance.',
 candidate,baseline,native:{invocation:nativeRun.identity,process:nativeRun.record,log:nativeRun.log,report:identity(nativeReport),result:nativeResult},
 boundaries:['node:test duration is runner wall time, not touch latency','Baseline characterization PASS is not a passed behavior goal','Standalone neutral native controls remain in raw evidence','Document replacement in the VM is an explicit model, not actual WindowProxy navigation','Native C++ component assertions do not validate the managed-to-WASM integration'],
 source:identity(resolve(root,'web/tests/multiplayer/managed-touch-component-summary.mjs'))};
writeFileSync(output,JSON.stringify(result,null,2)+'\n',{flag:'wx'});
console.log(JSON.stringify({output,...identity(output),candidateGoals:candidate.goals.total,candidateFailed:candidate.goals.failed.length,
 baselineGoals:baseline.goals.total,baselineFailed:baseline.goals.failed.length,native:nativeResult}));
