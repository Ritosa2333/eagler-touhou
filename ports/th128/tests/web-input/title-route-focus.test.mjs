import test from 'node:test';
import assert from 'node:assert/strict';
import {bootPlay} from './play-harness.mjs';

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
 assert.equal(h.state.paused,true);assert.equal(h.state.pressed.size,0);
 h.window.dispatchEvent({type:'focus'});assert.equal(h.state.paused,true);
 visible(h,true);
 assert.equal(h.state.paused,false,'route must resume without pressing Enter through the practice panel');
 assert.equal(h.menu.route,2);assert.equal(h.state.mainController.released,2);
});

test('focus never resumes a manual pause, a replacement Title, or a paused World',()=>{
 const manual=route();manual.state.paused=true;visible(manual,false);visible(manual,true);assert.equal(manual.state.paused,true);
 const replaced=route();visible(replaced,false);++replaced.state.action;visible(replaced,true);assert.equal(replaced.state.paused,true);
 const field=bootPlay();field.state.paused=true;field.menu.pauseOpen=true;visible(field,false);visible(field,true);assert.equal(field.state.paused,true);
});

test('the owned native World is held in the background and can receive route input after returning',()=>{
 const h=bootPlay();h.state.worldSnapshot={message:{active:true,routeChoice:{phase:'select',message7c:0}}};
 h.state.pressed.add('KeyZ');h.window.dispatchEvent({type:'blur'});visible(h,false);
 assert.equal(h.state.paused,true);assert.equal(h.state.pressed.size,0);
 assert.equal(h.menu.pauseOpen,false,'a hidden world must not open a gesture-gated native pause menu');
 assert.equal(h.pause.length,0);
 h.window.dispatchEvent({type:'focus'});assert.equal(h.state.paused,true);
 visible(h,true);assert.equal(h.state.paused,false);
 h.key('keydown','ArrowDown');assert(h.receipts.some(args=>args[1]===32));
});

test('the old background receipt cannot unpause a replacement World',()=>{
 const h=bootPlay();visible(h,false);++h.state.action;visible(h,true);
 assert.equal(h.state.paused,true);assert.equal(h.menu.pauseOpen,false);
});
