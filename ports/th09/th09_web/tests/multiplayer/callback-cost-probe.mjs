// Opt-in observation of the DEV-only native performance counters.
// This never advances the game or changes its input, audio, or render policy.
export function installLiveCallbackCostProbe() {
 const limits={frames:65536,marks:128,errors:32,schemaBytes:32768,fields:1024};
 const expectedHeader=['version','enabled','epoch','phaseCount','counterCount','gaugeCount'];
 const frames=[],marks=[],errors=[];
 const overflow={frames:false,marks:false,errors:false};
 let core=null,schema=null,fieldNames=null,epoch=null,lastValues=null,attached=false;
 let attachAt=null,fields=0,cumulativeEnd=0,attempts=0;
 const fail=message=>{
  if(errors.length<limits.errors)errors.push({at:performance.now(),message:String(message).slice(0,320)});
  else overflow.errors=true;
 };
 const require=(condition,message)=>{if(!condition)throw Error(message);};
 const names=(value,label)=>{
  require(Array.isArray(value)&&value.length<=128,'Invalid '+label+' list');
  require(value.every(name=>typeof name==='string'&&name.length>0&&name.length<=120),'Invalid '+label+' name');
  require(new Set(value).size===value.length,'Duplicate '+label+' names');
  return value;
 };
 const readSchema=()=>{
  const pointer=core._th09_probe_performance_schema(),heap=core.HEAPU8;
  require(Number.isInteger(pointer)&&pointer>0&&pointer<heap.length,'Invalid native schema pointer');
  let end=pointer;
  while(end<heap.length&&end-pointer<limits.schemaBytes&&heap[end]!==0)++end;
  require(end<heap.length&&end-pointer<limits.schemaBytes&&heap[end]===0,'Unterminated native schema');
  const value=JSON.parse(new TextDecoder('utf-8',{fatal:true}).decode(heap.subarray(pointer,end)));
  require(value.version===1,'Unsupported native performance schema');
  require(JSON.stringify(value.header)===JSON.stringify(expectedHeader),'Native header schema mismatch');
  names(value.phases,'phase');names(value.counters,'counter');names(value.gauges,'gauge');
  fields=6+value.phases.length*3+value.counters.length+value.gauges.length;
  require(fields<=limits.fields,'Native performance field limit exceeded');
  cumulativeEnd=6+value.phases.length*3+value.counters.length;
  fieldNames=[...value.header,...value.phases.flatMap(name=>[name+'.calls',name+'.inclusiveMs',name+'.exclusiveMs']),
   ...value.counters.map(name=>'counter.'+name),...value.gauges.map(name=>'gauge.'+name)];
  return value;
 };
 const read=()=>{
  require(attached&&core&&schema,'Native performance probe is not attached');
  const pointer=core._th09_probe_performance(),heap=core.HEAPU8;
  require(Number.isInteger(pointer)&&pointer>0&&pointer%8===0&&pointer+fields*8<=heap.byteLength,'Invalid native counter range');
  // Obtain a fresh view on every observation: Wasm heap growth replaces views.
  const values=Array.from(new Float64Array(heap.buffer,heap.byteOffset+pointer,fields));
  require(values.every(Number.isFinite),'Non-finite native performance value');
  require(values[0]===1&&values[1]===1,'Native performance probe is not enabled');
  require(values[3]===schema.phases.length&&values[4]===schema.counters.length&&values[5]===schema.gauges.length,'Native performance shape changed');
  const timingErrors=schema.counters.indexOf('timingErrors');
  require(timingErrors<0||values[6+schema.phases.length*3+timingErrors]===0,'Native performance timing error');
  require(Number.isInteger(values[2])&&values[2]>=0,'Invalid native performance epoch');
  if(epoch===null)epoch=values[2];
  require(values[2]===epoch,'Native performance counters were reset during observation');
  for(let index=6;index<cumulativeEnd;++index){
   require(values[index]>=0,'Negative cumulative native performance value');
   if(lastValues)require(values[index]+1e-6>=lastValues[index],'Cumulative native performance value decreased');
  }
  for(let index=6;index<6+schema.phases.length*3;index+=3){
   require(Number.isInteger(values[index]),'Non-integral native phase call count');
   require(values[index+2]<=values[index+1]+1e-6,'Exclusive native phase time exceeds inclusive time');
  }
  lastValues=values;
  return values;
 };
 const mark=label=>{
  if(marks.length>=limits.marks){overflow.marks=true;return;}
  try{marks.push({label:String(label).slice(0,120),observedAt:performance.now(),values:read()});}
  catch(error){fail(error.message);}
 };
 window.liveCallbackCostProbe={
  attach(module){
   require(!core,'Native performance probe attached more than once');core=module;
   for(const name of ['_th09_probe_performance_enable','_th09_probe_performance_reset','_th09_probe_performance','_th09_probe_performance_schema'])
    require(typeof core[name]==='function','Missing DEV-only export '+name);
   schema=readSchema();core._th09_probe_performance_enable(1);core._th09_probe_performance_reset();
   attachAt=performance.now();attached=true;mark('before-open');
   require(!errors.length,'Native performance probe could not start');
  },
  mark,
  onFrame(metadata){
   ++attempts;
   if(frames.length>=limits.frames){overflow.frames=true;return;}
   try{
    require(metadata&&Number.isInteger(metadata.sampleIndex)&&metadata.sampleIndex>=0,'Invalid frame sample index');
    if(frames.length)require(metadata.sampleIndex===frames.at(-1).sampleIndex+1,'Native cost frame sample gap');
    for(const key of ['frame','confirmed','present','phase','callbackWorkMs','measuredAt'])
     require(Number.isFinite(metadata[key]),'Invalid frame metadata '+key);
    frames.push({sampleIndex:metadata.sampleIndex,frame:metadata.frame,confirmed:metadata.confirmed,
     present:metadata.present,phase:metadata.phase,callbackWorkMs:metadata.callbackWorkMs,
     measuredAt:metadata.measuredAt,observedAt:performance.now(),values:read()});
   }catch(error){fail(error.message);}
  },
  snapshot(){
   let finalValues=null;
   if(attached)try{finalValues=read();}catch(error){fail(error.message);}
   return {version:1,attached,attachAt,snapshotAt:performance.now(),epoch,limits,attempts,overflow,errors,
    valid:attached&&errors.length===0&&!Object.values(overflow).some(Boolean),schema,fieldNames,marks,frames,finalValues,
    clock:'Browser performance.now() for observation times; native elapsed counter units are milliseconds.',
    scope:'DEV-only opt-in cumulative live callback, startup, renderer and audio measurements. Observed child scopes are subtracted from parent exclusive time. Elapsed time can include descheduling or driver waits; it is not process CPU time.',
    limitations:[
     'The instrumentation adds timers, counter updates and JS copies; this is not a release performance acceptance run.',
     'A difference between adjacent observations includes all measured native work in that interval, possibly more than one callback; phase call-count differences retain that distinction.',
     'Counters cover named scopes, not all browser/GPU/audio work; physical input and acoustic/display latency are outside this observation.',
     'Gauges are last-observed values; unchanged gauges do not prove the associated subsystem was idle.'
    ]};
  }
 };
}
