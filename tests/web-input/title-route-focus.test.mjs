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
 const field=bootPlay();visible(field,false);assert.equal(field.menu.pauseOpen,true);visible(field,true);assert.equal(field.state.paused,true);
});
