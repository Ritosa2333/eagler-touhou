import test from 'node:test';
import assert from 'node:assert/strict';
import {bindGameAudioMix,applyGameAudioMix} from '../../site/runtime/game-audio-mix.mjs';
import {musicOutput} from '../../site/runtime/music-output.mjs';
import {mountLocalBindings,readInputBindings} from '../../site/runtime/local-input-bindings.mjs';
import {environment} from './surface.mjs';
import {readLocalPlayerSettings,writeLocalPlayerSettings} from '../../site/runtime/local-player-settings.mjs';
const storage=()=>{const table=new Map();return{getItem:k=>table.get(k)??null,setItem:(k,v)=>table.set(k,v)};};
test('OGG source and effect output retain independent values including zero',()=>{
 const device={},context={destination:device,createGain(){return {context,gain:{value:1},connect(route){this.output=route;}};}};
 const effects=context.createGain();effects.connect(device);
 const {bgm}=bindGameAudioMix(context,effects,{musicVolume:25,effectsVolume:70});
 assert.equal(musicOutput(context,effects,'ogg'),bgm);assert.equal(bgm.output,device);
 assert.equal(bgm.gain.value,.25);assert.equal(effects.gain.value,.7);
 const muted=musicOutput(context,effects,'none');assert.equal(muted.gain.value,0);
 applyGameAudioMix(effects,{musicVolume:0,effectsVolume:83});assert.equal(bgm.gain.value,0);assert.equal(effects.gain.value,.83);
 assert.equal(musicOutput(context,effects,'none'),muted);
});
test('new launch inherits device touch and old volume but respects explicitly saved movement',()=>{
 const window={localStorage:storage(),sessionStorage:storage(),navigator:{maxTouchPoints:1}};
 window.sessionStorage.setItem('th128.volume','53');
 let s=readLocalPlayerSettings(window);assert(s.touch.touchEnabled);assert.equal(s.touch.touchMovementMode,'touch');assert.equal(s.effectsVolume,53);
 writeLocalPlayerSettings(window,{musicVolume:0,highRefresh:true,language:'ja',touch:{...s.touch,touchMovementMode:'touch-unlimited'}});
 s=readLocalPlayerSettings(window);assert.equal(s.touch.touchMovementMode,'touch-unlimited');assert.equal(s.musicVolume,0);assert.equal(s.effectsVolume,53);assert.equal(s.language,'ja');assert.equal(s.highRefresh,true);
 window.localStorage.setItem('th128-owned-touch-options-v1',JSON.stringify({...s.touch,touchMovementMode:'joystick'}));
 assert.equal(readLocalPlayerSettings(window).touch.touchMovementMode,'joystick');
});

test('local remapped keys and standard gamepad axes use the same existing input leases',()=>{
 const {window,document}=environment(),owners=new Map();let physical=[];
 window.navigator={getGamepads:()=>physical};
 const input={setKeySource:(id,keys)=>owners.set(id,keys),clearKeySource:id=>owners.delete(id)};
 const bindings=readInputBindings({ArrowUp:'KeyW'},{KeyZ:5});
 const step=mountLocalBindings({window,document,input,bindings,getActive:()=>true});
 const key=(type,code)=>window.dispatchEvent({type,code,isTrusted:true,preventDefault(){},stopImmediatePropagation(){}});
 key('keydown','KeyW');assert.deepEqual([...owners.values()],[['ArrowUp']]);key('keyup','KeyW');
 const buttons=Array.from({length:17},()=>({pressed:false}));buttons[5].pressed=true;
 physical=[{connected:true,mapping:'standard',axes:[-.7,0],buttons}];step();
 assert([...owners.values()].some(held=>held.includes('KeyZ')&&held.includes('ArrowLeft')));
 document.hidden=true;document.dispatchEvent({type:'visibilitychange'});step();assert.equal(owners.size,0);
});
