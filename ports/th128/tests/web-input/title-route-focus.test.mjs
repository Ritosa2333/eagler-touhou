import test from 'node:test';
import assert from 'node:assert/strict';
import {bootPlay,functionSource} from './play-harness.mjs';

function route(){
 const h=bootPlay({worldMode:false,practiceMenu:{isOpen:true}});
 h.menu.page='route';
 h.state.mainController={prepared:true,released:0,release(){++this.released;}};
 return h;
}
function visible(h,value){h.document.hidden=!value;h.document.dispatchEvent({type:'visibilitychange'});}

test('returning to a route with the practice panel open resumes its own focus pause',()=>{
 const h=route();h.menu.route=2;h.state.pressed.add('Enter');
 h.window.dispatchEvent({type:'blur'});visible(h,false);
 assert.equal(h.state.backgroundPaused,true);assert.equal(h.state.pressed.size,0);
 h.window.dispatchEvent({type:'focus'});assert.equal(h.state.backgroundPaused,true);
 visible(h,true);
 assert.equal(h.state.paused,false,'route must resume without pressing Enter through the practice panel');
 assert.equal(h.state.backgroundPaused,false);assert.equal(h.menu.route,2);assert.equal(h.state.mainController.released,1);
});

test('focus never resumes a manual pause, a replacement Title, or a paused World',()=>{
 const manual=route();manual.state.paused=true;visible(manual,false);visible(manual,true);assert.equal(manual.state.paused,true);
 const replaced=route();visible(replaced,false);++replaced.state.action;replaced.state.paused=true;visible(replaced,true);assert.equal(replaced.state.paused,true);
 const field=bootPlay();field.state.paused=true;field.menu.pauseOpen=true;visible(field,false);visible(field,true);assert.equal(field.state.paused,true);
});

test('the owned native World is held in the background and can receive route input after returning',()=>{
 const h=bootPlay();h.state.worldSnapshot={message:{active:true,routeChoice:{phase:'select',message7c:0}}};
 h.state.pressed.add('KeyZ');h.window.dispatchEvent({type:'blur'});visible(h,false);
 assert.equal(h.state.backgroundPaused,true);assert.equal(h.state.pressed.size,0);
 assert.equal(h.menu.pauseOpen,false,'a hidden world must not open a gesture-gated native pause menu');
 assert.equal(h.pause.length,0);
 h.window.dispatchEvent({type:'focus'});assert.equal(h.state.backgroundPaused,true);
 visible(h,true);assert.equal(h.state.backgroundPaused,false);assert.equal(h.state.paused,false);
 h.key('keydown','ArrowDown');assert(h.receipts.some(args=>args[1]===32));
});

test('the old background receipt cannot unpause a replacement World',()=>{
 const h=bootPlay();visible(h,false);++h.state.action;h.state.paused=true;visible(h,true);
 assert.equal(h.state.paused,true);assert.equal(h.menu.pauseOpen,false);
});

test('backgrounding during a stage or ending load preserves its request and resumes the replacement scene',()=>{
 const h=route();
 h.menu.launching=true;h.state.sceneLoading={};
 visible(h,false);
 assert.equal(h.menu.launching,true);assert.equal(h.menu.launchId,1);
 h.menu.launching=false;h.state.sceneLoading=null;h.state.endingLoading=true;
 ++h.state.action;visible(h,true);
 assert.equal(h.state.endingLoading,true);assert.equal(h.state.paused,false);
 assert.equal(h.state.backgroundPaused,false);
});

test('ending audio interrupted during an in-flight frame resumes before the original receipt continues',async()=>{
 const h=bootPlay(),audio=new EventTarget(),waiting=new AbortController();
 const calls=[];
 audio.state='running';
 audio.suspend=async()=>{calls.push('suspend');};
 audio.resume=async()=>{calls.push('resume');audio.state='running';audio.dispatchEvent(new Event('statechange'));};
 h.state.audio=audio;h.state.endingHost={owner:{input:{release(){}}}};
 let complete=false;
 audio.state='interrupted';visible(h,false);
 const receipt=h.waitForBackgroundAudio(audio,waiting.signal).then(()=>{complete=true;});
 h.state.endingStepPromise=receipt;
 await Promise.resolve();assert.equal(complete,false);
 visible(h,true);await receipt;
 assert.deepEqual(calls,['resume']);assert.equal(h.state.paused,false);
});

test('a loading deadline counts time spent in the foreground',()=>{
 const document=new EventTarget();document.hidden=false;
 let now=0,deadline,serial=0;
 const timeout=new Function('document','performance','setTimeout','clearTimeout',
  functionSource('foregroundTimeout')+';return foregroundTimeout;')(
  document,{now:()=>now},(callback,delay)=>{deadline={callback,delay};return ++serial;},()=>{deadline=null;});
 let fired=0;
 const stop=timeout(()=>{fired++;},15000);
 assert.equal(deadline.delay,15000);
 now=6000;document.hidden=true;document.dispatchEvent(new Event('visibilitychange'));
 assert.equal(deadline,null);assert.equal(fired,0);
 now=66000;document.hidden=false;document.dispatchEvent(new Event('visibilitychange'));
 assert.equal(deadline.delay,9000);
 deadline.callback();assert.equal(fired,1);
 stop();
});
