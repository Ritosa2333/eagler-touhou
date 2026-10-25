import assert from 'node:assert/strict';
import {readNativeSnapshotRaw} from '../../tests/adaptation/adapters/native-snapshot.mjs';
import {practiceSnapshotRequest} from '../../tests/adaptation/adapters/practice.mjs';
import {controlTouchMotionClock,touchClockRequest} from '../../tests/adaptation/adapters/touch-clock.mjs';
import {decodeNativeControls} from '../../tests/adaptation/adapters/native-controls.mjs';
import {createNativeUiControls} from './native-ui-controls.mjs';

function assertLockTimer(row,contract,{visible,enabled}){
  const f=contract.lockTimer.fields,w=row.displayRaw?.words;decodeNativeControls(row.displayRaw);
  const native=contract.snapshot.decode(row.raw);assert.equal(w[f.frame],native.frame);assert.equal(w[f.option],Number(enabled));
  assert.equal(w[f.generation],w[3]);assert.equal(w[f.visible],Number(visible));
  if(visible){assert.equal(w[f.frames],w[f.consumer]);assert.ok(w[f.vertices]>0);assert.ok(w[f.length]>0&&w[f.length]<=8);
    const text=String.fromCharCode(...w.slice(f.text,f.text+w[f.length]));assert.equal(text,Math.fround(Math.fround(w[f.consumer])/60).toFixed(2));}
  else{assert.equal(w[f.length],0);assert.equal(w[f.vertices],0);}
}

export function assertPracticeBossEffects(e,contract){
  assert.ok(['eagler-touhou/practice-boss-effects/1','eagler-touhou/practice-boss-effects/2'].includes(e.schema));assert.deepEqual(e.cleanupErrors,[]);
  const timer=e.schema==='eagler-touhou/practice-boss-effects/2';if(timer)assert.ok(contract.lockTimer);
  const decode=row=>contract.snapshot.decode(row.raw),first=e.locked[0],identity=first.identity,bossOf=s=>s.bosses[contract.slot],owner=bossOf(decode(first)).owner;
  assert.ok(owner>0);assert.ok(e.locked.length>=32&&e.unlocked.length>=32&&e.damage.length>=2);
  for(const [name,mask] of [['locked',contract.protectedMask],['damage',contract.vulnerableMask],['unlocked',contract.unlockedMask]]){
    const rows=e[name];let previous;
    for(const row of rows){const s=decode(row),boss=bossOf(s);assert.deepEqual(row.identity,identity);assert.equal(s.bombActive,false);
      assert.equal(s.active,true);assert.equal(s.replay,false);assert.equal(s.cheats,mask);assert.equal(s.stage,contract.stage);
      assert.equal(boss.present,true);assert.equal(boss.owner,owner);assert.ok(boss.life>0&&boss.timeout>0);assert.equal(s.deaths,0);
      if(previous){assert.equal(s.consumerSequence,previous.state.consumerSequence+1);assert.equal(boss.lifetime,previous.boss.lifetime+1);
        assert.equal(boss.timeout,previous.boss.timeout+(name==='unlocked'?0:1));
        if(name!=='damage')assert.equal(boss.life,previous.boss.life,'U failed to protect actual Boss life');}
      previous={state:s,boss};
      if(timer)assertLockTimer(row,contract,{visible:name==='locked',enabled:name!=='damage'});
    }
  }
  assert.ok(e.locked.filter(row=>bossOf(decode(row)).damage>0).length>=2,'U needs actual positive damage attempts');
  const before=bossOf(decode(e.damage[0])),after=bossOf(decode(e.damage.at(-1)));
  assert.ok(after.life<before.life&&e.damage.some(row=>bossOf(decode(row)).damage>0),'U-off control never received actual damage');
  if(timer){assert.ok(e.timerOff.length>=4);for(const row of e.timerOff){assert.deepEqual(row.identity,identity);assertLockTimer(row,contract,{visible:false,enabled:false});
    const s=decode(row);assert.equal(s.cheats,contract.protectedMask);assert.equal(bossOf(s).owner,owner);}}
  return{complete:false,timeLock:true,enemyInvincibility:true,lockTimerText:timer,source:'real Boss lifetime, timeout, positive projectile damage and life'};
}

/** The ordinary Practice Boss world has its own isolated browser storage. */
export async function runPracticeBossBrowser({page,frame,target,adapter,press,save}){
  const contract=adapter.practice.bossEffects,readRequest=practiceSnapshotRequest({runtime:adapter.runtime,contract:{snapshot:contract.snapshot}});
  const bossOf=row=>contract.snapshot.decode(row.raw).bosses[contract.slot];
  const e={schema:contract.lockTimer?'eagler-touhou/practice-boss-effects/2':'eagler-touhou/practice-boss-effects/1',locked:[],damage:[],unlocked:[],timerOff:[],cleanupErrors:[]};let manual=false,held=false,failure;
  const read=async()=>({raw:await frame.evaluate(readNativeSnapshotRaw,readRequest),
    ...(contract.lockTimer?{displayRaw:await frame.evaluate(readNativeSnapshotRaw,practiceSnapshotRequest({runtime:adapter.runtime,contract:{snapshot:contract.lockTimer.snapshot}}))}:{}),
    identity:await frame.evaluate(runtime=>({app:globalThis[runtime.global][runtime.app],documentTimeOrigin:performance.timeOrigin,url:location.href}),adapter.runtime)});
  const clock=action=>frame.evaluate(controlTouchMotionClock,touchClockRequest({...target,action}));
  const begin=async()=>{assert.equal((await clock('begin')).mode,'native-manual');manual=true;};
  const end=async()=>{if(manual){await clock('end');manual=false;}};
  const key=async down=>{await frame.locator('canvas').focus();await page.keyboard[down?'down':'up'](contract.keys.fire);held=down;};
  const toggle=async code=>{await press('Backspace');await press(code);await press('Backspace');};
  const tick=async()=>{await clock('tick');return read();};
  const timerUi=contract.lockTimer?createNativeUiControls({page,frame,runtime:adapter.runtime,contract:adapter.practice.options,save:async receipt=>{e.timerUi=receipt;await save(e);}}):null;
  const setTimer=async enabled=>{if(!timerUi)return;const c=contract.lockTimer;await press(c.menuKey);await timerUi.until('Lock timer options not open',s=>s.open);
    const before=await timerUi.locate(c.toggle);if(Boolean(before.raw.words[c.fields.option])!==enabled)await timerUi.click(c.toggle);
    await timerUi.until('Lock timer setting not consumed',s=>s.controls.some(item=>item.semantic===c.toggle&&Boolean(item.value)===enabled));await press(c.menuKey);await timerUi.until('Lock timer options not closed',s=>!s.open);};
  try{
    await setTimer(true);
    await press('Backspace');for(const key of [contract.keys.playerGuard,contract.keys.timeLock,contract.keys.enemyGuard])await press(key);await press('Backspace');await begin();await key(true);
    let ready;for(let n=0;n<900;n++){ready=await tick();const b=bossOf(ready);if(b.present&&b.timeout>0&&b.damage>0)break;}
    assert.ok(bossOf(ready).damage>0,'The actual Boss did not receive player shots');
    e.locked.push(ready);for(let n=0;n<39;n++)e.locked.push(await tick());await key(false);await end();await save(e);
    if(timerUi){await setTimer(false);await begin();for(let n=0;n<4;n++)e.timerOff.push(await tick());await end();}
    await toggle(contract.keys.enemyGuard);await begin();await key(true);e.damage.push(await read());
    const original=bossOf(e.damage[0]).life;
    for(let n=0;n<300;n++){const row=await tick();e.damage.push(row);if(bossOf(row).life<original)break;}
    await key(false);await end();await save(e);
    await setTimer(true);await toggle(contract.keys.enemyGuard);await toggle(contract.keys.timeLock);await begin();e.unlocked.push(await read());for(let n=0;n<39;n++)e.unlocked.push(await tick());
    e.assessment=assertPracticeBossEffects(e,contract);await save(e);return e;
  }catch(error){failure=error;e.error=String(error.stack||error);await save(e);throw error;}
  finally{try{if(held)await key(false);await end();}catch(error){e.cleanupErrors.push(String(error));}await save(e);
    if(e.cleanupErrors.length&&!failure)throw Error('Boss effect cleanup failed');}
}
