// Public aggregates only. Never persist client IDs, room codes or error text.
export function publicIdentity(config) {
  const id=config.publicId??config.id,label=config.publicLabel??config.label;
  if(!/^[a-z0-9-]{1,40}$/.test(id)||typeof label!=='string'||!label||label.length>80)
    throw Error('Invalid public status identity');
  return {id,label};
}
export const metricLimits = Object.freeze({rttMs:60000,jitterMs:60000,connectMs:300000,
  configuredDelay:120,actualDelay:120,rollback:1e9,resimulated:1e9,frameGap:100000,
  loadMs:600000});
export function sanitizeDiagnostics(raw) {
  if(!raw || typeof raw!=='object' || Array.isArray(raw))return {};
  const out={};
  for(const [key,max] of Object.entries(metricLimits))
    if(typeof raw[key]==='number'&&Number.isFinite(raw[key])&&raw[key]>=0&&raw[key]<=max)out[key]=raw[key];
  for(const [key,values] of Object.entries({phase:['loading','prepared','connecting','running','error'],
    protocol:['udp','tcp','mixed','unknown'],failure:['connection','runtime','resource','version','desync'],
    delayMode:['manual','auto','rollback']}))if(values.includes(raw[key]))out[key]=raw[key];
  return out;
}
// Bounded histograms permit merging across products and time ranges without
// retaining an unbounded list of user samples. Percentiles are bucket ceilings.
export const buckets=[0,1,2,4,8,16,32,64,128,256,512,1000,2000,5000,10000,30000,60000,300000,600000,1e9];
export function addMetrics(target,diagnostics) {
  target.metrics??={};target.phases??={};target.failures??={};target.protocols??={};target.delayModes??={};
  for(const [key,value] of Object.entries(diagnostics)) {
    if(key in metricLimits){
      const entry=target.metrics[key]??={count:0,sum:0,max:0,buckets:Array(buckets.length).fill(0)};
      entry.count++;entry.sum+=value;entry.max=Math.max(entry.max,value);
      entry.buckets[buckets.findIndex(limit=>value<=limit)]++;
    }else if(key==='phase')target.phases[value]=(target.phases[value]??0)+1;
    else if(key==='protocol')target.protocols[value]=(target.protocols[value]??0)+1;
    else if(key==='failure')target.failures[value]=(target.failures[value]??0)+1;
    else if(key==='delayMode')target.delayModes[value]=(target.delayModes[value]??0)+1;
  }
}
export function addEvent(target,event){
  target.events??={};target.events[event]=(target.events[event]??0)+1;
}
export const events=['connect-attempt','connect-ready','disconnect','reconnected','load-attempt','load-ready','load-failed','version-mismatch','desync'];
export function safeEvent(value){return events.includes(value)?value:null;}
export function historyBucket(at){return new Date(Math.floor(at/300000)*300000).toISOString();}

export function sanitizeInfrastructure(value,now=Date.now()) {
  if(!value||typeof value!=='object')return null;
  const checked=Date.parse(value.checkedAt);
  if(!Number.isFinite(checked)||checked>now+60000)return null;
  const counts={};
  for(const product of Object.keys(value.products??{})){
    if(!/^th\d{2,3}mp$/.test(product))continue;
    const source=value.products[product],row={};
    for(const key of ['rooms','waiting','playing','players','spectators'])
      if(Number.isSafeInteger(source[key])&&source[key]>=0&&source[key]<=100000)row[key]=source[key];
    counts[product]=row;
  }
  return {checkedAt:new Date(checked).toISOString(),stale:now-checked>90000,products:counts};
}
