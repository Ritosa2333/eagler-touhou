import {isIPv4} from 'node:net';
import {createHmac, randomBytes} from 'node:crypto';
import {WebSocket} from 'ws';

function call(url, name, params) {
  return new Promise(resolve => {
    const socket = new WebSocket(url, {handshakeTimeout: 2500, maxPayload: 8192});
    const timeout = setTimeout(() => socket.terminate(), 3000);
    let result;
    socket.once('close', () => {clearTimeout(timeout); resolve(result);});
    socket.on('error', () => socket.terminate());
    socket.once('open', () => socket.send(JSON.stringify({event:'call', data:{i:1, n:name, p:params}})));
    socket.on('message', raw => {
      let reply;
      try { reply = JSON.parse(raw); } catch { return; }
      if (reply.event !== 'result' || reply.data?.i !== 1) return;
      result = reply.data.r;
      socket.terminate();
    });
  });
}

export async function allocateSwarmPort(url, token, worker) {
  const reply = await call(url, 'new_naive_udp_relay', {access_token:token, worker_id:worker});
  const address = typeof reply?.relay_addr === 'string' ? /^(\S+):(\d{1,5})$/.exec(reply.relay_addr) : null;
  if (!address || !isIPv4(address[1]) || +address[2] < 1 || +address[2] > 65535 ||
      !Number.isInteger(reply.relay_code) || reply.relay_code < 0 || reply.relay_code > 0xffffffff) return;
  const secret = randomBytes(32).toString('hex');
  const username = `${Math.floor(Date.now() / 1000) + 86400}:desktop`;
  const credential = createHmac('sha1', secret).update(username).digest('base64');
  return {address:reply.relay_addr, code:reply.relay_code, secret, username, credential};
}

export function createSwarmWorkers(url) {
  let workers = [];
  let attempted = 0;
  let updated = 0;
  let pending;

  function refresh() {
    if (pending) return pending;
    if (!url || Date.now() - attempted < 30_000) return Promise.resolve(workers);
    attempted = Date.now();
    pending = call(url, 'list_workers', {}).then(result => {
      if (Array.isArray(result)) {
        const now = Date.now();
        workers = [...new Map(result.filter(node => Number.isSafeInteger(node?.id) && node.id >= 0
          && typeof node.ip === 'string' && isIPv4(node.ip) && Number.isFinite(node.active_at)
          && Math.abs(now - node.active_at) < 120_000)
          .map(({id, ip}) => [id, {id, ip}])).values()].slice(0, 24);
        updated = now;
      }
      return workers;
    }).finally(() => { pending = undefined; });
    return pending;
  }

  return {refresh, current: () => Date.now() - updated < 120_000 ? workers : []};
}
