// Assess retained browser evidence; never edits or re-labels an original run.
// A low maximum callback duration alone does not establish playable cadence.
import assert from 'node:assert/strict';
import {readFileSync,writeFileSync,existsSync} from 'node:fs';
import {resolve,join} from 'node:path';
import {createHash} from 'node:crypto';

const limits={minimumForwardHz:58,presentP95Ms:33.4,presentP99Ms:50,maxPresentMs:100,maxNativeMs:100};
function assess(r){
  const failures=[],require=(ok,message)=>{if(!ok)failures.push(message);};
  require(!!r.completedAt,'Run has not completed');
  require(r.sourceStable===true,'Source stability was not verified');
  require(r.status==='passed','Original browser acceptance did not pass');
  require(Array.isArray(r.errors)&&r.errors.length===0,'Browser errors or missing error inventory');
  require(r.correctness?.exportedNativeFieldsEqual===true,'Confirmed native field equality was not verified');
  require(!r.settings?.nativeProfile&&!r.settings?.audioProfile&&!r.settings?.cpuSamples,'Diagnostic run cannot establish performance acceptance');
  require(r.results?.length===2,'Both independent peers are required');
  const peers=(r.results||[]).map(({seat,metrics:m})=>{
    const p=m?.presentWindowHolds,n=m?.nativeReportedWall;
    const gate=(value,maximum,label)=>require(Number.isFinite(value)&&value<=maximum,`P${seat+1} ${label}: ${value} (maximum ${maximum})`);
    require(Number.isFinite(m?.logicHz)&&m.logicHz>=limits.minimumForwardHz,`P${seat+1} forward rate: ${m?.logicHz} (minimum ${limits.minimumForwardHz})`);
    gate(p?.p95,limits.presentP95Ms,'present p95 ms');gate(p?.p99,limits.presentP99Ms,'present p99 ms');
    gate(p?.max,limits.maxPresentMs,'maximum present hold ms');gate(n?.max,limits.maxNativeMs,'maximum native callback ms');
    return {seat,forwardHz:m?.logicHz,present:p,native:n,actualSendDelay:m?.actualSendDelay,rollback:m?.rollbackDelta};
  });
  return {passed:failures.length===0,failures,peers};
}
if(process.argv.includes('--self-test')){
  const m={logicHz:60,presentWindowHolds:{p95:17,p99:20,max:25},nativeReportedWall:{max:12}};
  const good=()=>({completedAt:'synthetic',sourceStable:true,status:'passed',errors:[],settings:{},correctness:{exportedNativeFieldsEqual:true},results:[{seat:0,metrics:structuredClone(m)},{seat:1,metrics:structuredClone(m)}]});
  assert.equal(assess(good()).passed,true);
  let r=good();r.results[0].metrics.logicHz=30;assert.equal(assess(r).passed,false);
  r=good();r.results[1].metrics.presentWindowHolds.p99=80;assert.equal(assess(r).passed,false);
  r=good();r.settings.cpuSamples=true;assert.equal(assess(r).passed,false);
  r=good();r.correctness.exportedNativeFieldsEqual=false;assert.equal(assess(r).passed,false);
  r=good();r.sourceStable=false;assert.equal(assess(r).passed,false);
  r=good();r.results.pop();assert.equal(assess(r).passed,false);
  console.log('PASS movement gate: smooth cadence, slow average, tail stalls, diagnostic contamination, equality, source identity and missing peer');
}else{
  const args=process.argv.slice(2),at=args.indexOf('--output');
  assert.ok(at>=0&&args[at+1],'Usage: node movement-performance-gate.mjs <receipt directories...> --output <fresh JSON file>');
  const output=resolve(args[at+1]);args.splice(at,2);
  assert.ok(args.length&&args.every(x=>!x.startsWith('--')),'Provide retained receipt directories');
  assert.ok(!existsSync(output),'Never overwrite an existing assessment');
  const runs=args.map(path=>{
    const file=join(resolve(path),'receipt.json'),bytes=readFileSync(file),r=JSON.parse(bytes);
    return {file,sha256:createHash('sha256').update(bytes).digest('hex'),settings:r.settings,...assess(r)};
  });
  const report={schema:'th11-movement-performance-gate/1',createdAt:new Date().toISOString(),limits,
    scope:'Derived cadence assessment only. Requested one-way delay is NOT measured RTT. Native field equality is NOT a complete world or pixel oracle.',runs};
  writeFileSync(output,JSON.stringify(report,null,2)+'\n',{flag:'wx'});
  for(const r of runs)console.log(JSON.stringify({file:r.file,passed:r.passed,failures:r.failures}));
  process.exitCode=runs.every(r=>r.passed)?0:1;
}
