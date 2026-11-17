import test from 'node:test';import assert from 'node:assert/strict';import{readFileSync}from'node:fs';import{resolve}from'node:path';import{core,memory,string,root,report}from'./helpers.mjs';
test('TH17 continuous movement survives exported replay, pause, stage changes and repeated selection',async()=>{
 const c=await core(),a=c.anm_manager_create();let frames=0,rejected=0;const dv=()=>new DataView(c.memory.buffer);
 const load=(name,id)=>{const b=readFileSync(resolve(root,'reference/assets',name)),p=c.allocate(b.length);memory(c,p,b.length).set(b);assert.equal(c.anm_manager_load(a,id,p,b.length),1,string(c,c.anm_manager_error(a)));c.deallocate(p);};
 load('ascii.anm',0);c.anm_manager_fallback(a,0);load('effect.anm',8);for(let char=0;char<3;char++)load('pl0'+char+'.anm',9+char);
 for(let char=0;char<3;char++)for(let animal=0;animal<3;animal++){
  const r=c.recording_create(),h=c.allocate(0x158),name=c.allocate(9),left=c.player_motion_create(a,9+char,8),right=c.player_motion_create(a,9+char,8),sht=c.allocate(0xe0),stream=[],meta=c.recording_metadata(r);let replay=0,p=0;
  try{
   memory(c,sht,0xe0).set(readFileSync(resolve(root,'reference/assets','pl0'+char+String.fromCharCode(97+animal)+'.sht')).subarray(0,0xe0));c.player_motion_configure(left,sht);c.player_motion_configure(right,sht);dv().setUint32(meta+0x88,char,true);dv().setUint32(meta+0x8c,animal,true);dv().setUint32(meta+0x90,animal,true);
   for(const stage of[1,3,7]){memory(c,h,0x158).fill(0);dv().setUint16(h,stage,true);assert.equal(c.recording_begin(r,h,0x158),1);c.recording_activate(r);const inputs=[];let previous=0;
    for(let frame=0;frame<180;frame++){const mode=frame%37<5?0:frame%23<3?2:1,x=Math.fround(Math.sin(frame*.091+stage)*160),y=Math.fround(220+Math.cos(frame*.071+animal)*140),held=(frame%17>5?8:0)|(frame%11?1:0),pressed=(held^previous)&held,released=(held^previous)&~held;previous=held;
     // Paused frames do not dispatch the original input callback.
     const clock=c.recording_clock(r);assert.equal(c.recording_tick(r,held,pressed,released,60,0),1);assert.equal(c.recording_clock(r),clock+1);
     // Replace the just-recorded digital frame with touch in an independent
     // recording below; this stream is the expected physical request.
     inputs.push({held,pressed,released,mode,x,y});
    }stream.push({stage,inputs});
   }
   // Recreate the capture with its real per-tick continuous targets.
   c.recording_delete(r);const capture=c.recording_create();
   try{const description=c.recording_metadata(capture);dv().setUint32(description+0x88,char,true);dv().setUint32(description+0x8c,animal,true);dv().setUint32(description+0x90,animal,true);
    for(const {stage,inputs}of stream){memory(c,h,0x158).fill(0);dv().setUint16(h,stage,true);assert.equal(c.recording_begin(capture,h,0x158),1);c.recording_activate(capture);for(const t of inputs)assert.equal(c.recording_touch_tick(capture,t.held,t.pressed,t.released,60,0,t.mode,t.x,t.y),1,string(c,c.recording_error(capture)));}
    memory(c,name,9).set(Buffer.from('TOUCH   '+String.fromCharCode(0)));assert.equal(c.recording_uses_touch(capture),1);assert.equal(c.recording_write(capture,name,1),1,string(c,c.recording_error(capture)));const bytes=Buffer.from(memory(c,c.recording_data(capture),c.recording_size(capture)));p=c.allocate(bytes.length);memory(c,p,bytes.length).set(bytes);replay=c.replay_create();assert.equal(c.replay_open(replay,p,bytes.length),1,string(c,c.replay_error(replay)));
    for(const {stage,inputs}of stream){assert.equal(c.replay_select(replay,stage),1);for(let frame=0;frame<inputs.length;frame++){
      const t=inputs[frame],at=c.replay_tick(replay,1),mode=dv().getInt32(at+8,true),x=dv().getFloat32(at+12,true),y=dv().getFloat32(at+16,true);assert.equal(mode,t.mode);if(mode){assert.equal(x,t.x);assert.equal(y,t.y);}assert.equal(dv().getUint16(at,true),t.held);
      if(frame%19===0){const saved=Buffer.from(memory(c,at,20)),cursor=c.replay_frame(replay);assert.deepEqual(Buffer.from(memory(c,c.replay_tick(replay,0),20)),saved,'pause retains current input');assert.equal(c.replay_frame(replay),cursor);}
      for(const motion of[left,right])dv().setInt32(c.player_motion_field(motion,9),frame+4,true);c.player_motion_touch(left,t.mode,t.x,t.y);c.player_motion_touch(right,mode,x,y);const rate=[1,.5,1.25,.25][frame%4];assert.equal(c.player_motion_step(left,t.held,1,rate),1,string(c,c.player_motion_error(left)));assert.equal(c.player_motion_step(right,dv().getUint16(at,true),1,rate),1,string(c,c.player_motion_error(right)));
      for(const[field,size]of[[0,8],[1,12],[2,8],[3,8],[4,12],[5,12],[7,4],[8,4],[18,264]])assert.deepEqual(Buffer.from(memory(c,c.player_motion_field(left,field),size)),Buffer.from(memory(c,c.player_motion_field(right,field),size)),'loadout '+char+'/'+animal+' stage '+stage+' frame '+frame+' field '+field);frames++;
     }
     const final=c.replay_tick(replay,1);if(stage===7)assert.equal(memory(c,final+7,1)[0],1,'last stage terminates');assert.equal(c.replay_select(replay,stage),1);assert.equal(dv().getUint16(c.replay_tick(replay,1),true),inputs[0].held,'selection resets touch cursor');
    }
    const touch=bytes.indexOf(Buffer.from([85,83,69,82]),0x24+bytes.readUInt32LE(0x1c));let offset=touch;while(bytes.readUInt32LE(offset+8)!==0x17)offset+=bytes.readUInt32LE(offset+4);
    for(const mutate of[b=>b.writeUInt32LE(2,offset+16),b=>b.writeUInt16LE(8,offset+32),b=>b.writeUInt32LE(0xffffffff,offset+36),b=>b.writeFloatLE(NaN,offset+40),b=>b.writeUInt16LE(3,offset+34),b=>b.writeUInt32LE(0xffffffff,offset+20)]){const bad=Buffer.from(bytes);mutate(bad);memory(c,p,bad.length).set(bad);assert.equal(c.replay_open(replay,p,bad.length),0,'malformed touch is rejected');rejected++;}memory(c,p,bytes.length).set(bytes);assert.equal(c.replay_open(replay,p,bytes.length),1);
   }finally{c.recording_delete(capture);}
  }finally{if(replay)c.replay_delete(replay);if(p)c.deallocate(p);c.deallocate(h);c.deallocate(name);c.deallocate(sht);c.player_motion_delete(left);c.player_motion_delete(right);}
 }
 c.anm_manager_delete(a);report('replay-touch',{passed:true,loadouts:9,stages:[1,3,7],frames,rejectedMalformed:rejected,scope:'Live continuous requests versus movement recovered from an exported/reopened native replay plus USER extension; all nine SHT loadouts, exact fixed movement/history, held/focus, pause, stage resets, repeated selection and rejected malformed entries. Touch mode is an explicit browser extension; original keyboard replays retain original movement.'});
});
