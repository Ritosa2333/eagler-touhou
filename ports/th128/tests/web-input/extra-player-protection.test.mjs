import test from 'node:test';
import assert from 'node:assert/strict';
import {source} from './play-harness.mjs';

// Observe the actual checkWorld call at the native player boundary. The asset
// and module are seams here; the real core is used for spell regressions.
const begin=source.indexOf('        const playerPointer=memory(playerBytes);');
const end=source.indexOf('        if(!module._th128_world_enable_player_motion',begin);
assert(begin>=0&&end>begin);
const configure=new Function('module','memory','playerBytes','extraRun','thprac',source.slice(begin,end));
function playerContext(extraRun,thprac){
 let configured;
 configure({_th128_world_enable_player_context(...args){configured=args;return 1;},_free(){}},()=>8,new Uint8Array(12),extraRun,thprac);
 return configured;
}

test('owned EX recorder does not confer permanent protection',()=>{
 assert.deepEqual(playerContext({phase:'prepared'},null),[8,12,1,0,120,0,0,0,0,0,0]);
 assert.deepEqual(playerContext(null,{params:{perfect_freeze:30000}}),[8,12,1,0,120,0,0,0,0,0,30000]);
});
