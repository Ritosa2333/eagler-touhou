import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,memory,root,report} from './helpers.mjs';
test('TH11 live laser cancellation emits converted player shots with original target and resource ownership',async()=>{
 const c=await core(),bytes=readFileSync(resolve(root,'../[th11] 东方地灵殿 (汉化版+日文版)/th11.dat')),data=c.allocate(bytes.length),source=c.resources_create(),s=c.game_session_create();let scenarios=0;
 memory(c,data,bytes.length).set(bytes);assert.equal(c.resources_open(source,data,bytes.length),1);
 const error=()=>{const p=c.game_session_error(s),b=memory(c,p,256);return new TextDecoder().decode(b.subarray(0,b.indexOf(0)));};
 try{for(let selection=0;selection<6;++selection)for(const infinite of[0,1])for(const count of[1,60]){
  assert.equal(c.game_session_begin(s,source,1,Math.floor(selection/3),selection%3,1,0),1,error());
  // Original line uses <=80, infinite uses <80: five versus four 16-unit samples.
  const samples=(infinite?4:5)*count;
  assert.equal(c.battle_converted_cut(s,infinite,count),samples,'actual manager -> GameBattle -> ShotManager');
  for(const kind of[0,1,2])assert.equal(c.battle_converted_count(s,kind),Math.min(samples,256),'pool / target / selected character ANM');
  assert.equal(c.game_session_value(s,5),0,'pool overflow is not a game failure');
  assert.equal(c.game_session_update(s,0,0,0,0,0,0),1,error());++scenarios;
 }report('converted-laser-integration',{passed:true,scenarios,scope:'Actual original-resource GameSession, line/infinite LaserManager circle cuts with both reward bits, live converted-shot adapter, selected character animation, enemy target and capacity overflow. The special-attack controller that requests these cuts remains unimplemented.'});
 }finally{c.game_session_delete(s);c.resources_delete(source);c.release(data);}
});
