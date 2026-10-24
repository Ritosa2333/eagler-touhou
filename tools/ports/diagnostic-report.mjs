import assert from 'node:assert/strict';

/** The session adds input and browser receipts to the immutable served build. */
export function assertDiagnosticCaptureBuild(captured,served){
  assert.ok(captured&&served&&Object.keys(served).length,'Missing diagnostic build identity');
  const projection=Object.fromEntries(Object.keys(served).map(key=>[key,captured[key]]));
  assert.deepEqual(projection,served,'Capture used another served diagnostic build');
}

/** Report adapters shared by the browser driver and its result-contract tests. */
export function plannedDiagnosticChecks(lanes,checkMap){
  return lanes.flatMap(lane=>(checkMap[lane]??[]).map(id=>({id,lane,status:'not-run',complete:false})));
}

export function recordDiagnosticLane(target,lane,laneReport,{checkIds,primaryCheckId}){
  const entries=checkIds.map(id=>{
    const complete=laneReport.status==='passed'&&id===primaryCheckId&&!!laneReport.observation;
    const status=laneReport.status==='passed'?(complete?'passed':'partial'):laneReport.status;
    const entry={id,lane,status,complete,scope:laneReport.scope??lane};
    if(laneReport.observation)entry.observation={game:laneReport.game,variant:laneReport.variant,
      adaptationIds:[id],observation:laneReport.observation.observation};
    if(laneReport.error)entry.error=laneReport.error;
    return entry;
  });
  target.checks=target.checks.filter(check=>check.lane!==lane).concat(entries);
  return entries;
}

export function finalizeDiagnosticTarget(target,laneReports,lanes,identityStable){
  const statuses=laneReports.map(lane=>lane.status);
  target.status=identityStable&&statuses.length===lanes.length&&statuses.every(status=>status==='passed')?'passed':
    statuses.some(status=>status==='failed')||identityStable===false?'failed':'incomplete';
  target.passed=target.status==='passed';
  target.complete=target.checks.length>0&&target.checks.every(check=>check.complete===true);
  return target;
}

export function diagnosticTargetCoverage(target){
  return{complete:target.complete===true,selectedChecks:target.checks.map(check=>check.id),
    completeChecks:target.checks.filter(check=>check.complete).map(check=>check.id)};
}

/** Preserve the finalized target outcome when the CLI seals its envelope. */
export function finalizeDiagnosticReport(report,target){
  assert.ok(report.targets?.includes(target),'Diagnostic report must own its finalized target');
  assert.ok(['passed','failed','incomplete'].includes(target.status),'Diagnostic target is not finalized');
  report.coverage=diagnosticTargetCoverage(target);
  report.status=target.status;
  report.passed=target.passed===true&&target.status==='passed';
  return report;
}
