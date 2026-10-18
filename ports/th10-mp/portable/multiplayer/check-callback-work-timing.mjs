// Offline ABI and startup-boundary tests. No browser, game tick, or scheduler runs.
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {
 callbackWorkPhases,callbackWorkTimingSchema,
 configureCallbackWorkTiming,readCallbackWorkTiming,
 observeCallbackWorkTiming,stopCallbackWorkTiming,
} from './callback-work-timing.mjs';
import {startPerformance} from './performance-driver.mjs';

const EXPECTED_PHASES=[
 'forwardCapture','historyCapture','forwardUpdate','historyUpdate',
 'forwardSemanticDraw','historySemanticDraw','restore','confirmedCommit',
];
const EXPECTED_SCHEMA=[
 'schema','enabled','valid','sequence','epoch','owner','generation',
 'frameBefore','frameAfter','exitReason','nativeInputAndStepMs','residualMs',
 'forwardElidedCalls','historyElidedCalls','diagnosticFlags','reserved',
 'forwardCaptureCalls','forwardCaptureMs','historyCaptureCalls','historyCaptureMs',
 'forwardUpdateCalls','forwardUpdateMs','historyUpdateCalls','historyUpdateMs',
 'forwardSemanticDrawCalls','forwardSemanticDrawMs','historySemanticDrawCalls','historySemanticDrawMs',
 'restoreCalls','restoreMs','confirmedCommitCalls','confirmedCommitMs',
];
const WORDS=32,BYTES=256;
const col=Object.fromEntries(EXPECTED_SCHEMA.map((name,index)=>[name,index]));
function published(overrides={}){
 const row=[
  1,1,1,9,7,4096,0,100,101,0,12.75,.75,1,2,0,0,
  1,1,2,1.5,1,2,2,3,1,1,2,1.5,1,1,1,1,
 ];
 for(const [name,value] of Object.entries(overrides))row[col[name]]=value;
 return row;
}
function fixture(row=published(),pointer=64){
 const memory=new WebAssembly.Memory({initial:1,maximum:4});
 new Float64Array(memory.buffer,64,WORDS).set(row);
 const calls={get:0,set:0,arguments:[]};
 const core={
  memory,
  sdl_callback_work_timing_status(){++calls.get;return pointer;},
  sdl_callback_work_timing_set_enabled(value){++calls.set;calls.arguments.push(value);return 1;},
 };
 return {core,memory,calls,row};
}
const groups=[];
let assertions=0;
const test=(name,run)=>groups.push({name,run});
const equal=(actual,expected,message)=>{++assertions;assert.equal(actual,expected,message);};
const deep=(actual,expected,message)=>{++assertions;assert.deepEqual(actual,expected,message);};
const ok=(value,message)=>{++assertions;assert.ok(value,message);};
const rejects=(run,match)=>{++assertions;assert.throws(run,match);};

test('independent 32-word ABI and phase order are immutable',()=>{
 deep(callbackWorkPhases,EXPECTED_PHASES);
 deep(callbackWorkTimingSchema,EXPECTED_SCHEMA);
 equal(callbackWorkTimingSchema.length,WORDS);
 equal(callbackWorkTimingSchema.length*8,BYTES);
 equal(new Set(callbackWorkTimingSchema).size,WORDS);
 ok(Object.isFrozen(callbackWorkPhases));
 ok(Object.isFrozen(callbackWorkTimingSchema));
});

test('OFF and omitted options bypass every poisoned core property',()=>{
 let reads=0;
 const poison=new Proxy({}, {get(){++reads;throw Error('OFF touched core');}});
 equal(readCallbackWorkTiming(poison),null);
 equal(readCallbackWorkTiming(poison,false),null);
 equal(readCallbackWorkTiming(undefined),null);
 equal(configureCallbackWorkTiming(poison),null);
 equal(configureCallbackWorkTiming(poison,{}),null);
 equal(configureCallbackWorkTiming(poison,{enabled:false,parentEnabled:false}),null);
 equal(configureCallbackWorkTiming(poison,{enabled:false,parentEnabled:true}),null);
 equal(configureCallbackWorkTiming(undefined),null);
 equal(reads,0);
});

test('reader requires a primitive boolean before accessing core',()=>{
 let reads=0;
 const poison=new Proxy({}, {get(){++reads;throw Error('Invalid flag touched core');}});
 for(const enabled of [null,0,1,'','false',[],{},new Boolean(false),NaN,Symbol('flag')])
  rejects(()=>readCallbackWorkTiming(poison,enabled),TypeError);
 equal(readCallbackWorkTiming(poison,undefined),null);
 equal(reads,0);
});

test('configuration strictly checks both booleans including disabled parent options',()=>{
 let reads=0;
 const poison=new Proxy({}, {get(){++reads;throw Error('Invalid options touched core');}});
 for(const value of [null,0,1,'false',[],{},new Boolean(true),NaN,Symbol('flag')]){
  rejects(()=>configureCallbackWorkTiming(poison,{enabled:value,parentEnabled:true}),TypeError);
  rejects(()=>configureCallbackWorkTiming(poison,{enabled:true,parentEnabled:value}),TypeError);
  rejects(()=>configureCallbackWorkTiming(poison,{enabled:false,parentEnabled:value}),TypeError);
 }
 equal(reads,0);
});

test('parent timing dependency rejects without touching native exports',()=>{
 let reads=0;
 const poison=new Proxy({}, {get(){++reads;throw Error('Missing parent touched core');}});
 rejects(()=>configureCallbackWorkTiming(poison,{enabled:true}),/requires the original callback timing/);
 rejects(()=>configureCallbackWorkTiming(poison,{enabled:true,parentEnabled:false}),/requires the original callback timing/);
 equal(reads,0);
});

test('missing and nonfunction status exports reject before reading memory',()=>{
 for(const getter of [undefined,null,0,1,true,'status',{}]){
  let memoryReads=0;
  const core={sdl_callback_work_timing_status:getter,
   get memory(){++memoryReads;throw Error('Missing status read memory');}};
  rejects(()=>readCallbackWorkTiming(core,true),/status export required/);
  equal(memoryReads,0);
 }
});

test('missing and nonfunction controls reject before status and memory',()=>{
 for(const setter of [undefined,null,0,1,true,'control',{}]){
  let reads=0;
  const core={sdl_callback_work_timing_set_enabled:setter,
   get sdl_callback_work_timing_status(){++reads;throw Error('Missing control read status');},
   get memory(){++reads;throw Error('Missing control read memory');}};
  rejects(()=>configureCallbackWorkTiming(core,{enabled:true,parentEnabled:true}),/control export required/);
  equal(reads,0);
 }
});

test('setter acknowledgement must be numeric one and rejection does not read status',()=>{
 for(const answer of [0,-1,-2,-3,2,false,true,null,undefined,'1',NaN]){
  let calls=0,reads=0;
  const core={
   sdl_callback_work_timing_set_enabled(value){++calls;equal(value,1);return answer;},
   get sdl_callback_work_timing_status(){++reads;throw Error('Rejected setter read status');},
  };
  rejects(()=>configureCallbackWorkTiming(core,{enabled:true,parentEnabled:true}),/option rejected/);
  equal(calls,1);
  equal(reads,0);
 }
});

test('accepted configure calls setter then getter then current memory exactly once',()=>{
 const events=[],memory=new WebAssembly.Memory({initial:1});
 const row=published();
 new Float64Array(memory.buffer,64,WORDS).set(row);
 const core={
  sdl_callback_work_timing_set_enabled(value){events.push(['set',value]);return 1;},
  sdl_callback_work_timing_status(){events.push(['get']);return 64;},
  get memory(){events.push(['memory']);return memory;},
 };
 deep(configureCallbackWorkTiming(core,{enabled:true,parentEnabled:true}),row);
 deep(events,[['set',1],['get'],['memory']]);
});

test('pointer domain alignment and out-of-bounds are rejected',()=>{
 const bad=[0,-8,1,4,12,64.5,NaN,Infinity,-Infinity,Number.MAX_SAFE_INTEGER+1,
  '64',64n,null,undefined,65536-BYTES+8,65536,65536+8];
 for(const pointer of bad){
  const f=fixture();
  f.core.sdl_callback_work_timing_status=()=>pointer;
  rejects(()=>readCallbackWorkTiming(f.core,true),/outside current memory/);
 }
 const short={memory:{buffer:new ArrayBuffer(128)},sdl_callback_work_timing_status:()=>8};
 rejects(()=>readCallbackWorkTiming(short,true),/outside current memory/);
});

test('lowest aligned pointer and exact final byte boundary are valid',()=>{
 for(const pointer of [8,65536-BYTES]){
  const f=fixture(published(),pointer);
  new Float64Array(f.memory.buffer,pointer,WORDS).set(f.row);
  deep(readCallbackWorkTiming(f.core,true),f.row);
  equal(f.calls.get,1);
 }
});

test('unknown schemas and every nonfinite field reject without normalization',()=>{
 for(const schema of [0,2,-1,1.5]){
  const f=fixture(published({schema}));
  rejects(()=>readCallbackWorkTiming(f.core,true),/Unsupported or non-finite/);
 }
 for(let index=0;index<WORDS;++index)for(const value of [NaN,Infinity,-Infinity]){
  const row=published();row[index]=value;
  const f=fixture(row);
  rejects(()=>readCallbackWorkTiming(f.core,true),/Unsupported or non-finite/);
  ok(Object.is(new Float64Array(f.memory.buffer,64,WORDS)[index],value));
 }
});

test('invalid publications flags and substantial negative residual remain raw evidence',()=>{
 const row=published({valid:0,residualMs:-3.125,diagnosticFlags:(1<<5)|(1<<9),reserved:17});
 const f=fixture(row);
 const result=readCallbackWorkTiming(f.core,true);
 deep(result,row);
 equal(result[col.valid],0);
 equal(result[col.diagnosticFlags],544);
 equal(result[col.residualMs],-3.125);
 equal(result[col.reserved],17);
 // The reader never claims native validity; the independent analyzer rejects it.
 deep(configureCallbackWorkTiming(f.core,{enabled:true,parentEnabled:true}),row);
});

test('initial unpublished row and u32 INVALID_FRAME survive unchanged',()=>{
 const row=Array(WORDS).fill(0);
 row[0]=1;row[col.frameBefore]=row[col.frameAfter]=0xffffffff;
 row[col.exitReason]=-1;row[col.diagnosticFlags]=1;
 const f=fixture(row);
 deep(readCallbackWorkTiming(f.core,true),row);
 equal(readCallbackWorkTiming(f.core,true)[col.frameAfter],4294967295);
 const finite=published({sequence:Number.MAX_SAFE_INTEGER,frameBefore:0xffffffff,
  frameAfter:0xffffffff,residualMs:-0});
 deep(readCallbackWorkTiming(fixture(finite).core,true),finite);
 ok(Object.is(readCallbackWorkTiming(fixture(finite).core,true)[col.residualMs],-0));
});

test('returned rows are detached copies of the current publication',()=>{
 const f=fixture(),first=readCallbackWorkTiming(f.core,true),expected=first.slice();
 first[col.forwardUpdateMs]=900;
 equal(new Float64Array(f.memory.buffer,64,WORDS)[col.forwardUpdateMs],expected[col.forwardUpdateMs]);
 new Float64Array(f.memory.buffer,64,WORDS)[col.sequence]=1000;
 equal(first[col.sequence],expected[col.sequence]);
 equal(readCallbackWorkTiming(f.core,true)[col.sequence],1000);
 ok(Array.isArray(first));
 equal(first.length,WORDS);
});

test('getter memory.grow uses the new buffer rather than a detached cached view',()=>{
 const f=fixture(),oldBuffer=f.memory.buffer,oldView=new Float64Array(oldBuffer,64,WORDS);
 const row=published({sequence:123,residualMs:-.0000005});
 const pointer=65536+8;
 f.core.sdl_callback_work_timing_status=()=>{
  ++f.calls.get;f.memory.grow(1);
  new Float64Array(f.memory.buffer,pointer,WORDS).set(row);
  return pointer;
 };
 deep(readCallbackWorkTiming(f.core,true),row);
 equal(f.calls.get,1);
 equal(oldBuffer.byteLength,0);
 equal(oldView.byteLength,0);
 equal(f.memory.buffer.byteLength,131072);
});

test('configure handles separate setter and getter growth and keeps call order',()=>{
 const f=fixture(),events=[],row=published({epoch:21});
 f.core.sdl_callback_work_timing_set_enabled=value=>{
  events.push(['set',value]);f.memory.grow(1);return 1;
 };
 f.core.sdl_callback_work_timing_status=()=>{
  events.push(['get']);f.memory.grow(1);
  const pointer=2*65536+8;
  new Float64Array(f.memory.buffer,pointer,WORDS).set(row);return pointer;
 };
 deep(configureCallbackWorkTiming(f.core,{enabled:true,parentEnabled:true}),row);
 deep(events,[['set',1],['get']]);
 equal(f.memory.buffer.byteLength,3*65536);
});

test('native getter and setter exceptions preserve their original error',()=>{
 const getterError=Error('native getter sentinel'),setterError=Error('native setter sentinel');
 let memoryReads=0,statusReads=0;
 const getterCore={
  sdl_callback_work_timing_status(){throw getterError;},
  get memory(){++memoryReads;throw Error('Getter failed before memory');},
 };
 rejects(()=>readCallbackWorkTiming(getterCore,true),error=>error===getterError);
 equal(memoryReads,0);
 const setterCore={
  sdl_callback_work_timing_set_enabled(){throw setterError;},
  get sdl_callback_work_timing_status(){++statusReads;throw Error('Setter failed before status');},
 };
 rejects(()=>configureCallbackWorkTiming(setterCore,{enabled:true,parentEnabled:true}),error=>error===setterError);
 equal(statusReads,0);
});

test('real driver rejects explicit nonbooleans before any API property or side effect',()=>{
 let reads=0;
 const poisonError=Error('driver API accessed'),api=new Proxy({}, {
  get(){++reads;throw poisonError;},
 });
 for(const name of ['callbackTiming','callbackWorkTiming'])
  for(const value of ['false',1,0,null,[],{},new Boolean(false),NaN]){
   rejects(()=>startPerformance(api,{[name]:value}),error=>
    error instanceof TypeError&&error.message===name+' must be boolean');
   equal(reads,0);
  }
 // Valid false and omitted values reach normal API setup; the poison stops there.
 for(const options of [{},{callbackTiming:false,callbackWorkTiming:false},
                        {callbackTiming:true,callbackWorkTiming:false}]){
  const before=reads;
  rejects(()=>startPerformance(api,options),error=>error===poisonError);
  equal(reads,before+1);
 }
});

test('runtime observation failure is bounded and stop disables without rereading a bad getter',()=>{
 let gets=0,sets=0;
 const core={sdl_callback_work_timing_status(){++gets;throw Error('x'.repeat(2000));},
  sdl_callback_work_timing_set_enabled(value){equal(value,0);++sets;return 0;}};
 const report={};
 equal(observeCallbackWorkTiming(core,true,report,'callback'),null);
 equal(report.callbackWorkTimingError.stage,'callback');
 equal(report.callbackWorkTimingError.message.length,512);
 const first=report.callbackWorkTimingError;
 equal(observeCallbackWorkTiming(core,true,report,'callback'),null);
 stopCallbackWorkTiming(core,true,report);
 equal(gets,1);equal(sets,1);equal(report.callbackWorkTimingError,first);
 equal(report.finalCallbackWorkTiming,null);equal(report.callbackWorkTimingDisableResult,0);
});

test('stop retains invalid finite publications and reports disable failures explicitly',()=>{
 for(const result of [0,1,-2,false]){
  const f=fixture(published({valid:0,diagnosticFlags:512,residualMs:-3})),report={};
  f.core.sdl_callback_work_timing_set_enabled=()=>result;
  stopCallbackWorkTiming(f.core,true,report);
  deep(report.finalCallbackWorkTiming,f.row);
  equal(report.callbackWorkTimingDisableResult,result);
  equal(Boolean(report.callbackWorkTimingError),result!==0);
  if(result!==0)equal(report.callbackWorkTimingError.stage,'disable');
 }
 const f=fixture(),report={};
 f.core.sdl_callback_work_timing_set_enabled=()=>{throw Error('disable sentinel');};
 stopCallbackWorkTiming(f.core,true,report);
 deep(report.finalCallbackWorkTiming,f.row);
 deep(report.callbackWorkTimingError,{stage:'disable',message:'disable sentinel'});
});

test('observation OFF bypasses poisoned report and native properties',()=>{
 let reads=0;
 const poison=new Proxy({},{get(){++reads;throw Error('OFF access');},set(){++reads;throw Error('OFF write');}});
 equal(observeCallbackWorkTiming(poison,false,poison,'callback'),null);
 equal(stopCallbackWorkTiming(poison,false,poison),undefined);equal(reads,0);
});

test('first observation error survives failed disable and unprintable native errors',()=>{
 let sets=0;
 const report={},core={sdl_callback_work_timing_status(){throw {get message(){throw Error('message getter');}};},
  sdl_callback_work_timing_set_enabled(){++sets;throw Error('later disable failure');}};
 stopCallbackWorkTiming(core,true,report);
 deep(report.callbackWorkTimingError,{stage:'finish',message:'Unprintable callback work timing error'});
 equal(report.finalCallbackWorkTiming,null);equal(sets,1);
});

const results=[];
for(const {name,run} of groups){
 const before=assertions;
 try{run();results.push({name,passed:true,assertions:assertions-before});}
 catch(error){results.push({name,passed:false,assertions:assertions-before,
  error:{name:error?.name??'Error',message:String(error?.message??error).slice(0,1000)}});}
}
const sourceIdentity={};
let identityError=null;
try{
 for(const name of ['callback-work-timing.mjs','performance-driver.mjs','check-callback-work-timing.mjs']){
  const bytes=readFileSync(new URL(name,import.meta.url));
  sourceIdentity[name]={bytes:bytes.length,sha256:createHash('sha256').update(bytes).digest('hex')};
 }
}catch(error){identityError=String(error?.message??error).slice(0,500);}
const passed=!identityError&&results.every(result=>result.passed);
console.log(JSON.stringify({
 schema:'th10-callback-work-timing-tests/1',passed,groups:results.length,
 passedGroups:results.filter(result=>result.passed).length,assertions,results,sourceIdentity,
 identityError,scope:'Offline real JS helper and real driver preflight only; no native loop, browser, timing performance or arithmetic-mode claim.',
}));
if(!passed)process.exitCode=1;
