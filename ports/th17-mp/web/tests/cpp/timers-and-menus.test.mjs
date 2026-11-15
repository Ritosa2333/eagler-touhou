import test from 'node:test';
import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';
test('TH17 SSE timer stepping preserves native state at fractional and integer limits',async()=>{
 const c=await core(),m=await oracle(),timer=c.allocate(20),native=m.allocate(20),scratch=m.allocate(4),wrapper=m.allocate(32),b=Buffer.from([0xf3,0x0f,0x10,0x0d,0,0,0,0,0xe8,0,0,0,0,0xc3]);b.writeUInt32LE(scratch,4);b.writeInt32LE(0x402a90-wrapper-13,9);m.write(wrapper,b);let checks=0;
 try{
  for(const speed of [0,.5,.99,.999,1,1.009,1.01,2,-1,NaN,Infinity])for(const value of [-2147483648,-999999,-1.5,0,1.75,16777216,2147483648]){
   const data=Buffer.alloc(20);data.writeInt32LE(11,0);data.writeInt32LE(Number.isFinite(value)&&value<2147483648?Math.trunc(value):0,4);data.writeFloatLE(value,8);data.writeUInt32LE(1,12);data.writeUInt32LE(0x12345679,16);memory(c,timer,20).set(data);m.write(native,data);m.f32(0x4b5918,speed);
   for(let i=0;i<5;i++){m.call(0x402ab0,{ecx:native});c.timer_tick(timer,speed);assert.deepEqual(Buffer.from(memory(c,timer,20)),Buffer.from(m.bytes(native,20)),JSON.stringify({speed,value,i}));checks++;}
   m.f32(scratch,speed);m.call(wrapper,{ecx:native});c.timer_add(timer,speed);assert.deepEqual(Buffer.from(memory(c,timer,20)),Buffer.from(m.bytes(native,20)));checks++;
  }
  report('timer',{passed:true,checks,functions:['0x402ab0','0x402a90']});
 }finally{m.close();c.deallocate(timer);}
});
test('Menu navigation, disabled entries and nested selection state match TH17',async()=>{
 const c=await core(),m=await oracle(),cursor=c.allocate(0xd8),native=m.allocate(0xd8);let checks=0;
 try{
  for(let count=2;count<=20;count++)for(const wrap of [0,1]){
   const data=Buffer.alloc(0xd8);data.writeInt32LE(count,8);data.writeInt32LE(wrap,0xd0);data.writeInt32LE(1,0x90);data.writeInt32LE(count>2?1:0,0xd4);memory(c,cursor,data.length).set(data);m.write(native,data);
   const sequence=[[1,count+2],[0,-1],[0,1],[0,2],[2,0],[1,-20],[0,-1],[2,0],[1,3],[3,0],[3,0],[3,0]];
   for(const [kind,value] of sequence){m.call([0x402a10,0x41b630,0x402970,0x4029c0][kind],{ecx:native,args:kind<2?[value]:[]});c.cursor_operation(cursor,kind,value);assert.deepEqual(Buffer.from(memory(c,cursor,0xd8)),Buffer.from(m.bytes(native,0xd8)),JSON.stringify({count,wrap,kind,value}));checks++;}
  }
  report('menu-cursor',{passed:true,checks,functions:['0x402a10','0x41b630','0x402970','0x4029c0']});
 }finally{m.close();c.deallocate(cursor);}
});
