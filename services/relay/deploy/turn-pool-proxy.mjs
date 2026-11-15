import {createHmac} from 'node:crypto';
import {createServer} from 'node:http';
import {readFileSync} from 'node:fs';
import {createRequire} from 'node:module';
import {pathToFileURL} from 'node:url';

export function mergeIce(message, cloudflare) {
  if (message === null || typeof message !== 'object') return message;
  if (Array.isArray(message.iceServers)) message.iceServers = [...cloudflare, ...message.iceServers];
  if (message.roomProbe && Array.isArray(message.roomProbe.iceServers))
    message.roomProbe.iceServers = [...cloudflare, ...message.roomProbe.iceServers];
  return message;
}

export function startProxy(config) {
  const require = createRequire(pathToFileURL(config.wsPackage));
  const {WebSocket, WebSocketServer} = require('ws');
  const server = createServer((_request, response) => {response.writeHead(404); response.end();});
  const wss = new WebSocketServer({noServer:true, maxPayload:config.maxPayload, perMessageDeflate:false, autoPong:false});
  server.on('upgrade', (request, socket, head) => {
    if (!['127.0.0.1','::ffff:127.0.0.1'].includes(request.socket.remoteAddress)
        || !/^\/(eagler-netplay|eagler-netplay-lifecycle-20261025)\//.test(request.url)) {socket.destroy(); return;}
    const address = request.headers['x-real-ip'];
    if (typeof address !== 'string' || address.length > 64) {socket.destroy(); return;}
    const overseas = request.headers['x-eagler-cn'] === '0';
    const upstream = new WebSocket(`ws://127.0.0.1:${config.upstreamPort}${request.url}`, {
      handshakeTimeout:4000, perMessageDeflate:false, maxPayload:config.maxPayload, autoPong:false,
      headers:{'X-Real-IP':address, ...(request.headers.origin ? {Origin:request.headers.origin} : {})},
    });
    let downstream;
    let queue = Promise.resolve();
    let pendingBytes = 0;
    let credentialPromise;
    const credentials = () => credentialPromise ||= fetch(`http://127.0.0.1:${config.brokerPort}/credentials`, {
      method:'POST', headers:{'Content-Type':'application/json','X-Broker-Token':config.brokerToken},
      body:JSON.stringify({address}), signal:AbortSignal.timeout(5000),
    }).then(async response => response.ok ? (await response.json()).iceServers : []).catch(() => []);
    const close = (target, code, reason) => {
      if (target?.readyState === WebSocket.OPEN) target.close(code === 1005 || code === 1006 || code === 1015 ? 1011 : code, reason);
      else if (target?.readyState === WebSocket.CONNECTING) target.terminate();
    };
    upstream.on('open', () => {
      wss.handleUpgrade(request, socket, head, client => {
        downstream = client;
        client.on('message', (data,isBinary) => {
          if (upstream.readyState !== WebSocket.OPEN) return;
          if (upstream.bufferedAmount > config.maxBuffered) {close(client,1013,'upstream busy');return;}
          upstream.send(data,{binary:isBinary});
        });
        client.on('close', (code,reason) => close(upstream,code,reason));
        client.on('error', () => upstream.terminate());
        client.on('ping', data => {if(upstream.readyState === WebSocket.OPEN) upstream.ping(data);});
        client.on('pong', data => {if(upstream.readyState === WebSocket.OPEN) upstream.pong(data);});
      });
    });
    upstream.on('message', (data,isBinary) => {
      pendingBytes += data.length;
      if (pendingBytes > config.maxBuffered) {downstream?.terminate();upstream.terminate();return;}
      // Serialize only upstream messages, keeping binary data and control order intact.
      queue = queue.then(async () => {
        if (downstream?.readyState !== WebSocket.OPEN) return;
        let payload = data;
        if (!isBinary && data.length < 65536) {
          try {
            const value = JSON.parse(data.toString());
            if (value?.turnMode !== 'swarm' &&
                (Array.isArray(value?.iceServers) || Array.isArray(value?.roomProbe?.iceServers))) {
              const entries = value.iceServers || value.roomProbe?.iceServers;
              const active = entries.find(e => e.username && e.credential);
              if (active) {
                const pool = JSON.parse(readFileSync(config.poolFile, 'utf8'));
                for (const node of [pool, ...pool.backups]) {
                  if (entries.some(e => [].concat(e.urls).includes(node.urls[0]))) continue;
                  entries.push({urls:node.urls.filter(url=>!url.startsWith('turns:')), username:active.username, credential:createHmac('sha1',node.sharedSecret).update(active.username).digest('base64')});
                }
              }
              const cf = overseas ? await credentials() : [];
              payload = JSON.stringify(mergeIce(value,cf));
            }
          } catch {}
        }
        if (downstream.readyState !== WebSocket.OPEN) return;
        if (downstream.bufferedAmount > config.maxBuffered) {close(downstream,1013,'client busy');return;}
        downstream.send(payload,{binary:isBinary});
      }).catch(() => {downstream?.terminate();upstream.terminate();}).finally(() => {pendingBytes -= data.length;});
    });
    upstream.on('ping', data => {if(downstream?.readyState === WebSocket.OPEN) downstream.ping(data);});
    upstream.on('pong', data => {if(downstream?.readyState === WebSocket.OPEN) downstream.pong(data);});
    upstream.on('close', (code,reason) => {if (downstream) queue.finally(() => close(downstream,code,reason)); else socket.destroy();});
    upstream.on('error', () => {downstream?.terminate();socket.destroy();});
    socket.on('error', () => upstream.terminate());
    socket.on('close', () => {if (!downstream) upstream.terminate();});
  });
  server.listen(config.proxyPort,'127.0.0.1');
  return server;
}

if (process.argv[1] && import.meta.url === pathToFileURL(process.argv[1]).href)
  startProxy(JSON.parse(readFileSync(process.argv[2],'utf8')));
