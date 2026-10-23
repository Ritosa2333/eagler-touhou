import assert from 'node:assert/strict';
import {mkdtemp,readFile,rm,writeFile} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {dirname,join,resolve} from 'node:path';
import {pathToFileURL,fileURLToPath} from 'node:url';
import test from 'node:test';

const testDirectory=dirname(fileURLToPath(import.meta.url));
const packageRoot=resolve(testDirectory,'../../..');
const workspaceRoot=resolve(packageRoot,'../..');
const productionBridgePath=resolve(packageRoot,'web/sdl-runtime/multiplayer.mjs');
const calibrationPath=resolve(workspaceRoot,'common/browser/adonis-calibration.mjs');

async function installPackageModule(source){
 const directory=await mkdtemp(join(tmpdir(),'eagler-th15mp-disconnect-'));
 await Promise.all([
  writeFile(join(directory,'multiplayer.mjs'),source),
  readFile(calibrationPath).then(bytes=>writeFile(join(directory,'adonis-calibration.mjs'),bytes)),
 ]);
 const loaded=await import(pathToFileURL(join(directory,'multiplayer.mjs')).href+`?fixture=${Math.random()}`);
 return{createMultiplayerBridge:loaded.createMultiplayerBridge,async close(){await rm(directory,{recursive:true,force:true});}};
}

function legacyDisconnectSource(source){
 const startMarker='pause:()=>{';
 const endMarker='},target});';
 const start=source.indexOf(startMarker),end=source.indexOf(endMarker,start);
 assert.ok(start>=0&&end>start,'Production multiplayer pause callback shape changed; update the legacy fault injection deliberately');
 assert.equal(source.indexOf(startMarker,start+1),-1,'Expected exactly one production pause callback');
 const legacy='pause:()=>{if(core._th15_phase()===0||getOptions().netplaySpectator)return;core._th15_loop_stop();onExit();}';
 return source.slice(0,start)+legacy+source.slice(end+1);
}

function createHarness(createMultiplayerBridge,{pumpResults=[0],phase=1}={}){
 const buffer=new ArrayBuffer(2048),HEAPU8=new Uint8Array(buffer),calibration=new Uint32Array(buffer,0,32);
 calibration[1]=5; // Native calibration is ready before the peer disconnects.
 calibration[4]=2;calibration[5]=0;calibration[6]=1;calibration[8]=0;calibration[11]=1;
 calibration[12]=12000;calibration[13]=129;calibration[18]=14000;calibration[19]=129;
 const nativeError='TH15 input connection ended';HEAPU8.set(new TextEncoder().encode(nativeError+'\0'),512);
 const transport={route:'relay',disconnected:false,error:'Relay peer P2 disconnected',onReceive:null,onDisconnect:null};
 const target={document:{hidden:false},__eaglerPeerTransport:transport};
 const events=[],errors=[],exits=[];let loopStops=0,pumpCalls=0;
 const core={HEAPU8,memory:{get buffer(){return HEAPU8.buffer;}},
  _malloc(){return 128;},_free(){},_th15_mp_error(){return 512;},
  _th15_mp_configure(){return 1;},_th15_mp_connect(){return 1;},_th15_mp_start(){return 1;},
  _th15_mp_calibration_status(){return 0;},_th15_mp_pump(){pumpCalls++;return pumpResults.shift()??0;},
  _th15_phase(){return phase;},_th15_loop_stop(){loopStops++;},_th15_mp_close(){},
  _th15_mp_status(){return 1024;},
 };
 const getOptions=()=>({netplayMode:'lan',netplayUrl:'ws://127.0.0.1:18142/netplay?room=th15mp-test&run=run1',
  netplayPlayer:0,netplayPlayerCount:2,netplaySpectator:false,netplayDifficulty:1,netplaySeed:42,
  netplayInputDelay:0,netplayInputDelayAuto:false,netplayAdonisMode:1,netplayPredictionReserve:2,
  netplayPredictionLimit:0,netplayLoadouts:[{character:0,shot:0},{character:1,shot:0}]});
 const bridge=createMultiplayerBridge({core,getOptions,
  manifest:{game:'th15',product:'th15mp',variant:'multiplayer',features:{multiplayer:true},execution:{sha256:'a'.repeat(64)}},
  emit:(event,payload)=>events.push({event,payload}),onError:error=>errors.push(error.message),
  onExit:result=>exits.push(result),target});
 return{bridge,transport,target,events,errors,exits,
  get loopStops(){return loopStops;},get pumpCalls(){return pumpCalls;}};
}

test('legacy production pause behavior is a success-exit negative control through the real calibration callback',async()=>{
 const source=await readFile(productionBridgePath,'utf8');
 const installed=await installPackageModule(legacyDisconnectSource(source));
 try{
  const harness=createHarness(installed.createMultiplayerBridge);
  await harness.bridge.launch();
  assert.equal(harness.events.some(value=>value.payload?.netplayTiming?.phase==='ready'),true);
  const notify=harness.transport.onDisconnect;
  assert.equal(typeof notify,'function','The packaged shared calibration must register the transport callback');
  harness.transport.disconnected=true;notify();
  assert.deepEqual(harness.errors,[]);
  assert.deepEqual(harness.exits,[undefined],'The old bridge reports success directly from onDisconnect after calibration is ready');
  assert.equal(harness.loopStops,1);
 }finally{await installed.close();}
});

test('active disconnect freezes on native error and a captured callback cannot report twice',async()=>{
 const installed=await installPackageModule(await readFile(productionBridgePath,'utf8'));
 try{
  const harness=createHarness(installed.createMultiplayerBridge,{pumpResults:[0,0]});
  await harness.bridge.launch();
  assert.equal(harness.events.some(value=>value.payload?.netplayTiming?.phase==='ready'),true);
  const notify=harness.transport.onDisconnect;
  assert.equal(typeof notify,'function');
  harness.transport.disconnected=true;notify();notify();
  assert.deepEqual(harness.errors,['TH15 input connection ended']);
  assert.deepEqual(harness.exits,[]);
  assert.equal(harness.loopStops,1);
  assert.equal(harness.target.__eaglerNetplayFailed,true);
  assert.equal(harness.target.__eaglerNetplayError,'TH15 input connection ended');
  assert.equal(harness.transport.onDisconnect,null);
 }finally{await installed.close();}
});

test('native-retired completion stays successful when a captured callback sees the same retired result',async()=>{
 const installed=await installPackageModule(await readFile(productionBridgePath,'utf8'));
 try{
  const harness=createHarness(installed.createMultiplayerBridge,{pumpResults:[1,1],phase:4});
  await harness.bridge.launch();
  const notify=harness.transport.onDisconnect;
  assert.equal(typeof notify,'function');
  harness.transport.disconnected=true;notify();notify();
  assert.deepEqual(harness.errors,[]);
  assert.deepEqual(harness.exits,[],'The shell observes native phase 4 and owns the normal exit');
  assert.equal(harness.loopStops,0);
  assert.equal(harness.pumpCalls,2,'A stable native-retired result stays healthy across repeated observation');
  assert.equal(harness.target.__eaglerNetplayFailed,false);
  assert.equal(harness.transport.onDisconnect,null);
 }finally{await installed.close();}
});
