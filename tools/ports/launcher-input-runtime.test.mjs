import assert from 'node:assert/strict';
import test from 'node:test';
import {runLauncherInputRuntime} from './launcher-input-runtime.mjs';

const target={game:'th08',variant:'normal',epoch:17};
const contract={...target,inputMasks:{fire:1,focus:4,bomb:2,escape:8},
  movement:{source:'th08 production movement owner',precision:{unitsPerPixel:1}},
  magnifier:{traceSource:'native-touch-trace-v2',pointerId:19471,
    pinch:{pointerIds:[19472,19473],start:[{x:.4,y:.5},{x:.6,y:.5}],end:[{x:.3,y:.5},{x:.7,y:.5}]},
    gesture:{start:{x:.25,y:.5},end:{x:.5,y:.5}}}};

function browserHarness({failMove=false,failDetach=false}={}){
  const state={scale:1,available:false,sequence:0,target:{x:320,y:240},origin:null,pending:null,
    events:[],active:new Map(),installed:0,disposed:0,detached:false,sendCalls:[]};
  const rect=()=>({left:100,top:50,width:800*state.scale,height:600*state.scale});
  const normalized=point=>({x:(point.clientX-rect().left)/rect().width,y:(point.clientY-rect().top)/rect().height});
  const fixture={
    install(value){state.installed++;return{game:value.game,epoch:value.epoch,owner:'real-launcher-touch-fixture'};},
    dispose(){state.disposed++;},
    magnifierScene(operation,value={}){
      if(operation==='snapshot')return{active:state.available,scale:state.scale,target:'player',surfaceOwner:'player'};
      if(operation==='frameRect')return rect();
      if(operation==='setAvailable'){state.available=value.available;return{active:state.available};}
      if(operation==='pointerCursor')return state.events.length;
      if(operation==='pointerEventsSince')return{events:state.events.slice(value.cursor),drops:0};
      if(operation==='reset'){state.scale=1;return{scale:1};}
      if(operation==='cancel'){state.origin=null;state.pending=null;return{cancelled:true};}
      throw Error(`unexpected fixture operation ${operation}`);
    },
  };
  const fakeWindow={EaglerLauncherTouchFixture:fixture,innerWidth:1024,innerHeight:768};
  const page={
    async evaluate(fn,arg){const previous=globalThis.window;globalThis.window=fakeWindow;
      try{return await fn(arg);}finally{if(previous===undefined)delete globalThis.window;else globalThis.window=previous;}},
    viewportSize:()=>({width:1024,height:768}),
  };
  const frame={evaluate:async()=>null};
  const session={
    async send(method,params){
      if(method!=='Input.dispatchTouchEvent')throw Error(`unexpected CDP method ${method}`);
      const {type,touchPoints=[]}=params;state.sendCalls.push({type,touchPoints:structuredClone(touchPoints)});
      if(type==='touchStart'){
        for(const point of touchPoints){state.active.set(point.id,{clientX:point.x,clientY:point.y});
          state.events.push({type:'pointerdown',pointerId:point.id+10000,pointerType:'touch',clientX:point.x,clientY:point.y,isTrusted:true,
            surfaceOwner:'player',surfaceTarget:'touchDirectSurface'});}
        if(touchPoints.length===1)state.origin={id:touchPoints[0].id,point:normalized({clientX:touchPoints[0].x,clientY:touchPoints[0].y}),target:{...state.target}};
      }else if(type==='touchMove'){
        if(failMove)throw Error('controlled CDP move failure');
        for(const point of touchPoints){
          const previous=state.active.get(point.id);if(!previous)throw Error(`unknown touch ${point.id}`);
          state.active.set(point.id,{clientX:point.x,clientY:point.y});
          state.events.push({type:'pointermove',pointerId:point.id+10000,pointerType:'touch',clientX:point.x,clientY:point.y,isTrusted:true,
            surfaceOwner:'player',surfaceTarget:'touchDirectSurface'});
          if(touchPoints.length===2){state.scale=1.5;state.available=true;}
          else if(state.origin){
            const next=normalized({clientX:point.x,clientY:point.y});
            state.pending={x:state.origin.target.x+(next.x-state.origin.point.x)*640,
              y:state.origin.target.y+(next.y-state.origin.point.y)*480};
          }
        }
      }else if(type==='touchEnd'){
        const remaining=new Set(touchPoints.map(point=>point.id));
        for(const [id,point] of state.active){if(!remaining.has(id)){
          state.events.push({type:'pointerup',pointerId:id+10000,pointerType:'touch',clientX:point.clientX,clientY:point.clientY,isTrusted:true,
            surfaceOwner:'player',surfaceTarget:'touchDirectSurface'});
          state.active.delete(id);if(state.origin&&state.origin.id===id)state.origin=null;
        }}
        for(const point of touchPoints)state.active.set(point.id,{clientX:point.x,clientY:point.y});
      }else if(type==='touchCancel'){
        for(const [id,point] of state.active)state.events.push({type:'pointercancel',pointerId:id+10000,pointerType:'touch',
          clientX:point.clientX,clientY:point.clientY,isTrusted:true,surfaceOwner:'player',surfaceTarget:'touchDirectSurface'});
        state.active.clear();state.origin=null;
      }else throw Error(`unexpected CDP touch type ${type}`);
    },
    async detach(){state.detached=true;if(failDetach)throw Error('controlled CDP detach failure');},
  };
  const context={newCDPSession:async value=>{assert.equal(value,page);return session;}};
  return{state,page,frame,context,fixture};
}

test('Launcher magnifier facade routes trusted CDP touch through the fixture and native trace, then cleans up',async()=>{
  const harness=browserHarness();
  const result=await runLauncherInputRuntime({kind:'magnifier',...harness,target,contract,step:async()=>{
    harness.state.sequence++;
    if(harness.state.pending){harness.state.target=harness.state.pending;harness.state.pending=null;}
  },readTrace:async()=>({available:true,complete:true,source:'native-touch-trace-v2',sequence:harness.state.sequence,
    target:{...harness.state.target}})});
  assert.equal(result.complete,true);
  assert.equal(result.kind,'magnifier');
  assert.equal(result.browserEvidence.trustedCdpTouch,true);
  assert.equal(harness.state.installed,1);
  assert.equal(harness.state.disposed,1);
  assert.equal(harness.state.detached,true);
  assert.ok(harness.state.sendCalls.some(call=>call.type==='touchStart'&&call.touchPoints.length===2));
  assert.ok(result.comparison.pinch.events.every(event=>event.isTrusted));
  assert.ok(result.gestures.every(gesture=>gesture.events.every(event=>event.isTrusted)));
  assert.equal(result.gestures[0].events[0].pointerId,29471,
    'The shared scene verifies the PointerEvent identity observed from the browser');
});

test('Launcher magnifier facade preserves the original input error and reports teardown errors',async()=>{
  const harness=browserHarness({failMove:true,failDetach:true});
  await assert.rejects(runLauncherInputRuntime({kind:'magnifier',...harness,target,contract,step:async()=>{harness.state.sequence++;},
    readTrace:async()=>({available:true,complete:true,source:'native-touch-trace-v2',sequence:harness.state.sequence,
      target:{...harness.state.target}})}),error=>{
    assert.match(error.message,/controlled CDP move failure/);
    assert.ok(error.cleanupErrors?.some(item=>item.operation==='detach-cdp'&&/controlled CDP detach failure/.test(item.error)));
    assert.equal(harness.state.disposed,1);
    assert.equal(harness.state.detached,true);
    assert.ok(harness.state.sendCalls.some(call=>call.type==='touchCancel'));
    return true;
  });
});

test('Launcher input facade validates variant contract before mounting a fixture',async()=>{
  const harness=browserHarness();
  await assert.rejects(runLauncherInputRuntime({kind:'magnifier',...harness,target:{...target,variant:'multiplayer'},contract,
    step:async()=>{},readTrace:async()=>({})}),/contract variant differs/);
  assert.equal(harness.state.installed,0);
});

function gamepadBrowserHarness(){
  const actionNames=['shoot','bomb','focus','pause','up','down','left','right','skip','confirm','action','restart'];
  const emptyProfile=()=>({threshold:.5,bindings:Object.fromEntries(actionNames.map(action=>[action,[]]))});
  const pads=[{key:'controller-A',name:'Controller A',index:0,buttons:Array(4).fill(false),axes:Array(4).fill(0)},
    {key:'controller-B',name:'Controller B',index:1,buttons:Array(4).fill(false),axes:Array(4).fill(0)}];
  const state={enabled:false,seats:['',''],profiles:{},sequence:0,fire:false,x:0,samples:[],installed:0,disposed:0};
  const snapshot=()=>({enabled:state.enabled,seats:[...state.seats],profiles:structuredClone(state.profiles),
    devices:pads.map(({key,name,index,buttons,axes})=>({key,name,index,standard:true,
      buttons:buttons.map((pressed,buttonIndex)=>pressed?{index:buttonIndex,pressed:true,value:1}:null).filter(Boolean),axes:[...axes]}))});
  const api={
    install(value){state.installed++;return{game:value.game,epoch:value.epoch};},
    dispose(){state.disposed++;},
    installGamepadMapping(){return{devices:snapshot().devices,seat:''};},
    gamepadMapping(operation,value={}){
      if(operation==='refresh'||operation==='snapshot')return snapshot();
      if(operation==='setEnabled'){state.enabled=value.enabled;return snapshot();}
      if(operation==='select'){state.seats[value.seat]=value.key;if(value.key&&!state.profiles[value.key])state.profiles[value.key]=emptyProfile();return snapshot();}
      if(operation==='bind'){
        state.profiles[value.key]??=emptyProfile();
        state.profiles[value.key].bindings[value.action]=structuredClone(value.controls);return snapshot();
      }
      if(operation==='button'){pads.find(pad=>pad.key===value.key).buttons[value.index]=value.pressed;return value;}
      if(operation==='axis'){pads.find(pad=>pad.key===value.key).axes[value.index]=value.value;return value;}
      if(operation==='reorder'){for(const item of value.assignments)pads.find(pad=>pad.key===item.key).index=item.index;return snapshot();}
      if(operation==='cursor')return state.samples.length;
      if(operation==='samplesSince')return state.samples.slice(value.cursor);
      if(operation==='neutralize'){for(const pad of pads){pad.buttons.fill(false);pad.axes.fill(0);}return snapshot();}
      if(operation==='reload')return snapshot();
      throw Error(`unexpected gamepad operation ${operation}`);
    },
  };
  const fakeWindow={EaglerLauncherTouchFixture:api};
  const page={async evaluate(fn,arg){const previous=globalThis.window;globalThis.window=fakeWindow;
    try{return await fn(arg);}finally{if(previous===undefined)delete globalThis.window;else globalThis.window=previous;}}};
  const frame={evaluate:async()=>null};
  const active=(key,control)=>{
    const pad=pads.find(item=>item.key===key);if(!pad)return false;
    return control.kind==='button'?pad.buttons[control.index]===true:pad.axes[control.index]===control.value;
  };
  return{state,page,frame,context:{newCDPSession:async()=>{throw Error('gamepad scene must not create CDP session');}},
    api,pads,async step(){
      state.sequence++;const key=state.seats[0],bindings=state.profiles[key]?.bindings??{};
      const shoot=(bindings.shoot??[]).some(control=>active(key,control)),right=(bindings.right??[]).some(control=>active(key,control));
      const bits=(shoot?1:0)|(right?128:0);state.fire=shoot;if(right)state.x++;
      const pad=pads.find(item=>item.key===key);
      state.samples.push({seat:0,bits,selectedKey:key,selectedIndex:pad?.index??null,
        devices:snapshot().devices});
    },
    readActions:async()=>({available:true,complete:true,phase:'consumer-run',sequence:state.sequence,localSeat:0,consumed:{fire:state.fire}}),
    observe:async()=>({position:{x:state.x,y:0}}),
  };
}

const facadeGamepadContract={...target,inputMasks:{fire:1},movement:{source:'th08 normal movement owner'},gamepad:{
  mappingSeat:0,nativeSeat:0,nativeSeatSource:'th08 normal local player input consumer',actions:{
    shoot:{first:{control:{kind:'button',index:0}},second:{control:{kind:'axis',index:1,rest:0,value:1}}},
    right:{first:{control:{kind:'axis',index:0,rest:0,value:1}},second:{control:{kind:'button',index:2}}},
  }}};

test('Launcher gamepad facade connects fixture operations to the real mapping scene and native consumers',async()=>{
  const harness=gamepadBrowserHarness();
  const result=await runLauncherInputRuntime({kind:'gamepad',...harness,target,contract:facadeGamepadContract,
    step:harness.step,readActions:harness.readActions,observe:harness.observe});
  assert.equal(result.complete,true);
  assert.equal(result.operations.some(row=>row.kind==='axis'),true);
  assert.equal(harness.state.installed,1);assert.equal(harness.state.disposed,1);
  assert.equal(result.contractEvidence.inputMasks.fire,1);
  assert.equal(result.contractEvidence.movement.source,'th08 normal movement owner');
  assert.deepEqual(result.contractEvidence.gamepad.firstShootControl,{kind:'button',index:3});
  assert.ok(result.operations.some(row=>row.label==='first-device-shoot'&&row.control?.index===3),
    'The facade must test a non-default shoot binding on the first controller');
});

function layoutBrowserHarness(){
  const state={viewport:{width:1024,height:768},saved:{version:6,profiles:{landscape:null,portrait:null}},draft:null,
    orientation:null,sequence:11,installed:0,disposed:0};
  const api={
    install(value){state.installed++;return{game:value.game,epoch:value.epoch};},dispose(){state.disposed++;},
    layoutScene(operation,value={}){
      if(operation==='begin'){state.orientation=value.orientation;state.draft=structuredClone(state.saved);
        state.draft.profiles[value.orientation]??={controls:{},viewport:{x:0}};
        return{...state.draft,target:'player',source:'player-controls',controls:
          ['focus','fire','bomb','joystick','escape','restart','function','thpracTab','thpracMenu']
            .map(name=>({name,id:`touch-${name}`,mounted:true,owner:'player'}))};}
      if(operation==='updateControl'){state.draft.profiles[state.orientation].controls[value.name]=structuredClone(value.placement);return state.draft;}
      if(operation==='save'){state.saved=structuredClone(state.draft);return{persisted:true,value:structuredClone(state.saved)};}
      if(operation==='reload'){state.draft=null;return{saved:structuredClone(state.saved)};}
      if(operation==='storageSnapshot')return{key:layoutBrowserContract.layout.storageKey,value:structuredClone(state.saved)};
      if(operation==='apply'){
        const placement=state.saved.profiles[value.orientation]?.controls?.[value.control];if(!placement)return{available:false};
        const safeRect={left:0,top:0,width:state.viewport.width,height:state.viewport.height},width=40*placement.scale,height=40*placement.scale;
        return{available:true,target:'player',orientation:value.orientation,storageKey:layoutBrowserContract.layout.storageKey,placement:structuredClone(placement),safeRect,
          controlRect:{left:placement.x*safeRect.width-width/2,top:placement.y*safeRect.height-height/2,width,height},renderedScale:placement.scale};
      }
      throw Error(`unexpected layout operation ${operation}`);
    },
  };
  const fakeWindow={EaglerLauncherTouchFixture:api,innerWidth:state.viewport.width,innerHeight:state.viewport.height};
  const page={async evaluate(fn,arg){const previous=globalThis.window;globalThis.window=fakeWindow;
      try{return await fn(arg);}finally{if(previous===undefined)delete globalThis.window;else globalThis.window=previous;}},
    viewportSize:()=>({...state.viewport}),async setViewportSize(value){state.viewport={...value};fakeWindow.innerWidth=value.width;fakeWindow.innerHeight=value.height;}};
  return{state,page,frame:{evaluate:async()=>null},context:{newCDPSession:async()=>{throw Error('layout scene must not create CDP session');}},
    readActions:async()=>({available:true,complete:true,phase:'consumer-run',sequence:state.sequence,consumed:{fire:false,focus:false}})};
}

const layoutBrowserContract={...target,inputMasks:{fire:1},movement:{source:'th08 normal movement owner'},layout:{
  storageKey:'eagler-touhou-touch-layout-v1',control:'fire',viewports:{landscape:{width:960,height:640},portrait:{width:640,height:960}},profiles:{
    landscape:{fire:{x:.32,y:.38,scale:1.25,priority:1}},portrait:{fire:{x:.68,y:.62,scale:1.4,priority:1}},
  }}};

test('Launcher layout facade applies real Playwright viewport changes and restores the caller viewport',async()=>{
  const harness=layoutBrowserHarness();
  const result=await runLauncherInputRuntime({kind:'layout',...harness,target,contract:layoutBrowserContract,readActions:harness.readActions});
  assert.equal(result.complete,true);
  assert.deepEqual(harness.state.viewport,{width:1024,height:768});
  assert.deepEqual(result.browserEvidence.viewportRestored,{width:1024,height:768});
  assert.equal(harness.state.installed,1);assert.equal(harness.state.disposed,1);
});
