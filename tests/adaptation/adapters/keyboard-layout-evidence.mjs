import assert from 'node:assert/strict';
import {resolveAdaptationAdapter} from './registry.mjs';
import {assertTouchActionEvidence} from '../../../tools/ports/touch-action-scenes.mjs';

const canonical=['ArrowUp','ArrowDown','ArrowLeft','ArrowRight'];
const bindings={up:'KeyI',down:'KeyK',left:'KeyU',right:'KeyO',shoot:'KeyF',bomb:'KeyG',focus:'KeyT',pause:'KeyP'};
const aliases=Object.fromEntries(Object.entries(bindings).map(([action,code])=>[code,
  {up:'ArrowUp',down:'ArrowDown',left:'ArrowLeft',right:'ArrowRight',shoot:'KeyZ',bomb:'KeyX',focus:'ShiftLeft',pause:'Escape'}[action]]));
const plans={
  wasd:{keys:['KeyW','KeyS','KeyA','KeyD'],codes:{ShiftLeft:'KeyL',KeyZ:'KeyJ',KeyX:'KeyK'}},
  custom:{keys:['KeyI','KeyK','KeyU','KeyO'],codes:{ShiftLeft:'KeyT',KeyZ:'KeyF',KeyX:'KeyG',Escape:'KeyP'}},
  arrows:{keys:canonical,codes:{}},
};

export function assertKeyboardActionJournal(e,{epoch,pulseTicks,codes={}}){
  assert.equal(e.source,'physical-keyboard');assert.equal(e.disposed,true);assert.equal(e.pulseTicks,pulseTicks);
  const expected={ShiftLeft:1,KeyZ:1,KeyX:1,Escape:2};
  assert.deepEqual([...new Set(e.events.map(row=>row.code))].sort(),Object.keys(expected).sort());
  for(const [code,count] of Object.entries(expected)){
    const downs=e.events.filter(row=>row.code===code&&row.down),ups=e.events.filter(row=>row.code===code&&!row.down);
    assert.equal(downs.length,count);assert.equal(ups.length,count);
    for(let i=0;i<count;i++){
      assert.ok(ups[i].afterTick>downs[i].afterTick);
      if(['KeyX','Escape'].includes(code))assert.equal(ups[i].afterTick-downs[i].afterTick,pulseTicks);
    }
  }
  for(const row of e.events){
    const physical=codes[row.code]??row.code;
    assert.equal(row.physical??row.code,physical);assert.equal(row.receipt.acknowledged,true);
    assert.equal(row.receipt.physicalReceipt.source,'physical');assert.equal(row.receipt.physicalReceipt.code,physical);
    assert.equal(row.receipt.physicalReceipt.down,row.down);assert.equal(row.receipt.physicalReceipt.epoch,epoch);
  }
}

export function assertKeyboardLayoutEvidence(e,{adapter,assertActions}){
  assert.deepEqual(e.bindings,bindings);assert.ok(e.saved.id.startsWith('saved-'));
  assert.deepEqual(e.saved.before,e.saved.after,'Saved custom layout must survive a fresh production preferences instance');
  assert.deepEqual(e.saved.after.aliases,aliases);assert.equal(e.saved.after.layout.selected,e.saved.id);
  assert.equal(e.saved.after.layout.persisted,true);
  assert.deepEqual(e.trials.map(row=>row.mode),['wasd','custom','arrows']);
  let previousEpoch=0;
  for(const trial of e.trials){
    const plan=plans[trial.mode],epoch=trial.actions.epoch;
    assert.ok(epoch>previousEpoch);previousEpoch=epoch;assert.equal(trial.transition.pagehide,true);
    assert.equal(trial.transition.epoch,epoch);assert.equal(trial.layout,trial.mode==='custom'?e.saved.id:trial.mode);
    assert.deepEqual(trial.codes,plan.codes);assert.equal(trial.directions.layout,trial.layout);
    assert.equal(trial.directions.selection.selected,trial.layout);
    if(trial.mode==='arrows')assert.equal(trial.directions.selection.base,'arrows');
    const expectedEdges=[];
    assert.deepEqual(trial.directions.directions.map(row=>row.physical),plan.keys);
    for(const [index,row] of trial.directions.directions.entries()){
      const axis=index<2?'y':'x',sign=index%2?1:-1;
      assert.equal(row.canonical,canonical[index]);assert.equal(row.axis,axis);assert.equal(row.sign,sign);
      assert.equal(row.before.input.value,0);assert.equal(row.held.input.value,adapter.movement.directionMasks[canonical[index]]);
      assert.ok((row.held.position[axis]-row.before.position[axis])*sign>0);
      assert.equal(row.released.input.value,0);assert.equal(row.settled.input.value,0);
      assert.deepEqual(row.settled.position,row.released.position);
      for(const down of [true,false])expectedEdges.push({code:plan.keys[index],down,canonical:canonical[index]});
    }
    assert.equal(trial.actions.complete,true);assert.equal(trial.actions.inputSource,'physical-keyboard');
    assertKeyboardActionJournal(trial.actions.keyboard,{epoch,pulseTicks:adapter.actions.escapePulseTicks,codes:plan.codes});
    assertActions(trial.actions,epoch);
    expectedEdges.push(...trial.actions.keyboard.events.map(row=>({code:row.physical,canonical:row.code,down:row.down})));
    const events=trial.events.filter(row=>row.source==='physical');
    assert.equal(events.length,expectedEdges.length,'Each physical edge must reach the production mapper exactly once');
    for(const [index,row] of events.entries()){
      const expected=expectedEdges[index];assert.equal(row.epoch,epoch);assert.equal(row.code,expected.code);assert.equal(row.down,expected.down);
      assert.equal(row.transitions.length,1,'One physical edge must generate one canonical edge');
      assert.equal(row.transitions[0].code,expected.canonical);assert.equal(row.transitions[0].down,expected.down);
    }
  }
}

export function assessKeyboardLayoutEvidence({driver,entry,product}={}){
  if(entry?.id!=='shared-keyboard-layouts')return{available:false,valid:false,kind:null};
  const result={available:true,valid:false,kind:'native-keyboard-layouts',checks:['INPUT-02']};
  try{
    assert.equal(driver.schema,'eagler-touhou/runtime-desktop-probes/1');
    const targets=driver.targets.filter(row=>row.game===product.game&&row.variant===product.variant);assert.equal(targets.length,1);
    const target=targets[0];assert.ok(target.checks.includes(entry)&&target.checks.filter(row=>row.id===entry.id).length===1);
    assert.equal(entry.status,'passed');const adapter=resolveAdaptationAdapter(product.game,product.variant);
    assertKeyboardLayoutEvidence(entry.observation,{adapter,assertActions:(actions,epoch)=>{
      assert.equal(actions.clock.mode,'native-manual');assert.ok(target.gameplayReadinessByEpoch.some(row=>row.epoch===epoch&&row.ready));
      assertTouchActionEvidence(actions.evidence,{inputMasks:adapter.actions.inputMasks,escapePulseTicks:adapter.actions.escapePulseTicks,bombResource:adapter.actions.bombResource});
    }});
    result.valid=true;
  }catch(error){result.reason=error.message;}
  return result;
}
