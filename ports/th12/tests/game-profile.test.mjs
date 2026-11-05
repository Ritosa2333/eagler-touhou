import test from 'node:test';
import assert from 'node:assert/strict';
import {GameProfile,normalizeSettings,spellMetadata} from '../src/game-profile.mjs';
function store(){const data=new Map();return {data,async read(p){return data.get(p);},async write(p,v){data.set(p,v);},async sync(){}};}
test('game profile preserves full-run, practice and extra unlock state independently of browser replay imports',async()=>{
  const files=store(),p=await new GameProfile(files).load();assert.equal(p.canExtra(0,0),false);assert.equal(p.canPractice(0,0,1,2),false);
  p.recordStageEntry(0,0,1,3);assert.equal(p.canPractice(0,0,1,3),true);assert.equal(p.canPractice(0,1,1,3),false);
  p.recordRun({character:0,shot:0,difficulty:1,stage:6,score:1234560,clear:true,continueCount:0,name:'PLAYER'});assert.equal(p.canExtra(0,0),true);
  p.unlockMusic('th12_01');p.recordSpell(20,0,0,{capture:true,time:120,bonus:100000});await p.save();
  const q=await new GameProfile(files).load();assert.equal(q.canExtra(0,0),true);assert.equal(q.rankings(0,0,1)[0].score,1234560);assert.equal(q.data.spellRecords['20:0'].captures,1);assert.deepEqual(q.data.music,['th12_01']);
});
test('browser profile does not silently overwrite unsupported retail score/config bytes',async()=>{
  const files=store(),raw=Uint8Array.of(1,2,3);files.data.set('/savesth12/scoreth12.dat',raw);const p=await new GameProfile(files).load();await p.save();assert.equal(files.data.get('/savesth12/scoreth12.dat'),raw);assert.equal(p.warnings.length,1);
});
test('settings volume and command bindings stay bounded and native spell history persists',async()=>{
  assert.deepEqual([normalizeSettings({bgmVolume:102,seVolume:-1}).bgmVolume,normalizeSettings({bgmVolume:102,seVolume:-1}).seVolume],[100,0]);
  const files=store(),p=await new GameProfile(files).load(),records=Array.from({length:128},()=>Array.from({length:7},()=>[0,0]));records[5][0]=[3,2];records[5][6]=[3,2];p.updateSpellHistory(records);await p.save();const q=await new GameProfile(files).load();assert.deepEqual(q.data.spellRecords['5:0'],{attempts:3,captures:2});assert.equal(q.spellHistoryWords()[70],3);
});

test('rankings separate ordinary, Extra and practice runs, exclude continues and return copies',()=>{
  const p=new GameProfile(store());
  for(const [character,shot,difficulty,stage,score,mode,continues] of [[0,0,1,4,100,'story',0],[0,0,1,6,900,'story',1],[0,0,4,7,200,'extra',0],[0,1,4,7,300,'extra',0],[0,0,1,4,400,'practice',0]])
    p.recordRun({character,shot,difficulty,stage,score,mode,continues,name:'ATB'});
  assert.deepEqual(p.rankings(0,0,1).map(r=>r.score),[100]);
  assert.deepEqual(p.rankings(0,0,4).map(r=>r.score),[200]);
  assert.deepEqual(p.rankings(0,undefined,4).map(r=>r.score),[300,200]);
  assert.deepEqual(p.rankings(0,0,1,4).map(r=>r.score),[400]);
  const copy=p.rankings(0,0,1)[0];copy.score=999999;assert.equal(p.rankings(0,0,1)[0].score,100);
  assert.equal(p.canExtra(0,0),false);
});

test('retail card numbers map the irregular normal-only cards and all Extra cards correctly',()=>{
  assert.deepEqual([0,1,2,3,4,5].map(id=>spellMetadata(id).difficulty),[2,3,0,1,2,3]);
  for(const ids of [[54,55,56],[93,94,95]])assert.deepEqual(ids.map(id=>spellMetadata(id).difficulty),[1,2,3]);
  for(const id of [100,105,112])assert.deepEqual(spellMetadata(id),{id,stage:7,difficulty:4});
  assert.deepEqual([9,10,25,26,41,42,56,57,76,77,99,100].map(id=>spellMetadata(id).stage),[1,2,2,3,3,4,4,5,5,6,6,7]);
  assert.equal(spellMetadata(113),null);
});

test('spell query filters machine, equipment, difficulty and stage and aggregates best capture values',()=>{
  const p=new GameProfile(store());
  p.recordSpell(89,0,0,{capture:false,title:'魔神復誦'});p.recordSpell(89,0,0,{capture:true,time:500,bonus:100});
  p.recordSpell(89,0,1,{capture:true,time:400,bonus:200});p.recordSpell(90,0,0,{capture:true,time:300});
  p.recordSpell(105,2,1,{capture:false,title:'鵺的スネークショー'});
  const one=p.spellRecords({character:0,shot:0,difficulty:0,stage:6});
  assert.equal(one.length,1);assert.equal(one[0].attempts,2);assert.equal(one[0].captures,1);assert.equal(one[0].captureRate,.5);
  const merged=p.spellRecords({character:0,difficulty:0})[0];
  assert.deepEqual([merged.attempts,merged.captures,merged.bestTime,merged.bestBonus],[3,2,400,200]);
  const extra=p.spellRecords({difficulty:4});assert.equal(extra.length,1);assert.equal(extra[0].id,105);
  one[0].attempts=999;assert.equal(p.spellRecords({character:0,shot:0,difficulty:0})[0].attempts,2);
});

test('native history counts take priority over caches, retain titles and use retail capped total column',async()=>{
  const files=store(),p=new GameProfile(files),records=Array.from({length:128},()=>Array.from({length:7},()=>[0,0]));
  p.recordSpell(105,2,1,{capture:true,time:400,bonus:500,title:'Original title'});
  records[105][0]=[3,2];records[105][5]=[7,4];records[105][6]=[10,6];p.updateSpellHistory(records);
  assert.deepEqual(p.spellRecords({character:2,shot:1}).map(r=>[r.attempts,r.captures,r.title]),[[7,4,'Original title']]);
  assert.deepEqual(p.spellRecords({difficulty:4}).map(r=>[r.attempts,r.captures]),[[10,6]]);
  const copy=p.spellHistoryWords();copy[105*14]=999;assert.equal(p.spellHistoryWords()[105*14],3);
  p.recordSpell(105,2,1,{capture:false});assert.equal(p.spellRecords({character:2,shot:1})[0].attempts,8);
  await p.save();const q=await new GameProfile(files).load();assert.equal(q.spellRecords({difficulty:4})[0].attempts,11);assert.equal(q.spellRecords({character:2,shot:1})[0].bestTime,400);
});

test('Player Data summary uses filtered records and preserves total playSeconds independently of rankings',async()=>{
  const files=store(),p=new GameProfile(files);p.data.playSeconds=1000;
  p.recordRun({character:0,shot:0,difficulty:1,stage:6,score:100,mode:'story',clear:true,continues:0,playSeconds:40});
  p.recordRun({character:0,shot:0,difficulty:1,stage:4,score:200,mode:'practice',clear:true,continues:0,playSeconds:10});
  p.recordRun({character:2,shot:1,difficulty:4,stage:7,score:900,mode:'extra',clear:false,playSeconds:80});
  p.recordSpell(3,0,0,{capture:true});p.recordSpell(105,2,1,{capture:false});
  const summary=p.dataSummary({character:0,shot:0,difficulty:1});
  assert.deepEqual([summary.runs,summary.clears,summary.noContinueClears,summary.practiceRuns,summary.playSeconds],[2,2,1,1,50]);
  assert.deepEqual([summary.spellCardsSeen,summary.spellAttempts,summary.spellCaptures],[1,1,1]);
  assert.equal(summary.rankings[0].score,100);assert.equal(p.dataSummary().playSeconds,1000);assert.equal(p.dataSummary({difficulty:4}).bestScore,900);
  await p.save();const q=await new GameProfile(files).load();assert.deepEqual(q.dataSummary({difficulty:4}),p.dataSummary({difficulty:4}));
});
