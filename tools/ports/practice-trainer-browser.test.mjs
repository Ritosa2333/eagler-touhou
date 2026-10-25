import test from 'node:test';
import assert from 'node:assert/strict';
import {resolveAdaptationAdapter} from '../../tests/adaptation/adapters/registry.mjs';
import {runPracticeTrainerBrowser} from './practice-trainer-browser.mjs';

test('Trainer sends only serializable ABI data, preserves the physical menu path and releases its clock on a read failure',async()=>{
  const adapter=resolveAdaptationAdapter('th08','normal'),trial=adapter.practice.trainer.cases[1],calls=[],requests=[];
  const frame={locator:()=>({focus:async()=>{}}),async evaluate(fn,args){
    structuredClone(args);requests.push(args);
    if(fn.name==='controlTouchMotionClock'){calls.push(args.action);return{mode:'native-manual'};}
    if(fn.name==='readNativeSnapshotRaw')throw Error('actual reader boundary');
    throw Error('Unexpected browser operation');
  }};
  await assert.rejects(runPracticeTrainerBrowser({page:{keyboard:{up:async()=>{}}},frame,target:{game:'th08',variant:'normal'},adapter,trial,arm:{},
    press:async key=>calls.push(key),save:async()=>{}}),/actual reader boundary/);
  assert.deepEqual(calls.slice(0,4),['Backspace','F1','Backspace','begin']);assert.equal(calls.at(-1),'end');
  const request=requests.find(r=>r.snapshot);assert.equal(request.snapshot.export,'practice_effects_snapshot_v1');
  assert.equal(request.snapshot.decode,undefined);
});
