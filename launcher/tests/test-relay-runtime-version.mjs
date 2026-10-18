// Real local relay sockets. No native engine or spectator acceptance claim.
import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import {once} from 'node:events';
import {WebSocket} from 'ws';

const relay=spawn(process.execPath,['server/netplay-relay.mjs'],{windowsHide:true,stdio:['ignore','pipe','pipe'],
  env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:'0',EAGLER_NETPLAY_STUN_URLS:''}});
const sockets=[];let log='';
try {
  const port=await new Promise((resolve,reject)=>{
    const timer=setTimeout(()=>reject(new Error('relay startup timeout: '+log)),10000);
    relay.stderr.on('data',data=>{log+=data;});
    relay.stdout.on('data',data=>{log+=data;const match=/listening ws:\/\/127\.0\.0\.1:(\d+)/.exec(log);if(match){clearTimeout(timer);resolve(Number(match[1]));}});
    relay.once('exit',()=>{clearTimeout(timer);reject(new Error('relay exited: '+log));});
  });
  const room='th08mp-75684312',A='a'.repeat(64),B='b'.repeat(64);
  function connect(params) {
    const socket=new WebSocket(`ws://127.0.0.1:${port}/?${new URLSearchParams({room,...params})}`),messages=[],waiters=[];sockets.push(socket);
    socket.on('message',(data,binary)=>{
      const message=binary?data:JSON.parse(String(data));messages.push(message);
      for(const waiter of [...waiters])if(waiter.test(message))waiter.resolve(message);
    });
    return {socket,messages,wait(test){
      const found=messages.find(test);if(found)return Promise.resolve(found);
      return new Promise((resolve,reject)=>{
        const timer=setTimeout(()=>reject(new Error('relay message timeout: '+log)),5000);
        const waiter={test,resolve(value){clearTimeout(timer);waiters.splice(waiters.indexOf(waiter),1);resolve(value);}};waiters.push(waiter);
      });
    }};
  }
  async function send(client,message,test) {
    client.messages.length=0;const pending=client.wait(test);client.socket.send(JSON.stringify(message));return pending;
  }
  async function reject(params) {
    const client=connect(params),[code,reason]=await once(client.socket,'close');
    assert.equal(code,1008);assert.equal(String(reason),'runtime version mismatch');
  }
  const host=connect({lobby:'runtime_host_123',member:'host_member_123'}),guest=connect({lobby:'runtime_guest_123',member:'guest_member_123'});
  assert.equal((await host.wait(m=>m.type==='state')).roomDirectory.runtimeVersions,true);
  await guest.wait(m=>m.type==='state');
  await send(host,{type:'take-seat',seat:0,loadout:0},m=>m.room?.seats[0]);
  await send(guest,{type:'take-seat',seat:1,loadout:1},m=>m.room?.seats[1]);
  // Mixed old/new clients cannot start even when the old client readied first.
  await send(guest,{type:'set-ready',ready:true},m=>m.room?.seats[1]?.ready);
  await send(host,{type:'set-ready',ready:true,runtimeGeneration:A},m=>m.code==='runtime-version');
  await send(guest,{type:'set-ready',ready:false},m=>m.room?.seats[1]?.ready===false);
  const first=await send(host,{type:'set-ready',ready:true,runtimeGeneration:A},m=>m.room?.seats[0]?.ready);
  assert.equal(first.room.seats[0].runtimeGeneration,A);
  for(const runtimeGeneration of [undefined,B,'invalid']) {
    const payload={type:'set-ready',ready:true,...(runtimeGeneration===undefined?{}:{runtimeGeneration})};
    await send(guest,payload,m=>m.code==='runtime-version');
  }
  await send(host,{type:'start'},m=>m.code==='runtime-version');
  assert.equal(host.messages.some(m=>m.type==='start'),false);
  await send(guest,{type:'set-ready',ready:true,runtimeGeneration:A},m=>m.room?.seats.slice(0,2).every(s=>s?.ready));
  const started=await send(host,{type:'start'},m=>m.type==='start');
  assert.equal(started.room.runtimeGeneration,A);
  const params={run:String(started.serial),players:'2',player:'0',member:'host_member_123'};
  for(const signal of ['0','1'])for(const generation of [undefined,B])await reject({...params,signal,...(generation?{runtimeGeneration:generation}:{})});
  const signal=connect({...params,signal:'1',runtimeGeneration:A});await signal.wait(m=>m.type==='peers');
  const hostPlay=connect({...params,runtimeGeneration:A});await once(hostPlay.socket,'open');
  const guestPlay=connect({...params,player:'1',member:'guest_member_123',runtimeGeneration:A});await once(guestPlay.socket,'open');
  const bytes=Buffer.from([1,2,3,4]),received=guestPlay.wait(Buffer.isBuffer);hostPlay.socket.send(bytes);
  assert.deepEqual(await received,bytes,'same-generation gameplay keeps its existing transport');
  console.log('Relay Runtime agreement: mixed/missing/malformed versions blocked; matching start and transport PASS');
} finally {
  for(const socket of sockets)if(socket.readyState<2)socket.terminate();
  if(relay.exitCode===null){const ended=once(relay,'exit');relay.kill();await ended;}
}
