import assert from 'node:assert/strict';
import {
  runLauncherGamepadMappingScene,
  runLauncherTouchLayoutScene,
  runLauncherTouchMagnifierScene,
} from './launcher-touch-scenes.mjs';

const FIXTURE='EaglerLauncherTouchFixture';
const GAMEPAD_BITS=Object.freeze({shoot:1,right:1<<7});
const GAMEPAD_TEST_CONTROLS=Object.freeze({firstShoot:Object.freeze({kind:'button',index:3})});

function requirePlaywrightInputs({kind,page,frame,context,target,step,readActions,observe,readTrace,contract}){
  assert.ok(['gamepad','layout','magnifier'].includes(kind),`Unsupported Launcher input scene ${String(kind)}`);
  assert.equal(typeof page?.evaluate,'function','Launcher input scene needs its Playwright host page');
  assert.equal(typeof frame?.evaluate,'function','Launcher input scene needs its launched Runtime frame');
  assert.equal(typeof context?.newCDPSession,'function','Launcher input scene needs its Playwright browser context');
  assert.ok(target&&typeof target.game==='string'&&target.game.length>0,'Launcher input scene needs the selected product target');
  assert.ok(target.variant==='normal'||target.variant==='multiplayer','Launcher input scene needs an explicit Runtime variant');
  assert.ok(Number.isSafeInteger(target.epoch)&&target.epoch>=0,'Launcher input scene needs the active Runtime epoch');
  assert.equal(contract?.game,target.game,'Launcher input contract game differs from the selected product');
  assert.equal(contract?.variant,target.variant,'Launcher input contract variant differs from the selected Runtime');
  assert.ok(contract?.inputMasks&&typeof contract.inputMasks==='object','Launcher input contract needs the product action masks');
  const fireMask=contract.inputMasks.fire;
  assert.ok(Number.isSafeInteger(fireMask)&&fireMask>0&&(fireMask&(fireMask-1))===0,
    'Launcher input contract needs the product single-bit native fire mask');
  assert.ok(contract?.movement&&typeof contract.movement==='object','Launcher input contract needs the registered movement declaration');
  assert.ok(typeof contract.movement.source==='string'&&contract.movement.source.length>0,
    'Launcher input contract needs the movement owner source');
  if(kind==='gamepad'){
    assert.equal(typeof step,'function','Gamepad scene needs the existing native clock step');
    assert.equal(typeof readActions,'function','Gamepad scene needs the existing native action reader');
    assert.equal(typeof observe,'function','Gamepad scene needs the existing native position observer');
    assert.ok(contract.gamepad&&Number.isInteger(contract.gamepad.mappingSeat)&&Number.isInteger(contract.gamepad.nativeSeat),
      'Gamepad contract needs its declared Launcher mapping and native seats');
    assert.ok(typeof contract.gamepad.nativeSeatSource==='string'&&contract.gamepad.nativeSeatSource.length>0,
      'Gamepad contract needs its native seat ownership source');
    assert.ok(contract.gamepad.actions?.shoot&&contract.gamepad.actions?.right,
      'Gamepad contract needs the shoot and right source controls');
  }else if(kind==='layout'){
    assert.equal(typeof readActions,'function','Touch layout scene needs the existing native action reader');
    assert.ok(contract.layout&&typeof contract.layout==='object','Touch layout contract is absent');
  }else{
    assert.equal(typeof step,'function','Magnifier scene needs the existing native clock step');
    assert.equal(typeof readTrace,'function','Magnifier scene needs the existing native touch target trace');
    assert.ok(contract.magnifier&&typeof contract.magnifier==='object','Magnifier contract is absent');
  }
}

async function evaluateFixture(page,method,args){
  return page.evaluate(({fixture,method,args})=>{
    const api=window[fixture];
    if(!api||typeof api[method]!=='function')throw Error(`Launcher input fixture ${method} is unavailable`);
    return api[method](args?.operation,args?.value);
  },{fixture:FIXTURE,method,args});
}

function makeCdpTouchDriver(session,readPointerEvents){
  const active=new Map();
  const point=(id,value)=>({id,x:value.clientX,y:value.clientY,radiusX:1,radiusY:1,force:1,rotationAngle:0});
  const activePoints=(source=active)=>[...source].map(([id,value])=>point(id,value));
  const send=async(type,touchPoints)=>session.send('Input.dispatchTouchEvent',{type,touchPoints});
  const start=async(points)=>{
    assert.ok(Array.isArray(points)&&points.length>0,'Browser touch start needs at least one point');
    const next=new Map(active);
    for(const value of points){
      assert.ok(Number.isSafeInteger(value?.id)&&!next.has(value.id),'Browser touch identity is invalid or already active');
      assert.ok(Number.isFinite(value.clientX)&&Number.isFinite(value.clientY),'Browser touch coordinates are invalid');
      next.set(value.id,{clientX:value.clientX,clientY:value.clientY});
    }
    await send('touchStart',activePoints(next));
    active.clear();for(const entry of next)active.set(...entry);
  };
  const move=async(points)=>{
    assert.ok(Array.isArray(points)&&points.length>0,'Browser touch move needs at least one point');
    const next=new Map(active);
    for(const value of points){
      assert.ok(Number.isSafeInteger(value?.id)&&next.has(value.id),'Browser touch move has no matching active contact');
      assert.ok(Number.isFinite(value.clientX)&&Number.isFinite(value.clientY),'Browser touch coordinates are invalid');
      next.set(value.id,{clientX:value.clientX,clientY:value.clientY});
    }
    await send('touchMove',activePoints(next));
    active.clear();for(const entry of next)active.set(...entry);
  };
  const end=async(ids)=>{
    assert.ok(Array.isArray(ids)&&ids.length>0&&ids.every(id=>active.has(id)),'Browser touch end has no matching active contact');
    const remaining=new Map(active);for(const id of ids)remaining.delete(id);
    await send('touchEnd',activePoints(remaining));
    active.clear();for(const entry of remaining)active.set(...entry);
  };
  const cancel=async()=>{
    if(!active.size)return;
    await send('touchCancel',[]);active.clear();
  };
  const receipt=async({type,pointerId,clientX,clientY})=>{
    const eventType={pointerdown:'pointerdown',pointermove:'pointermove',pointerup:'pointerup',pointercancel:'pointercancel'}[type];
    assert.ok(eventType,`Unsupported browser Pointer event ${String(type)}`);
    const before=await readPointerEvents();
    if(type==='pointerdown')await start([{id:pointerId,clientX,clientY}]);
    else if(type==='pointermove')await move([{id:pointerId,clientX,clientY}]);
    else if(type==='pointerup')await end([pointerId]);
    else await cancel();
    const after=await readPointerEvents();
    const beforeEvents=Array.isArray(before?.events)?before.events:[];
    const afterEvents=Array.isArray(after?.events)?after.events:[];
    const candidates=afterEvents.slice(beforeEvents.length).filter(row=>row?.type===eventType&&row.pointerType==='touch'&&
      Number.isSafeInteger(row.pointerId)&&row.clientX===clientX&&row.clientY===clientY);
    assert.equal(after?.drops??0,0,'Browser Pointer journal overflowed while dispatching input');
    assert.ok(candidates.length>0,`CDP ${type} did not reach the actual Runtime Pointer listener at the requested coordinates`);
    const event=candidates.at(-1);
    assert.equal(event.isTrusted,true,'CDP touch did not produce a trusted browser Pointer');
    return{type,requestedTouchId:pointerId,pointerId:event.pointerId,clientX:event.clientX,clientY:event.clientY,event};
  };
  const pointerAt=async value=>receipt(value);
  const pinch=async({pointerIds,start,end:finish})=>{
    assert.ok(Array.isArray(pointerIds)&&pointerIds.length===2&&new Set(pointerIds).size===2,
      'Browser pinch needs two distinct touch identities');
    assert.ok(Array.isArray(start)&&start.length===2&&Array.isArray(finish)&&finish.length===2,
      'Browser pinch needs two start and two end coordinates');
    await startTouch(start,pointerIds);
    await moveTouch(finish,pointerIds);
    await end(pointerIds);
    return{pointerIds:[...pointerIds],start:structuredClone(start),end:structuredClone(finish)};
  };
  async function startTouch(points,ids){
    await start(points.map((value,index)=>({id:ids[index],clientX:value.clientX,clientY:value.clientY})));
  }
  async function moveTouch(points,ids){
    await move(points.map((value,index)=>({id:ids[index],clientX:value.clientX,clientY:value.clientY})));
  }
  return{pointerAt,pinch,cancel,activeCount:()=>active.size};
}

/** Run the registered Launcher input scene against an already-started Runtime.
 * Runtime launch, native clock ownership, and action observers stay with the caller. */
export async function runLauncherInputRuntime({kind,page,frame,context,target,step,readActions,observe,readTrace,contract}={}){
  requirePlaywrightInputs({kind,page,frame,context,target,step,readActions,observe,readTrace,contract});
  let fixtureInstalled=false,fixtureReceipt=null,cdp=null,touchDriver=null,viewportBefore=null,viewportChanged=false;
  let sceneResult=null,gamepadContractEvidence=null,failure=null;
  const cleanupErrors=[];
  const pageViewport=()=>typeof page.viewportSize==='function'?page.viewportSize():null;
  const readViewport=()=>page.evaluate(()=>({width:window.innerWidth,height:window.innerHeight}));
  try{
    if(kind==='layout'){
      viewportBefore=pageViewport();
      if(!viewportBefore)viewportBefore=await readViewport();
    }
    fixtureReceipt=await page.evaluate(({game,epoch})=>{
      const fixture=window.EaglerLauncherTouchFixture;
      if(!fixture||typeof fixture.install!=='function')throw Error('Launcher touch fixture bundle is unavailable');
      return fixture.install({game,epoch});
    },{game:target.game,epoch:target.epoch});
    fixtureInstalled=true;

    if(kind==='gamepad'){
      const deviceReceipt=await page.evaluate(({fixture})=>window[fixture].installGamepadMapping({firstIndex:0,secondIndex:1}),{fixture:FIXTURE});
      const mapping={
        refresh:()=>evaluateFixture(page,'gamepadMapping',{operation:'refresh'}),
        snapshot:()=>evaluateFixture(page,'gamepadMapping',{operation:'snapshot'}),
        setEnabled:enabled=>evaluateFixture(page,'gamepadMapping',{operation:'setEnabled',value:{enabled}}),
        select:(seat,key)=>evaluateFixture(page,'gamepadMapping',{operation:'select',value:{seat,key}}),
        bind:(key,action,controls)=>evaluateFixture(page,'gamepadMapping',{operation:'bind',value:{key,action,controls}}),
        setButton:(key,index,pressed)=>evaluateFixture(page,'gamepadMapping',{operation:'button',value:{key,index,pressed}}),
        setAxis:(key,index,value)=>evaluateFixture(page,'gamepadMapping',{operation:'axis',value:{key,index,value}}),
        reorder:assignments=>evaluateFixture(page,'gamepadMapping',{operation:'reorder',value:{assignments}}),
        cursor:()=>evaluateFixture(page,'gamepadMapping',{operation:'cursor'}),
        samplesSince:cursor=>evaluateFixture(page,'gamepadMapping',{operation:'samplesSince',value:{cursor}}),
        reload:()=>evaluateFixture(page,'gamepadMapping',{operation:'reload'}),
        neutralize:()=>evaluateFixture(page,'gamepadMapping',{operation:'neutralize'}),
      };
      const gamepadContract={...contract,...contract.gamepad,game:contract.game,variant:contract.variant,actions:{
        shoot:{...contract.gamepad.actions.shoot,
          first:{...contract.gamepad.actions.shoot.first,control:{...GAMEPAD_TEST_CONTROLS.firstShoot}},
          bits:GAMEPAD_BITS.shoot},
        right:{...contract.gamepad.actions.right,bits:GAMEPAD_BITS.right},
      }};
      gamepadContractEvidence={mappingSeat:gamepadContract.mappingSeat,nativeSeat:gamepadContract.nativeSeat,
        firstShootControl:{...GAMEPAD_TEST_CONTROLS.firstShoot},secondShootControl:{...gamepadContract.actions.shoot.second.control},
        note:'first-device Shoot uses a non-default button so the native effect cannot be supplied by the raw default-button fallback'};
      sceneResult=await runLauncherGamepadMappingScene({mapping,step,readActions,observe},{contract:gamepadContract});
      sceneResult.deviceFixture=deviceReceipt;
    }else if(kind==='layout'){
      const layout={
        async setViewport(value){
          await page.setViewportSize(value);viewportChanged=true;
          return readViewport();
        },
        begin:orientation=>evaluateFixture(page,'layoutScene',{operation:'begin',value:{orientation}}),
        updateControl:(name,placement)=>evaluateFixture(page,'layoutScene',{operation:'updateControl',value:{name,placement}}),
        save:()=>evaluateFixture(page,'layoutScene',{operation:'save'}),
        reload:()=>evaluateFixture(page,'layoutScene',{operation:'reload'}),
        apply:(orientation,control)=>evaluateFixture(page,'layoutScene',{operation:'apply',value:{orientation,control}}),
        storageSnapshot:()=>evaluateFixture(page,'layoutScene',{operation:'storageSnapshot'}),
      };
      sceneResult=await runLauncherTouchLayoutScene({layout,readActions},{contract:{...contract,...contract.layout}});
    }else{
      cdp=await context.newCDPSession(page);
      const readPointerEvents=()=>evaluateFixture(page,'magnifierScene',{operation:'pointerEventsSince',value:{cursor:0}});
      touchDriver=makeCdpTouchDriver(cdp,readPointerEvents);
      const magnifier={
        snapshot:()=>evaluateFixture(page,'magnifierScene',{operation:'snapshot'}),
        frameRect:()=>evaluateFixture(page,'magnifierScene',{operation:'frameRect'}),
        setAvailable:available=>evaluateFixture(page,'magnifierScene',{operation:'setAvailable',value:{available}}),
        pinch:coordinates=>touchDriver.pinch(coordinates),
        pointerCursor:()=>evaluateFixture(page,'magnifierScene',{operation:'pointerCursor'}),
        pointerEventsSince:cursor=>evaluateFixture(page,'magnifierScene',{operation:'pointerEventsSince',value:{cursor}}),
        pointerAt:value=>touchDriver.pointerAt(value),
        reset:()=>evaluateFixture(page,'magnifierScene',{operation:'reset'}),
        cancel:async()=>{await touchDriver.cancel();return evaluateFixture(page,'magnifierScene',{operation:'cancel'});},
      };
      sceneResult=await runLauncherTouchMagnifierScene({magnifier,step,readNativeTarget:readTrace},
        {contract:{...contract,...contract.magnifier}});
    }
  }catch(error){
    failure=error instanceof Error?error:new Error(String(error),{cause:error});
  }

  if(touchDriver)try{await touchDriver.cancel();}catch(error){cleanupErrors.push({operation:'cancel-cdp-touch',error:String(error)});}
  if(cdp)try{await cdp.detach();}catch(error){cleanupErrors.push({operation:'detach-cdp',error:String(error)});}
  if(fixtureInstalled)try{
    await page.evaluate(({fixture})=>{
      const api=window[fixture];if(typeof api?.dispose!=='function')throw Error('Launcher fixture dispose is unavailable');
      return api.dispose();
    },{fixture:FIXTURE});
  }catch(error){cleanupErrors.push({operation:'dispose-fixture',error:String(error)});}
  if(kind==='layout'&&viewportChanged&&viewportBefore)try{
    await page.setViewportSize({width:viewportBefore.width,height:viewportBefore.height});
  }catch(error){cleanupErrors.push({operation:'restore-viewport',error:String(error)});}

  if(failure){
    if(cleanupErrors.length)failure.cleanupErrors=[...(failure.cleanupErrors??[]),...cleanupErrors];
    throw failure;
  }
  if(cleanupErrors.length)throw new AggregateError(cleanupErrors.map(row=>new Error(`${row.operation}: ${row.error}`)),
    'Launcher input scene cleanup failed');
  assert.ok(sceneResult,'Launcher input scene returned no evidence');
  return{...sceneResult,kind,target:{game:target.game,variant:target.variant,epoch:target.epoch},
    contractEvidence:{inputMasks:contract.inputMasks,movement:contract.movement,...(gamepadContractEvidence?{gamepad:gamepadContractEvidence}:{})},fixture:fixtureReceipt,
    browserEvidence:kind==='layout'?{viewportRestored:viewportBefore}:kind==='magnifier'?{trustedCdpTouch:true}:null};
}
