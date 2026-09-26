import assert from 'node:assert/strict';
import { spawn } from 'node:child_process';
import net from 'node:net';
import { WebSocket } from 'ws';
const port=await new Promise(resolve=>{const server=net.createServer();server.listen(0,'127.0.0.1',()=>{const p=server.address().port;server.close(()=>resolve(p));});});
const relay=spawn(process.execPath,['server/netplay-relay.mjs'],{env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:String(port),EAGLER_NETPLAY_STUN_URLS:''},stdio:['ignore','pipe','pipe']});
const clients=[];
const delay=ms=>new Promise(resolve=>setTimeout(resolve,ms));
async function connect(id,room='th07mp-roomprobe') {
  const socket=new WebSocket(`ws://127.0.0.1:${port}/?room=${room}&lobby=${id}`);
  const messages=[];socket.on('message',data=>messages.push(JSON.parse(String(data))));
  const client={socket,messages,send:value=>socket.send(JSON.stringify(value))};clients.push(client);
  await until(()=>messages.some(m=>m.type==='state'));
  assert.ok(messages[0].roomProbe,'capability must extend the initial state, not insert a protocol message');
  return client;
}
async function until(check) { const end=Date.now()+4000;while(!check()){if(Date.now()>end)throw new Error('relay expectation timed out');await delay(10);} }
async function seat(client,index) {client.send({type:'take-seat',seat:index,loadout:0,name:`Player${index}`});await until(()=>client.messages.some(m=>m.room?.seats[index]?.name===`Player${index}`));}
try {
  await new Promise((resolve,reject)=>{const timeout=setTimeout(()=>reject(new Error('relay start timeout')),5000);relay.stdout.on('data',data=>{if(String(data).includes('listening')){clearTimeout(timeout);resolve();}});relay.once('exit',()=>reject(new Error('relay exited')));});
  const a=await connect('client_alpha'), b=await connect('client_beta'), spectator=await connect('client_watcher'), outsider=await connect('client_outsider','th07mp-otherprobe');
  await seat(a,0);await seat(b,1);spectator.send({type:'spectate',name:'Watcher'});await until(()=>a.messages.some(m=>m.room?.spectators?.length));
  const state=a.messages.filter(m=>m.type==='state').at(-1).room;
  const probe={type:'room-probe',to:'client_beta',from:'spoof',lane:'relay',echo:'ping:7'};
  a.send(probe);await until(()=>b.messages.some(m=>m.type==='room-probe'));
  assert.deepEqual(b.messages.find(m=>m.type==='room-probe'),{type:'room-probe',from:'client_alpha',lane:'relay',echo:'ping:7'});
  assert.ok(!spectator.messages.some(m=>m.type==='room-probe'),'probes are not broadcast to spectators');
  b.messages.length=0; spectator.send(probe);outsider.send(probe);a.send({...probe,to:'client_outsider'});await delay(100);
  assert.ok(!b.messages.some(m=>m.type==='room-probe'),'unseated/cross-room senders cannot signal');
  assert.ok(!outsider.messages.some(m=>m.type==='room-probe'));
  a.send({type:'room-probe',to:'client_beta',lane:'direct',token:'session-1',restart:true});
  await until(()=>b.messages.some(m=>m.restart));
  assert.deepEqual(a.messages.filter(m=>m.type==='state').at(-1).room,state,'diagnostics do not mutate seats, settings, ready or phase');
  a.send({type:'stand-up'});await until(()=>a.messages.filter(m=>m.type==='state').at(-1).room.seats[0]===null);
  b.messages.length=0;a.send(probe);await delay(100);assert.ok(!b.messages.some(m=>m.type==='room-probe'));
  console.log('PASS real relay capability compatibility, targeted echo, authoritative sender, spectator/cross-room isolation, read-only diagnostics');
} finally { for(const client of clients)client.socket.terminate();relay.kill(); }
