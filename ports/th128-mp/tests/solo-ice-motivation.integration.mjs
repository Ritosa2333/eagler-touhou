// Real unmodified A1-3 DAT, solo World: reward ice damage independently of area and ice-star pickups.
import assert from 'node:assert/strict';
import {balanceFixture} from './coop-balance-fixture.mjs';
import {prepareA13Preview} from '../runtime/preview-world.mjs';
import {createMultiplayerRules} from '../../th128/site/runtime/multiplayer-rules.mjs';

const args=process.argv.slice(2),{options,build}=await balanceFixture(args);
const rules=createMultiplayerRules({schema:'th128-multiplayer-rules/1',mode:'parallel-stage',matchId:'solo-ice-motivation',
  epoch:1,route:'A1-3',seed:34772,inputDelay:0,rollbackWindow:0,...build});
const modules=[];
const preview=await prepareA13Preview({...options,async factory(args){
  const m=await options.factory(args),enable=m._th128_world_enable_shot_hits;
  // Begin below the motivation cap, as a new normal run does.
  m._th128_world_enable_shot_hits=(_,flag,busy,bypass,spell)=>enable(20000,flag,busy,bypass,spell);
  modules.push(m);return m;
}});
const plan=(f,up)=>f<up?16:f>=190&&f<250?1:0;
for(const up of [32,48,64]){
  const pair=await preview.openPair(rules);const native=modules.at(-2);
  const state=()=>JSON.parse(native.UTF8ToString(native._th128_world_state()));
  const damages=[];let prev=0,large=false;
  try{
    for(let f=0;f<490;f++){
      const h=plan(f,up),before=state();
      const input={player:0,held:h,pressed:h&~prev,released:prev&~h};prev=h;
      pair.step({frame:f,inputs:[input,{player:1,held:0,pressed:0,released:0}]});
      const after=state(),ice=(after.damageStats[0]-before.damageStats[0])>>>0,shot=(after.damageStats[1]-before.damageStats[1])>>>0;
      if(after.lastFrozenArea>=20)large=true;
      if(ice>0&&after.motivationRaw>=before.motivationRaw&&!shot){
        damages.push({frame:f,ice,gain:after.motivationRaw-before.motivationRaw});
      }
    }
    if(damages.length&&large){
      for(const {ice,gain,frame} of damages)assert.ok(gain>=Math.trunc(Math.min(ice,1700)/2),`frame ${frame}: ${ice} frozen damage gave only ${gain}/100% motivation`);
      console.log(JSON.stringify({largeFreeze:large,up,iceDamageFrames:damages.length,samples:damages.slice(0,6)}));
      process.exitCode=0;break;
    }
    console.log(JSON.stringify({up,large,damages:damages.slice(0,6),motivation:state().motivationRaw,lastArea:state().lastFrozenArea}));
    if(up===64)assert.fail('the original stage must yield both a large freeze and actual ice damage');
  }finally{pair.close();}
}
