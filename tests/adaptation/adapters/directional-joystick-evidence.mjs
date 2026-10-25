import assert from 'node:assert/strict';
import {resolveAdaptationAdapter} from './registry.mjs';
import {resolveTitleFlow} from './title-flow.mjs';

const vectors=[[-1,0],[1,0],[0,-1],[0,1],[-1,-1],[1,1],[-1,1],[1,-1]];
export function assertDirectionalJoystickEvidence(e,{product,movement,observation,isGameplayReady}){
  assert.equal(e.mode,'joystick');assert.equal(e.pagehide,true);assert.equal(e.start.input.value,0);
  const masks=movement.directionMasks;assert.ok(masks&&movement.directionSource);
  assert.deepEqual(e.actions.map(row=>[row.x,row.y]),vectors,'All four cardinal and diagonal directions are required');
  const inspect=state=>{
    assert.equal(state.game,product.game);assert.equal(state.variant,product.variant);assert.equal(isGameplayReady(state),true);
    assert.equal(state.localSeat,0);assert.ok(observation?.inputSemantics);assert.equal(state.input.inputSemantics,observation.inputSemantics);
    assert.ok(Number.isFinite(state.position?.x)&&Number.isFinite(state.position?.y));
    if(movement.keyboardReleasePolicy){
      assert.equal(state.input.source,movement.keyboardReleasePolicy.observationSource);
      assert.equal(state.input.sampleBoundary,movement.keyboardReleasePolicy.observationPhase);
    }
  };
  for(const row of e.actions){
    assert.equal(row.kind,'joystick-direction');assert.deepEqual(row.input,{joystickX:row.x*32767,joystickY:row.y*32767});
    for(const receipt of [row.press,row.release]){
      assert.equal(receipt.acknowledged,true);assert.equal(receipt.ok,true);assert.equal(receipt.protocol,'eagler-touhou/1');
      assert.equal(receipt.game,product.game);assert.equal(receipt.epoch,e.epoch);assert.ok(receipt.request);
    }
    assert.notEqual(row.press.request,row.release.request);
    const expected=(row.x<0?masks.ArrowLeft:row.x>0?masks.ArrowRight:0)|(row.y<0?masks.ArrowUp:row.y>0?masks.ArrowDown:0);
    const {before,held,released}=row;[before,held,released,...row.settled].forEach(inspect);
    assert.equal(before.input.value,0);assert.equal(held.input.value,expected);assert.equal(released.input.value,0);
    const clock=before.capabilities.restartFrame;assert.ok(typeof clock==='string'&&clock);
    assert.ok(Number.isSafeInteger(before.frames[clock]));
    const states=[before,held,released,...row.settled];
    for(let i=1;i<states.length;i++){
      assert.equal(states[i].capabilities.restartFrame,clock);assert.ok(Number.isSafeInteger(states[i].frames[clock])&&states[i].frames[clock]>states[i-1].frames[clock]);
    }
    for(const [axis,sign] of [['x',row.x],['y',row.y]]){
      const delta=held.position[axis]-before.position[axis];
      if(sign)assert.ok(delta*sign>0,`Joystick ${row.x},${row.y}: wrong ${axis} direction`);
      else assert.equal(delta,0,`Joystick moved the uncommanded ${axis} axis`);
    }
    assert.equal(row.settled.length,3,'Three later native frames must remain neutral');
    for(const state of row.settled){assert.equal(state.input.value,0);assert.deepEqual(state.position,released.position);}
  }
  inspect(e.neutral);assert.equal(e.neutral.input.value,0);
}

export function assessDirectionalJoystickEvidence({driver,entry,product}={}){
  if(entry?.id!=='runtime-epoch-touch-mode-joystick')return{available:false,valid:false,kind:null};
  const result={available:true,valid:false,kind:'native-directional-joystick',checks:['TOUCH-03']};
  try{
    assert.equal(driver.schema,'eagler-touhou/runtime-desktop-probes/1');
    const targets=driver.targets.filter(row=>row.game===product.game&&row.variant===product.variant);assert.equal(targets.length,1);
    const target=targets[0];assert.ok(target.checks.includes(entry)&&target.checks.filter(row=>row.id===entry.id).length===1);assert.equal(entry.status,'passed');
    const e=entry.observation,configs=target.launchOptionsByEpoch.filter(row=>row.epoch===e.epoch);assert.equal(configs.length,1);
    assert.equal(configs[0].options.touchMovementMode,'joystick');assert.equal(configs[0].acknowledgement.ok,true);
    assert.ok(target.gameplayReadinessByEpoch.some(row=>row.epoch===e.epoch&&row.ready));
    const adapter=resolveAdaptationAdapter(product.game,product.variant),flow=resolveTitleFlow(product);
    assertDirectionalJoystickEvidence(e,{product,movement:adapter.movement,observation:adapter.observation,isGameplayReady:state=>flow.isGameplayReady(state,'desktop')});
    result.valid=true;
  }catch(error){result.reason=error.message;}
  return result;
}
