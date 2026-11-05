import test from 'node:test';import assert from 'node:assert/strict';
import{core,oracle,memory,report}from'./helpers.mjs';import{animationOracle}from'./anm-oracle.mjs';import{normalizeAnimation,firstDifference}from'./shot-oracle.mjs';
test('TH11 game-over and practice results match native menu and score state',async()=>{
 const c=await core(),m=await oracle(),o=animationOracle(m),files=[o.load('text.anm'),o.load('front.anm',true)],resources=[];
 const read=p=>new DataView(c.memory.buffer).getUint32(p,true),put=(p,v)=>new DataView(c.memory.buffer).setUint32(p,v,true);
 for(let i=0;i<2;++i){const r=c.anm_create(),b=c.allocate(files[i].source.length);memory(c,b,files[i].source.length).set(files[i].source);assert.equal(c.anm_open(r,b,files[i].source.length),1);c.release(b);resources.push(r);m.u32(files[i].original,i?5:0);}
 const menu=m.allocate(0x300),battle=m.allocate(0x100),hud=m.allocate(0x4460),score=m.allocate(0x30000),recording=m.allocate(0x2dc),header=m.allocate(0x70),fps=m.allocate(0x40),sup=m.allocate(0x18500),localDate=m.allocate(36),dummy=m.allocate(0x440),heap=m.heap;
 m.u32(0x4a8d58,sup);m.u32(0x4c3444,dummy);
 m.u32(0x4a8e88,battle);m.u32(0x4a8d84,hud);m.u32(hud+0x444c,files[1].original);m.u32(0x4c3808,files[0].original);m.u32(0x4a8eb0,menu);
 m.u32(0x4a8ebc,score);m.u32(0x4a8eb8,recording);m.u32(recording+0x18,header);m.u32(0x4a8d80,fps);new DataView(m.view(fps,0x40).buffer,m.view(fps,0x40).byteOffset,0x40).setFloat64(0x24,1,true);new DataView(m.view(fps,0x40).buffer,m.view(fps,0x40).byteOffset,0x40).setFloat64(0x2c,1,true);
 let sounds=[],frames=0,checks=0,saves=0,action=0;
 const stack=n=>m.u32(m.reg('ESP')+n*4);
 const string=q=>{const b=Buffer.from(m.bytes(q,256)),n=b.indexOf(0);return b.subarray(0,n).toString('latin1');},cpString=q=>{const b=Buffer.from(memory(c,q,256));return b.subarray(0,b.indexOf(0)).toString('latin1');};
 m.replace(0x460be4,'calendar boundary',()=>{const d=new Date(Number(Buffer.from(m.bytes(stack(1),8)).readBigInt64LE())*1000);[d.getUTCSeconds(),d.getUTCMinutes(),d.getUTCHours(),d.getUTCDate(),d.getUTCMonth(),d.getUTCFullYear()-1900,d.getUTCDay(),0,0].forEach((v,i)=>m.i32(localDate+i*4,v));return localDate;});
 m.replace(0x45fdd2,'ASCII formatter',()=>{let q=stack(3);const value=string(stack(2)).replace(/%%|%([ 0]?)(\d*)(?:\.(\d+))?l?([dsc])/g,(all,pad,width,precision,type)=>{if(all==='%%')return'%';const v=m.i32(q);q+=4;let s=type==='s'?string(v):type==='c'?String.fromCharCode(v&255):(v<0?'-':'')+String(Math.abs(v)).padStart(Number(precision)||0,'0');return s.padStart(Number(width)||0,pad==='0'?'0':' ');});m.write(stack(1),Buffer.from(value+'\0','latin1'));return value.length;});
 m.replace(0x44a1e0,'result sound sink',()=>{sounds.push(m.reg('ESI'));return 0;});m.replace(0x44a9c0,'music transport',()=>0,2);m.replace(0x42a0d0,'result music cue',()=>1,2);
 m.replace(0x42f480,'GPU screenshot boundary',()=>0,1);m.replace(0x40dea0,'return title',()=>{action=2;return 0;});
 m.replace(0x46025c,'slot filename',()=>{const value=`th11_${String(stack(3)).padStart(2,'0')}.rpy`;m.write(stack(1),Buffer.from(value+'\0'));return value.length;});
 m.replace(0x435ef0,'empty catalog',()=>0,1);m.replace(0x435fc0,'empty disposal',()=>0);
 m.replace(0x4608b7,'fixed clock',()=>{m.u32(stack(1),1600000000);m.u32(stack(1)+4,0);m.reg('EDX',0);return 1600000000;});m.replace(0x436420,'save boundary',()=>{++saves;return 0;},1);
 try{for(const practice of[false,true])for(const complete of[false,true])for(const ranked of[false,true])for(const choice of[0,1,2,...(!practice&&!complete?[3]:[])]){
  m.heap=heap;o.reset();for(const[p,n]of[[menu,0x300],[battle,0x100],[score,0x30000]])m.view(p,n).fill(0);
  m.u32(0x4a5728,1);m.u32(0x4a5710,0);m.u32(0x4a5714,0);m.u32(0x4a5720,0);m.u32(0x4a573c,0);m.u32(0x4a5758,practice?16:0);m.u32(0x4a56e4,ranked?1000000:0);m.u32(0x4c37d8,0);
  const a=c.anm_manager_create(),s=c.score_file_create(),p=c.pause_create(a,...resources,s),ascii=c.ascii_create();for(let i=0;i<2;++i){put(c.anm_env_rng(a,i),12345);put(c.anm_env_rng(a,i)+4,0);}
  for(let rank=0;rank<10;++rank){put(c.score_file_data(s,0)+0x10+rank*28,500000);m.u32(score+0x18+rank*28,500000);}
  const name=Buffer.from('        \0');memory(c,c.score_file_data(s,7)+12,9).set(name);m.write(score+0x2dde0,name);
  sounds=[];m.call(complete?0x42d6b0:0x42d560);assert.equal(c.pause_end(p,+practice,+complete,ranked?1000000:0),1);
  const label=()=>`practice${practice} complete${complete} ranked${ranked} choice${choice} frame${frames}`;
  const compare=()=>{
   for(const[k,offset,size]of[[0,4,4],[1,0x10,12],[2,0x24,12],[3,0x38,0xd8],[4,0x1e8,8]])assert.deepEqual(Buffer.from(memory(c,c.pause_data(p,k),size)),Buffer.from(m.bytes(menu+offset,size)),label()+' state'+k);
   for(const[k,offset,size]of[[0,0x110,0xd8],[1,0x2cc,9],[2,0x1f0,4]])assert.deepEqual(Buffer.from(memory(c,c.pause_name(p,k),size)),Buffer.from(m.bytes(menu+offset,size)),label()+' name'+k);
   assert.equal(c.pause_end_value(p,0),m.u32(menu+0x1f8),label()+' ranked');assert.deepEqual(Buffer.from(memory(c,c.score_file_data(s,0),0x68d4)),Buffer.from(m.bytes(score+8,0x68d4)),label()+' score');
   assert.deepEqual(Array.from({length:c.pause_value(p,0)},(_,i)=>read(c.pause_data(p,5)+i*4)),sounds,label()+' sounds');
   o.update(true);assert.equal(c.anm_manager_update(a,1),1);const states=o.states();assert.equal(c.anm_manager_count(a),states.length,label()+' animation count');
   for(const vm of states){const cp=c.anm_manager_find(a,vm.id);assert.ok(cp);assert.equal(firstDifference(normalizeAnimation(memory(c,cp,0x434),read,read(cp+0x3a4)),normalizeAnimation(vm.bytes,p=>m.u32(p),m.u32(vm.p+0x3a4))),'',label()+' VM'+vm.id);++checks;}
   if(m.u32(menu+4)<27){m.u32(sup+0x1847c,0);m.u32(sup+0x18480,0xffffffff);m.f32(sup+0x18484,1);m.f32(sup+0x18488,1);m.call(0x42c150,{args:[menu]});const count=c.pause_ascii(p,ascii);assert.equal(count,m.u32(sup+0x1847c),label()+' ASCII count');
    for(let i=0;i<count;++i){const q=sup+0x87c+i*0x130,position=c.ascii_request(ascii,i,1),style=c.ascii_request(ascii,i,2);assert.equal(cpString(c.ascii_request(ascii,i,0)),string(q),label()+' ASCII text '+i);assert.deepEqual(Buffer.from(memory(c,position,12)),Buffer.from(m.bytes(q+0x100,12)),label()+' ASCII position '+i);assert.equal(read(style),m.u32(q+0x10c),label()+' ASCII color '+i);assert.equal(memory(c,style+16,1)[0],m.u32(q+0x124),label()+' shadow');++checks;}
   }
  };
  const tick=(pressed=0,repeat=0)=>{sounds=[];action=0;m.u32(0x4c92b4,pressed);m.u32(0x4c92b0,repeat);const state=m.u32(menu+4);if(state<27)m.call(state>=20?0x42e6b0:0x42d9d0,{args:[menu]});for(const offset of[0x10,0x24]){const n=m.i32(menu+offset+4);m.i32(menu+offset,n);m.i32(menu+offset+4,n+1);m.f32(menu+offset+8,m.f32(menu+offset+8)+1);}assert.equal(c.pause_update(p,pressed,repeat),1);compare();++frames;};
  try{compare();for(let i=0;i<11;++i)tick();
   if(!practice&&ranked){for(let i=0;i<10;++i)tick();for(let i=0;i<8;++i)tick(1);tick(2);tick(64);tick(1);tick(128);tick(1);}
   const compact=practice||(complete&&ranked);assert.equal(read(c.pause_data(p,0)),compact?22:14);for(let i=0;i<choice;++i)tick(32);tick(1);
   if(choice===(compact?1:2)){for(let i=0;i<11;++i)tick();tick(32);tick(1);for(let i=0;i<10;++i)tick();if(c.pause_name(p,2)&&read(c.pause_name(p,2))===0)for(let i=0;i<8;++i)tick(1);tick(1);for(let i=0;i<11;++i)tick();tick(2);tick(2);tick(1);}
   for(let i=0;i<13;++i)tick();assert.equal(read(c.pause_data(p,0)),27);
  }finally{c.ascii_delete(ascii);c.pause_delete(p);c.score_file_delete(s);c.anm_manager_delete(a);}
 }report('pause-end',{passed:true,frames,checks,saves,scope:'Native game-over/practice menu, high-score and replay-name state, all overlay animation families. Persistence and session continue routing validated separately.'});
 }finally{for(const r of resources)c.anm_delete(r);m.close();}
});
