import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';import {animationOracle} from './anm-oracle.mjs';import {normalizeAnimation,firstDifference} from './shot-oracle.mjs';
test('TH11 rectangle conversion executes original effects, point items and ECL 19a',async()=>{
 const c=await core(),m=await oracle(),o=animationOracle(m),{source,original}=o.load('bullet.anm');
 const data=c.allocate(source.length);memory(c,data,source.length).set(source);
 const bm=m.allocate(0x46d680),items=m.allocate(0x265e70),spell=m.allocate(0x900),lm=m.allocate(0x480),ne=m.allocate(0x2678),nc=m.allocate(0x1024),ni=m.allocate(32);
 m.u32(0x4a8d68,bm);m.u32(0x4a8e90,items);m.u32(0x4a8d6c,spell);m.u32(0x4a8e94,lm);const safeHeap=m.heap;
 const ef=c.enemy_fixture_create(),enemy=c.enemy_fixture_data(ef,0),context=c.allocate(0x1024),instruction=c.allocate(32);
 const dv=()=>new DataView(c.memory.buffer),put=(p,v)=>dv().setUint32(p,v,true),flt=(p,v)=>dv().setFloat32(p,v,true),read=p=>dv().getUint32(p,true);
 const sprites=[];for(let chunk=0,n=0;;){const count=source.readUInt16LE(chunk+4);for(let j=0;j<count;++j){const q=chunk+source.readUInt32LE(chunk+64+j*4);sprites.push({index:n++,width:source.readFloatLE(q+12)});}const next=source.readUInt32LE(chunk+36);if(!next)break;chunk+=next;}
 const choose=[-1,...[8,16,32,64].map(width=>sprites.find(s=>s.width===width)?.index).filter(x=>x!==undefined)];
 const normalizedItem=input=>{const b=Buffer.from(input);for(const off of [4,16,0x68,0xcc,0x118,0x144,0x190,0x1cc,0x218,0x244,0x270,0x29c,0x38c,0x3b0,0x45c])b.writeUInt32LE(0,off);const start=b.readUInt32LE(0x3a4);for(const off of[0x394,0x3a4,0x3a8])if(b.readUInt32LE(off))b.writeUInt32LE((b.readUInt32LE(off)-start)>>>0,off);b.writeUInt32LE(b.readUInt32LE(0x3ac)?1:0,0x3ac);return b;};
 let f=0,cases=0,bulletChecks=0,itemChecks=0,animationChecks=0;
 try{for(let sample=0;sample<36;++sample){
  if(f)c.cx_delete(f);f=c.cx_create();assert.equal(c.cx_load(f,data,source.length),1);m.heap=safeHeap;o.reset();for(const[p,n]of[[bm,0x46d680],[items,0x265e70],[spell,0x900],[lm,0x480]])m.view(p,n).fill(0);m.u32(bm+0x46d674,original);
  const am=c.cx_anm(f),center=sample%3===0?[0,0]:sample%3===1?[0,224]:[.125,111.875],size=sample%3===0?[0,0]:sample%3===1?[384,448]:[35.75,89.125],flags=sample>=24?1:0,id=157+sample%6,reward=sample%2,command=sample%4===3;
  for(const[j,value]of[...center,...size].entries()){flt(c.cx_rectangle(f)+j*4,value);m.f32(bm+(j<2?0x44:0x48)+j*4,value);}m.u32(spell+0x8e0,flags);m.i32(spell+0x8dc,id);
  const left=center[0]-size[0]/2,right=center[0]+size[0]/2,top=center[1]-size[1]/2,bottom=center[1]+size[1]/2;
  for(let k=0;k<2;++k){memory(c,c.anm_env_rng(am,k),8).fill(0);dv().setUint16(c.anm_env_rng(am,k),12345,true);}
  for(let j=0;j<2000;++j){const b=Buffer.alloc(0x910),sprite=choose[j%choose.length],width=sprite<0?0:sprites[sprite].width,colors=width>32?4:width>16?8:16;
   const p=[[left-2,center[1]],[left-2.0001,center[1]],[right+2,center[1]],[right+2.0001,center[1]],[center[0],top-2],[center[0],top-2.0001],[center[0],bottom+2],[center[0],bottom+2.0001],center][j%9];
   b.writeUInt32LE(j%16);b.writeInt32LE(j%4,4);b.writeFloatLE(p[0],0x43c);b.writeFloatLE(p[1],0x440);b.writeFloatLE(j%3,0x444);b.writeFloatLE(4,0x45c);b.writeFloatLE(4,0x460);b.writeInt16LE([1,2,0,3,4][Math.floor(j/9)%5],0x4b2);b.writeUInt32LE(123,0x4bc);b.writeInt16LE(j%colors,0x90e);
   memory(c,c.cx_bullet(f,j),b.length).set(b);assert.equal(c.cx_bullet_sprite(f,j,sprite),1);if(sprite>=0)b.writeUInt32LE(m.u32(original+0x118)+sprite*0x48,0x3b4);m.write(bm+0x64+j*0x910,b);
  }
  if(command){const ins=Buffer.alloc(32);ins.writeUInt16LE(0x19a,4);ins.writeUInt16LE(32,6);ins[10]=255;memory(c,instruction,32).set(ins);memory(c,context,0x1024).fill(0);put(context+4,instruction);m.write(ne,Buffer.from(memory(c,enemy,0x2678)));m.u32(ne,0x494074);m.u32(ne+4,nc);m.view(nc,0x1024).fill(0);m.u32(nc+4,ni);m.u32(nc+0x1014,ne);m.u32(ne+0x2650,ne);m.write(ni,ins);m.resetThreadFPU();assert.equal(c.cx_enemy_command(f,ef,context,flags,id),m.call(0x412e30,{ecx:ne+0x103c,limit:50000000})|0);}
  else{assert.equal(c.cx_cancel_rectangle(f,reward,flags,id),1);m.call(0x40b3a0,{args:[reward],limit:50000000});}
  ++cases;
  for(let j=0;j<2000;++j){const a=Buffer.from(memory(c,c.cx_bullet(f,j),0x910)),b=Buffer.from(m.bytes(bm+0x64+j*0x910,0x910));for(const x of[a,b])x.writeUInt32LE(x.readUInt32LE(0x3b4)?1:0,0x3b4);assert.equal(firstDifference(a,b),'',`sample ${sample} bullet ${j}`);++bulletChecks;}
  for(let j=0;j<2198;++j){const p=c.cx_item(f,j),q=items+0x14+j*0x478;if(read(p+0x464)||m.u32(q+0x464)){assert.equal(firstDifference(normalizedItem(memory(c,p,0x478)),normalizedItem(m.bytes(q,0x478))),'',`sample ${sample} item ${j}`);++itemChecks;}}
  for(const[k,off]of[[0,0x265e68],[1,0x265e6c],[2,0x265e64]])assert.equal(c.cx_value(f,k),m.u32(items+off));
  const states=o.states();assert.equal(c.anm_manager_count(am),states.length);
  for(const{id,p,bytes}of states){const q=c.anm_manager_find(am,id);assert.ok(q);assert.equal(firstDifference(normalizeAnimation(memory(c,q,0x434),read,read(q+0x3a4)),normalizeAnimation(bytes,p=>m.u32(p),m.u32(p+0x3a4))),'',`sample ${sample} effect ${id}`);++animationChecks;}
  for(let k=0;k<2;++k)assert.deepEqual(Buffer.from(memory(c,c.anm_env_rng(am,k),8)),Buffer.from(m.bytes(k?0x4c2ef8:0x4c2f00,8)));
 }
 report('rectangle-cancellation',{passed:true,cases,bulletChecks,itemChecks,animationChecks,scope:'Native 40b3a0 and ECL19a: all 2000 bullet slots, inclusive rectangle boundaries, zero/default rectangle, flags/state/delays, protected spells, sprite-width palettes, actual ANM and item pool/RNG. ECL laser branch uses an empty manager; populated laser cancellation has separate coverage.'});
 }finally{if(f)c.cx_delete(f);c.enemy_fixture_delete(ef);for(const p of[data,context,instruction])c.release(p);m.close();}
});
