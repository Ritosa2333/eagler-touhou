import {test} from 'node:test';
import assert from 'node:assert/strict';
import {freezeFrameWork} from './freeze-frame-work.mjs';
test('freeze preserves every sample and records the final cumulative boundary',()=>{
  const work={capture:{first:1,last:3,count:3},samples:[1,2,3],observer:()=>{}};
  let running=true;const module={onGameWorkFrame:work.observer,_th13_loop_stop(){running=false;}};
  globalThis.document={querySelector:()=>({contentWindow:{Module:module,__frameWork:work,__framebufferCapture:{candidate:null}}})};
  const boundary=freezeFrameWork();assert.equal(running,false);assert.deepEqual(work.samples,[1,2,3]);
  assert.equal(boundary.first,1);assert.equal(boundary.last,3);assert.equal(boundary.count,3);
});
test('missing or replaced observer fails; unassociated candidate remains visible',()=>{
  const work={capture:{first:1,last:1,count:1},observer:()=>{}};
  for(const mode of ['missing','replaced','candidate']) {
    const module=mode==='missing'?{}:{_th13_loop_stop(){},onGameWorkFrame:mode==='replaced'?()=>{}:work.observer};
    globalThis.document={querySelector:()=>({contentWindow:{Module:module,__frameWork:work,__framebufferCapture:{candidate:mode==='candidate'?{}:null}}})};
    if(mode==='candidate')assert.equal(freezeFrameWork().unmatchedCandidate,true);
    else assert.throws(freezeFrameWork,/Cannot freeze/);
  }
});
