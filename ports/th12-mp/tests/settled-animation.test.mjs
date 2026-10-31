import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {AnimationSampler} from '../src/animation.mjs';
const retail=JSON.parse(readFileSync(new URL('../site/assets/atlases.json',import.meta.url),'utf8'));
function snapshot(track){return {state:track.state,pc:track.pc,clock:track.clock,tickCount:track.tickCount,seed:track.seed,saved:track.saved,locals:[...track.locals],children:track.children.map(snapshot)};}
function run(atlases,name,id,age,{options={},forward=false,edit=null}={}){
  let randomCalls=0;const sampler=new AnimationSampler(atlases,{displayRandom32:()=>++randomCalls*127});
  sampler.sampleAll(name,id,0,null,options);const track=[...sampler.tracks.values()][0];let ticks=0;
  const tick=track.tick;track.tick=function(){ticks++;return tick.call(this);};edit?.(track);
  if(forward)while(track.tickCount<=age)track.tick();
  const poses=sampler.sampleAll(name,id,age,null,options);
  return {sampler,track,ticks,poses,randomCalls,world:snapshot(track),diagnostics:sampler.diagnostics()};
}
for(const [id,options]of [[0,{}],[13,{interrupt:2}],[13,{interrupt:3}],[13,{interrupt:7}],[21,{interrupt:2}],[21,{interrupt:3}],[80,{interrupts:[7,10,2]}]]){
  test(`retail HUD ${id} ${JSON.stringify(options)} skips settled ages without changing child clocks or output`,()=>{
    const reference=run(retail,'front',id,18000,{options,forward:true}),actual=run(retail,'front',id,18000,{options});
    assert.deepEqual(actual.poses,reference.poses);assert.deepEqual(actual.world,reference.world);assert.deepEqual(actual.diagnostics,reference.diagnostics);
    assert.equal(actual.randomCalls,reference.randomCalls);assert.ok(actual.ticks<200);assert.equal(reference.ticks,18000);
    const saved=structuredClone(actual.poses);actual.sampler.sampleAll('front',id,60000,null,options);assert.deepEqual(actual.poses,saved);
    assert.deepEqual(actual.sampler.sampleAll('front',id,3,null,options),new AnimationSampler(retail).sampleAll('front',id,3,null,options));
    for(const track of actual.sampler.tracks.values())assert.equal(track.frames.length,1);
  });
}
function commands(instructions){return instructions.map(([op,values=[],formats='i'.repeat(values.length),time=0],index)=>{
  const bytes=Buffer.alloc(values.length*4);[...formats].forEach((format,i)=>format==='f'?bytes.writeFloatLE(values[i],i*4):bytes.writeInt32LE(values[i],i*4));
  return {op,time,offset:index*32,mask:0,ints:values.map((_,i)=>bytes.readInt32LE(i*4)),floats:values.map((_,i)=>bytes.readFloatLE(i*4))};
});}
const fixtures=[
  ['rotation',[[53,[0,0,.1],'fff'],[63]]],
  ['scale',[[54,[.02,0],'ff'],[63]]],
  ['UV scroll',[[70,[.25],'f'],[63]]],
  ['held interpolation',[[56,[30,0,12,20,0],'iifff'],[63]]],
  ['future commands',[[3,[1]],[3,[2],'i',100],[63,[],'',200]]],
  ['future display RNG',[[87,[1]],[40,[100,10],'ii',100],[63,[],'',101]]],
  ['UV wrapping',[[63]],track=>{track.state.u=3.5;track.state.v=-2.5;}],
  ['authored angle normalization',[[63]],track=>{track.state.rotation=15;}],
  ['moving child',[[88,[1]],[63]],null,[[53,[0,0,.1],'fff'],[63]]],
];
for(const [name,instructions,edit=null,child=[]]of fixtures)test('settled sampling preserves '+name,()=>{
  const atlases={test:{scripts:[{commands:commands(instructions)},{commands:commands(child)}]}};
  const actual=run(atlases,'test',0,150,{edit}),reference=run(atlases,'test',0,150,{edit,forward:true});
  assert.deepEqual(actual.poses,reference.poses);assert.deepEqual(actual.world,reference.world);assert.equal(actual.randomCalls,reference.randomCalls);
  if(name==='future display RNG')assert.equal(actual.randomCalls,1);
  if(['rotation','scale','UV scroll','moving child','future commands'].includes(name))assert.equal(actual.ticks,150);
});
