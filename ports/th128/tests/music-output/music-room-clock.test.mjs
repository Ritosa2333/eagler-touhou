import assert from 'node:assert/strict';
import test from 'node:test';
import {OriginalMainMenuController,OriginalTitleAudio} from '../../site/runtime/sound.mjs';
import {FrameStepper,renderInterpolationPhase} from '../../site/runtime/timing.mjs';
import {functionSource} from '../web-input/play-harness.mjs';
import {graph,track,buffer} from './graph-fixture.mjs';

const turn=()=>new Promise(resolve=>setImmediate(resolve));
const previewTrack={...track,name:'th128_11.wav',file:'music/ogg/th128_11.ogg'};
const receipt=value=>({strictReadback:true,payload:JSON.stringify(value),digest:'a'.repeat(64)});

// Actual Title controller, presentation loop and audio consumer. Native input
// and the audio device/receipt interfaces are isolated, with explicit gates.
function room(){
 const g=graph(),decode=Promise.withResolvers(),loading=Promise.withResolvers();
 const pending=[],acks=[],actions=[],previews=[],overlays=[],errors=[],frames=[],inputs=[];
 const menu={page:5,ready:1,transitioning:0,selection:7},state={active:true,paused:false,worldMode:false};
 let pressed=0,ticks=0,serial=0,confirm=0,next=null;
 function request(operation,row){pending.push({epoch:1,serial:++serial,operation,row});}
 const module={UTF8ToString:x=>x,_th128_title_menu_state:()=>JSON.stringify(menu),
  _th128_title_main_input(held,edge){pressed=edge;inputs.push(edge);return 1;},
  _th128_tick(){
   ++ticks;
   if(menu.page===5){
    if(pressed&16)--menu.selection;
    if(pressed&1)request(++confirm===1?0:1,menu.selection);
    if(pressed&256){request(2,0);menu.page=0;}
   }else if(pressed&1)menu.mainAction=1;
   pressed=0;return 1;
  },_th128_title_music_bgm_request:()=>JSON.stringify(pending[0]??{}),
  _th128_title_music_bgm_ack(epoch,id,operation,row){
   assert.deepEqual(pending.shift(),{epoch,serial:id,operation,row});acks.push(operation);return pending.length;
  }};
 const audio=new OriginalTitleAudio({context:g.context,gain:g.master,base:'/',tracks:[previewTrack],resolveUrl:()=>'',
  onStopTitle(){},onStartTitle(){const source=g.context.createBufferSource();source.start();return{source,track,buffer};},
  onPreview:row=>previews.push(row),bgmLedger:{async commit(value){return receipt(value);}}});
 audio.music.load=async()=>{loading.resolve();await decode.promise;audio.music.preloaded.set(previewTrack.name,{track:previewTrack,buffer});};
 const controller=new OriginalMainMenuController({module,prepare:async()=>{},consumeSound:async()=>{},
  async consumeMusic(){while(pending.length){if(!await audio.consumeMusic(module))break;}},syncText:async()=>{},onAction:snapshot=>{actions.push(snapshot.mainAction);}});
 const bindings={state,menu,document:{hidden:false},clock:new FrameStepper(),originalMenuClock:new FrameStepper(),originalPresentationFPS:{reset(){}},
  highFramerateEnabled:false,highFramerateCatchup:false,practiceMenu:null,thpracToolsPanel:null,pollLocalBindings:null,
  renderInterpolationPhase,advance:()=>controller.step(),resetHighFrameratePresentation(){},captureHighFramerateWorld(){},syncGameplayFrameUI(){},
  draw(){frames.push({ticks,page:menu.page,row:menu.selection});return false;},requestAnimationFrame(fn){next=fn;},
  pause(){state.paused=true;overlays.push('动画已暂停');},status(){},error(problem){errors.push(problem);}};
 const present=new Function(...Object.keys(bindings),`return async ${functionSource('present')}`)(...Object.values(bindings));
 next=present;
 return{acks,actions,frames,inputs,overlays,errors,menu,state,loading,decode,audio,controller,previews,
  key(code){controller.keyDown(code);controller.keyUp(code);},
  frame(stamp){assert.ok(next,'menu did not schedule the next frame');const cb=next;next=null;return cb(stamp);},dispose(){controller.dispose();audio.dispose();}};
}

test('the second Z may wait for music without stopping snow animation or further menu input',async()=>{
 const h=room();let second;
 try{
  await h.frame(0);h.key('KeyZ');await h.frame(17);await turn();
  assert.deepEqual(h.acks,[0],'first Z consumes the original warning operation');
  h.key('KeyZ');second=h.frame(34);await h.loading.promise;
  const presented=await Promise.race([second.then(()=>true),turn().then(()=>false)]);
  if(!presented){
   h.decode.resolve();await second;await h.frame(384);
   assert.deepEqual(h.overlays,[],'waiting for a track must not trigger 动画已暂停');
  }
  h.key('ArrowUp');for(let stamp=51;stamp<=391;stamp+=17)await h.frame(stamp);
  assert.equal(h.frames.at(-1).row,6);assert(h.frames.at(-1).ticks>=20);
  assert.deepEqual(h.acks,[0],'no play acknowledgement before audio started');
  h.decode.resolve();await turn();await h.frame(408);
  assert.deepEqual(h.acks,[0,1]);assert.deepEqual(h.previews,[7]);assert.deepEqual(h.errors,[]);assert.deepEqual(h.overlays,[]);
  await h.frame(708);
  assert.deepEqual(h.overlays,['动画已暂停'],'a genuine gap in browser callbacks still pauses');
 }finally{h.decode.resolve();await second;h.dispose();}
});

test('leaving during a cold decode keeps Title drawing and waits for ordered media before accepting Start',async()=>{
 const h=room();let second;
 try{
  await h.frame(0);h.key('KeyZ');await h.frame(17);await turn();
  h.key('KeyZ');second=h.frame(34);await h.loading.promise;
  assert.equal(await Promise.race([second.then(()=>true),turn().then(()=>false)]),true,'cold audio stalled the original Music Room');
  h.key('Escape');await h.frame(51);assert.equal(h.frames.at(-1).page,0);
  h.key('KeyZ');await h.frame(68);assert.deepEqual(h.actions,[]);assert.deepEqual(h.acks,[0]);
  h.decode.resolve();await turn();await h.frame(85);
  assert.deepEqual(h.acks,[0,1,2]);assert.deepEqual(h.actions,[1]);assert.deepEqual(h.errors,[]);
 }finally{h.decode.resolve();await second;h.dispose();}
});

test('a retired Title audio owner cannot publish preview state or ACK a late durable write',async()=>{
 const g=graph(),finish=Promise.withResolvers(),entered=Promise.withResolvers();let preview=0,ack=0;
 const audio=new OriginalTitleAudio({context:g.context,gain:g.master,base:'/',tracks:[previewTrack],resolveUrl:()=>'',
  onStopTitle(){},onStartTitle(){assert.fail('unexpected restore');},onPreview(){++preview;},
  bgmLedger:{async commit(value){entered.resolve();await finish.promise;return receipt(value);}}});
 audio.music.preloaded.set(previewTrack.name,{track:previewTrack,buffer});
 const open=audio.consumeMusic({UTF8ToString:x=>x,_th128_title_music_bgm_request:()=>'{"epoch":1,"serial":1,"operation":1,"row":7}',
  _th128_title_music_bgm_ack(){++ack;return 0;}});
 await entered.promise;audio.dispose();finish.resolve();await assert.rejects(open,{name:'AbortError'});
 assert.equal(preview,0);assert.equal(ack,0);
});
