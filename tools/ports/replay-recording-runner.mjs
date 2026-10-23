import {createHash} from 'node:crypto';
import {mkdir, readFile, writeFile, access} from 'node:fs/promises';
import {join, resolve} from 'node:path';
import {
  assessReplayInputIsolation,
  assessReplayPlaybackWindow,
  assessReplayPointerConsumption,
  assessReplayRecordingInputWindow,
  createReplayRecordingScenes,
} from './replay-recording-scenes.mjs';

const HEX_SHA256=/^[a-f0-9]{64}$/;
const MAX_PLAYBACK_TICKS=600;
const MAX_BOOTSTRAP_TICKS=600;

class RunnerIssue extends Error {
  constructor(status, message, evidence={}) {
    super(message);
    this.status=status;
    this.evidence=evidence;
  }
}

function asBytes(input, label) {
  if(input instanceof Uint8Array)return Buffer.from(input.buffer,input.byteOffset,input.byteLength);
  if(input instanceof ArrayBuffer)return Buffer.from(input);
  throw new RunnerIssue('incomplete',`${label} did not return byte data.`);
}

function sha256(bytes) {
  return createHash('sha256').update(bytes).digest('hex');
}

function resultStatus(result, label) {
  if(result===true||result?.status==='passed')return;
  if(result?.status==='failed')throw new RunnerIssue('incomplete',`${label} did not establish a usable test boundary.`,{result});
  throw new RunnerIssue('incomplete',`${label} is unavailable or incomplete.`,{result:result??null});
}

function requireMethod(io, name) {
  if(typeof io?.[name]!=='function')throw new RunnerIssue('incomplete',`Replay runner IO is missing ${name}().`);
  return io[name].bind(io);
}

function routeMethod(route) {
  if(route==='keyboard')return'key';
  if(route==='virtual-gamepad')return'gamepad';
  if(route==='touch-controls')return'controls';
  return null;
}

function dispatchDescriptor(io, descriptor) {
  if(!descriptor||typeof descriptor!=='object')throw new RunnerIssue('incomplete','Replay input descriptor is missing.');
  const method=descriptor.kind==='keyboard-key'?'key':
    descriptor.kind==='browser-gamepad-bit'?'gamepad':
      descriptor.kind==='touch-fire-control'?'controls':null;
  if(!method)throw new RunnerIssue('incomplete',`Replay input kind ${String(descriptor.kind)} has no browser IO route.`);
  return requireMethod(io,method)(descriptor);
}

function playbackProbeDescriptor(probe, pressed) {
  if(probe?.operation&&typeof probe.operation==='object')return{...probe.operation,pressed};
  if(probe?.kind==='keyboard-key'&&typeof probe.code==='string')return{kind:probe.kind,code:probe.code,pressed};
  if(probe?.kind==='browser-gamepad-bit'&&Number.isSafeInteger(probe.bit))return{kind:probe.kind,bit:probe.bit,pressed};
  if(probe?.kind==='touch-fire-control')return{kind:probe.kind,enabled:pressed};
  return null;
}

function sceneRecord(scene) {
  return{id:scene.id,status:scene.status,kind:scene.kind,reason:scene.reason??null,
    result:null,samples:[],bootstrapSamples:[],bootstrapMarker:null,readiness:null,cleanup:[]};
}

function setResult(record, result) {
  record.result=result;
  record.status=result?.status??'incomplete';
  record.reason=result?.reason??null;
}

function mapIssue(record, error) {
  setResult(record,{status:error instanceof RunnerIssue?error.status:'incomplete',
    reason:error?.message??String(error),evidence:error?.evidence??null});
}

async function callOptionalStatus(io, name, args, label, record) {
  try {
    const method=requireMethod(io,name),result=await method(args);
    resultStatus(result,label);
    return result;
  } catch(error) {
    record?.cleanup.push({operation:name,status:'incomplete',reason:error?.message??String(error)});
    return null;
  }
}

async function clearInputs(io, record, {pointerRelease}={}) {
  if(pointerRelease&&typeof io?.directTouch==='function'){
    try {
      const result=await io.directTouch({...pointerRelease,action:'up',cleanup:true});
      if(result?.status&&result.status!=='passed')throw new Error(result.reason??'Pointer release was not acknowledged.');
      record?.cleanup.push({operation:'pointer-up',status:'passed'});
    } catch(error) {
      record?.cleanup.push({operation:'pointer-up',status:'incomplete',reason:error?.message??String(error)});
    }
  }
  try {
    const result=await requireMethod(io,'releaseAll')();
    if(result===false||result?.status==='failed'||result?.status==='incomplete')
      throw new Error(result?.reason??'Input release was not acknowledged.');
    record?.cleanup.push({operation:'release-all',status:'passed'});
  } catch(error) {
    record?.cleanup.push({operation:'release-all',status:'incomplete',reason:error?.message??String(error)});
  }
}

async function startTrace(io, record) {
  const control=requireMethod(io,'controlTrace');
  const disabled=await control({enabled:false,reset:true});
  resultStatus(disabled,'Resetting and disabling the Replay trace');
  const enabled=await control({enabled:true,reset:false});
  resultStatus(enabled,'Enabling the reset Replay trace');
  record.cleanup.push({operation:'trace-enable',status:'passed',reset:true});
}

async function stopTrace(io, record) {
  await callOptionalStatus(io,'controlTrace',{enabled:false,reset:false},'Disabling the Replay trace',record);
}

async function stepAndRead(io, cursor, record, bucket='samples', inputReceiptProbe=null) {
  const step=await requireMethod(io,'step')();
  if(step?.status&&step.status!=='passed')
    throw new RunnerIssue('incomplete','Manual logical step did not complete.',{step});
  const result=await requireMethod(io,'readTrace')(cursor===null?{}:{afterSequence:cursor});
  if(result?.gap===true||result?.status==='incomplete')
    throw new RunnerIssue('incomplete','Native Replay trace has a missing or rolled-off sample.',{trace:result});
  if(result?.status!=='passed'||!Array.isArray(result.samples)||result.samples.length!==1)
    throw new RunnerIssue('incomplete','One post-update native Replay trace sample was not available for this logical step.',{trace:result??null});
  const sample=result.samples[0];
  if(!Number.isSafeInteger(sample?.sequence)||cursor!==null&&sample.sequence!==cursor+1)
    throw new RunnerIssue('incomplete','Incremental native Replay trace sequence is not contiguous.',{afterSequence:cursor,sample});
  if(inputReceiptProbe?.ingress){
    try {
      sample.ingress=await requireMethod(io,'readInputReceipt')(inputReceiptProbe);
    } catch(error) {
      sample.ingress={status:'incomplete',receiptId:inputReceiptProbe.ingress.receipt,
        reason:error?.message??String(error)};
    }
    if(sample.ingress?.status!=='passed'||sample.ingress?.receiptId!==inputReceiptProbe.ingress.receipt){
      if(!Array.isArray(record.inputReceiptErrors))record.inputReceiptErrors=[];
      record.inputReceiptErrors.push({sequence:sample.sequence,receipt:sample.ingress});
    }
  }
  if(!Array.isArray(record[bucket]))record[bucket]=[];
  record[bucket].push(sample);
  return sample;
}

function playbackConsumerState(sample, stage, nativeTrace) {
  const values=nativeTrace?.values;
  if(!values||!Number.isSafeInteger(values.postUpdateBoundary)||!Number.isSafeInteger(values.consumerRunPhase)||
    !Number.isSafeInteger(values.consumedInputValidBit)||values.consumedInputValidBit<=0)
    throw new RunnerIssue('incomplete','The title trace contract does not declare playback consumer readiness values.');
  if(sample?.boundary!==values.postUpdateBoundary)
    throw new RunnerIssue('incomplete','A playback bootstrap sample was not captured at the declared post-update boundary.',{sample});
  if(sample?.stage!==stage)return false;
  const playback=sample.playback,consumer=sample.consumer;
  if(!playback||playback.loaded!==true||playback.stage!==stage||!consumer||consumer.stage!==stage||
    consumer.phase!==values.consumerRunPhase||!Number.isSafeInteger(consumer.validMask)||
    (consumer.validMask&values.consumedInputValidBit)===0)return false;
  if(![playback.frame,playback.cursor,playback.current,playback.previous,
    sample.pollSequence,sample.controlFrame,consumer.sequence,consumer.controlFrame,consumer.current,consumer.previous]
    .every(Number.isSafeInteger))
    throw new RunnerIssue('incomplete','The first playback consumer-ready sample is missing required native receipt fields.',{sample});
  if(playback.frame!==1)
    throw new RunnerIssue('incomplete','The first valid playback consumer receipt did not begin at stream frame 1.',
      {stage,playbackFrame:playback.frame,sample});
  return true;
}

async function bootstrapPlaybackConsumer(io, record, {stage,nativeTrace,inputReceiptProbe=null}={}) {
  let lastSequence=null;
  for(let tick=0;tick<MAX_BOOTSTRAP_TICKS;tick++){
    const sample=await stepAndRead(io,lastSequence,record,'bootstrapSamples',inputReceiptProbe);
    lastSequence=sample.sequence;
    if(playbackConsumerState(sample,stage,nativeTrace)){
      record.bootstrapMarker=sample;
      record.readiness={status:'passed',sampleSequence:sample.sequence,stage,
        playbackFrame:sample.playback.frame,consumerSequence:sample.consumer.sequence,
        controlFrame:sample.consumer.controlFrame,bootstrapTicks:tick+1};
      record.samples.push(sample);
      return lastSequence;
    }
  }
  throw new RunnerIssue('incomplete',`No stage ${stage} playback consumer receipt became ready within ${MAX_BOOTSTRAP_TICKS} bootstrap ticks.`,
    {bootstrapTicks:record.bootstrapSamples.length,lastSample:record.bootstrapSamples.at(-1)??null});
}

async function captureRecordingInput(io, scene, record) {
  let lastSequence=null;
  const method=routeMethod(scene.route);
  if(!method||typeof io?.[method]!=='function')throw new RunnerIssue('incomplete',`The ${scene.route} input adapter is unavailable.`);
  try {
    await startTrace(io,record);
    const release=await dispatchDescriptor(io,scene.release);
    if(release?.status&&release.status!=='passed')throw new RunnerIssue('incomplete','Initial route release was not acknowledged.',{release});
    const pressAt=scene.window.warmupTicks;
    const releaseAt=pressAt+scene.window.pressedTicks;
    for(let tick=0;tick<scene.window.totalTicks;tick++){
      if(tick===pressAt){
        const result=await dispatchDescriptor(io,scene.press);
        if(result?.status&&result.status!=='passed')throw new RunnerIssue('incomplete','Recording input press was not acknowledged.',{result});
      }else if(tick===releaseAt){
        const result=await dispatchDescriptor(io,scene.release);
        if(result?.status&&result.status!=='passed')throw new RunnerIssue('incomplete','Recording input release was not acknowledged.',{result});
      }
      const sample=await stepAndRead(io,lastSequence,record);
      lastSequence=sample.sequence;
    }
    record.captureStatus='passed';
  } finally {
    try {
      await dispatchDescriptor(io,scene.release);
      record.cleanup.push({operation:`release-${scene.route}`,status:'passed'});
    } catch(error) {
      record.cleanup.push({operation:`release-${scene.route}`,status:'incomplete',reason:error?.message??String(error)});
    }
    await clearInputs(io,record);
    await stopTrace(io,record);
  }
}

async function capturePointerRecording(io, scene, record) {
  let lastSequence=null;
  const pointerRelease=scene.trajectory.findLast(event=>event.action==='up');
  try {
    await startTrace(io,record);
    await stepAndRead(io,lastSequence,record);
    lastSequence=record.samples.at(-1).sequence;
    for(const event of scene.trajectory){
      const result=await requireMethod(io,'directTouch')(event);
      if(result?.status&&result.status!=='passed')throw new RunnerIssue('incomplete','Native pointer action was not acknowledged.',{event,result});
      const sample=await stepAndRead(io,lastSequence,record);
      lastSequence=sample.sequence;
    }
    record.captureStatus='passed';
  } finally {
    await clearInputs(io,record,{pointerRelease});
    await stopTrace(io,record);
  }
}

async function copyVerifiedReplay(io, path, expectedHash) {
  const bytes=asBytes(await requireMethod(io,'readFile')(path),'Production Replay readback');
  const actualHash=sha256(bytes);
  if(expectedHash&&actualHash!==expectedHash)
    throw new RunnerIssue('failed','The production Replay changed after finalization.',{expectedHash,actualHash});
  return{bytes:Buffer.from(bytes),sha256:actualHash};
}

async function capturePlayback(io, scene, record, {stage,bytes,replaySha256,opener,nativeTrace,
  windowTicks=scene.windowTicks,probe=null}={}) {
  let lastSequence=null;
  const inputReceiptProbe=scene?.probe?.ingress?scene.probe:null;
  try {
    const prepared=await requireMethod(io,'prepare')({phase:'playback',mode:'production-replay',stage,scene,label:scene.id,fresh:true});
    resultStatus(prepared,`Preparing fresh production playback ${scene.id}`);
    const opened=await requireMethod(io,'openPlayback')({opener,bytes:Buffer.from(bytes),sha256:replaySha256,
      stage,scene,label:scene.id});
    resultStatus(opened,`Opening production Replay for ${scene.id}`);
    if(!HEX_SHA256.test(opened?.sha256??''))
      throw new RunnerIssue('incomplete',`Production Replay opener did not report the loaded file identity for ${scene.id}.`,{opened});
    if(opened.sha256!==replaySha256)
      throw new RunnerIssue('failed',`Production Replay opener loaded different bytes for ${scene.id}.`,{expected:replaySha256,actual:opened.sha256});
    await startTrace(io,record);
    await clearInputs(io,record);
    lastSequence=await bootstrapPlaybackConsumer(io,record,{stage,nativeTrace,inputReceiptProbe});
    const press=probe?playbackProbeDescriptor(probe,true):null;
    const release=probe?(probe.release??playbackProbeDescriptor(probe,false)):null;
    if(probe&&(!press||!release))throw new RunnerIssue('incomplete',`Playback probe ${probe.name} has no browser input route.`);
    const pressAt=probe?Math.max(1,Math.floor(windowTicks/3)):Number.POSITIVE_INFINITY;
    const releaseAt=probe?pressAt+2:Number.POSITIVE_INFINITY;
    for(let tick=1;tick<windowTicks;tick++){
      if(tick===pressAt){
        const result=await dispatchDescriptor(io,press);
        if(result?.status&&result.status!=='passed')throw new RunnerIssue('incomplete','Live Replay perturbation was not acknowledged.',{result});
      }else if(tick===releaseAt){
        const result=await dispatchDescriptor(io,release);
        if(result?.status&&result.status!=='passed')throw new RunnerIssue('incomplete','Live Replay perturbation release was not acknowledged.',{result});
      }
      const sample=await stepAndRead(io,lastSequence,record,'samples',inputReceiptProbe);
      lastSequence=sample.sequence;
    }
    record.loadedReplaySha256=opened.sha256;
    record.captureStatus='passed';
  } finally {
    if(probe){
      try {
        const release=probe.release??playbackProbeDescriptor(probe,false);
        if(release)await dispatchDescriptor(io,release);
        record.cleanup.push({operation:'release-playback-probe',status:'passed'});
      }catch(error){record.cleanup.push({operation:'release-playback-probe',status:'incomplete',reason:error?.message??String(error)});}
    }
    await clearInputs(io,record);
    await stopTrace(io,record);
  }
}

async function capturePointerPlayback(io, scene, record, {stage,bytes,replaySha256,opener,nativeTrace,maxTicks}={}) {
  let lastSequence=null;
  try {
    const prepared=await requireMethod(io,'prepare')({phase:'playback',mode:'production-replay',stage,scene,
      label:scene.id,fresh:true});
    resultStatus(prepared,`Preparing fresh pointer playback ${scene.id}`);
    const opened=await requireMethod(io,'openPlayback')({opener,bytes:Buffer.from(bytes),sha256:replaySha256,
      stage,scene,label:scene.id});
    resultStatus(opened,`Opening production Replay for ${scene.id}`);
    if(!HEX_SHA256.test(opened?.sha256??''))
      throw new RunnerIssue('incomplete','Production opener did not report the pointer playback file identity.',{opened});
    if(opened.sha256!==replaySha256)
      throw new RunnerIssue('failed','Production pointer playback loaded different Replay bytes.',{expected:replaySha256,actual:opened.sha256});
    await clearInputs(io,record);
    await startTrace(io,record);
    lastSequence=await bootstrapPlaybackConsumer(io,record,{stage,nativeTrace});
    for(let tick=1;tick<maxTicks;tick++){
      const sample=await stepAndRead(io,lastSequence,record);
      lastSequence=sample.sequence;
    }
    record.loadedReplaySha256=opened.sha256;
    record.captureStatus='passed';
  } finally {
    await clearInputs(io,record);
    await stopTrace(io,record);
  }
}

function setIncomplete(record, reason, evidence=null) {
  if(record.status==='failed')return;
  record.status='incomplete';record.reason=reason;
  if(evidence!==null)record.evidence=evidence;
}

function setFailed(record, result) {
  record.status='failed';record.reason=result?.reason??'Replay scene behavior did not match its independent evidence.';
  record.result=result;
}

async function evaluateTraceCase(record, evaluator, args) {
  const result=evaluator(args);
  setResult(record,result);
  return result;
}

/**
 * Execute one production Replay recording group and bounded playback windows.
 * `io` owns real browser/runtime operations; this module owns ordering, trace
 * completeness, independent file identity and scene-result aggregation.
 */
export async function runReplayRecordingGroup({semantics,io,outputDir,stage=0,windowTicks=12}={}){
  const plan=createReplayRecordingScenes({semantics,stage,windowTicks});
  const report={schema:'eagler-touhou/replay-recording-run/1',title:plan.title,stage,windowTicks,
    status:'incomplete',completeReplay:false,planStatus:plan.status,requiredCases:[...plan.requiredCases],
    cases:plan.cases.map(sceneRecord),replay:null,artifacts:{},errors:[]};
  const records=new Map(report.cases.map(record=>[record.id,record]));
  const pointerScene=plan.cases.find(item=>item.kind==='recording-extension');
  const targetDir=typeof outputDir==='string'&&outputDir.trim()?resolve(outputDir):null;
  let recordingStarted=false,finalized=null,rawBytes=null,rawHash=null,rawFrames=null,extension=null;

  try {
    if(!targetDir)throw new RunnerIssue('incomplete','An outputDir is required to preserve the raw Replay and evidence report.');
    await mkdir(targetDir,{recursive:true});
    for(const name of ['replay-recording-finite.rpy','replay-recording-evidence.json']){
      try {await access(join(targetDir,name));throw new RunnerIssue('incomplete',`Refusing to overwrite existing Replay evidence ${name}.`);}
      catch(error){if(error instanceof RunnerIssue)throw error;}
    }
    if(plan.status!=='ready')throw new RunnerIssue('incomplete','One or more required title Replay scenes are blocked by their source contract.',{blocked:plan.cases.filter(item=>item.status!=='ready')});

    const prepared=await requireMethod(io,'prepare')({phase:'recording',mode:'normal-gameplay',stage,scene:null,label:'recording-group',fresh:true});
    resultStatus(prepared,'Preparing the normal production gameplay recording');
    recordingStarted=true;

    for(const scene of plan.cases.filter(item=>item.kind==='recording-input')){
      const record=records.get(scene.id);
      try {
        await captureRecordingInput(io,scene,record);
        record.captureStatus='passed';
      } catch(error) {
        mapIssue(record,error);
        report.errors.push({case:scene.id,reason:error?.message??String(error)});
      }
    }

    if(pointerScene){
      const record=records.get(pointerScene.id);
      try {
        await capturePointerRecording(io,pointerScene,record);
        record.captureStatus='passed';
      } catch(error) {
        mapIssue(record,error);
        report.errors.push({case:pointerScene.id,reason:error?.message??String(error)});
      }
    }
  } catch(error) {
    report.errors.push({phase:'recording-setup',reason:error?.message??String(error)});
    for(const record of report.cases)if(record.status==='ready')setIncomplete(record,error?.message??String(error),error?.evidence??null);
  } finally {
    await clearInputs(io,null);
    try {await requireMethod(io,'controlTrace')({enabled:false,reset:false});}
    catch(error){report.errors.push({phase:'trace-shutdown',reason:error?.message??String(error)});}
    if(recordingStarted){
      try {
        finalized=await requireMethod(io,'finalize')({finalizer:plan.finalizer,stage});
        if(finalized?.status==='failed')throw new RunnerIssue('failed','Production Replay finalization failed.',{finalized});
        if(finalized?.status!=='passed'||typeof finalized.path!=='string'||!finalized.path.trim())
          throw new RunnerIssue('incomplete','Production Replay finalizer did not return a successful file path.',{finalized});
        if(finalized.path!==plan.finalizer.path)
          throw new RunnerIssue('failed','Production Replay was finalized to a path outside its title declaration.',{expected:plan.finalizer.path,actual:finalized.path});
      }catch(error){
        report.errors.push({phase:'recording-finalize',reason:error?.message??String(error)});
        report.finalizeError={status:error?.status??'incomplete',reason:error?.message??String(error),evidence:error?.evidence??null};
      }
    }
  }

  if(finalized?.path){
    try {
      const file=await copyVerifiedReplay(io,finalized.path,finalized.sha256);
      rawBytes=file.bytes;rawHash=file.sha256;
      const rawPath=join(targetDir??resolve('.'),'replay-recording-finite.rpy');
      await writeFile(rawPath,rawBytes,{flag:'wx'});
      report.artifacts.replay=rawPath;
      report.replay={path:finalized.path,sha256:rawHash,bytes:rawBytes.length,
        titleReportedSha256:finalized.sha256??null,identityMatches:!finalized.sha256||finalized.sha256===rawHash};
      if(typeof io?.titleParser?.parseFrames!=='function'||typeof io?.titleParser?.parseExtension!=='function')
        throw new RunnerIssue('incomplete','Independent title Replay input and extension parsers were not supplied.');
      extension=io.titleParser.parseExtension(rawBytes,{expectedSha256:rawHash,expectedGame:Number(String(plan.title.game).replace(/^th0?/,''))||8});
      if(extension?.sha256!==rawHash||extension?.version!==pointerScene?.requiredVersion||!Array.isArray(extension?.touch))
        throw new RunnerIssue('failed','Saved Replay pointer extension does not match its source-declared version and identity.',{extension});
      if(extension.stage!==undefined&&extension.stage!==stage)
        throw new RunnerIssue('failed','Independent pointer extension parser returned the wrong stage.',{expected:stage,actual:extension.stage});
      extension={...extension,stage};
      const allRecorded=report.cases.filter(item=>item.kind==='recording-input').flatMap(item=>item.samples);
      const recordedFrame=Math.max(0,...allRecorded.map(sample=>sample.recording?.frame??0));
      const recordedPointerTick=Math.max(0,...(extension.touch.find(item=>item.stage===stage)?.entries??[]).map(item=>item.tick));
      const maxFrame=Math.max(recordedFrame,recordedPointerTick);
      if(maxFrame<1)throw new RunnerIssue('incomplete','No native recording frame was available for independent Replay parsing.');
      rawFrames=io.titleParser.parseFrames(rawBytes,{stage,expectedSha256:rawHash,firstFrame:0,frameCount:maxFrame+1});
      if(rawFrames?.sha256!==rawHash||!Array.isArray(rawFrames.frames)||!Number.isSafeInteger(rawFrames.stride))
        throw new RunnerIssue('incomplete','Independent Replay frame parser returned an incomplete identity or frame map.',{rawFrames});
      report.replay.parser={frames:{sha256:rawFrames.sha256,stride:rawFrames.stride,count:rawFrames.frames.length,
        firstFrame:rawFrames.frames[0]?.frame,lastFrame:rawFrames.frames.at(-1)?.frame},
        extension:{sha256:extension.sha256,version:extension.version,stage,
          touchEntries:extension.touch.find(item=>item.stage===stage)?.entries?.length??0}};
    } catch(error) {
      report.errors.push({phase:'independent-parse',reason:error?.message??String(error)});
      report.parseError={status:error?.status??'failed',reason:error?.message??String(error),evidence:error?.evidence??null};
      if(error?.status==='failed')for(const record of report.cases)
        if(record.status!=='blocked')setFailed(record,{status:'failed',reason:error?.message??String(error),evidence:error?.evidence??null});
    }
  }

  for(const scene of plan.cases.filter(item=>item.kind==='recording-input')){
    const record=records.get(scene.id);
    if(record.status==='failed'||record.status==='incomplete'&&record.captureStatus!=='passed')continue;
    if(!rawFrames||!rawHash){setIncomplete(record,report.parseError?.reason??'The saved Replay has no independent input parse.');continue;}
    const evaluated=await evaluateTraceCase(record,assessReplayRecordingInputWindow,
      {samples:record.samples,scene,rawFrames,expectedSha256:rawHash});
    if(evaluated.status==='failed')setFailed(record,evaluated);
  }
  if(pointerScene){
    const record=records.get(pointerScene.id);
    if(record.captureStatus!=='passed'){
      if(record.status==='ready')setIncomplete(record,'Pointer recording trace was not captured.');
    }else if(!extension||!rawHash)setIncomplete(record,report.parseError?.reason??'The saved Replay pointer extension was not parsed.');
    else if(record.status!=='failed')record.recordingExtension={status:'passed',scope:'pointer-events-saved-and-independently-parsed',
      completeReplay:false,entries:extension.touch.find(item=>item.stage===stage)?.entries?.length??0,replaySha256:rawHash};
  }

  if(rawBytes&&rawHash&&rawFrames&&extension&&finalized?.path){
    const playbackScenes=plan.cases.filter(item=>item.kind==='playback-baseline'||item.kind==='playback-live-perturbation');
    const recordedEndFrame=Math.max(0,...report.cases.filter(item=>item.kind==='recording-input')
      .flatMap(item=>item.samples.map(sample=>sample.recording?.frame??0)));
    const pointerEntries=extension.touch.find(item=>item.stage===stage)?.entries??[];
    const maxTick=pointerEntries.reduce((maximum,item)=>Math.max(maximum,item.tick),0);
    const playbackTicks=Math.max(windowTicks,recordedEndFrame+1,maxTick+1);
    const baselineScene=playbackScenes.find(item=>item.kind==='playback-baseline');
    if(baselineScene&&playbackTicks<=MAX_PLAYBACK_TICKS){
      const record=records.get(baselineScene.id);
      try {
        const current=await copyVerifiedReplay(io,finalized.path,rawHash);
        await capturePlayback(io,baselineScene,record,{stage,bytes:current.bytes,replaySha256:rawHash,opener:plan.opener,
          nativeTrace:plan.nativeTrace,windowTicks:playbackTicks});
        const playbackResult=assessReplayPlaybackWindow({samples:record.samples,rawFrames,stage,
          rawIndexOffset:baselineScene.rawIndexOffset,boundary:baselineScene.boundary,expectedSha256:rawHash});
        setResult(record,playbackResult);
      }catch(error){mapIssue(record,error);report.errors.push({case:baselineScene.id,reason:error?.message??String(error)});}
    }else if(baselineScene){
      setIncomplete(records.get(baselineScene.id),'The finite playback window required to cover recorded input exceeds its tick bound.',
        {playbackTicks,limit:MAX_PLAYBACK_TICKS});
    }

    const pointerPlaybackScene={...pointerScene,id:'playback-pointer-consumption',kind:'recording-extension',expectedPointerCount:extension.touch.find(item=>item.stage===stage)?.entries?.length??0};
    const pointerRecord=records.get(pointerScene.id);
    if(!pointerEntries.length||playbackTicks>MAX_PLAYBACK_TICKS){
      if(pointerRecord.status!=='failed')setIncomplete(pointerRecord,'Pointer extension playback window is absent or exceeds the finite tick bound.',{maxTick,recordedEndFrame,playbackTicks,limit:MAX_PLAYBACK_TICKS});
    }else{
      const playbackRecord={...sceneRecord(pointerPlaybackScene),id:pointerScene.id};
      try {
        const current=await copyVerifiedReplay(io,finalized.path,rawHash);
        await capturePointerPlayback(io,pointerPlaybackScene,playbackRecord,{stage,bytes:current.bytes,
          replaySha256:rawHash,opener:plan.opener,nativeTrace:plan.nativeTrace,maxTicks:playbackTicks});
        const consumed=assessReplayPointerConsumption({samples:playbackRecord.samples,extension,scene:pointerScene,expectedSha256:rawHash});
        pointerRecord.playbackExtension=consumed;
        if(consumed.status==='failed')setFailed(pointerRecord,consumed);
        else if(consumed.status==='passed'&&pointerRecord.recordingExtension?.status==='passed'){
          pointerRecord.status='passed';pointerRecord.reason=null;
          pointerRecord.result={status:'passed',scope:'finite-recorded-and-played-pointer-extension',completeReplay:false,
            recording:pointerRecord.recordingExtension,playback:consumed,replaySha256:rawHash};
        }else setIncomplete(pointerRecord,consumed.reason??'Native pointer extension playback was not fully observed.',consumed);
      }catch(error){mapIssue(pointerRecord,error);report.errors.push({case:pointerRecord.id,reason:error?.message??String(error)});}
      pointerRecord.recordingSamples=pointerRecord.samples;
      pointerRecord.bootstrapSamples=playbackRecord.bootstrapSamples;
      pointerRecord.bootstrapMarker=playbackRecord.bootstrapMarker;
      pointerRecord.readiness=playbackRecord.readiness;
      pointerRecord.samples=playbackRecord.samples;
      pointerRecord.cleanup.push(...playbackRecord.cleanup);
    }

    if(playbackTicks>MAX_PLAYBACK_TICKS){
      for(const scene of playbackScenes)if(scene.kind==='playback-live-perturbation')
        setIncomplete(records.get(scene.id),'The finite playback window required to cover recorded input exceeds its tick bound.',{playbackTicks,limit:MAX_PLAYBACK_TICKS});
    }
    for(const scene of playbackScenes.filter(item=>item.kind==='playback-live-perturbation')){
      const record=records.get(scene.id);
      if(playbackTicks>MAX_PLAYBACK_TICKS)continue;
      const baseline={...sceneRecord(scene),id:`${scene.id}:baseline`};
      const perturbed={...sceneRecord(scene),id:`${scene.id}:perturbed`};
      try {
        const baseFile=await copyVerifiedReplay(io,finalized.path,rawHash);
        await capturePlayback(io,scene,baseline,{stage,bytes:baseFile.bytes,replaySha256:rawHash,opener:plan.opener,
          nativeTrace:plan.nativeTrace,windowTicks:playbackTicks});
        const perturbedFile=await copyVerifiedReplay(io,finalized.path,rawHash);
        await capturePlayback(io,scene,perturbed,{stage,bytes:perturbedFile.bytes,replaySha256:rawHash,opener:plan.opener,
          nativeTrace:plan.nativeTrace,windowTicks:playbackTicks,probe:scene.probe});
        const isolation=assessReplayInputIsolation({baseline:baseline.samples,perturbed:perturbed.samples,scene,rawFrames,
          expectedSha256:rawHash,baselineSha256:baseline.loadedReplaySha256,perturbedSha256:perturbed.loadedReplaySha256});
        setResult(record,isolation);
        record.baseline={bootstrapSamples:baseline.bootstrapSamples,samples:baseline.samples,readiness:baseline.readiness,
          loadedReplaySha256:baseline.loadedReplaySha256};
        record.perturbed={bootstrapSamples:perturbed.bootstrapSamples,samples:perturbed.samples,readiness:perturbed.readiness,
          loadedReplaySha256:perturbed.loadedReplaySha256};
        record.cleanup.push(...baseline.cleanup,...perturbed.cleanup);
      }catch(error){
        mapIssue(record,error);
        record.baseline={bootstrapSamples:baseline.bootstrapSamples,samples:baseline.samples,readiness:baseline.readiness,cleanup:baseline.cleanup};
        record.perturbed={bootstrapSamples:perturbed.bootstrapSamples,samples:perturbed.samples,readiness:perturbed.readiness,cleanup:perturbed.cleanup};
        report.errors.push({case:scene.id,reason:error?.message??String(error)});
      }
    }
  }

  for(const scene of plan.cases){
    const record=records.get(scene.id);
    if(scene.status!=='ready'&&record.status==='blocked')continue;
    if(record.status==='ready')setIncomplete(record,'Scene did not reach its evaluator.');
    if(record.cleanup.some(item=>item.status==='incomplete')&&record.status==='passed')
      setIncomplete(record,'Input or trace cleanup was not fully acknowledged.',record.cleanup.filter(item=>item.status==='incomplete'));
  }
  report.counts=Object.fromEntries(['passed','failed','incomplete','blocked'].map(status=>[status,report.cases.filter(item=>item.status===status).length]));
  report.status=report.cases.some(item=>item.status==='failed')?'failed':
    report.cases.every(item=>item.status==='passed')?'passed':'incomplete';
  report.completedAt=new Date().toISOString();
  if(targetDir){
    const reportPath=join(targetDir,'replay-recording-evidence.json');
    report.artifacts.report=reportPath;
    try {
      await writeFile(reportPath,`${JSON.stringify(report,null,2)}\n`,{flag:'wx'});
    } catch(error) {
      delete report.artifacts.report;
      report.errors.push({phase:'report-write',reason:error?.message??String(error)});
      report.status=report.status==='failed'?'failed':'incomplete';
      report.reportWriteError=error?.message??String(error);
    }
  }
  return report;
}
