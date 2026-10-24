// Explicit diagnostic instrumentation. The production performance observer does
// not install these wrappers or start the sampler unless requested by its CLI.
// This function is serialized into each browser realm and has no dependencies.
export function installGlDiagnostic(state,{longCallThresholdMs=25,maxLongCalls=2000}={}){
  const methods=['texImage2D','texSubImage2D','texImage3D','texSubImage3D','compressedTexImage2D','compressedTexSubImage2D',
    'bufferData','bufferSubData','drawArrays','drawElements','drawArraysInstanced','drawElementsInstanced',
    'blitFramebuffer','readPixels','getParameter','getError','getBufferSubData','checkFramebufferStatus',
    'getShaderParameter','getProgramParameter','compileShader','linkProgram','finish','flush','clientWaitSync','waitSync'];
  const blank=()=>({calls:0,wallMs:0,maxMs:0,thrownCalls:0,textureUploadBytes:0,textureAllocationBytes:0,
    bufferUploadBytes:0,bufferAllocationBytes:0,unknownTextureUploadCalls:0,methods:{}});
  const diagnostic=state.glDiagnostic={schema:'eagler-touhou/th15-gl-diagnostic/1',longCallThresholdMs,maxLongCalls,
    methods:[],unavailableMethods:[],phases:{},longCalls:[],droppedLongCalls:0,
    meaning:'Observed synchronous WebGL API wall time and specified payload sizes, without querying GL state or reading pixels. Timings include command validation, submission, any synchronous wait and instrumentation overhead; they do not measure GPU execution. The wrappers are diagnostic overhead, so these runs are ineligible for performance acceptance.'};
  const gl=globalThis.WebGL2RenderingContext?.prototype;
  if(!gl){diagnostic.error='WebGL2 prototype unavailable';return diagnostic;}
  function texelBytes(format,type){
    if([32819,32820,33635].includes(type))return 2;
    if([33640,35899,35902,34042].includes(type))return 4;
    if(type===36269)return 8;
    const components=({6403:1,6406:1,6409:1,36244:1,6402:1,33319:2,33320:2,6410:2,6407:3,36248:3,6408:4,36249:4})[format];
    const scalar=({5120:1,5121:1,5122:2,5123:2,5131:2,5124:4,5125:4,5126:4})[type];
    return components&&scalar?components*scalar:null;
  }
  function payload(name,args){
    const bytes={textureUploadBytes:0,textureAllocationBytes:0,bufferUploadBytes:0,bufferAllocationBytes:0,unknownTextureUploadCalls:0};
    if(name==='bufferData'||name==='bufferSubData'){
      const data=args[name==='bufferData'?1:2];
      if(typeof data==='number'){if(name==='bufferData')bytes.bufferAllocationBytes=data;return bytes;}
      if(data?.byteLength!==undefined){const element=data.BYTES_PER_ELEMENT||1,offset=args[3]||0,length=args[4];
        bytes.bufferUploadBytes=length?length*element:Math.max(0,data.byteLength-offset*element);}
      return bytes;
    }
    if(!name.startsWith('tex')&&!name.startsWith('compressedTex'))return bytes;
    let width,height,depth=1,format,type,data;
    if(name==='texImage2D'&&args.length>=9){[width,height,format,type,data]=[args[3],args[4],args[6],args[7],args[8]];}
    else if(name==='texSubImage2D'&&args.length>=9){[width,height,format,type,data]=[args[4],args[5],args[6],args[7],args[8]];}
    else if(name==='texImage3D'&&args.length>=10){[width,height,depth,format,type,data]=[args[3],args[4],args[5],args[7],args[8],args[9]];}
    else if(name==='texSubImage3D'&&args.length>=11){[width,height,depth,format,type,data]=[args[5],args[6],args[7],args[8],args[9],args[10]];}
    else{bytes.unknownTextureUploadCalls=1;return bytes;}
    const texel=texelBytes(format,type);
    if(texel&&[width,height,depth].every(v=>Number.isInteger(v)&&v>=0)){
      bytes[data===null?'textureAllocationBytes':'textureUploadBytes']=width*height*depth*texel;
    }else bytes.unknownTextureUploadCalls=1;
    return bytes;
  }
  function add(bucket,name,ms,thrown,bytes){
    ++bucket.calls;bucket.wallMs+=ms;bucket.maxMs=Math.max(bucket.maxMs,ms);bucket.thrownCalls+=Number(thrown);
    for(const key of Object.keys(bytes))bucket[key]+=bytes[key];
    const row=bucket.methods[name]??=( {calls:0,wallMs:0,maxMs:0,thrownCalls:0,textureUploadBytes:0,textureAllocationBytes:0,
      bufferUploadBytes:0,bufferAllocationBytes:0,unknownTextureUploadCalls:0} );
    ++row.calls;row.wallMs+=ms;row.maxMs=Math.max(row.maxMs,ms);row.thrownCalls+=Number(thrown);
    for(const key of Object.keys(bytes))row[key]+=bytes[key];
  }
  for(const name of methods){
    const original=gl[name];if(typeof original!=='function'){diagnostic.unavailableMethods.push(name);continue;}
    diagnostic.methods.push(name);
    gl[name]=function(...args){
      if(this?.canvas?.id!=='canvas')return original.apply(this,args);
      const current=state.current?.row,entered=performance.now();let threw=true;
      try{const result=original.apply(this,args);threw=false;return result;}
      finally{
        const exited=performance.now(),ms=exited-entered,bytes=payload(name,args);
        const phase=state.phase==='measure'&&(entered<state.start||entered>state.end)?'outside-fixed-window':state.phase;
        const totals=diagnostic.phases[phase]??={all:blank(),outsideCallback:blank()};
        add(totals.all,name,ms,threw,bytes);
        if(current)add(current.glDiagnostic??=blank(),name,ms,threw,bytes);
        else add(totals.outsideCallback,name,ms,threw,bytes);
        if(ms>=longCallThresholdMs){
          const event={method:name,entered,exited,ms,phase,callbackId:current?.id??null,knownNativeCallback:current?.native??false,threw,...bytes};
          if(diagnostic.longCalls.length<maxLongCalls)diagnostic.longCalls.push(event);else ++diagnostic.droppedLongCalls;
        }
      }
    };
  }
  return diagnostic;
}

// A page CDP session samples the renderer containing the same-origin Runtime
// iframe. It does not sample the separate GPU/SwiftShader process. Clock pairs
// bound the sample interval relative to the page's performance timeline.
const pageClock=page=>page.evaluate(()=>({timeOrigin:performance.timeOrigin,now:performance.now(),dateNow:Date.now()}));
export async function startCpuDiagnostic(page,{samplingIntervalUs=4000}={}){
  const session=await page.context().newCDPSession(page),metadata={samplingIntervalUs,
    scope:'Page renderer CPU samples including the same-origin Runtime iframe; not the GPU process.',started:false,stopped:false};
  let enabled=false;
  try{
    await session.send('Profiler.enable');enabled=true;
    await session.send('Profiler.setSamplingInterval',{interval:samplingIntervalUs});
    metadata.beforeStart=await pageClock(page);await session.send('Profiler.start');metadata.started=true;
    metadata.afterStart=await pageClock(page);return {session,page,metadata};
  }catch(error){
    if(metadata.started)try{await session.send('Profiler.stop');}catch{}
    if(enabled)try{await session.send('Profiler.disable');}catch{}
    try{await session.detach();}catch{}
    throw error;
  }
}
export async function stopCpuDiagnostic(handle){
  if(handle.metadata.stopped)throw Error('CPU diagnostic already stopped');
  try{
    handle.metadata.beforeStop=await pageClock(handle.page);
    const {profile}=await handle.session.send('Profiler.stop');handle.metadata.stopped=true;
    handle.metadata.afterStop=await pageClock(handle.page);
    handle.metadata.profileStartTime=profile.startTime;handle.metadata.profileEndTime=profile.endTime;
    handle.metadata.nodes=profile.nodes?.length??0;handle.metadata.samples=profile.samples?.length??0;
    return {profile,metadata:handle.metadata};
  }finally{
    handle.closed=true;
    try{await handle.session.send('Profiler.disable');}finally{await handle.session.detach();}
  }
}
