// Real local relay sockets. BRO-only room options: systems + stage queue sync,
// ready invalidation, invalid rejection, cross-product isolation.
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
  const room='bromp-75684312';
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
  const host=connect({lobby:'bro_host_123',member:'host_member_123'}),guest=connect({lobby:'bro_guest_123',member:'guest_member_123'});
  const first=await host.wait(m=>m.type==='state');await guest.wait(m=>m.type==='state');
  assert.deepEqual(first.room.broSystems,[1,2,3,4,5,6,7,8,9,10,11],'默认全系统');
  assert.deepEqual(first.room.broStageQueue,[],'默认空队列');
  assert.deepEqual(first.room.difficulties,undefined,'difficulties 不在 room 快照');
  await send(host,{type:'take-seat',seat:0,loadout:0},m=>m.room?.seats[0]);await send(guest,{type:'take-seat',seat:1,loadout:1},m=>m.room?.seats[1]);
  await send(host,{type:'set-ready',ready:true,runtimeGeneration:'a'.repeat(64)},m=>m.room?.seats[0]?.ready);
  await send(guest,{type:'set-ready',ready:true,runtimeGeneration:'a'.repeat(64)},m=>m.room?.seats[1]?.ready);
  const systems=[1,2,11],queue=[3,1,7];
  const changed=await send(host,{type:'settings',playerCount:2,difficulty:1,broSystems:systems,broStageQueue:queue},m=>JSON.stringify(m.room?.broStageQueue)===JSON.stringify(queue));
  assert.deepEqual(changed.room.broSystems,systems);
  assert.deepEqual(changed.room.broStageQueue,queue);
  assert(changed.room.seats.slice(0,2).every(s=>!s.ready),'改设置清除双方准备');
  assert(changed.room.settingsVersion>first.room.settingsVersion,'版本递增');
  await guest.wait(m=>JSON.stringify(m.room?.broStageQueue)===JSON.stringify(queue));
  // 非法字段：越界系统 / 越界章节 / 重复系统
  await send(host,{type:'settings',playerCount:2,difficulty:1,broSystems:[12]},m=>m.type==='error');
  await send(host,{type:'settings',playerCount:2,difficulty:1,broStageQueue:[20]},m=>m.type==='error');
  await send(host,{type:'settings',playerCount:2,difficulty:1,broSystems:[2,2]},m=>m.type==='error');
  // 跨作品隔离：th19 房间不收 BRO 字段，反向也不泄漏
  const unrelated=connect({room:'th19mp-75684313',lobby:'other_host_123',member:'other_member_123'});
  const otherState=await unrelated.wait(m=>m.type==='state');
  assert.equal('broSystems' in otherState.room,false);
  assert.equal('broStageQueue' in otherState.room,false);
  await send(unrelated,{type:'take-seat',seat:0,loadout:0},m=>m.room?.seats[0]);
  await send(unrelated,{type:'settings',playerCount:2,difficulty:1,broSystems:systems},m=>m.type==='error');
  console.log('BRO Relay: systems/queue sync, ready invalidation, invalid rejection, cross-product isolation PASS');
} finally {
  for(const socket of sockets)if(socket.readyState<2)socket.terminate();
  if(relay.exitCode===null){const ended=once(relay,'exit');relay.kill();await ended;}
}
