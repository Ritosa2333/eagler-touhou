import {test} from 'vitest';
import assert from 'node:assert/strict';
import {getEventListeners} from 'node:events';
import {rectChange,layoutEntryChange,addChange} from '../visual/layout-watch-core.mjs';
import {inspectSurfaceMotion,inspectLayoutMotion,captureSurfaceMotion,surfaceMotionFailures} from '../browser/ui-layout-probe.mjs';
const a={x:10,y:20,width:100,height:40};
function layoutMotionSource(style={},animations=[],hidden=false) {
  const element={id:'avatar',classList:[],closest:()=>hidden,getAnimations:()=>animations,getBoundingClientRect:()=>({left:10,top:20,right:50,bottom:60,width:40,height:40})};
  return {innerWidth:390,innerHeight:844,document:{querySelectorAll:()=>[element]},getComputedStyle:()=>({visibility:'visible',transitionProperty:'none',transitionDuration:'0s',transitionDelay:'0s',...style})};
}

test('layout inspection flags ordinary control size transitions and delayed all transitions',()=>{
  assert.deepEqual(inspectLayoutMotion(layoutMotionSource({transitionProperty:'color,width,height',transitionDuration:'140ms'})).errors,['layout-transition:avatar:width,height']);
  assert.deepEqual(inspectLayoutMotion(layoutMotionSource({transitionProperty:'all',transitionDelay:'.2s'})).errors,['layout-transition:avatar:all']);
  assert.deepEqual(inspectLayoutMotion(layoutMotionSource({transitionProperty:'width',transitionDuration:'.2s'},[],true)),{errors:[],checks:0});
});

test('layout inspection catches running geometry animations while allowing compositor feedback',()=>{
  const running=frames=>({playState:'running',effect:{getKeyframes:()=>frames}});
  assert.deepEqual(inspectLayoutMotion(layoutMotionSource({},[running([{gridTemplateRows:'0fr'},{gridTemplateRows:'1fr'}])])).errors,['layout-animation:avatar:grid-template-rows']);
  assert.deepEqual(inspectLayoutMotion(layoutMotionSource({transitionProperty:'opacity,transform,background-color',transitionDuration:'.14s'},[running([{translate:'0px'},{translate:'4px'}])])).errors,[]);
  assert.deepEqual(inspectLayoutMotion(layoutMotionSource({transitionProperty:'all',transitionDuration:'0s'})).errors,[]);
});

test('control inspection catches a broad room rule overriding shared feedback duration',()=>{
  const source=layoutMotionSource({transitionProperty:'background,transform,color',transitionDuration:'.2s,.25s,.2s',getPropertyValue:()=> '140ms'});
  const [element]=source.document.querySelectorAll();element.matches=()=>true;
  assert.deepEqual(inspectLayoutMotion(source).errors,['feedback-duration:avatar:.2s,.25s,.2s']);
  source.getComputedStyle=()=>({visibility:'visible',transitionProperty:'background-color,transform',transitionDuration:'.14s',transitionDelay:'0s',getPropertyValue:()=> '140ms'});
  assert.deepEqual(inspectLayoutMotion(source).errors,[]);
});
function motionSource(style={},animations=[]) {
  const element={id:'options',classList:['tools'],getAnimations:()=>animations};
  return {document:{querySelectorAll:()=>[element]},getComputedStyle:()=>({transitionProperty:'none',transitionDuration:'0s',transitionDelay:'0s',...style})};
}
test('surface inspection catches the delayed opacity transition behind a native fade',()=>{
  const source=motionSource({transitionProperty:'color, opacity',transitionDuration:'140ms, .4s',transitionDelay:'0s, 240ms'});
  assert.deepEqual(inspectSurfaceMotion(source).errors,['surface-css-transition:options']);
  assert.equal(inspectSurfaceMotion(motionSource({transitionProperty:'opacity',transitionDelay:'.2s'})).errors.length,1,'zero-duration delayed jumps still conflict');
});
test('surface inspection allows feedback colors and native fades while detecting CSS fades',()=>{
  const native={effect:{getKeyframes:()=>[{opacity:0},{opacity:1}]}};
  assert.deepEqual(inspectSurfaceMotion(motionSource({transitionProperty:'color',transitionDuration:'.14s'},[native])).errors,[]);
  const css={animationName:'old-entry',effect:{getKeyframes:()=>[{opacity:0},{opacity:1}]}};
  assert.deepEqual(inspectSurfaceMotion(motionSource({},[css])).errors,['surface-css-animation:options']);
  assert.deepEqual(inspectSurfaceMotion(motionSource({transitionProperty:'all',transitionDuration:'0s'})).errors,[]);
});

const motionFrame=(opacity,{phase='enter',visible=true,animations=[]}={})=>({opacity,phase,visible,animations});
const nativeEffect={type:'Animation',state:'running',properties:['opacity']};
test('motion acceptance rejects the observed full-opacity flash even when the final screenshot is correct',()=>{
  const frames=[motionFrame(0,{animations:[nativeEffect]}),motionFrame(.997),motionFrame(0),motionFrame(.7),motionFrame(1)];
  assert.deepEqual(surfaceMotionFailures({frames,reduced:false}),['surface-opacity-reversed']);
});
test('motion acceptance permits intentional close/reopen reversals but rejects stale closing effects',()=>{
  const frames=[motionFrame(1,{animations:[nativeEffect]}),motionFrame(.4,{phase:'exit'}),motionFrame(.4),motionFrame(.8),motionFrame(1)];
  assert.deepEqual(surfaceMotionFailures({frames,reduced:false}),[]);
  frames.push(motionFrame(.2,{phase:'exit',animations:[nativeEffect]}));
  assert.deepEqual(surfaceMotionFailures({frames,reduced:false}),['surface-effects-retained','surface-final-state']);
});
test('motion acceptance distinguishes reduced-motion coverage and incomplete recording',()=>{
  const frames=[motionFrame(1)];
  assert.deepEqual(surfaceMotionFailures({frames,reduced:true}),['normal-motion-disabled','native-motion-not-observed']);
  assert.deepEqual(surfaceMotionFailures({frames,reduced:true},{expectMotion:false}),[]);
  assert.deepEqual(surfaceMotionFailures({frames,reduced:true,inputMode:'interaction'},{expectMotion:false}),['surface-initial-state'],'an already-open panel is not an opening test');
  assert.deepEqual(surfaceMotionFailures({frames:[],error:'interaction-not-observed'}),['interaction-not-observed','surface-not-sampled']);
});
test('motion acceptance rejects competing native/CSS fades and verifies closed visibility',()=>{
  const frames=[motionFrame(1,{animations:[nativeEffect,{type:'CSSTransition',properties:['opacity']}]}),motionFrame(0,{phase:'closed',visible:false})];
  assert.deepEqual(surfaceMotionFailures({frames,reduced:false},{end:'closed'}),['surface-css-motion-conflict']);
});
test('motion recording waits for accessible activation and releases listeners after an immediate sample',async()=>{
  const doc=new EventTarget(),source=new EventTarget();
  const element={hidden:false,classList:{contains:()=>false},hasAttribute:()=>false,matches:()=>false,getAnimations:()=>[],getBoundingClientRect:()=>({width:100,height:100})};
  Object.assign(doc,{hidden:false,body:{closest:()=>null,classList:{contains:()=>false}},querySelector:()=>element});
  Object.assign(source,{document:doc,performance:{now:()=>0},innerWidth:100,innerHeight:100,matchMedia:()=>({matches:false}),setTimeout,clearTimeout,requestAnimationFrame(){throw Error('An immediate sample must not retain a frame');},cancelAnimationFrame(){},getComputedStyle:()=>({visibility:'visible',display:'block',opacity:'1'})});
  let completed=false;
  const pending=captureSurfaceMotion({source,duration:0}).then(value=>{completed=true;return value;});
  await Promise.resolve();assert.equal(completed,false);
  doc.dispatchEvent(new Event('click'));
  const trace=await pending;assert.equal(trace.frames.length,1);
  for(const event of ['pointerdown','click','keydown'])assert.equal(getEventListeners(doc,event).length,0);
  assert.equal(getEventListeners(source,'pagehide').length,0);
});
test('layout watch separates user scrolling and resizing from control movement',()=>{
  assert.equal(rectChange(a,{...a,y:-80},{scrolling:true}),null);
  assert.equal(rectChange(a,{...a,x:11.5}),null);
  assert.equal(rectChange(a,{...a,width:140},{resizing:true}),null);
  assert.equal(rectChange(a,{...a,width:140},{scrolling:true}).delta.width,40);
  assert.equal(rectChange(a,{...a,y:227}).amount,207);
});
test('layout watch ignores newly mounted and removed regions',()=>{
  assert.equal(rectChange(null,a),null);assert.equal(rectChange(a,{...a,width:0}),null);
});
test('layout entries use full element bounds instead of clipped visible heights',()=>{
  const before={x:10,y:200,width:300,height:600},after={...before,y:260};
  const clippedBefore={...before,height:380},clippedAfter={...after,height:320};
  const change=layoutEntryChange(clippedBefore,clippedAfter,after,after);
  assert.equal(change.delta.y,60);assert.equal(change.delta.height,0);assert.equal(change.amount,60);
  assert.equal(layoutEntryChange(clippedBefore,clippedAfter,{...after,height:650},before).delta.height,50);
});

test('layout watch coalesces consecutive animation frames and bounds exported records',()=>{
  const records=[];
  const change=(time,key='panel')=>({kind:'geometry',path:'/',action:'打开选项',time,amount:12,sources:[{key,before:a,after:{...a,y:32},amount:12}]});
  const first=addChange(records,change(0));addChange(records,change(100));assert.equal(records.length,1);assert.equal(first.lastTime,100);
  addChange(records,{...change(120),kind:'layout'});assert.equal(records.length,1);assert.deepEqual(first.methods,['geometry','layout']);
  addChange(records,{...change(400),path:'/lobby.html'},2);addChange(records,change(900),2);
  assert.equal(records.length,2);assert.equal(records[0].path,'/lobby.html');
});
