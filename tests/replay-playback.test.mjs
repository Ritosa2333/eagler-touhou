import assert from 'node:assert/strict';
import {test} from 'node:test';
import {createMultiplayerRules} from '../../th128/site/runtime/multiplayer-rules.mjs';
import {createConfirmedHistory,readMultiplayerReplay} from '../runtime/confirmed-history.mjs';
import {openTh128ReplayPlayback} from '../runtime/replay-playback.mjs';

const step=1000/60;
async function archive(count){
  const build={coreSHA256:'a'.repeat(64),resourceSHA256:'b'.repeat(64)};
  const rules=createMultiplayerRules({schema:'th128-multiplayer-rules/1',mode:'parallel-stage',matchId:'test-replay-clock',epoch:4,
    route:'A1-3',seed:34772,inputDelay:0,rollbackWindow:2,...build});
  const history=await createConfirmedHistory({rules,allowedMask:763});let prior=[0,0];
  for(let frame=0;frame<count;frame++){
    const next=frame%3===0?[1,128]:[0,0];
    history.append({frame,inputs:next.map((held,player)=>({player,held,pressed:held&~prior[player],released:prior[player]&~held,predicted:false}))});
    prior=next;
  }
  history.seal('restart');return readMultiplayerReplay(history.replayBytes(),build);
}

function worlds(){
  const opened=[],painted=[];
  function openWorld(rules){
    const world={rules,ticks:[],closes:0,engine:{step(row){world.ticks.push(row);},close(){world.closes++;}}};
    opened.push(world);return world;
  }
  function render(world,seat,cursor){painted.push({world,seat,cursor});}
  return {opened,painted,openWorld,render};
}

test('archived two-seat input plays at 60 logical ticks per active second and stops at EOF',async()=>{
  const tape=await archive(65),fixture=worlds(),owner=await openTh128ReplayPlayback({tape,...fixture});
  assert.deepEqual(owner.status(),{phase:'paused',seat:0,frame:0,total:65,end:'restart'});
  assert.equal(owner.play(),true);
  for(let i=0;i<=60;i++) owner.advance(i*step);
  assert.equal(owner.status().frame,60);
  for(let i=61;i<70;i++) owner.advance(i*step);
  assert.deepEqual(owner.status(),{phase:'ended',seat:0,frame:65,total:65,end:'restart'});
  const [world]=fixture.opened;
  assert.deepEqual(world.ticks,Array.from({length:65},(_,frame)=>tape.read(frame)));
  assert.equal(fixture.painted.at(-1).cursor,65);
  assert.equal(owner.play(),false);
  owner.close();owner.close();assert.equal(world.closes,1);
});

test('pause, visibility changes and a discontinuity do not turn wall time into game input',async()=>{
  const fixture=worlds(),owner=await openTh128ReplayPlayback({tape:await archive(40),...fixture});
  owner.play();owner.advance(0);owner.advance(step);assert.equal(owner.status().frame,1);
  owner.pause();owner.advance(1000);assert.equal(owner.status().frame,1);
  owner.play();owner.advance(2500);owner.advance(2500+step);assert.equal(owner.status().frame,2);
  owner.advance(3000,false);owner.advance(7000,false);owner.advance(7100);owner.advance(7100+step);
  assert.equal(owner.status().frame,3);
  owner.advance(8500);assert.equal(owner.status().frame,3);
  owner.advance(8600);assert.equal(owner.status().frame,6,'catch-up is bounded to three ticks per presentation');
  owner.close();
});

test('view switching never consumes a frame and replay rebuilds the original two tracks',async()=>{
  const fixture=worlds(),owner=await openTh128ReplayPlayback({tape:await archive(3),...fixture});
  owner.play();owner.advance(0);owner.advance(step);
  owner.selectSeat(1);assert.equal(owner.status().frame,1);
  assert.deepEqual({...fixture.painted.at(-1),world:null},{world:null,seat:1,cursor:1});
  assert.throws(()=>owner.selectSeat(2),/seat/);
  const prior=fixture.opened[0];
  assert.equal(await owner.restart(),true);
  assert.equal(prior.closes,1);assert.equal(fixture.opened.length,2);
  assert.equal(owner.status().phase,'paused');assert.equal(owner.status().seat,1);assert.equal(owner.status().frame,0);
  owner.play();owner.advance(5000);owner.advance(5000+step);
  assert.deepEqual(prior.ticks,fixture.opened[1].ticks);
  owner.close();assert.deepEqual(fixture.opened.map(world=>world.closes),[1,1]);
});

test('an abort during restart closes the active world and any late candidate exactly once',async()=>{
  const tape=await archive(3),fixture=worlds(),controller=new AbortController();
  let release;
  const owner=await openTh128ReplayPlayback({tape,signal:controller.signal,render:fixture.render,
    openWorld(rules){return fixture.opened.length ? new Promise(resolve=>{release=()=>resolve(fixture.openWorld(rules));}) : fixture.openWorld(rules);}});
  const restarting=owner.restart();
  assert.equal(owner.status().phase,'opening');
  controller.abort();release();
  assert.equal(await restarting,false);assert.equal(owner.status().phase,'closed');
  assert.deepEqual(fixture.opened.map(world=>world.closes),[1,1]);
});

test('a failed replay restart preserves the old World and cannot consume pending time',async()=>{
  const tape=await archive(3),fixture=worlds();
  const owner=await openTh128ReplayPlayback({tape,render:fixture.render,openWorld(rules){
    if(fixture.opened.length)throw new Error('replacement could not open');return fixture.openWorld(rules);
  }});
  owner.play();owner.advance(0);owner.advance(step);
  await assert.rejects(owner.restart(),/replacement could not open/);
  assert.equal(owner.status().phase,'playing');assert.equal(owner.status().frame,1);
  assert.equal(fixture.opened[0].closes,0);
  owner.advance(2000);assert.equal(owner.status().frame,1);
  owner.advance(2000+step);assert.equal(owner.status().frame,2);
  owner.close();
});

test('seek runs only confirmed rows in bounded visible batches; backward seek rebuilds without drawing an intermediate frame',async()=>{
  const tape=await archive(12),fixture=worlds(),owner=await openTh128ReplayPlayback({tape,...fixture});
  owner.play();owner.advance(0);owner.advance(step);owner.selectSeat(1);
  const initialDraws=fixture.painted.length;
  await owner.seek(10);assert.equal(owner.status().phase,'seeking');assert.equal(owner.status().target,10);
  assert.equal(owner.advance(1000,false).advanced,0);assert.equal(owner.status().frame,1);
  assert.equal(owner.advance(1100).advanced,4);assert.equal(owner.advance(1120).advanced,4);
  assert.equal(fixture.painted.length,initialDraws);
  assert.equal(owner.advance(1140).advanced,1);assert.equal(owner.status().phase,'paused');
  assert.deepEqual(fixture.opened[0].ticks,Array.from({length:10},(_,i)=>tape.read(i)));
  await owner.seek(3);assert.equal(owner.status().frame,0);assert.equal(fixture.opened[0].closes,1);
  owner.advance(1160);assert.equal(owner.status().frame,3);assert.equal(owner.status().seat,1);
  assert.equal(fixture.painted.at(-1).cursor,3);assert.equal(fixture.painted.at(-1).seat,1);
  assert.deepEqual(fixture.opened[1].ticks,Array.from({length:3},(_,i)=>tape.read(i)));
  await assert.rejects(owner.seek(13),/bounds/);owner.close();
});
