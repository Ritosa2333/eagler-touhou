import {test} from 'node:test';
import assert from 'node:assert/strict';
import {createBuilder} from '../web/replay/builder.js';

test('match initializers follow selection, not native round transitions',async()=>{
  const rows=[2,1,1,0,1,2,1].map(scene=>({scene}));let frame=0,error;
  const replica={get frame(){return frame;},get scene(){return null;},stop(){},
    async save(){return {frame,bytes:new Uint8Array([frame])};},
    async advanceBatch(batch){frame+=batch.length;}};
  const tape={get length(){return rows.length;},row:index=>rows[index]},points=[];
  const builder=createBuilder({tickMs:1000/60},()=>tape,p=>points.push(p.frame),e=>{error=e;},{openReplica:async()=>replica});
  try{
    const deadline=Date.now()+3000;
    while((frame<rows.length||points.length<3)&&!error&&Date.now()<deadline)await new Promise(resolve=>setTimeout(resolve,10));
    assert.ifError(error);assert.deepEqual(points,[0,2,7]);
  }finally{builder.stop();}
});
