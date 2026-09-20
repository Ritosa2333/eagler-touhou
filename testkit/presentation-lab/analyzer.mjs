import {compactReport,defaultSeverity,groupFindings,mergeIssueGroups} from './report-core.mjs';

const finiteArray=value=>Array.isArray(value)&&value.every(Number.isFinite);
const asArray=value=>Array.isArray(value)?value:[value];
const wrap=(value,period)=>period?((value+period/2)%period+period)%period-period/2:value;
const distance=(a,b,period=0)=>a.length===b.length?Math.max(0,...a.map((value,index)=>Math.abs(wrap(b[index]-value,period)))):Infinity;
const identityKey=record=>record.key??[record.ownerId,record.objectId,record.generation??'?',record.partId??0].join(':');
const indexRecords=records=>{const map=new Map(),duplicates=new Set();for(const record of records||[]){const key=identityKey(record);if(map.has(key))duplicates.add(key);else map.set(key,record);}return {map,duplicates};};
const fieldMap=record=>new Map((record?.fields||[]).map(field=>[field.id,field]));

export function compareStateEvidence(before,afterSamples){
  if(!before||!Array.isArray(before.groups)||!Array.isArray(before.missingGroups))
    return {status:'unknown',changes:[],reason:'missing state coverage descriptor'};
  const baseline=new Map(before.groups.map(group=>[group.id,group.digest]));
  const changes=[];let unknown=before.missingGroups.length>0;
  for(let sample=0;sample<afterSamples.length;sample++){
    const evidence=afterSamples[sample];
    if(!evidence||evidence.coverageVersion!==before.coverageVersion||!Array.isArray(evidence.groups)){unknown=true;continue;}
    if(evidence.missingGroups?.length)unknown=true;
    const groups=new Map(evidence.groups.map(group=>[group.id,group.digest]));
    for(const [id,digest] of baseline){if(!groups.has(id)){unknown=true;continue;}if(groups.get(id)!==digest)changes.push({sample,group:id,before:digest,after:groups.get(id)});}
  }
  return {status:changes.length?'fail':unknown?'unknown':'pass',changes};
}

function classifyField(previous,current,samples){
  const p=asArray(previous.value),c=asArray(current.value),period=current.period||previous.period||0,tolerance=current.tolerance??previous.tolerance??0;
  const observed=samples.map(sample=>({alpha:sample.alpha,value:asArray(sample.field.value)}));
  const motion=distance(p,c,period),policy=current.interpolationPolicy||previous.interpolationPolicy||'unknown';
  const evidence={previous:p,current:c,samples:observed,motion,tolerance,interpolationPolicy:policy,evidence:current.evidence??previous.evidence??null};
  if(!finiteArray(p)||!finiteArray(c)||observed.some(sample=>!finiteArray(sample.value)))return {...evidence,status:'nonfinite'};
  for(let i=0;i<observed.length;i++)for(let j=i+1;j<observed.length;j++)if(observed[i].alpha===observed[j].alpha&&distance(observed[i].value,observed[j].value,period)>tolerance)return {...evidence,status:'non-idempotent'};
  const at0=observed.find(sample=>sample.alpha===0),at1=observed.find(sample=>sample.alpha===1);
  if(at1&&distance(at1.value,c,period)>tolerance*2)return {...evidence,status:'endpoint-mismatch-current'};
  if(policy==='continuous'&&at0&&distance(at0.value,p,period)>tolerance*2)return {...evidence,status:'endpoint-mismatch-previous'};
  const variation=Math.max(0,...observed.map(sample=>distance(sample.value,observed[0].value,period)));
  if(motion<=tolerance*2)return {...evidence,variation,status:variation>tolerance*2?'responsive':'static'};
  if(policy==='snap-current')return {...evidence,variation,status:observed.every(sample=>distance(sample.value,c,period)<=tolerance*2)?'snap-current':'snap-mismatch'};
  const noise=Math.max(1e-6,tolerance*.02);
  if(variation<=noise)return {...evidence,variation,status:policy==='continuous'?'missing-interpolation':'held-review'};
  if(policy!=='continuous')return {...evidence,variation,status:'responsive'};
  const delta=p.map((value,index)=>wrap(c[index]-value,period));
  const residual=Math.max(...observed.map(sample=>distance(sample.value,p.map((value,index)=>value+delta[index]*sample.alpha),period)));
  return {...evidence,variation,residual,status:residual<=Math.max(tolerance*2,motion*.02)?'interpolated':'responsive'};
}

export function analyzeNormalizedWindow({previous,current,samples,stateBefore,stateAfter=[],build={},label='',negativeControl=false}){
  if(!previous||!current||!Array.isArray(samples)||samples.length<3)throw Error('Need two reference frames and at least three presentation samples');
  if(samples.some(sample=>!Number.isFinite(sample.alpha)||sample.alpha<0||sample.alpha>1))throw Error('Invalid presentation alpha');
  const before=indexRecords(previous.records),now=indexRecords(current.records),sampleMaps=samples.map(sample=>({...sample,...indexRecords(sample.records)}));
  const consecutive=Number.isInteger(previous.simulationTick)&&current.simulationTick===previous.simulationTick+1;
  const state=compareStateEvidence(stateBefore,stateAfter);
  const report={schema:'presentation-lab/report/1',build,label,negativeControl,previousTick:previous.simulationTick,tick:current.simulationTick,
    previousDrawSerial:previous.referenceDrawSerial,currentDrawSerial:current.referenceDrawSerial,consecutive,purityStatus:state.status,stateChanges:state.changes,
    objects:[],counts:{},coverage:{},dropped:(previous.dropped||0)+(current.dropped||0)+samples.reduce((total,sample)=>total+(sample.dropped||0),0),
    limitations:[...new Set([...(previous.limitations||[]),...(current.limitations||[])])]};
  for(const [key,record] of now.map){
    const prior=before.map.get(key),selected=sampleMaps.map(sample=>({alpha:sample.alpha,record:sample.map.get(key)}));
    const row={key,owner:record.ownerLabel||String(record.ownerId),ownerId:record.ownerId,object:record.objectId,generation:record.generation,part:record.partId,draw:record.drawId,
      lifecycle:record.lifecycle,coordinateSpace:record.coordinateSpace,bounds:record.bounds,geometryQuality:record.geometryQuality,fields:[],status:'static'};
    let skip='';
    if(record.identityConfidence!=='proven')skip='identity-uncertain';
    else if(now.duplicates.has(key)||before.duplicates.has(key)||sampleMaps.some(sample=>sample.duplicates.has(key)))skip='ambiguous';
    else if(!consecutive||!prior||selected.some(sample=>!sample.record))skip='unobserved';
    else if(record.lifecycle?.continuous===false||prior.lifecycle?.continuous===false)skip='lifecycle';
    if(skip)row.status=skip;
    else{
      const previousFields=fieldMap(prior),currentFields=fieldMap(record),sampleFields=selected.map(sample=>({alpha:sample.alpha,fields:fieldMap(sample.record)}));
      for(const [id,field] of currentFields){
        const previousField=previousFields.get(id),observed=sampleFields.map(sample=>({alpha:sample.alpha,field:sample.fields.get(id)}));
        if(!previousField||observed.some(sample=>!sample.field)){row.fields.push({id,label:field.label||id,status:'unobserved'});continue;}
        row.fields.push({id,label:field.label||id,...classifyField(previousField,field,observed)});
      }
      row.status=row.fields.reduce((status,field)=>defaultSeverity(field.status)>defaultSeverity(status)?field.status:status,'static');
      if(row.status==='static'&&row.fields.some(field=>field.status==='interpolated'))row.status='interpolated';
    }
    row.severity=defaultSeverity(row.status);report.objects.push(row);report.counts[row.status]=(report.counts[row.status]||0)+1;
    const coverage=report.coverage[row.owner]??={observed:0,changing:0,issues:0,unresolved:0};coverage.observed++;
    if(row.fields.some(field=>(field.motion||0)>(field.tolerance||0)*2))coverage.changing++;
    if(row.severity>=3)coverage.issues++;
    if(['unobserved','identity-uncertain','ambiguous','lifecycle'].includes(row.status))coverage.unresolved++;
  }
  report.objects.sort((a,b)=>b.severity-a.severity||String(a.ownerId).localeCompare(String(b.ownerId))||a.key.localeCompare(b.key));
  report.valid=consecutive&&report.dropped===0&&samples.every(sample=>sample.simulationTick===undefined||sample.simulationTick===current.simulationTick)&&samples.some(sample=>sample.alpha===0)&&samples.some(sample=>sample.alpha===1)&&samples.filter(sample=>sample.alpha===.5).length>=2;
  report.issueGroups=groupFindings([report]);return report;
}

export {compactReport,defaultSeverity,groupFindings,mergeIssueGroups};
