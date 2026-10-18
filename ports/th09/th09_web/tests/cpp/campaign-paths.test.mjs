import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync,readdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,memory,report,root} from './helpers.mjs';
test('Every story and extra character traverses nine stages, ending and staff, with restart and continue paths',async()=>{
 const c=await core(),f=c.session_create(),name=c.allocate(128),data=c.allocate(16000000),out=c.allocate(16000000);let frames=0,transitions=0,endings=0;const scenarios=[];
 for(const n of readdirSync(resolve(root,'reference/assets'))){const b=readFileSync(resolve(root,'reference/assets',n));memory(c,name,128).fill(0);memory(c,name,128).set(new TextEncoder().encode(n));memory(c,data,b.length).set(b);c.session_file(f,name,data,b.length);if(n.endsWith('.jpg')){memory(c,name,128).fill(0);memory(c,name,128).set(new TextEncoder().encode('data/end/'+n));c.session_file(f,name,data,b.length);}}
 const error=()=>{const b=memory(c,c.session_error(f),256);return new TextDecoder().decode(b.subarray(0,b.indexOf(0)));};
 const dv=()=>new DataView(c.memory.buffer);
 const input=keys=>{assert.equal(c.session_step(f,keys,0,keys),1,error());++frames;c.session_warm(f);};
 const ready=()=>{for(let n=0;n<240;++n)input(257);};
 const phase=()=>c.session_value(f,0),stage=()=>c.session_value(f,5);
 try{
  for(let mode=0;mode<2;++mode)for(let character=0;character<14;++character){const difficulty=mode?4:character%4;assert.equal(c.session_begin(f,character,mode,difficulty,0x1024+character*139),1,error());
   for(let n=0;n<9;++n){assert.equal(stage(),n);ready();c.session_end_round(f,0);for(let budget=0;phase()===1&&stage()===n&&budget<2200;++budget)input(257);assert.equal(phase(),n===8?4:1,`phase ${mode}/${character}/${n}`);++transitions;}
   for(let budget=0;phase()===4&&budget<3000;++budget)input(257);assert.equal(phase(),5,`ending ${mode}/${character}`);++endings;
   memory(c,name,128).fill(0);memory(c,name,128).set(new TextEncoder().encode('CAMPAIGN'));const size=c.session_save(f,name,out,16000000);assert.ok(size,error());const replay=Uint8Array.from(memory(c,out,size));
   for(let n=0;n<9;++n){memory(c,data,size).set(replay);assert.equal(c.session_play(f,data,size,n),1,error());assert.equal(stage(),n);for(let t=0;t<10;++t)input(0);}
   scenarios.push({mode,character,difficulty,bytes:size});
  }
  assert.equal(c.session_begin(f,0,0,0,42),1,error());for(let loss=0;loss<3;++loss){ready();c.session_end_round(f,1);for(let n=0;n<1200&&phase()===1;++n){input(257);if(loss<2&&dv().getInt32(c.session_part(f,0,17)+8,true)===0)break;}}
  // Drain all remaining lives until the real continue prompt.
  for(let n=0;n<4&&phase()===1;++n){ready();c.session_end_round(f,1);for(let t=0;t<500&&phase()===1;++t)input(257);}
  assert.equal(phase(),2);assert.equal(c.session_continue(f),1,error());assert.equal(c.session_value(f,4),1);assert.equal(phase(),1);assert.equal(c.session_retry(f),1,error());assert.equal(c.session_value(f,4),0);assert.equal(stage(),0);
  report('campaign-paths',{frames,transitions,endings,scenarios,forcedVictories:true,scope:'All 28 authored story/extra campaign paths, native assets, nine-stage transitions, endings/staff, saved stage selection, loss/continue/retry. Forced victories cover flow, not battle-outcome equivalence.'});
 }finally{c.session_delete(f);}
});
