import {runWithReport} from '../testing/cli-report.mjs';

/** Bind one structured observation to every adaptation claim that consumes it. */
export function offlineObservationEnvelope(game,variant,adaptationIds,observation){
  return{game,variant,adaptationIds:[...adaptationIds],observation};
}

/** Keep execution of this offline lane separate from its adaptation coverage. */
export function offlineRuntimeOutcome({scenarioPassed,checks=[],updateScenario=null,scenarioId='offline-import-reopen'}={}) {
  const failed=scenarioPassed!==true||checks.some(check=>check?.status==='failed')||
    updateScenario!==null&&updateScenario.passed!==true;
  const complete=checks.length>0&&checks.every(check=>check?.status==='passed'&&check.complete===true);
  const status=failed?'failed':'passed';
  const scenario={id:scenarioId,status,passed:!failed};
  return {
    scenario,scenarios:[scenario,...(updateScenario?[updateScenario]:[])],
    coverage:{status:complete?'complete':'partial',complete},
    status,passed:!failed,
  };
}

/** Own report assembly and sealing after the browser journey, including its final audit. */
export function runOfflineRuntimeReport({output},journey) {
  return runWithReport({output,schema:'eagler-touhou/offline-runtime-browser/1'},async(report,save)=>{
    const result=await journey(report,save);
    Object.assign(report,offlineRuntimeOutcome({scenarioPassed:result?.scenarioPassed,
      checks:report.checks,updateScenario:result?.updateScenario??null,scenarioId:result?.scenarioId}));
    if(report.finalErrorAudit?.status==='failed'){
      report.preFinalAuditStatus=report.status;
      for(const scenario of report.scenarios){scenario.status='failed';scenario.passed=false;}
      report.status='failed';report.passed=false;
    }
  });
}
