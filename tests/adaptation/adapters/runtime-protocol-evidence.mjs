import assert from 'node:assert/strict';
import {assertRuntimeProtocolSession} from '../../../tools/ports/runtime-protocol-probe.mjs';

export function assessRuntimeProtocolEvidence({driver,entry,product}={}){
  if(!['configure-launch-first-frame','runtime-protocol-fields'].includes(entry?.id))return{available:false,valid:false,kind:null};
  const result={available:true,valid:false,kind:'packaged-runtime-protocol',checks:entry.id==='runtime-protocol-fields'?['RUNTIME-01','RUNTIME-03']:['RUNTIME-02']};
  try{
    assert.equal(driver.schema,'eagler-touhou/runtime-browser/1');
    const targets=driver.targets.filter(target=>target.game===product.game&&target.variant===product.variant);assert.equal(targets.length,1);
    const target=targets[0];assert.ok(target.checks.includes(entry)&&target.checks.filter(row=>row.id===entry.id).length===1);
    assert.equal(entry.status,'passed');assert.equal(target.finalErrorAudit?.status,'passed');
    assert.deepEqual(target.inputPostflight.packageIdentity,target.packageIdentity);assert.deepEqual(target.inputPostflight.dataIdentity,target.dataIdentity);
    if(entry.id==='runtime-protocol-fields'){
      const sessions=entry.observation.sessions;assert.equal(sessions.length,2);assert.notEqual(sessions[0].epoch,sessions[1].epoch);
      for(const [index,session] of sessions.entries())assertRuntimeProtocolSession(session,{game:product.game,epoch:index+1});
    }else{
      const snapshot=entry.observation,startup=snapshot.startup;assert.ok(snapshot.canvas.width>0&&snapshot.canvas.height>0);
      const values=[startup.ready,startup.configured,startup.launched,startup.firstFrame];
      for(const value of values){assert.equal(value.protocol,'eagler-touhou/1');assert.equal(value.game,product.game);assert.equal(value.epoch,1);}
      assert.equal(startup.ready.event,'ready');assert.equal(startup.configured.ok,true);assert.equal(startup.launched.ok,true);assert.equal(startup.firstFrame.event,'first-frame');
      assert.equal(startup.firstFrame.presentation.schema,'eagler-runtime/first-frame/1');
      assert.ok(Number.isSafeInteger(startup.firstFrame.presentation.nativePresentCount)&&startup.firstFrame.presentation.nativePresentCount>0);
      const positions=values.map(value=>startup.events.findIndex(event=>JSON.stringify(event)===JSON.stringify(value)));
      assert.ok(positions.every((position,index)=>position>=0&&(!index||position>positions[index-1])),'Startup protocol events were reordered');
    }
    result.valid=true;
  }catch(error){result.reason=error.message;}
  return result;
}
