import test from 'node:test';import assert from 'node:assert/strict';
import {mountPracticeMenu} from '../../site/runtime/practice-menu.mjs';
import {createPageLaunchContext} from '../../site/runtime/page-launch-context.mjs';
import {functionSource} from '../web-input/play-harness.mjs';
import {surface,flush,deferred} from './surface.mjs';
test('actual launcher runtime exit disposes open practice and aborts its pending owner',async()=>{
 const ui=surface(),held=deferred(),context=createPageLaunchContext({config:{}}),calls=[];
 const practiceMenu=mountPracticeMenu({...ui,getAvailability:()=>({ready:true,busy:false,inRun:false}),launch:async(route,owner)=>{calls.push(owner);return held.promise;}});
 const state={active:true,paused:false,loading:1,pressed:new Set(),controller:new AbortController()},menu={launchId:3};
 const factory=new Function('state','menu','practiceMenu','getPageLaunchContext','cancelOriginalEnding','releaseGameInput','stopAudio','clock','resetHighFrameratePresentation','setHighFramerateMode','find',
 functionSource('installLauncherRuntime')+';return installLauncherRuntime;');
 factory(state,menu,practiceMenu,()=>context,()=>{},()=>{},()=>{},{reset(){}},()=>{},()=>{},()=>({}))();
 ui.opener.emit('click');await flush();ui.nodes.start.emit('click');assert.equal(calls.length,1);assert.equal(ui.root.hidden,false);
 context.abort();assert.equal(state.launcherExited,true);assert.equal(state.controller.signal.aborted,true);
 assert.equal(ui.root.hidden,true,'Exited runtime leaves a live practice dialog');assert.equal(ui.opener.disabled,true);assert.equal(calls[0].signal.aborted,true);
 held.reject(Error('owned startup cancelled'));await flush();assert.equal(ui.root.hidden,true);
});
