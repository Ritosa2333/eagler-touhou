// Condense frozen local reports; fail if build/source identities or oracles disagree.
// Run after the commands in docs/multiplayer/rollback-4x-performance.md.
import assert from 'node:assert/strict';
import {readFileSync,writeFileSync} from 'node:fs';
import {execFileSync} from 'node:child_process';
import {resolve,relative} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
const workspace=fileURLToPath(new URL('../../../',import.meta.url));
const baselineWorkspace=resolve(process.env.BASELINE_WORKTREE||resolve(workspace,'../th09-rollback-4x-baseline'));
const common=resolve(process.env.EAGLER_COMMON_ROOT||resolve(workspace,'../../th09-eagler/third_party/eagler-common'));
const out=resolve(workspace,'th09_web/artifacts/multiplayer-tests');
const sha=p=>createHash('sha256').update(readFileSync(p)).digest('hex');
const portable=p=>relative(workspace,p).replaceAll('\\','/');
const metadata=[];
const read=name=>{const path=resolve(out,name),value=JSON.parse(readFileSync(path));assert.equal(value.passed,true,name);metadata.push({path:portable(path),sha256:sha(path)});return value;};
const report=label=>read(label+'-report.json');
const git=(cwd,...args)=>execFileSync('git',args,{cwd,encoding:'utf8'}).trim();
const baseRevision=git(workspace,'rev-parse','6ce1c46');
const selectedCodeRevision=git(workspace,'log','-1','--format=%H','--','portable/sdl/Renderer.cpp','th09_web/cpp');
const commonRevision=git(common,'rev-parse','HEAD');
assert.equal(commonRevision,'43add4dc9085c65468f7ab15428c00206cbe3834');
function build(owner,name,release,verify){
 const dir=resolve(owner,'th09_web/artifacts',name),path=resolve(dir,'build.json'),data=JSON.parse(readFileSync(path));
 const stem=release?'th09':'th09-presentation',wasm=resolve(dir,stem+'.wasm');
 assert.equal(sha(wasm),data.sha256);
 if(verify)for(const [file,hash] of Object.entries(data.sources))assert.equal(sha(resolve(owner,file)),hash,'Source changed since build: '+file);
 if(release)assert.ok(!data.exports.some(e=>e.name.startsWith('th09_probe_')||e.name==='th09_title_open'));
 return {directory:portable(dir),wasmSha256:data.sha256,loaderSha256:sha(resolve(dir,stem+'.mjs')),bytes:data.bytes,manifestSha256:sha(path),sourceCount:Object.keys(data.sources).length,sources:data.sources};
}
const builds={
 baselineRelease:build(baselineWorkspace,'rollback4x-baseline-release',true,true),
 selectedRelease:build(workspace,'rollback4x-candidate-release',true,true),
 selectedDevelopment:build(workspace,'rollback4x-background-span-profile',false,true),
 rendererDevelopment:build(workspace,'rollback4x-line-rebase-profile',false,false),
 baselineDevelopment:build(workspace,'rollback4x-baseline-profile',false,false)
};
assert.deepEqual(builds.selectedDevelopment.sources,builds.selectedRelease.sources);
for(const p of ['portable/sdl/Renderer.cpp','portable/sdl/Renderer.hpp','portable/sdl/Shaders.hpp','th09_web/cpp/sdl/GraphicsDevice.cpp'])
 assert.equal(builds.rendererDevelopment.sources[p],builds.selectedRelease.sources[p],'Renderer changed since boundary fixture');
const labels=['rollback4x-release-a1','rollback4x-release-b1','rollback4x-release-b2','rollback4x-release-a2'];
let trace,tape,parameters,harness;
const runs=labels.map((label,i)=>{
 const x=report(label),candidate=i===1||i===2;
 assert.equal(x.identity.wasm,builds[candidate?'selectedRelease':'baselineRelease'].wasmSha256);
 assert.equal(x.parameters.slow,4);assert.equal(x.parameters.release,true);assert.equal(x.parameters.profile,false);assert.equal(x.parameters.audioOutput,true);assert.equal(x.parameters.fallback,true);assert.equal(x.parameters.inputMode,'tape');
 if(parameters)assert.deepEqual(x.parameters,parameters);parameters=x.parameters;
 if(harness)assert.equal(x.identity.harness,harness);harness=x.identity.harness;
 const pair=x.results.map(r=>r.confirmedHashes.slice(0,15));
 assert.equal(pair[0].length,15);assert.deepEqual(pair[0],pair[1]);for(let n=0;n<15;++n)assert.equal(pair[0][n][0],(n+1)*120);
 if(trace)assert.deepEqual(pair[0],trace);trace=pair[0];
 const inputs=x.results.map(r=>r.inputTapeSha256);if(tape)assert.deepEqual(inputs,tape);tape=inputs;
 for(const r of x.results){assert.equal(r.route,'relay');assert.ok(r.audioOutput.nonzeroBlocks>100&&!r.audioOutput.overflow);}
 return {label,variant:candidate?'candidate':'baseline',identity:x.identity,browsers:x.browsers,summary:x.summary,
  peers:x.results.map(r=>({renderer:r.renderer,memoryBytes:r.memoryBytes,finalStats:r.stats,finalHash:r.hash,finalComparable:x.finalComparable,audio:r.audio,audioOutput:{blocks:r.audioOutput.blocks.length,nonzeroBlocks:r.audioOutput.nonzeroBlocks,sampledValues:r.audioOutput.sampledValues,maxAmplitude:r.audioOutput.maxAmplitude,overflow:r.audioOutput.overflow},sdlMeasured:r.sdlMeasured}))};
});
assert.equal(harness,sha(resolve(workspace,'th09_web/tests/multiplayer/smoothness-browser.mjs')));
const range=values=>[Math.min(...values),Math.max(...values)];
const group=variant=>{const samples=runs.filter(r=>r.variant===variant).map(r=>r.summary[1]);return {logicHz:range(samples.map(s=>s.logicHz)),callbackP50Ms:range(samples.map(s=>s.callbackWorkMs.p50)),presentationP50Ms:range(samples.map(s=>s.presentationGapMs.p50)),presentationP95Ms:range(samples.map(s=>s.presentationGapMs.p95)),presentationP99Ms:range(samples.map(s=>s.presentationGapMs.p99)),presentationMaximumMs:range(samples.map(s=>s.presentationGapMs.max)),logicHzArithmeticMean:samples.reduce((n,s)=>n+s.logicHz,0)/samples.length};};
const statsNames=['calls','batches','uploadBytes','readBytes','frames','presentations','bufferReplacements','bufferSubUpdates','vertexUploadBytes','directBytes','copiedBytes','layoutSetups','textureBinds','framebufferBinds','programCompiles','genericBatches','resamples'];
const named=values=>Object.fromEntries(statsNames.map((k,i)=>[k,values[i]]));
const releaseRender=report('rollback4x-release-render-j7200-final');
assert.deepEqual(releaseRender.identity.wasm,[builds.baselineRelease.wasmSha256,builds.selectedRelease.wasmSha256]);
assert.equal(releaseRender.identity.harness,sha(resolve(workspace,'th09_web/tests/multiplayer/release-render-browser.mjs')));
for(const pair of releaseRender.checks){assert.equal(pair[0].imageSha256,pair[1].imageSha256);assert.equal(pair[0].hash,pair[1].hash);assert.deepEqual(pair[0].session,pair[1].session);}
const renderTotals=releaseRender.checks.at(-1).map(x=>named(x.sdl));
const change=key=>(renderTotals[1][key]/renderTotals[0][key]-1)*100;
const devRender=report('rollback4x-render-equivalence-i1');
assert.deepEqual(devRender.identity.wasm,[builds.baselineDevelopment.wasmSha256,builds.rendererDevelopment.wasmSha256]);
for(const pair of devRender.checks){assert.equal(pair[0].imageSha256,pair[1].imageSha256);assert.equal(pair[0].hash,pair[1].hash);assert.deepEqual(pair[0].audit,pair[1].audit);}
const queue=read('rollback4x-queue-boundaries-i/report.json');
assert.equal(queue.identity.source,sha(resolve(workspace,'th09_web/tests/multiplayer/renderer-queue.cpp')));
assert.equal(queue.checks.length,5);for(const pair of queue.checks)assert.equal(pair[0].image,pair[1].image);
const world=report('rollback4x-world-sparse-round-j');
assert.equal(world.wasm,builds.selectedDevelopment.wasmSha256);assert.equal(world.result.checkpoints,270);
for(const key of ['poolBoundary','bulletBoundary','recordingBoundary'])assert.equal(world.result[key],1);
function peer(label,wasm){
 const x=report(label);assert.equal(x.wasm,wasm);assert.equal(new Set(x.result.hashes).size,1);assert.equal(x.result.framebuffersEqual,true);
 assert.deepEqual(x.result.audited[0],x.result.audited[1]);assert.deepEqual(x.result.audited[0],x.result.audited[2]);
 return {...x,result:{...x.result,samples:undefined}};
}
const deep8=peer('rollback4x-peer-deep8-i',builds.rendererDevelopment.wasmSha256);
const endurance=peer('rollback4x-peer-endurance-j7200',builds.selectedDevelopment.wasmSha256);
assert.ok(endurance.result.phases.every(p=>p.includes(3))&&endurance.result.pauseCalls.every(n=>n>20));
const phases=['rollback4x-phase-line-i','rollback4x-phase-span-j'].map(label=>{const x=report(label);return {label,wasm:x.wasm,scope:x.scope,frames:x.result.frames,bytes:x.result.bytes,summary:x.result.summary};});
const components=['graphics-projection','audio-pitch','frame-schedule','spectator-schedule','session','adonis','dynamic-state','bullet-snapshot'].map(name=>{
 const x=report(name);for(const [path,hash] of Object.entries(x.sources))assert.equal(sha(path),hash,'Component source changed: '+path);
 return {name,passed:x.passed,wasmSha256:x.wasmSha256,scope:x.scope,compiler:x.compiler,flags:x.flags,sources:x.sources};
});
const baseline=group('baseline'),candidate=group('candidate');
const evidence={
 schemaVersion:1,date:'2026-10-10',baseRevision,selectedCodeRevision,commonRevision,branch:'experiment/rollback-4x',
 verdict:{presentationImproved:true,stable60HzAt4xAchieved:false,description:'Release ABBA shows substantially more continuous presentation, but net logical advancement remains about 31 Hz. Do not describe the 4x target as met.'},
 measurement:{parameters,inputTapeSha256:tape,confirmedTrace:trace,order:'A1 B1 B2 A2; all runs serialized; only P2 CPU throttled',
  definition:'Net logic Hz is forward frame advancement divided by elapsed wall time; historical replay ticks do not count as new frames. Presentation gaps are RAF timestamps of callbacks with a new renderer Present count, not physical scanout. Completion gaps are separate.',
  statistics:'Ranges below are the min/max of the two individual run statistics, not pooled percentiles or confidence intervals. Two runs per variant do not establish statistical significance. Closed-loop timing changes the actual number of corrections/resimulations.',
  baseline,candidate,descriptiveMeanLogicChangePercent:(candidate.logicHzArithmeticMean/baseline.logicHzArithmeticMean-1)*100},
 builds,runs,
 correctness:{
  releaseRendering:{scope:releaseRender.scope,identity:releaseRender.identity,frames:releaseRender.frames,checkpointCount:releaseRender.checks.length,checks:releaseRender.checks.map(p=>({frame:p[0].frame,imageSha256:p[0].imageSha256,hash:p[0].hash,session:p[0].session})),renderers:releaseRender.renderers,totals:renderTotals,changePercent:{batches:change('batches'),bufferReplacements:change('bufferReplacements'),vertexUploadBytes:change('vertexUploadBytes')}},
  developmentRendering:{scope:devRender.scope,identity:devRender.identity,frames:devRender.frames,checkpoints:devRender.checks.length,final:devRender.checks.at(-1)},
  rendererBoundaries:queue,worldRoundReset:world,deep8,endurance,phaseCost:phases,components
 },
 rejected:[
  {candidate:'B/C common TouchMany and bulk-copy prototype',reason:'Measured live throughput regression; canonical eagler-common pin retained; isolated dirty prototype is not integrated.'},
  {candidate:'F O3/LTO',reason:'Live run did not improve measured throughput; original O2 and floating-point flags retained.'},
  {candidate:'H nonzero-first GL_LINES queue',reason:'Exact pixel checks found missing lines; replaced by separate line VAO with rebased attribute offsets in I/J.'},
  {candidate:'K bounded audio refill quantum',reason:'Controlled tape run worsened logic and presentation metrics; source reverted to J.'},
  {candidate:'Pre-resampled sound-effect PCM cache',reason:'Authored fixture probe found waveform/cursor differences; no cache integrated.'},
  {candidate:'4500-frame paused endurance fixture',reason:'Coverage assertion did not reach results; original failure preserved and coverage completed by the passing 7200-frame run.'}
 ],
 limitations:[
  'The 4x stable-60-Hz performance target is not met. Candidate still has >100 ms presentation gaps.',
  'Desktop Chromium 153 with Intel UHD ANGLE D3D11; no phone, Safari, public Internet, RTC/TURN or physical scanout acceptance.',
  'Real-relay ABBA measures 1800 logical input frames; two separate 7200-frame checks cover rollback correctness and production rendering, not 7200-frame real-relay 4x performance.',
  'Input impairment is seeded application-send delay 39 +/- 5 ms; delivered delay is higher under event-loop load and is recorded separately.',
  'Sampled nonzero SDL output blocks verify active audio output, not uninterrupted whole-stream or acoustic quality.',
  'MA_SOUND_FLAG_NO_PITCH removes the old unity-rate pitch filter. At 44100 Hz waveform bytes intentionally differ; tested source PCM, cursors, loops, seek, pan, gain, stop/resume. Necessary 22050/48000 Hz resampling is retained.',
  'Renderer queue reduces batch and buffer-replacement counts; vertex upload bytes rise about 0.49% in the 7200-frame full-render fixture. These counts are not proportional CPU-speedup claims.',
  'The 2130-byte Replay comparison is the final saved recording after lifecycle transitions, not a claim that one Replay contains all 7200 test frames.',
  'Portable renderer changes have TH09 and synthetic GLES coverage here; no full-game rebuild/acceptance for other titles.'
 ],
 rawReports:metadata
};
const destination=resolve(workspace,'docs/multiplayer/rollback-4x-evidence.json');
writeFileSync(destination,JSON.stringify(evidence,null,2)+'\n');
console.log(JSON.stringify({destination,sha256:sha(destination),sourceFilesVerified:builds.selectedRelease.sourceCount,baseline,candidate,rendererChangePercent:evidence.correctness.releaseRendering.changePercent,rawReports:metadata.length}));
