import test from'node:test';import assert from'node:assert/strict';import{readFileSync,existsSync}from'node:fs';import{resolve}from'node:path';import{core,memory,root,report,sha,target}from'./helpers.mjs';
test('TH11 external original clear replay traverses full campaign without added lives', {skip:!process.env.TH11_EXTERNAL_REPLAY},async()=>{
 const c=await core(),raw=readFileSync(resolve(process.env.TH11_EXTERNAL_REPLAY)),rp=c.allocate(raw.length),replay=c.replay_create(),archive=readFileSync(resolve(root,'../[th11] 东方地灵殿 (汉化版+日文版)/th11.dat')),data=c.allocate(archive.length),source=c.resources_create(),s=c.game_session_create(),render=c.comp_create();
 memory(c,rp,raw.length).set(raw);assert.equal(c.replay_open(replay,rp,raw.length),1);const header=Buffer.from(memory(c,c.replay_data(replay),112)),stages=[];
 for(let n=1;n<=7;++n){const p=c.replay_stage(replay,n);if(p)stages.push({stage:n,header:Buffer.from(memory(c,c.replay_data(replay)+new DataView(c.memory.buffer).getUint32(p+4,true),144))});}
 memory(c,data,archive.length).set(archive);assert.equal(c.resources_open(source,data,archive.length),1);const error=()=>{const b=memory(c,c.game_session_error(s),512);return new TextDecoder().decode(b.subarray(0,b.indexOf(0)));};
 let frames=0,lastStage=0;const visits=[];
 try{assert.equal(c.game_session_replay(s,source,rp,raw.length,0,0),1,error());
  const maximum=stages.reduce((n,x)=>n+x.header.readUInt32LE(4),0)+5000;
  for(;frames<maximum&&c.game_session_value(s,0)===1;++frames){
   const stage=c.game_session_value(s,2);if(stage!==lastStage){visits.push({stage,frame:frames});console.log('stage',stage,'frame',frames);lastStage=stage;}
   assert.equal(c.game_session_update(s,0,0,0,0,0,0),1,`stage${stage} frame${frames}: ${error()}`);assert.equal(c.game_session_render(s,render),1,error());
  }
  const stats=Buffer.from(memory(c,c.battle_data(s,0),48)),result={frames,visits,score:stats.readInt32LE(0),lives:stats.readInt32LE(16),expectedScore:header.readInt32LE(20)};console.log(result);
  assert.ok(frames<maximum,'recording must terminate');assert.deepEqual(visits.map(x=>x.stage),stages.map(x=>x.stage),'all recorded stages visited');
  // Native stage headers quantize point value. The score stored at recording
  // time is not necessarily the score obtained by the original during replay.
  // Accept a difference only with a completed, matching native-world proof.
  const name=stages[0].stage===7?'external-extra':'external-campaign',nativePath=resolve(root,'artifacts/cpp/verification/world-replay-'+name+'.json');let nativeProof=null;
  if(existsSync(nativePath)){
   const n=JSON.parse(readFileSync(nativePath));
   if(n.passed&&n.replaySha256===sha(raw)&&n.finalState?.phase===4&&n.firstInactiveFrame+1===frames&&n.checks===frames&&n.target.sha256===target.sha256){
    const current=sha(readFileSync(process.env.TH11_CORE_TEST_FILE?resolve(process.env.TH11_CORE_TEST_FILE):resolve(root,'artifacts/cpp/game-core-test.wasm')));
    let equivalent=n.coreSha256===current;
    const path=resolve(root,'artifacts/cpp/verification/gameplay-core-equivalence.json');
    if(!equivalent&&existsSync(path)){const e=JSON.parse(readFileSync(path));equivalent=e.passed&&e.coreSha256===current&&e.baselineCoreSha256===n.coreSha256&&e.cases.some(x=>x.replaySha256===sha(raw)&&x.checks===frames);}
    assert.ok(equivalent,'native proof must match this core or its complete replay equivalence proof');
    assert.equal(result.score,n.finalState.score,'original-machine final playback score');assert.equal(result.lives,n.finalState.lives,'original-machine final lives');
    nativeProof={coreSha256:n.coreSha256,checks:n.checks,score:n.finalState.score,lives:n.finalState.lives};
   }
  }
  if(!nativeProof)assert.equal(result.score,result.expectedScore,'header score differs; a completed matching native-world comparison is required');
  report(name,{passed:true,replaySha256:sha(raw),...result,headerScoreDifference:result.score-result.expectedScore,nativeProof,scope:'External original replay, real damage and life counters with no added lives or forced exits. All recorded stages and rendering callbacks execute. Final score is checked against completed original-machine playback when a matching proof exists; otherwise the original saved header must match exactly. Saved header differences are retained in the report, never silently ignored.'});
 }finally{c.comp_delete(render);c.game_session_delete(s);c.resources_delete(source);c.replay_delete(replay);c.release(data);c.release(rp);}
});
