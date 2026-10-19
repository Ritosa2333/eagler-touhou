import test from 'node:test';import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';import {resolve} from 'node:path';
import {core,oracle,memory,report,root} from './helpers.mjs';import {animationOracle} from './anm-oracle.mjs';
import {normalizeAnimation,firstDifference} from './shot-oracle.mjs';
test('TH11 complete player frame orchestration follows the original lifecycle and shot order',async()=>{
 const c=await core(),m=await oracle(),o=animationOracle(m);let frames=0,states=0,animations=0,shots=0,scenarios=0,events=[],special=0;
 const dv=()=>new DataView(c.memory.buffer),put=(p,v)=>dv().setInt32(p,v,true),flt=(p,v)=>dv().setFloat32(p,v,true),read=p=>dv().getUint32(p,true);
 const stack=n=>m.u32(m.reg('ESP')+n*4),vec=p=>[m.u32(p),m.u32(p+4),m.u32(p+8)],hook=(address,name,fn,n=0)=>m.replace(address,name,()=>{events.push(...fn());return 0;},n);
 hook(0x44a260,'shot sound output',()=>[1,m.reg('EDI'),stack(1)],1);hook(0x44a1e0,'player sound output',()=>[2,m.reg('ESI')]);
 hook(0x4245e0,'item attraction integration',()=>[3]);hook(0x40af90,'circular bullet cancellation',()=>[4,...vec(m.reg('EBX')),stack(1),stack(3)],3);
 hook(0x40b5c0,'global bullet cancellation',()=>[5,m.reg('EBX')]);hook(0x424fe0,'circular laser cancellation',()=>[6,...vec(m.reg('ESI')),stack(1),stack(2),stack(3)],3);
 hook(0x425040,'global laser cancellation',()=>[7,m.reg('EBX'),m.reg('EDI')]);hook(0x406510,'bomb start integration',()=>{m.i32(special+0x3c,1);return [8];});
 hook(0x424230,'death item spawn integration',()=>[9,m.reg('ECX'),...vec(m.reg('EAX')),stack(1),stack(2),stack(3)],3);
 hook(0x41a060,'lives display integration',()=>[10,m.reg('EDX'),stack(1)],1);hook(0x42f6f0,'death recording integration',()=>[11],1);
 hook(0x42c760,'replay game over integration',()=>[12,1]);hook(0x42d560,'game over integration',()=>[12,0]);
 const motionFields=[[0,0x87c,12],[12,0x888,8],[20,0x890,16],[36,0x8a0,12],[48,0x8ac,12],[60,0x8b8,8],[68,0x914,8],[76,0x91c,12],[88,0x8d20,4],[92,0x95c,4],[96,0x8ba0,16],[112,0x8bc8,4],[116,0x8bc4,4],[120,0x7c90,4],[124,0x8c14,4]];
 const frameFields=[[0,0x928,4],[64,0x908,12],[76,0x8e4,36],[112,0x8cc,24],[136,0x8bcc,72]];
 const economyGlobals=[0x4a56e4,0x4a56e8,0x4a56f0,0x4a56f4,0x4a5718,0x4a571c,0x4a5720,0x4a5744,0x4a5748,0x4a574c];
 try{for(const [combination,name]of ['pl00a','pl00b','pl00c','pl01a','pl01b','pl01c'].entries()){
  if(process.env.TH11_PLAYER_FILE&&name!==process.env.TH11_PLAYER_FILE)continue;
  const player=o.load(name.slice(0,4)+'.anm'),bullet=o.load('bullet.anm',true),anms=[];
  for(const r of [player,bullet]){const a=c.anm_create(),d=c.allocate(r.source.length);memory(c,d,r.source.length).set(r.source);assert.equal(c.anm_open(a,d,r.source.length),1);anms.push([a,d]);}
  const bytes=readFileSync(resolve(root,'reference/assets',name+'.sht')),sht=c.sht_create(),sd=c.allocate(bytes.length);memory(c,sd,bytes.length).set(bytes);assert.equal(c.sht_open(sht,sd,bytes.length),1);
  const ns=m.allocate(bytes.length);m.write(ns,bytes);const spawn=[0,0x434e20,0x435210,0x435250],update=[0,0x434e30,0x4352a0,0x435330];
  for(let g=0;g<bytes.readUInt16LE(2);++g){let off=bytes.readUInt32LE(0x268+g*8);m.u32(ns+0x268+g*8,ns+off);while(bytes.readInt8(off)>=0){m.u32(ns+off+0x24,spawn[bytes.readUInt32LE(off+0x24)]);m.u32(ns+off+0x28,update[bytes.readUInt32LE(off+0x28)]);off+=0x34;}}
  const np=m.allocate(0x8d40),nem=m.allocate(0x100),nb=m.allocate(0x46d680),hud=m.allocate(0x4460),spell=m.allocate(0x900),game=m.allocate(0x100),sound=m.allocate(0x100);special=m.allocate(0x500);const heap=m.heap;
  for(const speed of [.5,1,1.5]){
   if(process.env.TH11_PLAYER_RATE&&speed!==+process.env.TH11_PLAYER_RATE)continue;
   m.heap=heap;m.view(heap,6000000).fill(0);for(const [p,n]of [[np,0x8d40],[nem,0x100],[nb,0x46d680],[hud,0x4460],[spell,0x900],[game,0x100],[sound,0x100],[special,0x500]])m.view(p,n).fill(0);o.reset();
   for(const [addr,p]of [[0x4a8eb4,np],[0x4a8d7c,nem],[0x4a8d68,nb],[0x4a8d84,hud],[0x4a8d6c,spell],[0x4a8eb8,game],[0x4a8e88,sound],[0x4a8d64,special]])m.u32(addr,p);
   m.u32(np+0x10,player.original);m.u32(np+0x92c,ns);m.u32(nb+0x46d674,bullet.original);m.i32(0x4a5710,Math.floor(combination/3));m.i32(0x4a5714,combination%3);
   const manager=c.anm_manager_create(),f=c.pf_create(sht,anms[0][0],anms[1][0],manager),s=c.pf_pointer(f,0),state=c.pf_pointer(f,1),body=c.pf_pointer(f,2),sp=c.pf_pointer(f,3),economy=c.pf_pointer(f,4),rate=c.anm_env_rate(manager);
   flt(rate,speed);m.f32(0x4a7948,speed);for(const vis of [0,1]){memory(c,c.anm_env_rng(manager,vis),8).fill(0);put(c.anm_env_rng(manager,vis),12345);}
   const step=bytes.readInt32LE(0x24),max=bytes.readInt32LE(0x20)*step;put(economy+4,max);put(economy+8,25000000);put(economy+12,750);put(economy+16,4);put(economy+20,2);put(economy+28,123);put(economy+32,max);put(economy+36,step);
   for(const [i,addr]of economyGlobals.entries())m.u32(addr,read(economy+i*4));put(state+208,10000000);m.i32(0x4a5768,10000000);
   put(s+12,-1600);put(s+16,0xf000);flt(s,-12.5);flt(s+4,480);put(s+92,0);[2,2,0,16,16,0,48,48,0].forEach((v,i)=>flt(state+76+i*4,v));
   function setTimer(cp,native,value){c.timer_set(cp,value,rate);m.reg('EAX',np+native);m.call(0x406100,{args:[value]});}
   for(const [off,native]of [[4,0x944],[24,0x958],[44,0x8bb0]])setTimer(state+off,native,0);setTimer(c.pf_pointer(f,5),0x930,-1);
   c.pf_input(f,0,0,combination,3);assert.equal(c.pf_prepare(f,sht),1);for(const [off,native,n]of motionFields)m.write(np+native,memory(c,s+off,n));for(const [off,native,n]of frameFields)m.write(np+native,memory(c,state+off,n));
   // Native initialization below intentionally covers binding and option reconstruction,
   // not the unrelated resource-loading/registration constructor.
   m.i32(np+0x7c90,0);m.reg('EAX',np+0x14);m.reg('EDI',player.original);m.reg('EBX',0);m.call(0x44acd0);m.reg('EBX',np);m.call(0x432cc0);
   m.write(np+0x7c9c,memory(c,c.pf_pointer(f,6),32*0x74));for(let i=0;i<256;++i)m.write(np+0x96c+i*0x6c,memory(c,c.pf_shot(f,i),0x6c));
   function compare(label){
    const equal=(a,b,n,why)=>{assert.equal(firstDifference(Buffer.from(memory(c,a,n)),Buffer.from(m.bytes(b,n))),'',label+' '+why);++states;};
    for(const [off,native,n]of motionFields)equal(s+off,np+native,n,`motion0x${native.toString(16)}`);for(const [off,native,n]of frameFields)equal(state+off,np+native,n,`state0x${native.toString(16)}`);
    for(const [cp,native]of [[state+4,0x944],[state+24,0x958],[state+44,0x8bb0],[c.pf_pointer(f,5),0x930]]){equal(cp,np+native,12,'timer '+native.toString(16));equal(cp+16,np+native+16,4,'timer flags');}
    for(const [i,addr]of economyGlobals.entries())assert.equal(read(economy+i*4),m.u32(addr),label+' economy'+i);
    for(const [off,native]of [[0,0x8e0],[4,0x88c],[8,0x8e4]])equal(sp+off,spell+native,4,'spell');
    assert.equal(c.pf_value(f,1),m.u32(nem+0x10),label+' enemy death count');assert.equal(c.pf_value(f,5),m.u32(np+0x756c),label+' focus handle');assert.equal(c.pf_value(f,6),m.u32(special+0x3c),label+' bomb active');
    for(let i=0;i<8;++i){const a=Buffer.from(m.bytes(np+0x7570+i*0xe4,0xe4)),cb=a.readUInt32LE(0xdc);a.writeUInt32LE(cb===0x433690?1:cb===0x4337a0?2:cb,0xdc);assert.equal(firstDifference(Buffer.from(memory(c,c.pf_option(f,i),0xe4)),a),'',label+' option'+i);}
    for(let i=0;i<256;++i){const a=Buffer.from(m.bytes(np+0x96c+i*0x6c,0x6c)),b=Buffer.from(memory(c,c.pf_shot(f,i),0x6c));if(!a.readUInt32LE(0x68)&&!b.readUInt32LE(0x68))continue;for(const v of [a,b]){v.writeUInt32LE(0,12);for(const off of [0x54,0x68])v.writeUInt32LE(v.readUInt32LE(off)?1:0,off);}assert.equal(firstDifference(b,a),'',label+' shot'+i);++shots;}
    equal(c.pf_pointer(f,7),np+0x8b8c,20,'laser flags');const areas=c.pf_pointer(f,6);for(let i=0;i<32;++i){const a=Buffer.from(m.bytes(np+0x7c9c+i*0x74,0x74)),b=Buffer.from(memory(c,areas+i*0x74,0x74));a.writeUInt32LE(0,0x58);b.writeUInt32LE(0,0x58);assert.equal(firstDifference(b,a),'',label+' area'+i);}
    function animation(cp,np){assert.equal(firstDifference(normalizeAnimation(memory(c,cp,0x434),read,read(cp+0x3a4)),normalizeAnimation(m.bytes(np,0x434),p=>m.u32(p),m.u32(np+0x3a4))),'',label+' animation');++animations;}
    animation(body,np+0x14);const native=o.states();assert.equal(c.anm_manager_count(manager),native.length,label+' animation count');for(const item of native){const cp=c.anm_manager_find(manager,item.id);assert.ok(cp,label+' animation'+item.id);animation(cp,item.p);}
    const ep=c.pf_pointer(f,8);assert.deepEqual(Array.from({length:c.pf_value(f,0)},(_,i)=>read(ep+i*4)),events.map(v=>v>>>0),label+' world events');
    for(const vis of [0,1])assert.deepEqual(Buffer.from(memory(c,c.anm_env_rng(manager,vis),8)),Buffer.from(m.bytes(vis?0x4c2ef8:0x4c2f00,8)),label+' RNG');
    assert.equal(read(rate),m.u32(0x4a7948),label+' simulation rate');
   }
   try{for(let frame=0;frame<360;++frame){
    const label=`${name} rate${speed} frame${frame}`,held=frame===152?3:frame>=130&&frame<190?1|([0x80,0x48,0x18,0x20][Math.floor(frame/11)%4]):frame>=290?1:0,pressed=frame%29===0?0x400:0;
    if(frame===170)m.i32(special+0x3c,0);
    const flags=3|(m.i32(special+0x3c)?16:0)|(frame>=135&&frame<140?32:0)|(frame>=325&&frame<330?4:0);
    c.pf_input(f,held,pressed,combination,flags);m.u32(0x4c93c0,held);m.u32(0x4c93cc,pressed);m.i32(nem+0x70,1);m.i32(hud+0x4420,flags&32?16:0);m.i32(hud+0x4438,flags&4?1:0);events=[];
    if(frame===150||frame===190){put(sp,0x23);put(sp+4,frame===150?40:80);put(sp+8,10000);m.u32(spell+0x8e0,0x23);m.i32(spell+0x88c,frame===150?40:80);m.i32(spell+0x8e4,10000);m.reg('EAX',np);m.call(0x432a90);assert.equal(c.pf_action(f,2),1,label+' hit');compare(label+' hit');}
    if(frame===340){put(state,3);m.i32(np+0x928,3);setTimer(state+4,0x944,14);}
    assert.equal(m.call(0x431070,{args:[np],limit:4000000}),1);assert.equal(c.pf_action(f,1),1,label+` errors ${[2,3,4].map(k=>c.pf_value(f,k))}`);compare(label);++frames;
    for(const overlay of [0,1]){o.update(overlay);assert.equal(c.anm_manager_update(manager,overlay),1);}
   }++scenarios;
   // Stage completion recalls both option families; next-stage entry must
   // preserve coordinates, weapon selection and shots while resetting clocks.
   events=[];m.call(0x41a0f0);assert.equal(c.pf_action(f,5),1);compare(`${name} recall`);
   events=[];m.call(0x42fec0);assert.equal(c.pf_action(f,4),1);compare(`${name} next stage`);
   ++scenarios;
   for(const lives of [0,1,2,4])for(let level=0;level<=bytes.readInt32LE(0x20);++level){
    const label=`${name} rate${speed} drop lives${lives} level${level}`;
    put(economy+4,level*step);m.i32(0x4a56e8,level*step);put(economy+16,lives);m.i32(0x4a5718,lives);put(state,2);m.i32(np+0x928,2);setTimer(state+4,0x944,3);
    c.pf_input(f,0,0,combination,3);m.i32(special+0x3c,0);m.i32(hud+0x4438,0);m.i32(hud+0x4420,0);m.u32(0x4c93c0,0);events=[];
    assert.equal(m.call(0x431070,{args:[np],limit:4000000}),1);assert.equal(c.pf_action(f,1),1);compare(label);++frames;++scenarios;
    for(const overlay of [0,1]){o.update(overlay);assert.equal(c.anm_manager_update(manager,overlay),1);}
   }
   for(const replay of [0,1])for(const active of [0,1])for(const elapsed of [0,59,60,90]){
    const label=`${name} death replay${replay} bomb${active} spell${elapsed}`;
    put(economy+8,12000000);m.i32(0x4a56f0,12000000);put(economy+16,0);m.i32(0x4a5718,0);put(sp,0x23);put(sp+4,elapsed);put(sp+8,10000);m.i32(spell+0x8e0,0x23);m.i32(spell+0x88c,elapsed);m.i32(spell+0x8e4,10000);
    c.pf_input(f,0,0,combination,3|(active?16:0)|(replay?64:0));m.i32(game+0x10,replay);m.i32(special+0x3c,active);events=[];
    m.reg('EAX',np);m.call(0x4327d0);assert.equal(c.pf_action(f,3),1);compare(label);
    setTimer(state+4,0x944,30);assert.equal(m.call(0x431070,{args:[np],limit:4000000}),1);assert.equal(c.pf_action(f,1),1);compare(label+' game over');++frames;++scenarios;
    for(const overlay of [0,1]){o.update(overlay);assert.equal(c.anm_manager_update(manager,overlay),1);}
   }
   }finally{c.pf_delete(f);c.anm_manager_delete(manager);}
  }
  for(const [a,d]of anms){c.anm_delete(a);c.release(d);}c.sht_delete(sht);c.release(sd);console.log(name+' player lifecycle verified');
 }report('player-frame',{passed:true,scenarios,frames,states,shots,animations,scope:'Original 431070 frame, 432a90 hit and 4327d0 death with real movement, options, shot pool, damage areas and ANM execution. Six shot configurations, three initial clock rates, entry, deathbomb rescue, missed deathbomb, drops, respawn, shooting suppression and cutscene. Bullet/laser cancellation, bomb start, item spawning, HUD, audio and replay are recorded integration boundaries, not full-game verification.'});
 }finally{m.close();}
});

