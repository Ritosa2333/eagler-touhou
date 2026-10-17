import test from'node:test';import assert from'node:assert/strict';import{readFileSync}from'node:fs';import{resolve}from'node:path';import{core,memory,root,report,sha}from'./helpers.mjs';
test('TH11 live full-campaign inputs record and replay across every stage', {skip:!process.env.TH11_EXTERNAL_REPLAY},async()=>{
 const c=await core(),raw=readFileSync(resolve(process.env.TH11_EXTERNAL_REPLAY)),rp=c.allocate(raw.length),r=c.replay_create(),archive=readFileSync(resolve(root,'../[th11] 东方地灵殿 (汉化版+日文版)/th11.dat')),data=c.allocate(archive.length),source=c.resources_create(),s=c.game_session_create(),render=c.comp_create(),input=c.allocate(36),out=c.allocate(4<<20),name=c.allocate(9);
 memory(c,rp,raw.length).set(raw);assert.equal(c.replay_open(r,rp,raw.length),1);const header=Buffer.from(memory(c,c.replay_data(r),112)),stages=[];
 for(let n=1;n<=7;++n){const p=c.replay_stage(r,n);if(p){const b=Buffer.from(memory(c,c.replay_data(r)+new DataView(c.memory.buffer).getUint32(p+4,true),144));stages.push({stage:n,frames:b.readUInt32LE(4),seed:b.readUInt16LE(2)});}}
 memory(c,data,archive.length).set(archive);assert.equal(c.resources_open(source,data,archive.length),1);memory(c,name,9).set(Buffer.from('FULLRUN \0'));
 const error=()=>{const b=memory(c,c.game_session_error(s),512);return new TextDecoder().decode(b.subarray(0,b.indexOf(0)));};
 const snapshot=()=>Buffer.concat([Buffer.from(memory(c,c.battle_data(s,0),48)),Buffer.from(memory(c,c.battle_data(s,2),128)),Buffer.from(memory(c,c.anm_env_rng(c.game_session_animations(s),0),8))]);
 const expected=[];let stream=0,used=0,held=0,lastStage=0;const visited=[];
 try{new DataView(c.memory.buffer).setUint32(c.anm_env_rng(c.game_session_animations(s),0),stages[0].seed,true);
  assert.equal(c.game_session_begin(s,source,stages[0].stage,header.readUInt32LE(92),header.readUInt32LE(96),header.readUInt32LE(100),0),1,error());assert.equal(c.replay_select(r,stages[0].stage),1);
  for(let frame=0;frame<150000&&c.game_session_value(s,0)===1;++frame){
   if(used===stages[stream].frames&&stream+1<stages.length){++stream;used=0;assert.equal(c.replay_select(r,stages[stream].stage),1);}
   if(used<stages[stream].frames){c.replay_tick(r,input,1);++used;held=memory(c,input+20,1)[0]?0:new DataView(c.memory.buffer).getUint32(input,true);}else held=0;
   const stage=c.game_session_value(s,2);if(stage!==lastStage){lastStage=stage;visited.push(stage);console.log('live stage',stage,'frame',frame);}
   assert.equal(c.game_session_update(s,held,0,0,0,0,0),1,`live ${frame}: ${error()}`);assert.equal(c.game_session_render(s,render),1,error());
   if(c.game_session_value(s,0)===1)expected.push(snapshot());
  }
  const extra=header.readUInt32LE(100)===4;
  assert.equal(c.game_session_value(s,0),extra?3:5,'original inputs finish without replenishing lives');
  const length=c.game_session_save_replay(s,name,out,4<<20);assert.ok(length);const recording=Buffer.from(memory(c,out,length));console.log('recorded frames',expected.length,'bytes',length);
  assert.equal(c.replay_open(r,out,length),1);const liveFinalScore=new DataView(c.memory.buffer).getInt32(c.replay_data(r)+20,true),originalSavedScore=header.readInt32LE(20);
  // These two fixed clear samples start from the ordinary fresh-game state.
  // Keep their live score check separate from replay playback, whose stage
  // headers quantize point value. Arbitrary imported initial states are not
  // covered by this fresh-run fixture.
  const freshSample=['04bdd21390bebff68a68a0e6d7af9c008e797320a4269305ca23ffe471419e21','8503598ded249b04352bdec6120a4dcb05ad5f3ee17a1a55bd52a6abd4fb5175'].includes(sha(raw));
  if(freshSample)assert.equal(liveFinalScore,originalSavedScore,'original recorded clear score from fresh live inputs');
  console.log({liveFinalScore,originalSavedScore});
  assert.equal(c.game_session_replay(s,source,out,length,0,0),1,error());
  // Native 436f30 restores the header's rounded point value at each stage.
  // Live play retains the discarded units, so score and point value cannot
  // be compared to the live run; native world tests check those separately.
  let checks=0;for(let frame=0;frame<expected.length;++frame){assert.equal(c.game_session_update(s,0,0,0,0,0,0),1,`playback ${frame}: ${error()}`);if(c.game_session_value(s,0)!==1)break;assert.equal(c.game_session_render(s,render),1,error());const actual=snapshot(),want=Buffer.from(expected[frame]);if(!extra){actual.copy(want,0,0,4);actual.copy(want,8,8,12);}assert.deepEqual(actual,want,`full campaign frame${frame}`);++checks;}
  assert.ok(checks>expected.length-1000,'all combat and transition frames checked before original replay-ending boundary');
  // The final Extra reward is awarded on the tick that leaves gameplay.
  // Do not label the last active frame's pre-clear score as the final score.
  let endingTicks=0;while(c.game_session_value(s,0)===1&&endingTicks++<1000){assert.equal(c.game_session_update(s,0,0,0,0,0,0),1,error());assert.equal(c.game_session_render(s,render),1,error());}
  assert.equal(c.game_session_value(s,0),4,'saved replay reaches its terminal boundary');
  const replayFinalScore=new DataView(c.memory.buffer).getInt32(c.battle_data(s,0),true);if(extra)assert.equal(replayFinalScore,liveFinalScore,'Extra terminal score includes the clear reward');
  report(extra?'extra-recording':'campaign-recording',{passed:true,liveFinalScore,originalSavedScore,liveScoreCompared:freshSample,replayFinalScore,scoreAndPointValueCompared:extra,sourceSha256:sha(raw),recordingSha256:sha(recording),visited,frames:expected.length,checks,scope:'Original player held keys reenacted as a fresh live run with original seed, no extra lives or forced exits; the two fixed fresh-game clear samples additionally require the live saved score to equal the original header. Arbitrary original initial/menu states are not reconstructed by this fixture. New recording replay compares power, lives, fragments, difficulty, rank, limits, graze, communication, full player movement and RNG every tick including transitions. Only multi-stage runs exclude score and point value because native stage headers discard point-value low units (436f30); Extra compares both fields too. Terminal playback is advanced through the clear reward; post-clear live ending frames are outside the tick comparison.'});
 }finally{c.comp_delete(render);c.game_session_delete(s);c.resources_delete(source);c.replay_delete(r);for(const p of[data,rp,input,out,name])c.release(p);}
});
