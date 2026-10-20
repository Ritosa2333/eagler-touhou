/** Persist each browser startup transition before and after it runs. */
export const requiredDesktopStartupStages=Object.freeze([
  'host-navigation','ready','keyboard-bridge','configure','launch','first-frame',
]);

export const requiredDesktopReadinessEvidence=Object.freeze({
  id:'native-gameplay-preparation',
  source:'gameplayReadinessByEpoch',
  scope:'every-passed-first-frame-epoch',
  readyValue:true,
});

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
      entry.status='failed';
      entry.finishedAt=now().toISOString();
      entry.durationMs=Math.round(monotonic()-started);
      entry.error=String(error.stack||error);
      await save();
      throw error;
    }
  };
}

export function summarizeDesktopStartupEvidence({stages=[],gameplayReadinessByEpoch=[],requiredStages=requiredDesktopStartupStages}={}) {
  const initialStageResults=requiredStages.map(stage=>{
    const matching=(stages??[]).filter(item=>item?.stage===stage&&item.epoch===1);
    const status=matching.some(item=>item.status==='failed')?'failed':matching.some(item=>item.status==='passed')?'passed':'missing';
    return{stage,status};
  });
  const firstFrameEpochs=[...new Set((stages??[]).filter(item=>item?.stage==='first-frame'&&item.status==='passed')
    .map(item=>item.epoch).filter(Number.isSafeInteger))].sort((a,b)=>a-b);
  const readinessResults=firstFrameEpochs.map(epoch=>{
    const matching=(gameplayReadinessByEpoch??[]).filter(item=>item?.epoch===epoch);
    const status=matching.length===0?'missing':matching.length!==1?'ambiguous':matching[0].ready===true?'ready':'not-ready';
    return{epoch,status,reasons:matching.length===1?matching[0].reasons??[]:[]};
  });
  const failedStages=initialStageResults.filter(item=>item.status==='failed').map(item=>item.stage);
  const missingStages=initialStageResults.filter(item=>item.status==='missing').map(item=>item.stage);
  const incompleteReadiness=readinessResults.filter(item=>item.status!=='ready');
  const status=failedStages.length?'failed':missingStages.length||!firstFrameEpochs.includes(1)||incompleteReadiness.length?'incomplete':'passed';
  return{
    status,
    requiredStages:[...requiredStages],
    initialStageResults,
    readinessEvidence:{...requiredDesktopReadinessEvidence,epochs:readinessResults},
    failedStages,
    missingStages,
    incompleteReadiness:incompleteReadiness.map(item=>item.epoch),
  };
}
