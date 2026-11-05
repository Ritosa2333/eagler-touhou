import test from 'node:test';
import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import {mkdtempSync, rmSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {join, resolve, sep} from 'node:path';
import {fileURLToPath} from 'node:url';
import net from 'node:net';
import {WebSocket} from 'ws';
import {buildNetplayRelay} from '../scripts/build.mjs';
import {DatabaseSync} from 'node:sqlite';

test('real worker/SQLite/relay: public names, role migration, cooldown, outages and restart',async()=>{
  const directory=mkdtempSync(join(tmpdir(),'eagler-nickname-relay-')),state=join(directory,'names.sqlite');
  const root=fileURLToPath(new URL('../',import.meta.url)),sockets=[];let child,output='',port;
  async function wait(check,label){const end=Date.now()+15000;while(!check()){if(Date.now()>end)throw Error(label+'\n'+output);await new Promise(r=>setTimeout(r,5));}}
  async function start(entry='src/netplay-relay.mjs',recover=false){
    port=await new Promise(resolve=>{const s=net.createServer();s.listen(0,'127.0.0.1',()=>{const p=s.address().port;s.close(()=>resolve(p));});});
    output='';child=spawn(process.execPath,['--import='+new URL('./fixtures/nickname-provider.mjs',import.meta.url).href,entry],{
      cwd:root,env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:String(port),
        EAGLER_NETPLAY_PUBLIC_NICKNAMES:'1',EAGLER_NETPLAY_NICKNAME_STATE:state,EAGLER_NETPLAY_NICKNAME_API_KEY:'test-only-never-public',
        EAGLER_NETPLAY_NICKNAME_API_BASE:'https://provider.example.invalid:8888/v1',
        EAGLER_TEST_NICKNAME_RECOVER:recover?'1':'0',
        EAGLER_NETPLAY_TRUSTED_PROXIES:'127.0.0.1'},stdio:['ignore','pipe','pipe']});
    child.stdout.on('data',b=>output+=b);child.stderr.on('data',b=>output+=b);
    await wait(()=>output.includes('listening ws://'),'startup');
  }
  async function stop(){for(const s of sockets)if(s.readyState<2)s.close();if(child){const exited=new Promise(r=>child.once('close',r));child.kill();await exited;child=null;}}
  async function connect(query,ip){
    const socket=new WebSocket('ws://127.0.0.1:'+port+'/?'+query,{headers:{'X-Real-IP':ip}});sockets.push(socket);
    const row={socket,messages:[]};socket.on('message',b=>{row.messages.push(JSON.parse(String(b)));});socket.on('error',()=>{});
    await wait(()=>row.messages.length>0,'connect');return row;
  }
  const member='anonymous_host_12345',guestMember='anonymous_guest_12345';
  const lobby=(room,client,m,intent='join')=>new URLSearchParams({room,lobby:client,member:m,intent,players:'2'});
  const send=(row,message)=>row.socket.send(JSON.stringify(message));
  const room=row=>row.messages.findLast(m=>m.type==='state')?.room;
  const status=row=>row.messages.findLast(m=>m.type==='nickname-status');
  try{
    await start();const viewer=await connect('directory=1&member=directory_viewer_123','203.0.113.3');
    const host=await connect(lobby('th09mp-7312','host_client_123',member,'create'),'203.0.113.1');
    await wait(()=>status(host),'profile');
    send(host,{type:'take-seat',seat:0,loadout:0,name:' Ａ\u202elice '});
    await wait(()=>room(host)?.seats[0],'seat');assert.equal(room(host).seats[0].name,'','unreviewed name is never published');
    send(host,{type:'set-name',name:' Ａ\u202elice '});
    await wait(()=>status(host)?.status==='approved','review');
    await wait(()=>room(host)?.seats[0]?.name==='Alice','room public name');
    await wait(()=>viewer.messages.findLast(m=>m.type==='directory')?.rooms?.[0]?.seats?.[0]?.name==='Alice','directory full public name');
    const publicRoom=viewer.messages.findLast(m=>m.type==='directory').rooms[0];assert.equal(publicRoom.publicNames,true);
    assert.equal('clientId' in publicRoom.seats[0],false);
    assert.equal(status(host).nextChangeAt,0,'first name permits an immediate change');
    send(host,{type:'set-name',name:'AliceNew'});await wait(()=>status(host)?.name==='AliceNew','immediate second name');
    send(host,{type:'set-name',name:'另一个昵称'});await wait(()=>status(host)?.status==='cooldown','24h cooldown');
    assert.equal(room(host).seats[0].name,'AliceNew');
    send(host,{type:'take-seat',seat:0,loadout:0,name:'伪造新昵称'});await new Promise(r=>setTimeout(r,100));assert.equal(room(host).seats[0].name,'AliceNew');
    const guest=await connect(lobby('th09mp-7312','guest_client_123',guestMember),'203.0.113.2');
    send(guest,{type:'spectate',name:'拒绝昵称'});send(guest,{type:'set-name',name:'拒绝昵称'});await wait(()=>status(guest)?.status==='rejected','spectator refusal');
    assert.ok(room(guest).spectators.every(s=>s.name===''));
    assert.equal(status(guest).attemptsLeft,1);
    send(guest,{type:'set-name',name:'Alice'});await wait(()=>status(guest)?.status==='approved','same approved name cache');
    await wait(()=>room(guest).spectators.some(s=>s.name==='Alice'),'spectator approved publication');
    send(guest,{type:'take-seat',seat:1,loadout:1,name:'拒绝昵称'});await wait(()=>room(host)?.seats[1]?.name==='Alice','seat uses server profile, not wire candidate');
    const offline=await connect(lobby('th09mp-7312','outage_client_123','anonymous_outage_123'),'203.0.113.4');
    send(offline,{type:'set-name',name:'故障昵称'});await wait(()=>status(offline)?.status==='queued','durable queue acknowledgement');assert.equal(status(offline).name,'');
    assert.equal(status(offline).pendingName,'故障昵称');
    assert.doesNotMatch(JSON.stringify([...host.messages,...viewer.messages]),/故障昵称/,'pending spelling is private to its owner');
    await stop();
    const db=new DatabaseSync(state);assert.equal(db.prepare('SELECT name FROM review_queue').get().name,'故障昵称');db.exec('UPDATE review_queue SET next_attempt=0');db.close();
    const bundled=resolve(root,'.cache','nickname-bundle-'+Date.now());
    await buildNetplayRelay(bundled);
    // The production worker resolves no authored contract/compiler files.
    await start(resolve(bundled,'netplay-relay.mjs'),true);
    const restored=await connect(lobby('th11mp-7313','restored_client_123',member,'create'),'203.0.113.1');
    await wait(()=>status(restored)?.name==='AliceNew','persistent profile');
    send(restored,{type:'set-name',name:'重启不能绕过'});await wait(()=>status(restored)?.status==='cooldown','persistent 24h');
    const resumed=await connect(lobby('th11mp-7313','outage_restored_123','anonymous_outage_123'),'203.0.113.4');
    send(resumed,{type:'spectate',name:'伪造新昵称'});await wait(()=>status(resumed)?.name==='故障昵称','queued review resumes after restart without resubmission');
    await wait(()=>room(restored)?.spectators.some(s=>s.name==='故障昵称'),'background approval publishes to the room');
    for(const row of [host,viewer,guest,offline,restored]) {
      const published=JSON.stringify(row.messages);
      assert.doesNotMatch(published,/test-only-never-public|provider\.example\.invalid/);
      assert.doesNotMatch(JSON.stringify(row.messages.filter(m=>m.type!=='nickname-status')),/伪造新昵称|拒绝昵称|重启不能绕过|另一个昵称/);
    }
  }finally{await stop();assert.ok(resolve(directory).startsWith(resolve(tmpdir())+sep));rmSync(directory,{recursive:true,force:true});}
});
