import assert from 'node:assert/strict';
import vm from 'node:vm';
import {installLiveCallbackCostProbe} from './callback-cost-probe.mjs';

const schema={version:1,header:['version','enabled','epoch','phaseCount','counterCount','gaugeCount'],
 phases:['frame'],counters:['chunks'],gauges:['queuedBytes']};
function fixture(options={}){
 const window={},module={HEAPU8:new Uint8Array(65536)};
 const layout=options.schema??schema;
 module.HEAPU8.set(new TextEncoder().encode(JSON.stringify(layout)+'\0'),1024);
 let epoch=0,enabled=0;
 const values=()=>new Float64Array(module.HEAPU8.buffer,8192,11);
 const reset=()=>{values().set([1,enabled,++epoch,1,1,1,0,0,0,0,0]);};
 module._th09_probe_performance_schema=()=>1024;
 module._th09_probe_performance_enable=value=>{enabled=+!!value;values()[1]=enabled;};
 module._th09_probe_performance_reset=reset;
 module._th09_probe_performance=()=>8192;
 vm.runInNewContext('('+installLiveCallbackCostProbe.toString()+')()',{
  window,performance:{now:()=>100},TextDecoder,Float64Array
 });
 const metadata=index=>({sampleIndex:index,frame:index,confirmed:index,present:index,phase:3,callbackWorkMs:2,measuredAt:100+index});
 return {probe:window.liveCallbackCostProbe,module,values,metadata};
}

{
 const {probe,module,values,metadata}=fixture();probe.attach(module);probe.mark('after-open');
 values().set([1,1,1,1,1,1,1,2,1.5,3,4096]);probe.onFrame(metadata(0));
 const first=probe.snapshot();assert.equal(first.valid,true);assert.equal(first.frames[0].values[9],3);
 const grown=new Uint8Array(131072);grown.set(module.HEAPU8);module.HEAPU8=grown;
 values()[6]=2;values()[7]=4;values()[8]=3;values()[9]=6;values()[10]=2048;probe.onFrame(metadata(1));
 const next=probe.snapshot();assert.equal(next.valid,true);assert.equal(next.frames[1].values[9],6);
 assert.equal(next.frames[0].values[9],3,'A prior sample must remain a copy after memory growth and updates');
 assert.equal(next.frames[1].values[10],2048,'Last-observed gauges may decrease');
 assert.equal(next.fieldNames[7],'frame.inclusiveMs');
}
for(const [name,corrupt] of [
 ['epoch reset',values=>{values[2]++;}],
 ['negative cumulative',values=>{values[9]=-1;}],
 ['non-finite value',values=>{values[7]=NaN;}],
 ['exclusive exceeds inclusive',values=>{values[7]=1;values[8]=2;}],
 ['shape change',values=>{values[3]=2;}],
 ['disabled native counters',values=>{values[1]=0;}]
]){
 const {probe,module,values,metadata}=fixture();probe.attach(module);corrupt(values());probe.onFrame(metadata(0));
 const result=probe.snapshot();assert.equal(result.valid,false,name);assert.ok(result.errors.length,name);
}
{
 const {probe,module,values,metadata}=fixture();probe.attach(module);values()[9]=4;probe.onFrame(metadata(0));
 values()[9]=3;probe.onFrame(metadata(1));assert.equal(probe.snapshot().valid,false,'Counter decreases must be visible');
}
{
 const {probe,module,metadata}=fixture();probe.attach(module);probe.onFrame(metadata(0));probe.onFrame(metadata(2));
 assert.equal(probe.snapshot().valid,false,'Missing callback indices must be visible');
}
{
 const {probe,module}=fixture();delete module._th09_probe_performance_reset;
 assert.throws(()=>probe.attach(module),/Missing DEV-only export/);
}
{
 const {probe,module}=fixture({schema:{...schema,header:['wrong']}});
 assert.throws(()=>probe.attach(module),/header schema mismatch/);
}
{
 const {probe,module,values,metadata}=fixture({schema:{...schema,counters:['timingErrors']}});
 probe.attach(module);values()[9]=1;probe.onFrame(metadata(0));
 assert.equal(probe.snapshot().valid,false,'Native nesting/timing errors must invalidate the observation');
}
console.log('Live callback cost probe: schema, epoch, counters, coverage and heap-growth checks PASS');
