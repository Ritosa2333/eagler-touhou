const { createHmac } = require('node:crypto');
const { spawn } = require('node:child_process');
const { requireHostFile } = require('./integration-support.cjs');
const WebSocket = globalThis.WebSocket;

const port = 18143;
const secret = 'turn-contract-test-secret';
const ttl = 600;
const relayPath = requireHostFile('server/netplay-relay.mjs', 'netplay relay');

function fail(message) {
  throw new Error(message);
}

async function waitListening(child) {
  await new Promise((resolve, reject) => {
    const timer = setTimeout(() => reject(new Error('relay start timeout')), 5000);
    const onData = chunk => {
      const text = String(chunk);
      if (!text.includes('listening')) return;
      clearTimeout(timer);
      child.stdout.off('data', onData);
      resolve();
    };
    child.stdout.on('data', onData);
    child.once('exit', code => {
      clearTimeout(timer);
      reject(new Error(`relay exited before listen: ${code}`));
    });
  });
}

async function fetchIceServers() {
  return await new Promise((resolve, reject) => {
    const socket = new WebSocket(`ws://127.0.0.1:${port}/?room=turntest&run=1&player=0&players=2&signal=1`);
    const timer = setTimeout(() => {
      socket.close();
      reject(new Error('signaling response timeout'));
    }, 5000);
    socket.addEventListener('message', event => {
      let message;
      try { message = JSON.parse(String(event.data)); }
      catch { return; }
      if (message.type !== 'peers') return;
      clearTimeout(timer);
      socket.close();
      resolve(message.iceServers);
    });
    socket.addEventListener('error', () => reject(new Error('signaling socket error')));
  });
}

async function verifyRelayFallbackWithoutAllSignaling() {
  const sockets = [];
  const open = url => new Promise((resolve, reject) => {
    const socket = new WebSocket(url);
    sockets.push(socket);
    const timer = setTimeout(() => reject(new Error('fallback socket open timeout')), 3000);
    socket.addEventListener('open', () => { clearTimeout(timer); resolve(socket); }, { once: true });
    socket.addEventListener('error', () => { clearTimeout(timer); reject(new Error('fallback socket error')); }, { once: true });
  });
  try {
    const base = `ws://127.0.0.1:${port}/?room=fallbacktest&run=1&players=2`;
    const relay0 = await open(`${base}&player=0`);
    const relay1 = await open(`${base}&player=1`);
    // Deliberately omit P2 signaling. Before the fallback fix this prevented
    // the server timer from starting forever, despite both relay paths being ready.
    await open(`${base}&player=0&signal=1`);
    const waitRoute = socket => new Promise((resolve, reject) => {
      const timer = setTimeout(() => reject(new Error('relay fallback route timeout')), 3500);
      socket.addEventListener('message', event => {
        let message;
        try { message = JSON.parse(String(event.data)); } catch { return; }
        if (message.type !== 'route') return;
        clearTimeout(timer);
        resolve(message.mode);
      });
    });
    const routes = await Promise.all([waitRoute(relay0), waitRoute(relay1)]);
    if (routes.some(route => route !== 'relay')) fail(`unexpected fallback routes: ${routes.join(',')}`);
  } finally {
    for (const socket of sockets) try { socket.close(); } catch {}
  }
}

async function verifyIceRestartRequestForwarding() {
  const sockets = [];
  const open = player => new Promise((resolve, reject) => {
    const socket = new WebSocket(`ws://127.0.0.1:${port}/?room=restarttest&run=1&players=2&signal=1&player=${player}`);
    sockets.push(socket);
    const timer = setTimeout(() => reject(new Error('restart signaling open timeout')), 3000);
    socket.addEventListener('open', () => { clearTimeout(timer); resolve(socket); }, { once: true });
    socket.addEventListener('error', () => { clearTimeout(timer); reject(new Error('restart signaling socket error')); }, { once: true });
  });
  try {
    const signal0 = await open(0);
    const signal1 = await open(1);
    const waitRtcRoute = socket => new Promise((resolve, reject) => {
      const timer = setTimeout(() => reject(new Error('RTC route timeout')), 3000);
      socket.addEventListener('message', event => {
        let message;
        try { message = JSON.parse(String(event.data)); } catch { return; }
        if (message.type !== 'route' || message.mode !== 'rtc') return;
        clearTimeout(timer);
        resolve();
      });
    });
    const routes = Promise.all([waitRtcRoute(signal0), waitRtcRoute(signal1)]);
    signal0.send(JSON.stringify({ type: 'rtc-ready' }));
    signal1.send(JSON.stringify({ type: 'rtc-ready' }));
    await routes;

    const forwarded = new Promise((resolve, reject) => {
      const timer = setTimeout(() => reject(new Error('ICE restart request forwarding timeout')), 3000);
      signal0.addEventListener('message', event => {
        let message;
        try { message = JSON.parse(String(event.data)); } catch { return; }
        if (message.type !== 'ice-restart-request') return;
        clearTimeout(timer);
        resolve(message);
      });
    });
    signal1.send(JSON.stringify({ type: 'ice-restart-request', to: 0 }));
    const message = await forwarded;
    if (message.from !== 1) fail(`unexpected ICE restart requester: ${message.from}`);
  } finally {
    for (const socket of sockets) try { socket.close(); } catch {}
  }
}

async function main() {
  const child = spawn(process.execPath, [relayPath], {
    env: {
      ...process.env,
      TH07_RELAY_PORT: String(port),
      TH07_STUN_URLS: 'stun:stun.cloudflare.com:3478',
      TH07_TURN_URLS: 'turn:turn.example.test:3478?transport=udp,turns:turn.example.test:5349?transport=tcp',
      TH07_TURN_SHARED_SECRET: secret,
      TH07_TURN_TTL_SECONDS: String(ttl),
      TH07_RTC_TIMEOUT_MS: '1000',
    },
    stdio: ['ignore', 'pipe', 'pipe'],
  });

  try {
    await waitListening(child);
    const iceServers = await fetchIceServers();
    if (!Array.isArray(iceServers) || iceServers.length !== 2) fail('unexpected ICE server count');
    if (!Array.isArray(iceServers[0].urls) || iceServers[0].urls[0] !== 'stun:stun.cloudflare.com:3478')
      fail('STUN server missing');

    const turn = iceServers[1];
    if (!Array.isArray(turn.urls) || turn.urls.length !== 2) fail('TURN URLs missing');
    if (!/^\d+:turntest-1-p0$/.test(turn.username || '')) fail('TURN REST username format mismatch');
    const expires = Number(String(turn.username).split(':', 1)[0]);
    const now = Math.floor(Date.now() / 1000);
    if (!Number.isFinite(expires) || expires < now + ttl - 10 || expires > now + ttl + 10)
      fail('TURN credential TTL mismatch');
    const expected = createHmac('sha1', secret).update(turn.username).digest('base64');
    if (turn.credential !== expected) fail('TURN REST HMAC mismatch');
    await verifyRelayFallbackWithoutAllSignaling();
    await verifyIceRestartRequestForwarding();
  } finally {
    child.kill('SIGTERM');
  }

  const invalid = spawn(process.execPath, [relayPath], {
    env: {
      ...process.env,
      TH07_RELAY_PORT: String(port + 1),
      TH07_TURN_URLS: 'turn:turn.example.test:3478?transport=udp',
      TH07_TURN_SHARED_SECRET: '',
      TH07_TURN_USERNAME: '',
      TH07_TURN_CREDENTIAL: '',
    },
    stdio: ['ignore', 'pipe', 'pipe'],
  });
  const invalidExit = await new Promise(resolve => invalid.once('exit', resolve));
  if (invalidExit === 0) fail('TURN URLs without credentials must fail fast');

  console.log('TH07 TURN signaling contract: PASS');
}

main().catch(error => {
  console.error(error && error.stack || error);
  process.exitCode = 1;
});
