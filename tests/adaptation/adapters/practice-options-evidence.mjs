import assert from 'node:assert/strict';
import {resolveAdaptationAdapter} from './registry.mjs';
const matrix=[['ja-JP',true],['en-US',true],['zh-CN',true],['en-US',false]];

export function assertPracticeOptionsEvidence({target,adapter,product}){
  assert.equal(target.game,product.game);assert.equal(target.variant,product.variant);assert.equal(target.status,'passed');
  assert.equal(target.feature,true);assert.equal(target.finalErrorAudit.status,'passed');assert.deepEqual(target.postflight,target.identity);
  assert.equal(target.locales.length,4);
  for(const [index,[locale,enabled]] of matrix.entries()){
    const epoch=index+1,row=target.locales[index];assert.equal(row.epoch,epoch);assert.equal(row.enabled,enabled);assert.equal(row.locale,locale);assert.equal(row.status,'passed');
    const e=row.startup;assert.equal(e.configuration.options.thpracEnabled,enabled);assert.equal(e.configuration.options.thpracLocale,locale);
    for(const reply of [e.configured,e.launched]){
      assert.equal(reply.protocol,'eagler-touhou/1');assert.equal(reply.game,product.game);assert.equal(reply.epoch,epoch);assert.equal(reply.ok,true);assert.ok(reply.request);
    }
    assert.notEqual(e.configured.request,e.launched.request);
    const find=predicate=>{const indices=e.hostEvents.flatMap((event,i)=>predicate(event)?[i]:[]);assert.equal(indices.length,1);return indices[0];};
    const ready=find(event=>event.event==='ready'),configured=find(event=>event.request===e.configured.request),launched=find(event=>event.request===e.launched.request),firstFrame=find(event=>event.event==='first-frame');
    assert.ok(ready<configured&&configured<launched&&launched<firstFrame,'Options must be acknowledged before launch and the first frame');
    for(const event of e.hostEvents){assert.equal(event.protocol,'eagler-touhou/1');assert.equal(event.game,product.game);assert.equal(event.epoch,epoch);}
    const stages=target.stages.filter(stage=>stage.epoch===epoch&&['configure','launch','first-frame'].includes(stage.stage));
    assert.deepEqual(stages.map(stage=>stage.stage),['configure','launch','first-frame']);
    for(let i=0;i<stages.length;i++){
      assert.equal(stages[i].status,'passed');assert.ok(Number.isFinite(Date.parse(stages[i].startedAt))&&Date.parse(stages[i].finishedAt)>=Date.parse(stages[i].startedAt));
      if(i)assert.ok(Date.parse(stages[i].startedAt)>=Date.parse(stages[i-1].finishedAt));
    }
    const {identity,menu,snapshot}=e.firstFrame;
    assert.equal(identity.epoch,epoch);assert.ok(Number.isInteger(identity.app)&&identity.app>0&&Number.isFinite(identity.documentTimeOrigin));
    assert.equal(Number(new URL(identity.url).searchParams.get('runtimeEpoch')),epoch);
    assert.equal(menu.available,true);assert.equal(menu.open,false);assert.equal(menu.options.thpracEnabled,enabled);assert.equal(menu.options.thpracLocale,locale);
    const native=adapter.practice.snapshot.decode(snapshot);
    assert.equal(native.state.enabled,enabled,'Native practice owner must receive enable before gameplay');
    assert.equal(native.state.inGame,false);assert.equal(native.state.active,false);
  }
}

export function assessPracticeOptionsEvidence({driver,entry,product}={}){
  if(entry?.id!=='PRACTICE-02')return{available:false,valid:false,kind:null};
  const result={available:true,valid:false,kind:'practice-startup-options',checks:['PRACTICE-02']};
  try{
    assert.equal(driver.schema,'eagler-touhou/practice-browser/1');
    const targets=driver.targets.filter(row=>row.game===product.game&&row.variant===product.variant);assert.equal(targets.length,1);
    const target=targets[0];assert.ok(target.checks.includes(entry)&&target.checks.filter(row=>row.id===entry.id).length===1);
    assertPracticeOptionsEvidence({target,adapter:resolveAdaptationAdapter(product.game,product.variant),product});result.valid=true;
  }catch(error){result.reason=error.message;}
  return result;
}
