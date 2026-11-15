import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import {once} from 'node:events';
import {fileURLToPath} from 'node:url';
import {WebSocket} from 'ws';

const environment = {...process.env, EAGLER_NETPLAY_RELAY_HOST: '127.0.0.1',
  EAGLER_NETPLAY_RELAY_PORT: '0', EAGLER_NETPLAY_MAX_CONNECTIONS: '8', EAGLER_NETPLAY_MAX_ROOMS_PER_IP: '1'};
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

function attempt(port, query) {
  const socket = new WebSocket(`ws://127.0.0.1:${port}/?${query}`);
  sockets.push(socket);
  return socket;
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
  const ordinary = await connect(port, `room=${room}&lobby=${ordinaryClient}&member=ordinary_member_01&intent=create&players=2&difficulty=1&visibility=private`);
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

  const otherRoom = 'th09mp-19283746';
  const collision = attempt(port, `room=${otherRoom}&lobby=collision_client&member=collision_member&intent=create`);
  const collisionClosed = once(collision,'close');
  const collisionError = await nextMessage(collision, message => message.type === 'error', 'global room code conflict');
  assert.match(collisionError.error,/房间号已被使用/);
  assert.equal((await collisionClosed)[0],4007);

  const wrongGame = attempt(port, `room=${otherRoom}&lobby=lookup_client&member=lookup_member&intent=join`);
  const redirected = await nextMessage(wrongGame, message => message.type === 'room-product', 'resolve room across titles');
  assert.equal(redirected.product,'th08mp');
  assert.equal(redirected.code,'19283746');

  const guestReady = nextMessage(oldChannel.socket, message => message.room?.seats[1]?.ready, 'guest still in original room');
  oldChannel.socket.send(JSON.stringify({type:'switch-product',product:'th09mp'}));
  oldChannel.socket.send(JSON.stringify({type:'set-ready',ready:true}));
  await guestReady;

  const switched = [ordinary,oldChannel].map(({socket}) =>
    nextMessage(socket, message => message.type === 'room-product', 'move room member'));
  ordinary.socket.send(JSON.stringify({type:'switch-product',product:'th09mp'}));
  for (const message of await Promise.all(switched)) {
    assert.equal(message.product,'th09mp');
    assert.equal(message.code,'19283746');
  }
  const hostMoved = await connect(port, `room=${otherRoom}&lobby=${ordinaryClient}&member=ordinary_member_01&intent=join`);
  const guestMoved = await connect(port, `room=${otherRoom}&lobby=${oldChannelClient}&member=old_beta_member_01&intent=join`);
  assert.deepEqual(guestMoved.initial.room.seats.slice(0,2).map(seat=>[seat.clientId,seat.ready,seat.loadout]),
    [[ordinaryClient,false,0],[oldChannelClient,false,0]]);
  assert.equal(hostMoved.initial.room.seats[0].offline,false);
  assert.equal(hostMoved.initial.room.visibility,'private');
  const oldLink = attempt(port, `room=${room}&lobby=old_link_client&member=old_link_member&intent=join`);
  assert.equal((await nextMessage(oldLink,message=>message.type==='room-product','old invite follows room')).product,'th09mp');
  const oldRun = attempt(port,`room=${room}&run=1&player=0&players=2`);
  assert.equal((await once(oldRun,'close'))[0],1008);
  const back = nextMessage(hostMoved.socket, message=>message.type==='room-product','switch back using the same room code');
  hostMoved.socket.send(JSON.stringify({type:'switch-product',product:'th08mp'}));
  assert.equal((await back).product,'th08mp');
  console.log('Relay: shared room codes, host-only title change, preserved seats and old invites PASS');
} finally {
  for (const socket of sockets) if (socket.readyState < WebSocket.CLOSING) socket.terminate();
  if (relay.exitCode === null) {
    const ended = new Promise(resolve => relay.once('exit', resolve));
    relay.kill(); await ended;
  }
}
