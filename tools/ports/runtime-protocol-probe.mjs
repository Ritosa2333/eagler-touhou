import assert from 'node:assert/strict';

const protocol='eagler-touhou/1';
const faults=['protocol','game','epoch','source','origin','command-type'];
const fields=(game,epoch,request)=>({protocol,game,epoch,request,command:'sync'});

/** Real parent requests bracket controlled MessageEvent field faults in the packaged Shell. */
export async function probeRuntimeProtocol(page,{game,epoch}){
  const evidence=await page.evaluate(async({game,epoch,protocol,faults})=>{
    const frame=document.querySelector('iframe'),runtime=frame.contentWindow,origin=location.origin;
    const events=[],capture=event=>{if(event.source===runtime)events.push({data:event.data,origin:event.origin,sourceRuntime:true});};
    addEventListener('message',capture);
    const request=()=>`protocol-acceptance-${crypto.randomUUID()}`;
    const make=id=>({protocol,game,epoch,request:id,command:'sync'});
    const evidence={schema:'eagler-touhou/runtime-protocol-session/1',game,epoch,url:frame.src,origin,
      faultIngress:'controlled-MessageEvent',positiveIngress:'parent-postMessage',cases:[]};
    try{
      evidence.before=await rpc('list');
      const valid=make(request());
      runtime.dispatchEvent(new runtime.MessageEvent('message',{data:valid,origin,source:window}));
      await rpc('list');evidence.injectedPositive={sent:valid,responses:events.filter(event=>event.data?.request===valid.request)};
      for(const fault of faults){
        const sent=make(request());let eventOrigin=origin,sourceKind='parent';
        if(fault==='protocol')sent.protocol='invalid-protocol';
        if(fault==='game')sent.game=`${game}-invalid`;
        if(fault==='epoch')sent.epoch=epoch+111;
        if(fault==='source')sourceKind='runtime';
        if(fault==='origin')eventOrigin='https://invalid.example';
        if(fault==='command-type')sent.command=0;
        runtime.dispatchEvent(new runtime.MessageEvent('message',{data:sent,origin:eventOrigin,source:sourceKind==='parent'?window:runtime}));
        const recovered=await rpc('list');
        evidence.cases.push({fault,sent,eventOrigin,sourceKind,recovered,responses:events.filter(event=>event.data?.request===sent.request)});
      }
      const unknown={...make(request()),command:'acceptance-invalid-command'};
      send(unknown.command,unknown);const recovered=await rpc('list');
      evidence.unknown={sent:unknown,recovered,responses:events.filter(event=>event.data?.request===unknown.request)};
      evidence.after=await rpc('list');return evidence;
    }finally{removeEventListener('message',capture);}
  },{game,epoch,protocol,faults});
  assertRuntimeProtocolSession(evidence,{game,epoch});return evidence;
}

/** Each fault is one changed field; successful requests keep the same live channel responsive. */
export function assertRuntimeProtocolSession(evidence,{game,epoch}={}){
  assert.equal(evidence.schema,'eagler-touhou/runtime-protocol-session/1');assert.equal(evidence.game,game);assert.equal(evidence.epoch,epoch);
  assert.equal(Number(new URL(evidence.url).searchParams.get('runtimeEpoch')),epoch);
  assert.equal(new URL(evidence.url).origin,evidence.origin);
  assert.equal(evidence.faultIngress,'controlled-MessageEvent');assert.equal(evidence.positiveIngress,'parent-postMessage');
  const response=value=>{assert.equal(value.protocol,protocol);assert.equal(value.game,game);assert.equal(value.epoch,epoch);assert.equal(value.ok,true);assert.ok(typeof value.request==='string'&&value.request);};
  response(evidence.before);response(evidence.after);
  const positive=evidence.injectedPositive;
  assert.deepEqual(positive.sent,fields(game,epoch,positive.sent.request));assert.equal(positive.responses.length,1);
  response(positive.responses[0].data);assert.equal(positive.responses[0].data.request,positive.sent.request);
  assert.equal(evidence.cases.length,faults.length);
  const requests=[evidence.before.request,evidence.after.request,positive.sent.request];
  for(const fault of faults){
    const matches=evidence.cases.filter(row=>row.fault===fault);assert.equal(matches.length,1);
    const row=matches[0],expected=fields(game,epoch,row.sent.request);
    if(fault==='protocol')expected.protocol='invalid-protocol';if(fault==='game')expected.game=`${game}-invalid`;
    if(fault==='epoch')expected.epoch=epoch+111;if(fault==='command-type')expected.command=0;
    assert.deepEqual(row.sent,expected);assert.equal(row.eventOrigin,fault==='origin'?'https://invalid.example':evidence.origin);
    assert.equal(row.sourceKind,fault==='source'?'runtime':'parent');assert.deepEqual(row.responses,[],'Malformed protocol message was accepted');
    response(row.recovered);requests.push(row.sent.request,row.recovered.request);
  }
  const unknown=evidence.unknown;assert.deepEqual(unknown.sent,{...fields(game,epoch,unknown.sent.request),command:'acceptance-invalid-command'});
  assert.equal(unknown.responses.length,1);const rejected=unknown.responses[0].data;
  assert.equal(rejected.protocol,protocol);assert.equal(rejected.game,game);assert.equal(rejected.epoch,epoch);
  assert.equal(rejected.request,unknown.sent.request);assert.equal(rejected.ok,false);assert.ok(typeof rejected.error==='string'&&rejected.error&&!rejected.error.startsWith('Timeout:'));
  response(unknown.recovered);requests.push(unknown.sent.request,unknown.recovered.request);
  assert.equal(new Set(requests).size,requests.length,'Protocol requests were reused across cases');
  for(const event of [...positive.responses,...unknown.responses]){assert.equal(event.sourceRuntime,true);assert.equal(event.origin,evidence.origin);}
}
