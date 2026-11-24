import assert from 'node:assert/strict';
import {test} from 'node:test';
import {parseSpectatorPlayback,spectatorPlaybackCommand} from '../../../launcher/src/launcher/spectator-playback.mts';
import {createManagedSpectatorPlayback} from '../runtime/managed-spectator-playback.mjs';

const tick=1000/60;
function fixture(){
  const worlds=[],draws=[],receipts=[],errors=[];
  const life=new AbortController();
  function world(rules){const rows=[],x={rules,rows,closes:0,step(value){rows.push(value.frame);},close(){x.closes++;}};worlds.push(x);return x;}
  function tape(epoch,nextFrame,mode='parallel-stage'){
    const rules={epoch,stageId:3,route:'A1-3',mode},state={nextFrame,end:null};
    return {rules,state,track:{status:()=>({...state}),read:frame=>frame<state.nextFrame?{frame,inputs:[0,0]}:null}};
  }
  function open(extra={}){return createManagedSpectatorPlayback({run:25,signal:life.signal,
    openWorld:world,onState(value){const received=parseSpectatorPlayback(value);assert.ok(received);receipts.push(received);},
    render(world,frame){draws.push({epoch:world.rules.epoch,frame});},onError:error=>errors.push(error),...extra});}
  return {open,tape,world,worlds,draws,receipts,errors,life};
}
async function step(owner,time){owner.advance(time);await owner.settled();}

test('a fixed initial confirmed frontier gates the first draw, then ordinary playback consumes one frame per logical tick',async()=>{
  const f=fixture(),spectator=f.open(),a=f.tape(1,9);spectator.enqueue(a.track,a.rules);
  await step(spectator,0);assert.equal(f.worlds.length,0);assert.equal(spectator.status(),null);
  spectator.bootstrap(1,9);a.state.nextFrame=20;
  assert.throws(()=>spectator.command({run:spectator.status().run,action:'seek',frame:1}),/initial/i);
  await step(spectator,1);await step(spectator,20);await step(spectator,40);
  assert.equal(f.draws.length,0);assert.deepEqual(f.worlds[0].rows,[0,1,2,3,4,5,6,7]);
  await step(spectator,60);assert.deepEqual(f.draws.at(-1),{epoch:1,frame:9});
  assert.equal(spectator.status().seeking,false);assert.equal(spectator.status().received,20);
  await step(spectator,100);await step(spectator,100+tick);assert.equal(spectator.status().position,10);
  f.life.abort();assert.equal(f.worlds[0].closes,1);
});

test('pause, bounded backward seek and return live consume the old tail before adopting a new track',async()=>{
  const f=fixture(),spectator=f.open(),a=f.tape(1,4);spectator.enqueue(a.track,a.rules);spectator.bootstrap(1,4);
  await step(spectator,0);await step(spectator,20);
  const old=spectator.status().run;
  const command=(action,frame)=>spectator.command(spectatorPlaybackCommand(spectator.status(), action,frame));
  command('pause');a.state.nextFrame=8;a.state.end='restart';
  const b=f.tape(2,6);spectator.enqueue(b.track,b.rules);
  await step(spectator,10000);assert.equal(spectator.status().position,4);assert.equal(spectator.status().received,8);
  command('seek',2);await step(spectator,10020);await step(spectator,10040);
  assert.deepEqual(f.draws.at(-1),{epoch:1,frame:2});assert.equal(spectator.status().paused,true);
  assert.equal(f.worlds[0].closes,1);
  command('live');b.state.nextFrame=12;
  for(let t=10060;t<10300&&spectator.status()?.run===old;t+=20)await step(spectator,t);
  for(let t=10300;t<10500&&spectator.status()?.seeking;t+=20)await step(spectator,t);
  assert.ok(f.draws.some(value=>value.epoch===1&&value.frame===8));
  assert.deepEqual(f.draws.at(-1),{epoch:2,frame:6});
  assert.equal(spectator.status().following,true);assert.equal(spectator.status().received,12);
  assert.throws(()=>spectator.command({run:old,action:'pause'}),/stale/i);
  assert.throws(()=>spectator.command({run:spectator.status().run,action:'seek',frame:13}),/confirmed/i);
  assert.ok(f.receipts.every((value,index)=>index===0||value.revision>f.receipts[index-1].revision));
  spectator.close();assert.ok(f.worlds.every(world=>world.closes===1));
});

test('retirement closes a late world and the paused queue never admits a fourth track',async()=>{
  const f=fixture();let release;
  const spectator=f.open({openWorld:rules=>new Promise(resolve=>{release=()=>resolve(f.world(rules));})});
  for(let i=1;i<=3;i++){const a=f.tape(i,0);spectator.enqueue(a.track,a.rules);}
  const fourth=f.tape(4,0);assert.throws(()=>spectator.enqueue(fourth.track,fourth.rules),/limit/i);
  spectator.bootstrap(1,0);spectator.advance(0);
  await Promise.resolve();f.life.abort();release();await spectator.settled();
  assert.equal(f.worlds[0].closes,1);assert.equal(f.draws.length,0);assert.deepEqual(f.errors,[]);
});

for (const bootstrapFirst of [false, true]) {
  test(`a late co-op spectator presents a confirmed frame ${bootstrapFirst ? 'during' : 'before'} long catch-up`, async () => {
    const f = fixture(), spectator = f.open(), tape = f.tape(1, bootstrapFirst ? 216000 : 1, 'same-stage-coop');
    spectator.enqueue(tape.track, tape.rules);
    if (bootstrapFirst) spectator.bootstrap(1, 216000);
    await step(spectator, 0);
    await step(spectator, tick);
    const firstFrame = f.draws[0]?.frame;
    assert.ok(firstFrame >= 1 && firstFrame < 216000);
    if (!bootstrapFirst) {
      assert.equal(firstFrame, 1);
      assert.equal(spectator.status(), null);
      tape.state.nextFrame = 216000;
      spectator.bootstrap(1, 216000);
    }
    for (let draw = 2; draw < 2048 && spectator.status().seeking; draw++) {
      await step(spectator, draw * tick);
    }
    assert.equal(spectator.status().seeking, false, 'catch-up drains the long history in bounded batches');
    assert.deepEqual(f.draws.at(-1), {epoch: 1, frame: 216000});
    const rows = f.worlds[0].rows;
    assert.equal(rows.length, 216000);
    assert.ok(rows.every((row, index) => row === index));
    f.life.abort();
  });
}
