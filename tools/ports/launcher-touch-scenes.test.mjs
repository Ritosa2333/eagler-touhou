import assert from 'node:assert/strict';
import test from 'node:test';
import {createRequire} from 'node:module';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {runLauncherTouchScenes,runLauncherGamepadMappingScene,runLauncherTouchLayoutScene,runLauncherTouchMagnifierScene} from './launcher-touch-scenes.mjs';
const run=(io,options={})=>runLauncherTouchScenes(io,{fireMode:'toggle',...options});
const launcherRequire=createRequire(new URL('../../launcher/package.json',import.meta.url));
const {build}=launcherRequire('esbuild');
const gamepadMappingBuild=await build({entryPoints:[resolve(fileURLToPath(new URL('../../launcher',import.meta.url)),'src/launcher/gamepad-mapping.mts')],
  bundle:true,write:false,format:'esm',platform:'node'});
const {defaultGamepadProfile,gamepadActionBits}=await import(`data:text/javascript;base64,${Buffer.from(gamepadMappingBuild.outputFiles[0].text).toString('base64')}`);

/**
 * These fixtures test the shared scene's rejection rules against a modeled
 * native consumer. They do not replace the browser fixture or prove that the
 * production DOM controller reaches a real Runtime.
 */
function nativeConsumer({fault,fireMode='toggle'}={}){
  const state={sequence:0,steps:0,x:0,y:0,right:false,physicalFocus:false,
    touchFocus:false,physicalFire:false,gamepadFire:false,touchFire:false};
  const effectiveFocus=()=>state.physicalFocus||state.touchFocus;
  const actions=()=>({available:true,complete:true,phase:'consumer-run',sequence:state.sequence,
    focused:effectiveFocus(),consumed:{focus:effectiveFocus(),fire:state.physicalFire||state.gamepadFire||state.touchFire}});
  return{
    async key(code,down){
      if(code==='ArrowRight')state.right=down;
      if(code==='ShiftLeft')state.physicalFocus=down;
      if(code==='KeyZ'){
        state.physicalFire=down;
        if(!down&&fault==='physical-release-clears-touch')state.touchFire=false;
        if(!down&&fault==='physical-release-drops-touch-later'&&state.touchFire)state.dropTouchAfter=state.steps+2;
      }
    },
    async pointer({control,type}){
      if(control==='fire'&&fireMode==='held-key'){state.touchFire=type==='pointerdown';return;}
      if(type!=='pointerup'){
        if(fault==='focus-pointerup-releases'&&control==='focus'&&type==='pointerdown')state.touchFocus=true;
        return;
      }
      if(control==='focus'){
        if(fault==='focus-pointerup-releases')state.touchFocus=false;
        else state.touchFocus=!state.touchFocus;
      }
      if(control==='fire')state.touchFire=!state.touchFire;
    },
    async gamepad(down){
      state.gamepadFire=down;
      if(!down&&fault==='gamepad-release-clears-touch')state.touchFire=false;
    },
    async step(count){
      for(let i=0;i<count;i++){
        state.sequence+=fault==='consumer-sequence-skips'?2:1;
        state.steps++;
        if(state.dropTouchAfter&&state.steps>=state.dropTouchAfter)state.touchFire=false;
        if(state.right){
          const focused=effectiveFocus();
          const distance=fault==='toggle-has-no-slowdown'&&state.touchFocus?0.1:focused?0.05:0.1;
          state.x+=distance;
        }
      }
    },
    async readActions(){return actions();},
    async observe(){return{position:fault==='missing-native-position'?null:{x:state.x,y:state.y}};},
    async cancel(){
      const wasHeld=state.touchFire;
      state.touchFocus=false;state.touchFire=false;
      if(fault==='held-fire-survives-cancel'&&wasHeld)state.touchFire=true;
      if(fault==='cancel-clears-physical-fire')state.physicalFire=false;
      if(fault==='cancel-clears-gamepad-fire')state.gamepadFire=false;
    },
  };
}

test('Launcher touch scene accepts independent native focus, fire and position observations',async()=>{
  const result=await run(nativeConsumer());
  assert.equal(result.schema,'eagler-touhou/launcher-touch-scenes/1');
  assert.equal(result.complete,false,'scene evidence stays partial until the real browser owns it');
  assert.equal(result.comparisons.length,1);
  assert.ok(result.samples.some(sample=>sample.action.consumed.fire));
  assert.ok(result.samples.some(sample=>sample.action.consumed.focus));
  assert.ok(result.samples.every(sample=>Number.isFinite(sample.position.x)&&Number.isFinite(sample.position.y)));
});

test('held-key products use the same ownership assertions with a held touch contact',async()=>{
  const result=await run(nativeConsumer({fireMode:'held-key'}),{fireMode:'held-key'});
  assert.equal(result.fireMode,'held-key');
  assert.equal(result.gestures.filter(row=>row.control==='fire'&&row.type==='pointerdown').length,4);
});

test('an undeclared fire mode is rejected before a scene runs',async()=>{
  await assert.rejects(runLauncherTouchScenes(nativeConsumer()),/Declare the product touch fire ownership mode/);
});

test('a focus toggle released by pointerup cannot pass as a persistent toggle',async()=>{
  await assert.rejects(run(nativeConsumer({fault:'focus-pointerup-releases'}),{timeoutTicks:3}),
    /toggle-focus-on: native consumer never reached the requested state/);
});

test('a toggle that reports focus without reducing native movement speed fails',async()=>{
  await assert.rejects(run(nativeConsumer({fault:'toggle-has-no-slowdown'})),
    /Toggle focus speed differs from the native Shift reference/);
});

test('physical key release cannot clear the independent touch fire owner',async()=>{
  await assert.rejects(run(nativeConsumer({fault:'physical-release-clears-touch'})),
    /Physical key release removed the independent touch fire owner/);
});

test('touch cancellation cannot clear the physical fire owner',async()=>{
  await assert.rejects(run(nativeConsumer({fault:'cancel-clears-physical-fire'})),
    /Touch cancellation cleared physical fire/);
});

test('gamepad release cannot clear the independent touch fire owner',async()=>{
  await assert.rejects(run(nativeConsumer({fault:'gamepad-release-clears-touch'})),
    /Gamepad release removed the independent touch fire owner/);
});

test('active touch cancellation cannot clear the held gamepad fire owner',async()=>{
  await assert.rejects(run(nativeConsumer({fault:'cancel-clears-gamepad-fire'})),
    /Touch cancellation cleared gamepad fire/);
});

test('held touch Fire must release on cancellation and accept a fresh contact',async()=>{
  await assert.rejects(run(nativeConsumer({fireMode:'held-key',fault:'held-fire-survives-cancel'}),{fireMode:'held-key'}),
    /held-touch-cancel-released: native consumer never reached the requested state/);
});

test('one surviving touch fire tick cannot hide loss later in the held window',async()=>{
  await assert.rejects(run(nativeConsumer({fault:'physical-release-drops-touch-later'})),
    /Physical key release removed the independent touch fire owner/);
});

test('a skipped native consumer sequence fails the scene',async()=>{
  await assert.rejects(run(nativeConsumer({fault:'consumer-sequence-skips'})),
    /native consumer skipped a tick/);
});

test('missing native position is incomplete evidence and fails closed',async()=>{
  await assert.rejects(run(nativeConsumer({fault:'missing-native-position'})),
    /native position is absent/);
});

test('one failed keyboard release cannot suppress other owner cleanup or hide the original failure',async()=>{
  const io=nativeConsumer({fault:'missing-native-position'}),cleaned=[];
  io.key=async code=>{cleaned.push(code);throw Error('keyboard release failed');};
  const gamepad=io.gamepad,cancel=io.cancel;
  io.gamepad=async down=>{cleaned.push('gamepad');await gamepad(down);};
  io.cancel=async()=>{cleaned.push('cancel');await cancel();};
  await assert.rejects(run(io),error=>{
    assert.match(error.message,/native position is absent/);
    assert.equal(error.cleanupErrors.length,3);return true;
  });
  assert.deepEqual(cleaned,['cancel','ArrowRight','ShiftLeft','KeyZ','gamepad','cancel']);
});

const gamepadContract={game:'th08',variant:'normal',mappingSeat:0,nativeSeat:0,nativeSeatSource:'th08 normal local player input consumer',actions:{
  shoot:{bits:1,first:{control:{kind:'button',index:3}},second:{control:{kind:'axis',index:1,rest:0,value:1}}},
  right:{bits:128,first:{control:{kind:'axis',index:0,rest:0,value:1}},second:{control:{kind:'button',index:2}}},
}};

test('production standard profile combines unrelated default actions on the scene controls',()=>{
  const standard=defaultGamepadProfile(true),axisPad={id:'standard',index:1,connected:true,mapping:'standard',
    buttons:Array.from({length:18},()=>({pressed:false,value:0})),axes:[0,1,0,0]};
  const axisShoot={...standard,bindings:{...standard.bindings,shoot:[{kind:'axis',index:1,rest:0,value:1}]}};
  assert.equal(gamepadActionBits(axisPad,axisShoot),1|(1<<5),
    'axis1+1 emits both mapped Shoot and the untouched production Down binding');
  const buttonPad={...axisPad,buttons:axisPad.buttons.map((button,index)=>index===2?{pressed:true,value:1}:button),axes:[0,0,0,0]};
  const buttonRight={...standard,bindings:{...standard.bindings,right:[{kind:'button',index:2}]}};
  assert.equal(gamepadActionBits(buttonPad,buttonRight),(1<<7)|(1<<10),
    'button2 emits both mapped Right and the untouched production Action binding');
});

function gamepadRuntime({fault}={}){
  let model={enabled:false,seats:['',''],profiles:{}};const stored={value:null},devices=[
    {key:'controller-A',name:'Virtual A',index:0,buttons:{},axes:[0,0]},
    {key:'controller-B',name:'Virtual B',index:1,buttons:{},axes:[0,0]},
  ],samples=[];
  const state={sequence:0,fire:false,x:320,y:240};let blocked=new Set([0,1]);
  const persist=()=>{stored.value=structuredClone(model);};
  const snapshot=()=>({enabled:model.enabled,seats:[...model.seats],profiles:structuredClone(model.profiles),devices:devices.map(({key,name,index,buttons,axes})=>({key,name,index,
    buttons:Object.entries(buttons).filter(([,pressed])=>pressed).map(([buttonIndex])=>({index:Number(buttonIndex),pressed:true,value:1})),axes:[...axes]}))});
  const sampleBits=(seat)=>{
    const key=model.seats[seat],device=devices.find(item=>item.key===key);if(!model.enabled||!device)return -1;
    const pad={id:device.name,index:device.index,connected:true,mapping:'standard',buttons:Array.from({length:18},(_,index)=>{
      const pressed=!!device.buttons[index];return{pressed,value:pressed?1:0};}),axes:[...device.axes]};
    const profile=model.profiles[key]??defaultGamepadProfile(true);
    const bits=gamepadActionBits(pad,profile);
    if(blocked.has(seat)){if(bits===0)blocked.delete(seat);return 0;}return bits;
  };
  const deviceAction=(device,action)=>{
    const pad={id:device.name,index:device.index,connected:true,mapping:'standard',buttons:Array.from({length:18},(_,index)=>{
      const pressed=!!device.buttons[index];return{pressed,value:pressed?1:0};}),axes:[...device.axes]};
    return (gamepadActionBits(pad,model.profiles[device.key]??defaultGamepadProfile(true))& (action==='shoot'?1:128))!==0;
  };
  const map={
    async refresh(){return snapshot();},snapshot,
    async setEnabled(enabled){model.enabled=enabled;blocked=new Set([0,1]);persist();return snapshot();},
    async select(seat,key){model.seats[seat]=key;if(key&&!model.profiles[key])model.profiles[key]=defaultGamepadProfile(true);blocked.add(seat);persist();return snapshot();},
    async bind(key,action,controls){model.profiles[key]??=defaultGamepadProfile(true);
      model.profiles[key]={...model.profiles[key],bindings:{...model.profiles[key].bindings,[action]:structuredClone(controls)}};
      blocked.add(0);persist();return snapshot();},
    async setButton(key,index,pressed){devices.find(item=>item.key===key).buttons[index]=pressed;return{key,index,pressed};},
    async setAxis(key,index,value){devices.find(item=>item.key===key).axes[index]=value;return{key,index,value};},
    async reorder(assignments){for(const {key,index} of assignments)devices.find(item=>item.key===key).index=index;return snapshot();},
    async cursor(){return samples.length;},async samplesSince(cursor){return samples.slice(cursor);},
    async neutralize(){if(fault==='neutralize-fails')throw Error('gamepad neutralize failed');
      for(const device of devices){device.buttons={};device.axes.fill(0);}},
    async reload(){model=fault==='reload-drops-mapping'?{enabled:true,seats:['',''],profiles:{}}:structuredClone(stored.value??model);blocked=new Set([0,1]);return snapshot();},
  };
  return{
    mapping:map,
    async step(count){for(let n=0;n<count;n++){
      const key=model.seats[0],device=devices.find(item=>item.key===key),bits=sampleBits(0);
      const observedBits=fault==='missing-map-sample'?undefined:fault==='wrong-sample-seat'?0:bits;
      if(observedBits!==undefined)samples.push({seat:fault==='wrong-sample-seat'?1:0,bits:observedBits,selectedKey:key,selectedIndex:device?.index??null,
        devices:snapshot().devices});
      const other=devices.find(item=>item.key!==key);
      state.sequence++;state.fire=fault==='mapped-bits-without-native-fire'?false:!!(bits&1)||(fault==='nonselected-device-bleed'&&deviceAction(other,'shoot'));
      if(bits&128)state.x++;
    }},
    async readActions(){return{available:true,complete:true,phase:'consumer-run',sequence:state.sequence,
      localSeat:fault==='wrong-local-seat'?1:0,consumed:{fire:state.fire,focus:false}};},
    async observe(){return{position:{x:state.x,y:state.y}};},
  };
}

test('Launcher gamepad scene connects two production identities, retains selection after reorder and reload, and isolates the other source',async()=>{
  const result=await runLauncherGamepadMappingScene(gamepadRuntime(),{contract:gamepadContract});
  assert.equal(result.complete,true);assert.equal(result.reloaded,true);
  assert.equal(result.profileIsolation?.source,'production GamepadMappingSnapshot after selecting each connected device');
  for(const key of ['controller-A','controller-B']){
    const profile=result.profileIsolation.devices.find(item=>item.key===key);
    assert.ok(profile,`${key}: original and isolated production profile evidence is absent`);
    assert.deepEqual(profile.isolatedBindings,Object.fromEntries(profile.clearedActions.map(action=>[action,[]])),
      `${key}: the isolated scene must clear all declared production actions`);
  }
  const secondOriginal=result.profileIsolation.devices.find(item=>item.key==='controller-B').originalBindings;
  assert.ok(secondOriginal.down.some(control=>control.kind==='axis'&&control.index===1&&control.value===1),
    'The regression fixture must start with production standard Down axis1+1 binding');
  assert.ok(secondOriginal.action.some(control=>control.kind==='button'&&control.index===2),
    'The regression fixture must start with production standard Action button2 binding');
  const secondShoot=result.operations.find(item=>item.label==='second-device-shoot');
  assert.equal(secondShoot?.sample?.bits,1,'The isolated production mapping must emit exact Shoot bits for the second device');
  assert.equal(secondShoot.sample.devices.find(device=>device.key==='controller-B').axes[1],1,
    'The exact second-device Shoot sample must retain the real axis1 stimulus');
  assert.ok(result.operations.some(item=>item.kind==='index-reorder'&&item.before!==item.after));
  assert.ok(result.operations.some(item=>item.kind==='axis'&&item.label==='second-device-right-axis'));
  assert.ok(result.operations.some(item=>item.kind==='button'&&item.label==='first-device-shoot'&&item.control?.kind==='button'&&item.control.index===3),
    'The first native Shoot must use a non-default button, independent of the raw gamepad fallback');
  assert.ok(result.ticks.some(tick=>tick.label==='nonselected-device-button'&&tick.samples.every(row=>row.selectedKey==='controller-A'&&row.bits===0)));
  assert.ok(result.stimuli.some(row=>row.label==='nonselected-device-button:nonselected-active'&&row.key==='controller-B'&&
    row.active===true&&row.receipt?.value===1),'The negative row must retain the actual non-selected shoot-control stimulus receipt');
  assert.ok(result.stimuli.some(row=>row.label==='nonselected-device-axis:nonselected-active'&&row.key==='controller-B'&&
    row.active===true&&row.receipt?.pressed===true),'The negative row must retain the actual non-selected right-control stimulus receipt');
});

test('Launcher gamepad scene rejects native cross-seat/source bleed and missing mapper samples',async()=>{
  await assert.rejects(runLauncherGamepadMappingScene(gamepadRuntime({fault:'wrong-local-seat'}),{contract:gamepadContract}),/different local seat/);
  await assert.rejects(runLauncherGamepadMappingScene(gamepadRuntime({fault:'nonselected-device-bleed'}),{contract:gamepadContract}),/unselected controller reached/);
  await assert.rejects(runLauncherGamepadMappingScene(gamepadRuntime({fault:'missing-map-sample'}),{contract:gamepadContract}),/sample journal did not cover/);
  await assert.rejects(runLauncherGamepadMappingScene(gamepadRuntime({fault:'mapped-bits-without-native-fire'}),{contract:gamepadContract}),error=>{
    assert.match(error.message,/mapper emitted the requested bits, but the native fire consumer stayed inactive/);
    assert.equal(error.evidence?.label,'first-device-shoot-native-down');
    assert.ok(error.evidence?.recentTicks?.some(tick=>tick.mappedRows.some(row=>row.bits===1&&row.devices?.some(device=>device.buttons.some(button=>button.index===3)))));
    return true;
  });
  await assert.rejects(runLauncherGamepadMappingScene(gamepadRuntime({fault:'reload-drops-mapping'}),{contract:gamepadContract}),/selected controller identity did not survive model reload/);
  await assert.rejects(runLauncherGamepadMappingScene(gamepadRuntime({fault:'neutralize-fails'}),{contract:gamepadContract}),error=>{
    assert.match(error.message,/gamepad neutralize failed/);assert.equal(error.cleanupErrors?.length,1);return true;
  });
});

const layoutContract={game:'th08',variant:'normal',storageKey:'eagler-touhou-touch-layout-v1',control:'fire',
  viewports:{landscape:{width:960,height:640},portrait:{width:640,height:960}},profiles:{
    landscape:{fire:{x:.32,y:.38,scale:1.25}},portrait:{fire:{x:.68,y:.62,scale:1.4}},
  }};

function layoutRuntime({fault}={}){
  const state={viewport:null,draft:null,storage:{version:6,profiles:{landscape:null,portrait:null}},sequence:9,fire:false,focus:false,reads:0};
  return{
    layout:{
      async setViewport(value){state.viewport={...value};return{...state.viewport};},
      async begin(orientation){state.draft??=structuredClone(state.storage);state.orientation=orientation;state.draft.profiles[orientation]??={controls:{},viewport:{x:0}};
        return{...state.draft,target:fault==='foreign-layout-surface'?'fixture-panel':'player',source:'player-controls',controls:
          ['focus','fire','bomb','joystick','escape','restart','function','thpracTab','thpracMenu'].map(name=>({name,id:`touch-${name}`,mounted:fault!=='missing-player-control'||name!=='focus',owner:'player'}))};},
      async updateControl(name,placement){state.draft.profiles[state.orientation].controls[name]=structuredClone(placement);return state.draft;},
      async save(){const orientation=state.orientation,source=structuredClone(state.draft.profiles[orientation]);
        if(fault==='portrait-overwrites-landscape'&&orientation==='portrait')state.storage.profiles.landscape=source;
        state.storage.profiles[orientation]=source;return{persisted:true,value:structuredClone(state.storage)};},
      async reload(){state.draft=null;return{saved:structuredClone(state.storage)};},
      async storageSnapshot(){return{key:fault==='wrong-storage-owner'?'other-key':layoutContract.storageKey,value:structuredClone(state.storage)};},
      async apply(orientation,control){
        let actual=state.storage.profiles[orientation];if(fault==='wrong-orientation')actual=state.storage.profiles[orientation==='landscape'?'portrait':'landscape'];
        const placement=actual?.controls?.[control];if(!placement)return{available:false,orientation};
        const safeRect={left:0,top:0,width:state.viewport.width,height:state.viewport.height};
        const scale=fault==='wrong-scale'?1:placement.scale,w=40*scale,h=40*scale;
        const x=placement.x*safeRect.width,y=placement.y*safeRect.height;
        return{available:true,target:fault==='wrong-target'?'fixture-panel':'player',orientation,storageKey:layoutContract.storageKey,placement:structuredClone(placement),safeRect,
          controlRect:{left:x-w/2,top:y-h/2,width:w,height:h},renderedScale:scale};
      },
    },
    async readActions(){state.reads++;return{available:true,complete:true,phase:'consumer-run',sequence:state.sequence,
      consumed:{fire:state.fire||fault==='input-side-effect'&&state.reads>1,focus:state.focus}};},
  };
}

test('Launcher layout scene persists distinct portrait and landscape profiles and verifies actual applied geometry without gameplay input',async()=>{
  const result=await runLauncherTouchLayoutScene(layoutRuntime(),{contract:layoutContract});
  assert.equal(result.complete,true);assert.deepEqual(result.orientations.map(row=>row.orientation),['landscape','portrait']);
  assert.notDeepEqual(result.orientations[0].stored.value.profiles.landscape.controls.fire,
    result.orientations[1].stored.value.profiles.portrait.controls.fire);
});

test('Launcher layout scene rejects orientation overwrite, wrong storage owner, wrong scale, and input side effects',async()=>{
  await assert.rejects(runLauncherTouchLayoutScene(layoutRuntime({fault:'portrait-overwrites-landscape'}),{contract:layoutContract}),/overwrote the previously saved landscape layout/);
  await assert.rejects(runLauncherTouchLayoutScene(layoutRuntime({fault:'wrong-storage-owner'}),{contract:layoutContract}),/stored under a different owner/);
  await assert.rejects(runLauncherTouchLayoutScene(layoutRuntime({fault:'wrong-scale'}),{contract:layoutContract}),/actual control scaling differs/);
  await assert.rejects(runLauncherTouchLayoutScene(layoutRuntime({fault:'wrong-target'}),{contract:layoutContract}),/not the launched Player surface/);
  await assert.rejects(runLauncherTouchLayoutScene(layoutRuntime({fault:'foreign-layout-surface'}),{contract:layoutContract}),/layout capture did not use the launched Player surface/);
  await assert.rejects(runLauncherTouchLayoutScene(layoutRuntime({fault:'missing-player-control'}),{contract:layoutContract}),/missing from the launched Player surface/);
  await assert.rejects(runLauncherTouchLayoutScene(layoutRuntime({fault:'input-side-effect'}),{contract:layoutContract}),/changed native fire input/);
});

const magnifierContract={game:'th08',variant:'normal',traceSource:'native-touch-trace-v2',pointerId:19471,
  pinch:{pointerIds:[19472,19473],start:[{x:.4,y:.5},{x:.6,y:.5}],end:[{x:.3,y:.5},{x:.7,y:.5}]},
  gesture:{start:{x:.25,y:.5},end:{x:.5,y:.5}}};

function magnifierRuntime({fault}={}){
  const state={scale:1,available:false,sequence:0,target:{x:320,y:240},origin:null,pending:null,events:[],drops:0};
  const rect=()=>({left:100,top:50,width:fault==='no-zoom-geometry'?800:800*state.scale,height:fault==='no-zoom-geometry'?600:600*state.scale});
  const normalized=event=>({x:(event.clientX-rect().left)/rect().width,y:(event.clientY-rect().top)/rect().height});
  const magnifier={snapshot:()=>({active:state.available,scale:state.scale,target:fault==='wrong-target'?'fixture-panel':'player'}),frameRect:async()=>rect(),
    async setAvailable(value){state.available=value;},
    async pinch({pointerIds,start,end}){
      for(let index=0;index<2;index++)state.events.push({type:'pointerdown',pointerId:pointerIds[index],pointerType:'touch',
        clientX:start[index].clientX,clientY:start[index].clientY,isTrusted:fault!=='untrusted-pinch',surfaceOwner:fault==='foreign-surface'?'outside-player':'player',surfaceTarget:'touchDirectSurface'});
      for(let index=0;index<2;index++)state.events.push({type:'pointermove',pointerId:pointerIds[index],pointerType:'touch',
        clientX:end[index].clientX,clientY:end[index].clientY,isTrusted:fault!=='untrusted-pinch',surfaceOwner:fault==='foreign-surface'?'outside-player':'player',surfaceTarget:'touchDirectSurface'});
      for(let index=0;index<2;index++)state.events.push({type:'pointerup',pointerId:pointerIds[index],pointerType:'touch',
        clientX:end[index].clientX,clientY:end[index].clientY,isTrusted:fault!=='untrusted-pinch',surfaceOwner:fault==='foreign-surface'?'outside-player':'player',surfaceTarget:'touchDirectSurface'});
      if(fault==='event-overflow')state.drops++;
      if(fault!=='pinch-not-applied')state.scale=1.5;
    },
    async pointerCursor(){return state.events.length;},async pointerEventsSince(cursor){return{events:state.events.slice(cursor),drops:state.drops};},
    async pointerAt(event){
      state.events.push({...event,type:event.type,pointerType:'touch',isTrusted:fault!=='untrusted-single-pointer',
        surfaceOwner:fault==='foreign-surface'?'outside-player':'player',surfaceTarget:'touchDirectSurface'});
      if(event.type==='pointerdown')state.origin={point:normalized(event),target:{...state.target}};
      if(event.type==='pointermove'&&state.origin){const next=normalized(event),factor=fault==='wrong-zoom-mapping'&&state.scale>1?2:1;
        state.pending={x:state.origin.target.x+(next.x-state.origin.point.x)*640*factor,
          y:state.origin.target.y+(next.y-state.origin.point.y)*480*factor};}
      if(event.type==='pointerup')state.origin=null;
    },async reset(){state.scale=1;state.available=true;},async cancel(){state.origin=null;state.pending=null;},
  };
  return{magnifier,
    async step(){state.sequence++;if(state.pending){state.target=state.pending;state.pending=null;}},
    async readNativeTarget(){return{available:true,complete:true,source:'native-touch-trace-v2',sequence:state.sequence,target:{...state.target}};},
  };
}

test('magnifier scene compares trusted absolute screen Pointer coordinates with the native Runtime target before and after zoom',async()=>{
  const result=await runLauncherTouchMagnifierScene(magnifierRuntime(),{contract:magnifierContract});
  assert.equal(result.complete,true);assert.notEqual(result.gestures[0].screen.move.clientX,result.gestures[1].screen.move.clientX);
  assert.equal(new Set(result.comparison.pinch.events.filter(row=>row.type==='pointerdown').map(row=>row.pointerId)).size,2);
  assert.equal(Math.fround(result.gestures[0].delta.x),Math.fround(result.gestures[1].delta.x));
});

test('magnifier scene rejects untrusted input, an unchanged display, and a coordinate shift in the native trace',async()=>{
  await assert.rejects(runLauncherTouchMagnifierScene(magnifierRuntime({fault:'untrusted-pinch'}),{contract:magnifierContract}),/Magnifier pinch input must come from the browser touch path/);
  await assert.rejects(runLauncherTouchMagnifierScene(magnifierRuntime({fault:'untrusted-single-pointer'}),{contract:magnifierContract}),/Pointer input must come from the browser input path/);
  await assert.rejects(runLauncherTouchMagnifierScene(magnifierRuntime({fault:'no-zoom-geometry'}),{contract:magnifierContract}),/did not change the actual displayed Runtime frame geometry/);
  await assert.rejects(runLauncherTouchMagnifierScene(magnifierRuntime({fault:'wrong-zoom-mapping'}),{contract:magnifierContract}),/different native x target after zoom/);
  await assert.rejects(runLauncherTouchMagnifierScene(magnifierRuntime({fault:'pinch-not-applied'}),{contract:magnifierContract}),/did not activate production zoom/);
  await assert.rejects(runLauncherTouchMagnifierScene(magnifierRuntime({fault:'event-overflow'}),{contract:magnifierContract}),/Pointer journal overflowed/);
  await assert.rejects(runLauncherTouchMagnifierScene(magnifierRuntime({fault:'wrong-target'}),{contract:magnifierContract}),/not attached to the launched Player surface/);
  await assert.rejects(runLauncherTouchMagnifierScene(magnifierRuntime({fault:'foreign-surface'}),{contract:magnifierContract}),/did not reach a touch surface inside the launched Player/);
});
