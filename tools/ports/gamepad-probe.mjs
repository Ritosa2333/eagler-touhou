import assert from 'node:assert/strict';
import {createRequire} from 'node:module';
import {resolve} from 'node:path';
import {readRuntimeObservation,assertRuntimeObservation} from './runtime-observations.mjs';

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

export async function prepareVirtualGamepad(context,bundle){
  function initialize(){
    if(window===top){
      window.acceptancePad=EaglerAcceptanceInputs.virtualGamepad('xbox');
      window.acceptancePads=[acceptancePad];
      const pad=acceptancePad,key=JSON.stringify([pad.id,pad.mapping,pad.buttons.length,pad.axes.length])+'#0';
      try{localStorage.setItem('eagler-touhou-gamepad-mapping-v1',JSON.stringify({version:1,enabled:true,seats:[key,''],profiles:{}}));}catch{}
    }
    Object.defineProperty(navigator,'getGamepads',{configurable:true,value:()=>top.acceptancePads});
  }
  await context.addInitScript({content:bundle+'\nglobalThis.EaglerAcceptanceInputs=EaglerAcceptanceInputs;\n('+initialize.toString()+')();'});
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
  await page.bringToFront();await frame.locator('canvas').first().focus();
  await frame.evaluate(()=>{
    const owner=window.__eaglerGamepadMapping;
    if(!owner?.enabled())throw Error('Production gamepad policy port is not enabled');
    window.acceptancePadSamples=[];const sample=owner.sample;
    owner.sample=function(seat){const bits=sample.call(this,seat);
      if(acceptancePadSamples.length<10000)acceptancePadSamples.push({seat,bits,at:performance.now()});
      return bits;};
  });
  const samples=()=>frame.evaluate(()=>acceptancePadSamples);
  const reset=()=>frame.evaluate(()=>{acceptancePadSamples.length=0;});
  const native=()=>target?.variant==='normal'?frame.evaluate(readRuntimeObservation,target):null;
  await page.waitForTimeout(500);
  const initial=await samples();
  assert.ok(initial.length>=3,'Production native loop did not sample the enabled Launcher gamepad port');
  const result={neutral:initial,actions:[]};
  try{
    for(const [action,button,bits] of [['shoot',0,1],['focus',4,4],['right',15,128]]){
      const before=await native();
      await reset();await page.evaluate(button=>EaglerAcceptanceInputs.virtualButton(acceptancePad,button,1),button);
      await page.waitForTimeout(350);const held=await samples();
      assert.ok(held.some(sample=>sample.bits===bits),`${action}: native sampling did not receive the normalized action`);
      const heldNative=await native();
      if(before?.capabilities.input)assert.notEqual(heldNative.input.value,before.input.value,`${action}: sampled action did not change native gameplay input`);
      if(action==='right'&&before?.capabilities.position)assertRuntimeObservation(heldNative,'position-moved',{before:before.position,axis:'x',direction:'positive'});
      await reset();await page.evaluate(button=>EaglerAcceptanceInputs.virtualButton(acceptancePad,button,0),button);
      await page.waitForTimeout(250);const released=await samples();
      assert.ok(released.length&&released.at(-1).bits===0,`${action}: release did not reach native sampling`);
      const releasedNative=await native();
      if(before?.capabilities.input)assertRuntimeObservation(releasedNative,'input-released',{value:before.input.value});
      result.actions.push({action,bits,held,released,native:before?{before,held:heldNative,released:releasedNative}:null});
    }
    await reset();await page.evaluate(()=>{acceptancePad.connected=false;acceptancePads=[];});
    await page.waitForTimeout(250);result.disconnected=await samples();
    assert.ok(result.disconnected.length&&result.disconnected.at(-1).bits===0,'Disconnected virtual device retained native input');
    result.disconnectedNative=await native();
    const baseline=result.actions[0]?.native?.before;
    if(baseline?.capabilities.input)assertRuntimeObservation(result.disconnectedNative,'input-released',{value:baseline.input.value});
    result.effectVerified=target?.variant==='normal'&&result.actions.every(({action,native:state})=>
      state?.before.capabilities.input&&state.held.capabilities.input&&state.released.capabilities.input&&
      (action!=='right'||(state.before.capabilities.position&&state.held.capabilities.position)));
    return result;
  }finally{await page.evaluate(()=>{acceptancePad.connected=true;acceptancePads=[acceptancePad];for(let n=0;n<acceptancePad.buttons.length;n++)EaglerAcceptanceInputs.virtualButton(acceptancePad,n,0);});}
}
