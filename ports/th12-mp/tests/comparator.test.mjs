import test from 'node:test';import assert from 'node:assert/strict';import {compareTraces} from '../tools/replay-verifier/compare-traces.mjs';
const identity=Object.fromEntries(['replaySha256','executableSha256','resourceSha256','stateSchemaSha256'].map(k=>[k,'a'.repeat(64)]));
const trace=()=>({schema:'th12-gameplay-trace/1',identity,completion:{complete:true,ticks:2},ticks:[0,1].map(frame=>({frame,input:{held:0},player:{x:0,y:400},rng:{seed:1,calls:0},ecl:[],enemies:[],bullets:[],items:[],economy:{score:0},stage:1}))});
test('reports earliest authoritative divergence',()=>{const a=trace(),b=trace();b.ticks[1].player.x=1;assert.deepEqual(compareTraces(a,b),{status:'diverged',frame:1,path:'state.player.x',expected:0,actual:1});});
test('rejects shifted, cropped, incomplete and identity-incompatible traces',()=>{
  for(const mutate of [t=>t.ticks[0].frame++,t=>t.ticks.pop(),t=>delete t.ticks[0].bullets,t=>t.completion.complete=false,t=>t.identity={...t.identity,replaySha256:'b'.repeat(64)}]){const b=trace();mutate(b);assert.throws(()=>compareTraces(trace(),b));}
});
