import test from 'node:test';
import assert from 'node:assert/strict';
import {core,oracle,memory,string,report,enableNativeMath} from './helpers.mjs';
import {preloadStage} from './stage-assets-fixture.mjs';

test('TH15 presentation clock matches native start/end sampling, frame capture, rounding and checksum',async()=>{
 const c=await core(),m=await oracle(),spell=m.allocate(0xbc),fixture=c.spell_clock_create();enableNativeMath(m);
 m.u32(0x4e9a70,spell);m.u32(0x4e7794,0x100);m.u32(0x521e34,1);
 let now=0,reads=0,checks=0;
 m.replace(0x472c90,'observed platform clock',()=>{const bytes=new Uint8Array(16);new DataView(bytes.buffer).setFloat64(0,now,true);m.cpu.reg_write(m.uc.X86_REG_XMM0,bytes);reads++;return 0;});
 const data=()=>new DataView(c.memory.buffer),set=(field,offset,value)=>{data().setInt32(c.spell_clock_field(fixture,field),value,true);m.i32(spell+offset,value);};
 const compare=label=>{for(const[field,offset,size]of[[0,0x78,4],[1,0x8c,4],[2,0x88,4],[3,0x90,4],[4,0xa4,4],[5,0x94,8]])assert.deepEqual(Buffer.from(memory(c,c.spell_clock_field(fixture,field),size)),Buffer.from(m.bytes(spell+offset,size)),label+' field '+field);assert.equal(c.spell_clock_valid(data().getInt32(c.spell_clock_field(fixture,4),true)),Number(!(m.call(0x41f6e0,{ecx:spell})&255)));checks+=7;};
 try{
  const durations=[0,.008349999,.00835,.008350001,.009,.01,.0167,.019,1.234,59.99,60,61,999.999,1000,3600,86400,-.01,-1,-60];
  for(let i=0;i<256;i++)durations.push(i*1.375+.0167*(i%13)+.00835*(i%3));
  for(let sample=0;sample<durations.length;sample++){
   m.view(spell,0xbc).fill(0);for(const[field,offset,value]of[[0,0x78,0x800001],[1,0x8c,73+sample],[2,0x88,0],[3,0x90,-1],[4,0xa4,12345]])set(field,offset,value);
   data().setFloat64(c.spell_clock_field(fixture,5),0,true);now=50;reads=0;
   assert.equal(c.spell_clock_needs_sample(fixture),1);m.call(0x420060);assert.equal(c.spell_clock_sample(fixture,now),1);compare('start '+sample);assert.equal(reads,1);
   now=50+durations[sample]/2;assert.equal(c.spell_clock_needs_sample(fixture),0);m.call(0x420060);assert.equal(c.spell_clock_sample(fixture,now),0);compare('active '+sample);assert.equal(reads,1);
   set(0,0x78,0x800040);set(1,0x8c,137+sample);now=50+durations[sample];assert.equal(c.spell_clock_needs_sample(fixture),1);m.call(0x420060);assert.equal(c.spell_clock_sample(fixture,now),2);compare('end '+sample);assert.equal(reads,2);
   now+=100;assert.equal(c.spell_clock_needs_sample(fixture),0);m.call(0x420060);assert.equal(c.spell_clock_sample(fixture,now),0);compare('idle '+sample);assert.equal(reads,2);
  }
  for(const seconds of[-60,-1,0,1,60,999])for(const hundredths of[0,1,33,66,99]){m.call(0x41f730,{ecx:spell,args:[seconds,hundredths]});assert.equal(c.spell_clock_encode(seconds,hundredths),m.i32(spell+0xa4));checks++;}
  report('spell-presentation-clock',{passed:true,cases:durations.length,checks,originalFunctions:['0x420060','0x41f6e0','0x41f730'],scope:'Original after-presentation start/finish branches, exact double wall-time rounding, completed gameplay frames, persistent origin/result, checksum and sample call count. Clock comes from a platform boundary; full application wiring and replay snapshot exchange are verified separately.'});
 }finally{c.spell_clock_delete(fixture);m.close();}
});

test('TH15 spell timings survive live snapshot copies, original-format export and independent playback',async()=>{
 const c=await core(),value=c.allocate(4),label=c.allocate(9),asset=preloadStage(c,1,0),stage=c.stage_gameplay_create(asset.fixture);let live=0,reader=0,playAsset,playStage=0,playRun=0,file=0;
 const times=Array.from({length:20},(_,i)=>i===3?0:i===7?12345:c.spell_clock_encode(i*11,i*7%100));
 try{
  assert.equal(c.stage_gameplay_prepare(stage),1,string(c,c.stage_gameplay_error(stage)));live=c.stage_session_replay_create(stage);assert.equal(c.stage_session_replay_enter(live,0,0),1,string(c,c.stage_session_replay_error(live)));
  for(let i=0;i<times.length;i++){new DataView(c.memory.buffer).setInt32(value,times[i],true);assert.equal(c.stage_session_replay_timing(live,i,value),1);assert.equal(new DataView(c.memory.buffer).getInt32(c.stage_session_replay_header(live,1)+0x1e4+i*4,true),times[i]);}
  memory(c,label,9).set(Buffer.from('CLOCK'+String.fromCharCode(0)));assert.equal(c.stage_session_replay_export(live,label),1,string(c,c.stage_session_replay_error(live)));
  const bytes=Buffer.from(memory(c,c.stage_session_replay_data(live),c.stage_session_replay_size(live)));file=c.allocate(bytes.length);memory(c,file,bytes.length).set(bytes);reader=c.replay_create();assert.equal(c.replay_open(reader,file,bytes.length),1,string(c,c.replay_error(reader)));
  const decoded=c.replay_data(reader);for(let i=0;i<times.length;i++)assert.equal(new DataView(c.memory.buffer).getInt32(decoded+0xa4+0x1e4+i*4,true),times[i],'exported recording copy '+i);
  playAsset=preloadStage(c,1,0);playStage=c.stage_gameplay_create(playAsset.fixture);assert.equal(c.stage_gameplay_prepare(playStage),1);playRun=c.stage_session_replay_create(playStage);assert.equal(c.stage_session_replay_enter(playRun,file,bytes.length),1,string(c,c.stage_session_replay_error(playRun)));
  for(let i=0;i<times.length;i++){new DataView(c.memory.buffer).setInt32(value,-1,true);assert.equal(c.stage_session_replay_timing(playRun,i,value),1);assert.equal(new DataView(c.memory.buffer).getInt32(value,true),c.spell_clock_valid(times[i])?times[i]:c.spell_clock_encode(999,99),'imported timing/fallback '+i);assert.equal(new DataView(c.memory.buffer).getInt32(c.stage_session_replay_header(playRun,1)+0x1e4+i*4,true),times[i],'preserved imported bytes '+i);}
  assert.equal(c.stage_session_replay_timing(playRun,20,value),0,'bounded original 20-word table');
  report('session-spell-timings',{passed:true,slots:20,exportBytes:bytes.length,scope:'Live stage and recording snapshots both updated, actual original-format encrypted replay export reopened, independent gameplay session imports exact times, native checksum fallback for invalid timing words, untouched imported bytes and bounded table.'});
 }finally{if(playRun)c.stage_session_replay_delete(playRun);if(playStage)c.stage_gameplay_delete(playStage);playAsset?.close();if(reader)c.replay_delete(reader);if(file)c.release(file);if(live)c.stage_session_replay_delete(live);c.stage_gameplay_delete(stage);asset.close();c.release(value);c.release(label);}
});
