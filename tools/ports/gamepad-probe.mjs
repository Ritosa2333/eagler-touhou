import assert from 'node:assert/strict';
import {createRequire} from 'node:module';
import {resolve} from 'node:path';
import {readRuntimeObservation,assertRuntimeObservation} from './runtime-observations.mjs';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';

const root=resolve(import.meta.dirname,'../..');
export async function gamepadProbeBundle(){
  const {build}=createRequire(resolve(root,'launcher/package.json'))('esbuild');
  const result=await build({stdin:{contents:`
    export {createGamepadMappingModel} from './launcher/src/launcher/gamepad-mapping.mts';
    export {createPlayerGamepad} from './launcher/app/services/player-gamepad.ts';
    export {virtualGamepad,virtualButton} from './launcher/tests/fixtures/virtual-gamepads.mjs';
  `,resolveDir:root},bundle:true,write:false,platform:'browser',format:'iife',globalName:'EaglerAcceptanceInputs'});
  return result.outputFiles[0].text;
}

const REQUIRED_NATIVE_MASK=0x107; // phase, consumed input, position, local seat
const GAMEPLAY_PHASE=game=>game==='th09'?'match':'stage';

export function gamepadTargetFor(game,variant,localSeat,{products=PRODUCT_GAMES}={}){
  const product=products[game];
  assert.ok(product&&product.inputProfile!=='fighting',`Unsupported gamepad target: ${game}`);
  assert.ok(variant==='normal'||variant==='multiplayer',`Unsupported gamepad variant: ${variant}`);
  const counts=product.multiplayer?.playerCounts;
  if(variant==='multiplayer')assert.ok(product.multiplayerRuntime&&Array.isArray(counts)&&counts.length&&
    counts.every(count=>Number.isInteger(count)&&count>=2),`${game}: multiplayer gamepad seats are not registered`);
  const maxSeat=variant==='normal'?0:Math.max(...counts)-1;
  assert.ok(Number.isInteger(localSeat)&&localSeat>=0&&localSeat<=maxSeat,`${game}: invalid local seat ${localSeat}`);
  assert.ok(variant!=='normal'||localSeat===0,'Normal Runtime gamepad input belongs to seat 0');
  // TH09 reads a distinct Launcher mapping for each versus side. The other
  // current shells call BrowserGamepadState() with its default mapping slot 0;
  // their native observer still identifies the local multiplayer seat.
  return {game,variant,localSeat,mappingSeat:product.multiplayer?.gameplay==='versus'?localSeat:0};
}

function validateTarget(target){
  assert.ok(target&&typeof target==='object','A gamepad target with game, variant, and localSeat is required');
  const expected=gamepadTargetFor(target.game,target.variant,target.localSeat);
  assert.equal(target.mappingSeat,expected.mappingSeat,`${target.game}: incorrect Launcher gamepad mapping seat`);
  return expected;
}

export function assertGamepadNativeObserver(observation,target,context='gameplay'){
  const expected=validateTarget(target);
  const th08LegacyObserver=expected.game==='th08';
  assert.ok(observation?.nativeObservation||th08LegacyObserver,
    `${expected.game}: incomplete gamepad evidence; Runtime has no native observer for the local input`);
  if(observation.nativeObservation){
    assert.equal(observation.nativeObservation.version,1,`${expected.game}: unsupported native gamepad observation version`);
    if(observation.capabilities?.phase===true)assert.equal(observation.phase?.name,GAMEPLAY_PHASE(expected.game),
      `${expected.game}: ${context} gamepad check lost its active gameplay precondition, got ${observation.phase?.name}`);
    if(!th08LegacyObserver)assert.equal(observation.nativeObservation.validMask&REQUIRED_NATIVE_MASK,REQUIRED_NATIVE_MASK,
      `${expected.game}: incomplete gamepad evidence; native phase, consumed input, position, and local seat are required`);
  }
  assert.equal(observation.capabilities?.phase,true,`${expected.game}: native gameplay phase is unavailable`);
  assert.equal(observation.capabilities?.input,true,`${expected.game}: native consumed input is unavailable`);
  assert.equal(observation.capabilities?.position,true,`${expected.game}: native player position is unavailable`);
  assert.equal(observation.localSeat,expected.localSeat,`${expected.game}: native input belongs to seat ${observation.localSeat}, expected ${expected.localSeat}`);
  assert.ok(typeof observation.phase?.name==='string',`${expected.game}: native phase value is unavailable`);
  assert.equal(observation.phase.name,GAMEPLAY_PHASE(expected.game),`${expected.game}: ${context} gamepad check requires active gameplay, got ${observation.phase.name}`);
  assert.ok(Number.isFinite(observation.input?.value),`${expected.game}: native consumed input value is unavailable`);
  assert.ok(Number.isFinite(observation.position?.x),`${expected.game}: native player x position is unavailable`);
  return observation;
}

export function nativeGamepadReadiness(observation,target,{mappingSamples=0}={}){
  const expected=validateTarget(target),reasons=[];
  if(!observation){return {ready:false,reasons:['native observation is unavailable'],mappingSamples};}
  if(observation.game!==expected.game)reasons.push(`reader returned ${observation.game??'no game identity'}`);
  if(observation.nativeObservation){
    if(observation.nativeObservation.version!==1)reasons.push(`unsupported native observation ABI ${observation.nativeObservation.version}`);
    else if(expected.game!=='th08'&&((observation.nativeObservation.validMask&REQUIRED_NATIVE_MASK)!==REQUIRED_NATIVE_MASK))
      reasons.push(`native ABI fields incomplete (validMask ${observation.nativeObservation.validMask})`);
  }else if(expected.game!=='th08')reasons.push('native observation ABI unavailable');
  if(observation.localSeat!==undefined&&observation.localSeat!==expected.localSeat)
    throw Error(`${expected.game}: native input belongs to seat ${observation.localSeat}, expected ${expected.localSeat}`);
  if(!Number.isInteger(observation.localSeat))reasons.push('native local seat is not ready');
  if(observation.capabilities?.phase!==true||typeof observation.phase?.name!=='string')reasons.push('native gameplay phase is not ready');
  else if(observation.phase.name!==GAMEPLAY_PHASE(expected.game))reasons.push(`native phase is ${observation.phase.name}, waiting for ${GAMEPLAY_PHASE(expected.game)}`);
  if(observation.capabilities?.input!==true||!Number.isFinite(observation.input?.value))reasons.push('native consumed input is not ready');
  if(observation.capabilities?.position!==true||!Number.isFinite(observation.position?.x))reasons.push('native player position is not ready');
  if(mappingSamples<3)reasons.push(`native getter has ${mappingSamples}/3 samples for mapping seat ${expected.mappingSeat}`);
  return {ready:reasons.length===0,reasons,mappingSamples,localSeat:observation.localSeat,
    phase:observation.phase?.name??null,validMask:observation.nativeObservation?.validMask??null};
}

export function assertGamepadActionEvidence({target,action,bits,heldSamples,releasedSamples,before,held,released}){
  const expected=validateTarget(target);
  assertGamepadNativeObserver(before,expected,`${action} before`);
  assertGamepadNativeObserver(held,expected,`${action} held`);
  assertGamepadNativeObserver(released,expected,`${action} released`);
  assert.ok(heldSamples?.some(sample=>sample.seat===expected.mappingSeat&&sample.bits===bits),
    `${expected.game}: ${action} did not reach the expected native gamepad getter for mapping seat ${expected.mappingSeat}`);
  assert.ok(releasedSamples?.some(sample=>sample.seat===expected.mappingSeat&&sample.bits===0),
    `${expected.game}: ${action} release did not reach the native gamepad getter for mapping seat ${expected.mappingSeat}`);
  assert.notEqual(held.input.value,before.input.value,`${expected.game}: ${action} sample did not change native consumed input`);
  assert.equal(released.input.value,before.input.value,`${expected.game}: ${action} release did not restore neutral native input`);
  if(action==='right')assertRuntimeObservation(held,'position-moved',
    {before:before.position,axis:'x',direction:'positive'});
  return {action,mappingSeat:expected.mappingSeat,localSeat:expected.localSeat,nativeInputBefore:before.input.value,
    nativeInputHeld:held.input.value,nativeInputReleased:released.input.value};
}

export function assertGamepadDisconnectEvidence({target,heldSamples,heldNative,heldBits,samples,native,neutralInput}){
  const expected=validateTarget(target);
  assertGamepadNativeObserver(heldNative,expected,'disconnect while held');
  assertGamepadNativeObserver(native,expected,'disconnected');
  assert.ok(heldSamples?.some(sample=>sample.seat===expected.mappingSeat&&sample.bits===heldBits),
    `${expected.game}: disconnect was not preceded by a held native gamepad input for mapping seat ${expected.mappingSeat}`);
  assert.notEqual(heldNative.input.value,neutralInput,`${expected.game}: disconnect fixture had no held native input`);
  assert.ok(samples?.some(sample=>sample.seat===expected.mappingSeat&&(sample.bits===0||sample.bits===-1)),
    `${expected.game}: disconnected device did not reach the native gamepad getter for mapping seat ${expected.mappingSeat}`);
  assert.equal(native.input.value,neutralInput,`${expected.game}: disconnect left native consumed input active`);
  return {mappingSeat:expected.mappingSeat,localSeat:expected.localSeat,heldBits,nativeInputWhileHeld:heldNative.input.value,
    nativeInputAfterDisconnect:native.input.value};
}

function attachWaitDiagnostic(error,context){
  const failure=error instanceof Error?error:new Error(String(error));
  failure.diagnostic={game:context.game,label:context.label,action:context.action??null,phase:context.phase??null,
    timeoutMs:context.timeoutMs,attempts:context.attempts,elapsedMs:Math.max(0,context.now()-context.startedAt),
    lastNativeObservation:context.last.native,readiness:context.last.readiness??null,
    recentSamples:context.last.samples.slice(-8)};
  return failure;
}

export async function waitForGamepadCondition({game,target,label,action,phase,predicate,readSamples,readNative,
  timeoutMs=8000,allowIncompleteNative=false,pause=async()=>{},now=Date.now}){
  const startedAt=now(),deadline=startedAt+timeoutMs;
  let attempts=0,last={samples:[],native:null,readiness:null};
  while(now()<deadline){
    attempts++;
    try{
      last={samples:await readSamples(),native:await readNative(),readiness:null};
      if(allowIncompleteNative){
        last.readiness=nativeGamepadReadiness(last.native,target,
          {mappingSamples:last.samples.filter(sample=>sample.seat===target.mappingSeat).length});
      }else assertGamepadNativeObserver(last.native,target,label);
      const value=predicate(last);
      if(value)return {...last,value};
    }catch(error){
      throw attachWaitDiagnostic(error,{game,label,action,phase,timeoutMs,attempts,last,now,startedAt});
    }
    await pause(50);
  }
  const error=new Error(`${game}: timed out waiting for ${label}; last native=${JSON.stringify(last.native)}; readiness=${JSON.stringify(last.readiness)}; last samples=${JSON.stringify(last.samples.slice(-8))}`);
  throw attachWaitDiagnostic(error,{game,label,action,phase,timeoutMs,attempts,last,now,startedAt});
}

export function gamepadProbeFailureEvidence(error,target){
  return {passed:false,target,error:String(error?.stack||error),diagnostic:error?.diagnostic??null};
}

export async function prepareVirtualGamepad(context,bundle,{mappingSeat=0}={}){
  assert.ok(mappingSeat===0||mappingSeat===1,`Unsupported virtual gamepad mapping seat: ${mappingSeat}`);
  function initialize(mappingSeat){
    if(window===top){
      window.acceptancePad=EaglerAcceptanceInputs.virtualGamepad('xbox');
      window.acceptancePads=[acceptancePad];
      const pad=acceptancePad,key=JSON.stringify([pad.id,pad.mapping,pad.buttons.length,pad.axes.length])+'#0',seats=['',''];
      seats[mappingSeat]=key;
      try{localStorage.setItem('eagler-touhou-gamepad-mapping-v1',JSON.stringify({version:1,enabled:true,seats,profiles:{}}));}catch{}
    }
    Object.defineProperty(navigator,'getGamepads',{configurable:true,value:()=>top.acceptancePads});
  }
  await context.addInitScript({content:bundle+'\nglobalThis.EaglerAcceptanceInputs=EaglerAcceptanceInputs;\n('+initialize.toString()+')('+mappingSeat+');'});
}

// A protocol-host scenario uses the same production mapper and policy port as
// Launcher. Real room scenarios already have this port and never replace it.
export async function bindProtocolHostGamepad(page,game,epoch){
  await page.evaluate(({game,epoch})=>{
    const model=EaglerAcceptanceInputs.createGamepadMappingModel({storage:localStorage,getGamepads:()=>navigator.getGamepads()});
    model.refresh();model.setEnabled(true);model.select(0,model.getSnapshot().devices[0].key);
    const context={game,epoch,protocol:'eagler-touhou/1',target:document.querySelector('iframe').contentWindow,targetOrigin:location.origin,launched:true,spectator:false};
    window.acceptanceGamepadService=EaglerAcceptanceInputs.createPlayerGamepad({model,window,document,playerOpen:()=>true,
      runtime:{getInputContext:()=>context,subscribe:()=>()=>{}}});
  },{game,epoch});
}

export async function probeNativeGamepad(page,frame,target){
  target=validateTarget(target);
  await page.bringToFront();await frame.locator('canvas').first().focus();
  await frame.evaluate(()=>{
    const owner=window.__eaglerGamepadMapping;
    if(!owner?.enabled())throw Error('Production gamepad policy port is not enabled');
    window.acceptancePadSamples=[];const sample=owner.sample;window.acceptancePadOriginalSample=sample;
    owner.sample=function(seat){const bits=sample.call(this,seat);
      if(acceptancePadSamples.length<10000)acceptancePadSamples.push({seat,bits,at:performance.now()});
      return bits;};
  });
  const samples=()=>frame.evaluate(()=>acceptancePadSamples);
  const reset=()=>frame.evaluate(()=>{acceptancePadSamples.length=0;});
  const native=()=>frame.evaluate(readRuntimeObservation,target);
  const waitFor=(label,predicate,options={})=>waitForGamepadCondition({game:target.game,target,label,predicate,
    readSamples:samples,readNative:native,pause:milliseconds=>page.waitForTimeout(milliseconds),...options});
  const result={mappingSeat:target.mappingSeat,localSeat:target.localSeat,neutral:[],actions:[]};
  try{
    const initial=await waitFor('active gameplay and native gamepad sampling',({readiness})=>readiness.ready,
      {timeoutMs:10000,allowIncompleteNative:true,action:'gameplay-readiness',phase:'precondition'});
    result.neutral=initial.samples;
    for(const [action,button,bits] of [['shoot',0,1],['focus',4,4],['right',15,128]]){
      const before=assertGamepadNativeObserver(await native(),target,`${action} before`);
      await reset();await page.evaluate(button=>EaglerAcceptanceInputs.virtualButton(acceptancePad,button,1),button);
      const held=await waitFor(`${action} native input effect`,({samples,native})=>
        samples.some(sample=>sample.seat===target.mappingSeat&&sample.bits===bits)&&
        native.input.value!==before.input.value&&
        (action!=='right'||native.position.x>before.position.x),{action,phase:'held'});
      await reset();await page.evaluate(button=>EaglerAcceptanceInputs.virtualButton(acceptancePad,button,0),button);
      const released=await waitFor(`${action} native input release`,({samples,native})=>
        samples.some(sample=>sample.seat===target.mappingSeat&&sample.bits===0)&&native.input.value===before.input.value,
        {action,phase:'released'});
      const evidence=assertGamepadActionEvidence({target,action,bits,heldSamples:held.samples,releasedSamples:released.samples,
        before,held:held.native,released:released.native});
      result.actions.push({...evidence,bits,held:held.samples,released:released.samples,native:{before,held:held.native,released:released.native}});
    }
    const neutralInput=result.actions.at(-1).native.released.input.value;
    await reset();await page.evaluate(()=>EaglerAcceptanceInputs.virtualButton(acceptancePad,4,1));
    const disconnectHeld=await waitFor('focus input held before device disconnect',({samples,native})=>
      samples.some(sample=>sample.seat===target.mappingSeat&&sample.bits===4)&&native.input.value!==neutralInput,
      {action:'disconnect',phase:'held'});
    await reset();await page.evaluate(()=>{acceptancePad.connected=false;acceptancePads=[];});
    const disconnected=await waitFor('native input neutral after device disconnect',({samples,native})=>
      samples.some(sample=>sample.seat===target.mappingSeat&&(sample.bits===0||sample.bits===-1))&&native.input.value===neutralInput,
      {action:'disconnect',phase:'disconnected'});
    result.disconnected=disconnected.samples;result.disconnectedNative=disconnected.native;
    result.disconnect=assertGamepadDisconnectEvidence({target,heldSamples:disconnectHeld.samples,heldNative:disconnectHeld.native,heldBits:4,
      samples:result.disconnected,native:result.disconnectedNative,neutralInput});
    result.coverage={status:'partial',checks:['INPUT-04','INPUT-06'],complete:false,
      scope:'shoot/focus/right action, positive-x movement, neutral release, and disconnect while focus is held'};
    result.effectVerified=true;
    return result;
  }finally{
    try{await page.evaluate(()=>{acceptancePad.connected=true;acceptancePads=[acceptancePad];for(let n=0;n<acceptancePad.buttons.length;n++)EaglerAcceptanceInputs.virtualButton(acceptancePad,n,0);});}
    finally{await frame.evaluate(()=>{const owner=window.__eaglerGamepadMapping;if(owner&&window.acceptancePadOriginalSample)owner.sample=acceptancePadOriginalSample;delete window.acceptancePadOriginalSample;}).catch(()=>{});}
  }
}
