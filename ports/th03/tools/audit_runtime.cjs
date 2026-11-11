// Static-only distribution audit: no emulator, browser, service or gameplay.
const fs=require('node:fs'),path=require('node:path'),crypto=require('node:crypto'),assert=require('node:assert/strict'),{spawnSync}=require('node:child_process');
const root=path.resolve(__dirname,'..'),web=path.join(root,'web'),read=name=>fs.readFileSync(path.join(web,name)),sha=b=>crypto.createHash('sha256').update(b).digest('hex');
const runtime=JSON.parse(read('vendor/dosbox-x/runtime.json')),network=JSON.parse(read('lockstep-runtime.json'));
assert.equal(network.protocol,'th03-rollback/7');assert.equal(network.cpu_cycles,16000);assert.equal(network.snapshotStride,2);
for(const [name,hash]of Object.entries(network.files))assert.equal(sha(read(name)),hash,'Network fingerprint '+name);
for(const [name,hash]of Object.entries(runtime.files))assert.equal(sha(read('vendor/dosbox-x/'+name)),hash,'Core fingerprint '+name);
const buildPath=path.join(web,'vendor/dosbox-x/core-build.json');
const build=fs.existsSync(buildPath)?JSON.parse(fs.readFileSync(buildPath)):null;
if(build){
 assert.equal(build.game,'TH03');assert([false,'th03-battle-wait-four-instructions'].includes(build.idleFusion));assert.equal(build.zeroTimeCooperativeYields,true);
 assert.equal(runtime.network.idleFusion,build.idleFusion);
 if(build.idleFusion)assert.equal(network.idleFusion,build.idleFusion);
 assert.equal(build.cycles,16000);assert.equal(build.tickHz,60);assert.equal(build.snapshotStride,2);
 assert.equal(build.snapshotScan,'adaptive-word8');assert.equal(runtime.sourceWasm,build.rawWasmSha256);
 for(const [name,expected]of Object.entries({...build.localRuntimeFiles,...build.files}))
  assert.equal(sha(read('vendor/dosbox-x/'+name)),expected,'Frozen distribution resource '+name);
 for(const [name,expected]of Object.entries(build.behaviorFiles))assert.equal(sha(read(name)),expected,'Preserved behavior resource '+name);
 for(const [key,value]of Object.entries(build.releaseProfile))assert.equal(runtime.network[key],value,'Native profile '+key);
 assert.equal(network.snapshotScan,build.snapshotScan);
 if(build.inputPolicy){
  assert.deepEqual(build.inputPolicy,{mode:'room-rtt-hybrid',minInputDelay:0,maxInputDelay:6,zeroDelayRttMs:20,rttSamples:3,directionPrediction:6,maxPredictionLead:4,maxRollback:12,lockstepBootDelay:2,schedulingMarginSteps:0,policySchema:2,hybridReserveSteps:2,predictionJitterSteps:2});
  assert.deepEqual(network.inputPolicy,build.inputPolicy);assert.deepEqual(runtime.network.inputPolicy,build.inputPolicy);
 }
 if(build.networkWorker){
  assert.deepEqual(build.networkWorker,{version:1,default:'page',selection:'per-device-opt-in',maxImageTransfers:2,serializedGuestTransactions:true});
  for(const name of ['async-lockstep.js','frame-controls.js','simulation.js','simulation-worker.js','simulation-client.js'])assert(build.behaviorFiles['netplay/'+name]);
 }
 if(build.inputSampling)assert.deepEqual(build.inputSampling,{version:1,keyboardEventSampling:true,liveFrontierCapture:true});
 assert(read('dosbox-adapter.js').includes(Buffer.from('deterministic:net,nativeScheduling:true,nativeCpu:true,nativePlanar:false,nativeFramePresentation:false,')));
}else{
 const before=JSON.parse(fs.readFileSync(path.join(root,'build/before-dirty-pages-20261014/web/vendor/dosbox-x/runtime.json')));
 for(const name of ['wdosbox-x-fast.wasm','wdosbox-x-jspi.wasm','wdosbox-x-rollback.wasm','dosbox-jspi.mjs','dosbox-rollback.mjs'])assert.equal(runtime.files[name],before.files[name],'Existing execution changed '+name);
}
for(const name of ['disks/manifest.json','native-patch.json','native/main.exe','native/start.com','native/music.com','controls.js','fire-control.js','native-music.js','lockstep.js','dosbox-disk.js']){
 const old=spawnSync('git',['show','HEAD:web/'+name],{cwd:root,maxBuffer:64*1024*1024});assert.equal(old.status,0);
 // Intentional host changes are frozen separately. Native music lifecycle
 // now retains the DOS base across TSR removal; all DOS binaries stay fixed.
 if((name==='controls.js'&&build?.inputSampling)||(name==='lockstep.js'&&build?.inputPolicy)||(name==='native-music.js'&&build?.bridgeLifecycle)){
  assert(build.behaviorFiles[name]);continue;
 }
 if(/\.(js|json)$/.test(name))assert.equal(read(name).toString().replace(/\r\n/g,'\n'),old.stdout.toString().replace(/\r\n/g,'\n'),'Behavior resource changed '+name);
 else assert(read(name).equals(old.stdout),'Behavior resource changed '+name);
}
const normal=new WebAssembly.Module(read('vendor/dosbox-x/wdosbox-x-rollback.wasm')),tracked=new WebAssembly.Module(read('vendor/dosbox-x/wdosbox-x-rollback-tracked.wasm'));
assert.deepEqual(WebAssembly.Module.exports(tracked),WebAssembly.Module.exports(normal));
assert.deepEqual(WebAssembly.Module.imports(tracked).filter(i=>i.name!=='thDirty'),WebAssembly.Module.imports(normal));
assert(WebAssembly.Module.imports(tracked).some(i=>i.module==='env'&&i.name==='thDirty'&&i.kind==='memory'));
const visited=new Set();
function inspect(name){if(visited.has(name))return;visited.add(name);const source=read(name).toString();
 const syntax=spawnSync(process.execPath,['--input-type=module','--check'],{input:source,encoding:'utf8'});assert.equal(syntax.status,0,name+' '+syntax.stderr);
 for(const match of source.matchAll(/\b(?:from\s*|import\s*\(?\s*)['"](\.[^'"]+)['"]/g)){const child=path.posix.normalize(path.posix.join(path.posix.dirname(name),match[1]));assert(fs.existsSync(path.join(web,child)),child);if(/\.(js|mjs)$/.test(child))inspect(child);}
}
for(const name of ['app.js','solo-bootstrap.js','solo.js','solo-worker.js','netplay/simulation-worker.js','lan.js','launcher.js','vendor/dosbox-x/dosbox-rollback-tracked.mjs'])inspect(name);
const helper=new WebAssembly.Module(read('vendor/dosbox-x/snapshot-diff-tracked.wasm'));
for(const name of ['scan','packPages','restorePages'])assert(WebAssembly.Module.exports(helper).some(e=>e.name===name));
assert(read('dosbox-disk.js').includes(Buffer.from('16000')));
const report={date:build?'2026-10-15':'2026-10-14',scope:'static compilation, syntax, fingerprints and preserved local execution/gameplay resources',gameplayRun:false,browserRun:false,networkRun:false,networkResources:Object.keys(network.files).length,engineResources:Object.keys(runtime.files).length,modulesParsed:visited.size,localExecutionAndGameplayResourcesUnchanged:true,promotedNativeCore:!!build,snapshotScan:runtime.network.snapshotScan,writeJournal:runtime.network.writeJournal};
fs.mkdirSync(path.join(root,'reports'),{recursive:true});fs.writeFileSync(path.join(root,build?'reports/th03-main-release-static-20261015.json':'reports/dos-write-journal-static-20261014.json'),JSON.stringify(report,null,2)+'\n');console.log(JSON.stringify(report,null,2));
