import test from 'node:test';import assert from 'node:assert/strict';import fs from 'node:fs';
const url=new URL('../../site/runtime/multiplayer-rules.mjs',import.meta.url),product=fs.existsSync(url)?await import(url):{};
const base=()=>({schema:'th128-multiplayer-rules/1',matchId:'owned-match-1',epoch:1,mode:'parallel-stage',route:'A1-3',seed:34772,coreSHA256:'1'.repeat(64),resourceSHA256:'2'.repeat(64),inputDelay:3,rollbackWindow:60});
const create=value=>{assert.equal(typeof product.createMultiplayerRules,'function','Multiplayer rule consumer missing');return product.createMultiplayerRules(value);};
test('finite two-player rules derive fixed60 stage/rank and non-normal eligibility',()=>{
 const r=create(base());assert.equal(r.stageId,3);assert.equal(r.difficulty,1);assert.equal(r.tickRate,60);assert.equal(r.players,2);assert.equal(r.normalModeClear,false);assert.equal(r.carryVerified,false);assert.equal(r.progress,0);assert(Object.isFrozen(r));
});
for(const[route,id,rank]of[['A2-3',5,1],['B1-3',8,1],['B2-3',10,1],['C1-3',13,1],['C2-3',15,1],['EX',16,4]])test('finite match route '+route+' uses original stage/rank',()=>{
 const r=create({...base(),route});assert.equal(r.stageId,id);assert.equal(r.difficulty,rank);
});
test('input configuration is detached from the caller before fingerprinting',async()=>{
 const input=base(),r=create(input);input.seed=1;assert.equal(r.seed,34772);assert.throws(()=>{r.seed=2;});const digest=await product.multiplayerRulesFingerprint(r);assert.match(digest,/^[a-f0-9]{64}$/);assert.equal(r.seed,34772);
});
test('wire rules reject unknown/derived fields and missing identity or metadata',()=>{
 assert.equal(typeof product.createMultiplayerRules,'function','Validation consumer missing');
 for(const mutate of [r=>r.extra=true,r=>r.players=3,r=>r.stageId=1,r=>delete r.seed,r=>r.epoch=0,r=>r.matchId='',r=>r.matchId='../bad',r=>r.mode='shared-world',r=>r.route='A1-1',r=>r.seed=-1,r=>r.seed=65536,r=>r.seed=1.2,r=>r.coreSHA256='ABC',r=>r.resourceSHA256='x'.repeat(64),r=>r.inputDelay=9,r=>r.inputDelay=-1,r=>r.rollbackWindow=121,r=>r.rollbackWindow=-1]){
 const input=base();mutate(input);assert.throws(()=>create(input));
 }
 assert.throws(()=>create(null));assert.throws(()=>create([]));
});
test('canonical fingerprint matches different property insertion order and rejects mismatched rules',async()=>{
 const wire=base(),local=create(wire),remote=Object.fromEntries(Object.entries(wire).reverse()),a=await product.multiplayerRulesFingerprint(local);
 assert.equal(await product.multiplayerRulesFingerprint(create(remote)),a);
 const accepted=await product.acceptMultiplayerRules(local,remote,a);assert.equal(accepted.accepted,true);assert.equal(accepted.fingerprint,a);assert.notEqual(accepted.rules,local);
 for(const[field,value]of [['seed',111],['epoch',2],['route','EX'],['coreSHA256','3'.repeat(64)],['resourceSHA256','4'.repeat(64)],['inputDelay',4],['rollbackWindow',30],['matchId','other-match']])await assert.rejects(product.acceptMultiplayerRules(local,{...wire,[field]:value},a),/mismatch|规则/);
 await assert.rejects(product.acceptMultiplayerRules(local,wire,'0'.repeat(64)),/fingerprint|指纹/);
});
test('accessors and forged derived values cannot bypass wire validation',async()=>{
 const wire=base();Object.defineProperty(wire,'seed',{get(){throw Error('getter executed');},enumerable:true});
 assert.throws(()=>create(wire),/data|字段|property|plain/i);
 const local=create(base());await assert.rejects(product.multiplayerRulesFingerprint({...local,stageId:16}),/rules|规则|derived|canonical/i);
});
