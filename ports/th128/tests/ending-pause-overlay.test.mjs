import test from 'node:test';
import assert from 'node:assert/strict';
import {functionSource} from './web-input/play-harness.mjs';

test('pausing an Ending leaves its picture visible while native pause and resume still run',async()=>{
 const inputs=[],node={hidden:true},module={},state={module,endingTicket:4,paused:false,
  endingHost:{owner:{input:{release(){inputs.push('release');}},setPaused(value){inputs.push(value);}}},
  audio:{state:'running',async suspend(){this.state='suspended';},async resume(){this.state='running';}}};
 const api=new Function('state','find','clock','resetHighFrameratePresentation','titlePauseOverlay','status','error',
  [functionSource('pauseOriginalEnding'),functionSource('resumeOriginalEnding'),'return {pauseOriginalEnding,resumeOriginalEnding};'].join('\n'))(
  state,()=>node,{reset(){}},()=>{},()=>{node.hidden=false;},()=>{},error=>{throw error;});
 api.pauseOriginalEnding();await state.endingPausePromise;
 assert.equal(state.paused,true);assert.equal(node.hidden,true);assert.deepEqual(inputs,['release',true]);
 await api.resumeOriginalEnding();assert.equal(state.paused,false);assert.equal(state.audio.state,'running');assert.deepEqual(inputs,['release',true,false]);
});
