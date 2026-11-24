import assert from 'node:assert/strict';
import test from 'node:test';
import {makeCoopRules, coopWire, coopFingerprint, checkCoopAgreement} from '../runtime/coop-rules.mjs';
import {createConfirmedHistory,historyRules,readMultiplayerReplay} from '../runtime/confirmed-history.mjs';
const start={schema:'th128-coop/1',mode:'same-stage-coop',matchId:'th128mp-4812-1',epoch:1,players:2,difficulty:1,route:'A',challengeMode:false,seed:4812,inputDelay:0,rollbackWindow:2,coreSHA256:'b'.repeat(64),resourceSHA256:'d'.repeat(64)};
test('all main difficulties and Extra have a real native entry, with both seat counts and independent challenge switch',async()=>{
  const fingerprints=new Set();
  for(const players of [2,3])for(const difficulty of [0,1,2,3,4]) for(const challengeMode of [false,true])for(const route of difficulty===4?['EX']:['A','B','C']){
    const wire={...start,players,difficulty,route,challengeMode};const rules=makeCoopRules(wire);
    assert.equal(rules.stageId,({A:1,B:6,C:11,EX:16})[route]);assert.equal(rules.ecl,({A:'st_a1_1.ecl',B:'st_b1_1.ecl',C:'st_c1_1.ecl',EX:'st_ex.ecl'})[route]);
    assert.deepEqual(coopWire(rules),wire);const key=await coopFingerprint(rules);assert.ok(!fingerprints.has(key));fingerprints.add(key);
    await checkCoopAgreement(rules,wire,key);
  }
  assert.equal(fingerprints.size,52);
});
test('peers cannot silently downgrade, mix parallel saves or add practice/unlock overrides',async()=>{
  const rules=makeCoopRules(start);for(const fields of [{players:1},{players:4},{difficulty:'1'},{difficulty:5},{challengeMode:1},{route:'EX'}, {route:'A',difficulty:4},{mode:'parallel-stage'},{thpracEnabled:true},{unlocked:true}])assert.throws(()=>makeCoopRules({...start,...fields}));
  const key=await coopFingerprint(rules);for(const fields of [{players:3},{challengeMode:true},{difficulty:0}])await assert.rejects(checkCoopAgreement(rules,{...start,...fields},key));
});
test('freeze motivation defaults to original and is fixed for the match and its recording',async()=>{
  const baseline=makeCoopRules(start),explicit=makeCoopRules({...start,freezeMotivationMultiplier:1});
  assert.equal(baseline.freezeMotivationMultiplier,1);
  assert.deepEqual(coopWire(baseline),coopWire(explicit));
  for(const freezeMotivationMultiplier of [2.5,2,1,0.75,0.5]){
    const rules=makeCoopRules({...start,freezeMotivationMultiplier});
    assert.equal(rules.freezeMotivationMultiplier,freezeMotivationMultiplier);
    if(freezeMotivationMultiplier!==1){
      assert.equal(coopWire(rules).freezeMotivationMultiplier,freezeMotivationMultiplier);
      await assert.rejects(checkCoopAgreement(baseline,coopWire(rules),await coopFingerprint(rules)),/differ/);
    }
    await checkCoopAgreement(rules,coopWire(rules),await coopFingerprint(rules));
    const history=await createConfirmedHistory({rules,allowedMask:763});history.seal('left');
    const tape=await readMultiplayerReplay(history.replayBytes(),start);
    assert.equal(historyRules(tape.header.rules).freezeMotivationMultiplier,freezeMotivationMultiplier);
  }
  for(const value of [0,4,5,2.25,'0.75',null,true])assert.throws(()=>makeCoopRules({...start,freezeMotivationMultiplier:value}),/motivation/i);
  assert.throws(()=>makeCoopRules({...start,freezeMotivationArea:2.5}),/unknown/i,'an old area divisor is not a multiplier');
});
