import test from 'node:test';import assert from 'node:assert/strict';import{core,oracle,memory,string,report}from './helpers.mjs';
test('TH15 original pause frame gates on demo/retry/focus/active callback and advances both clocks at the restored rate',async()=>{
 const c=await core(),m=await oracle(),a=c.anm_manager_create(),f=c.pause_frame_create(a),native=m.allocate(0x400),game=m.allocate(0x400),gui=m.allocate(0x400),notice=m.allocate(0x610),dialogue=m.allocate(0x100),callback=m.allocate(0x40);let events=[],cases=0,checks=0;
 const sp=()=>m.reg('ESP'),push=(...values)=>{events.push(...values);return 0;};
 m.u32(0x4e9bb4,native);m.u32(0x4e9a94,game);m.u32(0x4e9a8c,gui);m.view(gui,0x400).fill(0);m.u32(gui+0x2c8,101);m.u32(gui+0x1b8,dialogue);m.u32(gui+0x11c,88);m.f32(0x51bc04,2);m.u32(0x4ca620,0x4e73e8);
 m.replace(0x43d2c0,'fixture wall-clock play-time accounting',()=>push(1));
 m.replace(0x488510,'fixture animation owner lookup',()=>{if(m.u32(sp()+4)===88){push(9);return notice;}return 0;},1);
 m.replace(0x423020,'fixture menu/snapshot visual creation',()=>{const out=m.u32(sp()+4),script=m.i32(sp()+8);m.u32(out,script===52?0x2222:0x1111);if(script!==52)push(2,script);return out;},3);
 m.replace(0x488620,'fixture delayed menu interrupt',()=>push(m.i32(sp()+8)),2);
 m.replace(0x43e440,'fixture audio slot reset',()=>push(3));m.replace(0x476360,'fixture menu sound',()=>push(4,m.i32(sp()+4)),2);
 m.replace(0x476f10,'fixture audio pause command',()=>{assert.equal(m.i32(sp()+4),6);assert.equal(m.i32(sp()+8),0);assert.equal(m.string(m.u32(sp()+12)),'Pause');return push(5);},3);
 m.replace(0x476570,'fixture audio queue completion',()=>push(6));
 m.replace(0x450bf0,'fixture full-frame capture',()=>{m.u32(native+0x1e8,0x2222);return push(7,0);});
 m.replace(0x452c20,'fixture result playfield capture',()=>push(7,1),5);
 m.replace(0x439640,'fixture active dialogue freeze',()=>push(8));
 m.replace(0x44d360,'fixture game-over track preparation',()=>{assert.equal(m.i32(sp()+4),0);assert.equal(m.string(m.u32(sp()+8)),'th128_08');return push(11);},2);
 m.replace(0x44d3e0,'fixture game-over track playback',()=>{assert.equal(m.i32(sp()+4),0);assert.equal(m.i32(sp()+8),0);return push(12);},2);

 m.u32(game+4,callback);
 try{for(const screen of[0,1,2,3,4])for(const mode of[0,0x40,0x100,0x120])for(const flags of[0,0x10000,0x4000,0x80])for(const input of[0,0x100,0x10000,0x80001])for(const focus of[false,true])for(const enabled of[false,true])for(const age of[29,30]){
  const rate=[.125,.75,1,1.125][cases%4];c.pause_frame_configure(f,mode,flags,rate,screen,age);m.view(native,0x400).fill(0);m.write(native,Buffer.from(memory(c,c.pause_activation_state(f),0x3f4)));m.i32(native+0x1f4,5);m.u32(game+0x90,flags);m.u32(game+0xb4,0);m.u32(callback+4,enabled?2:0);m.i32(game+0x10,age);m.u32(0x4e7794,mode);m.u32(0x4e6d1c,input);m.u32(0x4e6d18,0);m.u32(0x4e7f08,focus?0x10:0);m.f32(0x4e73e8,rate);m.i32(0x51bc60,7);m.view(notice,0x610).fill(0);events=[];
  m.call(0x450290,{ecx:native,limit:100000});assert.equal(c.pause_frame_update(f,input,0,focus,enabled),1,string(c,c.pause_frame_error(f)));const label=[screen,mode,flags,input,focus,enabled,age].join('/'),actual=Buffer.from(memory(c,c.pause_activation_state(f),0x3f4));assert.deepEqual(actual,Buffer.from(m.bytes(native,0x3f4)),label+' complete projected menu/timers');const p=c.pause_activation_field(f,0),view=new DataView(c.memory.buffer);assert.equal(view.getUint32(p+96,true),m.u32(game+0x90),label+' actual pause flag');assert.equal(view.getFloat32(p+92,true),m.f32(0x4e73e8),label+' restored animation/timer rate');assert.equal(view.getInt32(c.pause_activation_field(f,2),true),m.i32(0x51bc60),label+' frame skip');assert.deepEqual(Array.from(new Int32Array(c.memory.buffer,c.pause_activation_events(f),c.pause_activation_event_count(f))),events,label+' opening service order');cases++;checks+=5;
 }
 report('pause-frame',{passed:true,cases,checks,originalFunctions:['0x450290','0x450e00','0x451740'],scope:'Original callback and real original activation execute against named frame controller; inactive/three active/unknown screen, demo, checkpoint retry, ending flag, ESC/focus/other keys, enabled session callback and age-29/30 boundaries; full projected menu, both fractional timers, flags/rate/skip and opening service order. Active menus use the original no-action phase 5 because their complete navigation/ANM trees have separate native proof. Audio/capture/clock/visual services remain fixtures.'});
 }finally{c.pause_frame_delete(f);c.anm_manager_delete(a);m.close();}
});
