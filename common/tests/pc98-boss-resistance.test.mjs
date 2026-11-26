import {test} from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {gunzipSync} from 'node:zlib';
import {Fat12} from '../../ports/th04/web/solo-fat12.js';
import {installBossResistance,updateBossResistance} from '../browser/pc98-boss-resistance.mjs';
const game='04';
for(const language of ['jp','cn'])test(`TH${game} ${language} installs and tracks living players`,()=>{
 const patch=JSON.parse(readFileSync(new URL(`../../ports/th04/web/patch${language==='jp'?'-jp':''}.json`,import.meta.url)));
 const file=`ports/th04/web/th04-coop${language==='jp'?'-jp':''}.hdi.gz`;
 const fat=new Fat12(gunzipSync(readFileSync(new URL(`../../${file}`,import.meta.url)))),exe=fat.read(fat.find('GENSO/MAIN.EXE'));
 installBossResistance(exe,game,patch);
 const heap=new Uint8Array(131072),code=4096,mailbox=code+patch.mailbox_cs_offset,scale=code+0xad7c+1,marks=[];
 for(let count=1;count<=3;count++)for(let mask=0;mask<8;mask++){
  for(let seat=0;seat<3;seat++)heap[mailbox+patch.resource_schema.offset+seat*patch.resource_schema.stride+6]=(mask>>seat)&1;
  const alive=Array.from({length:count},(_,seat)=>!(mask&(1<<seat))).filter(Boolean).length;
  updateBossResistance(heap,game,mailbox,patch,count,(at,size)=>marks.push([at,size]));
  assert.equal(heap[scale],alive>=3?2:alive===2?3:5);
 }
 const offline=code+patch.offline_mask_cs_offset;heap.fill(0,offline,offline+1);
 for(let seat=0;seat<3;seat++)heap[mailbox+patch.resource_schema.offset+seat*patch.resource_schema.stride+6]=0;
 heap[offline]=4;updateBossResistance(heap,game,mailbox,patch,3,(at,size)=>marks.push([at,size]));assert.equal(heap[scale],3);assert(marks.some(([at,size])=>at===scale&&size===1));
});
