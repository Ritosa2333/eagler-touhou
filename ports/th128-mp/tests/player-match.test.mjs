import assert from 'node:assert/strict';
import {test} from 'node:test';
import {createMultiplayerRules} from '../../th128/site/runtime/multiplayer-rules.mjs';
import {readMultiplayerReplay} from '../runtime/confirmed-history.mjs';
import {createPlayerMatch} from '../runtime/player-match.mjs';

const build={coreSHA256:'a'.repeat(64),resourceSHA256:'b'.repeat(64)};
const wire={schema:'th128-multiplayer-rules/1',matchId:'real-owner-test',epoch:1,mode:'parallel-stage',route:'A1-3',
  seed:34772,...build,inputDelay:0,rollbackWindow:0};
async function pair({failure=false,options={},rulesPatch={},beforeOpen=()=>{},effects=()=>undefined}={}){
  const messages=[],sent=[],owners=[[],[]];
  const peers=await Promise.all([0,1].map(localPlayer=>createPlayerMatch({
    rules:createMultiplayerRules({...wire,...rulesPatch}),localPlayer,allowedMask:763,...options,
    send:message=>{const delivery=[localPlayer,JSON.parse(JSON.stringify(message))];sent.push(delivery);messages.push(delivery);},
    async openEngine(rules){
      if(failure&&localPlayer===1&&rules.epoch===2)throw new Error('engine preparation failed');
      await beforeOpen(rules,localPlayer);
      const frames=[],entry={epoch:rules.epoch,frames,closed:false};owners[localPlayer].push(entry);
      return {checkpoint(){return frames.length;},restore(length){frames.length=length;},
        step(row){assert.equal(entry.closed,false);assert.equal(row.frame,frames.length);frames.push(row.inputs.map(input=>input.held));return effects(entry,row);},
        close(){assert.equal(entry.closed,false);entry.closed=true;}};
    },
  })));
  const deliver=async()=>{
    for(let limit=0;messages.length;limit++){
      assert.ok(limit<40,'unbounded messages');
      const [from,message]=messages.shift();await peers[1-from].receive(message,from);
    }
  };
  const start=async()=>{for(const peer of peers)peer.announceStart();await deliver();};
  const frame=async(a,b)=>{
    peers[0].tick(a);peers[1].tick(b);await deliver();peers[0].tick(a);peers[1].tick(b);await deliver();
  };
  const ready=async()=>{for(const peer of peers)await peer.waitRestart();await deliver();};
  return {peers,owners,start,frame,deliver,ready,messages,sent};
}

test('both terminal seats seal one confirmed completed Replay and keep it through a synchronized restart',async()=>{
  const {peers:[a,b],owners,start,frame,deliver,ready,sent}=await pair({effects:(entry,row)=>
    ({bothGameOver:entry.epoch===1&&row.frame>=1})});
  try{
    await start();await frame(64,128);assert.equal(a.previousReplay(),null);
    await frame(0,0);
    const source=a.spectatorSource(),archive=a.previousReplay();
    assert.deepEqual(source.status(),{nextFrame:2,end:'completed'});
    assert.deepEqual(a.replayReceipt(),{epoch:1,end:'completed'});
    assert.deepEqual(archive,b.previousReplay());
    assert.deepEqual((await readMultiplayerReplay(archive,build)).status(),source.status());
    const messages=sent.length;
    for(const peer of [a,b])assert.deepEqual(peer.tick(()=>assert.fail('completed session sampled input')),{advanced:false,reason:'round-completed'});
    assert.equal(sent.length,messages);
    a.requestRestart();b.requestRestart();await deliver();await ready();
    assert.equal(a.status().epoch,2);assert.equal(b.status().epoch,2);
    assert.deepEqual(a.previousReplay(),archive);assert.deepEqual(b.previousReplay(),archive);
    assert.deepEqual(source.status(),{nextFrame:2,end:'completed'});
    await frame(128,64);assert.deepEqual(owners.map(seat=>seat[1].frames),[[[128,64]],[[128,64]]]);
  }finally{a.close();b.close();}
});

test('a predicted finished pair discarded by delayed authenticated input does not leak a terminal Replay',async()=>{
  const {peers:[a,b],start,deliver,messages}=await pair({rulesPatch:{rollbackWindow:2},effects:(_entry,row)=>
    ({bothGameOver:row.frame===1&&row.inputs[1].held===0})});
  try{
    await start();a.tick(64);b.tick(0);await deliver();
    a.tick(64);b.tick(128);
    assert.equal(a.status().history.end,null);assert.equal(a.previousReplay(),null);
    // The newer actual P2 input proves the prediction at frame 1 was wrong.
    const incoming=messages.findIndex(([seat,row])=>seat===1&&row.frame===1);
    assert.notEqual(incoming,-1);const [from,packet]=messages.splice(incoming,1)[0];
    await a.receive(packet,from);await deliver();
    assert.equal(a.status().history.end,null);assert.equal(b.status().history.end,null);
    assert.equal(a.previousReplay(),null);assert.equal(a.status().round.replayedFrames,1);
  }finally{a.close();b.close();}
});

test('late confirmation seals at the first terminal row and discards speculative post-terminal timeline slots',async()=>{
  const {peers:[a,b],start,deliver,messages,sent}=await pair({rulesPatch:{rollbackWindow:2},effects:(_entry,row)=>
    ({bothGameOver:row.frame>=1})});
  try{
    await start();a.tick(64,{allowPrediction:false});b.tick(0,{allowPrediction:false});await deliver();
    a.tick(64);b.tick(0);assert.equal(a.status().history.nextFrame,1);assert.equal(b.status().history.nextFrame,1);
    a.tick(64);a.tick(64);b.tick(0);
    assert.equal(a.status().history.nextFrame,1);assert.equal(a.status().round.pendingFrames,2);
    assert.equal(a.previousReplay(),null,'a terminal effect on an unconfirmed input cannot seal');
    const late=messages.findIndex(([seat,row])=>seat===1&&row.frame===1);
    assert.notEqual(late,-1);const [seat,row]=messages.splice(late,1)[0];await a.receive(row,seat);await deliver();
    assert.deepEqual(a.status().history,{nextFrame:2,end:'completed'});assert.equal(a.status().round.pendingFrames,0);
    b.flush();await deliver();
    assert.deepEqual(a.previousReplay(),b.previousReplay());
    const archive=await readMultiplayerReplay(a.previousReplay(),build);assert.equal(archive.read(2),null);
    const count=sent.length;a.tick(64);b.tick(128);await deliver();assert.equal(sent.length,count);
  }finally{a.close();b.close();}
});

test('actual match owners wait on readiness then vote, prepare and adopt the fresh World as one epoch',async()=>{
  const {peers:[a,b],owners,start,frame,deliver,ready}=await pair();
  try{
    assert.equal(a.tick(1).reason,'waiting-ready');assert.equal(owners[0][0].frames.length,0);
    await start();await frame(1,128);assert.deepEqual(owners.map(row=>row[0].frames),[[[1,128]],[[1,128]]]);
    a.requestRestart();await deliver();
    assert.equal(b.status().epoch,1);assert.equal(b.status().phase,'voting');assert.equal(owners[0].length,1);
    b.requestRestart();await deliver();await ready();
    assert.equal(a.status().epoch,2);assert.equal(b.status().epoch,2);
    assert.ok(owners.every(row=>row.length===2&&row[0].closed&&!row[1].closed));
    await assert.rejects(a.receive({matchId:wire.matchId,epoch:1,player:1,frame:1,held:128},1),/epoch/);
    const replay=await readMultiplayerReplay(a.previousReplay(),build);
    assert.equal(replay.header.rules.epoch,1);assert.equal(replay.status().end,'restart');
    assert.deepEqual(replay.read(0).inputs.map(input=>input.held),[1,128]);
    await frame(128,1);assert.deepEqual(owners.map(row=>row[1].frames),[[[128,1]],[[128,1]]]);
    const {header,chunk}=b.spectatorSource();assert.equal(header.rules.epoch,2);assert.deepEqual(chunk(0,1).frames,[[128,1]]);
  }finally{a.close();b.close();}
});

test('a failed candidate does not close, advance or replace the running World',async()=>{
  const {peers:[a,b],owners,start,frame,deliver,ready}=await pair({failure:true});
  try{
    await start();await frame(1,128);
    a.requestRestart();b.requestRestart();await deliver();await ready();
    assert.equal(a.status().epoch,1);assert.equal(b.status().epoch,1);
    assert.ok(owners.every(row=>!row[0].closed));
    assert.ok(owners[0].slice(1).every(world=>world.closed));assert.equal(a.previousReplay(),null);
    await frame(0,0);assert.deepEqual(owners.map(row=>row[0].frames),[[[1,128],[0,0]],[[1,128],[0,0]]]);
  }finally{a.close();b.close();}
});

for(const ahead of [0,1])test(`restart keeps the published prefix even when seat ${ahead} is one World step ahead`,async()=>{
  const {peers:[a,b],owners,start,deliver,ready,sent}=await pair();
  try{
    await start();a.tick(64);b.tick(128);await deliver();
    const leading=[a,b][ahead],trailing=[b,a][ahead],spectator=leading.spectatorSource();
    assert.equal(leading.tick(ahead?128:64).advanced,true);
    assert.deepEqual(spectator.chunk(0,1).frames,[[64,128]]);
    assert.equal(trailing.status().history.nextFrame,0);
    a.requestRestart();b.requestRestart();await deliver();await ready();
    assert.deepEqual([a.status().epoch,b.status().epoch],[2,2]);
    assert.deepEqual(owners.map(row=>row[0].frames),[[[64,128]],[[64,128]]]);
    assert.deepEqual(a.previousReplay(),b.previousReplay());
    assert.deepEqual((await readMultiplayerReplay(a.previousReplay(),build)).status(),{nextFrame:1,end:'restart'});
    assert.deepEqual(spectator.status(),{nextFrame:1,end:'restart'});
    assert.equal(sent.filter(([,message])=>Number.isInteger(message.frame)).length,2,'catch-up must not sample or send an input');
  }finally{a.close();b.close();}
});

test('late authentic input may complete the common restart prefix; a stop is not a neutral sample',async()=>{
  const {peers:[a,b],owners,start,deliver,ready,messages,sent}=await pair();
  try{
    await start();a.tick(64);b.tick(128);
    const [delayed,actual]=messages.splice(0);assert.equal(delayed[0],0);assert.equal(actual[0],1);
    await a.receive(actual[1],1);assert.equal(a.tick(64).advanced,true);
    a.requestRestart();b.requestRestart();await deliver();await ready();
    assert.deepEqual([a.status().epoch,b.status().epoch],[1,1],'acknowledge no new epoch until both have the same confirmed prefix');
    assert.equal(b.tick(0).reason,'restart-preparing');assert.equal(owners[1][0].frames.length,0);
    await b.receive(delayed[1],0);await deliver();await ready();
    assert.deepEqual([a.status().epoch,b.status().epoch],[2,2]);
    assert.deepEqual(a.previousReplay(),b.previousReplay());
    assert.deepEqual(owners[1][0].frames,[[64,128]]);
    assert.equal(sent.filter(([,message])=>Number.isInteger(message.frame)).length,2);
  }finally{a.close();b.close();}
});

test('an expired input wait releases the fence and the same round can retry with authentic input',async()=>{
  let time=0;
  const {peers:[a,b],owners,start,deliver,ready,messages}=await pair({options:{restartClock:()=>time,restartTimeoutMs:10}});
  try{
    await start();a.tick(64);b.tick(128);
    const [missing,actual]=messages.splice(0);await a.receive(actual[1],1);assert.equal(a.tick(64).advanced,true);
    a.requestRestart();b.requestRestart();await deliver();await ready();
    assert.equal(a.status().phase,'ready');assert.equal(b.status().phase,'preparing');
    time=11;b.flush();await deliver();
    assert.ok([a,b].every(p=>p.status().epoch===1&&p.status().phase==='running'&&p.status().attempt===2&&p.previousReplay()===null));
    assert.match(b.status().lastError,/deadline/);assert.ok(owners.every(row=>!row[0].closed));
    await b.receive(missing[1],0);assert.equal(b.tick(0).advanced,true);
    assert.deepEqual(owners.map(row=>row[0].frames),[[[64,128]],[[64,128]]]);
    a.requestRestart();b.requestRestart();await deliver();await ready();
    assert.ok([a,b].every(p=>p.status().epoch===2&&p.status().lastRestartFrameCount===1));
    assert.deepEqual(a.previousReplay(),b.previousReplay());
  }finally{a.close();b.close();}
});

test('late real input beyond both frozen prefixes cannot extend the committed old Replay',async()=>{
  const resume=Promise.withResolvers();
  const {peers:[a,b],start,deliver,ready,messages}=await pair({rulesPatch:{rollbackWindow:2},
    beforeOpen:rules=>rules.epoch===2?resume.promise:undefined});
  try{
    await start();const source=a.spectatorSource();
    assert.equal(a.tick(64).advanced,true);assert.equal(b.tick(128).advanced,true);
    const delayed=messages.splice(0);a.requestRestart();b.requestRestart();await deliver();
    assert.ok([a,b].every(p=>p.status().restartFrameCount===0));
    for(const [source,packet] of delayed)await [a,b][1-source].receive(packet,source);
    assert.deepEqual(source.status(),{nextFrame:0,end:null});
    resume.resolve();await ready();
    assert.ok([a,b].every(p=>p.status().epoch===2&&p.status().lastRestartFrameCount===0));
    assert.deepEqual(source.status(),{nextFrame:0,end:'restart'});
    assert.deepEqual(a.previousReplay(),b.previousReplay());
  }finally{resume.resolve();a.close();b.close();}
});
