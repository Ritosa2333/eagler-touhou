import test from 'node:test';
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {createSocket} from 'node:dgram';
import {createHash} from 'node:crypto';
import {mkdtemp,readFile,rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {createUptimeStore} from '../src/uptime-store.mjs';
import {createStatusMonitor,stunBinding} from '../src/status-monitor.mjs';
import {startTelemetryServer,createTelemetryStore} from '../src/netplay-telemetry.mjs';

const targets=[{id:'main',label:'站点页面',kind:'site',url:'https://private.example/health'},
  {id:'download',label:'下载节点',kind:'resource',url:'https://private.example/data'}];
test('history persists across restart, retains empty intervals and deduplicates probes',()=>{
  let now=Date.UTC(2026,9,27,12),store=createUptimeStore({},()=>now);
  const sample=status=>({id:'main',status,checkedAt:new Date(now).toISOString(),latencyMs:43});
  store.record([sample('operational')]);store.record([sample('operational')]);
  now+=30000;store.record([sample('outage')]);
  const view=store.snapshot(targets,[],'24h');
  assert.equal(view.services[0].availability,.5);
  assert.equal(view.services[0].points.reduce((n,p)=>n+p.samples,0),2);
  assert(view.services[0].points.some(p=>p.availability===null));
  store=createUptimeStore(JSON.parse(JSON.stringify(store.export())),()=>now);
  assert.equal(store.snapshot(targets,[],'7d').services[0].availability,.5);
  assert.equal(store.snapshot(targets,[],'90d').services[0].availability,.5);
  assert.deepEqual(store.snapshot(targets,[],'24h',{showDownloads:false}).services.map(s=>s.id),['main']);
  now+=91*86400000;
  assert.equal(store.snapshot(targets,[],'90d').services[0].availability,null);
  assert.equal(Object.keys(store.export().entries.main.hours).length,0);
});

test('real HTTP Range and WebSocket checks report availability, then age to unknown',async()=>{
  let range=null,now=Date.now();const sockets=new Set();
  const server=createServer((req,res)=>{range=req.headers.range??range;res.writeHead(req.url==='/bad'?503:200).end('x');});
  server.on('upgrade',(req,socket)=>{
    sockets.add(socket);socket.on('error',()=>{});socket.on('close',()=>sockets.delete(socket));
    const key=createHash('sha1').update(req.headers['sec-websocket-key']+'258EAFA5-E914-47DA-95CA-C5AB0DC85B11').digest('base64');
    socket.write('HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: '+key+'\r\n\r\n');
    socket.on('data',()=>socket.end(Buffer.from([0x88,0])));
  });
  await new Promise(r=>server.listen(0,'127.0.0.1',r));
  const base='http://127.0.0.1:'+server.address().port;
  try {
    const monitor=createStatusMonitor({clock:()=>now,targets:[{...targets[0],url:base+'/'},{...targets[1],url:base+'/data'},
      {id:'ws',label:'联机',kind:'websocket',url:base.replace('http:','ws:')+'/'},{id:'bad',label:'中断',kind:'site',url:base+'/bad'}]});
    await monitor.refresh();assert.equal(range,'bytes=0-0');
    assert.deepEqual(monitor.snapshot().services.map(s=>s.status),['operational','operational','operational','outage']);
    assert(!JSON.stringify(monitor.snapshot()).includes('127.0.0.1'));
    now+=90001;assert(monitor.snapshot().services.every(s=>s.status==='stale'));
  } finally {for(const s of sockets)s.destroy();await new Promise(r=>server.close(r));}
});

test('STUN Binding validates its transaction and handles an unreachable target',async()=>{
  const socket=createSocket('udp4');await new Promise(r=>socket.bind(0,'127.0.0.1',r));
  socket.on('message',(request,peer)=>{const response=Buffer.from(request);response.writeUInt16BE(0x101,0);socket.send(response,peer.port,peer.address);});
  const port=socket.address().port;
  try {assert.equal(await stunBinding('stun:127.0.0.1:'+port,200),true);}
  finally {await new Promise(r=>socket.close(r));}
  assert.equal(await stunBinding('stun:127.0.0.1:'+port,40),false);
});

test('uptime HTTP API hides downloads by configuration and preserves history on disk',async()=>{
  const directory=await mkdtemp(join(tmpdir(),'eagler-uptime-')),stateFile=join(directory,'state.json');
  const config={targets,showDownloads:false,fetchImpl:async()=>new Response('ok')};let service;
  try {
    service=await startTelemetryServer({port:0,origin:'https://site.example',stateFile,monitorConfig:config});
    let base='http://127.0.0.1:'+service.server.address().port;
    const response=await fetch(base+'/uptime/summary?range=24h'),data=await response.json();
    assert.equal(response.status,200);assert.deepEqual(data.services.map(s=>s.id),['main']);
    assert.equal(data.services[0].availability,1);assert.equal(data.services[0].points.length,48);
    assert(!JSON.stringify(data).includes('private.example'));
    assert.equal((await fetch(base+'/uptime/summary?range=constructor')).status,400);
    const moved=await fetch(base+'/netplay-stats/',{redirect:'manual'});assert.equal(moved.status,308);
    assert.equal(moved.headers.get('location'),'../uptime/');
    const summary=await(await fetch(base+'/netplay-stats/summary')).json();
    assert(summary.monitor);assert(summary.nodes);assert(summary.capacity);
    await service.close();service=null;
    const saved=JSON.parse(await readFile(stateFile,'utf8'));assert(saved.uptime.entries.main.hours);
    service=await startTelemetryServer({port:0,origin:'https://site.example',stateFile,monitorConfig:config});
    base='http://127.0.0.1:'+service.server.address().port;
    assert.equal((await(await fetch(base+'/uptime/summary?range=90d')).json()).services[0].availability,1);
  } finally {await service?.close();await rm(directory,{recursive:true,force:true});}
});

test('deployed diagnostic metrics and events remain aggregate-only',()=>{
  const store=createTelemetryStore();
  assert(store.accept({version:1,id:'independent-client-123456',seq:1,product:'th08mp',players:2,role:'player',automated:false,seconds:0,route:'direct',links:[],
    diagnostics:{rttMs:43,phase:'running',privateAddress:'not-retained'},events:['connect-ready','private-event']}));
  const summary=store.snapshot(),day=Object.values(summary.days)[0];
  assert.equal(day.total.metrics.rttMs.sum,43);assert.equal(day.total.events['connect-ready'],1);
  assert(!JSON.stringify(summary).includes('not-retained'));assert(!JSON.stringify(summary).includes('independent-client'));
});

test('public aliases preserve private-keyed history and require authentication for node details',async()=>{
  const token='administrator-token-12345678901234567890',nodeToken='node-report-token-12345678901234567890';
  const target={id:'private-region',label:'Private Region',publicId:'turn-n04',publicLabel:'TURN N04',kind:'site',url:'https://198.51.100.9/private'};
  const now=Date.now(),history=createUptimeStore({},()=>now);
  history.record([{id:target.id,status:'operational',latencyMs:4,checkedAt:new Date(now).toISOString()}]);
  const restored=createUptimeStore(JSON.parse(JSON.stringify(history.export())),()=>now);
  assert.equal(restored.snapshot([target],[]).services[0].id,'turn-n04');
  assert.equal(restored.snapshot([target],[]).services[0].availability,1);
  const config={targets:[target],fetchImpl:async()=>new Response('ok'),adminToken:token,
    loadNodes:[{id:'private-node',label:'Private Provider',publicId:'load-n04',publicLabel:'N04',token:nodeToken}]};
  const service=await startTelemetryServer({port:0,origin:'https://site.example',monitorConfig:config});
  const base='http://127.0.0.1:'+service.server.address().port;
  try {
    const node={boot:'12345678-1234-1234',uptime:100,cpuTotal:100,cpuIdle:80,cores:2,load:[0,0,0],
      memoryTotal:1024,memoryAvailable:512,rxBytes:1,txBytes:2,diskTotal:4096,diskAvailable:2048,tcpEstablished:3};
    assert.equal((await fetch(base+'/netplay-stats/nodes/private-node',{method:'POST',headers:{'Content-Type':'application/json',Authorization:'Bearer '+nodeToken},body:JSON.stringify(node)})).status,204);
    for(const path of ['/uptime/summary','/netplay-stats/summary']){
      const response=await fetch(base+path),text=await response.text();assert.equal(response.status,200);
      for(const hidden of ['private-region','Private Region','private-node','Private Provider','198.51.100.9','memoryTotal','diskTotal','cores',token,nodeToken])assert(!text.includes(hidden),hidden);
      assert(text.includes('turn-n04'));assert(text.includes('TURN N04'));
    }
    const publicData=await(await fetch(base+'/netplay-stats/summary')).json();
    assert.deepEqual(Object.keys(publicData.nodes[0]).sort(),['checkedAt','id','label','status']);
    assert.equal(publicData.nodes[0].id,'load-n04');assert.equal(publicData.nodes[0].status,'operational');
    const privateResponse=await fetch(base+'/netplay-stats/summary',{headers:{Authorization:'Bearer '+token}}),privateData=await privateResponse.json();
    assert.equal(privateResponse.status,200);assert.equal(privateResponse.headers.get('cache-control'),'no-store');
    assert.equal(privateData.nodes[0].id,'private-node');assert.equal(privateData.nodes[0].publicId,'load-n04');assert.equal(privateData.nodes[0].memoryTotal,1024);
    assert.equal(privateData.monitor.services[0].id,'private-region');
    assert.equal((await fetch(base+'/netplay-stats/summary',{headers:{Authorization:'Bearer '+nodeToken}})).status,403);
    assert.equal((await fetch(base+'/netplay-stats/summary',{headers:{Authorization:'Bearer invalid'}})).status,403);
  } finally {await service.close();}
});

test('invalid and duplicate public identities are rejected before monitoring starts',()=>{
  assert.throws(()=>createStatusMonitor({targets:[{...targets[0],publicId:'unsafe/url'}]}));
  assert.throws(()=>createStatusMonitor({targets:targets.map(t=>({...t,publicId:'duplicate'}))}));
});
