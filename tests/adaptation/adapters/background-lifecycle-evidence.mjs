import assert from 'node:assert/strict';
import {resolveAdaptationAdapter} from './registry.mjs';
import {resolveTitleFlow} from './title-flow.mjs';
import {inspectMinimizedKeyboardClear,inspectResumedKeyboardMovement,inspectNativeBackgroundEvents,inspectPostCleanupForeground} from '../../../tools/ports/runtime-desktop-lifecycle.mjs';

function barrier(receipt,{game,epoch}){
  assert.equal(receipt.ok,true);assert.equal(receipt.protocol,'eagler-touhou/1');assert.equal(receipt.game,game);assert.equal(receipt.epoch,epoch);assert.ok(receipt.request);
}
export function assertPointerCancellation(e,{product,isGameplayReady}){
  assert.equal(e.clock.mode,'native-manual');assert.equal(e.fixture.game,product.game);assert.equal(e.fixture.epoch,e.epoch);
  assert.deepEqual(e.commands.map(row=>row.fields.type),['pointerdown','pointermove','pointercancel']);
  assert.deepEqual(e.commands.map(row=>[row.fields.x,row.fields.y]),[[.5,.75],[.7,.7],[.7,.7]]);
  assert.equal(new Set(e.commands.map(row=>row.barrier.request)).size,3);
  for(const command of e.commands){
    assert.equal(command.source,'production-dom-pointer');assert.equal(command.fields.control,'directSurface');assert.equal(command.fields.id,9812);
    barrier(command.barrier,{game:product.game,epoch:e.epoch});
  }
  assert.equal(e.moving.length,4);assert.equal(e.cancelled.length,4);assert.equal(e.before.input.value,0);
  const samples=[e.before,...e.moving,...e.cancelled],clock=e.before.capabilities.restartFrame;assert.ok(clock);
  for(const [i,state] of samples.entries()){
    assert.equal(isGameplayReady(state),true);assert.equal(state.localSeat,0);
    if(i)assert.equal(state.frames[clock],samples[i-1].frames[clock]+1);
  }
  assert.ok(e.moving.some(state=>state.position.x!==e.before.position.x||state.position.y!==e.before.position.y));
  for(const state of e.cancelled){assert.equal(state.input.value,0);assert.deepEqual(state.position,e.moving.at(-1).position);}
}
export function assertHeldTouchLifecycle(touch,{game,epoch,focusMask}){
  assert.equal(touch.fixture.game,game);assert.equal(touch.fixture.epoch,epoch);assert.equal(touch.fixture.focus,'hold-button');
  assert.equal(touch.down.fields.control,'focus');assert.equal(touch.down.fields.type,'pointerdown');
  barrier(touch.down.barrier,{game,epoch});
  assert.equal(touch.before.focusEnabled,true);assert.ok(touch.held.input.value&focusMask);
  assert.equal(touch.hidden.focusEnabled,false);assert.equal(touch.restored.focusEnabled,false);
  for(const state of [touch.hidden,touch.restored]){assert.equal(state.heldFire,false);assert.equal(state.functionDown,false);assert.equal(state.joystickX,0);assert.equal(state.joystickY,0);}
}
export function assertGamepadReconnect(e,{movement}){
  assert.ok(Number.isSafeInteger(e.epoch)&&e.epoch>0);
  for(const receipt of [e.neutral,e.left,e.disconnect,e.reconnectHeld,e.rearm,e.right,e.release])assert.equal(receipt.epoch,e.epoch);
  assert.equal(e.neutral.bits,0);assert.equal(e.before.input.value,0);
  assert.equal(e.left.bits,64);assert.equal(e.held.input.value,movement.directionMasks.ArrowLeft);assert.ok(e.held.position.x<e.before.position.x);
  assert.equal(e.disconnect.device.connected,false);assert.equal(e.disconnect.bits,0);assert.equal(e.released.input.value,0);
  assert.equal(e.reconnectHeld.device.connected,true);assert.equal(e.reconnectHeld.device.buttons[14],1);assert.equal(e.reconnectHeld.bits,0);
  assert.equal(e.suppressed.input.value,0);assert.deepEqual(e.suppressed.position,e.released.position);
  assert.equal(e.rearm.bits,0);assert.equal(e.right.bits,128);assert.equal(e.resumed.input.value,movement.directionMasks.ArrowRight);
  assert.ok(e.resumed.position.x>e.suppressed.position.x);assert.equal(e.release.bits,0);assert.equal(e.final.input.value,0);
}

export function assessBackgroundLifecycleEvidence({driver,entry,product}={}){
  if(entry?.id!=='background-input-lifecycle-contract')return{available:false,valid:false,kind:null};
  const result={available:true,valid:false,kind:'native-background-input-lifecycle',checks:entry.completeChecks??[]};
  try{
    assert.equal(driver.schema,'eagler-touhou/runtime-desktop-probes/1');assert.equal(driver.browser.headless,false);
    assert.ok(result.checks.length&&result.checks.every(id=>['INPUT-06','TOUCH-16'].includes(id)));
    const targets=driver.targets.filter(row=>row.game===product.game&&row.variant===product.variant);assert.equal(targets.length,1);
    const target=targets[0];assert.ok(target.checks.includes(entry));assert.equal(entry.status,'passed');assert.equal(target.status,'passed');
    assert.equal(target.checks.filter(row=>row.id===entry.id).length,1);
    assert.deepEqual(target.packageIdentityPostflight,target.packageIdentity);assert.deepEqual(target.dataIdentityPostflight,target.dataIdentity);
    const get=id=>{const rows=target.checks.filter(row=>row.id===id);assert.equal(rows.length,1);assert.equal(rows[0].status,'passed');return rows[0].observation;};
    const m=get('window-minimize-foreground-and-release'),t=get('background-tab-foreground-release'),adapter=resolveAdaptationAdapter(product.game,product.variant);
    const d=m.minimized,o=d.minimizedInputObservation;
    assert.equal(o.status,'neutral');assert.ok(o.validSamples>0);
    assert.equal(inspectMinimizedKeyboardClear({windowState:d.windowState,nativeState:d.nativeState,events:d.lifecycleEvents,hostEvents:d.hostKeyboardLifecycle,
      hiddenStartAtMs:d.hiddenStartAtMs,hiddenSampledAtMs:d.hiddenSampledAtMs,inputValue:o.inputValue,neutralInputValue:m.before.input.value,inputObservation:o}).status,'passed');
    assert.equal(inspectResumedKeyboardMovement({hidden:d.hiddenInput,restored:m.focused,neutralInputValue:m.before.input.value}).status,'passed');
    assert.equal(m.afterKeyup.input.value,0);assert.equal(t.afterReturn.input.value,0);assert.equal(t.afterKeyup.input.value,0);
    assert.equal(inspectNativeBackgroundEvents({state:t.hidden.state,events:t.lifecycleEvents}).observed,true);
    assert.ok(t.hostKeyboardLifecycle.some(row=>row.accepted===true&&['host-hidden','host-blur'].includes(row.reason)));
    const post=t.postCleanupForeground;
    assert.equal(inspectPostCleanupForeground({hostState:post.hostFocus.state,runtimeState:post.runtimeFocus.state,beforePage:t.gamePageTarget,
      afterPage:post.pageTargetAfterCleanup,backgroundPageCloseStatus:post.temporaryPageClosed?'closed':'failed'}).ready,true);
    if(result.checks.includes('TOUCH-16')){
      const e=get('touch-pointer-cancel-and-release'),flow=resolveTitleFlow(product);
      assertPointerCancellation(e,{product,isGameplayReady:state=>flow.isGameplayReady(state,'desktop')});
      for(const row of [m,t])assertHeldTouchLifecycle(row.touch,{game:product.game,epoch:e.epoch,focusMask:adapter.actions.inputMasks.focus});
    }
    if(result.checks.includes('INPUT-06'))assertGamepadReconnect(get('gamepad-disconnect-and-reconnect'),{movement:adapter.movement});
    result.valid=true;
  }catch(error){result.reason=error.message;}
  return result;
}
