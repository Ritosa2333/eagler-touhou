import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import net from 'node:net';
import {fileURLToPath} from 'node:url';

const listener = net.createServer();
await new Promise(resolve => listener.listen(0, '127.0.0.1', resolve));
const port = listener.address().port;
await new Promise((resolve, reject) => listener.close(error => error ? reject(error) : resolve()));
const child = spawn(process.execPath, [fileURLToPath(new URL('../src/netplay-relay.mjs', import.meta.url))], {
  env: {...process.env, EAGLER_NETPLAY_RELAY_HOST: '127.0.0.1', EAGLER_NETPLAY_RELAY_PORT: String(port),
    EAGLER_NETPLAY_PUBLIC_NICKNAMES:'0',EAGLER_NETPLAY_MAX_ROOMS_PER_IP:'16',
    EAGLER_NETPLAY_MAX_CONNECTIONS_PER_IP:'160',EAGLER_NETPLAY_CONNECTIONS_PER_MINUTE:'120'},
  stdio: ['ignore', 'pipe', 'pipe'],
  windowsHide: true,
});
const sockets = [];
let log = '';
child.stdout.on('data', bytes => log += bytes);
child.stderr.on('data', bytes => log += bytes);

async function wait(test, label) {
  const end = Date.now() + 8000;
  while (!test()) {
    if (Date.now() > end) throw new Error(`${label}\n${log}`);
    await new Promise(resolve => setTimeout(resolve, 5));
  }
}
async function open(query) {
  const ws = new WebSocket(`ws://127.0.0.1:${port}/?${query}`);
  const socket = {ws, json: [], closed:null};
  sockets.push(ws);
  ws.addEventListener('message', event => {
    if (typeof event.data === 'string') socket.json.push(JSON.parse(event.data));
  });
  ws.addEventListener('close',event=>{socket.closed={code:event.code,reason:event.reason};});
  await wait(() => ws.readyState === WebSocket.OPEN||socket.closed, `open ${query}`);
  assert.equal(ws.readyState,WebSocket.OPEN,JSON.stringify({query,closed:socket.closed}));
  return socket;
}
const send = (socket, message) => socket.ws.send(JSON.stringify(message));
const latest = socket => [...socket.json].reverse().find(message => message.room && typeof message.room === 'object')?.room;
try {
  await wait(() => log.includes('listening'), 'relay ready');
  for(const product of ['th06mp','th07mp','th08mp','th09mp','th10mp','th11mp','th15mp']){
  const room=product+'-720001';
  const hostId=product+'_chat_host',viewerId=product+'_chat_viewer';
  const host = await open(`room=${room}&lobby=${hostId}&member=${product}_chat_member_host`);
  const guest = await open(`room=${room}&lobby=${product}_chat_guest&member=${product}_chat_member_guest`);
  const viewer = await open(`room=${room}&lobby=${viewerId}&member=${product}_chat_member_viewer`);
  const outsider = await open(`room=${room}&lobby=${product}_chat_outsider&member=${product}_chat_member_outsider`);

  send(host, {type: 'take-seat', seat: 0, loadout: 0});
  send(guest, {type: 'take-seat', seat: 1, loadout: 0});
  send(viewer, {type: 'spectate'});
  await wait(() => latest(host)?.seats?.slice(0, 2).every(Boolean) &&
    latest(host)?.spectators?.some(member => member.clientId === viewerId), 'players and spectator membership');

  send(host, {type: 'set-ready', ready: true});
  send(guest, {type: 'set-ready', ready: true});
  await wait(() => latest(host)?.seats?.slice(0, 2).every(seat => seat?.ready), 'both players ready');
  send(host, {type: 'start',...(product==='th15mp'?{adonisMode:1}:{})});
  await wait(() => host.json.some(message => message.type === 'start' && message.serial === 1), 'room started');

  send(viewer, {type: 'quick-chat', serial: 1, phrase: 'thanks'});
  for (const socket of [host, guest, viewer]) {
    await wait(() => socket.json.some(message => message.type === 'quick-chat' &&
      message.clientId === viewerId && message.seat === -1 && message.spectator === true),
    product+' spectator quick chat broadcast');
  }
  const spectatorMessage = host.json.findLast(message => message.type === 'quick-chat' && message.clientId === viewerId);
  assert.deepEqual({room: spectatorMessage.room, serial: spectatorMessage.serial, seat: spectatorMessage.seat,
    spectator: spectatorMessage.spectator, phrase: spectatorMessage.phrase},
  {room, serial: 1, seat: -1, spectator: true, phrase: 'thanks'});

  send(host, {type: 'quick-chat', serial: 1, phrase: 'follow-me'});
  for (const socket of [host, guest, viewer]) {
    await wait(() => socket.json.some(message => message.type === 'quick-chat' &&
      message.clientId === hostId && message.seat === 0 && message.spectator !== true),
    product+' player message reaches every member');
  }

  const beforeRejected = host.json.filter(message => message.type === 'quick-chat').length;
  send(outsider, {type: 'quick-chat', serial: 1, phrase: 'thanks'});
  send(viewer, {type: 'quick-chat', serial: 0, phrase: 'thanks'});
  await new Promise(resolve => setTimeout(resolve, 80));
  assert.equal(host.json.filter(message => message.type === 'quick-chat').length, beforeRejected,
    'nonmember messages and stale run serials stay rejected');
  console.log(product+': authenticated spectator chat, player reply and stale/outsider rejection PASS');
  for(const socket of sockets)socket.close();
  await wait(()=>sockets.every(socket=>socket.readyState===WebSocket.CLOSED),'scenario sockets retired');
  sockets.length=0;
  }
} finally {
  for (const socket of sockets) socket.close();
  child.kill();
  await new Promise(resolve => child.once('exit', resolve));
}
