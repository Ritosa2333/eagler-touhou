import test from'node:test';import assert from'node:assert/strict';
import{mountWebInput}from'../../site/runtime/web-input.mjs';import{environment}from'../web-input/surface.mjs';
function fixture(options={},useDefaultMode=false){
 const e=environment(),data=new Map(),storage={getItem:k=>data.get(k)??null,setItem:(k,v)=>data.set(k,v),removeItem:k=>data.delete(k)},commands=[],keys=new Set();
 let context={key:'game1',kind:'game',instance:1,gestureScope:1,ready:true,generation:7,x:0,y:51200,fast:576,slow:256};
 const input=mountWebInput({...e,getContext:()=>1,getGameContext:()=>context,onTouchMotion:motion=>commands.push(motion),storage,options:{touchEnabled:true,...(useDefaultMode?{}:{touchMovementMode:'touch-unlimited'}),touchSensitivity:100,...options}});
 assert.equal(typeof input.sampleTouch,'function','full upstream touch must have fixed-tick page consumer');
 e.window.addEventListener('keydown',event=>{if(input.acceptKeyDown(event))keys.add(event.code);});e.window.addEventListener('keyup',event=>{if(input.acceptKeyUp(event))keys.delete(event.code);});
 return{...e,input,commands,keys,storage,data,setContext:next=>context={...context,...next},node:name=>e.find(e.container,'id',name)};
}
test('a fresh touch layout uses limited direct motion while an explicit unlimited preference stays available',()=>{
 const e=fixture({},true);
 assert.equal(e.input.touchSnapshot().options.touchMovementMode,'touch');
 const surface=e.node('touchDirectSurface');e.emit(surface,'pointerdown',1,640,480);e.emit(surface,'pointermove',1,660,480);
 e.input.sampleTouch();assert.equal(e.commands.at(-1).mode,1);
 e.input.configureTouch({touchMovementMode:'touch-unlimited'},true);
 assert.equal(e.input.touchSnapshot().options.touchMovementMode,'touch-unlimited');e.input.dispose();
});
test('whole page direct touch samples native command only on fixed tick with TH128 rapid fire and focus ownership',()=>{
 const e=fixture(),surface=e.node('touchDirectSurface');assert(surface);e.emit(surface,'pointerdown',1,640,480);e.emit(surface,'pointermove',1,660,480);assert.equal(e.commands.length,0);
 e.input.sampleTouch();assert.deepEqual(e.commands.at(-1),{mode:2,x:1280,y:51200,generation:7});assert(e.keys.has('KeyC'));assert(!e.keys.has('KeyZ'));
 e.emit(surface,'pointerdown',2,500,500);e.input.sampleTouch();assert(e.keys.has('ShiftLeft'));e.key('keydown','ShiftRight');e.emit(surface,'pointerup',2,500,500);e.input.sampleTouch();assert(e.keys.has('ShiftLeft')||e.keys.has('ShiftRight'));e.key('keyup','ShiftRight');assert(!e.keys.has('ShiftLeft')&&!e.keys.has('ShiftRight'));
 e.emit(surface,'pointerup',1,660,480);e.input.sampleTouch();assert.deepEqual(e.commands.at(-1),{mode:0,x:0,y:0,generation:7});e.input.dispose();
});
test('all four upstream movement modes configure live, free joystick keeps analog magnitude, replay and blur release touch',()=>{
 const e=fixture();for(const mode of ['touch','touch-unlimited','joystick','joystick-free']){e.input.configureTouch({touchMovementMode:mode});assert.equal(e.input.touchSnapshot().options.touchMovementMode,mode);}
 e.input.touchControls({fireEnabled:false,focusEnabled:true,bombSerial:0,escapeSerial:0,joystickX:16384,joystickY:-32767});e.input.sampleTouch();assert.deepEqual(e.commands.at(-1),{mode:3,x:16384,y:-32767,generation:7});
 e.setContext({key:'replay1',kind:'replay'});const before=e.commands.length;e.input.sampleTouch();assert.equal(e.commands.length,before);assert(!e.keys.has('KeyC'));
 e.setContext({key:'game2',kind:'game',instance:2,generation:8});e.window.dispatchEvent({type:'blur'});e.input.sampleTouch();assert.equal(e.commands.at(-1).mode,0);e.input.dispose();
});
test('original layout model edits scale and position with separate orientation storage and explicit save/cancel',()=>{
 const e=fixture();e.input.beginTouchLayout('landscape');e.input.updateTouchLayout('fire',{x:.2,y:.3,scale:2.5});e.input.saveTouchLayout();e.input.endTouchLayout();
 e.input.beginTouchLayout('portrait');e.input.updateTouchLayout('fire',{x:.7,y:.8,scale:.6});e.input.saveTouchLayout();e.input.endTouchLayout();
 const s=e.input.touchSnapshot();assert.equal(s.layout.saved.profiles.landscape.controls.fire.scale,2.5);assert.equal(s.layout.saved.profiles.portrait.controls.fire.x,.7);assert(e.data.size>0);
 e.input.beginTouchLayout('landscape');e.input.updateTouchLayout('fire',{scale:99});assert.equal(e.input.touchSnapshot().layout.draft.profiles.landscape.controls.fire.scale,3);e.input.endTouchLayout();assert.equal(e.input.touchSnapshot().layout.saved.profiles.landscape.controls.fire.scale,2.5);e.input.dispose();
});
test('Z and X shape edits use the same per-orientation save as position and scale',()=>{
 const e=fixture();e.input.beginTouchLayout('landscape');
 const selector=e.node('touchLayoutSelected'),shape=e.node('touchLayoutShape');assert(shape,'Layout offers button shapes');
 selector.value='function';selector.dispatchEvent({type:'change'});shape.value='rectangle';shape.dispatchEvent({type:'change'});
 e.input.updateTouchLayout('function',{x:.2,y:.7,scale:1.35});
 selector.value='bomb';selector.dispatchEvent({type:'change'});shape.value='square';shape.dispatchEvent({type:'change'});
 assert.equal(e.node('touchFunction').getAttribute('data-touch-shape'),'rectangle');
 assert.equal(e.node('touchBomb').getAttribute('data-touch-shape'),'square');
 e.input.saveTouchLayout();e.input.endTouchLayout();
 e.input.beginTouchLayout('portrait');assert.equal(e.node('touchBomb').getAttribute('data-touch-shape'),'circle');
 e.input.endTouchLayout();e.input.beginTouchLayout('landscape');
 const z=e.input.touchSnapshot().layout.draft.profiles.landscape.controls.function;
 assert.equal(z.shape,'rectangle');assert.deepEqual([z.x,z.y,z.scale],[.2,.7,1.35]);e.input.dispose();
});
test('an editing preview accepts Z/X selection and dragging without sending gameplay keys',()=>{
 const e=fixture({touchEnabled:false});e.input.beginTouchLayout('landscape');
 assert.equal(e.input.layer.style.display,'block');assert.equal(e.node('touchDirectSurface').style.pointerEvents,'none');
 for(const [id,name] of [['touchFunction','function'],['touchBomb','bomb']]){
  const button=e.node(id);e.emit(button,'pointerdown',1,250,300,'mouse');e.emit(button,'pointermove',1,350,400,'mouse');e.emit(button,'pointerup',1,350,400,'mouse');
  assert.equal(e.input.touchSnapshot().layout.selected,name);assert(!e.keys.has('KeyX')&&!e.keys.has('KeyZ'));
 }
 e.input.endTouchLayout();e.input.configureTouch({touchEnabled:true});
 assert.equal(e.node('touchDirectSurface').style.pointerEvents,'auto');
 e.emit(e.node('touchFunction'),'pointerdown',1);assert(e.keys.has('KeyZ'));e.emit(e.node('touchFunction'),'pointerup',1);assert(!e.keys.has('KeyZ'));e.input.dispose();
});
test('direct player display samples the live target between ticks without submitting input or ignoring the speed cap',()=>{
 for(const mode of ['touch','touch-unlimited']){
  const e=fixture({touchMovementMode:mode,touchSensitivity:100}),pad=e.node('touchDirectSurface');
  e.emit(pad,'pointerdown',5,640,480);e.emit(pad,'pointermove',5,680,480);
  const a=e.input.previewDirectMotion(.25),b=e.input.previewDirectMotion(.75);
  assert.equal(a.generation,7);assert.equal(a.y,0);assert.equal(b.y,0);
  assert.equal(b.x,a.x*3);assert.equal(b.x,mode==='touch'?3.375:15);
  assert.equal(e.commands.length,0);assert.equal(e.keys.size,0);
  e.key('keydown','ArrowLeft');assert.equal(e.input.previewDirectMotion(.5),null);e.key('keyup','ArrowLeft');
  e.input.sampleTouch();assert.equal(e.commands.at(-1).x,2560);
  e.emit(pad,'pointercancel',5,680,480);assert.equal(e.input.previewDirectMotion(.5),null);e.input.dispose();
 }
});
test('the launch master switch also blocks touch practice shortcuts without blocking ordinary controls',()=>{
 const e=fixture({thpracEnabled:false,thpracTouchControlsEnabled:true,restartButtonEnabled:true});
 assert.equal(e.node('touchThpracMenu').hidden,true);
 e.input.touchControls({thpracMobilePanelsEnabled:true,thpracMobilePanelsVisible:true});
 assert.equal(e.node('touchThpracMenu').hidden,true);
 for(const id of ['touchThpracBackspace','touchAdvanced'])e.emit(e.node(id),'pointerdown',3);
 assert(!e.keys.has('Backspace')&&!e.keys.has('F12'));
 assert.equal(e.node('touchRestart').hidden,false);e.emit(e.node('touchRestart'),'pointerdown',4);assert(e.keys.has('KeyR'));
 e.input.dispose();
});
test('menu gestures and mobile Backspace/F12/R controls use real shared keyboard owners, no gameplay modal freeze',()=>{
 const e=fixture({thpracTouchControlsEnabled:true,restartButtonEnabled:true});e.setContext({key:'route',kind:'menu',ready:false});const surface=e.node('touchDirectSurface');e.emit(surface,'pointerdown',1,100,100);e.emit(surface,'pointermove',1,160,100);e.input.sampleTouch();assert(e.keys.has('ArrowRight'));e.input.sampleTouch();assert(!e.keys.has('ArrowRight'));
 for(const[id,code]of[['touchThpracBackspace','Backspace'],['touchAdvanced','F12'],['touchRestart','KeyR']]){const n=e.node(id);assert(n,id);e.emit(n,'pointerdown',2);assert(e.keys.has(code),code);e.emit(n,'pointerup',2);assert(!e.keys.has(code),code);}
 e.input.dispose();
});
test('an open advanced panel owns the screen while mobile ESC remains available and direct motion resumes cleanly',()=>{
 const e=fixture(),surface=e.node('touchDirectSurface');assert.equal(typeof e.input.setOverlayActive,'function','advanced panel must release touch interception');
 e.input.setOverlayActive(true);e.emit(surface,'pointerdown',1,100,100);e.emit(surface,'pointermove',1,200,100);e.input.sampleTouch();assert.equal(e.commands.length,0);
 const escape=e.node('touchEscape');e.emit(escape,'pointerdown',2);assert(e.keys.has('Escape'));e.emit(escape,'pointerup',2);assert(!e.keys.has('Escape'));
 e.input.setOverlayActive(false);e.input.sampleTouch();assert.equal(e.commands.at(-1).mode,0);e.input.dispose();
});
