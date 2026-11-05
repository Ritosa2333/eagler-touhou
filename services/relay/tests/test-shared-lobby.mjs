import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import {fileURLToPath} from 'node:url';
import {WebSocket} from 'ws';

const environment = {...process.env, EAGLER_NETPLAY_RELAY_HOST: '127.0.0.1',
  EAGLER_NETPLAY_RELAY_PORT: '0', EAGLER_NETPLAY_MAX_CONNECTIONS: '8'};
delete environment.EAGLER_RELEASE_CHANNEL;
const relay = spawn(process.execPath, [fileURLToPath(new URL('../src/netplay-relay.mjs', import.meta.url))], {
  env: environment, stdio: ['ignore', 'pipe', 'pipe'], windowsHide: true,
});
const sockets = [];
let log = '';
relay.stdout.on('data', bytes => {log += bytes;});
relay.stderr.on('data', bytes => {log += bytes;});

function nextMessage(socket, predicate, label) {
  return new Promise((resolve, reject) => {
    const timer = setTimeout(() => {cleanup(); reject(new Error(`${label} timeout\n${log}`));}, 5000);
    const onMessage = bytes => {
      let value;
      try {value = JSON.parse(String(bytes));}
      catch (error) {cleanup(); reject(error); return;}
      if (!predicate(value)) return;
      cleanup(); resolve(value);
    };
    const onError = () => {cleanup(); reject(new Error(`${label}: Relay socket failed`));};
    const onClose = () => {cleanup(); reject(new Error(`${label}: Relay socket closed`));};
    const cleanup = () => {
      clearTimeout(timer);
      socket.off('message', onMessage); socket.off('error', onError); socket.off('close', onClose);
    };
    socket.on('message', onMessage); socket.once('error', onError); socket.once('close', onClose);
  });
}

async function connect(port, query) {
  const socket = new WebSocket(`ws://127.0.0.1:${port}/?${query}`);
  sockets.push(socket);
  const state = nextMessage(socket, message => message.type === 'state', 'initial room state');
  await new Promise((resolve, reject) => {
    socket.once('open', resolve); socket.once('error', reject);
  });
  return {socket, initial: await state};
}

try {
  const port = await new Promise((resolve, reject) => {
    const timer = setTimeout(() => reject(new Error(`Relay startup timeout\n${log}`)), 10000);
    const onData = bytes => {
      const match = /listening ws:\/\/127\.0\.0\.1:(\d+)/.exec(log);
      if (match) {clearTimeout(timer); relay.stdout.off('data', onData); resolve(Number(match[1]));}
    };
    relay.stdout.on('data', onData);
    relay.once('exit', () => {clearTimeout(timer); reject(new Error(`Relay exited\n${log}`));});
  });
  const room = 'th08mp-19283746';
  const ordinaryClient = 'ordinary_client_01', oldChannelClient = 'old_beta_client_01';
  const ordinary = await connect(port, `room=${room}&lobby=${ordinaryClient}&member=ordinary_member_01&intent=create&players=2&difficulty=1`);
  const oldChannel = await connect(port, `room=${room}&lobby=${oldChannelClient}&member=old_beta_member_01&channel=beta&intent=join&players=2&difficulty=1`);
  assert.equal(ordinary.initial.room.phase, 'lobby');
  assert.equal(oldChannel.initial.room.phase, 'lobby');
  assert.equal(new URL(oldChannel.socket.url).searchParams.get('channel'), 'beta');

  const ordinaryHost = nextMessage(ordinary.socket, message => message.type === 'state' &&
    message.room.seats[0]?.clientId === ordinaryClient, 'ordinary client seat update');
  const oldChannelHost = nextMessage(oldChannel.socket, message => message.type === 'state' &&
    message.room.seats[0]?.clientId === ordinaryClient, 'old-channel view of ordinary client');
  ordinary.socket.send(JSON.stringify({type: 'take-seat', seat: 0, loadout: 0}));
  await Promise.all([ordinaryHost, oldChannelHost]);

  const ordinaryGuest = nextMessage(ordinary.socket, message => message.type === 'state' &&
    message.room.seats[1]?.clientId === oldChannelClient, 'ordinary view of old-channel client');
  const oldChannelGuest = nextMessage(oldChannel.socket, message => message.type === 'state' &&
    message.room.seats[1]?.clientId === oldChannelClient, 'old-channel client seat update');
  oldChannel.socket.send(JSON.stringify({type: 'take-seat', seat: 1, loadout: 0}));
  const [ordinaryState, oldChannelState] = await Promise.all([ordinaryGuest, oldChannelGuest]);
  for (const state of [ordinaryState, oldChannelState]) {
    assert.equal(state.room.seats[0]?.clientId, ordinaryClient);
    assert.equal(state.room.seats[1]?.clientId, oldChannelClient);
  }
  console.log('Relay: old channel URL and ordinary URL join the same lobby and share both seats PASS');
} finally {
  for (const socket of sockets) if (socket.readyState < WebSocket.CLOSING) socket.terminate();
  if (relay.exitCode === null) {
    const ended = new Promise(resolve => relay.once('exit', resolve));
    relay.kill(); await ended;
  }
}
