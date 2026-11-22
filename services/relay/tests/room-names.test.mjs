import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import {once} from 'node:events';
import {createServer} from 'node:http';
import {test} from 'node:test';
import {fileURLToPath} from 'node:url';
import {normalizeRoomName} from '../../../common/contracts/room-name.mjs';
import {createRoomNameReviewer} from '../src/room-name-review.mjs';

test('room name length is ten visible characters and review has its own policy', async () => {
  const ten = '弹'.repeat(10);
  assert.equal(normalizeRoomName(`  ${ten}  `),ten);
  assert.equal(normalizeRoomName(ten+'幕'),null);
  assert.equal(normalizeRoomName('𠮷'.repeat(10)),'𠮷'.repeat(10));
  assert.equal(normalizeRoomName('tag\u202E'),null);
  const calls = [];
  const review = createRoomNameReviewer({apiKey:'test',fetch:async (_url,{body})=>{
    calls.push(JSON.parse(body).messages);
    return Response.json({choices:[{finish_reason:'stop',message:{content:'1'}}]});
  }});
  assert.equal(await review('127.0.0.1',ten+'幕'),'rejected');
  assert.equal(await review('127.0.0.1',ten),'approved');
  assert.equal(calls.length,1);
  assert.match(calls[0][0].content,/联机房间名/);
  assert.equal(calls[0][1].content,JSON.stringify(ten));
});

test('room names publish only after approval and never reset ready state', {timeout:20000}, async () => {
  const pending = new Map(), clients = [];
  const bridge = createServer(async (req,res)=>{
    const chunks = [];
    for await (const chunk of req) chunks.push(chunk);
    const body = JSON.parse(Buffer.concat(chunks).toString());
    res.setHeader('content-type','application/json');
    if (req.url === '/room-name') pending.set(body.name,result=>res.end(JSON.stringify(result)));
    else res.end(JSON.stringify({name:'',status:'idle',nextChangeAt:0,retryAt:0,attemptsLeft:null}));
  });
  bridge.listen(0,'127.0.0.1');
  await once(bridge,'listening');
  const relay = spawn(process.execPath,[fileURLToPath(new URL('../src/netplay-relay.mjs',import.meta.url))],{windowsHide:true,
    env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1', EAGLER_NETPLAY_RELAY_PORT:'0',
      NICKNAME_BRIDGE_SECRET:'tag-test-only-secret'.repeat(3), NICKNAME_BRIDGE_PORT:String(bridge.address().port)},
    stdio:['ignore','pipe','pipe']});
  let log = '';
  for (const stream of [relay.stdout,relay.stderr]) stream.on('data',value=>log+=value);
  async function wait(match,label) {
    const deadline=Date.now()+5000;
    for (;;) {
      const found=match();
      if (found) return found;
      if (Date.now()>deadline) throw Error(label+'\n'+log);
      await new Promise(resolve=>setTimeout(resolve,10));
    }
  }
  try {
    const port=(await wait(()=>/listening ws:\/\/127\.0\.0\.1:(\d+)/.exec(log),'relay startup'))[1];
    async function connect(query) {
      const ws=new WebSocket(`ws://127.0.0.1:${port}/?${query}`), messages=[];
      clients.push(ws);
      ws.addEventListener('message',e=>messages.push(JSON.parse(e.data)));
      await wait(()=>ws.readyState===1,'socket open');
      return {messages, send:message=>ws.send(JSON.stringify(message)), next:(match,label)=>wait(()=>messages.findLast(match),label)};
    }
    const room='th02mp-1718', dir=await connect('directory=1&member=tags_listener');
    const host=await connect(`room=${room}&lobby=tags_host_id&member=tags_host_member&intent=create`);
    const guest=await connect(`room=${room}&lobby=tags_guest_id&member=tags_guest_member&intent=join`);
    assert.equal((await host.next(m=>m.roomNames,'capability')).roomNames,true);
    for (const [seat,client] of [host,guest].entries()) {
      client.send({type:'take-seat',seat,loadout:0});
      await host.next(m=>m.room?.seats[seat],'seat');
    }
    for (const client of [host,guest]) client.send({type:'set-ready',ready:true});
    const ready=await host.next(m=>m.room?.seats.slice(0,2).every(s=>s?.ready),'ready');
    assert.equal(ready.room.roomName,null);
    guest.send({type:'set-room-name',roomName:'非房主'});
    host.send({type:'set-room-name',roomName:'甲'.repeat(11)});
    await host.next(m=>m.type==='room-name-status'&&m.status==='invalid','over limit');
    host.send({type:'set-room-name',roomName:'练习避弹'});
    const approve=await wait(()=>pending.get('练习避弹'),'review requested');
    assert.ok(!pending.has('非房主'));
    assert.ok(!JSON.stringify(guest.messages).includes('练习避弹'));
    assert.ok(!JSON.stringify(dir.messages).includes('练习避弹'));
    approve('approved');
    const named = await guest.next(m=>m.room?.roomName==='练习避弹','approved room name');
    assert.equal(named.room.settingsVersion,ready.room.settingsVersion);
    assert.ok(named.room.seats.slice(0,2).every(s=>s?.ready));
    await dir.next(m=>m.rooms?.some(r=>r.roomName==='练习避弹'),'approved public name');
    host.send({type:'set-room-name',roomName:'未通过'});
    (await wait(()=>pending.get('未通过'),'second review'))('rejected');
    await host.next(m=>m.type==='room-name-status'&&m.status==='rejected','rejected');
    assert.equal(guest.messages.findLast(m=>m.room)?.room.roomName,'练习避弹');
    const beforeClear = dir.messages.length;
    host.send({type:'set-room-name',roomName:null});
    await guest.next(m=>m.room?.roomName===null&&m.room.seats[1]?.ready&&m.room!==ready.room,'name cleared');
    await wait(()=>dir.messages.slice(beforeClear).find(m=>m.rooms?.some(r=>r.code==='1718'&&!r.roomName)),'public name cleared');
  } finally {
    for (const ws of clients) ws.close();
    if (relay.exitCode===null) {const ended=once(relay,'exit');relay.kill();await ended;}
    bridge.closeAllConnections();
    await new Promise(resolve=>bridge.close(resolve));
  }
});
