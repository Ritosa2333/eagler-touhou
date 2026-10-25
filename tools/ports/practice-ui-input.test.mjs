import test from 'node:test';
import assert from 'node:assert/strict';
import {createPracticeUiInput,practiceUiVirtualKey} from './practice-ui-input.mjs';
import {resolveAdaptationAdapter} from '../../tests/adaptation/adapters/registry.mjs';

function fixture(){
  let reads=0,held=0;const events=[],adapter=resolveAdaptationAdapter('th08','normal');
  const native=Array(128).fill(0);native[0]=native[1]=native[2]=native[3]=1;native[10]=11;native[19]=2;native[42]=1;
  const frame={locator:()=>({focus:async()=>{}}),evaluate:async(_fn,request)=>{
    if(!request.snapshot)return{app:123,documentTimeOrigin:100,url:'https://fixture.invalid/runtime'};
    if(request.snapshot.export!=='practice_ui_snapshot_v1')return{available:true,words:[...native]};
    reads++;const words=Array(128).fill(0);words[0]=words[1]=words[9]=words[39]=1;words[2]=reads;words[3]=reads===1?0:reads;
    words[104]=reads>=3?1:0;words[105]=reads===3?1:0;words[106]=held?1:0;words[110]=0x50554b31;words[111]=1;words[114]=held;
    return{available:true,words};
  }},page={waitForTimeout:async()=>{},keyboard:{down:async code=>{events.push(['down',code]);held=2**26;},up:async code=>{events.push(['up',code]);held=0;}}};
  return{io:createPracticeUiInput({page,frame,adapter}),events};
}
test('Practice readiness requires an actual completed native frame, released keys, arming and an idle widget',async()=>{
  const {io}=fixture(),ready=await io.waitForPracticeInput();assert.ok(ready.ui.inputGeneration>=4);
  assert.equal(ready.ui.practiceInput.armed,true);assert.equal(ready.ui.practiceInput.widgetBusy,false);assert.equal(io.evidence.readiness.length,1);
});
test('Practice key acknowledgement retains native bitmaps and the confirmed release',async()=>{
  const {io,events}=fixture();await io.press('KeyZ');assert.deepEqual(events,[['down','KeyZ'],['up','KeyZ']]);
  assert.equal(io.evidence.keys[0].samples.at(-1).ui.keyboard.down[2],2**26);
  assert.equal(io.evidence.keys[0].release.ui.keyboard.down[2],0);
});
test('portable UI key codes cover function keys and refuse undeclared chords',()=>{
  assert.equal(practiceUiVirtualKey('F1'),112);assert.equal(practiceUiVirtualKey('F12'),123);assert.equal(practiceUiVirtualKey('KeyU'),85);
  assert.equal(practiceUiVirtualKey('ArrowRight'),39);assert.throws(()=>practiceUiVirtualKey('Shift+KeyF'));
});
