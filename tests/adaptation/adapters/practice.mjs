/** Shared practice evidence helpers. Title codecs normalize native state before these checks run. */
export function practiceBuildFeature(manifest,product){
  if(typeof manifest.features?.thprac!=='boolean')throw new TypeError('Current practice acceptance requires an explicit boolean package feature');
  const enabled=manifest.features.thprac;
  if(enabled&&product.features.thprac!==true)throw new Error('Package practice feature exceeds product support');
  return enabled;
}

/** Generic exact comparison over the title codec's normalized parameter view. */
export function assessPracticeParameters({snapshot,expectedConfig,expectedApplied,configFields,appliedFields,active=true}={}){
  const missing=[];
  const own=(value,key)=>value!==null&&typeof value==='object'&&Object.hasOwn(value,key);
  const scalar=value=>typeof value==='boolean'||(typeof value==='string'&&value.trim().length>0)||
    (typeof value==='number'&&Number.isFinite(value));
  if(!snapshot||snapshot.available===false)missing.push('native practice observation');
  if(!snapshot?.state||!snapshot.configured||!snapshot.run||!snapshot.applied)
    missing.push('configured, accepted and applied values');
  if(!Array.isArray(configFields)||!configFields.length||configFields.some(field=>typeof field!=='string'||!field.trim())||
    new Set(configFields).size!==configFields.length)missing.push('title-declared config fields');
  if(!Array.isArray(appliedFields)||!appliedFields.length||appliedFields.some(field=>typeof field!=='string'||!field.trim())||
    new Set(appliedFields).size!==appliedFields.length)missing.push('title-declared applied fields');
  if(expectedConfig==null||typeof expectedConfig!=='object'||Array.isArray(expectedConfig)||
    expectedApplied==null||typeof expectedApplied!=='object'||Array.isArray(expectedApplied))
    missing.push('independent parameter expectations');
  for(const field of Array.isArray(configFields)?configFields:[]){
    for(const block of ['configured','run']){
      if(!own(expectedConfig,field)||!scalar(expectedConfig[field])||!own(snapshot?.[block],field)||!scalar(snapshot[block][field]))
        missing.push(`${block}.${field}`);
    }
  }
  for(const field of Array.isArray(appliedFields)?appliedFields:[]){
    if(!own(expectedApplied,field)||!scalar(expectedApplied[field])||!own(snapshot?.applied,field)||!scalar(snapshot.applied[field]))
      missing.push(`applied.${field}`);
  }
  if(active&&(!own(snapshot?.state,'active')||typeof snapshot.state.active!=='boolean'))missing.push('state.active');
  if(active&&(!own(snapshot?.state,'inGame')||typeof snapshot.state.inGame!=='boolean'))missing.push('state.inGame');
  if(missing.length)return{status:'incomplete',complete:false,missing:[...new Set(missing)],mismatches:[]};
  const mismatches=[];
  for(const field of configFields){
    for(const block of ['configured','run'])if(snapshot[block][field]!==expectedConfig[field])
      mismatches.push({field:`${block}.${field}`,expected:expectedConfig[field],actual:snapshot[block][field]});
  }
  for(const field of appliedFields)if(snapshot.applied[field]!==expectedApplied[field])
    mismatches.push({field:`applied.${field}`,expected:expectedApplied[field],actual:snapshot.applied[field]});
  if(active&&snapshot.state.active!==true)mismatches.push({field:'state.active',expected:true,actual:snapshot.state.active});
  if(active&&snapshot.state.inGame!==true)mismatches.push({field:'state.inGame',expected:true,actual:snapshot.state.inGame});
  return{status:mismatches.length?'failed':'observed',complete:false,missing:[],mismatches};
}

/** Project a title descriptor onto the data-only ABI sent across Playwright's frame boundary. */
export function practiceSnapshotRequest({runtime,contract,kind='snapshot'}={}){
  if(!['snapshot','initial'].includes(kind))throw new TypeError('Practice snapshot kind must be snapshot or initial');
  const spec=kind==='initial'?contract?.initialSnapshot:contract?.snapshot;
  if(!runtime||['global','core','module','app'].some(key=>typeof runtime[key]!=='string'||!runtime[key].trim())||
    !spec||!Number.isSafeInteger(spec.wordCount)||spec.wordCount<=0||
    !['f64','u32','i32'].includes(spec.wordType)||typeof spec.export!=='string'||typeof spec.app!=='boolean')
    throw new TypeError('Practice snapshot descriptor cannot be projected to a native ABI request');
  return{runtime:{global:runtime.global,core:runtime.core,module:runtime.module,app:runtime.app},
    snapshot:{export:spec.export,wordCount:spec.wordCount,wordType:spec.wordType,app:spec.app}};
}

/** Serialized for frame.evaluate; accepts only the projected mounting names and ABI width. */
export function readPracticeSnapshotRaw({runtime,snapshot:spec}={}){
  const invalid=reason=>({available:false,reason});
  if(!runtime||!spec||!Number.isSafeInteger(spec.wordCount)||spec.wordCount<=0||
    !['f64','u32','i32'].includes(spec.wordType)||typeof spec.export!=='string'||typeof spec.app!=='boolean')
    return invalid('Practice snapshot contract is incomplete');
  const root=typeof window==='object'?window:globalThis;
  const mounted=root[runtime.global];
  const module=mounted?.[runtime.module];
  const core=mounted?.[runtime.core]??module;
  const app=mounted?.[runtime.app];
  if(!mounted||(spec.app&&!Number.isInteger(app)))return invalid('Runtime iframe identity unavailable');
  let snapshot;
  for(const owner of [core,module]){
    const candidate=owner?.[spec.export]??owner?.[`_${spec.export}`];
    if(typeof candidate==='function'){snapshot=candidate.bind(owner);break;}
  }
  const buffer=core?.memory?.buffer??module?.HEAPU8?.buffer;
  if(typeof snapshot!=='function'||!buffer)return invalid('Practice snapshot export or wasm memory unavailable');
  const ArrayType=spec.wordType==='f64'?Float64Array:spec.wordType==='u32'?Uint32Array:Int32Array;
  const pointer=spec.app?snapshot(app):snapshot(),bytes=spec.wordCount*ArrayType.BYTES_PER_ELEMENT;
  if(!Number.isInteger(pointer)||pointer<=0||pointer+bytes>buffer.byteLength)return invalid('Practice snapshot pointer is outside wasm memory');
  return{available:true,words:Array.from(new ArrayType(buffer,pointer,spec.wordCount))};
}

/** Read the shared practice-menu contract only from the title-declared Runtime mount. */
export function readPracticeMenuRaw({runtime}={}){
  const root=typeof window==='object'?window:globalThis;
  const mounted=runtime?.global?root[runtime.global]:null;
  const module=mounted&&runtime?.module?mounted[runtime.module]:null;
  if(!mounted||!module)return{available:false,reason:'Declared practice Runtime module is unavailable'};
  const options=module.eaglerOptions;
  return{available:true,open:module.eaglerThpracMenuOpen===true,options:options?{
    thpracEnabled:options.thpracEnabled,thpracLocale:options.thpracLocale}:null,
    events:root.__practiceBrowserEvents??[]};
}

/** Run the driver's explicit IDBFS save sync through the declared Runtime mount. */
export async function syncPracticeIdbfsRaw({runtime}={}){
  const root=typeof window==='object'?window:globalThis;
  const mounted=runtime?.global?root[runtime.global]:null;
  const module=mounted&&runtime?.module?mounted[runtime.module]:null;
  const fs=module?.FS;
  if(!mounted||!module)return{available:false,reason:'Declared practice Runtime module is unavailable'};
  if(typeof fs?.syncfs!=='function')return{available:false,reason:'Runtime IDBFS syncfs is unavailable'};
  return new Promise(resolve=>fs.syncfs(false,error=>resolve({available:true,succeeded:!error,error:error?String(error):null})));
}

/** Keep the exact practice Replay assessor input in JSON reports for independent re-evaluation. */
export function createPracticeReplayObservationInput({format,evidence}={}){
  if(typeof format!=='string'||!format.trim()||!evidence||typeof evidence!=='object'||Array.isArray(evidence))
    throw new TypeError('Practice Replay observation input needs a format and evidence object');
  const rawBytes=(value,label)=>{
    let bytes;
    if(value instanceof Uint8Array)bytes=value;
    else if(ArrayBuffer.isView(value))bytes=new Uint8Array(value.buffer,value.byteOffset,value.byteLength);
    else if(value instanceof ArrayBuffer)bytes=new Uint8Array(value);
    else if(Array.isArray(value)&&value.every(byte=>Number.isInteger(byte)&&byte>=0&&byte<=255))bytes=Uint8Array.from(value);
    else throw new TypeError(`Practice Replay observation input needs ${label} bytes`);
    return Array.from(bytes);
  };
  for(const key of ['recording','persisted','import'])
    if(!evidence[key]||typeof evidence[key]!=='object'||Array.isArray(evidence[key]))
      throw new TypeError(`Practice Replay observation input needs ${key} evidence`);
  return{schema:'eagler-touhou/practice-replay-input/1',format,evidence:{
    ...evidence,
    recording:{...evidence.recording,rawBytes:rawBytes(evidence.recording.rawBytes,'recording')},
    persisted:{...evidence.persisted,rawBytes:rawBytes(evidence.persisted.rawBytes,'persisted')},
    import:{...evidence.import,rawBytes:rawBytes(evidence.import.rawBytes,'import')},
  }};
}

export function practiceGameplayReady(state,gameplayFrame=state?.frames?.simulation??state?.frames?.enemy){
  return ['stage','match'].includes(state?.phase?.name)&&state?.pause?.active===false&&
    state?.localSeat===0&&Number.isSafeInteger(gameplayFrame)&&gameplayFrame>=0;
}

/** Bind a title menu operation to the native gameplay-ready snapshot it produced. */
export async function runPracticeGameplayStage({record,stage,epoch,perform,ready,assess}={}){
  if(!record||typeof stage!=='function'||!Number.isSafeInteger(epoch)||epoch<1||
    typeof perform!=='function'||typeof ready!=='function'||typeof assess!=='function')
    throw new TypeError('Practice gameplay stage requires its record, epoch, stage runner, operation, readiness and assessor');
  const result=await stage('native-practice-selection',{epoch},async()=>{
    const observed=await perform();
    const snapshot=observed?.snapshot;
    if(!snapshot||!ready(snapshot))throw new Error('Practice selection did not return a native gameplay-ready snapshot');
    const parameterConsumption=assess(observed.practice);
    record.parameterConsumption=parameterConsumption;
    if(parameterConsumption?.status!=='observed')
      throw new Error(`Native practice parameters did not match the independent title declaration: ${JSON.stringify(parameterConsumption)}`);
    return{...observed,snapshot,parameterConsumption};
  });
  if(!result?.snapshot||!ready(result.snapshot))throw new Error('Practice selection stage lost its native gameplay snapshot');
  record.nativePracticeMenu=result;
  record.gameplay=result.snapshot;
  return result;
}
