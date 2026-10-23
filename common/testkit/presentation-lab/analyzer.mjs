import {compactReport,defaultSeverity,groupFindings,mergeIssueGroups} from './report-core.mjs';
import {validateStateAuthority} from './state-authority.mjs';

const finiteArray=value=>Array.isArray(value)&&value.every(Number.isFinite);
const asArray=value=>Array.isArray(value)?value:[value];
const wrap=(value,period)=>period?((value+period/2)%period+period)%period-period/2:value;
const distance=(a,b,period=0)=>a.length===b.length?Math.max(0,...a.map((value,index)=>Math.abs(wrap(b[index]-value,period)))):Infinity;
const identityKey=record=>record.key??[record.ownerId,record.objectId,record.generation??'?',record.partId??0].join(':');
const indexRecords=records=>{const map=new Map(),duplicates=new Set();for(const record of records||[]){const key=identityKey(record);if(map.has(key))duplicates.add(key);else map.set(key,record);}return {map,duplicates};};
const fieldMap=record=>new Map((record?.fields||[]).map(field=>[field.id,field]));

export function compareStateEvidence(before,afterSamples){
  const groupsOf=evidence=>{
    if(!evidence||typeof evidence.coverageVersion!=='string'||!evidence.coverageVersion||!Array.isArray(evidence.groups)||!Array.isArray(evidence.missingGroups)||!evidence.missingGroups.every(group=>typeof group==='string'&&group))return null;
    if(evidence.groups.length===0)return null;
    const groups=new Map();
    for(const group of evidence.groups){const digestValid=typeof group?.digest==='string'?group.digest.length>0:Number.isFinite(group?.digest);if(!group||typeof group.id!=='string'||!group.id||!digestValid||groups.has(group.id))return null;groups.set(group.id,group.digest);}
    const authority=validateStateAuthority(evidence);
    return {groups,authority,authorityKey:JSON.stringify((evidence.authority?.domains||[]).map(domain=>({id:domain.id,groups:[...(domain.groups||[])].sort(),sourceFields:[...(domain.sourceFields||[])].sort()})).sort((a,b)=>String(a.id).localeCompare(String(b.id))))};
  };
  const baseline=groupsOf(before);
  if(!baseline)
    return {status:'unknown',changes:[],reason:'missing state coverage descriptor',authorityCoverage:{complete:false,reason:'missing state coverage descriptor'}};
  if(!Array.isArray(afterSamples)||afterSamples.length===0)
    return {status:'unknown',changes:[],reason:'missing draw-only state samples',authorityCoverage:baseline.authority};
  const changes=[],sampleAuthority=[];let unknown=before.missingGroups.length>0||!baseline.authority.complete;
  for(let sample=0;sample<afterSamples.length;sample++){
    const evidence=afterSamples[sample];
    const groups=groupsOf(evidence);
    if(!groups||evidence.coverageVersion!==before.coverageVersion||groups.groups.size!==baseline.groups.size){unknown=true;continue;}
    if(evidence.missingGroups.length||!groups.authority.complete||groups.authorityKey!==baseline.authorityKey){unknown=true;sampleAuthority.push({sample,...groups.authority,mappingStable:groups.authorityKey===baseline.authorityKey});}
    for(const [id,digest] of baseline.groups){if(!groups.groups.has(id)){unknown=true;continue;}if(groups.groups.get(id)!==digest)changes.push({sample,group:id,before:digest,after:groups.groups.get(id)});}
  }
  return {status:changes.length?'fail':unknown?'unknown':'pass',changes,authorityCoverage:{...baseline.authority,samples:sampleAuthority}};
}

export function presentationWindowHealth(value){
  const purity=value?.purityStatus??(value?.purity===false?'fail':value?.purity===true?'pass':'unknown');
  if(purity==='fail')return {status:'error',message:'重画改变了已覆盖状态；本窗口不可作为纯 presentation 证据。'};
  if(value?.validityStatus==='invalid')return {status:'error',message:'采样窗口无效；帧序列或观测完整性不符合契约。'};
  if(value?.validityStatus!=='valid'||value?.valid!==true)return {status:'warn',message:'采样窗口有效性未知；缺失的帧或范围信息不能作为完整通过证据。'};
  if(purity!=='pass')return {status:'warn',message:'状态覆盖不完整；对象插值结果可查看，但纯度保持未知。'};
  return {status:'ready',message:'帧序列有效，且已覆盖状态在重复重画前后保持一致。'};
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

export function analyzeNormalizedWindow({previous,current,samples,stateBefore,statesAfter=[],build={},label='',negativeControl=false}){
  if(!previous||!current||!Array.isArray(samples)||samples.length<3)throw Error('Need two reference frames and at least three presentation samples');
  if(samples.some(sample=>!Number.isFinite(sample.alpha)||sample.alpha<0||sample.alpha>1))throw Error('Invalid presentation alpha');
  const before=indexRecords(previous.records),now=indexRecords(current.records),sampleMaps=samples.map(sample=>({...sample,...indexRecords(sample.records)}));
  const consecutive=Number.isInteger(previous.simulationTick)&&current.simulationTick===previous.simulationTick+1;
  const state=compareStateEvidence(stateBefore,statesAfter);
  const validityIssues=[],unknownValidity=[];
  const frames=[['previous',previous],['current',current],...samples.map((sample,index)=>[`sample-${index}`,sample])];
  const validEpoch=frame=>typeof frame.sessionEpoch==='number'?Number.isFinite(frame.sessionEpoch):typeof frame.sessionEpoch==='string'&&frame.sessionEpoch.length>0;
  for(const [name,frame] of frames){
    if(!validEpoch(frame))unknownValidity.push({code:'missing-session-epoch',frame:name});
    if(!Number.isInteger(frame.simulationTick))unknownValidity.push({code:'missing-simulation-tick',frame:name});
    if(frame.completeness!=='complete')unknownValidity.push({code:frame.completeness==='incomplete'?'incomplete-frame':'missing-frame-completeness',frame:name});
    if(!Array.isArray(frame.records))unknownValidity.push({code:'missing-record-list',frame:name});
    if(!Number.isInteger(frame.dropped)||frame.dropped<0)unknownValidity.push({code:'missing-drop-count',frame:name});
  }
  const epochs=frames.filter(([,frame])=>validEpoch(frame)).map(([,frame])=>frame.sessionEpoch);
  if(epochs.some(epoch=>epoch!==epochs[0]))validityIssues.push({code:'session-epoch-mismatch'});
  if(Number.isInteger(previous.simulationTick)&&Number.isInteger(current.simulationTick)&&!consecutive)
    validityIssues.push({code:'reference-tick-mismatch'});
  for(let index=0;index<samples.length;index++){
    const tick=samples[index].simulationTick;
    if(Number.isInteger(tick)&&Number.isInteger(current.simulationTick)&&tick!==current.simulationTick)
      validityIssues.push({code:'sample-tick-mismatch',frame:`sample-${index}`,expected:current.simulationTick,actual:tick});
  }
  const alphaCoverage=samples.some(sample=>sample.alpha===0)&&samples.some(sample=>sample.alpha===1)&&samples.filter(sample=>sample.alpha===.5).length>=2;
  if(!alphaCoverage)validityIssues.push({code:'incomplete-alpha-coverage'});
  const dropped=frames.reduce((total,[,frame])=>total+(Number.isInteger(frame.dropped)&&frame.dropped>=0?frame.dropped:0),0);
  if(dropped!==0)validityIssues.push({code:'dropped-observations',count:dropped});
  const validityStatus=validityIssues.length?'invalid':unknownValidity.length?'unknown':'valid';
  const report={schema:'presentation-lab/report/1',build,label,negativeControl,previousTick:previous.simulationTick,tick:current.simulationTick,
    previousDrawSerial:previous.referenceDrawSerial,currentDrawSerial:current.referenceDrawSerial,consecutive,validityStatus,validityIssues,unknownValidity,
    purityStatus:state.status,stateChanges:state.changes,stateAuthority:state.authorityCoverage,objects:[],counts:{},coverage:{},dropped,
    limitations:[...new Set([...(previous.limitations||[]),...(current.limitations||[])])]};
  for(const [key,record] of now.map){
    const prior=before.map.get(key),selected=sampleMaps.map(sample=>({alpha:sample.alpha,record:sample.map.get(key)}));
    const row={key,owner:record.ownerLabel||String(record.ownerId),ownerId:record.ownerId,object:record.objectId,generation:record.generation,part:record.partId,draw:record.drawId,
      anm:record.lifecycle?.fileIndex,script:record.lifecycle?.scriptIndex,kind:record.coordinateSpace,
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
  for(const [key,record] of before.map){
    if(now.map.has(key))continue;
    const row={key,owner:record.ownerLabel||String(record.ownerId),ownerId:record.ownerId,object:record.objectId,generation:record.generation,part:record.partId,draw:record.drawId,
      anm:record.lifecycle?.fileIndex,script:record.lifecycle?.scriptIndex,kind:record.coordinateSpace,lifecycle:record.lifecycle,coordinateSpace:record.coordinateSpace,
      bounds:record.bounds,geometryQuality:record.geometryQuality,status:'disappeared',diagnostic:'present-in-previous-absent-in-current',severity:0,
      fields:(record.fields||[]).map(field=>({id:field.id,label:field.label||field.id,status:'unobserved',reason:'object-absent-from-current-reference'}))};
    report.objects.push(row);report.counts.disappeared=(report.counts.disappeared||0)+1;
  }
  report.objects.sort((a,b)=>b.severity-a.severity||String(a.ownerId).localeCompare(String(b.ownerId))||a.key.localeCompare(b.key));
  report.valid=validityStatus==='valid';
  report.issueGroups=groupFindings([report]);return report;
}

export {compactReport,defaultSeverity,groupFindings,mergeIssueGroups};
