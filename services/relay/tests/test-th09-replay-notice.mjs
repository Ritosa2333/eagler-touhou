import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import {createServer} from 'node:http';
import {mkdtempSync,rmSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {once} from 'node:events';
import {fileURLToPath} from 'node:url';
import {signTh09ReplayReceipt} from '../../../common/contracts/th09-replay-receipt.mjs';

const secret='replay-notice-relay-test-secret-at-least-32-bytes';
const directory=mkdtempSync(join(tmpdir(),'th09-replay-notice-'));
let saved;
const api=createServer(async (request,response) => {
  assert.equal(request.headers.authorization,'Bearer '+secret);
  let result=[];
  if (request.url.endsWith('/rounds')) {
    let body='';
    for await (const chunk of request) body+=chunk;
    saved=JSON.parse(body);
    result={players:[null,null],settlement:{id:1,author:'ritosa',settledAt:Date.now(),changes:[]},
      replayUpload:{token:'test-grant',expiresAt:Date.now()+60000}};
  } else if (request.url.includes('/history?')) result={entries:[],ratings:[],throughId:0};
  response.writeHead(200,{'Content-Type':'application/json'}).end(JSON.stringify(result));
});
await new Promise(resolve => api.listen(0,'127.0.0.1',resolve));
const reserve=createServer();
await new Promise(resolve => reserve.listen(0,'127.0.0.1',resolve));
const port=reserve.address().port;
await new Promise(resolve => reserve.close(resolve));
const child=spawn(process.execPath,[fileURLToPath(new URL('../src/netplay-relay.mjs',import.meta.url))],{
  windowsHide:true,stdio:['ignore','pipe','pipe'],env:{...process.env,
    ACCOUNT_RELAY_SECRET:secret,CHAT_BRIDGE_SECRET:secret,
    ACCOUNT_API_INTERNAL_URL:`http://127.0.0.1:${api.address().port}`,
    EAGLER_NETPLAY_TH09_OUTBOX:join(directory,'outbox.sqlite'),
    EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:String(port),
    EAGLER_NETPLAY_GAMEPLAY_RELAY:'1',EAGLER_NETPLAY_PUBLIC_NICKNAMES:'0',
  },
});
let log='';
child.stdout.on('data',bytes=>log+=bytes);
child.stderr.on('data',bytes=>log+=bytes);
const sockets=[];
async function wait(condition,label) {
  const deadline=Date.now()+5000;
  while(!condition()) {
    if(Date.now()>deadline) throw Error(label+'\n'+log);
    await new Promise(resolve=>setTimeout(resolve,5));
  }
}
async function open(query) {
  const ws=new WebSocket(`ws://127.0.0.1:${port}/?${query}`),row={ws,messages:[]};
  sockets.push(ws);
  ws.addEventListener('message',event=>{
    if(typeof event.data==='string')row.messages.push(JSON.parse(event.data));
  });
  await wait(()=>ws.readyState===WebSocket.OPEN,'open');
  return row;
}
const send=(row,message)=>row.ws.send(JSON.stringify(message));
const room=row=>row.messages.findLast(message=>typeof message.room==='object')?.room;
const notices=row=>row.messages.filter(message=>message.type==='th09-rating'&&message.settlement.replayUploadedAt);
try {
  await wait(()=>log.includes('listening'),'relay ready');
  const id='th09mp-628174';
  const host=await open(`room=${id}&lobby=notice_host&member=notice_member_host`);
  const guest=await open(`room=${id}&lobby=notice_guest&member=notice_member_guest`);
  send(host,{type:'take-seat',seat:0,loadout:0});
  send(guest,{type:'take-seat',seat:1,loadout:1});
  await wait(()=>room(host)?.seats?.[0]&&room(host)?.seats?.[1],'seats');
  send(host,{type:'set-ready',ready:true});send(guest,{type:'set-ready',ready:true});
  await wait(()=>room(host)?.seats.slice(0,2).every(seat=>seat?.ready),'ready');
  send(host,{type:'start'});
  await wait(()=>host.messages.some(message=>message.type==='start'),'start');
  const base=`room=${id}&run=1&players=2&signal=1`;
  const first=await open(base+'&player=0&member=notice_member_host');
  const second=await open(base+'&player=1&member=notice_member_guest');
  const result={type:'th09-match',serial:1,start:100,frames:3600,winner:0,characters:[0,1]};
  send(first,result);send(second,result);
  await wait(()=>first.messages.some(message=>message.type==='th09-replay-upload'),'upload grant');
  const claim={duelId:saved.duelId,roundId:saved.roundId,uploadedAt:Date.now()};
  const receipt=signTh09ReplayReceipt(secret,claim);
  send(first,{type:'th09-replay-saved',receipt:signTh09ReplayReceipt('wrong-key',claim)});
  send(first,{type:'th09-replay-saved',receipt:signTh09ReplayReceipt(secret,{...claim,roundId:'1:999'})});
  await new Promise(resolve=>setTimeout(resolve,40));
  assert.equal(notices(host).length,0);
  send(first,{type:'th09-replay-saved',receipt});
  send(second,{type:'th09-replay-saved',receipt});
  await wait(()=>notices(host).length===1&&notices(guest).length===1,'saved notice');
  send(first,{type:'th09-replay-saved',receipt});
  await new Promise(resolve=>setTimeout(resolve,40));
  assert.equal(notices(host).length,1);
  assert.equal(notices(host)[0].settlement.replayUploadedAt,claim.uploadedAt);
  console.log('Replay upload confirmation broadcasts once and rejects forged or unrelated receipts PASS');
} finally {
  for(const socket of sockets)socket.close();
  const closed=once(child,'exit');
  child.kill();
  await closed;
  await new Promise(resolve=>api.close(resolve));
  rmSync(directory,{recursive:true,force:true});
}
