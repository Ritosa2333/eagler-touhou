import test from 'node:test';
import assert from 'node:assert/strict';
import {createMultiplayerRuntime} from '../src/multiplayer.mjs';

test('returning during stage preparation fences the pending multiplayer connection and start',async()=>{
  let finish,entered;
  const gate=new Promise(resolve=>{finish=resolve;}),loading=new Promise(resolve=>{entered=resolve;});
  const calls={configure:0,connect:0,start:0,stop:0},heap=new Uint8Array(8192);
  const core={HEAPU8:heap,_malloc:()=>256,_free(){},_th12_mp_stop(){calls.stop++;},
    _th12_mp_configure(){calls.configure++;return 1;},_th12_mp_connect(){calls.connect++;return 1;},_th12_mp_start(){calls.start++;return 1;}};
  const options={netplayMode:'lan',netplayUrl:'ws://localhost/?room=test&run=1',netplayPlayerCount:2,
    netplayPlayer:0,netplaySeed:1,netplayDifficulty:1,netplayLoadouts:[{character:0,shot:0},{character:1,shot:0}],netplayAdonisMode:1,netplayPredictionLimit:0};
  const runtime=createMultiplayerRuntime({core,manifest:{game:'th12',product:'th12mp',variant:'multiplayer',profile:'multiplayer',features:{multiplayer:true},execution:{sha256:'1'.repeat(64)}},getOptions:()=>options,emit(){},
    loadStage:async()=>{entered();await gate;},onError:error=>{throw error;}});
  const launch=runtime.launch();await loading;runtime.stop();finish();
  await assert.rejects(launch,/已取消/);
  assert.equal(runtime.active,false);
  assert.deepEqual(calls,{configure:0,connect:0,start:0,stop:1});
});
