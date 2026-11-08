import {readFile} from 'node:fs/promises';

export async function compareMotion(frame,path){
 const capture=JSON.parse(await readFile(path,'utf8'));
 const results=await frame.evaluate(({player,bullets,lasers,slowdown,rank})=>{
  const m=Module;
  const results=[];
  function compare(name,frames,step){
   let first;
   let mismatches=0;
   for(const [tick,expected] of frames.entries()){
    const actual=step(tick,expected.length);
    if(JSON.stringify(actual)!==JSON.stringify(expected)){
     mismatches++;
     first??={tick,expected,actual};
    }
   }
   results.push({name,frames:frames.length,mismatches,...(first&&{first})});
  }
  for(const [character,frames] of player.cases.entries()){
   if(!m._th05_native_player_motion_start(character))throw Error('Could not start player movement fixture');
   compare('player-'+character,frames,(tick,length)=>{
    const addr=m._th05_native_player_motion_step(player.keys[tick]);
    return Array.from(new Int32Array(m.HEAPU8.buffer,addr,length));
   });
  }
  for(const fixture of bullets){
   m._th05_native_bullet_motion_start(...fixture.start.slice(0,8));
   if(fixture.start.length>9)m._th05_native_bullet_motion_player(...fixture.start.slice(9));
   compare(fixture.name,fixture.frames,(tick,length)=>{
    const addr=m._th05_native_bullet_motion_step(tick,+(tick===fixture.start[8]));
    return Array.from(new Int32Array(m.HEAPU8.buffer,addr,length));
   });
  }
  for(const fixture of lasers){
   m._th05_native_laser_motion_start(...fixture.start.slice(0,-1));
   compare(fixture.name,fixture.frames,(tick,length)=>{
    const addr=m._th05_native_laser_motion_step(+(tick===fixture.start.at(-1)));
    return Array.from(new Int32Array(m.HEAPU8.buffer,addr,length));
   });
  }
  compare('slowdown',slowdown.map(row=>[row.at(-1)]),tick=>[
   m._th05_native_slowdown_probe(...slowdown[tick].slice(0,-1)),
  ]);
  const values=(pointer,length)=>Array.from(new Int32Array(m.HEAPU8.buffer,pointer,length));
  compare('rank-initial',rank.initial,difficulty=>values(m._th05_native_rank_initial(difficulty),3));
  if(!m._th05_native_audit_start(0,1,24,0))throw Error('Could not start Rank fixture');
  compare('rank-tune',rank.tuning.map(row=>row.at(-1)),tick=>values(m._th05_native_rank_tune(...rank.tuning[tick].slice(0,-1)),5));
  compare('rank-speed',rank.speeds,(tick,length)=>Array.from({length},(_,speed)=>m._th05_native_rank_speed(tick+16,speed)));
  if(!m._th05_native_audit_start(0,1,24,0))throw Error('Could not start Rank item fixture');
  compare('rank-items',rank.items.map(row=>row.slice(4)),tick=>values(m._th05_native_rank_item(...rank.items[tick].slice(0,4)),2));
  compare('rank-stage-carry',[[rank.afterStageInit]],()=>[m._th05_native_rank_carry()]);
  if(!m._th05_native_stage_start(0,2))throw Error('Could not start Rank summary fixture');
  compare('rank-summary',rank.summary.map(row=>row.slice(2)),tick=>[m._th05_native_rank_summary(...rank.summary[tick].slice(0,2))]);
  return results;
 },capture);
 for(const result of results)console.log(JSON.stringify(result));
 if(results.some(result=>result.mismatches))throw Error('TH05 motion differs from PC-98');
}
