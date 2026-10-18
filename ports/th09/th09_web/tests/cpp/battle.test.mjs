import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {oracle,core,memory,report,root} from './helpers.mjs';
import {installAnm,normalizedAnm} from './anm-oracle.mjs';

test('Two battle fields share original RNG, enemy scripts, shots, hazards and cross-field attacks',async()=>{
 const pairs=process.env.TH09_BATTLE_PAIR?[process.env.TH09_BATTLE_PAIR.split(',').map(Number)]:[[0,1],[2,3],[4,5],[6,7],[8,9],[10,11],[12,13],[14,15]],deepInterval=Number(process.env.TH09_BATTLE_DEEP_INTERVAL||60);
 const m=await oracle(),c=await core(),f=c.battle_fixture(),name=c.allocate(128),data=c.allocate(16000000),files=new Map();
 const source=n=>{if(!files.has(n))files.set(n,readFileSync(resolve(root,'reference/assets',n)));return files.get(n);};
 const add=n=>{const b=source(n);memory(c,name,128).fill(0);memory(c,name,128).set(new TextEncoder().encode(n));memory(c,data,b.length).set(b);c.battle_file(f,name,data,b.length);};
 const arg=i=>m.u32(m.reg('ESP')+4+i*4),alloc=n=>{const p=m.allocate(n);m.view(p,n).fill(0);return p;};
 let anms=[],scene=0,checks=0,frames=0;const originalHeap=m.heap;
 m.replace(0x401660,'battle-animation-resource',()=>{const a=anms[arg(0)];assert.ok(a,'animation slot '+arg(0));return a.file;},1);
 m.replace(0x42c970,'battle-file-read',()=>{const b=source(m.string(m.reg('ECX'),128)),p=alloc(b.length);m.write(p,b);return p;},1);
 for(const [address,argc] of [[0x43e380,2],[0x43e2f0,2],[0x41a270,0],[0x41a2d0,0],[0x41a320,1],[0x41a380,1],[0x41a3e0,1],[0x418950,1],[0x41a3c0,0],[0x4346a0,4],[0x43be50,0],[0x4011c0,0]])m.replace(address,'battle-presentation',()=>0,argc);
 m.replace(0x415d70,'battle-winner',()=>{m.u32(scene+0x1095c,1);m.u32(scene+0x11ea8,1);return 0;},1);
 m.replace(0x43ad40,'unused-render',()=>0,1);
 const nativeInput=side=>0x4ace18+side*0x8e;
 const mappings=(fn,count,width=3)=>Array.from({length:count()},(_,i)=>Array.from({length:width},(_,j)=>new DataView(c.memory.buffer).getUint32(fn()+i*width*4+j*4,true)));
 const motion=mappings(c.motion_fields,c.motion_field_count),control=mappings(c.shot_control_fields,c.shot_control_field_count),bulletFields=mappings(c.bullet_fields,c.bullet_field_count);
 const enemyFields=mappings(c.ecl_var_fields,c.ecl_var_field_count,4).filter(x=>x[0]===0),enemyExtras=mappings(c.ecl_vm_extras,c.ecl_vm_extra_count);
 const error=()=>{const p=c.battle_error(f),b=memory(c,p,256);return new TextDecoder().decode(b.subarray(0,b.indexOf(0)));};
 function compareAnimation(cpp,native,slot,label){const a=anms[slot],p=[0,1,2].map(i=>c.battle_resource(f,slot,i));assert.deepEqual(normalizedAnm(memory(c,cpp,676),...p),normalizedAnm(m.bytes(native,676),a.file,a.source,a.sprites),label);}
 try{
  for(const n of ['text.anm','ascii.anm','capture.anm','etama.anm','front.anm','world01.anm','enemy1.anm','enemy.ecl'])add(n);
  for(const [left,right] of pairs){
   const chars=[left,right];for(const ch of chars)for(const ext of ['anm','ecl','sht'])add('pl'+String(ch).padStart(2,'0')+'.'+ext);
   m.heap=originalHeap;anms=[];for(const [slot,n] of [[0,'text.anm'],[1,'ascii.anm'],[3,'capture.anm'],[4,'world01.anm'],[5,`pl${String(left).padStart(2,'0')}.anm`],[6,`pl${String(right).padStart(2,'0')}.anm`],[8,'etama.anm'],[9,'enemy1.anm'],[10,'front.anm']])anms[slot]=installAnm(m,source(n),slot);
   const players=[alloc(0x31000),alloc(0x31000)],bullets=[alloc(0x25e200),alloc(0x25e200)],enemies=[alloc(0x2ac450),alloc(0x2ac450)],effects=[alloc(0x2dc),alloc(0x2dc),alloc(0x2dc)],controllers=[alloc(0x1368),alloc(0x1368)],scores=[alloc(256),alloc(256)],backgrounds=[alloc(0x6600),alloc(0x6600)],huds=[alloc(0xac00),alloc(0xac00)],queue=alloc(0x4c80),config=alloc(512);scene=alloc(0x1293c);
   m.view(0x4a7d90,0x400).fill(0);m.view(0x4ace18,3*0x8e).fill(0);m.u32(0x4a7e38,scene);m.i32(scene+0xe94c,-1);m.u32(0x4a7e78,config);m.f32(0x4a7e7c,1);m.u32(0x4a7ea8,2);m.u32(0x4a7eac,1);m.u32(0x4a7e44,1);m.u32(0x4a7e48,2);m.u32(0x4a7eb0,255);m.u32(0x4a7ec4,4);m.u32(0x4b36d4,0);m.f32(0x4b36b8,1);m.call(0x41a9b8,{ecx:0x4a7d90});m.u32(0x4b36cc,anms[1].file);
   for(let s=0;s<2;++s){const own=0x4a7d90+s*0x38;m.u32(own,backgrounds[s]);m.u32(own+4,players[s]);m.u32(own+8,bullets[s]);m.u32(own+12,effects[s]);m.u32(own+16,enemies[s]);m.u32(own+20,controllers[s]);m.u32(own+24,huds[s]);m.u32(own+28,scores[s]);m.u32(own+32,chars[s]);m.u32(own+44,20);m.u32(huds[s]+0xa678,anms[5+s].file);m.u32(huds[s]+0xa668,anms[10].file);m.u32(0x4a8108+s*4,10);m.u32(0x4a8160+s*4,0);m.u32(0x4b3178+s*0xf0+0xcc,16+s*320);m.u32(0x4b3178+s*0xf0+0xd0,16);}
   m.u32(0x4ace0c,0x7531);m.u32(0x4ace10,0);
   for(let s=0;s<2;++s){const p=players[s],own=0x4a7d90+s*0x38;m.u32(p+8,s);m.u32(p+12,own);m.u32(p+16,0x4a7d90+(1-s)*0x38);m.call(0x401500,{ecx:p+0x303c8,args:[0]});assert.equal(m.call(0x41ee50,{ecx:p}),0);
    const b=bullets[s];m.u32(b+0x25e18c,s);m.u32(b+0x25e190,own);m.u32(b+0x25e194,0x4a7d90+(1-s)*0x38);assert.equal(m.call(0x413e40,{ecx:b}),0);m.call(0x412470,{ecx:b});
    const e=enemies[s];m.call(0x40fa70,{ecx:e});m.u32(e+0x31c,s);m.u32(e+0x320,own);m.u32(e+0x324,0x4a7d90+(1-s)*0x38);assert.equal(m.call(0x411db0,{ecx:e}),0);
    const a=controllers[s];m.u32(a,s);m.u32(a+4,own);m.u32(a+8,0x4a7d90+(1-s)*0x38);m.u32(a+20,anms[10].file);
   }
   for(let s=0;s<3;++s){const e=effects[s],capacity=s===2?800:256,reserved=s===2?1:7;m.u32(e+12,s);m.u32(e+16,0x4a7d90+s*0x38);m.u32(e+20,0x4a7d90+(1-s)*0x38);m.u32(e+0x30,alloc((capacity+reserved+1)*0xd8));m.u32(e+0x34,capacity);m.u32(e+0x38,reserved);m.u32(e+0x2d4,anms[8].file);}
   m.u32(0x4a7e0c,effects[2]);m.u32(0x4a7e3c,queue);m.call(0x415750,{ecx:queue});
   assert.equal(c.battle_initialize(f,left,right,0x7531,1,0),1,error());
   assert.deepEqual(memory(c,c.battle_part(f,0,6),8),m.bytes(0x4ace0c,8),'initial RNG');assert.deepEqual(memory(c,c.battle_part(f,0,16),256),m.bytes(0x4a7c88,256),'shared spawn patterns');
   for(let s=0;s<2;++s){m.i32(players[s],0);m.call(0x401500,{ecx:players[s]+0x303c8,args:[0]});m.f32(players[s]+0x1b88,0);m.f32(players[s]+0x1b8c,384);}
   function compare(label,deep=false){
    const values=new Uint32Array(c.memory.buffer,c.battle_values(f),34);assert.equal(values[2],m.u32(0x4a7ec4),label+' freeze');assert.equal(values[3],m.u32(scene+0x1095c),label+' phase');
    for(let s=0;s<2;++s){const p=players[s];for(const [kind,fields] of [[0,motion],[1,control]])for(const [original,local,size] of fields)assert.deepEqual(memory(c,c.battle_part(f,s,kind)+local,size),m.bytes(p+original,size),label+' player'+s+'/'+original.toString(16));
     assert.deepEqual(memory(c,c.battle_part(f,s,5),60),m.bytes(p+0x30414,60),label+' combo');compareAnimation(c.battle_part(f,s,2),p+0xc0,5+s,label+' body');
     for(let n=0;n<512;++n){const q=p+0x28bc+n*68,cpp=c.battle_part(f,s,9)+n*68;if(!m.bytes(q+60,1)[0]&&!memory(c,cpp+60,1)[0])continue;assert.deepEqual(memory(c,cpp,68),m.bytes(q,68),label+' area'+s+'/'+n);}
     const e=enemies[s],b=bullets[s],expected=[...m.readWords(e+0x2ac3ac,4),m.i32(e+0x2ac3e8),m.i32(e+0x2ac3f4),0,m.bytes(e+0x2ac3d4,1)[0],m.u32(b+0x25e198),m.u32(b+0x25e19c),m.u32(b+0x25e1a0),...m.readWords(p+0xb0,3),m.u32(scores[s]+8)];
     // Timeline cursor uses owned resource offsets in C++; compare the common
     // simulation counters here and full actor data at periodic checkpoints.
     for(const n of [4,5,7])assert.equal(values[4+s*15+n],expected[n]>>>0,label+' field'+s+' timeline'+n);
     for(let n=0;n<128;++n){const q=e+0x5758+n*0x5430,cpp=c.battle_enemy(f,s,n);if(!(m.u32(q+0x337c)&1))continue;for(const [original,local,size] of enemyExtras.filter(x=>[0x2dbc,0x2dc8,0x33a8,0x3398,0x338a,0x3386,0x53a8,0x2e5c].includes(x[0])))assert.deepEqual(memory(c,cpp+local,size),m.bytes(q+original,size),label+' enemy extra'+s+'/'+n+'/'+original.toString(16));}
     for(let n=0;n<128;++n){const q=e+0x5758+n*0x5430,cpp=c.battle_enemy(f,s,n),flags=enemyExtras.find(x=>x[0]===0x337c);assert.deepEqual(memory(c,cpp+flags[1],4),m.bytes(q+flags[0],4),label+' enemy'+s+'/'+n+' flags');if(!(m.u32(q+0x337c)&1))continue;for(const [,original,local,size] of enemyFields)assert.deepEqual(memory(c,cpp+local,size),m.bytes(q+original,size),label+' enemy'+s+'/'+n+'/'+original.toString(16));}
     for(const n of [0,1,2,3,11,12,13,14])assert.equal(values[4+s*15+n],expected[n]>>>0,label+' field'+s+' counter'+n);
     // Two alignment bytes before the C++ definition pointer have no state.
     for(let n=0;n<128;++n){const q=p+0xc11c+n*0x484,cpp=c.battle_shot(f,s,n);if(!m.bytes(q+0x462,2)[0]&&!memory(c,cpp+0x462,2)[0])continue;assert.deepEqual(memory(c,cpp+0x2a4,0x1ce),m.bytes(q+0x2a4,0x1ce),label+' shot'+s+'/'+n);compareAnimation(cpp,q,5+s,label+' shot ANM'+s+'/'+n);}
     if(deep)for(let n=0;n<536;++n){if(n===175)continue;const q=b+0x1a900+n*0x10c4,cpp=c.battle_bullet(f,s,n,0),state=bulletFields.find(x=>x[0]===0xdbe);assert.deepEqual(memory(c,cpp+state[1],2),m.bytes(q+0xdbe,2),label+' active bullet'+s+'/'+n);if(m.bytes(q+0xdbe,2).every(x=>x===0))continue;for(const [original,local,size] of bulletFields){if(original===0x224)continue;assert.deepEqual(memory(c,cpp+local,size),m.bytes(q+original,size),label+' bullet'+s+'/'+n+'/'+original.toString(16));}}
    }assert.deepEqual(memory(c,c.battle_part(f,0,6),8),m.bytes(0x4ace0c,8),label+' RNG');++checks;
   }
   compare('initial');
   for(let frame=0;frame<1500;++frame){let leftKeys=(frame%19<2?1:0)|(frame%120<60?0x40:0x80)|(frame%80<30?4:0),rightKeys=(frame%17<2?1:0)|(frame%140<70?0x80:0x40)|(frame%110<40?4:0);
    for(let s=0;s<2;++s)if(frame===760+s*45||frame===1050+s*45){const amount=frame<1000?400:300,field=control.find(x=>x[0]===0x30388);new DataView(c.memory.buffer).setFloat32(c.battle_part(f,s,1)+field[1],amount,true);m.f32(players[s]+0x30388,amount);if(s)rightKeys|=2;else leftKeys|=2;}
    assert.equal(c.battle_step(f,leftKeys,rightKeys,63),1,`C++ frame ${frame}: `+error());
    for(let s=0;s<2;++s)m.write(nativeInput(s),memory(c,c.battle_part(f,s,3),88));
    for(let s=0;s<2;++s)m.call(0x410730,{ecx:enemies[s]});for(let s=0;s<2;++s)m.call(0x4146f0,{ecx:bullets[s]});m.call(0x415340,{ecx:queue});for(let s=0;s<2;++s)m.call(0x41e900,{ecx:players[s]});for(let s=0;s<2;++s)m.call(0x4041f0,{ecx:controllers[s]});for(let s=0;s<3;++s)m.call(0x40cdd0,{ecx:effects[s]});
    compare(`characters ${left}/${right} frame ${frame}`,frame%deepInterval===0);++frames;
   }
  }
  report('battle',{checks,frames,pairs,deepInterval,scope:'Integrated dual-field native comparisons with original ANM, SHT and ECL resources. Human simulation inputs; scene, HUD, audio and rasterization are presentation boundaries.'});
 }finally{c.battle_delete(f);c.release(name);c.release(data);m.close();}
});
