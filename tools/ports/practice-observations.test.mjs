import test from 'node:test';
import assert from 'node:assert/strict';
import {practiceBuildFeature,practiceGameplayReady} from './practice-observations.mjs';

test('practice readiness uses the title world clock and rejects app/network clocks or another seat',()=>{
  const th08={game:'th08',localSeat:0,phase:{name:'stage'},pause:{active:false},frames:{application:800},
    worldProgress:{counters:{stage:1,enemySceneFrame:80}}};
  assert.equal(practiceGameplayReady(th08),true);
  assert.equal(practiceGameplayReady({...th08,worldProgress:null}),false);
  assert.equal(practiceGameplayReady({...th08,localSeat:1}),false);
  assert.equal(practiceGameplayReady({...th08,pause:{active:true}}),false);
  const th11={game:'th11',localSeat:0,phase:{name:'stage'},pause:{active:false},frames:{simulation:80}};
  assert.equal(practiceGameplayReady(th11),true);
  assert.equal(practiceGameplayReady({...th11,frames:{multiplayer:80,application:90}}),false);
  assert.equal(practiceGameplayReady({...th11,phase:{name:'title'}}),false);
});

test('unknown or malformed practice declarations cannot become an unsupported-product N/A',()=>{
  const unsupported={features:{thprac:false}},supported={features:{thprac:true}};
  for(const thprac of [undefined,null,0,1,'true','false'])
    assert.throws(()=>practiceBuildFeature({features:{thprac}},unsupported),/explicit boolean/);
  assert.throws(()=>practiceBuildFeature({features:{thprac:true}},unsupported),/exceeds product support/);
  assert.equal(practiceBuildFeature({features:{thprac:false}},unsupported),false);
  assert.equal(practiceBuildFeature({features:{thprac:true}},supported),true);
  assert.equal(practiceBuildFeature({features:{thprac:false}},supported),false);
});
