import assert from 'node:assert/strict';
import test from 'node:test';
import {Netplay} from '../web/sdl-runtime/netplay.mjs';
function simulate(rtt,delay=0){
 const queue=[],committed=[[],[]];let now=0;
 const peers=[0,1].map(side=>{
  const core={HEAP32:new Int32Array(0),_th19_rollback_can_predict:()=>0,
   _th19_network_step:(f,a,b)=>{committed[side].push([f,a,b]);return 1;},
   _th19_played_frames:()=>1,_th19_network_hash:()=>42};
  const p=new Netplay(core);Object.assign(p,{side,delay,lastTick:0,budget:0,inputBudget:0,nextInputFrame:delay});
  p.sample=()=>({keys:(Math.floor(now/240)%2)?1:0,direct:0,x:0,y:0});
  p.send=bytes=>{if(bytes[5]===3)queue.push({at:now+rtt/2,to:1-side,bytes:bytes.slice()});};
  p.confirm=()=>{};p.compare=()=>{};p.prepareRollback();return p;
 });
 globalThis.__th19PeerTransport={peers:new Map([[0,{rttMs:rtt}],[1,{rttMs:rtt}]])};
 for(now=8;now<=6000;now+=8){
  for(let i=queue.length-1;i>=0;i--)if(queue[i].at<=now){const {to,bytes}=queue.splice(i,1)[0],v=new DataView(bytes.buffer);peers[to].rollback.receive(v.getUint32(6,true),{keys:v.getUint16(10,true),direct:bytes[12],x:v.getFloat32(13,true),y:v.getFloat32(17,true)});}
  for(const p of peers)p.pumpRollback(now);
 }
 const frames=Math.min(...peers.map(p=>p.frame));
 assert.deepEqual(committed[0].slice(0,frames),committed[1].slice(0,frames));
 const lead=peers.map(p=>p.nextInputFrame-p.frame);
 return {frames,fps:frames/6,lead};
}
for(const rtt of [40,100,200])test(`confirmed UI input pipeline at ${rtt}ms RTT`,()=>{
 const result=simulate(rtt);console.log({rtt,...result});assert.ok(result.fps>55,JSON.stringify(result));assert.ok(result.lead.every(n=>n<=16));
});
test('configured input delay remains contiguous',()=>{assert.ok(simulate(100,4).fps>55);});
