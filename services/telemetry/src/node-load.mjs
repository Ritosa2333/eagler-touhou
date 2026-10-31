import {createHash,timingSafeEqual} from 'node:crypto';
import {publicIdentity} from './status-model.mjs';
const finite=(n,max=1e16)=>typeof n==='number'&&Number.isFinite(n)&&n>=0&&n<=max;
export function validateNodeSample(value){
  if(!value||typeof value!=='object'||typeof value.boot!=='string'||!/^[a-f0-9-]{16,64}$/i.test(value.boot))return null;
  for(const key of ['uptime','cpuTotal','cpuIdle','memoryTotal','memoryAvailable','rxBytes','txBytes','diskTotal','diskAvailable','tcpEstablished'])if(!finite(value[key]))return null;
  if(!Number.isInteger(value.cores)||value.cores<1||value.cores>4096||!Array.isArray(value.load)||value.load.length!==3||value.load.some(n=>!finite(n,100000)))return null;
  if(!value.memoryTotal||value.memoryAvailable>value.memoryTotal||value.diskAvailable>value.diskTotal||value.cpuIdle>value.cpuTotal)return null;
  return Object.fromEntries(['boot','uptime','cpuTotal','cpuIdle','cores','load','memoryTotal','memoryAvailable','rxBytes','txBytes','diskTotal','diskAvailable','tcpEstablished'].map(k=>[k,value[k]]));
}
const digest=value=>createHash('sha256').update(value).digest();
export function matchesBearer(authorization,token){
  return typeof authorization==='string'&&authorization.length<=512&&typeof token==='string'&&token.length>=32&&
    timingSafeEqual(digest(authorization),digest('Bearer '+token));
}
export function createNodeLoads(config=[],clock=Date.now){
  if(!Array.isArray(config)||config.length>24)throw Error('Invalid load node configuration');
  const entries=new Map(),publicIds=new Set();
  for(const node of config){
    if(!/^[a-z0-9-]{1,40}$/.test(node.id)||typeof node.label!=='string'||node.label.length>80||typeof node.token!=='string'||node.token.length<32||entries.has(node.id))throw Error('Invalid load node configuration');
    const identity=publicIdentity(node);
    if(publicIds.has(identity.id))throw Error('Duplicate public load node identity');
    publicIds.add(identity.id);
    entries.set(node.id,{config:node,sample:null,at:0,cpu:null,rx:null,tx:null,history:[]});
  }
  function accept(id,authorization,raw){
    const entry=entries.get(id);
    if(!entry||!matchesBearer(authorization,entry.config.token))return false;
    const sample=validateNodeSample(raw),now=clock();if(!sample||entry.at&&now-entry.at<10000)return false;
    const old=entry.sample,elapsed=old?sample.uptime-old.uptime:0;
    entry.cpu=entry.rx=entry.tx=null;
    if(old&&old.boot===sample.boot&&elapsed>=10&&elapsed<=120&&sample.cpuTotal>old.cpuTotal&&sample.cpuIdle>=old.cpuIdle){
      entry.cpu=Math.max(0,Math.min(100,100*(1-(sample.cpuIdle-old.cpuIdle)/(sample.cpuTotal-old.cpuTotal))));
      if(sample.rxBytes>=old.rxBytes)entry.rx=(sample.rxBytes-old.rxBytes)*8/elapsed/1e6;
      if(sample.txBytes>=old.txBytes)entry.tx=(sample.txBytes-old.txBytes)*8/elapsed/1e6;
    }
    entry.sample=sample;entry.at=now;
    entry.history.push({at:new Date(now).toISOString(),cpuPercent:entry.cpu,rxMbps:entry.rx,txMbps:entry.tx});
    entry.history=entry.history.filter(p=>now-Date.parse(p.at)<3600000).slice(-120);
    return true;
  }
  function snapshot({publicOnly=false}={}){return [...entries.values()].map(({config,sample,at,cpu,rx,tx,history})=>{
    const stale=!!at&&clock()-at>90000;
    if(publicOnly)return {...publicIdentity(config),status:!sample?'unknown':stale?'stale':'operational',checkedAt:at?new Date(at).toISOString():null};
    const identity=publicIdentity(config);
    return {id:config.id,label:config.label,publicId:identity.id,publicLabel:identity.label,
      status:!sample?'unknown':stale?'stale':'operational',checkedAt:at?new Date(at).toISOString():null,
      cores:sample?.cores??null,cpuPercent:stale?null:cpu,load:sample?.load??null,
      memoryTotal:sample?.memoryTotal??null,memoryUsed:sample?sample.memoryTotal-sample.memoryAvailable:null,
      diskTotal:sample?.diskTotal??null,diskUsed:sample?sample.diskTotal-sample.diskAvailable:null,
      rxMbps:stale?null:rx,txMbps:stale?null:tx,tcpEstablished:sample?.tcpEstablished??null,history};
  });}
  return {accept,snapshot};
}
