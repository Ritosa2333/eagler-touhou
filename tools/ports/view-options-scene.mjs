import assert from 'node:assert/strict';
import {assertControlledSceneWorld,assertControlledSceneArmed} from '../../tests/adaptation/adapters/scene.mjs';

export function decodeViewOptions(raw){
  assert.equal(raw?.available,true);const w=raw.words;assert.ok(Array.isArray(w)&&w.length===32&&w.every(Number.isFinite));
  assert.equal(w[0],1);assert.equal(w[1],1);for(const i of [2,3,4,5,6])assert.ok(w[i]===0||w[i]===1);
  return{always:Boolean(w[2]),consumedAlways:Boolean(w[3]),focused:Boolean(w[4]),marker:{exists:Boolean(w[5]),visible:Boolean(w[6]),
    flags:w[7],script:w[8],sprite:w[9],alpha:w[10],x:w[11],y:w[12]},x:w[13],y:w[14],input:w[15],
    halfBoxes:[w.slice(16,18),w.slice(18,20),w.slice(20,22)],frame:w[22]};
}
const sampleGroups=e=>[[e.before,e.fast.before,e.fast.held,e.fast.released,e.fast.settled,e.focused,e.slow.before,e.slow.held,e.slow.released,e.slow.settled,e.unfocused],
  [e.enabled,e.enabledFast.before,e.enabledFast.held,e.enabledFast.released,e.enabledFast.settled],[e.final]];
const close=(a,b,label)=>assert.ok(Math.abs(a-b)<.0001,label+': '+a+' / '+b);
function marker(sample){const m=sample.view.marker;assert.equal(m.exists,true);assert.equal(m.visible,true);assert.ok(m.alpha>0&&m.sprite>=0);}
function movement(row){assert.ok(row.held.view.x>row.before.view.x);close(row.held.view.y,row.before.view.y,'horizontal movement');
  assert.equal(row.released.view.input,0);assert.equal(row.settled.view.input,0);close(row.released.view.x,row.settled.view.x,'released movement');
  close(row.released.view.y,row.settled.view.y,'released vertical movement');return row.held.view.x-row.before.view.x;}
export function assertViewOptionsScene(e,{markerOffset={x:0,y:0}}={}){
  assert.equal(e.schema,'eagler-touhou/view-options-scene/1');assert.deepEqual(e.cleanupErrors,[]);
  const initial=e.before;
  assert.ok(initial.view.halfBoxes.every(box=>box.every(value=>value>0)));
  const identities=[];
  for(const [index,group] of sampleGroups(e).entries()){
    const setup=e.optionChanges[index];assertControlledSceneArmed(setup.arm);assert.equal(setup.clock.mode,'native-manual');
    let frame=group[0].view.frame;const identity=group[0].identity;identities.push(identity);
    assert.ok(identity.app>0&&identity.timeOrigin>0);assert.equal(Number(new URL(identity.url).searchParams.get('runtimeEpoch')),setup.epoch);
    for(const sample of group){
      assert.deepEqual(decodeViewOptions(sample.raw),sample.view);assert.deepEqual(sample.identity,identity);
      assertControlledSceneWorld(sample.scene);assert.equal(sample.scene.requestGeneration,setup.arm.requestGeneration);
      assert.deepEqual(sample.view.halfBoxes,initial.view.halfBoxes,'Display option changed native collision dimensions');
      if(sample.view.marker.exists){close(sample.view.marker.x,sample.view.x+markerOffset.x,'Native marker horizontal alignment');
        close(sample.view.marker.y,sample.view.y+markerOffset.y,'Native marker vertical alignment');}
      assert.ok(sample.view.frame>=frame);frame=sample.view.frame;
    }
  }
  assert.equal(new Set(identities.map(row=>row.timeOrigin)).size,3,'Every option launch needs a fresh native document');
  assert.ok(e.optionChanges.every((row,i)=>!i||row.epoch>e.optionChanges[i-1].epoch));
  for(const row of [e.before,e.final]){assert.equal(row.view.always,false);assert.equal(row.view.consumedAlways,false);
    assert.equal(row.view.focused,false);assert.equal(row.view.marker.exists,false);assert.equal(row.view.input,0);}
  assert.equal(e.focused.view.focused,true);marker(e.focused);
  assert.equal(e.enabled.view.always,true);assert.equal(e.enabled.view.consumedAlways,true);assert.equal(e.enabled.view.focused,false);marker(e.enabled);
  assert.equal(e.enabled.view.marker.script,e.focused.view.marker.script);assert.equal(e.enabled.view.marker.sprite,e.focused.view.marker.sprite);
  const fast=movement(e.fast),enabled=movement(e.enabledFast);close(enabled,fast,'Always-hitbox must preserve maximum movement');
  assert.ok(e.slow.held.view.x-e.slow.before.view.x>0&&e.slow.held.view.x-e.slow.before.view.x<fast,'Actual low-speed baseline must remain distinct');
  for(const row of [e.enabledFast.before,e.enabledFast.held,e.enabledFast.released,e.enabledFast.settled]){
    assert.equal(row.view.focused,false);assert.equal(row.view.always,true);marker(row);
  }
  assert.deepEqual(e.optionChanges.map(row=>row.value),[false,true,false]);
  for(const row of e.optionChanges){assert.equal(row.source,'Launcher #alwaysHitboxToggle');assert.equal(row.checkedBeforeLaunch,row.value);}
  assert.deepEqual(e.screenshots.map(row=>row.name),['disabled','focused','enabled']);
  for(const row of e.screenshots){assert.equal(row.beforeFrame,row.afterFrame);assert.ok(row.bytes>100&&/^[a-f0-9]{64}$/.test(row.sha256));}
  assert.ok(e.fast.settled.view.frame>initial.view.frame&&e.enabledFast.settled.view.frame>e.enabled.view.frame);
}

export function assertViewOptionsRestart(e){
  const r=e.restart,key=r.before.capabilities.restartFrame;assert.ok(key);assert.equal(r.before.pause.active,false);
  assert.equal(r.paused.pause.active,true);assert.equal(r.frozen.pause.active,true);assert.equal(r.paused.frames[key],r.frozen.frames[key]);
  assert.equal(r.reset.pause.active,false);assert.ok(r.reset.frames[key]<r.before.frames[key]);assert.ok(r.advanced.frames[key]>r.reset.frames[key]);
  assert.deepEqual(r.identity,e.final.identity);
}

export async function runViewOptionsScene(io){
  const e={schema:'eagler-touhou/view-options-scene/1',optionChanges:[],screenshots:[],cleanupErrors:[]};let failure;
  const save=()=>io.save(e),read=()=>io.read(),step=count=>io.step(count);
  const options=async value=>{e.optionChanges.push({value,...await io.open(value)});await step(400);};
  const move=async()=>{const before=await read();await io.key('ArrowRight',true);await step(6);const held=await read();
    await io.key('ArrowRight',false);await step(3);const released=await read();await step(2);return{before,held,released,settled:await read()};};
  try{
    await options(false);e.before=await read();e.screenshots.push(await io.capture('disabled'));e.fast=await move();await save();
    await io.key('ShiftLeft',true);await step(12);e.focused=await read();e.screenshots.push(await io.capture('focused'));
    // Shift is kept while collecting the low-speed baseline; its input is expected.
    e.slow=await move();await io.key('ShiftLeft',false);await step(12);e.unfocused=await read();await save();
    await options(true);e.enabled=await read();e.screenshots.push(await io.capture('enabled'));e.enabledFast=await move();await save();
    await options(false);await step(45);e.final=await read();
  }catch(error){failure=error;error.evidence=e;throw error;}
  finally{try{await io.release();}catch(error){e.cleanupErrors.push(String(error));}await save();
    if(e.cleanupErrors.length&&!failure)throw Object.assign(Error('View-option input cleanup failed'),{evidence:e});}
  assertViewOptionsScene(e,io.policy);await save();return e;
}
