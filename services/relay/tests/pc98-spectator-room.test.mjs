import {test} from 'node:test';
import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import net from 'node:net';
import {fileURLToPath} from 'node:url';

for (const route of ['rtc','relay']) test(`PC98 room admits a late viewer over ${route} and retains confirmed packets`,async()=>{
  const port=await new Promise(resolve=>{const server=net.createServer();server.listen(0,'127.0.0.1',()=>{const p=server.address().port;server.close(()=>resolve(p));});});
  const child=spawn(process.execPath,[fileURLToPath(new URL('../src/netplay-relay.mjs',import.meta.url))],{windowsHide:true,env:{...process.env,EAGLER_NETPLAY_GAMEPLAY_RELAY_FALLBACK:'1',EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:String(port),EAGLER_NETPLAY_STUN_URLS:'',EAGLER_NETPLAY_MAX_ROOMS_PER_IP:'24',EAGLER_NETPLAY_ROOM_LOOKUPS_PER_MINUTE:'24',EAGLER_NETPLAY_MAX_CONNECTIONS_PER_IP:'160',EAGLER_NETPLAY_CONNECTIONS_PER_MINUTE:'480'},stdio:['ignore','pipe','pipe']});
  const sockets=[];let log='';child.stdout.on('data',b=>log+=b);child.stderr.on('data',b=>log+=b);
  async function wait(fn,label){const deadline=Date.now()+10000;while(!fn()){if(Date.now()>deadline)throw Error(label+' timeout\n'+log);await new Promise(resolve=>setTimeout(resolve,10));}}
  async function open(query){const ws=new WebSocket(`ws://127.0.0.1:${port}/?${query}`),client={ws,json:[],binary:[],closed:false};sockets.push(ws);ws.binaryType='arraybuffer';
    ws.onmessage=e=>typeof e.data==='string'?client.json.push(JSON.parse(e.data)):client.binary.push(new Uint8Array(e.data));ws.onclose=()=>client.closed=true;
    await wait(()=>ws.readyState===1||client.closed,'connect');assert.equal(ws.readyState,1,query+'\n'+log);return client;}
  const send=(c,value)=>c.ws.send(JSON.stringify(value)),room=c=>c.json.findLast(v=>v.room&&typeof v.room==='object')?.room;
  const packet=(kind,first,count,body)=>{const p=new Uint8Array(17+body.length),v=new DataView(p.buffer);p.set([0xe8,80,57,56,83,1,kind,2,0]);v.setUint32(9,first,true);v.setUint32(13,count,true);p.set(body,17);return p;};
  try{
    await wait(()=>log.includes('listening'),'server');
    for(const game of ['01','02','03','04']){
      const id=`th${game}mp-1234`,base=`room=${id}&run=1&players=2`;
      const a=await open(`room=${id}&lobby=host_client_${game}&member=member_host${game}`),b=await open(`room=${id}&lobby=guest_client_${game}&member=member_guest${game}`);
      send(a,{type:'take-seat',seat:0,loadout:0});send(b,{type:'take-seat',seat:1,loadout:0});await wait(()=>room(a)?.seats?.[0]&&room(a)?.seats?.[1],'seats');
      for(const c of [a,b])send(c,{type:'resource-progress',status:'ready',stage:'runtime',percent:100});
      await wait(()=>room(a)?.seats?.slice(0,2).every(s=>s?.resource?.status==='ready'),'resources');
      for(const c of [a,b])send(c,{type:'set-ready',ready:true});await wait(()=>room(a)?.seats?.slice(0,2).every(s=>s?.ready),'ready');
      send(a,{type:'start',adonisMode:game==='01'?1:0});await wait(()=>a.json.some(m=>m.type==='start'||m.type==='error'),'start');assert.equal(a.json.at(-1).type,'start',JSON.stringify(a.json.at(-1)));
      const p0=await open(base+`&player=0&member=member_host${game}`),p1=await open(base+`&player=1&member=member_guest${game}`);
      const s0=await open(base+`&player=0&signal=1&member=member_host${game}`),s1=await open(base+`&player=1&signal=1&member=member_guest${game}`);
      send(s0,{type:route==='rtc'?'rtc-ready':'rtc-failed'});send(s1,{type:route==='rtc'?'rtc-ready':'rtc-failed'});await wait(()=>s0.json.some(m=>m.type==='route'&&m.mode===route),'route');
      p0.ws.send(packet(1,0,0,[1]));p0.ws.send(packet(2,0,1,[2]));
      await wait(()=>room(a)?.spectatorTimeline===true,'spectator available');
      const lobby=await open(`room=${id}&lobby=viewer_client_${game}&member=member_viewer${game}`);send(lobby,{type:'spectate'});
      await wait(()=>room(a)?.spectators?.some(s=>s.clientId==='viewer_client_'+game),'admission');
      const viewer=await open(base+`&spectator=viewer_client_${game}&member=member_viewer${game}`);await wait(()=>viewer.binary.length>=2,'confirmed history');
      assert.deepEqual(viewer.binary.slice(0,2).map(p=>p[5]),[1,2]);
      p0.ws.send(packet(3,1,0,[]));await wait(()=>viewer.binary.some(p=>p[5]===3),'end');
      p0.ws.send(packet(2,0,1,[9]));await wait(()=>viewer.closed,'invalid spectator stream closes only the viewer');
      assert.equal(p0.closed,false);assert.equal(p1.closed,false);
      if(route==='relay'){p0.ws.send(new Uint8Array([0xe7,1,9,8,7]));await wait(()=>p1.binary.some(p=>p.at(-1)===7),'players continue');}
      for(const socket of sockets.splice(0))socket.close();
    }
  }finally{for(const socket of sockets)socket.close();child.kill();}
});
