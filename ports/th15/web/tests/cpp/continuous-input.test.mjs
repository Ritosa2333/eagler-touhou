import test from 'node:test';import assert from 'node:assert/strict';import{readFileSync}from'node:fs';import{resolve}from'node:path';import{core,oracle,memory,string,report,root}from'./helpers.mjs';
test('TH15 physical virtual-key mapping matches every original key and combined keyboard sample',async()=>{
 const c=await core(),m=await oracle(),p=c.allocate(256);let samples=0,seed=51521;const random=()=>{seed^=seed<<13;seed^=seed>>>17;seed^=seed<<5;return seed>>>0;};let bytes=Buffer.alloc(256);
 m.replace(0x401b20,'gamepad boundary; return original keyboard mask',()=>m.u32(m.reg('ESP')+4),1);m.replace(0x4077c0,'raw keyboard edge calculation boundary',()=>0);m.u32(0x519bc0,1);m.u32(0x4e7f08,0);
 for(const entry of m.importMap.values())if(entry.name==='GetKeyboardState'){entry.argc=1;entry.handler=()=>{m.write(m.u32(m.reg('ESP')+4),bytes);return 1;};}
 try{for(let n=0;n<1024;n++){bytes=Buffer.alloc(256);if(n<256)bytes[n]=128;else if(n<512)bytes[n-256]=1;else for(let i=0;i<256;i++)bytes[i]=random()&255;memory(c,p,256).set(bytes);m.call(0x401f50);assert.equal(c.game_keyboard_keys(p),m.u32(0x4e6d10),'sample '+n);samples++;}report('game-keyboard',{passed:true,samples,originalFunctions:['0x401f50'],scope:'Every VK down and toggle-only byte plus 512 combinations, native GetKeyboardState branch before gamepad merging. Window polling, SDL and joystick transport are separate.'});}finally{c.release(p);m.close();}
});
test('TH15 continuous targets replay all four SHT movement speeds, focus, fixed bounds, rate changes and stage boundaries',async()=>{
 const c=await core();let frames=0,assertions=0;const load=(manager,name,id)=>{const bytes=readFileSync(resolve(root,'reference/assets',name)),p=c.allocate(bytes.length);memory(c,p,bytes.length).set(bytes);assert.equal(c.anm_manager_load(manager,id,p,bytes.length),1,string(c,c.anm_manager_error(manager)));c.release(p);};
 const fields=[0,1,2,3,4,5,7,8],sizes=[8,12,8,8,12,12,4,4];
 for(let character=0;character<4;character++){
  const manager=c.anm_manager_create(),record=c.recording_create(),replay=c.replay_create(),header=c.allocate(0x238),name=c.allocate(9),shot=c.allocate(0xe0);let live,play;
  try{
   load(manager,'ascii.anm',2);assert.equal(c.anm_manager_fallback(manager,2),1);load(manager,'effect.anm',8);load(manager,'pl0'+character+'.anm',9);
   memory(c,shot,0xe0).set(readFileSync(resolve(root,'reference/assets','pl0'+character+'.sht')).subarray(0,0xe0));live=c.player_motion_create(manager,9,8);play=c.player_motion_create(manager,9,8);for(const owner of[live,play]){c.player_motion_configure(owner,shot);const p=c.player_motion_field(owner,0);new DataView(c.memory.buffer).setInt32(p,0,true);new DataView(c.memory.buffer).setInt32(p+4,51200,true);new DataView(c.memory.buffer).setInt32(c.player_motion_field(owner,9),6,true);}
   const meta=c.recording_metadata(record);memory(c,meta,0xa4).fill(0);new DataView(c.memory.buffer).setUint32(meta+0x8c,character,true);new DataView(c.memory.buffer).setUint32(meta+0x94,1,true);memory(c,name,9).set(Buffer.from('TOUCH'+String.fromCharCode(0)));const sequences=[];
   for(let stage=1;stage<=3;stage++){
    memory(c,header,0x238).fill(0);new DataView(c.memory.buffer).setUint16(header,stage,true);assert.equal(c.recording_begin(record,header,0x238),1);c.recording_activate(record);const sequence=[];let previous=0;
    for(let n=0;n<450;n++){
     const held=(n%4?1:0)|(n%29>13?8:0)|(n%53===0?0x90:0),pressed=held&~previous,released=previous&~held;previous=held;
     const mode=n%43<5?0:n%71<2?2:1,x=Math.fround(Math.sin((n+stage*17)*.071)*175),y=Math.fround(240+Math.cos((n+stage*31)*.037)*175),rate=[1,.5,.25,1.25][(n>>5)%4];
     const old=Buffer.from(memory(c,c.player_motion_field(live,0),8));c.player_motion_touch(live,mode,x,y);assert.equal(c.player_motion_step(live,held,1,rate),1,string(c,c.player_motion_error(live)));assert.equal(c.recording_touch_tick(record,held,pressed,released,60,0,mode,x,y),1,string(c,c.recording_error(record)));
     const state=fields.map((field,i)=>Buffer.from(memory(c,c.player_motion_field(live,field),sizes[i]))),fixed=state[0];assert.ok(fixed.readInt32LE()>=-23552&&fixed.readInt32LE()<=23552);assert.ok(fixed.readInt32LE(4)>=4096&&fixed.readInt32LE(4)<=55296);
     if(mode===1){const speed=new DataView(c.memory.buffer).getInt32(c.player_motion_field(live,6)+((held&8)?4:0),true),dx=fixed.readInt32LE()-old.readInt32LE(),dy=fixed.readInt32LE(4)-old.readInt32LE(4);assert.ok(Math.hypot(dx,dy)<=speed*rate+2,'speed bound');}
     sequence.push({state,held,pressed,released,rate,mode,x,y});frames++;assertions+=3;
    }sequences.push(sequence);
   }
   assert.equal(c.recording_uses_touch(record),1);c.recording_finish(record,1700000000n,3,0);assert.equal(c.recording_write(record,name,1),1,string(c,c.recording_error(record)));const bytes=Buffer.from(memory(c,c.recording_data(record),c.recording_size(record)));assert.equal(c.replay_open(replay,c.recording_data(record),bytes.length),1,string(c,c.replay_error(replay)));assert.equal(c.replay_uses_touch(replay),1);
   for(let stage=1;stage<=3;stage++){
    assert.equal(c.replay_select(replay,stage),1);for(const [n,expected]of sequences[stage-1].entries()){
     const input=c.replay_tick(replay,1),view=new DataView(c.memory.buffer),held=view.getUint16(input,true),mode=view.getInt32(input+8,true),x=view.getFloat32(input+12,true),y=view.getFloat32(input+16,true);assert.equal(held,expected.held);assert.equal(mode,expected.mode);if(mode){assert.equal(x,expected.x);assert.equal(y,expected.y);}c.player_motion_touch(play,mode,x,y);assert.equal(c.player_motion_step(play,held,1,expected.rate),1,string(c,c.player_motion_error(play)));fields.forEach((field,i)=>assert.deepEqual(Buffer.from(memory(c,c.player_motion_field(play,field),sizes[i])),expected.state[i],'character '+character+' stage '+stage+' frame '+n+' field '+field));assertions+=10;
    }
   }
   const packed=bytes.readUInt32LE(0x1c);let touchStart=0x24+packed;while(bytes.readUInt32LE(touchStart+8)!==0x15)touchStart+=bytes.readUInt32LE(touchStart+4);const entries=bytes.readUInt32LE(touchStart+20);assert.ok(entries>1000);
   const malformed=[b=>b.writeUInt32LE(0xffffffff,touchStart+20),b=>b.writeUInt16LE(4,touchStart+34),b=>b.writeUInt32LE(450,touchStart+36),b=>b.writeFloatLE(NaN,touchStart+40),b=>b.writeFloatLE(1e20,touchStart+44),b=>b.writeUInt32LE(b.readUInt32LE(touchStart+36),touchStart+52),b=>b.writeUInt16LE(7,touchStart+32)];for(const mutate of malformed){const b=Buffer.from(bytes);mutate(b);const p=c.allocate(b.length);memory(c,p,b.length).set(b);assert.equal(c.replay_open(replay,p,b.length),0);c.release(p);assertions++;}
  }finally{if(live)c.player_motion_delete(live);if(play)c.player_motion_delete(play);c.release(header);c.release(name);c.release(shot);c.replay_delete(replay);c.recording_delete(record);c.anm_manager_delete(manager);}
 }
 report('continuous-input',{passed:true,characters:4,stages:3,frames,assertions,scope:'Four actual SHT speed/ANM movement owners execute 5,400 continuous/digital ticks; USER extension reopen restores exact fixed coordinates, focus, velocity, pose-direction data through stage boundaries. Bounds, speed limit and malformed streams checked. Independent original keyboard movement remains verified separately; this does not prove full-world or mobile-browser replay parity.'});
});
