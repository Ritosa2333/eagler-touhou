import test from 'node:test';
import assert from 'node:assert/strict';
import {assertPracticeControlScene,practiceControlSamples} from './practice-ui-scene.mjs';
const keys=[...Array.from({length:6},(_,i)=>({key:'F'+(i+1),kind:'cheat-mask',mask:1<<i})),{key:'F7',kind:'everlasting-bgm'},{key:'KeyU',kind:'cheat-mask',mask:64}];
function fixture(){
  let frame=100;
  const scene=()=>({available:true,accepted:true,armStatus:2,requestGeneration:1,requestPending:false,
    acceptedBeforeFirstUpdate:true,firstTimelineWithheld:true,updateStarted:true,sessionWriteLocked:true,
    writeReceipt:{locked:true,loadScope:false,attempts:1,ramOverrides:1,discardedBytes:64,committedBytes:0,rejectedWrites:0},
    writePolicy:{initialization:'ram-score-overlay'},active:true,eligible:true,ready:true,busy:false,replay:false,assisted:true,
    enemies:0,bullets:0,lasers:0,timelineCount:1,stage:0,controlFrame:frame++,enemyFrame:frame,withheldTimelineUpdates:frame});
  const panel=expanded=>({exists:true,active:true,hidden:false,vertexCount:expanded?200:10,expanded,rect:{x:8,y:8,width:150,height:28}});
  const sample=(cheats=0,bgm=false)=>({scene:scene(),identity:{url:'https://example.test/runtime?runtimeEpoch=1',timeOrigin:10,app:64},
    practice:{applied:{cheats,everlastingBgm:bgm}},held:Object.fromEntries([...keys.map(row=>row.key),'Backspace','Tab','F12'].map(key=>[key,false])),
    ui:{inputGeneration:1,renderedGeneration:1,menuOpen:true,trackerOpen:false,advancedOpen:false,mobileEnabled:true,mobileVisible:true,
      pointerDown:false,pendingPointerEdges:0,panels:{cheats:panel(true),tracker:panel(false),advanced:panel(false)},
      items:keys.map((row,index)=>({key:row.key,value:row.kind==='cheat-mask'?Boolean(cheats&row.mask):bgm,
        id:index+1,generation:1,rect:{x:16,y:50+index*22,width:120,height:20}}))}});
  const e={schema:'eagler-touhou/practice-control-scene/1',before:sample(),menuOpened:sample(),mobileOpened:sample(),cheatsExpanded:sample(),trials:[],keyboardPanels:{},mobilePanels:{},final:sample(),cleanupErrors:[]};
  e.before.ui.menuOpen=false;e.mobileOpened.ui.panels.cheats.expanded=false;
  for(const [index,key] of keys.entries())e.trials.push({key:key.key,before:sample(),keyboard:sample(key.mask??0,key.kind==='everlasting-bgm'),pointer:sample(),
    touch:{source:'browser-touch',logical:{x:76,y:60+index*22},down:{isTrusted:true},up:{isTrusted:true}}});
  for(const trial of e.trials){const t=trial.touch;t.canvas={x:100,y:200,width:640,height:480};t.frameBox={...t.canvas};t.frameViewport={width:640,height:480};
    t.rect=structuredClone(trial.keyboard.ui.items.find(item=>item.key===trial.key).rect);
    t.requestedPage={x:100+t.logical.x,y:200+t.logical.y};
    for(const [edge,type] of [['down','pointerdown'],['up','pointerup']])t[edge]={type,pointerType:'touch',pointerId:1,isTrusted:true,
      clientX:t.logical.x,clientY:t.logical.y,realm:'runtime',url:trial.keyboard.identity.url,timeOrigin:trial.keyboard.identity.timeOrigin};
    t.journals=[{events:[],drops:0},{events:[structuredClone(t.down),structuredClone(t.up)],drops:0}];}
  for(const [name,field] of [['tracker','trackerOpen'],['advanced','advancedOpen']]){
    e.keyboardPanels[name]={before:sample(),opened:sample(),closed:sample()};e.keyboardPanels[name].opened.ui[field]=true;
    e.mobilePanels[name]=sample();e.mobilePanels[name].ui.panels[name]=panel(true);
  }
  e.arm={available:true,armAccepted:true,armStatus:1,requestPending:true,requestGeneration:1};
  for(const row of practiceControlSamples(e))row.scene=scene();
  e.final.ui.menuOpen=false;e.final.ui.mobileVisible=false;return e;
}
test('shared Practice controls require all declared native keys and reversible actual touch effects',()=>assertPracticeControlScene(fixture(),keys));
for(const [name,mutate] of Object.entries({
  'one key omitted':e=>e.trials.pop(),'key callback disconnected':e=>e.trials[0].keyboard.practice.applied.cheats=0,
  'wrong unrelated cheat changed':e=>e.trials[0].keyboard.practice.applied.cheats=65,
  'touch did not restore native value':e=>e.trials[0].pointer.practice.applied.cheats=1,
  'UI value differs from native state':e=>e.trials[0].keyboard.ui.items[0].value=false,
  'synthetic touch substituted':e=>e.trials[0].touch.down.isTrusted=false,
  'wrong native widget coordinates':e=>e.trials[0].touch.logical.x=1,
  'stale widget geometry':e=>e.trials[0].keyboard.ui.items[0].generation=0,
  'function key remains held':e=>e.trials[0].keyboard.held.F1=true,
  'pointer remains held':e=>e.trials[0].pointer.ui.pointerDown=true,
  'Tracker owner did not open':e=>e.keyboardPanels.tracker.opened.ui.trackerOpen=false,
  'Advanced panel did not expand':e=>e.mobilePanels.advanced.ui.panels.advanced.expanded=false,
  'no actual advanced panel draw':e=>e.mobilePanels.advanced.ui.panels.advanced.vertexCount=0,
  'native panel still hidden for layout':e=>e.mobileOpened.ui.panels.cheats.hidden=true,
  'new application replaced the tested owner':e=>e.final.identity.app=65,
  'scene was not armed before navigation':e=>e.arm.armAccepted=false,
  'live hazards entered the widget scene':e=>e.trials[0].pointer.scene.bullets=1,
  'a later scene replaced the accepted world':e=>e.final.scene.requestGeneration=2,
  'scene clock moved backwards':e=>e.final.scene.controlFrame=0,
  'controlled run committed user data':e=>e.final.scene.writeReceipt.committedBytes=1,
  'requested page point does not match the canvas':e=>e.trials[0].touch.requestedPage.x++,
  'trusted event hit a different widget':e=>{const t=e.trials[0].touch;t.down.clientX+=20;t.journals[1].events[0].clientX+=20;},
  'touch event belongs to another document':e=>{const t=e.trials[0].touch;t.down.url+='old';t.journals[1].events[0].url+='old';},
  'journal overflow hid an extra input':e=>e.trials[0].touch.journals[0].drops=1,
}))test(`shared Practice controls reject ${name}`,()=>{const e=fixture();mutate(e);assert.throws(()=>assertPracticeControlScene(e,keys));});
