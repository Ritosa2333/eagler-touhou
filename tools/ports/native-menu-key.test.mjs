import test from 'node:test';
import assert from 'node:assert/strict';
import {pressObservedMenuKey} from './native-menu-key.mjs';
const options={code:'KeyX',mask:2,inputField:'input',transitionFields:['scene','state']};
function fixture(states){let now=0,index=0;const events=[];return{events,io:{now:()=>now,read:async()=>states[Math.min(index++,states.length-1)],
  down:async code=>events.push(['down',code]),up:async code=>events.push(['up',code]),wait:async ms=>{now+=ms;}}};}
test('slow rendering keeps one physical edge held until the native menu actually samples it',async()=>{
  const neutral={scene:5,state:20,input:0},f=fixture([neutral,neutral,neutral,{...neutral,input:2}]);
  const e=await pressObservedMenuKey(f.io,options);assert.equal(e.acknowledged,true);assert.equal(e.samples.length,3);assert.equal(e.released,true);
  assert.deepEqual(f.events,[['down','KeyX'],['up','KeyX']]);
});
test('a scene transition can retire the original input owner before it is read again',async()=>{
  const f=fixture([{scene:5,state:20,input:0},{scene:5,state:21,input:0}]);assert.equal((await pressObservedMenuKey(f.io,options)).acknowledged,true);
});
test('missing consumption fails within budget and releases the physical key without retries',async()=>{
  const f=fixture([{scene:5,state:20,input:0}]);await assert.rejects(pressObservedMenuKey(f.io,{...options,timeoutMs:40}),/native consumer/);
  assert.deepEqual(f.events,[['down','KeyX'],['up','KeyX']]);
});
test('an already held menu key is not presented as a fresh edge',async()=>{
  const f=fixture([{scene:5,state:20,input:2}]);await assert.rejects(pressObservedMenuKey(f.io,options),/already held/);assert.deepEqual(f.events,[]);
});
