import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';
test('TH11 frame windows and FPS display requests match original callbacks',async()=>{
 const c=await core(),m=await oracle(),counter=c.frame_stats_create(),label=c.allocate(16),native=m.allocate(0x8c),game=m.allocate(0x80),clock=m.allocate(8),supervisor=m.allocate(0x184c0);
 m.view(native,0x8c).fill(0);m.view(game,0x80).fill(0);m.u32(0x4a8d80,native);m.u32(0x4a8e88,game);m.u32(0x4a8d58,supervisor);
 // The platform timer returns a supplied double in ST0. All statistics and
 // formatting-request arithmetic still run in the original instructions.
 const timer=Buffer.from([0xdd,0x05,0,0,0,0,0xc3]);timer.writeUInt32LE(clock,2);m.write(0x446920,timer);
 let request; m.replace(0x401600,'capture original FPS ASCII request',()=>{request={color:m.u32(supervisor+0x18480),position:[0,4,8].map(off=>m.f32(m.reg('EBX')+off)),format:m.string(m.u32(m.reg('ESP')+4),32),value:Buffer.from(m.bytes(m.reg('ESP')+8,8)).readDoubleLE()};return 0;});
 let time=0,checks=0,labels=0;
 try{for(let i=0;i<6000;++i){const active=i%11!==0,ticks=i%251===0?3:1;time+=1/[120,90,60,50,30,15,75][Math.floor(i/180)%7];if(i&&i%777===0)time-=5;
  const b=Buffer.alloc(8);b.writeDoubleLE(time);m.write(clock,b);m.u32(game+0x60,active?0:4);m.view(0x4c3466,1)[0]=ticks-1;
  c.frame_stats_sample(counter,time,+active,ticks);m.call(0x419d90);
  const values=new Float64Array(c.memory.buffer,c.frame_stats_values(counter),7),want=[Buffer.from(m.bytes(native+0x14,8)).readDoubleLE(),Buffer.from(m.bytes(native+0x24,8)).readDoubleLE(),Buffer.from(m.bytes(native+0x2c,8)).readDoubleLE(),m.u32(native+0x20),m.u32(native+0x1c),m.f32(native+0x34)];
  assert.deepEqual(Array.from(values).slice(0,6),want,'window '+i);assert.equal(values[6],want[2]?Math.fround(100-Math.fround(want[1]/want[2])*100):0);++checks;
  if(i%53===0){m.reg('EAX',native);m.call(0x419ea0);const p=c.frame_stats_label(counter,label),fields=Array.from(new Uint32Array(c.memory.buffer,label,4)),text=new TextDecoder().decode(memory(c,p,64).subarray(0,memory(c,p,64).indexOf(0)));
   assert.deepEqual(fields,[request.color,1,request.position[0],request.position[1]]);assert.equal(request.format,'%2.1ffps');assert.equal(request.value,want[5]+.05);assert.equal(text,request.value.toFixed(1)+'fps');++labels;
  }
 }report('frame-statistics',{passed:true,checks,labels,scope:'Original 419d90 one-second sampling, backwards clocks, skipped draw counts, active/inactive slowdown accounting, plus 419ea0 FPS color/position/format arguments. Timer source and ASCII output are captured at their host boundaries.'});
 }finally{c.frame_stats_delete(counter);c.release(label);m.close();}
});
