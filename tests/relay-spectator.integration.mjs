import assert from 'node:assert/strict';
import {setTimeout as delay} from 'node:timers/promises';
import {createMultiplayerRules} from '../../th128/site/runtime/multiplayer-rules.mjs';
import {createConfirmedHistory,readMultiplayerReplay} from '../runtime/confirmed-history.mjs';
import {createTh128SpectatorEncoder,createTh128SpectatorDecoder} from '../runtime/spectator-wire.mjs';

export async function verifyLocalSpectatorRelay(port,{beforeSocket,runtimeGeneration,endReason='restart',signal}){
  assert.ok(runtimeGeneration===undefined||/^[a-f0-9]{64}$/.test(runtimeGeneration));
  assert.ok(['restart','completed'].includes(endReason));
  const room='th128mp-8154',matchId='th128-spectator-integration',build={coreSHA256:'a'.repeat(64),resourceSHA256:'b'.repeat(64)};
  const root=`ws://127.0.0.1:${port}/`,connections=[],checks=[],errors=[];
  const runtimePin=runtimeGeneration?{runtimeGeneration}:{};
  const check=(name,result)=>{assert.ok(result,name);checks.push(name);};
  const wait=async(fn,what)=>{
    const end=Date.now()+2500;
    do{signal?.throwIfAborted();assert.deepEqual(errors,[]);const value=fn();if(value)return value;await delay(8);}while(Date.now()<end);
    throw Error(`Local Relay timeout: ${what}`);
  };
  async function connect(query){
    beforeSocket();const ws=new WebSocket(root+'?'+new URLSearchParams({room,...query}));ws.binaryType='arraybuffer';
    const client={ws,rows:[],closeCode:null};connections.push(client);
    ws.addEventListener('close',event=>{client.closeCode=event.code;});
    ws.addEventListener('error',()=>errors.push(`socket error ${Object.keys(query).join('/')}`));
    ws.addEventListener('message',event=>{
      try{client.rows.push(typeof event.data==='string'?JSON.parse(event.data):new Uint8Array(event.data));}
      catch(error){errors.push(error.message);}
    });
    await wait(()=>ws.readyState===1,`connect ${Object.keys(query).join('/')}`);return client;
  }
  const send=(client,message)=>client.ws.send(JSON.stringify(message));
  const state=client=>client.rows.findLast(message=>message.room)?.room;
  const names=['th128-host','th128-peer','th128-observer'];
  const members=['local-host-member','local-peer-member','local-observer-member'];
  const lobby=(seat,intent)=>connect({lobby:names[seat],member:members[seat],players:'2',intent,visibility:'private',
    ...(runtimeGeneration&&intent==='create'?{difficulty:'0'}:{})});
  let run='1';
  const gameplay=(seat,signal=false)=>connect({run,players:'2',player:String(seat),member:members[seat],...runtimePin,...(signal?{signal:'1'}:{})});
  const encoder=async(epoch,logs)=>{
    const rules=createMultiplayerRules({schema:'th128-multiplayer-rules/1',matchId,epoch,mode:'parallel-stage',route:'A1-3',seed:34772,
      ...build,inputDelay:0,rollbackWindow:2});
    const log=await createConfirmedHistory({rules,allowedMask:763});if(logs)logs.push(log);
    return createTh128SpectatorEncoder(log.header,build);
  };
  const frame=(at,pair,previous=[0,0])=>({frame:at,inputs:pair.map((held,player)=>({player,held,
    pressed:held&~previous[player],released:previous[player]&~held,predicted:false}))});
  try{
    const host=await lobby(0,'create');await wait(()=>state(host),'host room created');
    const player=await lobby(1,'join');await wait(()=>state(player),'peer sees room');
    for(const [n,l] of [host,player].entries())send(l,{type:'take-seat',seat:n,loadout:0});
    await wait(()=>state(host)?.seats.slice(0,2).every(Boolean),'assigned author lobby seats');
    for(const client of [host,player])send(client,{type:'resource-progress',status:'ready',stage:'runtime',percent:100});
    await wait(()=>state(host)?.seats.slice(0,2).every(seat=>seat?.resource?.status==='ready'),'declared local fixture resources ready');
    for(const client of [host,player])send(client,{type:'set-ready',ready:true,...runtimePin});
    await wait(()=>state(host)?.seats.slice(0,2).every(seat=>seat?.ready),'both author lobby ready');
    send(host,{type:'start',adonisMode:0,inputDelay:0,predictionLimit:2});
    const start=await wait(()=>host.rows.find(message=>message.type==='start'),'author lobby start');
    run=String(start.serial);check('the author run has exactly the two assigned players',
      state(host).playerCount===2&&state(host).seats.slice(0,2).every((seat,index)=>seat.clientId===names[index]));
    if(runtimeGeneration)check('both author seats and the started Normal room attest the exact program generation',
      state(host).difficulty===0&&start.room.runtimeGeneration===runtimeGeneration&&
      state(host).seats.slice(0,2).every(seat=>seat.runtimeGeneration===runtimeGeneration));
    const p0=await gameplay(0),p1=await gameplay(1),s0=await gameplay(0,true),s1=await gameplay(1,true);
    send(s0,{type:'rtc-failed'});send(s1,{type:'rtc-failed'});
    const route=await wait(()=>s0.rows.find(message=>message.type==='route'),'author Relay selection');
    check('the author selected the WebSocket gameplay path',route.mode==='relay');
    const logs=[];const old=await encoder(1,logs);send(p0,old.announce());p0.ws.send(old.open());
    const first=frame(0,[64,128]),second=frame(1,[0,128],[64,128]);
    p0.ws.send(old.frame(first));p0.ws.send(old.frame(second));
    await wait(()=>{
      if(p0.closeCode)throw new Error(`TH128 spectator upload denied by Relay (${p0.closeCode})`);
      return state(host)?.spectatorTimeline===true;
    },'author snapshots advertise a live TH128 history');
    const observer=await lobby(2,'join');send(observer,{type:'spectate'});
    await wait(()=>observer.rows.find(message=>message.type==='spectator-start'),'late viewer was admitted');
    check('late viewer does not take a player slot',state(host).seats.slice(0,2).every((seat,index)=>seat.clientId===names[index]));
    const view=await connect({run,players:'2',spectator:names[2],member:members[2],...runtimePin});
    const timeline=createTh128SpectatorDecoder({build,matchId});let read=0;
    async function receiveUntil(fn,name){
      while(!fn()){
        await wait(()=>view.rows[read],name);
        const message=view.rows[read++];if(message instanceof Uint8Array)await timeline.accept(message);
        else if(message.type==='spectator-session')timeline.announce(message);
        else throw Error(`Unexpected viewer message ${message.type}`);
      }
    }
    await receiveUntil(()=>timeline.status().bootstrapped,'author checkpoint and fixed confirmed frontier');
    check('late viewer reconstructs both retained confirmed inputs',timeline.status().nextFrame===2);
    assert.deepEqual(timeline.read(0),first);assert.deepEqual(timeline.read(1),second);
    const tail=frame(2,[1,0],[0,128]);p0.ws.send(old.frame(tail));p0.ws.send(old.finish(endReason));
    await receiveUntil(()=>timeline.status().end===endReason,'last confirmed frame and End');
    check('live viewer receives the confirmed tail before the next epoch',timeline.status().nextFrame===3);
    assert.deepEqual(timeline.read(2),tail);
    for(const row of [first,second,tail])logs[0].append(row);
    logs[0].seal(endReason);
    const replay=await readMultiplayerReplay(logs[0].replayBytes(),build),retained=timeline.retain();
    assert.deepEqual(replay.status(),retained.status());
    for(let frame=0;frame<3;frame++)assert.deepEqual(replay.read(frame),retained.read(frame));
    checks.push('sealed file and actual Relay viewer agree on all rows and End reason');
    const next=await encoder(2);send(p0,next.announce());p0.ws.send(next.open());p0.ws.send(next.frame(frame(0,[128,64])));
    await receiveUntil(()=>timeline.status().epoch===2&&timeline.status().nextFrame===1,'live epoch 2 prefix');
    assert.deepEqual(timeline.read(0),frame(0,[128,64]));
    check('new epoch resets the viewer without reusing old inputs',timeline.status().end===null);
    assert.deepEqual(retained.status(),{nextFrame:3,end:endReason});
    if(runtimeGeneration)return {checks,sockets:connections.length,worlds:0,devicePages:0,transport:route.mode,
      viewerPrefix:2,viewerTail:3,nextEpoch:2,endReason,runtimeGeneration};
    // A voluntary policy check in this owned room: the documented spectator
    // socket must close on attempted writes without stopping either player.
    view.ws.send(new Uint8Array([0x7b,0x7d]));await wait(()=>view.closeCode,'receive-only enforcement');
    check('Relay rejects writes from the viewer only',view.closeCode===1008&&p0.ws.readyState===1&&p1.ws.readyState===1);
    p0.ws.send(new Uint8Array([0x7b,0x7d]));
    const payload=await wait(()=>p1.rows.find(row=>row instanceof Uint8Array),'player path after viewer denial');
    check('bound player continues with the author seat envelope',payload.join(',')==='230,0,123,125');
    return {checks,sockets:connections.length,worlds:0,devicePages:0,transport:route.mode,viewerPrefix:2,viewerTail:3,nextEpoch:2};
  }finally{
    for(const {ws} of connections)if(ws.readyState<2)ws.close();
    const until=Date.now()+2500;while(connections.some(({ws})=>ws.readyState!==3)&&Date.now()<until)await delay(8);
    assert.ok(connections.every(({ws})=>ws.readyState===3),'owned Relay sockets did not close');
  }
}
