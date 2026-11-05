import test from 'node:test';import assert from 'node:assert/strict';import{core,oracle,memory,string,report}from './helpers.mjs';
test('TH15 pause, retry-pause, game-over and result entrances match original state, audio/capture order and replay destinations',async()=>{
 const c=await core(),m=await oracle(),f=c.pause_activation_create(),native=m.allocate(0x400),game=m.allocate(0x400),gui=m.allocate(0x400),notice=m.allocate(0x610),dialogue=m.allocate(0x100),position=m.allocate(8);let events=[],cases=0,checks=0;
 const sp=()=>m.reg('ESP'),push=(...values)=>{events.push(...values);return 0;};
 m.u32(0x4e9bb4,native);m.u32(0x4e9a94,game);m.u32(0x4e9a8c,gui);m.view(gui,0x400).fill(0);m.u32(gui+0x2c8,101);m.u32(gui+0x1b8,dialogue);m.u32(gui+0x11c,88);m.f32(0x51bc04,2);m.u32(0x4ca620,0x4e73e8);
 m.replace(0x43d2c0,'fixture wall-clock play-time accounting',()=>push(1));
 m.replace(0x488510,'fixture animation owner lookup',()=>{if(m.u32(sp()+4)===88){push(9);return notice;}return 0;},1);
 m.replace(0x423020,'fixture menu/snapshot visual creation',()=>{const out=m.u32(sp()+4),script=m.i32(sp()+8);m.u32(out,script===52?0x2222:0x1111);if(script!==52)push(2,script);return out;},3);
 m.replace(0x488620,'fixture immediate menu interrupt',()=>push(m.i32(sp()+8)),2);
 m.replace(0x43e440,'fixture audio slot reset',()=>push(3));m.replace(0x476360,'fixture menu sound',()=>push(4,m.i32(sp()+4)),2);
 m.replace(0x476f10,'fixture audio pause command',()=>{assert.equal(m.i32(sp()+4),6);assert.equal(m.i32(sp()+8),0);assert.equal(m.string(m.u32(sp()+12)),'Pause');return push(5);},3);
 m.replace(0x476570,'fixture audio queue completion',()=>push(6));
 m.replace(0x450bf0,'fixture full-frame capture',()=>{m.u32(native+0x1e8,0x2222);return push(7,0);});
 m.replace(0x452c20,'fixture result playfield capture',()=>push(7,1),5);
 m.replace(0x439640,'fixture active dialogue freeze',()=>push(8));
 m.replace(0x44d360,'fixture game-over track preparation',()=>{assert.equal(m.i32(sp()+4),0);assert.equal(m.string(m.u32(sp()+8)),'th128_08');return push(11);},2);
 m.replace(0x44d3e0,'fixture game-over track playback',()=>{assert.equal(m.i32(sp()+4),0);assert.equal(m.i32(sp()+8),0);return push(12);},2);
 m.view(position,8).set(Buffer.from(new Float64Array([123.125]).buffer));const getPosition=Buffer.from([0xf2,0x0f,0x10,0x05,0,0,0,0,0xc3]);getPosition.writeUInt32LE(position,4);m.write(0x452ca0,getPosition);
 const owned=[[0xc,20],[0x20,20],[0x108,4],[0x1e4,28],[0x208,4],[0x2e0,12],[0x3ec,8]];
 try{for(const entrance of[0,1,2,3])for(const flags of[0,0x10,0x20,0x30,0x100,0x200,0x120,0x130,0x300])for(const chapter of[0,42,43,81])for(const replay of[0,1])for(const selection of[false,true]){
  const rate=[.125,.75,1,1.125][chapter%4],screen=cases%4,stateFlags=cases%16;c.pause_activation_configure(f,flags,chapter,replay,rate,screen,stateFlags);m.view(native,0x400).fill(0);m.write(native,Buffer.from(memory(c,c.pause_activation_state(f),0x3f4)));m.view(game,0x400).fill(0);m.u32(game+0x90,0x4080);m.u32(game+0xb4,replay);m.u32(0x4e7794,flags);m.i32(0x4e73f8,chapter);m.f32(0x4e73e8,rate);m.i32(0x51bc60,7);m.u32(0x4e7f08,selection?0x2000:0);m.i32(0x4e7ecc,-1);m.view(notice,0x610).fill(0);m.u32(notice+0x18,2);m.write(0x51dfa4,Buffer.from('native-current.wav'+String.fromCharCode(0)));events=[];
  m.call([0x450e00,0x450fa0,0x451100,0x4512a0][entrance],{ecx:native,limit:50000});if((entrance===2||entrance===3)&&replay)events.push(13,m.i32(0x4e7ecc));
  assert.equal(c.pause_activation_open(f,entrance,selection),1,string(c,c.pause_activation_error(f)));const label='entrance '+entrance+' flags '+flags+' chapter '+chapter+' replay '+replay+' selection '+selection,bytes=Buffer.from(memory(c,c.pause_activation_state(f),0x3f4));
  for(const[at,n]of owned){assert.deepEqual(bytes.subarray(at,at+n),Buffer.from(m.bytes(native+at,n)),label+' state '+at.toString(16));checks++;}
  assert.equal(string(c,c.pause_activation_state(f)+0x2ec),m.string(native+0x2ec),label+' saved wave');const dv=new DataView(c.memory.buffer),p=c.pause_activation_field(f,0);assert.equal(dv.getUint32(p+96,true),m.u32(game+0x90),label+' scene flags');assert.equal(dv.getFloat32(p+92,true),m.f32(0x4e73e8),label+' rate');assert.equal(dv.getInt32(c.pause_activation_field(f,2),true),m.i32(0x51bc60),label+' frame skip');assert.deepEqual(Array.from(new Int32Array(c.memory.buffer,c.pause_activation_events(f),c.pause_activation_event_count(f))),events,label+' exact service order');checks+=5;cases++;
 }
 report('pause-activation',{passed:true,cases,checks,originalFunctions:['0x450e00','0x450fa0','0x451100','0x4512a0','0x44fc60','0x44fd10'],scope:'Unmodified original entrance/select/phase logic, retained timers and menu fields, scene pause flag/rate/frame skip, four distinct capture/music/audio orders, exact original game-over track and saved music position, spell/Pointdevice/replay conditionals. GPU capture, sound devices, elapsed wall time and menu visuals are explicit fixture services. Full paused-menu update/result persistence/application remain separate.'});
 }finally{c.pause_activation_delete(f);m.close();}
});
