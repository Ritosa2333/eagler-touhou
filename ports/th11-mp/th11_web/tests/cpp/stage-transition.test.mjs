import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,memory,root,report} from './helpers.mjs';

test('TH11 stage completion enters the next live stage while retaining player and outgoing resource owners',async()=>{
 const c=await core(),raw=readFileSync(resolve(root,'../[th11] 东方地灵殿 (汉化版+日文版)/th11.dat'));
 const data=c.allocate(raw.length),source=c.resources_create(),s=c.game_session_create(),render=c.comp_create();
 memory(c,data,raw.length).set(raw);assert.equal(c.resources_open(source,data,raw.length),1);
 const dv=()=>new DataView(c.memory.buffer),get=p=>dv().getInt32(p,true),put=(p,n)=>dv().setInt32(p,n,true);
 const error=()=>{const bytes=memory(c,c.game_session_error(s),512);return new TextDecoder().decode(bytes.subarray(0,bytes.indexOf(0)));};
 const value=k=>c.game_session_transition_value(s,k),step=()=>{assert.equal(c.game_session_update(s,0,0,0,0,0,0),1,error());assert.equal(c.game_session_render(s,render),1,error());};
 let transitions=0,frames=0;
 try{
  for(let selection=0;selection<6;++selection){
   assert.equal(c.game_session_begin(s,source,1,Math.floor(selection/3),selection%3,1,0),1,error());
   for(let i=0;i<70;++i)step();
   const player=c.game_session_transition_data(s,4),economy=c.battle_data(s,0),motion=c.battle_data(s,2);
   // An earned power item gives real option VMs that must be recalled and
   // rebuilt at the boundary rather than disappearing with the old stage.
   assert.equal(c.battle_item(s,4),1);const power=get(economy+4);
   put(economy+16,8);
   for(let stage=1;stage<6;++stage){
    const old=c.game_session_stage_data(s,0),oldFrames=get(old+0x300c),score=get(economy);
    const position=Buffer.from(memory(c,motion,12));
    assert.equal(c.game_session_complete(s,0),1,error());
    assert.equal(value(11),(8+stage)*1000000);
    assert.equal(get(economy),score+(8+stage)*100000);
    assert.ok(value(9),'stage-clear animation is created');
    assert.equal(c.game_session_update(s,0,0,0,0,0,0),1,error());
    assert.equal(c.game_session_value(s,2),stage+1);
    assert.equal(value(0),1);assert.equal(value(1),0);assert.equal(value(2),0);
    assert.equal(c.game_session_transition_data(s,0),old);
    assert.equal(c.game_session_transition_data(s,4),player);
    assert.equal(c.game_session_render(s,render),1,error());
    const next=c.game_session_stage_data(s,0);
    for(let tick=0;tick<119;++tick){step();++frames;assert.equal(value(1),0,'original gate waits until result clock reaches 120');assert.equal(get(next+0x300c),0);}
    assert.equal(value(5),120);assert.ok(get(old+0x300c)>oldFrames);
    for(let tick=0;tick<30;++tick){step();++frames;assert.equal(value(1),1);assert.equal(value(2),0,'new stage enemies stay disabled for the background handoff');}
    assert.equal(get(old+0x2ff0)&8,8,'outgoing draw timer completed');
    step();++frames;
    assert.equal(value(2),1);assert.equal(value(3),0);assert.equal(value(4),1,'stage game clock restarts at activation');
    assert.equal(value(6)&0x200,0);assert.equal(value(5),0);
    assert.equal(c.game_session_transition_data(s,4),player,'same player object across stages');
    assert.deepEqual(Buffer.from(memory(c,motion,12)),position,'input-free transition preserves the actual player position');
    assert.equal(get(economy+4),power,'power survives stage handoff');assert.equal(get(economy+16),8);
    assert.ok(c.battle_value(s,7)>0,'options rebuilt from the retained power');
    for(let tick=0;tick<80;++tick){step();++frames;}
    assert.equal(value(0),0);assert.equal(value(8),0,'old resources retire only after both animation registries release them');
    assert.ok(get(next+0x300c)>30);assert.equal(c.battle_value(s,5),1,'new enemy ANM binding');++transitions;
   }
  }
  report('stage-transition',{passed:true,transitions,frames,scope:'Archive-backed MSG21 completion through stages 1–6 for six selections: result gate, overlapping STD draw, retained player/score/power, option recall/rebuild, enemy activation, resource retirement. Native control/event comparison and full original replay remain separate gates.'});
 }finally{c.comp_delete(render);c.game_session_delete(s);c.resources_delete(source);c.release(data);}
});
