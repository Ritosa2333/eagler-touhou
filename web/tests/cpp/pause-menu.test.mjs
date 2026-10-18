import test from 'node:test';import assert from 'node:assert/strict';
import{core,oracle,memory,report}from'./helpers.mjs';import{animationOracle}from'./anm-oracle.mjs';import{normalizeAnimation,firstDifference}from'./shot-oracle.mjs';
test('TH11 pause overlay transitions and original animation families',async()=>{
 const c=await core(),m=await oracle(),o=animationOracle(m),files=[o.load('text.anm'),o.load('front.anm',true)],resources=[];
 const read=p=>new DataView(c.memory.buffer).getUint32(p,true),put=(p,v)=>new DataView(c.memory.buffer).setUint32(p,v,true);
 for(let i=0;i<2;++i){const r=c.anm_create(),b=c.allocate(files[i].source.length);memory(c,b,files[i].source.length).set(files[i].source);assert.equal(c.anm_open(r,b,files[i].source.length),1);c.release(b);resources.push(r);m.u32(files[i].original,i?5:0);}
 const menu=m.allocate(0x300),battle=m.allocate(0x100),hud=m.allocate(0x4460),score=m.allocate(0x30000),recording=m.allocate(0x2dc),header=m.allocate(0x70),heap=m.heap;
 m.u32(0x4a8e88,battle);m.u32(0x4a8d84,hud);m.u32(hud+0x444c,files[1].original);m.u32(0x4c3808,files[0].original);m.u32(0x4a5728,1);
 let sounds=[],capture=0,action=0,frames=0,checks=0;
 m.replace(0x44a1e0,'pause sound sink',()=>{sounds.push(m.reg('ESI'));return 0;});m.replace(0x44a9c0,'pause music transport',()=>0,2);
 m.replace(0x42f480,'GPU screenshot request boundary',()=>{capture++;return 0;},1);
 m.replace(0x42c880,'resume gameplay boundary',()=>{action=1;return 0;});m.replace(0x40dea0,'return title boundary',()=>{action=2;return 0;});
 m.u32(0x4a8ebc,score);m.u32(0x4a8eb8,recording);m.u32(recording+0x18,header);m.u32(0x4a5758,0);
 const stack=n=>m.u32(m.reg('ESP')+n*4);m.replace(0x46025c,'replay slot filename',()=>{const value=`th11_${String(stack(3)).padStart(2,'0')}.rpy`;m.write(stack(1),Buffer.from(value+'\0'));return value.length;});
 m.replace(0x435ef0,'empty replay catalog',()=>0,1);m.replace(0x435fc0,'empty replay disposal',()=>0);
 m.replace(0x4608b7,'recording timestamp',()=>{m.u32(stack(1),1600000000);m.u32(stack(1)+4,0);m.reg('EDX',0);return 1600000000;});
 let saves=0;m.replace(0x436420,'replay save boundary',()=>{++saves;return 0;},1);
 try{for(const replay of[false,true])for(const choice of[0,1,2,3,256,0x10000,0x200000]){
  if(replay&&choice===2)continue;
  m.heap=heap;o.reset();m.view(menu,0x300).fill(0);m.view(battle,0x100).fill(0);m.u32(battle+0x74,+replay);m.u32(0x4c37d8,0);sounds=[];capture=0;
  const a=c.anm_manager_create(),s=c.score_file_create(),p=c.pause_create(a,...resources,s);for(let i=0;i<2;++i){put(c.anm_env_rng(a,i),12345);put(c.anm_env_rng(a,i)+4,0);}
  const name=Buffer.from('        \0');memory(c,c.score_file_data(s,7)+12,9).set(name);m.write(score+0x2dde0,name);
  m.reg('EBX',menu);m.call(0x42c620);assert.equal(c.pause_begin(p,+replay),1);assert.equal(capture,1);assert.equal(c.pause_value(p,2),1);
  const compare=()=>{const label=`replay${replay} choice${choice} frame${frames}`;
   for(const [kind,offset,size]of[[0,4,4],[1,0x10,12],[2,0x24,12],[3,0x38,0xd8],[4,0x1e8,8],[6,0x1fc,4]])assert.deepEqual(Buffer.from(memory(c,c.pause_data(p,kind),size)),Buffer.from(m.bytes(menu+offset,size)),label+' state'+kind);
   assert.deepEqual(Array.from({length:c.pause_value(p,0)},(_,i)=>read(c.pause_data(p,5)+i*4)),sounds,label+' sounds');
   if(choice===2){for(const[k,o,n]of[[0,0x110,0xd8],[1,0x2cc,9],[2,0x1f0,4]])assert.deepEqual(Buffer.from(memory(c,c.pause_name(p,k),n)),Buffer.from(m.bytes(menu+o,n)),label+' name');assert.deepEqual(Buffer.from(memory(c,c.score_file_data(s,7)+12,9)),Buffer.from(m.bytes(score+0x2dde0,9)),label+' saved name');}
   o.update(true);assert.equal(c.anm_manager_update(a,1),1);const states=o.states();assert.equal(c.anm_manager_count(a),states.length,label+' animations');
   for(const vm of states){const cp=c.anm_manager_find(a,vm.id);assert.ok(cp);assert.equal(firstDifference(normalizeAnimation(memory(c,cp,0x434),read,read(cp+0x3a4)),normalizeAnimation(vm.bytes,p=>m.u32(p),m.u32(vm.p+0x3a4))),'',label+' VM'+vm.id);++checks;}
  };
  const tick=(pressed=0,repeat=0)=>{sounds=[];action=0;m.u32(0x4c92b4,pressed);m.u32(0x4c92b0,repeat);m.call(0x42c8d0,{args:[menu]});for(const offset of[0x10,0x24]){const n=m.i32(menu+offset+4);m.i32(menu+offset,n);m.i32(menu+offset+4,n+1);m.f32(menu+offset+8,m.f32(menu+offset+8)+1);}assert.equal(c.pause_update(p,pressed,repeat),1);compare();++frames;};
  try{compare();for(let i=0;i<12;++i)tick();
   if(choice>=256){tick(choice);for(let i=0;i<12;++i)tick();}
   else{for(let i=0;i<4&&read(c.pause_data(p,3))!==choice;++i)tick(32);tick(1);
    if(choice&&!replay){for(let i=0;i<30;++i)tick();tick(1);for(let i=0;i<20;++i)tick();assert.equal(read(c.pause_data(p,0)),3);tick(1);for(let i=0;i<30;++i)tick();tick(16);tick(1);for(let i=0;i<20;++i)tick();}
    if(choice===2){for(let i=0;i<11;++i)tick();tick(32);tick(1);for(let i=0;i<10;++i)tick();tick(2);for(let i=0;i<10;++i)tick();tick(1);for(let i=0;i<10;++i)tick();tick(1);tick(128);tick(1);for(let i=0;i<6;++i)tick(1);tick(2);tick(64);tick(1);tick(128);tick(1);assert.equal(saves,1);for(let i=0;i<11;++i)tick();tick(2);}
    for(let i=0;i<12;++i)tick();
   }
   assert.equal(read(c.pause_data(p,0)),0);const expected=choice===0||choice===256?1:choice===1||choice===2||choice===0x10000?2:3;assert.equal(c.pause_value(p,1),expected);
  }finally{c.pause_delete(p);c.score_file_delete(s);c.anm_manager_delete(a);}
 }report('pause-menu',{passed:true,frames,checks,scope:'Native 42c620 / 42c8d0 pause entry, resume, title/retry confirmations, keyboard shortcuts, replay-disabled save, timers, complete cursor/history and overlay ANM state. GPU capture, save-replay subflow and game-over variants are separate.'});
 }finally{for(const r of resources)c.anm_delete(r);m.close();}
});
