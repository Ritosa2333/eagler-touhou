import test from 'node:test';
import assert from 'node:assert/strict';
import {Netplay} from '../web/sdl-runtime/netplay.mjs';
function simulate(rtt){
 let now=0;const queue=[],frames=[[],[]],draws=[0,0];
 const peers=[0,1].map(side=>{const core={_th19_network_step:(frame,...args)=>{frames[side].push(frame);draws[side]+=args.at(-1);return 1;}};const n=new Netplay(core);Object.assign(n,{side,selectionFrame:0,selectionNext:0,selectionLocal:new Map(),selectionRemote:new Map(),selectionTick:0,selectionBudget:0,selectionInputBudget:0});n.sample=()=>({keys:0});n.send=bytes=>queue.push({at:now+rtt/2,to:1-side,bytes:bytes.slice()});return n;});
 globalThis.__th19PeerTransport={peers:new Map([[0,{rttMs:rtt}],[1,{rttMs:rtt}]])};
 for(now=8;now<=6000;now+=8){for(let i=queue.length-1;i>=0;i--)if(queue[i].at<=now){const q=queue.splice(i,1)[0],d=new DataView(q.bytes.buffer);peers[q.to].selectionRemote.set(d.getUint32(6,true),d.getUint16(10,true));}for(const n of peers)n.pumpSelection(now);}
 assert.deepEqual(frames[0],frames[1]);assert(frames[0].every((f,i)=>f===i));return {rtt,fps:frames[0].length/6,draws,lead:peers.map(n=>n.selectionNext-n.selectionFrame)};
}
for(const rtt of [40,100,200])test(`selection scheduling at ${rtt}ms RTT`,()=>{const r=simulate(rtt);console.log(r);if(!process.env.BASELINE)assert(r.fps>55,JSON.stringify(r));});
test('selection catch-up presents only its final available frame',()=>{let draws=0,ticks=0;const n=new Netplay({_th19_network_step:(...args)=>{ticks++;draws+=args.at(-1);return 1;}});Object.assign(n,{side:0,selectionFrame:0,selectionNext:4,selectionLocal:new Map([0,1,2,3].map(f=>[f,0])),selectionRemote:new Map([0,1,2,3].map(f=>[f,0])),selectionTick:0,selectionBudget:0,selectionInputBudget:0});n.send=()=>{};n.sample=()=>({keys:0});n.pumpSelection(70);assert.equal(ticks,4);if(!process.env.BASELINE)assert.equal(draws,1);console.log({ticks,draws});});
