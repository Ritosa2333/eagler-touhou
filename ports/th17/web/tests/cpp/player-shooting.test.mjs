import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,string,report} from './helpers.mjs';
const fields=[[0,0x18dd4,20],[1,0x18de8,20],[2,0x19080,20],[3,0x19098,12],[4,0x19094,4],[5,0x19040,4],[6,0x19044,1],[7,0x610,12],[8,0x61c,8],[9,0x18dd0,4]];
test('TH17 normal firing and Youmu charge/release/roaring timing preserve original state and requests',async()=>{
 const c=await core(),m=await oracle(),owner=m.allocate(0x19100),panning=m.allocate(4);m.u32(0x4b77d0,owner);m.u32(0x4a0430,0x4b5918);let events=[],nextHandle=1000,frames=0,checks=0;
 const event=(...e)=>events.push(...e.map(x=>x>>>0),...Array(8-e.length).fill(0));
 m.replace(0x44a710,'observe SHT generation request',()=>{const sp=m.reg('ESP');event(1,m.u32(sp+4),m.u32(sp+8),m.u32(owner+0x610),m.u32(owner+0x614),m.u32(owner+0x618),m.u32(owner+0x18dd0),0);return 0;},2);
 for(const [address,action]of [[0x4654f0,0],[0x4655a0,2]]){
  const stub=m.registerImport({dll:'fixture',name:'observe shot audio',argc:1,handler:()=>{event(2,m.u32(m.reg('ESP')+4),action,m.u32(panning));return 0;}}),bridge=m.allocate(32),code=Buffer.from([0xf3,0x0f,0x11,0x15,0,0,0,0,0xe9,0,0,0,0]);code.writeUInt32LE(panning,4);code.writeInt32LE(stub-bridge-code.length,9);m.write(bridge,code);const jump=Buffer.alloc(5);jump[0]=0xe9;jump.writeInt32LE(bridge-address-5,1);m.write(address,jump);
 }
 m.replace(0x4655d0,'observe stop shot audio',()=>{event(2,m.u32(m.reg('ESP')+4),1,0);return 0;},1);
 m.replace(0x4112b0,'observe charge indicator creation',()=>{const sp=m.reg('ESP'),out=m.u32(sp+4),pos=m.u32(sp+12),handle=++nextHandle;event(3,m.u32(sp+8),m.u32(pos),m.u32(pos+4),m.u32(pos+8),handle);m.u32(out,handle);return out;},6);
 m.replace(0x476620,'observe charge indicator retirement',()=>{event(4,m.u32(m.reg('ESP')+4));return 0;},1);
 m.replace(0x476510,'observe charge indicator interrupt',()=>{event(5,m.u32(m.reg('ESP')+4),m.u32(m.reg('ESP')+8));return 0;},2);
 try{
  for(let character=0;character<3;character++)for(let animal=0;animal<3;animal++)for(let scenario=0;scenario<8;scenario++){
   const fixture=c.player_shooting_create();m.view(owner,0x19100).fill(0);nextHandle=1000;for(const[field,offset,n]of fields)m.write(owner+offset,Buffer.from(memory(c,c.player_shooting_field(fixture,field),n)));
   m.i32(0x4b59f4,character);m.i32(0x4b59f8,animal);
   for(let frame=0;frame<720;frame++){
    const cycle=frame%240,held=scenario%4===0?true:scenario%4===1?cycle<90||cycle>=150:scenario%4===2?cycle%11<3:cycle<65||cycle>180;
    const focus=scenario%3===0?1:scenario%3===1?cycle<140:cycle>=30&&cycle<180,state=frame%313<5?frame%5:1,rate=[1,.5,.25,1.25,.995,1.005,1,1][scenario],roar=scenario<4?0:frame%360>=180?2|(scenario&1?4:0):0,kind=[0,1,2,3][Math.floor(frame/180)%4];
    const dv=new DataView(c.memory.buffer);for(const[field,offset,n]of [[5,0x19040,4],[6,0x19044,1]]){memory(c,c.player_shooting_field(fixture,field),n).fill(n===1?1:0);if(n===4)dv.setInt32(c.player_shooting_field(fixture,field),19,true);n===1?m.view(owner+offset,1).fill(1):m.i32(owner+offset,19);}
    dv.setInt32(c.player_shooting_field(fixture,9),focus,true);m.i32(owner+0x18dd0,focus);
    for(let axis=0;axis<3;axis++){const value=Math.fround((frame%37-19)*.03125+axis*31.25);dv.setFloat32(c.player_shooting_field(fixture,7)+axis*4,value,true);m.f32(owner+0x610+axis*4,value);if(axis<2){dv.setInt32(c.player_shooting_field(fixture,8)+axis*4,Math.trunc(value*128),true);m.i32(owner+0x61c+axis*4,Math.trunc(value*128));}}
    m.i32(owner+0x18db0,state);m.u32(0x4b3448,held?1:0);m.f32(0x4b5918,rate);m.u32(0x4b5ac4,roar);m.i32(0x4b5abc,kind);events=[];
    assert.equal(c.player_shooting_step(fixture,state,held,rate,character,animal,roar,kind),1,string(c,c.player_shooting_error(fixture)));assert.equal(m.call(0x44a8d0,{ecx:owner}),0);
    const label=JSON.stringify({character,animal,scenario,frame,state,held,focus,roar,kind});
    for(const[field,offset,n]of fields)assert.deepEqual(Buffer.from(memory(c,c.player_shooting_field(fixture,field),n)),Buffer.from(m.bytes(owner+offset,n)),label+' field '+field);
    for(let i=0;i<12;i++)assert.equal(new DataView(c.memory.buffer).getInt32(c.player_shooting_snap(fixture,i),true),m.i32(owner+0x734+i*0xe4),label+' snap '+i);
    const count=c.player_shooting_event_count(fixture);assert.deepEqual(Array.from(new Uint32Array(c.memory.buffer,c.player_shooting_events(fixture),count*8)),events,label+' requests');frames++;checks+=24;
   }c.player_shooting_delete(fixture);
  }
  report('player-shooting-timers',{passed:true,frames,checks,originalFunctions:['0x44a8d0','0x4461b0','0x411210','0x44a0f0'],scope:'Normal firing, continuous timer, Youmu charging and charged release in every animal selection, focus changes, roaming-active flags, KO cleanup, fractional rate and exact ordered shot/audio/indicator requests. Shot generation and audio/ANM boundaries are observed, not substituted as game behavior; real projectile execution remains separate.'});
 }finally{m.close();}
});
