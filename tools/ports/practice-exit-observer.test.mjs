import test from 'node:test';
import assert from 'node:assert/strict';
import {installPracticeExitObserver,releasePracticeExitObserver} from './practice-exit-observer.mjs';
function fixture(run){
  const memory=new ArrayBuffer(4096);let calls=0,invalid=0,faults=0;
  const original=function(result,duration){calls++;assert.equal(this,module);return result+duration;},module={runtimeFinish:original},
    core={memory:{buffer:memory},status:(app,k)=>{assert.equal(app,64);return k===2?invalid:k===4?faults:k;},
      practice_snapshot_v1:()=>256,gameplay_initial_snapshot_v1:()=>1536};
  globalThis.__practiceExitUnit={app:64,module,core};
  try{run({module,original,core,setFaults:(i,f)=>{invalid=i;faults=f;},calls:()=>calls});}
  finally{if(globalThis.__eaglerPracticeExitObserver){module.runtimeFinish=globalThis.__eaglerPracticeExitObserver.wrapper;releasePracticeExitObserver();}delete globalThis.__practiceExitUnit;}
}
const arm=()=>installPracticeExitObserver({runtime:{global:'__practiceExitUnit',module:'module',core:'core',app:'app'}});
test('completion observer preserves callback owner, arguments and return while recording before teardown',()=>fixture(({module,original,calls})=>{
  assert.equal(arm().armed,true);assert.equal(module.runtimeFinish(1,5),6);const raw=releasePracticeExitObserver();
  assert.equal(calls(),1);assert.equal(raw.entries.length,1);assert.equal(raw.entries[0].result,1);assert.equal(raw.entries[0].practice.words.length,128);
  assert.equal(raw.entries[0].initial.words.length,80);assert.equal(raw.restored,true);assert.equal(module.runtimeFinish,original);
}));
test('successful ticks create no exit observations and native fault masks remain independently visible',()=>fixture(({module,setFaults})=>{
  arm();assert.equal(module.runtimeFinish(0,5),5);setFaults(0,8);module.runtimeFinish(0,2);const raw=releasePracticeExitObserver();
  assert.equal(raw.entries.length,1);assert.equal(raw.entries[0].faults,8);assert.equal(raw.entries[0].invalid,0);
}));
test('observation failures preserve production behavior and remain explicit evidence',()=>fixture(({module,core,calls})=>{
  arm();core.practice_snapshot_v1=()=>4090;assert.equal(module.runtimeFinish(1,5),6);const raw=releasePracticeExitObserver();
  assert.equal(calls(),1);assert.match(raw.entries[0].captureError,/outside native memory/);
}));
test('completion cleanup refuses to overwrite a replaced callback owner',()=>fixture(({module})=>{
  arm();module.runtimeFinish=()=>0;assert.throws(releasePracticeExitObserver,/owner changed/);
}));
