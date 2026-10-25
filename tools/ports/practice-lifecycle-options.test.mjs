import test from 'node:test';
import assert from 'node:assert/strict';
import {TH08_PRACTICE_OPTIONS as options} from '../../tests/adaptation/adapters/titles/th08-practice-options.mjs';
import {decodeNativeControls} from '../../tests/adaptation/adapters/native-controls.mjs';
import {assertPracticeLifecycleOption} from './practice-lifecycle-options.mjs';

function row(frame,changes={}){
  const words=Array(1024).fill(0);words.splice(0,8,1,1,frame+100,frame+100,0,1,0,0);
  words[16]=1;words[17]=words[18]=frame;words[22]=3;words[24]=4;words[36]=1;Object.assign(words,changes);
  const ui=Array(128).fill(0);ui[0]=ui[1]=1;ui[2]=ui[3]=frame+100;ui[110]=0x50554b31;ui[111]=1;
  return{raw:{available:true,words},uiRaw:{available:true,words:ui},identity:{app:123,documentTimeOrigin:45,url:'https://localhost/runtime'},
    scene:{available:true,accepted:true,acceptedBeforeFirstUpdate:true,firstTimelineWithheld:true,updateStarted:true,sessionWriteLocked:true,
      active:true,ready:true,replay:false,assisted:true,writePolicy:{initialization:'ram-score-overlay'},
      writeReceipt:{locked:true,loadScope:false,attempts:1,ramOverrides:1,discardedBytes:64,committedBytes:0,rejectedWrites:0},
      timelineCount:1,withheldTimelineUpdates:frame,enemies:0,bullets:0,lasers:0,stage:0,enemyFrame:frame,controlFrame:frame,requestGeneration:1},
    runtime:{pause:{active:false},worldProgress:{comparable:true}}};
}
function widget(sample,enabled,activations=0){
  const w=sample.raw.words;w[4]=1;w[6]=1;w.splice(128,12,11,987,20,20,100,20,0,1,Number(enabled),w[2],activations,0);
  sample.state=decodeNativeControls(sample.raw);return sample;
}
function fixture(kind){
  const e={schema:'eagler-touhou/practice-lifecycle-option/1',kind,arm:{available:true,armAccepted:true,armStatus:1,requestPending:true,requestGeneration:1},cleanupErrors:[],before:row(300)};
  if(kind==='fast-retry'){
    e.hit={accepted:true};e.samples=[row(301,{21:2,52:15}),row(302,{52:14}),row(303,{52:1}),row(80)];
    e.samples[1].runtime.pause.active=e.samples[2].runtime.pause.active=true;e.samples.at(-1).scene.active=false;
  }else{
    e.disabledTitle={kind:'disabled-click',semantic:11,before:widget(row(1,{16:0,22:0,24:0}),false),after:widget(row(4,{16:0,22:0,24:0}),false)};
    e.activated={kind:'click',semantic:11,before:widget(row(301),true),after:widget(row(303,{21:2,22:0,24:0}),true,1)};
    e.samples=[row(304,{21:2,22:0,24:0}),row(305,{21:2,22:0,24:0}),row(332,{21:1,22:0,24:0,29:1})];
  }return e;
}
for(const kind of ['fast-retry','one-key-death'])test(kind+' requires actual lifecycle consumers and excludes physical menu-key assistance',()=>{
  const e=fixture(kind);assert.equal(assertPracticeLifecycleOption(e,options,kind).complete,true);
  for(const change of [e=>e.samples[0].identity.app++,e=>e.samples[0].scene.writeReceipt.committedBytes=1,
    e=>{e.samples[0].uiRaw.words[112]=1<<27;e.samples[0].uiRaw.words[107]=1;},e=>e.arm.armAccepted=false]){
    const bad=structuredClone(e);change(bad);assert.throws(()=>assertPracticeLifecycleOption(bad,options,kind));
  }
  const bad=fixture(kind);
  if(kind==='fast-retry')bad.samples.at(-1).raw.words[17]=350;else bad.activated.after.raw.words[24]=4;
  assert.throws(()=>assertPracticeLifecycleOption(bad,options,kind));
});
