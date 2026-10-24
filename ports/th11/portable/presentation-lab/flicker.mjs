// Threshold analysis for interpolated-frame displacement. Pure data in/out so
// it can run in the browser, in node tests, or over an exported campaign.
export const DEFAULT_THRESHOLDS=Object.freeze({maxStep:48,maxError:2.5,minStep:8,rebaseStep:200});
const round=value=>Math.round(value*1000)/1000;
const field=(record,id)=>{const value=record?.fields?.find(entry=>entry.id===id)?.value;return Array.isArray(value)?value:[0,0,0];};
export function recordPosition(record){
  const p=field(record,'position'),s=field(record,'script-position'),c=field(record,'child-position');
  return [p[0]+s[0]+c[0],p[1]+s[1]+c[1],p[2]+s[2]+c[2]];
}
const distance=(a,b)=>Math.hypot(a[0]-b[0],a[1]-b[1],a[2]-b[2]);
const midpoint=(a,b,alpha)=>[a[0]+(b[0]-a[0])*alpha,a[1]+(b[1]-a[1])*alpha,a[2]+(b[2]-a[2])*alpha];
export function analyzeFlicker({previous,current,samples,thresholds={}}){
  const limits={...DEFAULT_THRESHOLDS,...thresholds};
  const prior=new Map(previous.records.map(record=>[record.key,record]));
  const flagged=[],rebases=[];let checked=0,skipped=0;
  for(const record of current.records){
    const before=prior.get(record.key);
    if(!before){++skipped;continue;}
    const from=recordPosition(before),to=recordPosition(record),step=distance(from,to);
    if(step<limits.minStep){++skipped;continue;}
    ++checked;
    let maxSampleStep=0,maxError=0,worstAlpha=null,last=null,present=0;
    for(const sample of samples){
      const entry=sample.records.find(candidate=>candidate.key===record.key);
      if(!entry)continue;
      const got=recordPosition(entry),expected=midpoint(from,to,sample.alpha),error=distance(got,expected);
      ++present;
      if(error>maxError){maxError=error;worstAlpha=sample.alpha;}
      if(last)maxSampleStep=Math.max(maxSampleStep,distance(last,got));
      last=got;
    }
    if(present<2)continue;
    const reasons=[];
    if(maxSampleStep>limits.maxStep)reasons.push('large-interpolated-step');
    if(maxError>limits.maxError)reasons.push('endpoint-mismatch');
    if(!reasons.length)continue;
    const entry={tick:current.tick||0,key:record.key,owner:record.ownerLabel,objectId:record.objectId,generation:record.generation,layer:record.layer,
      referenceStep:round(step),maxSampleStep:round(maxSampleStep),maxError:round(maxError),worstAlpha,reasons,
      fileIndex:record.lifecycle?.fileIndex,scriptIndex:record.lifecycle?.scriptIndex,spriteIndex:record.lifecycle?.spriteIndex};
    // A large world step with no sample movement is an authored rebase that
    // correctly snapped; keep it separately instead of reporting flicker.
    // A snap shows only the two endpoints (min/mid max error); a blend moves
    // through intermediate values. Both are authored rebases when the step is
    // at the world-rebase scale.
    if(step>=limits.rebaseStep&&(maxSampleStep<.5||maxSampleStep>=step*.9)){entry.kind='authored-rebase';rebases.push(entry);continue;}
    entry.kind=step>=limits.rebaseStep?'blended-discontinuity':maxSampleStep<.5?'no-interpolation':'interpolated-step';
    flagged.push(entry);
  }
  return {thresholds:limits,checked,skipped,flagged,rebases};
}
