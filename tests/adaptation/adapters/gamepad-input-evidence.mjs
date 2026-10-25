import assert from 'node:assert/strict';
import {resolveAdaptationAdapter} from './registry.mjs';
import {resolveTitleFlow} from './title-flow.mjs';
import {assertTouchActionEvidence} from '../../../tools/ports/touch-action-scenes.mjs';
import {assertGamepadDirections} from '../../../tools/ports/gamepad-direction-scenes.mjs';
import {GAMEPAD_ACTION_BUTTONS} from '../../../tools/ports/gamepad-action-input.mjs';
import {assessRuntimeInputEvidence} from './input-evidence.mjs';

export function assertGamepadActionJournal(e,{epoch,pulseTicks}){
  assert.equal(e.source,'production-gamepad');assert.equal(e.disposed,true);assert.equal(e.pulseTicks,pulseTicks);
  assert.deepEqual([...new Set(e.events.map(row=>row.action))].sort(),['bomb','escape','fire','focus']);
  const held=new Set(),bits={fire:1,bomb:2,focus:4,escape:8};
  for(const [action,count] of Object.entries({fire:1,focus:1,bomb:1,escape:2})){
    const downs=e.events.filter(row=>row.action===action&&row.down),ups=e.events.filter(row=>row.action===action&&!row.down);
    assert.equal(downs.length,count);assert.equal(ups.length,count);
    for(let i=0;i<count;i++){
      assert.ok(ups[i].afterTick>downs[i].afterTick);
      if(['bomb','escape'].includes(action))assert.equal(ups[i].afterTick-downs[i].afterTick,pulseTicks);
    }
  }
  for(const row of e.events){
    assert.equal(row.button,GAMEPAD_ACTION_BUTTONS[row.action]);
    assert.equal(row.receipt.acknowledged,true);assert.equal(row.receipt.source,'production-gamepad');assert.equal(row.receipt.epoch,epoch);
    assert.equal(row.receipt.button,row.button);assert.equal(row.receipt.down,row.down);
    if(row.down){assert.ok(!held.has(row.action));held.add(row.action);}else{assert.ok(held.has(row.action));held.delete(row.action);}
    assert.equal(row.receipt.device.connected,true);assert.deepEqual(row.receipt.device.axes,[0,0,0,0]);
    for(const [action,button] of Object.entries(GAMEPAD_ACTION_BUTTONS))assert.equal(row.receipt.device.buttons[button],Number(held.has(action)));
    assert.equal(row.receipt.bits,[...held].reduce((sum,action)=>sum|bits[action],0),'Production mapping must preserve all concurrently held actions');
  }
  assert.equal(held.size,0);
}

export function assessGamepadInputEvidence({driver,entry,product}={}){
  if(entry?.id!=='shared-gamepad-native-input')return{available:false,valid:false,kind:null};
  const result={available:true,valid:false,kind:'native-gamepad-input',checks:['INPUT-04']};
  try{
    assert.equal(driver.schema,'eagler-touhou/runtime-desktop-probes/1');
    const targets=driver.targets.filter(row=>row.game===product.game&&row.variant===product.variant);assert.equal(targets.length,1);
    const target=targets[0];assert.ok(target.checks.includes(entry)&&target.checks.filter(row=>row.id===entry.id).length===1);assert.equal(entry.status,'passed');
    const adapter=resolveAdaptationAdapter(product.game,product.variant),flow=resolveTitleFlow(product),e=entry.observation,a=e.actions;
    assert.equal(e.transition.pagehide,true);assert.equal(e.transition.epoch,a.epoch);assert.equal(a.inputSource,'production-gamepad');assert.equal(a.clock.mode,'native-manual');
    assert.ok(target.gameplayReadinessByEpoch.some(row=>row.epoch===a.epoch&&row.ready));
    assertGamepadDirections(a.preparation,{movement:adapter.movement,observation:adapter.observation,epoch:a.epoch,isGameplayReady:state=>flow.isGameplayReady(state,'desktop')});
    assertTouchActionEvidence(a.evidence,{inputMasks:adapter.actions.inputMasks,escapePulseTicks:adapter.actions.escapePulseTicks,bombResource:adapter.actions.bombResource});
    assertGamepadActionJournal(a.gamepad,{epoch:a.epoch,pulseTicks:adapter.actions.escapePulseTicks});
    const menu=target.checks.filter(row=>row.id==='shared-original-menu-dialogue-input');assert.equal(menu.length,1);
    const verdict=assessRuntimeInputEvidence({driver,entry:menu[0],product});assert.equal(verdict.valid,true,verdict.reason);
    result.valid=true;
  }catch(error){result.reason=error.message;}
  return result;
}
