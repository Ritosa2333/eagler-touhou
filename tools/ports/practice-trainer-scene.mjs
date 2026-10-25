import assert from 'node:assert/strict';
import {assertControlledSceneArmed,assertControlledSceneWorld,assertControlledSceneStimulus} from '../../tests/adaptation/adapters/scene.mjs';
import {decodePracticeUiSnapshot} from '../../tests/adaptation/adapters/practice-ui.mjs';

/** Recompute a trainer trial from every native tick, rather than its UI flags. */
export function assertPracticeTrainerTrial(e,{contract,trial}){
  assert.equal(e.schema,'eagler-touhou/practice-trainer-trial/1');assert.equal(e.id,trial.id);
  assertControlledSceneArmed(e.arm);assert.ok(e.samples.length>2&&e.samples.length<=700);
  const decoded=e.samples.map(row=>({row,state:contract.snapshot.decode(row.raw)}));
  const identity=e.samples[0].identity;let previous,previousScene;
  assert.ok(Number.isSafeInteger(identity.app)&&identity.app>=0&&Number.isFinite(identity.documentTimeOrigin)&&new URL(identity.url).protocol==='https:');
  for(const {row,state:s} of decoded){
    assert.deepEqual(row.identity,identity);assert.equal(s.active,true);assert.equal(s.enabled,true);assert.equal(s.replay,false);
    assert.equal(s.cheats,trial.mask);assert.equal(s.consumerValid,true);
    if(!previousScene)assertControlledSceneWorld(row.scene);
    else assertControlledSceneStimulus(row.scene,previousScene,{kind:'single-projectile'});
    assert.equal(row.scene.requestGeneration,e.arm.requestGeneration);previousScene=row.scene;
    if(previous){assert.equal(s.consumerSequence,previous.consumerSequence+1);assert.ok(s.frame>previous.frame);assert.equal(s.stage,previous.stage);}
    previous=s;
    const ui=decodePracticeUiSnapshot(row.uiRaw);assert.ok(ui.keyboard,'Missing physical native key observation');
    const vk=contract.bombVirtualKey;
    if(trial.kind==='auto-bomb')assert.equal(ui.keyboard.down[vk>>>5]&(1<<(vk&31)),0,'Auto-bomb trial received a real X key');
    const tracker=row.tracker;assert.equal(tracker.length,7);assert.equal(tracker[0],1);
    for(const [i,field] of [[2,'deaths'],[3,'dissolve'],[4,'bombsUsed'],[5,'spellCaptures'],[6,'lastSpellCaptures']])assert.equal(tracker[i],s[field]);
  }
  const named=label=>{const rows=decoded.filter(x=>x.row.label===label);assert.equal(rows.length,1,'Missing '+label);return rows[0].state;};
  const before=named('before'),after=named('after');assert.equal(before.life,contract.alive);assert.equal(before.hit.count,0);
  assert.ok((trial.allowZeroLives?before.lives>=0:before.lives>0)&&before.power>contract.deathPowerCost&&before.bombs>=contract.deathbombCost);
  if(trial.startLives!==undefined)assert.equal(before.lives,trial.startLives);
  for(const option of trial.options??[]){assert.deepEqual(e.optionsSetup.identity,identity);assert.equal(e.optionsSetup.raw.words[option.stateWord],option.value);}
  const collision=decoded.find(x=>x.state.hit.count>0)?.state;
  if(trial.stimulus==='hit'){
    assert.equal(e.stimulus.accepted,true);assert.ok(collision,'No actual tagged projectile collision');
    assert.equal(collision.hit.result,1);assert.equal(collision.hit.before,contract.alive);assert.equal(collision.hit.count,1);
    assert.equal(collision.hit.sequence,collision.consumerSequence);
    assert.equal(collision.hit.after,trial.kind==='invincible'?contract.alive:contract.deathWindow);
  }
  if(trial.kind==='miss'){
    const committed=decoded.find(x=>x.state.deaths===before.deaths+1)?.state;assert.ok(committed,'No committed death');
    assert.equal(committed.power,trial.expectedPower??before.power-(trial.keepPower?0:contract.deathPowerCost));
    assert.equal(after.life,contract.respawning);assert.equal(after.lives,before.lives-(trial.keepLife?0:1));
    assert.equal(after.deaths,before.deaths+1);assert.equal(after.dissolve,before.dissolve+1);assert.equal(after.bombsUsed,before.bombsUsed);
  }else if(trial.kind==='invincible'){
    assert.ok(decoded.filter(x=>x.state.hit.count===1).length>=8,'Invincibility needs a post-collision window');
    for(const {state:s} of decoded){assert.equal(s.life,contract.alive);for(const field of ['deaths','dissolve','lives','power','bombsUsed'])assert.equal(s[field],before[field]);}
  }else{
    assert.equal(after.bombActive,true);assert.equal(after.bombsUsed,before.bombsUsed+1);assert.equal(after.deaths,before.deaths);assert.equal(after.lives,before.lives);
    if(['auto-bomb','hit-bomb','late-hit-bomb'].includes(trial.kind)){
      const late=trial.kind==='late-hit-bomb';
      assert.ok((late?contract.deathbombTypes:contract.autoBombTypes).includes(after.bombType));
      assert.equal(after.bombs,before.bombs-(late?contract.deathbombCost:contract.autoBombCost));
      assert.equal(after.dissolve,before.dissolve+1);
      assert.equal(e.physicalBomb,trial.kind!=='auto-bomb','Automatic Bomb cannot be proved with a physical Bomb input');
    }else{
      assert.equal(e.physicalBomb,true);assert.equal(after.bombs,before.bombs-(trial.keepBombs?0:contract.bombCost));
      assert.equal(after.dissolve,before.dissolve);assert.ok(after.bombType<2);
    }
    if(e.physicalBomb){const vk=contract.bombVirtualKey;assert.ok(e.samples.some(row=>decodePracticeUiSnapshot(row.uiRaw).keyboard.down[vk>>>5]&(1<<(vk&31))),
      'Physical Bomb control lacks actual native keyboard sampling');}
  }
  return{complete:true,id:trial.id,samples:decoded.length,consumer:'actual collision, resource and Tracker owners'};
}

export async function runPracticeTrainerTrial(io,{contract,trial}){
  const e={schema:'eagler-touhou/practice-trainer-trial/1',id:trial.id,arm:io.arm,samples:[],physicalBomb:false,
    ...(io.optionsSetup?{optionsSetup:io.optionsSetup}:{})};
  const capture=async label=>{const row=await io.read();row.label=label;e.samples.push(row);return contract.snapshot.decode(row.raw);};
  const tick=async label=>{await io.tick();return capture(label);};
  try{
    const readReady=io.readReady??io.read;
    let ready;for(let i=0;i<600;i++){const row=await readReady();ready=contract.snapshot.decode(row.raw);if(ready.life===contract.alive)break;await io.tick();}
    assert.equal(ready.life,contract.alive,'Player never reached native collision eligibility');
    await capture('before');
    if(trial.stimulus==='hit')e.stimulus=await io.hit();
    else{await io.key(contract.bombKey,true);e.physicalBomb=true;}
    // Physical input is queued for the following movement consumer. Submit it
    // before the collision tick to match F6's first post-hit consumer exactly.
    if(trial.kind==='hit-bomb'){await io.key(contract.bombKey,true);e.physicalBomb=true;}
    let collisionTicks=0,finished=false;
    for(let i=0;i<96;i++){
      const state=await tick('tick');if(state.hit.count)collisionTicks++;
      if(trial.kind==='late-hit-bomb'&&state.hit.count&&!e.physicalBomb){await io.key(contract.bombKey,true);e.physicalBomb=true;e.physicalBombAfterCollision=state.consumerSequence;}
      if(e.physicalBomb&&i===2)await io.key(contract.bombKey,false);
      const done=trial.kind==='miss'?state.deaths>0&&state.life===contract.respawning:
        trial.kind==='invincible'?collisionTicks>=10:state.bombActive&&state.bombsUsed>0;
      if(done&&i>=3){e.samples.at(-1).label='after';finished=true;break;}
    }
    assert.equal(finished,true,'Trainer consumer did not reach its expected effect');
    e.assessment=assertPracticeTrainerTrial(e,{contract,trial});await io.save(e);return e;
  }catch(error){e.error=String(error.stack||error);await io.save(e);throw Object.assign(error,{practiceTrainerEvidence:e});}
  finally{await io.key(contract.bombKey,false);}
}

export function assertPracticeTrainerMatrix(rows,contract){
  assert.deepEqual(rows.map(row=>row.trainerId),contract.cases.map(trial=>trial.id),'Every declared trainer and control must execute exactly once');
  return{schema:'eagler-touhou/practice-trainer-matrix/1',complete:false,
    trials:rows.map((row,index)=>{assert.equal(row.status,'passed');return assertPracticeTrainerTrial(row.trainer,{contract,trial:contract.cases[index]});})};
}
