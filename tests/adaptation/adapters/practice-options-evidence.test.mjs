import test from 'node:test';
import assert from 'node:assert/strict';
import {assertPracticeOptionsEvidence} from './practice-options-evidence.mjs';
import {resolveAdaptationAdapter} from './registry.mjs';
const product={game:'th08',variant:'normal'},adapter=resolveAdaptationAdapter('th08','normal');
function fixture(){
  const target={...product,status:'passed',feature:true,finalErrorAudit:{status:'passed'},identity:{hash:'fixture'},postflight:{hash:'fixture'},locales:[],stages:[]};
  for(const [index,[locale,enabled]] of [['ja-JP',true],['en-US',true],['zh-CN',true],['en-US',false]].entries()){
    const epoch=index+1,base={protocol:'eagler-touhou/1',game:'th08',epoch};
    const configured={...base,request:`configure-${epoch}`,ok:true},launched={...base,request:`launch-${epoch}`,ok:true};
    const words=Array(128).fill(0);words[0]=1;words[1]=enabled?1:0;words[2]=Number(enabled);words[42]=1;
    const options={thpracEnabled:enabled,thpracLocale:locale};
    target.locales.push({epoch,locale,enabled,status:'passed',startup:{configuration:{options},configured,launched,
      hostEvents:[{...base,event:'ready'},configured,launched,{...base,event:'first-frame'}],
      firstFrame:{menu:{available:true,open:false,options:{...options}},snapshot:{available:true,words},identity:{epoch,app:100,documentTimeOrigin:1000,url:`https://fixture/runtime?runtimeEpoch=${epoch}`}}}});
    for(const [i,stage] of ['configure','launch','first-frame'].entries())target.stages.push({epoch,stage,status:'passed',startedAt:new Date(1000+i*10).toISOString(),finishedAt:new Date(1005+i*10).toISOString()});
  }
  return target;
}
test('practice startup proof binds four enable/locale combinations before native gameplay',()=>{
  assertPracticeOptionsEvidence({target:fixture(),adapter,product});
});
for(const [name,mutate] of Object.entries({
  'missing disabled control':t=>t.locales.pop(),
  'late configuration':t=>t.locales[0].startup.hostEvents.reverse(),
  'unacknowledged configure':t=>t.locales[0].startup.configured.ok=false,
  'wrong locale':t=>t.locales[1].startup.firstFrame.menu.options.thpracLocale='ja-JP',
  'native enable omitted':t=>t.locales[0].startup.firstFrame.snapshot.words[2]=0,
  'snapshot captured after entering gameplay':t=>t.locales[0].startup.firstFrame.snapshot.words[9]=1,
  'foreign native frame':t=>t.locales[0].startup.firstFrame.identity.url='https://fixture/runtime?runtimeEpoch=2',
  'missing startup stage':t=>t.stages.shift(),
  'failed error audit':t=>t.finalErrorAudit.status='failed',
  'package changed':t=>t.postflight.hash='changed',
}))test(`practice startup rejects ${name}`,()=>{const target=fixture();mutate(target);assert.throws(()=>assertPracticeOptionsEvidence({target,adapter,product}));});
