import assert from 'node:assert/strict';
import {test} from 'node:test';
import {createPcmOutput,createTrackSelection} from '../native-solo/web/audio.mjs';

test('music and effects queue in parallel; a music reset keeps effects scheduled',()=>{
  const completed=[],nodes=[];
  const context={currentTime:1,destination:{},
    createBuffer(_channels,count,rate){return {duration:count/rate,getChannelData:()=>new Float32Array(count)};},
    createBufferSource(){
      const node={connect(){},disconnect(){},start(at){this.at=at;},stop(){this.stopped=true;}};
      nodes.push(node);return node;
    },
  };
  const pcm=createPcmOutput(()=>context,header=>completed.push(header));
  const samples=new Int16Array(960);
  pcm.write(0,100,samples);pcm.write(1,200,samples);
  assert.equal(nodes[0].at,nodes[1].at);
  pcm.reset(1);
  assert.equal(nodes[1].stopped,true);
  assert.equal(pcm.playing,false);
  assert.equal(nodes[0].stopped,undefined);
  nodes[0].onended();
  assert.deepEqual(completed,[100]);
});

test('a decoded recording cannot start after the game selects another track',async()=>{
  const requests=new Map(),played=[];
  const music=createTrackSelection((track,signal)=>new Promise(resolve=>requests.set(track,{signal,resolve})),assert.fail);
  const start=track=>()=>{played.push(track);return ()=>played.push(-track);};
  music.select(1);music.select(2);music.select(2);
  assert.equal(requests.get(1).signal.aborted,true);
  requests.get(2).resolve(start(2));
  requests.get(1).resolve(start(1));
  await Promise.resolve();
  music.select(null);
  assert.deepEqual(played,[2,-2]);
});
