/** Keep execution of this offline lane separate from its adaptation coverage. */
export function offlineRuntimeOutcome({scenarioPassed,checks=[],updateScenario=null}={}) {
  const failed=scenarioPassed!==true||checks.some(check=>check?.status==='failed')||
    updateScenario!==null&&updateScenario.passed!==true;
  const complete=checks.length>0&&checks.every(check=>check?.status==='passed'&&check.complete===true);
  const status=failed?'failed':'passed';
  const scenario={id:'offline-import-reopen',status,passed:!failed};
  return {
    scenario,scenarios:[scenario,...(updateScenario?[updateScenario]:[])],
    coverage:{status:complete?'complete':'partial',complete},
    status,passed:!failed,
  };
}
