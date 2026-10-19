import test from'node:test';import assert from'node:assert/strict';import{readFileSync}from'node:fs';import{resolve}from'node:path';import{core,memory,root,report}from'./helpers.mjs';
// Diagnostic coverage, not proof of a legitimate clear or replay equivalence:
// replenish only spare lives so every real stage script can reach its exit.
// Damage, collision, deaths, power loss, shots, Bombs and boss timers run normally.
test('TH11 complete campaign scripts and Extra reach their original exit', {skip:!process.env.TH11_CAMPAIGN_SOAK},async()=>{
 const c=await core(),raw=readFileSync(resolve(root,'../[th11] 东方地灵殿 (汉化版+日文版)/th11.dat')),data=c.allocate(raw.length),source=c.resources_create();memory(c,data,raw.length).set(raw);assert.equal(c.resources_open(source,data,raw.length),1);
 const samples=process.env.TH11_CAMPAIGN_SOAK==='all'?Array.from({length:30},(_,i)=>[Math.floor(i/5),i%5]):process.env.TH11_CAMPAIGN_SOAK.split(',').map(v=>v.split(':').map(Number));const cases=[];
 try{for(const[selection,difficulty]of samples){const s=c.game_session_create(),render=c.comp_create(),visited=[],name=`shot${selection} level${difficulty}`;let spareLivesGranted=0,frames=0,lastStage=0;
  const error=()=>{const b=memory(c,c.game_session_error(s),512);return new TextDecoder().decode(b.subarray(0,b.indexOf(0)));};
  try{assert.equal(c.game_session_begin(s,source,difficulty===4?7:1,Math.floor(selection/3),selection%3,difficulty,0),1,error());
   for(;frames<300000&&c.game_session_value(s,0)===1;++frames){
    const stage=c.game_session_value(s,2);if(stage!==lastStage){visited.push({stage,frame:frames});lastStage=stage;console.log(name,'stage',stage,'frame',frames);}
    const economy=c.battle_data(s,0),view=new DataView(c.memory.buffer),lives=view.getInt32(economy+16,true);if(lives<8)spareLivesGranted+=8-lives;view.setInt32(economy+16,8,true);
    const held=512|8|(frames%17===0?0:1)|(frames%180===0?2:0);
    assert.equal(c.game_session_update(s,held,0,0,0,0,0),1,`${name} stage${stage} frame${frames}: ${error()}`);
    assert.equal(c.game_session_render(s,render),1,`${name} stage${stage} draw frame${frames}: ${error()}`);
   }
   assert.ok(frames<300000,`${name} did not finish: ${JSON.stringify(visited)}`);assert.equal(c.game_session_value(s,0),difficulty===4?3:5,name+' final result/ending');
   assert.deepEqual(visited.map(x=>x.stage),difficulty===4?[7]:[1,2,3,4,5,6],name+' stage sequence');cases.push({selection,difficulty,frames,spareLivesGranted,visited});
  }finally{c.comp_delete(render);c.game_session_delete(s);}
 }report('campaign-soak',{passed:true,cases,frames:cases.reduce((n,x)=>n+x.frames,0),scope:'Diagnostic full stage scripts, stage exits, dialogue, damage/death, Bomb and resource transitions for the listed selections. Spare lives are replenished; this is not a legitimate clear, native whole-campaign comparison or rendered phone performance measurement.'});
 }finally{c.resources_delete(source);c.release(data);}
});
