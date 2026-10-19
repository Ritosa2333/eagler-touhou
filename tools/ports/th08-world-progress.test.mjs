import test from 'node:test';
import assert from 'node:assert/strict';
import {compareTh08FrozenWorld,compareTh08LiveWorld,th08DisconnectedWaitState} from './th08-world-progress.mjs';

function snapshot({seat=0,stage=2,enemy=100,control=200,network=300,confirmed=300,next=301,
  waiting=false,frameOpen=false,rollback=null,correcting=false,correctionEnd=null,dialogue=false,
  paused=false,loading=false,transition=false,touchContext='gameplay',x=12,y=34}={}){
  const frontierNext=next;
  const worldProgress={schema:'eagler-touhou/th08-world-progress/1',comparable:!waiting,
    counters:{stage,enemySceneFrame:enemy,controlTick:control,lastCompletedNetworkFrame:network,
      confirmedNetworkFrame:confirmed,nextNetworkFrame:next},
    state:{loading,controlLoadState:0,pauseState:paused?1:0,paused,retrying:false,gameOver:false,
      dialogue,touchContext,touchContextRaw:touchContext==='gameplay'?1:touchContext==='dialogue'?2:3,inTransition:transition},
    localPlayer:{seat,eligible:true,movementAvailable:null,inputButtons:0,position:{x,y},lifeState:0},
    netplay:{version:1,configured:true,canStart:true,nextFrame:next,lastCompletedFrame:network,confirmedThrough:confirmed,
      rollbackFrame:rollback,worldReady:true,frameOpen,waiting,generation:1,retired:false,inputDelay:2,
      nextCaptureFrame:next,mode:0,frontier:{predictionFrontier:frontierNext,nextFrame:frontierNext,
        correcting,callbackYielded:false,correctionYields:0,correctionEnd},settled:!waiting}};
  return{observation:{game:'th08',variant:'multiplayer',localSeat:seat,worldProgress}};
}

test('requires all three live world clocks to advance in the same active stage',()=>{
  const result=compareTh08LiveWorld(snapshot(),snapshot({enemy:106,control:206,network:306,confirmed:306,next:307}),{seat:0});
  assert.equal(result.passed,true,result.errors.join('; '));
});

test('a one-frame prediction lead does not block live qualification and stays explicit in evidence',()=>{
  const after=snapshot({enemy:106,control:206,network:306,confirmed:305,next:307});
  after.observation.worldProgress.comparable=false;
  const result=compareTh08LiveWorld(snapshot(),after,{seat:0});
  assert.equal(result.passed,true,result.errors.join('; '));
  assert.equal(result.evidence.networkQualification.after.allFramesConfirmed,false);
  assert.equal(result.evidence.networkQualification.after.confirmedThrough,305);
  assert.equal(result.evidence.networkQualification.after.lastCompletedFrame,306);
  assert.equal(result.evidence.networkQualification.after.correctionIdle,true);
});

test('waiting is recorded but live clocks and an idle correction state decide activity',()=>{
  const before=snapshot({waiting:true});
  const after=snapshot({waiting:true,enemy:106,control:206,network:306,confirmed:305,next:307});
  after.observation.worldProgress.comparable=false;
  const result=compareTh08LiveWorld(before,after,{seat:0});
  assert.equal(result.passed,true,result.errors.join('; '));
  assert.equal(result.evidence.networkQualification.before.waiting,true);
  assert.equal(result.evidence.networkQualification.after.waiting,true);
});

test('network advancement alone cannot substitute for enemy and control progress',()=>{
  const result=compareTh08LiveWorld(snapshot(),snapshot({network:306,confirmed:306,next:307}),{seat:0});
  assert.equal(result.passed,false);
  assert.ok(result.errors.some(error=>error.includes('enemySceneFrame did not advance')));
  assert.ok(result.errors.some(error=>error.includes('controlTick did not advance')));
});

test('a dialogue pause cannot be mistaken for a live gameplay baseline',()=>{
  const result=compareTh08LiveWorld(snapshot({dialogue:true,touchContext:'dialogue'}),
    snapshot({enemy:100,control:206,network:306,confirmed:306,next:307,dialogue:true,touchContext:'dialogue'}),{seat:0});
  assert.equal(result.passed,false);
  assert.ok(result.errors.some(error=>error.includes('not in active gameplay')));
});

test('a settled waiting sample rejects an open frame or active rollback correction',()=>{
  assert.equal(th08DisconnectedWaitState(snapshot({waiting:true}),{seat:0}).ready,true);
  assert.ok(th08DisconnectedWaitState(snapshot({waiting:true,frameOpen:true}),{seat:0}).blockers.includes('network-frame-open'));
  assert.ok(th08DisconnectedWaitState(snapshot({waiting:true,rollback:300}),{seat:0}).blockers.includes('rollback-active'));
  assert.ok(th08DisconnectedWaitState(snapshot({waiting:true,correcting:true}),{seat:0}).blockers.includes('correction-active'));
});

test('three settled wait samples prove stable enemy, control, network, position, and state',()=>{
  const samples=[snapshot({waiting:true}),snapshot({waiting:true}),snapshot({waiting:true})];
  const result=compareTh08FrozenWorld(...samples,{seat:0});
  assert.equal(result.passed,true,result.errors.join('; '));
});

test('world progress or position during a stalled network still fails the freeze check',()=>{
  const notification=snapshot({waiting:true});
  const settled=snapshot({waiting:true,enemy:101});
  const stable=snapshot({waiting:true,enemy:101,x:13});
  const result=compareTh08FrozenWorld(notification,settled,stable,{seat:0});
  assert.equal(result.passed,false);
  assert.ok(result.errors.some(error=>error.includes('world state changed')));
});

test('does not infer TH08 evidence from another game, seat, or missing reader fields',()=>{
  const wrongGame={observation:{...snapshot().observation,game:'th11'}};
  assert.ok(compareTh08LiveWorld(wrongGame,snapshot({enemy:101,control:201,network:301,confirmed:301,next:302}),{seat:0}).errors.length);
  assert.ok(compareTh08LiveWorld(snapshot({seat:1}),snapshot({seat:2,enemy:101,control:201,network:301,confirmed:301,next:302}),{seat:1}).errors.length);
  const missing=snapshot();delete missing.observation.worldProgress.netplay.frontier;
  assert.ok(th08DisconnectedWaitState(missing,{seat:0}).errors.length);
});
