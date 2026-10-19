// Pure post-processing: no browser hooks, input changes, or sample filtering.
import assert from 'node:assert/strict';

export const SMOOTHNESS_WINDOW_SCHEMA='continuous-prediction-frontier-v1';

const distribution=values=>{
 const a=values.slice().sort((x,y)=>x-y);
 return {count:a.length,total:a.reduce((sum,n)=>sum+n,0),p50:a[Math.floor(a.length*.5)]??null,p95:a[Math.floor(a.length*.95)]??null,
  p99:a[Math.floor(a.length*.99)]??null,max:a.at(-1)??null,over25ms:a.filter(n=>n>25).length,over50ms:a.filter(n=>n>50).length,
  over100ms:a.filter(n=>n>100).length,over250ms:a.filter(n=>n>250).length};
};
const largest=(rows,key)=>rows.reduce((best,row)=>best===null||row[key]>best[key]?row:best,null);

export function summarizeSmoothnessWindow(result,{startFrame=180,endFrame}={}){
 assert.ok(Number.isInteger(startFrame)&&startFrame>=0&&Number.isInteger(endFrame)&&endFrame>startFrame,'Invalid formal frame bounds');
 const samples=result.measure.samples,n=samples.length,pacing=result.measure.networkPacing??[];
 for(const row of samples){
  assert.ok(Array.isArray(row)&&row.length>=14,'Incomplete smoothness sample');
  for(const col of [0,1,2,3,5,6,7,11,12,13])assert.ok(Number.isFinite(row[col]),'Non-finite smoothness sample');
 }
 const native=new Array(n).fill(null),nativeErrors=[];
 for(const row of pacing){
  if(!Array.isArray(row)||row.length<7||!Number.isInteger(row[0])||row[0]<0||row[0]>=n){nativeErrors.push({kind:'invalid-pacing-index',row});continue;}
  const i=row[0];
  if(native[i]!==null){nativeErrors.push({kind:'duplicate-pacing-index',sampleIndex:i});continue;}
  native[i]=row;
  if(!Number.isInteger(row[5])||row[5]<samples[i][3])nativeErrors.push({kind:'invalid-native-frontier',sampleIndex:i,frontier:row[5],next:samples[i][3]});
 }
 const missingNative=native.flatMap((row,i)=>row===null?[i]:[]);
 const nativeComplete=n>0&&missingNative.length===0&&nativeErrors.length===0;
 let highWater=0;
 const observedHighWater=samples.map(row=>highWater=Math.max(highWater,row[3]));
 const frontier=nativeComplete?native.map(row=>row[5]):observedHighWater;
 const source=nativeComplete?'native-networkPacing-PredictionFrontier':'observed-next-high-water';
 const epochErrors=[];
 for(let i=1;i<n;++i){
  if(frontier[i]<frontier[i-1])epochErrors.push({kind:'frontier-regressed',sampleIndex:i});
  for(const [col,name] of [[0,'sampledAt'],[5,'corrections'],[6,'resimulated'],[7,'captures'],[11,'renderer-submissions'],[13,'RAF-timestamp']])
   if(samples[i][col]<samples[i-1][col])epochErrors.push({kind:name+'-regressed',sampleIndex:i});
 }
 const epochValid=nativeErrors.length===0&&epochErrors.length===0;
 const start=frontier.findIndex(f=>f>=startFrame),end=frontier.findIndex(f=>f>=endFrame);
 const bodyStart=start<0?n:start,bodyEnd=end<0?n:end;
 const elapsed=start>=0&&end>start?samples[end][0]-samples[start][0]:null;
 const closed=epochValid&&elapsed!==null&&elapsed>0;
 const point=i=>i<0||i>=n?null:{sampleIndex:i,sample:samples[i].slice(),nativePacing:native[i]?.slice()??null,predictionFrontier:frontier[i]};
 const deltas=(a,b)=>({frontier:frontier[b]-frontier[a],next:samples[b][3]-samples[a][3],corrections:samples[b][5]-samples[a][5],
  resimulated:samples[b][6]-samples[a][6],captures:samples[b][7]-samples[a][7],rendererSubmissions:samples[b][11]-samples[a][11]});
 const pair=(a,b)=>({fromSample:a,toSample:b,rafTimestampMs:samples[b][13]-samples[a][13],sampledAtMs:samples[b][0]-samples[a][0],
  frontierDelta:frontier[b]-frontier[a],nextDelta:samples[b][3]-samples[a][3],rendererSubmissions:samples[b][11]-samples[a][11],
  fromSessionPhase:samples[a][12],toSessionPhase:samples[b][12]});
 const adjacent=[],presented=[],newStates=[];
 let lastPresent=0,lastNew=0;
 for(let i=1;i<n;++i){
  adjacent.push(pair(i-1,i));
  if(samples[i][11]>samples[lastPresent][11]){presented.push(pair(lastPresent,i));lastPresent=i;}
  // The cursor must have completed recovery to the observed frontier. A capture
  // ahead of replay completion, or another submission of the same state, is not
  // a newly simulated and submitted game state.
  if(samples[i][11]>samples[lastNew][11]&&frontier[i]>frontier[lastNew]&&samples[i][3]===frontier[i]){
   newStates.push(pair(lastNew,i));lastNew=i;
  }
 }
 const inside=(rows,a,b)=>rows.filter(row=>row.fromSample>=a&&row.toSample<b);
 const gapStats=rows=>({rafTimestampMs:distribution(rows.map(row=>row.rafTimestampMs)),sampledAtMs:distribution(rows.map(row=>row.sampledAtMs)),
  maximumRafGap:largest(rows,'rafTimestampMs'),maximumSampledAtGap:largest(rows,'sampledAtMs')});
 const segment=(a,b)=>{
  const rows=samples.slice(a,b),states=inside(newStates,a,b);
  return {range:[a,b],callbacks:rows.length,waitCallbacks:rows.filter(row=>row[1]===2).length,
   callbackWorkMs:distribution(rows.map(row=>row[2])),sessionPhases:[...new Set(rows.map(row=>row[12]))],
   adjacentCallbacks:gapStats(inside(adjacent,a,b)),newStates:gapStats(states)};
 };
 const body=samples.slice(bodyStart,bodyEnd),bodyPresent=inside(presented,bodyStart,bodyEnd),bodyNew=inside(newStates,bodyStart,bodyEnd);
 const bodyAdjacent=inside(adjacent,bodyStart,bodyEnd),closingNew=closed?inside(newStates,bodyStart,end+1):[];
 const markerDeltas=closed?deltas(start,end):null;
 const crossings=boundary=>boundary<0?[]:newStates.filter(row=>row.fromSample<boundary&&row.toSample>=boundary);
 const statuses=[];
 if(!nativeComplete)statuses.push('native-frontier-incomplete');
 if(!epochValid)statuses.push('invalid-progress-epoch');
 if(start<0)statuses.push('start-not-observed');
 else if(end<0)statuses.push('end-not-observed');
 else if(end<=start||!(elapsed>0))statuses.push('empty-boundary-span');
 const phaseValues=[...new Set(body.map(row=>row[12]))];
 const lastObserved=start>=0&&bodyEnd>bodyStart?bodyEnd-1:-1;
 const lastClosedNew=closingNew.at(-1)?.toSample??(start>=0?start:-1);
 return {
  logicHz:closed?markerDeltas.frontier*1000/elapsed:null,elapsedMs:closed?elapsed:null,
  firstFrame:start>=0?frontier[start]:null,lastFrame:end>=0?frontier[end]:null,
  callbacks:body.length,waitCallbacks:body.filter(row=>row[1]===2).length,
  corrections:markerDeltas?.corrections??null,resimulated:markerDeltas?.resimulated??null,captures:markerDeltas?.captures??null,
  callbackWorkMs:distribution(body.map(row=>row[2])),presentationGapMs:distribution(bodyPresent.map(row=>row.rafTimestampMs)),
  effectivePresentationGapMs:distribution(bodyNew.map(row=>row.rafTimestampMs)),callbackCompletionGapMs:distribution(bodyPresent.map(row=>row.sampledAtMs)),
  rafGapMs:distribution(bodyAdjacent.filter(row=>row.rafTimestampMs!==0).map(row=>row.rafTimestampMs)),
  plannedDelayMs:distribution(result.impairment?.planned??[]),deliveredDelayMs:distribution(result.impairment?.delivered??[]),
  scenes:phaseValues,sessionPhases:phaseValues,
  window:{schema:SMOOTHNESS_WINDOW_SCHEMA,status:statuses.length?statuses:['complete'],complete:closed&&nativeComplete,
   requestedFrameRange:[startFrame,endFrame],progressSource:source,nativeFrontierComplete:nativeComplete,
   nativeFrontierMissingSampleIndices:missingNative,nativeFrontierErrors:nativeErrors,epochValid,epochErrors,
   nativeMatchesObservedNextHighWater:nativeComplete?frontier.every((f,i)=>f===observedHighWater[i]):null,
   startSampleIndex:start<0?null:start,endSampleIndexExclusive:end<0?null:end,bodyRange:[bodyStart,bodyEnd],
   startMarker:point(start),endMarker:point(end),closedBoundaryAvailable:closed,markerDeltas,
   bodyFirstToLast:epochValid&&lastObserved>bodyStart?{fromSample:bodyStart,toSample:lastObserved,elapsedMs:samples[lastObserved][0]-samples[bodyStart][0],deltas:deltas(bodyStart,lastObserved)}:null,
   formalWithEndBoundary:{available:closed,newStates:gapStats(closingNew),adjacentCallbacks:gapStats(closed?inside(adjacent,bodyStart,end+1):[]),
    lastObservedNewStateSample:lastClosedNew>=0?lastClosedNew:null,endMarkerHasNewState:closed?newStates.some(row=>row.toSample===end):null,
    observedWithoutNewStateToEnd:closed&&lastClosedNew<end?pair(lastClosedNew,end):null},
   crossingNewStateIntervals:{start:crossings(start),end:crossings(end)},
   segments:{startup:segment(0,bodyStart),body:segment(bodyStart,bodyEnd),tail:segment(bodyEnd,n),allObserved:segment(0,n)},
   scopes:{body:'Every original sample in [first frontier >= startFrame, first frontier >= endFrame); no phase or rewound-cursor filtering. Without a terminal crossing the observed body is right-censored.',
    progress:'Closed Hz and cumulative counter deltas use endpoint minus start, including the endpoint counters. The endpoint callback cost belongs to tail; body cost includes start. Missing native frontier uses explicitly labelled observed next high-water, never captures, and complete stays false.',
    gaps:'Top-level gap fields retain body-only submission/RAF timestamp semantics. formalWithEndBoundary also closes through the terminal marker. Cross-boundary intervals and all sampled startup/tail costs remain separate. No sample is removed from raw results.',
    timing:'callbackCompletionGapMs is the post-native sampledAt gap between renderer submissions, not whole observer callback return; RAF timestamps and submissions are not physical scanout.',
    phase:'scenes is a compatibility alias of sessionPhases (SessionPhase, not internal MatchScene); no phase is excluded.',
    capture:'captures is a once-only input-capture counter and may be ahead of simulation completion; it never chooses boundaries.',
    nativeWork:'callbackWorkMs is the existing native game callback measurement, not total JavaScript observer time or GPU completion.'}
  }
 };
}
