import test from 'node:test';
import assert from 'node:assert/strict';
import {mountWebInput} from '../../site/runtime/web-input.mjs';
import {environment,Surface} from '../web-input/surface.mjs';

// These exercise the whole page widget. Browser CSS/real viewport acceptance is separate.
function fixture({data=new Map(),ios=false}={}){
 const e=environment(),body=new Surface('body');e.document.body=body;body.appendChild(e.container);
 Object.assign(e.window,{innerWidth:900,innerHeight:650,navigator:{userAgent:ios?'iPhone':'test',maxTouchPoints:1}});
 e.window.visualViewport=new Surface('viewport');Object.assign(e.window.visualViewport,{width:900,height:650,offsetLeft:0,offsetTop:0});
 e.container.rect=e.canvas.rect={left:0,top:0,width:900,height:650};
 const storage={getItem:k=>data.get(k)??null,setItem:(k,v)=>data.set(k,v),removeItem:k=>data.delete(k)},commands=[],keys=new Set();
 const input=mountWebInput({...e,storage,getContext:()=>1,getGameContext:()=>({key:'game',kind:'game',instance:1,gestureScope:1,ready:true,generation:7,x:0,y:51200,fast:576,slow:256}),onTouchMotion:c=>commands.push(c),options:{touchEnabled:true,touchMovementMode:'touch-unlimited',touchSensitivity:100,touchFocusMode:'hold-button'}});
 e.window.addEventListener('keydown',event=>{if(input.acceptKeyDown(event))keys.add(event.code);});e.window.addEventListener('keyup',event=>{if(input.acceptKeyUp(event))keys.delete(event.code);});
 const node=id=>e.find(body,'id',id);
 // The minimal DOM fixture does not inherit CSS dimensions from its parent.
 const layer=node('touchDirectSurface').parentElement;layer.rect=e.container.rect;layer.clientWidth=900;layer.clientHeight=650;
 function panel(id){const n=node(id);assert(n,id);n.getBoundingClientRect=()=>({left:parseFloat(n.style.left)||0,top:parseFloat(n.style.top)||0,width:320,height:n.classList.contains('is-collapsed')?50:280});return n;}
 function rotate(width,height){Object.assign(e.window,{innerWidth:width,innerHeight:height});Object.assign(e.window.visualViewport,{width,height});e.window.dispatchEvent({type:'resize'});}
 function drag(id,from,to){const n=node(id);assert(n,'missing real drag handle '+id);e.emit(n,'pointerdown',71,...from);e.emit(e.document,'pointermove',71,...to);e.emit(e.document,'pointerup',71,...to);}
 function touch(n,type,id,x=100,y=100){n.dispatchEvent({type,changedTouches:[{identifier:id,clientX:x,clientY:y,target:n}],preventDefault(){},stopPropagation(){}});}
 return{...e,body,input,node,panel,rotate,drag,touch,data,commands,keys};
}
test('settings is a draggable nonmodal window with durable position and touch cleanup',()=>{
 const e=fixture(),p=e.panel('touchSettings');e.node('touchSettingsOpen').dispatchEvent({type:'click'});
 e.drag('touchSettingsDragHandle',[100,100],[280,210]);assert.equal(p.style.left,'564px');assert.equal(p.style.top,'186px');assert(!p.hidden);
 e.input.sampleTouch();assert(!e.keys.has('KeyC'));assert.equal(e.commands.filter(c=>c.mode!==0).length,0);
 const before={left:p.style.left,top:p.style.top};e.node('touchSettingsClose').dispatchEvent({type:'click'});assert(p.hidden);
 e.node('touchSettingsOpen').dispatchEvent({type:'click'});assert.deepEqual({left:p.style.left,top:p.style.top},before);
 assert([...e.data.keys()].some(k=>k.includes('window-positions')));e.input.dispose();
});
test('editor window drag, collapse and landscape/portrait positions survive remount independently',()=>{
 const e=fixture(),p=e.panel('touchLayoutEditor');e.input.beginTouchLayout('landscape');
 e.drag('touchLayoutEditorDragHandle',[100,100],[200,200]);assert.equal(p.style.top,'176px');const landscape={left:p.style.left,top:p.style.top};
 e.node('touchLayoutCollapse').dispatchEvent({type:'click'});assert(e.node('touchWorkbenchBody').hidden);
 e.node('touchLayoutCollapse').dispatchEvent({type:'click'});assert(!e.node('touchWorkbenchBody').hidden);
 e.rotate(500,900);e.drag('touchLayoutEditorDragHandle',[100,100],[150,250]);assert.equal(e.input.touchSnapshot().layout.orientation,'portrait');
 e.rotate(900,650);assert.deepEqual({left:p.style.left,top:p.style.top},landscape);e.input.endTouchLayout();e.input.dispose();
 const again=fixture({data:e.data}),next=again.panel('touchLayoutEditor');again.input.beginTouchLayout('landscape');
 assert.deepEqual({left:next.style.left,top:next.style.top},landscape);again.input.dispose();
});
test('viewport edit drags the actual canvas transform and ends without discarding the layout draft',()=>{
 const e=fixture();e.panel('touchLayoutEditor');e.input.beginTouchLayout('landscape');
 const adjust=e.node('touchViewportAdjust');assert(adjust,'viewport has its original edit action');adjust.dispatchEvent({type:'click'});
 const surface=e.node('touchViewportDragSurface');assert(surface&&!surface.hidden);e.emit(surface,'pointerdown',3,100,100);e.emit(e.document,'pointermove',3,280,100);e.emit(e.document,'pointerup',3,280,100);
 assert(Math.abs(e.input.touchSnapshot().layout.draft.profiles.landscape.viewport.x-.2)<1e-6);
 assert.match(e.canvas.style.transform,/translate3d\(180px,/);e.node('touchViewportDone').dispatchEvent({type:'click'});assert(surface.hidden);assert(e.input.touchSnapshot().layout.isEditing);
 e.input.endTouchLayout();e.input.dispose();
});
test('iOS without PointerEvent uses real focus, rapid fire, joystick and cancel consumers',()=>{
 const e=fixture({ios:true}),focus=e.node('touchFocus');e.touch(focus,'touchstart',2);e.input.sampleTouch();assert(e.keys.has('ShiftLeft'));
 e.touch(focus,'touchend',2);e.input.sampleTouch();assert(!e.keys.has('ShiftLeft'));assert(e.keys.has('KeyC'));
 e.touch(e.node('touchFire'),'touchstart',3);e.input.sampleTouch();assert(!e.keys.has('KeyC'));
 e.input.configureTouch({touchMovementMode:'joystick-free'});const joystick=e.node('touchJoystick');joystick.rect={left:0,top:0,width:128,height:128};e.touch(joystick,'touchstart',4,100,64);e.input.sampleTouch();assert.equal(e.commands.at(-1).mode,3);assert(e.commands.at(-1).x>0);
 e.touch(joystick,'touchcancel',4,100,64);e.input.sampleTouch();assert.equal(e.commands.at(-1).mode,0);e.input.dispose();assert.equal(e.keys.size,0);
});
test('pinch uses the real zoom controller and visibility cancellation releases every pointer',()=>{
 const e=fixture(),surface=e.node('touchDirectSurface');e.emit(surface,'pointerdown',1,200,200);e.emit(surface,'pointerdown',2,300,200);e.emit(surface,'pointermove',2,400,200);
 assert.equal(e.input.touchSnapshot().zoom.scale,2);e.window.dispatchEvent({type:'blur'});assert.equal(e.input.touchSnapshot().zoom.pointerCount,0);e.input.dispose();
});
