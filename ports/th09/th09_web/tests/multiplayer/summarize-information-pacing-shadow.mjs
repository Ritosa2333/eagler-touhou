// Post-execution summary of the frozen shadow evidence; never reads original game raw.
// No model rerun, control change, browser, subprocess, or parameter search.
import fs from 'node:fs';
import path from 'node:path';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {fileURLToPath} from 'node:url';

const project=path.resolve(fileURLToPath(new URL('../../../',import.meta.url)));
const self=fileURLToPath(import.meta.url);
const base='docs/multiplayer/rollback-information-pacing-shadow-';
const source=path.join(project,base+'evidence.json');
const output=path.join(project,base+'summary.json');
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
const identity=relative=>{const bytes=fs.readFileSync(path.join(project,relative));return {path:relative,bytes:bytes.length,sha256:hash(bytes)};};
assert.ok(!fs.existsSync(output),'Create-only summary already exists; no evidence read.');
const bytes=fs.readFileSync(source);
assert.equal(bytes.length,11695729,'Evidence size changed');
assert.equal(hash(bytes),'e08302b1efce870057e8197eb8d17d10f66f20d133c3939fe3433356b54fda6c','Evidence SHA changed');
const evidence=JSON.parse(bytes.toString('utf8'));
assert.equal(evidence.analysisValid,true);
assert.equal(evidence.contractChecks.checks,12);
const d=evidence.dictionaries,p=evidence.originalPlan.policy;
const pc=Object.fromEntries(d.pointColumns.map((key,index)=>[key,index]));
const gc=Object.fromEntries(d.gapColumns.map((key,index)=>[key,index]));
const value=(row,name)=>{assert.ok(Object.hasOwn(pc,name),name);return row[pc[name]];};
const gapValue=(row,name)=>{assert.ok(Object.hasOwn(gc,name),name);return row[gc[name]];};
const tally=(rows,column,names)=>Object.fromEntries(names.map((name,index)=>[name,rows.filter(row=>value(row,column)===index).length]));
const flags=mask=>Object.entries(d.localObservationMask).filter(([,bit])=>(mask&bit)!==0).map(([name])=>name);

function analyzePeer(peer){
 const rows=peer.pointRows,gaps=peer.gapRows,gapById=new Map(gaps.map(gap=>[gapValue(gap,'gapId'),gap]));
 const gapFor=row=>gapById.get(value(row,'gapId'));
 const example=row=>row?{
  sampleIndex:value(row,'sampleIndex'),cohort:d.cohort[value(row,'cohort')],F:value(row,'F'),A:value(row,'A'),B:value(row,'B'),
  Q:value(row,'Q'),Qfull:value(row,'Qfull'),R:value(row,'R'),networkScaleMilli:value(row,'networkScaleMilli'),
  effectiveScaleMilli:value(row,'effectiveScaleMilli'),ageMs:[value(row,'ageLowerMs'),value(row,'ageUpperMs')],
  eligibleHistoryCount:value(row,'eligibleHistoryCount'),conditionalSurvivors:value(row,'conditionalSurvivors'),
  estimate:d.estimateStatus[value(row,'estimateStatus')],residualNominalMs:value(row,'residualNominalMs'),
  holdSafeMs:value(row,'holdSafeMs'),rampSafeMs:value(row,'rampSafeMs'),warning:d.warningCode[value(row,'warningCode')],
  closureQualification:d.closureQualification[value(row,'closureQualification')],geometry:d.geometricNextProgressClass[value(row,'geometricNextProgressClass')],
  nextAProgressDelayMs:[value(row,'nextAProgressDelayLowerMs'),value(row,'nextAProgressDelayUpperMs')],
  originalThresholdLeadMs:[value(row,'originalBThresholdLeadLowerMs'),value(row,'originalBThresholdLeadUpperMs')],
  prebrakeCandidate:!!value(row,'prebrakeCandidate'),positiveLeadModelOpportunity:!!value(row,'positiveLeadModelOpportunity'),
  runningGapFlags:flags(value(row,'runningGapObservationMask')),wholeGapFlags:flags(gapValue(gapFor(row),'wholeGapObservationMask'))
 }:null;
 const firstWarm=rows.find(row=>value(row,'eligibleHistoryCount')>=p.minimumHistoryGaps);
 const firstWarnings=gaps.filter(gap=>gapValue(gap,'firstWarningIndex')!==null).map(gap=>rows[gapValue(gap,'firstWarningIndex')]);
 const ranges={};
 for(const [name,selected] of [['all',rows],['body',rows.filter(row=>d.cohort[value(row,'cohort')]==='body')]]){
  let retained=selected;
  const policyFunnel=[{stage:'all-points',remaining:retained.length,excludedAtThisStage:0}];
  for(const [stage,keep] of [
   ['A-below-F',row=>value(row,'A')<value(row,'F')],
   ['more-than-one-frame-prediction-room',row=>value(row,'Q')>p.safeReserveFrames],
   ['no-current-or-prior-observed-brake',row=>value(row,'prebrakeCandidate')===1],
   ['running-gap-observed-fields-unflagged',row=>value(row,'runningGapObservationMask')===0]
  ]){const prior=retained.length;retained=retained.filter(keep);policyFunnel.push({stage,remaining:retained.length,excludedAtThisStage:prior-retained.length});}
  const prebrake=selected.filter(row=>value(row,'prebrakeCandidate')===1);
  const bridge=selected.filter(row=>value(row,'geometricNextProgressClass')===2);
  let opportunity=bridge;
  const opportunityFunnel=[{stage:'geometric-next-A-interval-between-safe-hold-and-ramp',remaining:bridge.length,excludedAtThisStage:0}];
  for(const [stage,keep] of [
   ['prebrake-candidate',row=>value(row,'prebrakeCandidate')===1],
   ['at-least-16-eligible-history-gaps',row=>value(row,'eligibleHistoryCount')>=p.minimumHistoryGaps],
   ['not-left-censored',row=>gapValue(gapFor(row),'leftCensored')===0],
   ['closed-and-observed-fields-unflagged',row=>value(row,'closureQualification')===1],
   ['strictly-positive-original-B-threshold-lead',row=>value(row,'originalBThresholdLeadLowerMs')!==null&&value(row,'originalBThresholdLeadLowerMs')>0&&gapValue(gapFor(row),'firstObservedBThresholdIndex')>value(row,'sampleIndex')]
  ]){const prior=opportunity.length;opportunity=opportunity.filter(keep);opportunityFunnel.push({stage,remaining:opportunity.length,excludedAtThisStage:prior-opportunity.length});}
  const storedOpportunities=selected.filter(row=>value(row,'positiveLeadModelOpportunity')===1);
  assert.equal(opportunity.length,storedOpportunities.length,'Stored opportunity flag differs from fixed gate extraction');
  const selectedGaps=name==='all'?gaps:gaps.filter(gap=>d.cohort[gapValue(gap,'warningCohortOrGapStartCohort')]==='body');
  const assignedFirst=selectedGaps.filter(gap=>gapValue(gap,'firstWarningIndex')!==null).map(gap=>rows[gapValue(gap,'firstWarningIndex')]);
  const noWarningGaps=selectedGaps.filter(gap=>gapValue(gap,'warningPointCount')===0);
  ranges[name]={
   originalPredeclaredSummary:peer.summaries[name],
   policyCandidateFunnel:policyFunnel,
   candidateEstimateStates:{allPoints:tally(selected,'estimateStatus',d.estimateStatus),prebrakeCandidates:tally(prebrake,'estimateStatus',d.estimateStatus),
    currentGapUnflaggedPrebrakeCandidates:tally(retained,'estimateStatus',d.estimateStatus)},
   currentGapUnflaggedPrebrakeWarnings:tally(retained,'warningCode',d.warningCode),
   retrospectivelyBridgeablePoints:{count:bridge.length,qualification:tally(bridge,'closureQualification',d.closureQualification),
    funnel:opportunityFunnel,strictOpportunities:storedOpportunities.length,strictOpportunityGaps:new Set(storedOpportunities.map(row=>value(row,'gapId'))).size,
    strictOpportunitiesWithWarning:storedOpportunities.filter(row=>value(row,'warningCode')!==0).length,
    strictOpportunitiesWithoutWarning:storedOpportunities.filter(row=>value(row,'warningCode')===0).length,
    examples:bridge.slice(0,3).map(example)},
   firstWarningOnly:{count:assignedFirst.length,warningTypes:tally(assignedFirst,'warningCode',d.warningCode),
    qualifications:tally(assignedFirst,'closureQualification',d.closureQualification),
    qualifiedGeometry:tally(assignedFirst.filter(row=>value(row,'closureQualification')===1),'geometricNextProgressClass',d.geometricNextProgressClass),
    strictOpportunities:assignedFirst.filter(row=>value(row,'positiveLeadModelOpportunity')===1).length},
   noWarningGapRetrospective:{gaps:noWarningGaps.length,rightCensored:noWarningGaps.filter(gap=>gapValue(gap,'rightCensored')===1).length,
    withStoredStrictOpportunity:noWarningGaps.filter(gap=>gapValue(gap,'firstRetrospectiveOpportunityIndex')!==null).length,
    examples:noWarningGaps.filter(gap=>gapValue(gap,'firstRetrospectiveOpportunityIndex')!==null).slice(0,3).map(gap=>example(rows[gapValue(gap,'firstRetrospectiveOpportunityIndex')]))},
   finalCensoredRows:selected.filter(row=>value(row,'closureQualification')===3).length
  };
 }
 return {side:peer.side,status:peer.status,clock:peer.clock,boundary:peer.boundary,sourceValidation:peer.sourceValidation,
  evidenceQualificationScope:peer.evidenceQualificationScope,unmeasured:peer.unmeasured,
  warmup:{minimumEligibleHistoryGaps:p.minimumHistoryGaps,historyLimit:p.historyGaps,firstWarmObservation:example(firstWarm),
   maximumEligibleHistoryCount:Math.max(...rows.map(row=>value(row,'eligibleHistoryCount'))),
   finalEligibleHistoryCount:rows.length?value(rows.at(-1),'eligibleHistoryCount'):null,admittedGapCount:gaps.filter(gap=>gapValue(gap,'trainingAdmitted')===1).length},
  ranges,firstWarningExamples:Object.fromEntries(d.warningCode.slice(1).map((name,index)=>[name,firstWarnings.filter(row=>value(row,'warningCode')===index+1).slice(0,index===0?8:2).map(example)]))};
}

const results=evidence.results.map(result=>({id:result.id,status:result.status,integrity:result.integrity,peers:result.peers.map(analyzePeer)}));
const summary={schema:'th09-information-pacing-shadow-summary-v1',
 purpose:'Post-execution extraction of fixed evidence. No original raw reads, model rerun, new warning thresholds, or counterfactual game simulation.',
 createdAt:new Date().toISOString(),sourceEvidence:{path:base+'evidence.json',bytes:bytes.length,sha256:hash(bytes)},
 extractionSource:{path:path.relative(project,self),sha256:hash(fs.readFileSync(self))},
 executionRecord:identity(base+'execution.json'),syntaxRecord:identity(base+'syntax.json'),
 analyzer:identity('th09_web/tests/multiplayer/analyze-information-pacing-shadow.mjs'),plan:identity(base+'plan.json'),
 interpretation:{funnel:'Sequential descriptive gate counts; exclusion totals depend on gate order and are not causal attribution.',
  support:'Four conditional survivors gate the residual forecast, not the retrospective strict-opportunity flag; warm history is a distinct gate.',
  censor:'No-warning/right-censored/contaminated gaps are not true negatives or proof of no room for improvement.',
  cohorts:'Original continuous F boundaries. Gap assignment uses first warning or gap start; closure may fall outside its assigned cohort.',
  clocks:'All event examples remain in one peer Page performance clock; no cross-clock subtraction.',
  online:'Whole-report diagnostic eligibility is retrospective. Unflagged fields do not isolate a network cause.'},results};
const text=JSON.stringify(summary,null,2)+'\n';
assert.ok(Buffer.byteLength(text)<=262144,'Summary exceeds the 256 KiB extraction bound');
fs.writeFileSync(output,text,{flag:'wx'});
console.log(JSON.stringify({output,bytes:Buffer.byteLength(text),sha256:hash(text),executionRecord:summary.executionRecord,syntaxRecord:summary.syntaxRecord,
 cases:results.map(result=>({id:result.id,peers:result.peers.map(peer=>({side:peer.side,firstWarmSample:peer.warmup.firstWarmObservation?.sampleIndex??null,
  maximumEligibleHistoryCount:peer.warmup.maximumEligibleHistoryCount,admittedGapCount:peer.warmup.admittedGapCount,
  firstWarnings:peer.ranges.all.firstWarningOnly,allStrictOpportunities:peer.ranges.all.retrospectivelyBridgeablePoints.strictOpportunities,
  allStrictOpportunitiesWithoutWarning:peer.ranges.all.retrospectivelyBridgeablePoints.strictOpportunitiesWithoutWarning,
  noWarningGapRetrospective:peer.ranges.all.noWarningGapRetrospective.withStoredStrictOpportunity}))}))}));

