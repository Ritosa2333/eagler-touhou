import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {core,memory} from '../../th15_web/tests/cpp/helpers.mjs';
export async function inspectReplay(input,{signal}={}) {
  signal?.throwIfAborted();const c=await core(),bytes=typeof input==='string'?readFileSync(input):Buffer.from(input),p=c.allocate(bytes.length),file=c.replay_create();
  memory(c,p,bytes.length).set(bytes);assert.equal(c.replay_open(file,p,bytes.length),1,'valid replay');
  try {
    const decoded=Buffer.from(memory(c,c.replay_data(file),c.replay_size(file))),stages=[];
    for(let stage=1;stage<=7;stage++){const record=c.replay_stage(file,stage);if(record){const v=new DataView(c.memory.buffer),offset=v.getUint32(record+4,true),frames=v.getUint32(record+8,true);
      const inputs=decoded.subarray(offset+0x238,offset+0x238+frames*6);
      stages.push({stage,frames,terminalMarker:inputs.subarray(-6).equals(Buffer.alloc(6,255))});}}
    return {character:c.replay_field(file,1),difficulty:c.replay_field(file,2),mode:c.replay_field(file,3),clearStage:decoded.readUInt32LE(0x98),stages};
  }finally{c.replay_delete(file);c.release(p);}
}
if(process.argv[1]?.endsWith('inspect-replay.mjs'))console.log(JSON.stringify(await inspectReplay(process.argv[2]),null,2));
