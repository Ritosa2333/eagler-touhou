import assert from 'node:assert/strict';
import {test} from 'node:test';
import {validateMultiplayerOptions,multiplayerSetupWords} from '../src/multiplayer.mjs';
import {multiplayerConfigForProduct,multiplayerInputTimingPolicy} from '../../../common/contracts/product-catalog.mjs';
const options={netplayMode:'lan',netplayUrl:'ws://localhost:8765/?room=test&run=1',netplayPlayerCount:2,netplayPlayer:0,netplaySeed:123,netplayDifficulty:1,netplayLoadouts:[{character:0,shot:0},{character:1,shot:1}],netplayAdonisMode:1,netplayPredictionLimit:0};
test('TH12 restores rollback and retains measured/manual delay',()=>{
 const policy=multiplayerInputTimingPolicy(multiplayerConfigForProduct('th12mp'));
 assert.equal(policy.rollback,true);assert.equal(policy.predictionLimit,12);assert.equal(policy.measuredStartup,true);assert.equal(policy.manualDelayLimit,9);
});
test('TH12 accepts restored rollback and unchanged delay sessions',async()=>{
 for(const delay of [0,3,9]){
  const {config,words}=await multiplayerSetupWords({...options,netplayInputDelay:delay},[1,2,3,4]);
  assert.equal(config.prediction,0);assert.equal(words[7],delay);assert.equal(words[14],1);
 }
 assert.equal(validateMultiplayerOptions({...options,netplayInputDelayAuto:true}).automatic,true);
 for(const mode of [0,2]){
  const {config,words}=await multiplayerSetupWords({...options,netplayAdonisMode:mode,netplayPredictionLimit:12},[1,2,3,4]);
  assert.equal(config.prediction,12);assert.equal(words[14],mode);
 }
 assert.throws(()=>validateMultiplayerOptions({...options,netplayAdonisMode:0,netplayPredictionLimit:12,netplayInputDelay:3}));
 assert.throws(()=>validateMultiplayerOptions({...options,netplayPredictionLimit:12}));
 assert.throws(()=>validateMultiplayerOptions({...options,netplayInputDelayAuto:true,netplayInputDelay:3}));
});
