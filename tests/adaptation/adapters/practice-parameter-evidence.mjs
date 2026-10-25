import assert from 'node:assert/strict';
import {resolveAdaptationAdapter} from './registry.mjs';

export function assessPracticeParameterEvidence({driver,entry,product}={}){
  if(driver?.schema!=='eagler-touhou/practice-browser/1'||!driver.parametersOnly||entry?.id!=='PRACTICE-04')return{available:false,valid:false,kind:null};
  const result={available:true,valid:false,kind:'practice-parameter-consumers',checks:['PRACTICE-04']};
  try{
    assert.equal(product.variant,'normal');assert.equal(driver.finalErrorAudit.status,'passed');assert.deepEqual(driver.teardownErrors,[]);
    const targets=driver.targets.filter(row=>row.game===product.game&&row.variant===product.variant);assert.equal(targets.length,1);
    const target=targets[0];assert.equal(target.status,'passed');assert.equal(target.finalErrorAudit.status,'passed');assert.deepEqual(target.postflight,target.identity);
    assert.ok(target.checks.includes(entry)&&target.checks.filter(row=>row.id===entry.id).length===1);assert.equal(entry.status,'passed');assert.equal(entry.complete,true);
    const practice=resolveAdaptationAdapter(product.game,product.variant).practice;
    for(const row of target.locales){
      assert.deepEqual(row.exitObserver,{available:true,armed:true});
      assert.deepEqual(row.exitEvidence,{available:true,restored:true,entries:[],dropped:0},'Parameter scene lost its native owner before completing');
      assert.deepEqual(practice.snapshot.decode(row.parameterRaw),row.practiceSnapshot);
      const startup=row.startup;assert.equal(startup.configuration.options.thpracEnabled,true);
      for(const reply of [startup.configured,startup.launched]){assert.equal(reply.ok,true);assert.equal(reply.game,product.game);assert.equal(reply.epoch,row.epoch);assert.ok(reply.request);}
      const order=['ready',startup.configured.request,startup.launched.request,'first-frame'].map(id=>startup.hostEvents.findIndex(e=>e.event===id||e.request===id));
      assert.ok(order.every((value,index)=>value>=0&&(!index||value>order[index-1])),'Parameter configuration missed the native first-frame boundary');
    }
    assert.equal(practice.assessParameterMatrix(target.locales).complete,true);result.valid=true;
  }catch(error){result.reason=error.message;}
  return result;
}
