const { spawn } = require('node:child_process');
const { requireHostFile } = require('./integration-support.cjs');

const WebSocket = globalThis.WebSocket;
const port = 18145;
const relayPath = requireHostFile('server/netplay-relay.mjs', 'netplay relay');

function waitListening(child) {
  return new Promise((resolve, reject) => {
    const timer = setTimeout(() => reject(new Error('relay start timeout')), 3000);
    const onData = chunk => {
      if (!String(chunk).includes('listening')) return;
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

async function connect(clientId) {
  const socket = new WebSocket(`ws://127.0.0.1:${port}/?room=lobbycontract&lobby=${clientId}`);
  const messages = [];
  const waiters = [];
  socket.addEventListener('message', event => {
    let message;
    try { message = JSON.parse(String(event.data)); } catch { return; }
    let delivered = false;
    for (const waiter of [...waiters]) {
      if (!waiter.predicate(message)) continue;
      clearTimeout(waiter.timer);
      waiters.splice(waiters.indexOf(waiter), 1);
      waiter.resolve(message);
      delivered = true;
      break;
    }
    if (!delivered) messages.push(message);
  });
  await new Promise((resolve, reject) => {
    const timer = setTimeout(() => reject(new Error(`open timeout: ${clientId}`)), 1500);
    socket.addEventListener('open', () => { clearTimeout(timer); resolve(); }, { once: true });
    socket.addEventListener('error', () => { clearTimeout(timer); reject(new Error(`socket error: ${clientId}`)); }, { once: true });
  });
  const wait = (predicate, label) => {
    const existingIndex = messages.findIndex(predicate);
    if (existingIndex >= 0) return Promise.resolve(messages.splice(existingIndex, 1)[0]);
    return new Promise((resolve, reject) => {
      const waiter = { predicate, resolve, timer: null };
      waiter.timer = setTimeout(() => {
        const index = waiters.indexOf(waiter);
        if (index >= 0) waiters.splice(index, 1);
        reject(new Error(`message timeout: ${label}`));
      }, 1500);
      waiters.push(waiter);
    });
  };
  await wait(message => message.type === 'state', `${clientId} initial state`);
  return { socket, wait };
}

const seatState = (index, predicate) => message =>
  message.type === 'state' && predicate(message.room?.seats?.[index]);

async function main() {
  if (typeof WebSocket !== 'function') throw new Error('global WebSocket is required');
  const child = spawn(process.execPath, [relayPath], {
    env: {
      ...process.env,
      TH07_RELAY_PORT: String(port),
      TH07_STUN_URLS: '',
      TH07_LOBBY_RECONNECT_GRACE_MS: '180',
    },
    stdio: ['ignore', 'pipe', 'pipe'],
  });
  const sockets = [];
  try {
    await waitListening(child);
    const a = await connect('client_a_contract'); sockets.push(a.socket);
    a.socket.send(JSON.stringify({ type: 'take-seat', seat: 0, loadout: 0, ready: true }));
    await a.wait(seatState(0, seat => seat?.clientId === 'client_a_contract'), 'A takes P1');
    a.socket.send(JSON.stringify({ type: 'settings', playerCount: 2, difficulty: 5 }));
    await a.wait(message => message.type === 'state' && message.room?.difficulty === 5,
      'Phantasm room setting is retained');

    const b = await connect('client_b_contract'); sockets.push(b.socket);
    b.socket.send(JSON.stringify({ type: 'take-seat', seat: 1, loadout: 2, ready: true }));
    await a.wait(seatState(1, seat => seat?.clientId === 'client_b_contract' && !seat.offline), 'B takes P2');

    b.socket.close(4001, 'network transition');
    await a.wait(seatState(1, seat => seat?.clientId === 'client_b_contract' && seat.offline), 'B retained offline');

    const b2 = await connect('client_b_contract'); sockets.push(b2.socket);
    await b2.wait(seatState(1, seat => seat?.clientId === 'client_b_contract' && !seat.offline), 'B seat restored');
    await a.wait(seatState(1, seat => seat?.clientId === 'client_b_contract' && !seat.offline), 'A sees B restored');

    b2.socket.close(4001, 'network transition');
    await a.wait(seatState(1, seat => seat?.clientId === 'client_b_contract' && seat.offline), 'B retained for grace');
    await a.wait(seatState(1, seat => seat == null), 'B cleared after grace');

    const observer = await connect('client_observer_contract'); sockets.push(observer.socket);
    a.socket.close(1000, 'leave room');
    await observer.wait(seatState(0, seat => seat == null), 'deliberate leave clears immediately');
  } finally {
    for (const socket of sockets) try { socket.close(); } catch {}
    child.kill('SIGTERM');
  }
  console.log('TH07 lobby reconnect contract: PASS');
}

main().catch(error => {
  console.error(error && error.stack || error);
  process.exitCode = 1;
});
