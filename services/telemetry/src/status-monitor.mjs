import {readFile} from 'node:fs/promises';
import {createSocket} from 'node:dgram';
import {randomBytes} from 'node:crypto';
import {sanitizeInfrastructure,publicIdentity} from './status-model.mjs';
import {uptimeGroup} from './uptime-store.mjs';

export async function stunBinding(url, timeout = 3000) {
  const endpoint = new URL('http://' + url.slice(5).replace(/^\/\//, ''));
  if (endpoint.username || endpoint.password) throw Error('STUN probe does not accept credentials');
  const host = endpoint.hostname.replace(/^\[|\]$/g, '');
  const socket = createSocket(host.includes(':') ? 'udp6' : 'udp4'), id = randomBytes(12), packet = Buffer.alloc(20);
  packet.writeUInt16BE(1, 0); packet.writeUInt32BE(0x2112a442, 4); id.copy(packet, 8);
  return new Promise(resolve => {
    let finished = false;
    const done = ok => {if (finished) return; finished = true; clearTimeout(timer); try {socket.close();} catch {} resolve(ok);};
    const timer = setTimeout(() => done(false), timeout);
    socket.once('error', () => done(false));
    socket.on('message', message => {if (message.length >= 20 && message.readUInt16BE(0) === 0x101 && message.readUInt32BE(4) === 0x2112a442 && message.subarray(8,20).equals(id)) done(true);});
    socket.send(packet, Number(endpoint.port) || 3478, host, error => {if (error) done(false);});
  });
}
export function websocketOpen(url, timeout = 5000) {
  return new Promise(resolve => {
    let socket, finished = false;
    const done = ok => {if (finished) return; finished = true; clearTimeout(timer); socket?.close(); resolve(ok);};
    const timer = setTimeout(() => done(false), timeout);
    try {socket = new WebSocket(url); socket.addEventListener('open', () => done(true)); socket.addEventListener('error', () => done(false));} catch {done(false);}
  });
}
/** Probe URLs and credentials are deployment-owned and never exposed publicly. */
export function createStatusMonitor({targets = [], infrastructureUrl, fetchImpl = fetch, clock = Date.now} = {}) {
  if (!Array.isArray(targets) || targets.length > 24) throw Error('Invalid status probe targets');
  const ids = new Set(), publicIds = new Set();
  for (const t of targets) {
    if (!/^[a-z0-9-]{1,40}$/.test(t.id) || ids.has(t.id) || typeof t.label !== 'string' || !t.label || t.label.length > 80 ||
      !['site','signaling','websocket','turn','spectator','resource'].includes(t.kind) || !['site','download','netplay','turn'].includes(uptimeGroup(t)) ||
      !['http:','https:','ws:','wss:','stun:'].includes(new URL(t.url).protocol)) throw Error('Invalid status probe target');
    const identity=publicIdentity(t);
    if(publicIds.has(identity.id))throw Error('Duplicate public status identity');
    ids.add(t.id);publicIds.add(identity.id);
  }
  let services = [], infrastructure = null, checkedAt = null, pending = null;
  function refresh() {
    if (pending) return pending;
    pending = (async () => {
      services = await Promise.all(targets.map(async t => {
        const start = clock(); let ok = false;
        try {
          if (t.url.startsWith('stun:')) ok = await stunBinding(t.url);
          else if (/^wss?:/.test(t.url)) ok = await websocketOpen(t.url);
          else {const r = await fetchImpl(t.url, {headers:t.kind === 'resource' ? {Range:'bytes=0-0'} : {}, signal:AbortSignal.timeout(5000)}); ok = r.ok; await r.body?.cancel();}
        } catch {}
        return {id:t.id, label:t.label, kind:t.kind, status:ok ? 'operational' : 'outage', latencyMs:Math.max(0, clock()-start), checkedAt:new Date(clock()).toISOString(),
          evidence:t.url.startsWith('stun:')?'STUN Binding | UDP 端口可达性':/^wss?:/.test(t.url)?'WebSocket 握手 | 服务连接可达性':t.kind==='resource'?'HTTP Range 下载探测':'HTTP 健康检查 | 服务响应'};
      }));
      if (infrastructureUrl) try {const r = await fetchImpl(infrastructureUrl, {signal:AbortSignal.timeout(3000)}); if (!r.ok) throw Error('unavailable'); infrastructure = sanitizeInfrastructure(await r.json(), clock());}
      catch {if (infrastructure) infrastructure = {...infrastructure, stale:true};}
      checkedAt = new Date(clock()).toISOString();
    })().finally(() => {pending = null;});
    return pending;
  }
  return {refresh, idle:()=>pending, snapshot:({publicOnly=false}={})=>({checkedAt,
    services:services.map((s,i) => ({...s, ...(publicOnly?publicIdentity(targets[i]):{}),
      status:clock()-Date.parse(s.checkedAt)>90000 ? 'stale' : s.status})), infrastructure})};
}
export async function readMonitorConfig(path) {return path ? JSON.parse(await readFile(path, 'utf8')) : {};}
