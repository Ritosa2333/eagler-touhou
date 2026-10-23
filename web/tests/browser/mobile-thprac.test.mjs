import test from 'node:test';
import assert from 'node:assert/strict';
import {touchControls} from '../../sdl-runtime/eagler-host.mjs';

test('mobile panel snapshots reach Module without clearing held THPrac keys',()=>{
 const core={sdl_touch_controls(){},sdl_touch_options(){}};
 const options={touchEnabled:true,touchSensitivity:100};
 const Module={eaglerControls:{thpracKeyboardBits:512}};
 touchControls(core,options,{controls:{thpracMobilePanelsEnabled:true,thpracMobilePanelsVisible:true}},Module);
 assert.deepEqual(Module.eaglerControls,{thpracKeyboardBits:512,thpracMobilePanelsEnabled:true,thpracMobilePanelsVisible:true});
 touchControls(core,options,{thpracMobilePanelsEnabled:true,thpracMobilePanelsVisible:false},Module);
 assert.equal(Module.eaglerControls.thpracMobilePanelsVisible,false);
 touchControls(core,options,{},Module);
 assert.equal(Module.eaglerControls.thpracMobilePanelsEnabled,false);
 assert.equal(Module.eaglerControls.thpracKeyboardBits,512);
});
