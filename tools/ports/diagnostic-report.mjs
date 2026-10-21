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
