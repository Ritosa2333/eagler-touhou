import assert from 'node:assert/strict';
import {once} from 'node:events';
import {createServer} from 'node:http';
import {mkdtemp, rm, writeFile} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {fileURLToPath} from 'node:url';
import {test} from 'node:test';
import {WebSocket, WebSocketServer} from 'ws';
import {startProxy} from '../deploy/turn-pool-proxy.mjs';

test('TURN pool augments ordinary WebRTC and leaves Swarm ICE policy intact', async () => {
  const directory = await mkdtemp(join(tmpdir(), 'eagler-relay-pool-'));
  const poolFile = join(directory, 'pool.json');
  const swarm = {urls:['turn:swarm.test:3478'], username:'swarm', credential:'fixture'};
  const stun = {urls:['stun:stun.test:3478']};
  const ordinary = {urls:['turn:ordinary.test:3478'], username:'ordinary', credential:'fixture'};
  const cloudflare = {urls:['turn:cf.test:3478'], username:'cf', credential:'fixture'};
  const backup = 'turn:backup.test:3478';
  let brokerCalls = 0;
  const broker = createServer((_request,response) => {
    ++brokerCalls;
    response.end(JSON.stringify({iceServers:[cloudflare]}));
  }).listen(0,'127.0.0.1');
  const upstream = new WebSocketServer({host:'127.0.0.1',port:0});
  upstream.on('connection', (peer, request) => {
    const mode = new URL(request.url,'http://127.0.0.1').searchParams.get('mode');
    const iceServers = mode === 'native' ? [stun] : [stun, mode === 'swarm' ? swarm : ordinary];
    peer.send(JSON.stringify({type:'peers',iceServers,...(mode !== 'ordinary' ? {turnMode:'swarm'} : {})}));
  });
  await Promise.all([once(broker,'listening'),once(upstream,'listening')]);
  await writeFile(poolFile, JSON.stringify({urls:[backup], sharedSecret:'fixture', backups:[]}));
  const proxy = startProxy({
    wsPackage:fileURLToPath(new URL('../package.json',import.meta.url)), proxyPort:0,
    upstreamPort:upstream.address().port, brokerPort:broker.address().port, brokerToken:'fixture',
    poolFile, maxPayload:65536, maxBuffered:65536,
  });
  async function peers(mode) {
    const ws = new WebSocket(`ws://127.0.0.1:${proxy.address().port}/eagler-netplay/?mode=${mode}`, {
      headers:{'X-Real-IP':'198.51.100.1','X-Eagler-CN':'0'},
    });
    try {
      const [message] = await once(ws,'message',{signal:AbortSignal.timeout(5000)});
      return JSON.parse(message).iceServers;
    } finally {
      const closed = once(ws,'close');
      ws.terminate();
      await closed;
    }
  }
  try {
    await once(proxy,'listening');
    assert.deepEqual(await peers('swarm'), [stun,swarm]);
    assert.deepEqual(await peers('native'), [stun]);
    assert.equal(brokerCalls,0);
    assert.deepEqual((await peers('ordinary')).flatMap(server => server.urls),
      [...cloudflare.urls,...stun.urls,...ordinary.urls,backup]);
    assert.equal(brokerCalls,1);
  } finally {
    await Promise.all([proxy,upstream,broker].map(server => new Promise(resolve => server.close(resolve))));
    await rm(directory,{recursive:true,force:true});
  }
});
