export const DEFAULT_STATUS_SEVERITY = Object.freeze({
  nonfinite: 6,
  'non-idempotent': 6,
  'endpoint-mismatch': 5,
  'downstream-held': 5,
  'missing-interpolation': 4,
  'held-review': 3,
  responsive: 2,
});

export const defaultSeverity = status => DEFAULT_STATUS_SEVERITY[status] || 0;

// Collapse repeated object instances without losing the original window and
// object keys used to inspect source evidence.
export function groupFindings(reports,{severityOf=defaultSeverity}={}) {
  const groups=new Map();
  for(const report of reports)for(const object of report.objects)for(const field of object.fields){
    if(severityOf(field.status)<3)continue;
    const key=[object.ownerId,object.anm,object.script,object.kind,field.id,field.status].join(':');
    let group=groups.get(key);
    if(!group){group={key,owner:object.owner,ownerId:object.ownerId,anm:object.anm,script:object.script,kind:object.kind,field:field.id,label:field.label,status:field.status,severity:severityOf(field.status),
      firstTick:report.tick,lastTick:report.tick,observations:0,instances:new Set(),windows:new Set(),maxMotion:0,exampleKey:object.key};groups.set(key,group);}
    group.firstTick=Math.min(group.firstTick,report.tick);group.lastTick=Math.max(group.lastTick,report.tick);group.observations++;group.instances.add(object.key);group.windows.add(report.tick);
    if((field.motion||0)>group.maxMotion){group.maxMotion=field.motion;group.exampleKey=object.key;}
  }
  return [...groups.values()].map(group=>({...group,instanceCount:group.instances.size,windowCount:group.windows.size,exampleKeys:[...group.instances].slice(0,8),instances:undefined,windows:undefined}))
    .sort((a,b)=>b.severity-a.severity||b.windowCount-a.windowCount||b.instanceCount-a.instanceCount||a.key.localeCompare(b.key));
}

export function compactReport(report,{maxObjects=8192,images=true}={}) {
  return {...report,images:images?report.images:undefined,totalObjects:report.objects.length,detailsTruncated:report.objects.length>maxObjects,
    objects:report.objects.slice(0,maxObjects).map(object=>({...object,fields:object.fields.filter(field=>field.status!=='static')}))};
}

// Preserve full-window counters when stored object details have been bounded.
export function mergeIssueGroups(reports,{group=groupFindings}={}) {
  const groups=new Map();
  for(const report of reports)for(const finding of report.issueGroups||group([report])){
    let entry=groups.get(finding.key);
    if(!entry){entry={...finding,observations:0,peakInstances:0,windowCount:0,exampleWindows:[]};delete entry.instanceCount;groups.set(finding.key,entry);}
    entry.observations+=finding.observations;entry.peakInstances=Math.max(entry.peakInstances,finding.instanceCount);
    entry.windowCount++;entry.firstTick=Math.min(entry.firstTick,finding.firstTick);entry.lastTick=Math.max(entry.lastTick,finding.lastTick);entry.maxMotion=Math.max(entry.maxMotion,finding.maxMotion);
    if(entry.exampleWindows.length<4)entry.exampleWindows.push({tick:report.tick,key:finding.exampleKey});
  }
  return [...groups.values()].sort((a,b)=>b.severity-a.severity||b.windowCount-a.windowCount||a.key.localeCompare(b.key));
}
