import assert from 'node:assert/strict';
import {test} from 'node:test';
import {createMultiplayerRules} from '../../th128/site/runtime/multiplayer-rules.mjs';
import {createConfirmedHistory} from '../runtime/confirmed-history.mjs';
import {makeTh128ReplayFile,readTh128ReplayFile} from '../runtime/replay-file.mjs';
import {createManagedReplayMenu} from '../runtime/managed-replay-menu.mjs';

const build={coreSHA256:'a'.repeat(64),resourceSHA256:'b'.repeat(64)};
async function recording(epoch,identity=build){
  const rules=createMultiplayerRules({schema:'th128-multiplayer-rules/1',mode:'parallel-stage',matchId:'managed-reader',epoch,
    route:'A1-3',seed:34772,inputDelay:0,rollbackWindow:2,...identity});
  const history=await createConfirmedHistory({rules,allowedMask:763});
  for(let frame=0;frame<8;frame++)history.append({frame,inputs:[0,1].map(player=>({player,held:0,pressed:0,released:0,predicted:false}))});
  history.seal('restart');return makeTh128ReplayFile(history.replayBytes(),identity);
}
function fixture(extra={}){
  const life=new AbortController(),draws=[],states=[],worlds=[],titles=[];
  function world(rules){const value={rules,closes:0,inputs:[]};worlds.push(value);
    value.engine={step(row){value.inputs.push(row);},close(){value.closes++;}};return value;}
  const menu=createManagedReplayMenu({build,signal:life.signal,openWorld:world,
    render(scene,seat,frame){draws.push({epoch:scene.rules.epoch,seat,frame});},
    async restoreTitle(signal){titles.push(signal);return true;},onChange(value){states.push(value);},...extra});
  return {life,menu,draws,states,worlds,world,titles};
}

test('only an offered managed title can open a validated local file; bad imports preserve an active replay',async()=>{
  const f=fixture(),file=await recording(1);
  assert.equal(await f.menu.open(file),false);f.menu.enableTitle();
  assert.equal(await f.menu.open(file),true);assert.equal(f.menu.status().playback.phase,'paused');
  assert.equal(f.menu.status().filename,file.name);await f.menu.act('play');f.menu.advance(0);f.menu.advance(50);
  assert.equal(f.menu.status().playback.frame,3);await f.menu.act('pause');
  const wrong=await recording(2,{...build,resourceSHA256:'c'.repeat(64)});
  assert.equal(await f.menu.open(wrong),false);assert.match(f.menu.status().error,/build/);
  assert.equal(f.menu.status().filename,file.name);assert.equal(f.menu.status().playback.frame,3);
  assert.equal(await f.menu.open(new File(['rpy'],'single.rpy')),false);assert.match(f.menu.status().error,/th128mp/);
  await f.menu.act('seat',1);await f.menu.act('seek',6);f.menu.advance(60);
  assert.deepEqual(f.draws.at(-1),{epoch:1,seat:1,frame:6});
  assert.equal(await f.menu.returnToTitle(),true);assert.equal(f.titles.length,1);assert.equal(f.worlds[0].closes,1);
  assert.ok(f.menu.status().enabled);assert.equal(f.menu.status().playback,null);f.life.abort();
});

test('live ownership invalidates a pending file read, an opening scene, and a pending local title restoration',{timeout:10000},async()=>{
  let releaseRead,releaseWorld,releaseTitle,controller,opened;
  const opening=()=>new Promise(resolve=>{opened=resolve;});
  const f=fixture({readFile:(file,id)=>new Promise(resolve=>{releaseRead=()=>resolve(readTh128ReplayFile(file,id));}),
    openWorld:rules=>new Promise(resolve=>{releaseWorld=()=>resolve(f.world(rules));opened();}),
    restoreTitle:signal=>new Promise(resolve=>{controller=signal;releaseTitle=()=>resolve(true);})});
  const file=await recording(1);
  f.menu.enableTitle();const first=f.menu.open(file);f.menu.suspend();releaseRead();
  assert.equal(await first,false);assert.equal(f.worlds.length,0);
  f.menu.enableTitle();const secondReady=opening(),second=f.menu.open(file);releaseRead();await secondReady;
  assert.ok(releaseWorld);f.menu.suspend();releaseWorld();assert.equal(await second,false);
  assert.equal(f.worlds[0].closes,1);assert.equal(f.draws.length,0);
  f.menu.enableTitle();releaseWorld=null;const thirdReady=opening(),third=f.menu.open(file);releaseRead();await thirdReady;
  assert.ok(releaseWorld);releaseWorld();assert.equal(await third,true);
  const returning=f.menu.returnToTitle();f.menu.suspend();assert.ok(controller.aborted);releaseTitle();
  assert.equal(await returning,false);assert.equal(f.menu.status().enabled,false);assert.equal(f.worlds[1].closes,1);f.life.abort();
});

test('explicit replay entry can be left without selecting a file; return closes access until chosen again',async()=>{
  const f=fixture(),file=await recording(1);f.menu.enter();
  assert.equal(await f.menu.returnToTitle(),true);assert.equal(f.menu.status().enabled,false);assert.equal(await f.menu.open(file),false);
  f.menu.enter();assert.equal(await f.menu.open(file),true);assert.equal(await f.menu.returnToTitle(),true);
  assert.equal(f.menu.status().enabled,false);assert.equal(f.titles.length,2);f.life.abort();
});
