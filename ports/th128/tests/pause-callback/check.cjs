'use strict';
const fs=require('node:fs'),path=require('node:path'),assert=require('node:assert/strict'),{pathToFileURL}=require('node:url');
const [playArg,soundArg,drawArg]=process.argv.slice(2),repo=path.resolve(__dirname,'../..');
const play=path.resolve(playArg||path.join(repo,'site/play.mjs')),soundPath=path.resolve(soundArg||path.join(repo,'site/runtime/sound.mjs')),drawPath=path.resolve(drawArg||path.join(repo,'site/runtime/draw.mjs'));
function exactFunction(file,startMarker,endMarker){const s=fs.readFileSync(file,'utf8'),start=s.indexOf(startMarker),end=s.indexOf(endMarker,start);assert(start>=0&&end>start);assert.equal(s.indexOf(startMarker,start+1),-1);return s.slice(start,end).trimEnd();}
const source=exactFunction(play,'function originalPauseConsumer(){','\nasync function openOriginalNativePause');
function deferred(){let resolve,reject;const promise=new Promise((a,b)=>{resolve=a;reject=b;});return{promise,resolve,reject};}
function barrier(){const entered=deferred(),held=deferred();return{entered:entered.promise,held:held.promise,mark:entered.resolve,release:held.resolve,reject:held.reject};}
const fnv=b=>{let h=0x811c9dc5;for(const v of b)h=Math.imul(h^v,0x01000193)>>>0;return h;};
const params=['state','menu','clock','resetHighFrameratePresentation','originalMenuClock','find','worldMovementInput','status','OwnedReplayStore','OwnedLastNameFieldStore','bindOriginalReplayPreviews','bindOriginalNameProfile','bindOriginalReplayNameContext','ordinaryWorldAudioLane','consumeMenuSoundWindow','OwnedMenuSoundLedger','releaseOriginalPauseInput','OriginalPauseController','MenuAudioGate','loadOriginalHelpPage','originalHelpPagePins','safeAssetUrl','prepareStage','startLaunch','recordOwnedMainSession','returnToTitle','returnOwnedReplayToTitle','restartOwnedReplay','window','Date'];
let sound;
function native(id,events,snapshot){const heap=Uint8Array.from({length:2048},(_,n)=>n&255),call=name=>events.push({kind:'native',id,name});return{id,HEAPU8:heap,UTF8ToString:x=>typeof x==='string'?x:'native-seam',
 _th128_world_pause_open:()=>{call('pause_open');return 1;},_th128_world_pause_tick:()=>{call('pause_tick');return 1;},
 _th128_world_pause_commit:()=>{call('pause_commit');return 1;},_th128_world_pause_state:()=>JSON.stringify(snapshot),
 _th128_world_error:()=>'',_th128_world_replay_snapshot:()=>{call('snapshot');return 300;},
 _th128_world_replay_data:()=>{call('data');return 100;},_th128_world_replay_checksum:()=>{call('checksum');return fnv(heap.slice(100,400));},
 _th128_world_replay_saved:()=>{call('saved');return 1;},_th128_world_replay_save_open:()=>{call('save_open');return 1;}};
}
async function environment(point){
 const events=[],gate=barrier(),phase3=['replay-save','name-save'].includes(point),manual=point==='help',restarting=point==='restart';
 const snapshot={active:true,ready:true,action:restarting?6:phase3||manual?0:4,replayPhase:phase3?3:0,replaySlot:1,replaySerial:7,replayName:[65,65,65,65,65,65,65,65,0],replayNameContextReady:true,manual,helpPage:3};
 const A=native('A',events,snapshot),selection={stage:{id:10},difficulty:1};
 const state={module:A,action:7,stageSelection:selection,nativePause:null,gameSounds:{context:{state:'running'},gain:{gain:{value:1}},channels:new Set(),retiredChannels:new Set(),disposed:false,stopChannel(){}},
 controller:new AbortController(),renderer:{id:'A'},base:'assets/',pressed:new Set(),worldMode:true,active:true,paused:true,replayPlayback:false,
 replaySlots:[{digest:'old0'},{digest:'old1'}],nameFieldProfile:{digest:'old-name'},manualPageLoaded:0};
 const menu={pauseOpen:true,page:'pause',fault:false},statuses=[];
 const wait=async tag=>{events.push({kind:'wait-entry',tag});if(tag===point){gate.mark();await gate.held;}};
 const oldSlots=[{digest:'observed0'},{digest:'observed1'}],oldName={digest:'observed-name'},savedName={digest:'saved-name'};
 const receipt={strictFullReadback:true,digest:'saved',bytes:300,rawBytes:300,rawChecksum:fnv(A.HEAPU8.slice(100,400))};
 const replayStore={async observe(){await wait('replay-observe');events.push({kind:'store-completed',tag:'replay-observe'});return oldSlots;},
 async save(){await wait('replay-save');events.push({kind:'store-completed',tag:'replay-save'});return receipt;}};
 const nameStore={async observe(){await wait('name-observe');return oldName;},async save(){await wait('name-save');events.push({kind:'store-completed',tag:'name-save'});return savedName;}};
 state.replayStore=replayStore;state.nameFieldStore=nameStore;
 class ReplayStore{static nativeChecksum(b){return fnv(b);}}
 class NameStore{} class Ledger{}
 const deps=[state,menu,{reset(){}},()=>{},{reset(){}},()=>({value:'zh',hidden:false,focus(){}}),()=>{},
 value=>{statuses.push(value);events.push({kind:'status',value});},ReplayStore,NameStore,
 ()=>events.push({kind:'bind',name:'previews'}),()=>events.push({kind:'bind',name:'name-profile'}),()=>events.push({kind:'bind',name:'name-context'}),
 async()=>{await wait('audio-lane');return point==='audio-lane'?{sound(){events.push({kind:'sound-after-lane'});}}:null;},
 async()=>events.push({kind:'sound-consume'}),Ledger,()=>events.push({kind:'release-input'}),sound.OriginalPauseController,sound.MenuAudioGate,
 async()=>{await wait('help');events.push({kind:'allowed-old-loader-completion'});},
 {zh:[{page:0,file:'0'},{page:1,file:'1'},{page:2,file:'2'},{page:3,file:'3'}]},(base,file)=>base+file,
 ()=>({stage:{id:6}}),async()=>events.push({kind:'launch'}),async()=>{await wait('restart');events.push({kind:'store-completed',tag:'restart'});},
 ()=>events.push({kind:'title'}),()=>events.push({kind:'replay-title'}),async()=>events.push({kind:'restart-replay'}),{__TH128_FAST_REPLAY__:false},Date];
 const consumer=new Function(...params,'return ('+source+');')(...deps),controller=consumer();
 controller.active=true;
 return{events,gate,state,menu,statuses,controller,A,oldSlots,oldName,savedName,receipt,snapshot,replayStore,nameStore};
}
function snapshotWrites(e){return{slots:e.state.replaySlots,slot1:e.state.replaySlots?.[1],name:e.state.nameFieldProfile,manual:e.state.manualPageLoaded,menuOpen:e.menu.pauseOpen};}
async function executeScenario(point,mode){
 const e=await environment(point);
 let settled=false;const pending=e.controller.step().then(()=>{settled=true;return{ok:true};},error=>{settled=true;return{ok:false,error};});
 let timer;await Promise.race([e.gate.entered,new Promise((_,reject)=>{timer=setTimeout(()=>reject(Error('barrier never entered '+point)),1000);})]);clearTimeout(timer);
 await Promise.resolve();await Promise.resolve();assert.equal(settled,false,'the awaited operation must still be held');
 if(mode==='dispose')await e.controller.dispose();
 if(mode==='replace'||mode==='reject-replace'){
  e.state.module=native('B',e.events,e.snapshot);e.state.nativePause={active:true};
  e.state.replaySlots=[{digest:'successor0'},{digest:'successor1'}];e.state.nameFieldProfile={digest:'successor-name'};e.state.manualPageLoaded=9;
  e.state.replayStore={async observe(){e.events.push({kind:'successor-store'});return[];},async save(){e.events.push({kind:'successor-store'});return e.receipt;}};
  e.state.nameFieldStore={async observe(){e.events.push({kind:'successor-name'});return{};},async save(){e.events.push({kind:'successor-name'});return{};}};
 }
 if(mode==='selection')e.state.stageSelection={stage:{id:1},difficulty:0};
 if(mode==='store')e.state.replayStore={};
 const boundary=e.events.length,before=snapshotWrites(e),statusCount=e.statuses.length;
 if(mode.startsWith('reject'))e.gate.reject(Error('expected-current-failure'));else e.gate.release();
 const outcome=await pending;
 if(mode==='reject-current'){assert.equal(outcome.ok,false);assert.match(outcome.error.message,/expected-current-failure/);return;}
 assert.equal(outcome.ok,true,outcome.error?.stack);
 if(mode==='current'){
  if(point==='replay-observe'||point==='name-observe'){assert.equal(e.state.replaySlots,e.oldSlots);assert.equal(e.state.nameFieldProfile,e.oldName);assert.equal(e.events.filter(v=>v.kind==='native'&&v.name==='save_open').length,1);}
  if(point==='replay-save'||point==='name-save'){assert.equal(e.state.nameFieldProfile,e.savedName);assert.equal(e.state.replaySlots[1].digest,'saved');assert.equal(e.events.filter(v=>v.kind==='native'&&v.name==='saved').length,1);}
  if(point==='help')assert.equal(e.state.manualPageLoaded,3);
  if(point==='restart')assert.equal(e.events.filter(v=>v.kind==='launch').length,1);
  if(point==='audio-lane')assert(e.events.some(v=>v.kind==='sound-after-lane'));
 }else{
  const late=e.events.slice(boundary).filter(v=>v.kind!=='store-completed'&&v.kind!=='allowed-old-loader-completion');
  assert.deepEqual(late,[],'retired callback made new native/store/bind/UI calls');
  assert.deepEqual(snapshotWrites(e),before,'retired callback changed successor state');assert.equal(e.statuses.length,statusCount);
 }
}
async function helpLoaderScenario(mode){
 const file=fs.readFileSync(drawPath,'utf8'),start=file.indexOf('export async function loadOriginalHelpPage('),end=file.indexOf('\nexport ',start+7);
 assert(start>=0&&end>start);const body=file.slice(start,end).replace(/^export /,'');
 const gate=barrier(),calls=[],textures=new Map();let current=true,closed=0;
 const gl={NO_ERROR:0,createTexture(){calls.push('createTexture');return{};},bindTexture(){calls.push('bindTexture');},texImage2D(){calls.push('texImage2D');},texSubImage2D(){calls.push('texSubImage2D');},texParameteri(){},getError(){return return0();},deleteTexture(){}};
 function return0(){return 0;}
 const renderer={gl,textures,disposed:false,releaseTexture(){calls.push('releaseTexture');}};
 const loader=new Function('fetch','createImageBitmap','Blob','crypto','DOMException','return ('+body+');')(
 async()=>({ok:true,async arrayBuffer(){return new Uint8Array([1,2,3]).buffer;}}),
 async()=>{gate.mark();await gate.held;return{close(){closed++;}};},Blob,globalThis.crypto,DOMException);
 const ac=new AbortController();const p=loader(renderer,{page:3,url:'owned-help',width:384,height:448},{signal:ac.signal,isCurrent:()=>current}).then(()=>({ok:true}),error=>({ok:false,error}));
 await gate.entered;
 if(mode==='retired')current=false;if(mode==='aborted')ac.abort();gate.release();const outcome=await p;
 assert.equal(closed,1,'bitmap must be closed exactly once');
 if(mode==='current'){assert.equal(outcome.ok,true,outcome.error?.stack);assert(calls.includes('texSubImage2D'));assert.equal(textures.get('46:1')?.helpPage,3);}
 else{assert.deepEqual(calls,[],'retired help loader uploaded to renderer');assert.equal(textures.size,0);assert.equal(outcome.ok,false);assert.equal(outcome.error.name,'AbortError');}
}
(async()=>{
 sound=await import(pathToFileURL(soundPath).href);
 const rows=[];async function run(name,fn){try{await fn();rows.push({name,passed:true});console.log('PASS '+name);}catch(error){rows.push({name,passed:false,error:error.stack});console.log('FAIL '+name+' '+error.stack);}}
 for(const point of ['replay-observe','name-observe','replay-save','name-save','help','audio-lane','restart'])
  for(const mode of ['current','dispose','replace','reject-current','reject-replace'])await run(point+'/'+mode,()=>executeScenario(point,mode));
 for(const [point,mode]of [['replay-save','selection'],['replay-save','store'],['name-observe','selection']])await run(point+'/'+mode,()=>executeScenario(point,mode));
 for(const mode of ['current','retired','aborted'])await run('actual-help-loader/'+mode,()=>helpLoaderScenario(mode));
 const failed=rows.filter(v=>!v.passed).length;console.log(JSON.stringify({total:rows.length,passed:rows.length-failed,failed,scope:'whole production pause consumer/controller and help loader; explicit synthetic native/store/DOM/GL;0World/browser',rows}));if(failed)process.exitCode=1;
})().catch(e=>{console.error(e);process.exitCode=1;});
