import test from 'node:test';
import assert from 'node:assert/strict';
import {assertRuntimeProtocolSession} from './runtime-protocol-probe.mjs';
import {assessRuntimeProtocolEvidence} from '../../tests/adaptation/adapters/runtime-protocol-evidence.mjs';

function session(){
  let id=0;const game='th08',epoch=1,origin='https://127.0.0.1:8123',protocol='eagler-touhou/1';
  const sent=()=>({protocol,game,epoch,request:`request-${++id}`,command:'sync'});
  const response=()=>({protocol,game,epoch,request:`request-${++id}`,ok:true,files:[]});
  const positive=sent(),positiveResponse={data:{...positive,ok:true},origin,sourceRuntime:true};
  const cases=['protocol','game','epoch','source','origin','command-type'].map(fault=>{
    const message=sent();if(fault==='protocol')message.protocol='invalid-protocol';if(fault==='game')message.game='th08-invalid';
    if(fault==='epoch')message.epoch=112;if(fault==='command-type')message.command=0;
    return{fault,sent:message,eventOrigin:fault==='origin'?'https://invalid.example':origin,sourceKind:fault==='source'?'runtime':'parent',responses:[],recovered:response()};
  });
  const unknown={...sent(),command:'acceptance-invalid-command'};
  return{schema:'eagler-touhou/runtime-protocol-session/1',game,epoch,url:origin+'/game.html?runtimeEpoch=1',origin,
    faultIngress:'controlled-MessageEvent',positiveIngress:'parent-postMessage',before:response(),after:response(),cases,
    injectedPositive:{sent:positive,responses:[positiveResponse]},unknown:{sent:unknown,recovered:response(),responses:[
      {origin,sourceRuntime:true,data:{protocol,game,epoch,request:unknown.request,ok:false,error:'Unsupported runtime command: acceptance-invalid-command'}}]}};
}

test('protocol field faults retain a positive control and responsive real parent channel',()=>{
  assertRuntimeProtocolSession(JSON.parse(JSON.stringify(session())),{game:'th08',epoch:1});
});
for(const [name,mutate] of [
  ['accepted wrong identity',e=>e.cases[0].responses.push({data:{ok:true}})],
  ['dropped valid injection',e=>e.injectedPositive.responses=[]],
  ['broken channel after fault',e=>e.cases[2].recovered.ok=false],
  ['borrowed response',e=>e.cases[1].recovered.epoch=2],
  ['wrong source was never sent',e=>e.cases[3].sourceKind='parent'],
  ['wrong origin was never sent',e=>e.cases[4].eventOrigin=e.origin],
  ['unknown command times out',e=>e.unknown.responses[0].data.error='Timeout: command'],
  ['unknown command accepted',e=>e.unknown.responses[0].data.ok=true],
  ['reused request identity',e=>e.cases[1].sent.request=e.cases[0].sent.request],
])test(`protocol completeness rejects ${name}`,()=>{
  const evidence=session();mutate(evidence);assert.throws(()=>assertRuntimeProtocolSession(evidence,{game:'th08',epoch:1}));
});

function startup(){
  const base={protocol:'eagler-touhou/1',game:'th08',epoch:1};
  const ready={...base,event:'ready'},configured={...base,request:'configure',ok:true},launched={...base,request:'launch',ok:true};
  const firstFrame={...base,event:'first-frame',presentation:{schema:'eagler-runtime/first-frame/1',nativePresentCount:1}};
  const entry={id:'configure-launch-first-frame',status:'passed',checks:['RUNTIME-02'],observation:{canvas:{width:640,height:480},
    startup:{ready,configured,launched,firstFrame,events:[ready,configured,launched,firstFrame]}}};
  const target={game:'th08',variant:'normal',checks:[entry],finalErrorAudit:{status:'passed'},packageIdentity:{sha256:'1'.repeat(64)},dataIdentity:{sha256:'2'.repeat(64)}};
  target.inputPostflight={packageIdentity:structuredClone(target.packageIdentity),dataIdentity:structuredClone(target.dataIdentity)};
  return{driver:{schema:'eagler-touhou/runtime-browser/1',targets:[target]},entry,product:{game:'th08',variant:'normal'}};
}
test('startup completeness uses the original event sequence and native presentation count',()=>{
  assert.equal(assessRuntimeProtocolEvidence(startup()).valid,true);
  for(const mutate of [e=>e.entry.observation.startup.events.reverse(),e=>e.entry.observation.canvas.width=0,
    e=>e.entry.observation.startup.firstFrame.presentation.nativePresentCount=0,e=>e.entry.observation.startup.launched.epoch=2]){
    const evidence=startup();mutate(evidence);assert.equal(assessRuntimeProtocolEvidence(evidence).valid,false);
  }
});
