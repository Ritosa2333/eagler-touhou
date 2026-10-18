import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report,root} from './helpers.mjs';import {readFileSync} from 'node:fs';import{resolve}from'node:path';import {animationOracle} from './anm-oracle.mjs';import {normalizeAnimation,firstDifference} from './shot-oracle.mjs';
test('TH11 original title selection state machine and scripted animations',async()=>{
 const c=await core(),m=await oracle(),o=animationOracle(m),loaded=[o.load('title.anm'),o.load('title_v.anm',true),o.load('ascii.anm',true),o.load('text.anm',true)],resources=[];let frames=0,checks=0;
 const read=p=>new DataView(c.memory.buffer).getUint32(p,true),put=(p,v)=>new DataView(c.memory.buffer).setUint32(p,v>>>0,true);
 for(let i=0;i<4;++i){const r=c.anm_create(),d=c.allocate(loaded[i].source.length);memory(c,d,loaded[i].source.length).set(loaded[i].source);assert.equal(c.anm_open(r,d,loaded[i].source.length),1);c.release(d);resources.push(r);m.u32(loaded[i].original,[24,25,2,0][i]);}
 m.u32(0x4c3808,loaded[3].original);
 const comment=readFileSync(resolve(root,'reference/assets/musiccmt.txt')),comments=c.allocate(comment.length);memory(c,comments,comment.length).set(comment);
 const native=m.allocate(0x5b80),score=m.allocate(0x30000),sup=m.allocate(0x18500),cb=m.allocate(0x30),buttons=m.allocate(32),localDate=m.allocate(36),heap=m.heap;let sounds=[];
 m.replace(0x4575a0,'SDL controller sample boundary',()=>buttons);
 m.replace(0x4594a0,'asynchronous replay catalog boundary',()=>0,1);
 m.replace(0x435c50,'replay catalog fixture destructor',()=>0,1);
 m.replace(0x45fd49,'replay catalog fixture free',()=>0);
 m.u32(0x4a8ebc,score);m.u32(0x4a8d58,sup);m.u32(sup+0x184ac,loaded[2].original);
 m.replace(0x44a1e0,'menu audio sink',()=>{sounds.push(m.reg('ESI'));return 0;});
 for(const[a,n]of [[0x40fca0,2],[0x4489e0,5],[0x42a270,1],[0x44a9c0,2],[0x458100,0]])m.replace(a,'menu host boundary',()=>0,n);
 const string=q=>{const b=Buffer.from(m.bytes(q,256)),n=b.indexOf(0);return b.subarray(0,n).toString('latin1');},stack=n=>m.u32(m.reg('ESP')+4*n);let texts=[],music=-1;
 m.replace(0x460be4,'platform local calendar',()=>{const time=Buffer.from(m.bytes(stack(1),8)).readBigInt64LE(),d=new Date(Number(time)*1000);[d.getUTCSeconds(),d.getUTCMinutes(),d.getUTCHours(),d.getUTCDate(),d.getUTCMonth(),d.getUTCFullYear()-1900,d.getUTCDay(),0,0].forEach((v,i)=>m.i32(localDate+i*4,v));return localDate;});
 const format=(fmt,p)=>{let q=p;return fmt.replace(/%%|%([ 0]?)(\d*)(?:\.(\d+))?l?([dsfc])/g,(match,pad,width,precision,type)=>{if(match==='%%')return'%';let value;if(type==='f'){value=Buffer.from(m.bytes(q,8)).readDoubleLE().toFixed(precision===undefined?6:Number(precision));q+=8;}else{const v=m.i32(q);q+=4;if(type==='s')value=string(v);else if(type==='c')value=String.fromCharCode(v&255);else value=(v<0?'-':'')+String(Math.abs(v)).padStart(Number(precision)||0,'0');}return value.padStart(Number(width)||0,pad==='0'?'0':' ');});};
 m.replace(0x45fdd2,'menu CRT format boundary',()=>{const value=format(string(stack(2)),stack(3));m.write(stack(1),Buffer.from(value+'\0','latin1'));return value.length;});
 m.replace(0x46025c,'menu sprintf boundary',()=>{const value=format(string(stack(2)),m.reg('ESP')+12);m.write(stack(1),Buffer.from(value+'\0','latin1'));return value.length;});
 m.replace(0x458400,'music comments file',()=>{m.u32(stack(1),comment.length);const q=m.allocate(comment.length+32);m.view(q,comment.length+32).fill(0);m.write(q,comment);return q;},2);
 m.replace(0x454c60,'music text output',()=>{const text=string(stack(5)).replace('%2d',String(stack(6)).padStart(2,' '));texts.push([m.u32(m.reg('ESI')),stack(1),stack(3),stack(4),m.reg('EDI'),text]);return 0;});
 m.replace(0x454de0,'centered records text output',()=>{let arg=6;const text=string(stack(5)).replace(/%(\d*)([ds])/g,(_,width,type)=>{const v=stack(arg++);return(type==='s'?string(v):String(v|0)).padStart(Number(width)||0,' ');});texts.push([m.u32(m.reg('ESI')),stack(1),stack(3),0,stack(4),text]);return 0;});
 m.replace(0x42a0d0,'music track output',()=>{const name=string(stack(2));music=Number(name.match(/th\d+_(\d+)\.wav$/)?.[1]??-1);return 0;},2);
 m.replace(0x435ef0,'empty save replay catalog',()=>0,1);
 m.replace(0x44a9c0,'music transport',()=>{if(stack(1)===3)music=-2;return 0;},2);m.replace(0x42a150,'music title reset',()=>0,2);
 const selectionGlobals=[0x4a5720,0x4a3824,null,0x4a5710,0x4a5714,0x4c325c,0x4c3260,0x4a5728,0x4a5758,0x4c3258,0x4a5764];
 try{for(const unlocked of[false,true])for(const target of[0,1,2,3,4,5,6,7]){
  if(target===1&&!unlocked)continue;
  m.heap=heap;m.view(native,0x5b80).fill(0);m.view(score,0x30000).fill(0);o.reset();
  const a=c.anm_manager_create(),s=c.score_file_create(),p=c.title_create(a,...resources.slice(0,3),s),ascii=c.ascii_create(),cursor=c.title_data(p,0),sel=c.title_data(p,2);sounds=[];assert.equal(c.title_music_load(p,resources[3],comments,comment.length),1);
  for(const v of[0,1]){put(c.anm_env_rng(a,v),12345);put(c.anm_env_rng(a,v)+4,0);}
  m.u32(native+0x14,loaded[0].original);m.u32(native+0x18,loaded[1].original);m.u32(native+0x10,cb);m.i32(native+0x1c,1);m.u32(native+0xf4,1);m.u32(native+0x5b74,2);m.u32(native+0x2bc,0x4a7948);m.u32(native+0x2c0,1);m.i32(native+0x2b0,-1);
  for(let i=0;i<selectionGlobals.length;++i)if(selectionGlobals[i])m.u32(selectionGlobals[i],read(sel+i*4));m.u32(native+0x58e8,1);
  if(unlocked)for(let i=0;i<6;++i){memory(c,c.score_file_data(s,7)+0x1c+i,1)[0]=16;m.view(score+0x2ddf0+i,1)[0]=16;}
  m.write(0x4c3448,memory(c,c.title_data(p,5),0x3c));m.write(0x4c93dc,memory(c,c.title_data(p,5)+4,18));m.view(buttons,32).fill(0);
  m.u32(0x4c3254,0);m.u32(0x4c9890,0);m.u32(0x4c9894,0);m.view(0x4c9bc0,256).fill(0);let keysSample=Buffer.alloc(256);m.replace(0x458100,'keyboard input boundary',()=>{m.write(0x4c9bc0,keysSample);return 1;});
  for(let id=0;id<175;id+=3)for(let player=0;player<7;++player){const off=0x664+id*0x90,cp=c.score_file_data(s,player)+off,np=score+8+player*0x68d4+off;const name=Buffer.from('Spell '+id+'\0');memory(c,cp,name.length).set(name);m.write(np,name);for(const [offset,value]of[[0x80,id%5],[0x84,id+1]]){put(cp+offset,value);m.u32(np+offset,value);}}
  const tick=(pressed=0,repeat=0)=>{
   const screen=c.title_value(p,0),address={1:0x43a0a0,3:0x43a6f0,4:0x43b9d0,5:0x43c7c0,6:0x43cbe0,7:0x43d0b0,8:0x43d6a0,10:0x43e790,11:0x43dd90,13:0x4408f0,14:0x43f6b0,15:0x43ffa0}[screen];assert.ok(address,'supported menu');
   sounds=[];texts=[];music=-1;m.u32(0x4c92b4,pressed);m.u32(0x4c92b0,repeat);m.reg('EAX',native);m.reg('EBX',native);m.call(address,{ecx:screen===15?native:0,args:[5,6,7,14].includes(screen)?[native]:[]});
   const now=m.i32(native+0x2b4);m.i32(native+0x2b0,now);m.i32(native+0x2b4,now+1);m.f32(native+0x2b8,m.f32(native+0x2b8)+1);
   const success=c.title_update(p,pressed,repeat),error=Buffer.from(memory(c,c.title_error(p),128));assert.equal(success,1,`menu error screen${screen} frame${frames}: ${error.subarray(0,error.indexOf(0))}`);
   const label=`unlock${unlocked} target${target} frame${frames} screen${screen}`;
   assert.equal(c.title_value(p,0),m.i32(native+0x1c),label+' screen');assert.equal(c.title_value(p,1),m.i32(native+0x20),label+' substate');assert.equal(c.title_value(p,2),m.u32(native+0x5b74),label+' flags');
   assert.deepEqual(Buffer.from(memory(c,cursor,0xd8)),Buffer.from(m.bytes(native+0x24,0xd8)),label+' cursor');
   assert.deepEqual(Buffer.from(memory(c,c.title_data(p,1),12)),Buffer.from(m.bytes(native+0x2b0,12)),label+' timer');
   for(let i=0;i<selectionGlobals.length;++i)if(selectionGlobals[i])assert.equal(read(sel+i*4),m.u32(selectionGlobals[i]),label+' selection '+i);
   assert.deepEqual(Buffer.from(memory(c,c.title_data(p,5),0x3c)),Buffer.from(m.bytes(0x4c3448,0x3c)),label+' config');
   const cpString=q=>{const b=Buffer.from(memory(c,q,256));return b.subarray(0,b.indexOf(0)).toString('latin1');};
   assert.deepEqual(Array.from({length:c.title_text_count(p)},(_,i)=>{const q=c.title_text_data(p,i,0);return [...Array.from({length:5},(_,n)=>read(q+n*4)),cpString(c.title_text_data(p,i,1))];}),texts,label+' text');
   assert.equal(c.title_music_request(p),music,label+' music');
   if(screen===13){assert.deepEqual(Buffer.from(memory(c,c.title_music_state(p),16)),Buffer.from(m.bytes(native+0x680,16)),label+' music state');assert.equal(read(c.title_music_state(p)+16),m.u32(native+0x58d0),label+' scroll');}
   if(screen===11){assert.deepEqual(Buffer.from(memory(c,c.title_page(p),0xd8)),Buffer.from(m.bytes(native+0x1d4,0xd8)),label+' replay page');for(const[k,a]of[[0,0x4c3254],[1,native+0x59d4],[2,native+0x59d8]])assert.equal(c.title_replay_value(p,k),m.i32(a),label+' replay '+k);}
   if(screen===10){for(const [ptr,off]of[[c.title_page(p),0x1d4],[c.title_secondary(p),0xfc]])assert.deepEqual(Buffer.from(memory(c,ptr,0xd8)),Buffer.from(m.bytes(native+off,0xd8)),label+' records cursor');for(const [k,a]of[[0,native+0x2ac],[1,0x4c9890],[2,0x4c9894]])assert.equal(c.title_record_value(p,k),m.i32(a),label+' record '+k);assert.deepEqual(Buffer.from(memory(c,c.score_file_data(s,7),0x448)),Buffer.from(m.bytes(score+0x2ddd4,0x448)),label+' unlock settings');}
   if(screen===14||screen===15){for(const [kind,offset,size]of[[0,0x58ec,0xd8],[1,0x58d4,9],[2,0x58e0,8]])assert.deepEqual(Buffer.from(memory(c,c.title_name_data(p,kind),size)),Buffer.from(m.bytes(native+offset,size)),label+' name '+kind);for(let index=0;index<8;++index)assert.deepEqual(Buffer.from(memory(c,c.score_file_data(s,index),index<7?0x68d4:0x448)),Buffer.from(m.bytes(index<7?score+8+index*0x68d4:score+0x2ddd4,index<7?0x68d4:0x448)),label+' saved score '+index);}
   assert.deepEqual(Array.from({length:c.title_value(p,3)},(_,i)=>read(c.title_data(p,4)+4*i)),sounds,label+' sounds');
   for(let i=0;i<239;++i)assert.equal(read(c.title_data(p,3)+i*4),m.u32(native+0x2c4+i*4),label+' handle '+i);
   const draw={8:0x43daa0,10:0x43f410,11:0x43e2f0,14:0x43fc50,15:0x440520}[c.title_value(p,0)];
   if(draw){m.u32(sup+0x1847c,0);m.u32(sup+0x18480,0xffffffff);m.u32(sup+0x18494,0);m.f32(sup+0x18484,1);m.f32(sup+0x18488,1);m.reg('EDI',native);m.call(draw,{args:[14,15].includes(c.title_value(p,0))?[native]:[]});const count=c.title_ascii(p,ascii);assert.equal(count,m.u32(sup+0x1847c),label+' ascii count');
    for(let i=0;i<count;++i){const q=sup+0x87c+i*0x130,position=c.ascii_request(ascii,i,1),style=c.ascii_request(ascii,i,2);assert.equal(cpString(c.ascii_request(ascii,i,0)),string(q),label+' ascii text '+i);assert.deepEqual(Buffer.from(memory(c,position,12)),Buffer.from(m.bytes(q+0x100,12)),label+' ascii position '+i);assert.equal(read(style),m.u32(q+0x10c),label+' ascii color '+i);assert.equal(memory(c,style+16,1)[0],m.u32(q+0x124),label+' ascii shadow');++checks;}
   }
   o.update(false);assert.equal(c.anm_manager_update(a,0),1);const states=o.states();assert.equal(c.anm_manager_count(a),states.length,label+' VM count');
   for(const vm of states){const cp=c.anm_manager_find(a,vm.id);assert.ok(cp,label+' VM id');const actual=normalizeAnimation(memory(c,cp,0x434),read,read(cp+0x3a4)),expected=normalizeAnimation(vm.bytes,p=>m.u32(p),m.u32(vm.p+0x3a4));assert.equal(firstDifference(actual,expected),'',label+' VM '+vm.id);++checks;}
   ++frames;
  };
  try{
   for(let i=0;i<84;++i)tick();
   // Exercise cancel-to-exit and movement wrap before the actual choice.
   tick(2);tick(32);tick(16);tick(32);
   for(let i=0;i<8&&read(cursor)!==target;++i)tick(32);assert.equal(read(cursor),target);
   tick(1);for(let i=0;i<20;++i)tick();
   if([0,1,2].includes(target)){
    for(let i=0;i<9;++i)tick();tick(32);tick(16);tick(1);for(let i=0;i<14;++i)tick();
    for(let i=0;i<9;++i)tick();tick(128);tick(64);tick(1);for(let i=0;i<14;++i)tick();
    for(let i=0;i<9;++i)tick();tick(32);tick(32);tick(16);tick(1);
    const wait=target===2?10:40;for(let i=0;i<wait;++i)tick();
    if(target===2){
     for(let i=0;i<13;++i)tick();tick(32);tick(1);assert.equal(c.title_value(p,1),2,'locked practice stays on list');
     const choice=read(cursor),difficulty=read(sel),character=read(sel+12),partner=read(sel+16),off=0x5a9+(choice+difficulty*6)*8,index=character*3+partner;
     memory(c,c.score_file_data(s,index)+off,1)[0]=1;m.view(score+8+index*0x68d4+off,1)[0]=1;
     tick(1);for(let i=0;i<40;++i)tick();
    }
   }
   if(target===6){
    for(let i=0;i<9;++i)tick();for(let i=0;i<24;++i)tick(64);for(let i=0;i<24;++i)tick(128);
    tick(32);for(let i=0;i<65;++i)tick();for(let i=0;i<24;++i)tick(64);for(let i=0;i<24;++i)tick(128);
    tick(32);tick(32);tick(1);tick(16);tick(1);for(let i=0;i<10;++i)tick();for(let i=0;i<9;++i)tick();
    const button=number=>{c.title_buttons(p,1<<number);m.view(buttons,32).fill(0);m.view(buttons+number,1)[0]=128;tick();c.title_buttons(p,0);m.view(buttons,32).fill(0);};
    button(0);button(7);tick(32);button(10);tick(32);button(7);tick(32);button(30);tick(32);button(0);
    tick(32);tick(1);tick(16);button(12);tick(32);tick(32);tick(1);for(let i=0;i<10;++i)tick();for(let i=0;i<9;++i)tick();
    tick(2);tick(2);for(let i=0;i<10;++i)tick();for(let i=0;i<13;++i)tick();
   }
   if(target===5){
    for(let i=0;i<40;++i)tick();tick(32);tick(1);for(let i=0;i<20;++i)tick();tick(1);for(let i=0;i<20;++i)tick();
    for(let i=0;i<25;++i)tick(32);for(let i=0;i<25;++i)tick(16);tick(2);for(let i=0;i<10;++i)tick();for(let i=0;i<13;++i)tick();
   }
   if(target===3){
    tick();for(let i=0;i<12;++i)tick();tick(1);assert.equal(c.title_value(p,1),2,'empty slot stays on list');
    for(let index=0;index<4;++index){
     const raw=readFileSync(resolve(root,`reference/assets/demo${index}.rpy`)),data=c.allocate(raw.length),r=c.replay_create();memory(c,data,raw.length).set(raw);assert.equal(c.replay_open(r,data,raw.length),1);assert.equal(c.title_replay_load(p,index===3?25:index,data,raw.length),1);
     const size=c.replay_size(r),bytes=Buffer.from(memory(c,c.replay_data(r),size)),nr=m.allocate(0x2dc),nb=m.allocate(size);m.view(nr,0x2dc).fill(0);m.write(nb,bytes);m.u32(nr+0x18,nb);
     for(let st=1;st<=7;++st){const sp=c.replay_stage(r,st);if(sp)m.u32(nr+0xb4+st*0x24,nb+read(sp+4));}
     const path=Buffer.from(index===3?'th11_udtest.rpy\0':`th11_0${index+1}.rpy\0`);memory(c,data,path.length).set(path);c.title_replay_path(p,index===3?25:index,data);m.write(nr+0x1dc,path);
     m.u32(native+0x59dc+(index===3?25:index)*4,nr);c.replay_delete(r);c.release(data);
    }
    c.title_scan_done(p);m.u32(native+0x5b74,m.u32(native+0x5b74)|8);
    tick(128);tick(1);for(let i=0;i<17;++i)tick();tick(32);tick(16);tick(2);tick(64);tick(32);tick(1);for(let i=0;i<17;++i)tick();
    if(unlocked){tick(1);for(let i=0;i<32;++i)tick();assert.equal(c.title_replay_value(p,4),1);}
    else{tick(2);tick(2);for(let i=0;i<6;++i)tick();for(let i=0;i<13;++i)tick();}
   }
   if(target===4){
    for(let i=0;i<9;++i)tick();for(let i=0;i<6;++i){tick(128);tick(1);tick(32);}for(let i=0;i<6;++i){tick(64);tick(1);tick(16);}
    while(read(c.title_secondary(p))!==4)tick(32);while(read(cursor)!==3)tick(128);
    for(const key of[31,30,21,23,38,24,47,18,48,18,18,19]){keysSample[key]=128;memory(c,c.title_key_edges(p)+key,1)[0]=128;tick();keysSample.fill(0);memory(c,c.title_key_edges(p),256).fill(0);tick();}
    tick();assert.equal(memory(c,c.score_file_data(s,7)+0x1c,1)[0],17,'original hidden unlock');tick(2);for(let i=0;i<6;++i)tick();for(let i=0;i<13;++i)tick();
   }
   if(target===7){
    const value=unlocked?1234567:-1,stamp=1600000000,clock=m.allocate(0x40);m.u32(0x4a8d80,clock);m.view(clock,0x40).fill(0);const time=Buffer.alloc(16);time.writeDoubleLE(600);time.writeDoubleLE(600,8);m.write(clock+0x24,time);
    m.replace(0x4608b7,'result wall-clock boundary',()=>{m.u32(stack(1),stamp);m.u32(stack(1)+4,0);m.reg('EDX',0);return stamp;});m.i32(0x4a56e4,value);m.i32(0x4a573c,0);
    const name=Buffer.from('        \0');memory(c,c.score_file_data(s,7)+12,9).set(name);m.write(score+0x2dde0,name);
    c.title_result_set(p,value,stamp);c.title_state(p,14,0,c.title_value(p,2));m.i32(native+0x1c,14);m.i32(native+0x20,0);m.i32(native+0x2b0,-1);m.i32(native+0x2b4,0);m.f32(native+0x2b8,0);
    for(let i=0;i<9;++i)tick();
    if(unlocked){tick(2);tick(1);tick(128);tick(1);tick(16);tick(32);tick(64);tick(128);for(let i=0;i<8;++i)tick(1);tick(2);tick(64);tick(1);tick(128);tick(1);}
    else tick(2);
    for(let i=0;i<6;++i)tick();for(let i=0;i<9;++i)tick();tick(32);tick(16);tick(2);for(let i=0;i<6;++i)tick();
   }
  }finally{c.ascii_delete(ascii);c.title_delete(p);c.score_file_delete(s);c.anm_manager_delete(a);}
 }
 report('title-selection',{passed:true,frames,checks,scope:'Original main, difficulty, character and partner states, disabled Extra entries, transition timing, cursor/history, sounds and full normalized ANM state. Both initial and return menus, practice handoff and original game-start handoff.'});
 }finally{for(const r of resources)c.anm_delete(r);m.close();}
});
