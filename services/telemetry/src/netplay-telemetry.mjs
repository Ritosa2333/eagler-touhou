import { createServer } from 'node:http';
import { readFile, writeFile, rename, mkdir } from 'node:fs/promises';
import { dirname } from 'node:path';
import { pathToFileURL } from 'node:url';
import { isMultiplayerProductId } from '../../../common/contracts/product-catalog.mjs';
import {sanitizeDiagnostics,addMetrics,addEvent,safeEvent,historyBucket} from './status-model.mjs';
import {createStatusMonitor,readMonitorConfig} from './status-monitor.mjs';
import {createUptimeStore, UPTIME_RANGES} from './uptime-store.mjs';
import {createNodeLoads,matchesBearer} from './node-load.mjs';

const routes = ['direct', 'turn', 'mixed', 'websocket', 'unknown'];
export function validateSample(value) {
  if (!value || value.version !== 1 || typeof value.id !== 'string' || !/^[a-z0-9-]{16,64}$/i.test(value.id) ||
      !Number.isSafeInteger(value.seq) || value.seq < 1 || value.seq > 1e7 ||
      !isMultiplayerProductId(value.product) || ![2,3].includes(value.players) ||
      !['player','spectator'].includes(value.role) || !routes.includes(value.route) ||
      typeof value.automated !== 'boolean' ||
      !Number.isFinite(value.seconds) || value.seconds < 0 || value.seconds > 90 ||
      !Array.isArray(value.links) || value.links.length > value.players - 1) return null;
  const links = [];
  for (const link of value.links) {
    if (!['direct','turn'].includes(link.path) || !Number.isFinite(link.seconds) || link.seconds !== value.seconds || !link.seconds ||
        ['tx','rx','txPackets','rxPackets'].some(k => !Number.isSafeInteger(link[k]) || link[k] < 0 || link[k] > 1e8)) return null;
    links.push({path:link.path,seconds:link.seconds,tx:link.tx,rx:link.rx,txPackets:link.txPackets,rxPackets:link.rxPackets});
  }
  if ((value.route === 'websocket' || value.role === 'spectator') && links.length) return null;
  if (value.role === 'spectator' && value.route !== 'websocket') return null;
  if (value.seconds > 0 && ['direct','turn','mixed'].includes(value.route)) {
    const paths=new Set(links.map(link=>link.path));
    if (links.length !== value.players-1 || (value.route==='mixed' ? paths.size!==2 : paths.size!==1 || !paths.has(value.route))) return null;
  }
  return {version:1,id:value.id,seq:value.seq,product:value.product,players:value.players,role:value.role,
    automated:value.automated,seconds:value.seconds,route:value.route,links,
    statusReported:!!value.diagnostics&&typeof value.diagnostics==='object',
    diagnostics:sanitizeDiagnostics(value.diagnostics),events:Array.isArray(value.events)?[...new Set(value.events.map(safeEvent).filter(Boolean))].slice(0,9):[]};
}
const blankLink = () => ({seconds:0,tx:0,rx:0,txPackets:0,rxPackets:0});
const blank = () => ({samples:0,sessions:0,playerSeconds:0,spectatorSeconds:0,
  routeSeconds:Object.fromEntries(routes.map(r=>[r,0])),direct:blankLink(),turn:blankLink()});
function add(target, value) {
  target.samples += 1;
  if(value.statusReported)target.statusSamples=(target.statusSamples??0)+1;
  addMetrics(target,value.diagnostics);
  for(const event of value.events)addEvent(target,event);
  target[value.role === 'player' ? 'playerSeconds' : 'spectatorSeconds'] += value.seconds;
  if (value.role === 'player') target.routeSeconds[value.route] += value.seconds;
  for(const link of value.links)if(link.seconds>=10){target.peakPayloadBytesPerSecond=Math.max(target.peakPayloadBytesPerSecond??0,link.tx/link.seconds);target.peakPacketsPerSecond=Math.max(target.peakPacketsPerSecond??0,link.txPackets/link.seconds);}
  for (const link of value.links) for (const key of Object.keys(target[link.path])) target[link.path][key] += link[key];
}
export function capacityModel(days) {
  let payloadBytesPerSecond=15250,packetsPerSecond=143;
  for(const day of Object.values(days))for(const product of Object.values(day.products))for(const path of ['direct','turn']){
    const link=product[path];if(link.seconds>0){payloadBytesPerSecond=Math.max(payloadBytesPerSecond,link.tx/link.seconds,product.peakPayloadBytesPerSecond??0);packetsPerSecond=Math.max(packetsPerSecond,link.txPackets/link.seconds,product.peakPacketsPerSecond??0);}
  }
  const headerBytesPerPacket=64,turnHops=2,headroom=1.5;
  const unitMbps=Math.ceil((payloadBytesPerSecond+packetsPerSecond*headerBytesPerPacket)*8/1e6*turnHops*headroom*10)/10;
  return {payloadBytesPerSecond,packetsPerSecond,headerBytesPerPacket,turnHops,headroom,unitMbps,
    basis:'Highest observed 15-second per-peer rate or per-product average or TH11 baseline. Conservative egress budget per player per opponent, not billed traffic.'};
}
export function createTelemetryStore(initial = {}, clock = Date.now) {
  const days = initial.days ?? {}, history=initial.history??{}, sessions = new Map();
  const startedAt = initial.startedAt ?? new Date(clock()).toISOString();
  let revision = 0, savedRevision = 0, pruneAt = 0;
  function accept(raw) {
    const value = validateSample(raw); if (!value) return false;
    const at=clock();if(at>=pruneAt){prune();pruneAt=at+30000;}const old=sessions.get(value.id);
    if (old && (value.seq <= old.seq || at-old.at < 1000 || old.product !== value.product || old.role !== value.role || old.automated!==value.automated)) return false;
    // Never trust a client to add more time than has passed at the collector.
    if (old && value.seconds > (at-old.at)/1000 + 2) return false;
    if (!old && value.seconds > 0) value.seconds=0, value.links=[];
    if (!old && sessions.size >= 20000) return false;
    sessions.set(value.id,{seq:value.seq,at,product:value.product,role:value.role,route:value.route,automated:value.automated});
    const day=new Date(at).toISOString().slice(0,10);
    const entry=days[day]??= {total:blank(),products:{}};
    if(value.automated){const automation=entry.automation??=blank();if(!old)automation.sessions++;add(automation,value);revision++;return true;}
    const product=entry.products[value.product]??=blank();
    if (!old) entry.total.sessions++, product.sessions++;
    add(entry.total,value);add(product,value);revision++;
    const bucket=history[historyBucket(at)]??={total:blank(),products:{}};
    add(bucket.total,value);add(bucket.products[value.product]??=blank(),value);
    return true;
  }
  function prune() {
    const now=clock();for(const [id,s] of sessions)if(now-s.at>180000)sessions.delete(id);
    const cutoff=new Date(now-6*86400000).toISOString().slice(0,10);
    for(const day of Object.keys(days))if(day<cutoff)delete days[day];
    for(const key of Object.keys(history))if(Date.parse(key)<now-7*86400000)delete history[key];
  }
  function snapshot() {
    prune(); const now=clock(), active={players:0,spectators:0,routes:Object.fromEntries(routes.map(r=>[r,0]))};
    active.products={};
    for(const s of sessions.values())if(!s.automated&&now-s.at<45000){
      const p=active.products[s.product]??={players:0,spectators:0};p[s.role==='spectator'?'spectators':'players']++;
      if(s.role==='spectator')active.spectators++;else active.players++,active.routes[s.route]++;
    }
    return {schema:'eagler-netplay-telemetry/1',startedAt,updatedAt:new Date(now).toISOString(),
      coverage:'Reporting clients only; server room census is separate. Not unique people.',active,days,history,capacity:capacityModel(days)};
  }
  return {accept,snapshot,isDirty:()=>revision!==savedRevision,revision:()=>revision,markSaved:r=>{savedRevision=r;}};
}

export async function startTelemetryServer({port=18153,host='127.0.0.1',origin,stateFile,monitorConfig={}}={}) {
  if(monitorConfig.adminToken!=null&&(typeof monitorConfig.adminToken!=='string'||monitorConfig.adminToken.length<32||monitorConfig.adminToken.length>256))
    throw Error('Invalid status administrator token');
  let initial={};if(stateFile)try{initial=JSON.parse(await readFile(stateFile,'utf8'));}catch(e){if(e.code!=='ENOENT')throw e;}
  const store=createTelemetryStore(initial);
  const uptime=createUptimeStore(initial.uptime);
  const nodeLoads=createNodeLoads(monitorConfig.loadNodes??[]);
  const monitor=createStatusMonitor(monitorConfig);
  const refresh=async()=>{await monitor.refresh();uptime.record(monitor.snapshot().services);};
  await refresh();
  const probeTimer=setInterval(()=>void refresh().catch(e=>console.error('Status probe failed:',e.code)),30000);probeTimer.unref();
  let rateAt=0,rateCount=0;
  const server=createServer(async(req,res)=>{
    res.setHeader('Cache-Control','no-store');res.setHeader('X-Content-Type-Options','nosniff');
    if(req.method==='GET'&&req.url==='/netplay-stats/summary'){
      const authorization=req.headers.authorization;
      const admin=matchesBearer(authorization,monitorConfig.adminToken);
      if(authorization&&!admin){res.writeHead(403).end();return;}
      res.setHeader('Content-Type','application/json; charset=utf-8');
      res.end(JSON.stringify({...store.snapshot(),monitor:monitor.snapshot({publicOnly:!admin}),nodes:nodeLoads.snapshot({publicOnly:!admin})}));return;
    }
    const url=new URL(req.url??'/', 'http://loopback');
    if(req.method==='GET'&&url.pathname==='/uptime/summary'){
      const range=url.searchParams.get('range')??'24h';
      if(!Object.hasOwn(UPTIME_RANGES,range)){res.writeHead(400).end();return;}
      res.setHeader('Content-Type','application/json; charset=utf-8');
      res.end(JSON.stringify(uptime.snapshot(monitorConfig.targets??[],monitor.snapshot().services,range,{showDownloads:monitorConfig.showDownloads!==false})));return;
    }
    if(req.method==='GET'&&['/netplay-stats','/netplay-stats/'].includes(url.pathname)){res.writeHead(308,{Location:url.pathname.endsWith('/')?'../uptime/':'./uptime/'}).end();return;}
    const nodeMatch=/^\/netplay-stats\/nodes\/([a-z0-9-]{1,40})$/.exec(req.url??'');
    if(req.method!=='POST'||req.url!=='/netplay-stats/sample'&&!nodeMatch){res.writeHead(404).end();return;}
    const second=Math.floor(Date.now()/1000);if(second!==rateAt){rateAt=second;rateCount=0;}if(++rateCount>500){res.writeHead(429).end();return;}
    if((!nodeMatch&&req.headers.origin!==origin)||req.headers['content-type']!=='application/json'){res.writeHead(403).end();return;}
    let bytes=0,chunks=[];
    try{for await(const chunk of req){bytes+=chunk.length;if(bytes>4096){res.writeHead(413).end();req.destroy();return;}chunks.push(chunk);}const value=JSON.parse(Buffer.concat(chunks).toString('utf8'));
      if(nodeMatch){if(!nodeLoads.accept(nodeMatch[1],req.headers.authorization,value)){res.writeHead(403).end();return;}}
      else if(!store.accept(value)){res.writeHead(400).end();return;}res.writeHead(204).end();
    }catch{if(!res.headersSent)res.writeHead(400).end();}
  });
  server.requestTimeout=10000;server.headersTimeout=5000;
  async function persist(){if(!stateFile||!store.isDirty()&&!uptime.isDirty())return;const history=uptime.export(),revision=store.revision(),uptimeRevision=uptime.revision(),data=JSON.stringify({...store.snapshot(),uptime:history});await mkdir(dirname(stateFile),{recursive:true});await writeFile(stateFile+'.next',data,{mode:0o600});await rename(stateFile+'.next',stateFile);store.markSaved(revision);uptime.markSaved(uptimeRevision);}
  let saving=null;const timer=setInterval(()=>{if(saving)return;saving=persist().catch(e=>console.error('Telemetry persistence failed:',e.code)).finally(()=>{saving=null;});},15000);timer.unref();
  await new Promise(resolve=>server.listen(port,host,resolve));
  return {server,store,monitor,uptime,refresh,close:async()=>{clearInterval(timer);clearInterval(probeTimer);await new Promise(resolve=>server.close(resolve));await monitor.idle();uptime.record(monitor.snapshot().services);await saving;await persist();}};
}
if(process.argv[1]&&import.meta.url===pathToFileURL(process.argv[1]).href){
  const service=await startTelemetryServer({port:Number(process.env.EAGLER_NETPLAY_TELEMETRY_PORT||18153),
    origin:process.env.EAGLER_NETPLAY_TELEMETRY_ORIGIN,stateFile:process.env.EAGLER_NETPLAY_TELEMETRY_STATE,
    monitorConfig:await readMonitorConfig(process.env.EAGLER_NETPLAY_STATUS_CONFIG)});
  console.log('Anonymous netplay telemetry listening on loopback');
  for(const signal of ['SIGTERM','SIGINT'])process.on(signal,()=>{void service.close().then(()=>process.exit(0));});
}
