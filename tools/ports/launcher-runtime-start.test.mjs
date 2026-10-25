import test from 'node:test';
import assert from 'node:assert/strict';
import {runLauncherRuntimeStart} from './launcher-runtime-start.mjs';
const ready=(epoch=3)=>({url:'https://example.test/runtime?runtimeEpoch='+epoch,epoch,errors:[],prompt:{open:false},
  ready:{protocol:'eagler-touhou/1',game:'th08',epoch,event:'ready'},first:{protocol:'eagler-touhou/1',game:'th08',epoch,event:'first-frame',presentation:{nativePresentCount:2}}});
function io(samples){let index=0,time=0;const calls={launch:0,confirm:0,closed:0};return{calls,now:()=>time,launch:async()=>calls.launch++,
  read:async()=>structuredClone(samples[Math.min(index++,samples.length-1)]),confirm:async()=>calls.confirm++,waitPromptClosed:async()=>calls.closed++,wait:async ms=>{time+=ms;}};}
test('launch flow ignores old epochs and accepts only a fresh native first frame',async()=>{const driver=io([ready(1),ready(3)]),e=await runLauncherRuntimeStart(driver,{game:'th08',previousEpoch:1});
  assert.equal(driver.calls.launch,1);assert.equal(e.runtime.epoch,3);assert.equal(e.transitions.length,2);assert.deepEqual(e.consents,[]);});
test('one known music choice is confirmed and fully closed before first-frame acceptance',async()=>{const prompt={...ready(3),ready:null,first:null,
  prompt:{open:true,message:'将会在无音乐的情况下继续。 如果想听正常的音乐，请在音乐中选择 OGG。',confirm:'仍要启动'}};const driver=io([prompt,ready(3)]);
  const e=await runLauncherRuntimeStart(driver,{game:'th08',previousEpoch:1});
  assert.equal(e.consents.length,1);assert.equal(driver.calls.confirm,1);assert.equal(driver.calls.closed,1);assert.equal(e.runtime.epoch,3);
});
for(const [name,mutate] of Object.entries({
  'native error':s=>s.errors=[{event:'error'}],
  'wrong game':s=>s.first.game='th10','wrong first-frame epoch':s=>s.first.epoch=1,
  'no native present':s=>s.first.presentation.nativePresentCount=0,
  'fractional native present':s=>s.first.presentation.nativePresentCount=.5,
  'import required':s=>s.blockingUi={importRequired:true,importReason:'missing required font'},
  'startup panel':s=>s.blockingUi={startupError:'native initialization failed'},
  'unknown decision':s=>s.prompt={open:true,message:'delete all data',confirm:'yes'},
}))test(`launch flow rejects ${name}`,async()=>{const sample=ready();mutate(sample);await assert.rejects(()=>runLauncherRuntimeStart(io([sample]),{game:'th08',previousEpoch:1}));});
test('an old first frame cannot satisfy a new launch when the deadline expires',async()=>assert.rejects(()=>runLauncherRuntimeStart(io([ready(1)]),{game:'th08',previousEpoch:1,timeoutMs:200}),/deadline/));
