/** Keep execution of this offline lane separate from its adaptation coverage. */
export function offlineRuntimeOutcome({scenarioPassed,checks=[]}={}) {
  const failed=scenarioPassed!==true||checks.some(check=>check?.status==='failed');
  const complete=checks.length>0&&checks.every(check=>check?.status==='passed'&&check.complete===true);
  const status=failed?'failed':'passed';
  return {
    scenario:{id:'offline-import-reopen',status,passed:!failed},
    coverage:{status:complete?'complete':'partial',complete},
    status,passed:!failed,
  };
}
