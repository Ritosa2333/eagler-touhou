import assert from 'node:assert/strict';
import {readFileSync,writeFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
const out=fileURLToPath(new URL('../../artifacts/multiplayer-tests/',import.meta.url));
const labels=process.argv.slice(2);assert.ok(labels.length);
const sha=bytes=>createHash('sha256').update(bytes).digest('hex');
const distribution=values=>{const a=values.slice().sort((a,b)=>a-b);return {count:a.length,p50:a[Math.floor(a.length*.5)]??null,p95:a[Math.floor(a.length*.95)]??null,p99:a[Math.floor(a.length*.99)]??null,max:a.at(-1)??null,over25ms:a.filter(n=>n>25).length,over50ms:a.filter(n=>n>50).length};};
const runs=labels.map(label=>{assert.match(label,/^[\w-]+$/);const bytes=readFileSync(resolve(out,label+'-report.json'));return {label,sha256:sha(bytes),report:JSON.parse(bytes)};});
const base=runs[0].report;
for(const {label,report:r} of runs){
 assert.ok(r.passed,label);assert.ok(r.confirmedChecks>=Math.floor(r.parameters.limit/120));
 for(let side=0;side<2;++side){
  const x=r.results[side];assert.doesNotMatch(x.renderer,/swiftshader|llvmpipe|microsoft basic render/i);
  for(let i=0;i<r.confirmedChecks;++i)assert.equal(x.confirmedHashes[i][0],(i+1)*120,'Unexpected confirmed frame');
 }
 assert.deepEqual(r.results[0].confirmedHashes.slice(0,r.confirmedChecks),r.results[1].confirmedHashes.slice(0,r.confirmedChecks));
 if(r.parameters.inputMode==='tape'&&base.parameters.inputMode==='tape'&&r.parameters.limit===base.parameters.limit){
  for(let side=0;side<2;++side){
   assert.equal(r.results[side].inputTapeSha256,base.results[side].inputTapeSha256,'Wire input changed across builds');
   assert.deepEqual(r.results[side].confirmedHashes.slice(0,r.confirmedChecks),base.results[side].confirmedHashes.slice(0,r.confirmedChecks),'Cross-build confirmed state differs');
  }
 }
}
const summary=runs.map(({label,sha256,report:r})=>({label,sha256,identity:r.identity,parameters:r.parameters,browsers:r.browsers,confirmedChecks:r.confirmedChecks,
 endpoints:r.results.map((x,side)=>{
  const samples=x.measure.samples.filter(s=>s[3]>=180&&s[3]<r.parameters.limit);
  const byPhase=new Map(),segments=[];let prior,part;
  for(const s of samples){
   if(!part||part.phase!==s[12]){part={phase:s[12],firstFrame:s[3],lastFrame:s[3],firstMs:s[0],lastMs:s[0],callbacks:0};segments.push(part);}
   ++part.callbacks;part.lastFrame=s[3];part.lastMs=s[0];
   let p=byPhase.get(s[12]);if(!p){p={phase:s[12],elapsedMs:0,netFrames:0,callbacks:0,waits:0,work:[],gaps:[],lastPresentation:null};byPhase.set(s[12],p);}
   ++p.callbacks;p.waits+=s[1]===2;p.work.push(s[2]);
   if(prior?.[12]===s[12]){p.elapsedMs+=s[0]-prior[0];p.netFrames+=s[3]-prior[3];}
   else p.lastPresentation=null;
   if(!p.lastPresentation||s[11]!==p.lastPresentation[11]){if(p.lastPresentation)p.gaps.push(s[13]-p.lastPresentation[13]);p.lastPresentation=s;}
   prior=s;
  }
  const blocks=x.audioOutput?.blocks||[],audioGaps=[],excess=[];let silentRun=0,maxSilentRun=0;
  for(let i=0;i<blocks.length;++i){const b=blocks[i];silentRun=b[4]<=1e-6?silentRun+1:0;maxSilentRun=Math.max(maxSilentRun,silentRun);
   if(i){audioGaps.push(b[0]-blocks[i-1][0]);excess.push(Math.max(0,(b[1]-blocks[i-1][1]-blocks[i-1][3])*1000));}
  }
  return {side,renderer:x.renderer,whole:r.summary[side],inputTapeSha256:x.inputTapeSha256,confirmedTraceSha256:sha(JSON.stringify(x.confirmedHashes.slice(0,r.confirmedChecks))),
   segments:segments.map(s=>({...s,elapsedMs:s.lastMs-s.firstMs,netFrames:s.lastFrame-s.firstFrame})),
   phases:[...byPhase.values()].map(p=>({phase:p.phase,elapsedMs:p.elapsedMs,netFrames:p.netFrames,logicHz:p.netFrames*1000/p.elapsedMs,callbacks:p.callbacks,waits:p.waits,workMs:distribution(p.work),submittedRafGapMs:distribution(p.gaps)})),
   audio:{blocks:blocks.length,nonzeroBlocks:x.audioOutput?.nonzeroBlocks,maxSilentRun,sampledSilentBlocksAfterOneSecond:blocks.filter(b=>b[0]>=1000&&b[4]<=1e-6).length,callbackGapMs:distribution(audioGaps),scheduledGapExcessMs:distribution(excess),scope:'64 sampled PCM values per SDL callback; not acoustic/device underrun proof'},memoryBytes:x.memoryBytes};
 })}));
const report={passed:true,scope:'Whole run and contiguous scene segments retained. Phase 1=match, phase 3=match_complete. Same tape comparisons require cross-build confirmed checks at every 120 frames.',runs:summary};
writeFileSync(resolve(out,labels[0]+'-summary.json'),JSON.stringify(report,null,2)+'\n');
console.log(JSON.stringify(report));
