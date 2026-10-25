import test from 'node:test';
import assert from 'node:assert/strict';
import {decodeViewOptions,assertViewOptionsScene,assertViewOptionsRestart} from './view-options-scene.mjs';
function fixture(){
  const identity={url:'https://example.test/runtime?runtimeEpoch=1',timeOrigin:1,app:64};
  const sample=(frame,x,{always=false,focused=false,input=0}={})=>{
    const w=Array(32).fill(0);w[0]=w[1]=1;w[2]=w[3]=+always;w[4]=+focused;w[5]=w[6]=+(always||focused);
    w[8]=22;w[9]=5;w[10]=255;w[11]=w[13]=x;w[12]=w[14]=100;w[15]=input;w[16]=w[17]=1.2;w[18]=w[19]=30;w[20]=w[21]=8;w[22]=frame;
    return{raw:{available:true,words:w},view:decodeViewOptions({available:true,words:w}),identity:{...identity},
      scene:{available:true,accepted:true,armStatus:2,requestGeneration:1,requestPending:false,acceptedBeforeFirstUpdate:true,firstTimelineWithheld:true,
        updateStarted:true,sessionWriteLocked:true,writeReceipt:{locked:true,loadScope:false,attempts:1,ramOverrides:1,discardedBytes:64,committedBytes:0,rejectedWrites:0},
        writePolicy:{initialization:'ram-score-overlay'},active:true,eligible:true,ready:true,busy:false,replay:false,assisted:true,enemies:0,bullets:0,lasers:0,
        timelineCount:2,stage:0,controlFrame:frame+30,enemyFrame:frame,withheldTimelineUpdates:frame}};
  };
  const move=(frame,x,delta,options={})=>({before:sample(frame,x,options),held:sample(frame+6,x+delta,{...options,input:(options.input??0)|128}),
    released:sample(frame+9,x+delta+4,options),settled:sample(frame+11,x+delta+4,options)});
  const e={schema:'eagler-touhou/view-options-scene/1',arm:{available:true,armAccepted:true,armStatus:1,requestPending:true,requestGeneration:1},clock:{mode:'native-manual'},
    cleanupErrors:[],before:sample(400,80),fast:move(400,80,20),focused:sample(423,104,{focused:true,input:4}),slow:move(423,104,10,{focused:true,input:4}),
    unfocused:sample(446,118),enabled:sample(458,118,{always:true}),enabledFast:move(458,118,20,{always:true}),final:sample(526,142),
    optionChanges:[false,true,false].map((value,index)=>({value,source:'Launcher #alwaysHitboxToggle',checkedBeforeLaunch:value,epoch:index+1,
      arm:{available:true,armAccepted:true,armStatus:1,requestPending:true,requestGeneration:1},clock:{mode:'native-manual'}})),screenshots:['disabled','focused','enabled'].map((name,index)=>({name,
      beforeFrame:400+index,afterFrame:400+index,bytes:101,sha256:'a'.repeat(64)}))};
  const state=(frame,paused=false)=>({capabilities:{restartFrame:'simulation'},pause:{active:paused},frames:{simulation:frame}});
  for(const row of [e.enabled,...Object.values(e.enabledFast)])row.identity={url:'https://example.test/runtime?runtimeEpoch=2',timeOrigin:2,app:65};
  e.final.identity={url:'https://example.test/runtime?runtimeEpoch=3',timeOrigin:3,app:66};
  e.restart={before:state(800),paused:state(801,true),frozen:state(801,true),reset:state(1),advanced:state(2),identity:{...e.final.identity}};return e;
}
test('view options compare real normal/focused markers, same-speed input, collision dimensions and native restart',()=>{
  const e=fixture();assertViewOptionsScene(e);assertViewOptionsRestart(e);
});
for(const [name,mutate] of Object.entries({
  'display changes native movement':e=>{e.enabledFast.held.view.x++;e.enabledFast.held.raw.words[13]++;},
  'always marker remains hidden':e=>{e.enabled.view.marker.visible=false;e.enabled.raw.words[6]=0;},
  'always marker is a different native sprite':e=>{e.enabled.view.marker.sprite++;e.enabled.raw.words[9]++;},
  'always display forces slow mode':e=>{e.enabled.view.focused=true;e.enabled.raw.words[4]=1;},
  'collision box changed':e=>{e.enabled.view.halfBoxes[0][0]++;e.enabled.raw.words[16]++;},
  'native marker is offset from the player':e=>{e.enabled.view.marker.x++;e.enabled.raw.words[11]++;},
  'release leaves native movement':e=>{e.enabledFast.settled.view.x++;e.enabledFast.settled.raw.words[13]++;},
  'derived fields replace raw state':e=>e.enabled.view.always=false,
  'old owner supplies an input observation':e=>e.fast.settled.identity.app++,
  'unrelated controlled world was substituted':e=>e.before.scene.requestGeneration++,
  'screenshot spans different logic frames':e=>e.screenshots[0].afterFrame++,
}))test(`view options reject ${name}`,()=>{const e=fixture();mutate(e);assert.throws(()=>assertViewOptionsScene(e));});
for(const [name,mutate] of Object.entries({
  'pause continues simulation':e=>e.restart.frozen.frames.simulation++,
  'restart does not rebuild the world':e=>e.restart.reset.frames.simulation=800,
  'restart never resumes':e=>e.restart.advanced.frames.simulation=1,
  'restart replaces the application':e=>e.restart.identity.app++,
}))test(`view restart rejects ${name}`,()=>{const e=fixture();mutate(e);assert.throws(()=>assertViewOptionsRestart(e));});
