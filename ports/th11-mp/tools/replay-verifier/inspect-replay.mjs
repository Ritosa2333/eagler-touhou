import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {core,memory} from '../../th11_web/tests/cpp/helpers.mjs';
export async function inspectReplay(input,{signal}={}) {
  signal?.throwIfAborted();const c=await core(),raw=typeof input==='string'?readFileSync(input):Buffer.from(input),p=c.allocate(raw.length),r=c.replay_create();memory(c,p,raw.length).set(raw);
  assert.equal(c.replay_open(r,p,raw.length),1,'Valid TH11 replay');
  try{const bytes=Buffer.from(memory(c,c.replay_data(r),c.replay_size(r))),stages=[];
    for(let stage=1;stage<=7;stage++){const record=c.replay_stage(r,stage);if(record){const offset=new DataView(c.memory.buffer).getUint32(record+4,true);stages.push({stage,frames:bytes.readUInt32LE(offset+4)});}}
    return {character:bytes.readUInt32LE(92),subtype:bytes.readUInt32LE(96),difficulty:bytes.readUInt32LE(100),stages};
  }finally{c.replay_delete(r);c.release(p);}
}
