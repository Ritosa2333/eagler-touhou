/** Enable/reset a title's declared native Replay trace from a serializable page request. */
export function controlReplayTrace({runtime,contract,enabled,reset}={}){
  const result=(status,reason,extra={})=>({schema:'eagler-touhou/replay-native-trace-control/1',status,...(reason?{reason}:{}),...extra});
  if(!Number.isSafeInteger(enabled)||![0,1].includes(enabled)||!Number.isSafeInteger(reset)||![0,1].includes(reset))
    return result('incomplete','Replay trace controls require explicit integer enabled/reset values of 0 or 1.');
  if(!runtime||typeof runtime.global!=='string'||typeof runtime.core!=='string'||typeof runtime.module!=='string'||
    !contract||typeof contract.controlExport!=='string'||!contract.controlExport.trim())
    return result('incomplete','The selected adapter does not declare a Replay trace control export.');
  const root=typeof window==='object'?window:globalThis;
  const mounted=root[runtime.global];
  const module=mounted?.[runtime.module];
  const core=mounted?.[runtime.core]??module;
  let control;
  for(const owner of [core,module]){
    if(!owner)continue;
    const candidate=owner[contract.controlExport]??owner[`_${contract.controlExport}`];
    if(typeof candidate==='function'){control=candidate.bind(owner);break;}
  }
  if(!control)return result('incomplete',`Native Replay trace control ${contract.controlExport} is unavailable.`);
  const accepted=control(enabled,reset);
  if(accepted!==1)return result('failed','The native Runtime rejected the Replay trace control request.',{enabled,reset,nativeResult:accepted});
  return result('passed',null,{enabled,reset,nativeResult:accepted});
}

/** Read one adapter-declared native input receipt without modifying Runtime state. */
export function readReplayInputReceipt({runtime,contract,receiptId}={}){
  const result=(status,reason,extra={})=>({schema:'eagler-touhou/replay-native-input-receipt/1',status,...(reason?{reason}:{}),...extra});
  if(!runtime||typeof runtime.global!=='string'||typeof runtime.core!=='string'||typeof runtime.module!=='string'||
    !contract||!contract.inputReceipts||typeof receiptId!=='string'||!receiptId.trim())
    return result('incomplete','The adapter has no declared native input-receipt interface.');
  const declaration=contract.inputReceipts[receiptId];
  if(!declaration||typeof declaration.export!=='string'||!declaration.export.trim()||
    !Number.isSafeInteger(declaration.wordCount)||declaration.wordCount<1||!declaration.fields||
    typeof declaration.source!=='string'||!declaration.source.trim())
    return result('incomplete',`Native input receipt ${receiptId} is not fully declared.`,{receiptId});
  const entries=Object.entries(declaration.fields);
  if(entries.length===0||entries.some(([,field])=>!Number.isSafeInteger(field?.index)||field.index<0||
    field.index>=declaration.wordCount||!['i32','bool01'].includes(field.type))||
    new Set(entries.map(([,field])=>field.index)).size!==entries.length)
    return result('incomplete',`Native input receipt ${receiptId} has an invalid field map.`,{receiptId});
  const root=typeof window==='object'?window:globalThis;
  const mounted=root[runtime.global];
  const module=mounted?.[runtime.module];
  const core=mounted?.[runtime.core]??module;
  let read;
  for(const owner of [core,module]){
    if(!owner)continue;
    const candidate=owner[declaration.export]??owner[`_${declaration.export}`];
    if(typeof candidate==='function'){read=candidate.bind(owner);break;}
  }
  if(!read)return result('incomplete',`Native input receipt export ${declaration.export} is unavailable.`,{receiptId});
  let pointer;
  try{pointer=read();}catch(error){
    return result('incomplete',`Native input receipt export ${declaration.export} could not be read.`,
      {receiptId,detail:error?.message??String(error)});
  }
  if(!Number.isSafeInteger(pointer))return result('incomplete','The native input-receipt export returned no memory address.',{receiptId});
  const address=pointer>>>0;
  const memory=core?.memory?.buffer?core.memory:module?.memory?.buffer?module.memory:null;
  const buffer=memory?.buffer??core?.HEAP32?.buffer??module?.HEAP32?.buffer;
  if(address===0||!buffer||!Number.isSafeInteger(buffer.byteLength)||address%4!==0||address+declaration.wordCount*4>buffer.byteLength)
    return result('incomplete','The native input-receipt memory range is unavailable or out of bounds.',{receiptId,address});
  const view=new DataView(buffer),fields={};
  for(const [name,field] of entries){
    const value=view.getInt32(address+field.index*4,true);
    if(field.type==='bool01'&&value!==0&&value!==1)
      return result('incomplete',`Native input-receipt boolean ${name} is not encoded as 0 or 1.`,
        {receiptId,fields:{...fields,[name]:value},source:declaration.source});
    fields[name]=value;
  }
  return result('passed',null,{receiptId,fields,wordCount:declaration.wordCount,source:declaration.source});
}

/** Read new entries from a title-mapped fixed-capacity native Replay trace. */
export function readReplayTrace({runtime,contract,afterSequence=0}={}){
  const result=(status,reason,extra={})=>({schema:'eagler-touhou/replay-native-trace-read/1',status,...(reason?{reason}:{}),...extra});
  if(!Number.isSafeInteger(afterSequence)||afterSequence<0)
    return result('incomplete','afterSequence must be a nonnegative safe integer.',{samples:[]});
  if(!runtime||typeof runtime.global!=='string'||typeof runtime.core!=='string'||typeof runtime.module!=='string'||
    !contract||!Number.isSafeInteger(contract.schema)||!Number.isSafeInteger(contract.wordCount)||contract.wordCount<1||
    !contract.statusFields||!contract.sample||typeof contract.readExport!=='string'||!contract.readExport.trim())
    return result('incomplete','The selected adapter does not declare a complete Replay trace read contract.',{samples:[]});
  const statusNames=['schema','nextSequence','firstAvailable','count','capacity'];
  if(statusNames.some(key=>!Number.isSafeInteger(contract.statusFields[key])))
    return result('incomplete','Replay trace metadata field indexes are incomplete.',{samples:[]});
  const statusIndexes=Object.values(contract.statusFields),sampleIndexes=Object.values(contract.sample);
  if(statusIndexes.some(value=>!Number.isSafeInteger(value)||value<0)||new Set(statusIndexes).size!==statusIndexes.length||
    sampleIndexes.some(value=>!Number.isSafeInteger(value)||value<0||value>=contract.wordCount)||
    new Set(sampleIndexes).size!==sampleIndexes.length||
    !Number.isSafeInteger(contract.values?.postUpdateBoundary))
    return result('incomplete','Replay trace field map is invalid or aliases two native words.',{samples:[]});
  const root=typeof window==='object'?window:globalThis;
  const mounted=root[runtime.global];
  const module=mounted?.[runtime.module];
  const core=mounted?.[runtime.core]??module;
  let read;
  for(const owner of [core,module]){
    if(!owner)continue;
    const candidate=owner[contract.readExport]??owner[`_${contract.readExport}`];
    if(typeof candidate==='function'){read=candidate.bind(owner);break;}
  }
  if(!read)return result('incomplete',`Native Replay trace reader ${contract.readExport} is unavailable.`,{samples:[]});
  const meta={};
  for(const key of statusNames){
    const value=read(contract.statusFields[key]);
    if(!Number.isSafeInteger(value)||value<0)
      return result('incomplete',`Native Replay trace metadata ${key} is unavailable.`,{samples:[],metadata:meta});
    meta[key]=value;
  }
  meta.samplesStart=contract.statusFields.samplesStart;
  if(meta.schema!==contract.schema)
    return result('incomplete',`Native Replay trace schema ${meta.schema} does not match declared schema ${contract.schema}.`,{samples:[],metadata:meta});
  if(meta.capacity<1||meta.count>meta.capacity||meta.firstAvailable!==meta.nextSequence-meta.count||meta.samplesStart<0)
    return result('incomplete','Native Replay trace ring metadata is internally inconsistent.',{samples:[],metadata:meta});
  if(afterSequence>=meta.nextSequence)
    return result('incomplete','afterSequence is ahead of the native Replay trace.',{samples:[],metadata:meta});
  const gap=afterSequence<meta.firstAvailable-1;
  const firstSequence=Math.max(meta.firstAvailable,afterSequence+1),samples=[];
  for(let sequence=firstSequence;sequence<meta.nextSequence;sequence++){
    const ringIndex=sequence-meta.firstAvailable,words=[];
    for(let word=0;word<contract.wordCount;word++){
      const raw=read(meta.samplesStart+ringIndex*contract.wordCount+word);
      if(!Number.isSafeInteger(raw))return result('incomplete',`Native Replay trace word ${word} is unavailable for sequence ${sequence}.`,
        {samples,metadata:meta,gap});
      words.push(raw>>>0);
    }
    const field=(name)=>Number.isSafeInteger(contract.sample[name])?words[contract.sample[name]]:null;
    const boolean=(name)=>{
      const value=field(name);return value===0?false:value===1?true:null;
    };
    const required=['sequence','boundary','stage'];
    if(required.some(name=>field(name)===null))
      return result('incomplete',`Replay trace sample mapping is missing a required field for sequence ${sequence}.`,{samples,metadata:meta,gap});
    const recordingActive=boolean('recordingActive'),playbackLoaded=boolean('playbackLoaded'),motionPlaying=boolean('motionPlaying');
    if([recordingActive,playbackLoaded,motionPlaying].some(value=>value===null))
      return result('incomplete',`Replay trace boolean fields are invalid for sequence ${sequence}.`,{samples,metadata:meta,gap});
    const pointCount=field('pointCount'),pointIdRaw=Number.isSafeInteger(contract.sample.pointId)?words[contract.sample.pointId]:null;
    const pointXBits=field('pointXFloat32Bits'),pointYBits=field('pointYFloat32Bits');
    let point=null;
    if(pointCount===1&&pointIdRaw!==null&&pointXBits!==null&&pointYBits!==null){
      const floatBytes=new ArrayBuffer(4),floatView=new DataView(floatBytes);
      floatView.setUint32(0,pointXBits,true);const x=floatView.getFloat32(0,true);
      floatView.setUint32(0,pointYBits,true);const y=floatView.getFloat32(0,true);
      const released=boolean('pointReleased');
      if(!Number.isFinite(x)||!Number.isFinite(y)||released===null)
        return result('incomplete',`Replay trace pointer point is malformed for sequence ${sequence}.`,{samples,metadata:meta,gap});
      point={id:pointIdRaw|0,x,y,released};
    }
    const sample={sequence:field('sequence'),boundary:field('boundary'),stage:field('stage'),
      ...(field('pollSequence')!==null?{pollSequence:field('pollSequence')}:{}),
      ...(field('liveInput')!==null?{liveInput:field('liveInput')}:{}),
      controlFrame:field('consumerControlFrame'),
      recording:recordingActive?{active:true,stage:field('recordingStage'),frame:field('recordingFrame'),
        cursor:field('recordingInputCursor'),physical:field('recordingPhysicalInput')}:{active:false,stage:field('recordingStage')},
      playback:playbackLoaded?{loaded:true,stage:field('playbackStage'),frame:field('playbackFrame'),
        cursor:field('playbackInputCursor'),current:field('playbackCurrent'),previous:field('playbackPrevious')}:{loaded:false,stage:field('playbackStage')},
      motion:motionPlaying?{playing:true,stage:field('motionStage'),tick:field('motionTick'),
        movementCursor:field('movementCursor'),movementCount:field('movementCount'),touchCursor:field('touchCursor'),
        touchCount:field('touchCount'),pointCount,point}:{playing:false,stage:field('motionStage'),
        touchCursor:field('touchCursor'),touchCount:field('touchCount'),pointCount,point},
      consumer:{phase:field('consumerPhase'),validMask:field('consumerValidMask'),sequence:field('consumerSequence'),
        controlFrame:field('consumerControlFrame'),stage:field('consumerStage'),current:field('consumerCurrent'),previous:field('consumerPrevious')},
      rawWords:words};
    if(sample.sequence!==sequence)
      return result('incomplete',`Native Replay trace ring sequence ${sequence} contains sample ${sample.sequence}.`,{samples,metadata:meta,gap});
    samples.push(sample);
  }
  return result(gap?'incomplete':'passed',gap?'Replay trace samples before firstAvailable were overwritten.':null,
    {metadata:meta,firstAvailable:meta.firstAvailable,nextSequence:meta.nextSequence,count:meta.count,
      capacity:meta.capacity,gap,samples});
}
