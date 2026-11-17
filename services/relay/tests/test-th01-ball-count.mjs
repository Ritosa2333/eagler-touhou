// Real local relay sockets. No native engine or spectator acceptance claim.
import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import {once} from 'node:events';
import {WebSocket} from 'ws';

const relay=spawn(process.execPath,['src/netplay-relay.mjs'],{windowsHide:true,stdio:['ignore','pipe','pipe'],
  env:{...process.env,EAGLER_NETPLAY_GAMEPLAY_RELAY_FALLBACK:'1',EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:'0',EAGLER_NETPLAY_STUN_URLS:''}});
const sockets=[];let log='';
try {
  const port=await new Promise((resolve,reject)=>{
    const timer=setTimeout(()=>reject(new Error('relay startup timeout: '+log)),60000);
    relay.stderr.on('data',data=>{log+=data;});
    relay.stdout.on('data',data=>{log+=data;const match=/listening ws:\/\/127\.0\.0\.1:(\d+)/.exec(log);if(match){clearTimeout(timer);resolve(Number(match[1]));}});
    relay.once('exit',()=>{clearTimeout(timer);reject(new Error('relay exited: '+log));});
  });
  const room='th01mp-75684312',A='a'.repeat(64),B='b'.repeat(64);
  function connect(params) {
    const socket=new WebSocket(`ws://127.0.0.1:${port}/?${new URLSearchParams({room,...params})}`),messages=[],waiters=[];sockets.push(socket);
    socket.on('message',(data,binary)=>{
      const message=binary?data:JSON.parse(String(data));messages.push(message);
      for(const waiter of [...waiters])if(waiter.test(message))waiter.resolve(message);
    });
    return {socket,messages,wait(test){
      const found=messages.find(test);if(found)return Promise.resolve(found);
      return new Promise((resolve,reject)=>{
        const timer=setTimeout(()=>reject(new Error('relay message timeout: '+log)),15000);
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
  const host=connect({lobby:'th01_host_123',member:'host_member_123',intent:'create'}),guest=connect({lobby:'th01_guest_123',member:'guest_member_123'});
  const first=await host.wait(m=>m.type==='state');await guest.wait(m=>m.type==='state');assert.equal(first.room.ballCount,2);assert.equal(first.room.adonisMode,0,'TH01 room enables rollback by default');
  await send(host,{type:'take-seat',seat:0,loadout:0},m=>m.room?.seats[0]);await send(guest,{type:'take-seat',seat:1,loadout:0},m=>m.room?.seats[1]);
  await send(host,{type:'set-ready',ready:true,runtimeGeneration:A},m=>m.room?.seats[0]?.ready);await send(guest,{type:'set-ready',ready:true,runtimeGeneration:A},m=>m.room?.seats[1]?.ready);
  const changed=await send(host,{type:'settings',playerCount:2,difficulty:1,ballCount:6},m=>m.room?.ballCount===6);
  assert(changed.room.seats.slice(0,2).every(s=>!s.ready));assert(changed.room.settingsVersion>first.room.settingsVersion);await guest.wait(m=>m.room?.ballCount===6);
  await send(host,{type:'settings',playerCount:2,difficulty:1,ballCount:9},m=>m.room?.ballCount===9);
  await guest.wait(m=>m.room?.ballCount===9);
  const denied=await send(guest,{type:'settings',playerCount:2,difficulty:1,ballCount:2},m=>m.type==='error');assert(denied.error);
  await send(host,{type:'settings',playerCount:2,difficulty:1,ballCount:5},m=>m.type==='error');
  console.log('TH01 relay ball count, host authority and ready reset PASS');
} finally {
  for(const socket of sockets)if(socket.readyState<2)socket.terminate();
  if(relay.exitCode===null){const ended=once(relay,'exit');relay.kill();await ended;}
}
