import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {initializeCandidate} from '../tools/replay-verifier/capture-candidate.mjs';
import {ReplayRecorder} from '../src/replay-recorder.mjs';
import {parseReplay} from '../src/replay.mjs';
import {startReplayStage} from '../src/replay-session.mjs';
const wasmBinary=fs.readFileSync(new URL('../site/runtime/core.wasm',import.meta.url));
const json=(c,name)=>{const p=c['_th12_'+name](),size=c['_th12_'+name+'_size']();return JSON.parse(new TextDecoder().decode(c.HEAPU8.subarray(p,p+size)));};
const h=c=>[...new Float32Array(c.HEAPU8.buffer,c._th12_hud(),c._th12_hud_size())];
test('full-game first-stage playing behavior matches legacy, and freshly recorded inputs replay exactly',async()=>{
  const full=await initializeCandidate(1,{wasmBinary}),legacy=await initializeCandidate(1,{wasmBinary});
  full._th12_start_full_game(0,0,1,1234,0);legacy._th12_start(0,0,1,1234);
  const flow=json(full,'stage_flow'),r=flow.recordHeader;
  const recorder=new ReplayRecorder({character:0,shot:0,difficulty:1,timestamp:1000000});
  recorder.recordStage({number:1,seed:r.seed,initial:{...r,score:r.scoreStored,pointValue:r.pointValueStored}});
  let previous=0;const checks=[];
  for(let frame=0;frame<900;frame++){
    const held=1|8|(frame%240<120?64:128),pressed=held&~previous;recorder.pushInput({held,pressed,released:previous&~held},60);previous=held;
    full._th12_tick(held,pressed);legacy._th12_tick(held,pressed);
    const a=h(full),b=h(legacy);assert.deepEqual(a,b,'legacy isolation at '+frame);if(frame%90===0)checks.push(a);
  }
  const replay=parseReplay(recorder.result({name:'TEST'})),candidate=await initializeCandidate(1,{wasmBinary});startReplayStage(candidate,replay,replay.stages[0]);candidate._th12_set_full_game_mode(0);
  for(let frame=0;frame<900;frame++){
    const i=replay.input(1,frame);candidate._th12_tick(i.held,i.pressed);
    if(frame%90===0)assert.deepEqual(h(candidate),checks[frame/90],'recorded full-game state at '+frame);
  }
});
test('native spell history uses128 cards by7 counters and validates bounds independently of game reset',async()=>{
  const c=await initializeCandidate(1,{wasmBinary}),words=new Int32Array(1792);words[70]=5;words[71]=3;
  const p=c._malloc(words.byteLength);try{new Int32Array(c.HEAPU8.buffer,p,1792).set(words);assert.equal(c._th12_load_spell_history(p,1792),1);assert.equal(c._th12_load_spell_history(p,1791),0);c._th12_start_full_game(0,0,1,1234,0);assert.deepEqual(json(c,'spell_history').records[5][0],[5,3]);}finally{c._free(p);}
});
