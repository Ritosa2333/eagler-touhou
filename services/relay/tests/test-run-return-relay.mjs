// Real lobby, gameplay and signaling sockets. Synthetic input/history payloads
// verify room ownership and transport lifetime; they do not certify the engine.
import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import {once} from 'node:events';
import {fileURLToPath} from 'node:url';
import {WebSocket} from 'ws';
import {multiplayerConfigForProduct} from '../../../common/contracts/product-catalog.mjs';

const child=spawn(process.execPath,[fileURLToPath(new URL('../src/netplay-relay.mjs',import.meta.url))],{
  stdio:['ignore','pipe','pipe'],env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:'0',
    EAGLER_NETPLAY_GAMEPLAY_RELAY_FALLBACK:'1',EAGLER_NETPLAY_PUBLIC_NICKNAMES:'0',EAGLER_NETPLAY_STUN_URLS:'',EAGLER_NETPLAY_MAX_ROOMS_PER_IP:'2'}});
const sockets=[];let log='';
try {
  const port=await new Promise((resolve,reject)=>{
    const timer=setTimeout(()=>reject(Error('relay startup timeout\n'+log)),5000);
    child.stderr.on('data',data=>{log+=data;});
    child.stdout.on('data',data=>{log+=data;const match=/listening ws:\/\/127\.0\.0\.1:(\d+)/.exec(log);if(match){clearTimeout(timer);resolve(Number(match[1]));}});
    child.once('exit',()=>{clearTimeout(timer);reject(Error('relay exited\n'+log));});
  });
  function connect(room,params){
    const ws=new WebSocket(`ws://127.0.0.1:${port}/?${new URLSearchParams({room,...params})}`),messages=[],waiters=[];
    sockets.push(ws);
    ws.on('message',(data,binary)=>{
      const value=binary?Buffer.from(data):JSON.parse(String(data));messages.push(value);
      for(const item of [...waiters])if(item.test(value))item.resolve(value);
    });
    return {ws,messages,wait(test){
      const found=messages.find(test);if(found)return Promise.resolve(found);
      return new Promise((resolve,reject)=>{
        const timer=setTimeout(()=>{waiters.splice(waiters.indexOf(item),1);reject(Error('message timeout\n'+log));},5000);
        const item={test,resolve(value){clearTimeout(timer);waiters.splice(waiters.indexOf(item),1);resolve(value);}};waiters.push(item);
      });
    }};
  }
  const send=(client,message)=>client.ws.send(JSON.stringify(message));
  const latest=client=>client.messages.findLast(message=>message.room?.seats)?.room;
  const barrier=async client=>{const pong=once(client.ws,'pong');client.ws.ping();await pong;};
  async function state(client,message,test){client.messages.length=0;const result=client.wait(value=>value.room&&test(value.room));send(client,message);return (await result).room;}
  const probe=client=>state(client,{type:'take-seat',seat:client.seat,loadout:0},()=>true);
  async function readyAll(players){for(const player of players)await state(player,{type:'set-ready',ready:true},room=>room.seats[player.seat]?.ready);}
  async function start(players,serial){
    players[0].messages.length=0;send(players[0],{type:'start'});
    const result=await players[0].wait(m=>m.type==='start'&&m.serial===serial);assert.equal(result.room.phase,'starting');
  }
  function stageEvent(count,kind,frame,serial){
    const bytes=Buffer.alloc(kind===1?44:32);bytes.set([69,0x4d,83,86,2,kind,count,0]);
    bytes.writeBigUInt64LE(9n,8);bytes.writeUInt32LE(7,16);bytes.writeUInt32LE(frame,20);bytes.writeUInt32LE(1,24);bytes.writeUInt32LE(serial,28);
    if(kind===1){bytes.writeUInt32LE(1,32);bytes.writeUInt32LE(4,36);bytes.set([4,3,2,1],40);}return bytes;
  }
  for(const count of [2,3]){
    const room=`th13mp-${7830+count}`,players=[];
    for(let seat=0;seat<count;seat++){
      const player=connect(room,{lobby:`return_${count}_${seat}`,member:`return_member_${count}_${seat}`});player.seat=seat;players.push(player);
      const initial=await player.wait(m=>m.type==='state');assert.equal(initial.roomDirectory.runReturn,true);
      await state(player,{type:'take-seat',seat,loadout:0},value=>value.seats[seat]?.clientId===`return_${count}_${seat}`);
      if(!seat)await state(player,{type:'settings',playerCount:count},value=>value.playerCount===count);
    }
    const observer=connect(room,{lobby:`return_viewer_${count}`,member:`return_viewer_member_${count}`});await observer.wait(m=>m.type==='state');
    await state(observer,{type:'spectate'},value=>value.spectatorCount===1);
    const outsider=connect(room,{lobby:`return_outsider_${count}`,member:`return_outsider_member_${count}`});await outsider.wait(m=>m.type==='state');
    // Missing run / future serial must not poison the next start.
    for(const serial of [undefined,0,1,'1',1.5])send(players[0],{type:'run-returned',serial});await barrier(players[0]);
    await readyAll(players);await start(players,1);
    const play=[],signal=[];
    for(let seat=0;seat<count;seat++){
      const params={run:'1',players:String(count),player:String(seat),member:`return_member_${count}_${seat}`};
      const p=connect(room,params),s=connect(room,{...params,signal:'1'});play.push(p);signal.push(s);
      await once(p.ws,'open');await s.wait(m=>m.type==='peers');
    }
    for(const client of signal)send(client,{type:'rtc-failed'});
    await signal[0].wait(m=>m.type==='route');await players[0].wait(m=>m.room?.phase==='running');assert.equal(latest(players[0]).phase,'running');
    const viewer=connect(room,{run:'1',players:String(count),spectator:`return_viewer_${count}`,member:`return_viewer_member_${count}`});await once(viewer.ws,'open');
    play[0].ws.send(Buffer.concat([Buffer.from([0xe8]),stageEvent(count,1,600,1)]));
    await viewer.wait(Buffer.isBuffer);
    if(count===2){
      // Optional spectator failure does not own player completion.
      const closed=once(viewer.ws,'close');send(signal[0],{type:'spectator-stop'});const [code]=await closed;assert.equal(code,1011);
    }else{
      play[0].ws.send(Buffer.concat([Buffer.from([0xe8]),stageEvent(count,3,600,2)]));
      await viewer.wait(value=>Buffer.isBuffer(value)&&value[5]===3);
    }
    for(const serial of [undefined,0,2,-1,'1',1.5])send(players[0],{type:'run-returned',serial});
    send(observer,{type:'run-returned',serial:1,player:1});send(outsider,{type:'run-returned',serial:1,clientId:`return_${count}_1`});
    await Promise.all([barrier(observer),barrier(outsider),barrier(players[0])]);
    assert.equal((await probe(players[0])).phase,'running','invalid, observer and unseated reporters cannot release the room');
    send(players[0],{type:'run-returned',serial:1});send(players[0],{type:'run-returned',serial:1});
    await barrier(players[0]);assert.equal((await probe(players[0])).phase,'running','one player and its duplicates do not count as two');
    // A public lobby ID cannot be resumed with a replacement private member.
    const replaced=connect(room,{lobby:`return_${count}_1`,member:`replacement_member_${count}`});
    const [replacedCode]=await once(replaced.ws,'close');assert.equal(replacedCode,1008);
    for(let seat=1;seat<count;seat++){
      const last=seat===count-1;
      if(last)players[0].messages.length=0;
      send(players[seat],{type:'run-returned',serial:1});await barrier(players[seat]);
      if(last)await players[0].wait(m=>m.room?.phase==='lobby');
      else assert.equal((await probe(players[0])).phase,'running');
    }
    assert.ok(latest(players[0]).seats.slice(0,count).every(seat=>seat&&!seat.ready));
    for(const client of [...play,...signal])assert.equal(client.ws.readyState,WebSocket.OPEN,'return preserves terminal transport');
    const tail=Buffer.from([11,22,33]);play[0].ws.send(Buffer.concat([Buffer.from([0xe7,1]),tail]));
    assert.deepEqual(await play[1].wait(Buffer.isBuffer),Buffer.concat([Buffer.from(multiplayerConfigForProduct('th13mp').relaySourceEnvelope?[0xe6,0]:[]),tail]),'old input repair can finish after lobby reset');
    send(signal[0],{type:'signal',to:1,candidate:{candidate:'terminal-tail'}});
    await signal[1].wait(m=>m.type==='signal'&&m.candidate?.candidate==='terminal-tail');
    if(count===3){await barrier(viewer);assert.equal(viewer.ws.readyState,WebSocket.OPEN);assert.equal(viewer.messages.filter(Buffer.isBuffer).at(-1)[5],3,'completed history remains available');}
    await readyAll(players);
    for(const player of players)send(player,{type:'run-returned',serial:1});
    await Promise.all(players.map(barrier));assert.ok((await probe(players[0])).seats.slice(0,count).every(seat=>seat?.ready),'duplicates cannot clear new Ready');
    await start(players,2);
    for(const player of players)send(player,{type:'run-returned',serial:1});
    await Promise.all(players.map(barrier));assert.equal((await probe(players[0])).phase,'starting','old serial cannot return a new run');
    send(players[0],{type:'run-returned',serial:2});await barrier(players[0]);assert.equal((await probe(players[0])).phase,'starting','previous returned set cannot release the next run');
    for(let seat=1;seat<count;seat++)send(players[seat],{type:'run-returned',serial:2});
    await players[0].wait(m=>m.room?.phase==='lobby'&&m.room.startSerial===2);
    console.log(`PASS ${count} players: all-seat logical return, serial/member negative controls, Ready preservation and old transport/history retention`);
  }
} finally {
  for(const socket of sockets)if(socket.readyState<2)socket.terminate();
  if(child.exitCode===null){const done=once(child,'exit');child.kill();await done;}
}
