import assert from 'node:assert/strict';
import {resolvePracticeAdapter} from './registry.mjs';

const matrix=[['ja-JP',true],['en-US',true],['zh-CN',true],['en-US',false]];

/** Recompute native scene ownership across a single application, independently of verdict flags. */
export function assertPracticeLifecycleEvidence(driver,entry,product){
  assert.equal(driver.schema,'eagler-touhou/practice-browser/1');
  const targets=driver.targets?.filter(target=>target.game===product.game&&target.variant===product.variant)??[];
  assert.equal(targets.length,1,'Practice lifecycle needs one matching product');
  const target=targets[0];
  assert.ok(target.checks.includes(entry)&&target.checks.filter(row=>row.id===entry.id).length===1,'Practice lifecycle claim is not unique');
  assert.equal(target.status,'passed');assert.equal(target.feature,true);
  assert.equal(target.finalErrorAudit?.status,'passed','Practice browser error audit failed');
  assert.deepEqual(target.postflight,target.identity,'Practice Runtime or DATA changed');
  assert.equal(target.locales.length,matrix.length,'Practice locale/disabled matrix is incomplete');
  for(const [index,[locale,enabled]] of matrix.entries()){
    const rows=target.locales.filter(row=>row.epoch===index+1);assert.equal(rows.length,1,'Missing or duplicate practice epoch');
    const row=rows[0];assert.equal(row.status,'passed');assert.equal(row.locale,locale);assert.equal(row.enabled,enabled);
    assert.equal(row.opened.options.thpracEnabled,enabled);assert.equal(row.opened.options.thpracLocale,locale);
    assert.equal(row.before.open,false);assert.equal(row.opened.open,enabled);assert.equal(row.closed.open,false);
    if(enabled){
      assert.ok(row.opened.events.some(event=>event.open===true),'Native menu opening was not observed');
      assert.ok(row.closed.events.slice(row.opened.events.length).some(event=>event.open===false),'Native menu closing was not observed');
    }else assert.deepEqual(row.closed.events,row.before.events,'Disabled practice published a menu transition');
  }
  const adapter=resolvePracticeAdapter(product.game,product.variant);
  assert.equal(typeof adapter?.lifecycle?.assess,'function','Register native practice lifecycle semantics');
  const evidence=target.locales.find(row=>row.epoch===1).practiceLifecycle?.evidence;
  assert.ok(evidence,'Practice lifecycle raw boundaries are missing');
  assert.deepEqual(entry.observation?.observation?.lifecycleEvidence,evidence,'Practice proof differs from the actual raw boundaries');
  // The first non-default setup comes from the registered case, not a second
  // copy of the observed setup or a stored "complete" result.
  assert.deepEqual(Object.fromEntries(Object.keys(adapter.parameterCase).map(key=>[key,evidence.before.configured[key]])),
    adapter.parameterCase,'Practice lifecycle did not start with the independent parameter case');
  assert.equal(evidence.identities.before.epoch,1,'Practice lifecycle belongs to another Runtime epoch');
  const result=adapter.lifecycle.assess(evidence);
  assert.equal(result.status,'observed',`Practice ownership transition failed: ${JSON.stringify(result)}`);
  assert.equal(result.complete,true,'Practice lifecycle has missing native boundaries');
}

export function assessRuntimePracticeLifecycleEvidence({driver,entry,product}={}){
  if(entry?.id!=='PRACTICE-03'||!resolvePracticeAdapter(product?.game,product?.variant)?.lifecycle)return{available:false,valid:false,kind:null};
  try{assertPracticeLifecycleEvidence(driver,entry,product);return{available:true,valid:true,kind:'native-practice-lifecycle',checks:['PRACTICE-03']};}
  catch(error){return{available:true,valid:false,kind:'native-practice-lifecycle',checks:['PRACTICE-03'],reason:error.message};}
}
