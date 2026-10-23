/** Persist each browser startup transition before and after it runs. */
export const requiredDesktopStartupStages=Object.freeze([
  'host-navigation','host-focus-before-ready','ready','keyboard-bridge','foreground','configure','launch-foreground','launch','first-frame',
]);

export const requiredDesktopReadinessEvidence=Object.freeze({
  id:'native-gameplay-preparation',
  source:'gameplayReadinessByEpoch',
  scope:'every-passed-first-frame-epoch',
  readyValue:true,
});

export const requiredAudioTitleReadinessEvidence=Object.freeze({
  id:'native-audio-title-preparation',
  source:'audioTitleReadinessByEpoch',
  scope:'every-passed-first-frame-epoch',
  readyValue:true,
});

/** Store startup readiness separately from same-epoch transition observations. */
export function recordDesktopGameplayReadiness(record,{purpose,epoch,label,readiness,samples,durationMs,state}={}){
  const collection=purpose==='startup'?'gameplayReadinessByEpoch':purpose==='transition'?'gameplayReadinessTransitions':null;
  if(!collection)throw new TypeError('Gameplay readiness needs an explicit startup or transition purpose');
  if(!record||typeof record!=='object'||!Number.isSafeInteger(epoch)||epoch<1||typeof label!=='string'||!label.trim())
    throw new TypeError('Gameplay readiness needs a report, positive epoch and label');
  if(typeof readiness?.ready!=='boolean'||!Array.isArray(readiness.reasons??[])||
    !Number.isSafeInteger(samples)||samples<1||!Number.isFinite(durationMs)||durationMs<0)
    throw new TypeError('Gameplay readiness needs a boolean result, reasons, sample count and elapsed time');
  const receipt={purpose,epoch,label,ready:readiness.ready,reasons:[...(readiness.reasons??[])],samples,durationMs,state};
  record[collection]??=[];
  record[collection].push(receipt);
  return receipt;
}

/** The initial desktop environment is unavailable before any Runtime launch. */
export class DesktopStartupPreconditionError extends Error {
  constructor(message){super(message);this.name='DesktopStartupPreconditionError';}
}

/** Serializable read of the Host ledger, including a startup that rejected or timed out. */
export function readDesktopHostEvents(){
  const names=['events','lifecycleEvents','hostKeyboardLifecycle','keyboardForwardEvents','keyboardMapEvents'];
  const result={};
  for(const name of names){
    if(!Array.isArray(globalThis[name]))throw new Error(`Desktop Host ledger is unavailable: ${name}`);
    result[name]=globalThis[name];
  }
  return result;
}

export function createStageRecorder(record, save, {now=()=>new Date(), monotonic=()=>performance.now()}={}) {
  record.stages ??= [];
  return async (stage, details, operation) => {
    const entry={stage,...details,status:'running',startedAt:now().toISOString()};
    record.stages.push(entry);
    await save();
    const started=monotonic();
    try {
      const result=await operation();
      entry.status='passed';
      entry.finishedAt=now().toISOString();
      entry.durationMs=Math.round(monotonic()-started);
      await save();
      return result;
    } catch(error) {
      entry.status=error instanceof DesktopStartupPreconditionError?'incomplete':'failed';
      entry.finishedAt=now().toISOString();
      entry.durationMs=Math.round(monotonic()-started);
      entry.error=String(error.stack||error);
      await save();
      throw error;
    }
  };
}

export function summarizeDesktopStartupEvidence({stages=[],gameplayReadinessByEpoch=[],
  readinessByEpoch=gameplayReadinessByEpoch,readinessEvidence=requiredDesktopReadinessEvidence,
  requiredStages=requiredDesktopStartupStages}={}) {
  if(!readinessEvidence?.id||!readinessEvidence.source||readinessEvidence.scope!=='every-passed-first-frame-epoch'||
    readinessEvidence.readyValue!==true)throw new TypeError('Native readiness requires an explicit source and one ready observation per first-frame epoch');
  const initialStageResults=requiredStages.map(stage=>{
    const matching=(stages??[]).filter(item=>item?.stage===stage&&item.epoch===1);
    const status=matching.some(item=>item.status==='failed')?'failed':matching.some(item=>item.status==='passed')?'passed':
      matching.some(item=>item.status==='incomplete')?'incomplete':'missing';
    return{stage,status};
  });
  const stageIndexes=requiredStages.map(stage=>(stages??[]).findIndex(item=>item?.stage===stage&&item.epoch===1&&item.status==='passed'));
  const stageOrderReady=stageIndexes.every((index,position)=>index>=0&&(position===0||index>stageIndexes[position-1]));
  const allRequiredStagesPassed=initialStageResults.every(item=>item.status==='passed');
  const firstFrameEpochs=[...new Set((stages??[]).filter(item=>item?.stage==='first-frame'&&item.status==='passed')
    .map(item=>item.epoch).filter(Number.isSafeInteger))].sort((a,b)=>a-b);
  const readinessResults=firstFrameEpochs.map(epoch=>{
    const matching=(readinessByEpoch??[]).filter(item=>item?.epoch===epoch);
    const status=matching.length===0?'missing':matching.length!==1?'ambiguous':matching[0].ready===true?'ready':'not-ready';
    return{epoch,status,reasons:matching.length===1?matching[0].reasons??[]:[]};
  });
  const failedStages=initialStageResults.filter(item=>item.status==='failed').map(item=>item.stage);
  const missingStages=initialStageResults.filter(item=>item.status==='missing').map(item=>item.stage);
  const incompleteStages=initialStageResults.filter(item=>item.status==='incomplete').map(item=>item.stage);
  const incompleteReadiness=readinessResults.filter(item=>item.status!=='ready');
  const invalidStageOrder=allRequiredStagesPassed&&!stageOrderReady;
  const status=failedStages.length||invalidStageOrder?'failed':missingStages.length||incompleteStages.length||!firstFrameEpochs.includes(1)||incompleteReadiness.length?'incomplete':'passed';
  return{
    status,
    requiredStages:[...requiredStages],
    initialStageResults,
    readinessEvidence:{...readinessEvidence,epochs:readinessResults},
    failedStages,
    missingStages,
    incompleteStages,
    stageOrder:{ready:stageOrderReady,stages:requiredStages.map((stage,index)=>({stage,index:stageIndexes[index]})),invalid:invalidStageOrder},
    incompleteReadiness:incompleteReadiness.map(item=>item.epoch),
  };
}

/** Preserve the exact startup stage or first-frame readiness reason in the target report. */
export function describeDesktopStartupEvidenceIssue(summary){
  if(summary?.status!=='incomplete')return null;
  const details=[];
  for(const item of summary.initialStageResults??[])
    if(item?.status!=='passed')details.push(`startup stage ${item?.stage??'unknown'}: ${item?.status??'missing'}`);
  for(const item of summary.readinessEvidence?.epochs??[]){
    if(item?.status==='ready')continue;
    const reasons=Array.isArray(item?.reasons)?item.reasons.filter(value=>typeof value==='string'&&value.trim()):[];
    details.push(`epoch ${item?.epoch??'unknown'} gameplay readiness ${item?.status??'missing'}${reasons.length?`: ${reasons.join('; ')}`:''}`);
  }
  return`Desktop startup evidence is incomplete: ${details.length?details.join('; '):'required startup evidence is unavailable'}`;
}

/** Add startup-summary diagnostics to the target-level incomplete explanation. */
export function recordDesktopStartupEvidenceIssue(record,summary){
  if(!record||typeof record!=='object')throw new TypeError('Startup evidence issue needs a report record');
  const issue=describeDesktopStartupEvidenceIssue(summary);
  if(issue)record.incompleteReason=[record.incompleteReason,issue].filter(Boolean).join('; ');
  return issue;
}
