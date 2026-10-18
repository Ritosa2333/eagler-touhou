import test from'node:test';import assert from'node:assert/strict';import{readFileSync}from'node:fs';import{resolve}from'node:path';
import{core,oracle,memory,report,root}from'./helpers.mjs';import{animationOracle}from'./anm-oracle.mjs';import{normalizeAnimation,firstDifference}from'./shot-oracle.mjs';
test('TH11 all ending scripts and staff roll execute like the original interpreter',async()=>{
 const c=await core(),m=await oracle(),o=animationOracle(m),text=o.load('text.anm'),textResource=c.anm_create(),bytes=c.allocate(text.source.length);memory(c,bytes,text.source.length).set(text.source);assert.equal(c.anm_open(textResource,bytes,text.source.length),1);c.release(bytes);m.u32(text.original,0);m.u32(0x4c3808,text.original);
 const raw=readFileSync(resolve(root,'../[th11] 东方地灵殿 (汉化版+日文版)/th11.dat')),data=c.allocate(raw.length),source=c.resources_create();memory(c,data,raw.length).set(raw);assert.equal(c.resources_open(source,data,raw.length),1);
 const heap=m.heap,read=p=>new DataView(c.memory.buffer).getUint32(p,true),put=(p,v)=>new DataView(c.memory.buffer).setUint32(p,v,true),stack=n=>m.u32(m.reg('ESP')+n*4),string=p=>{const b=Buffer.from(m.bytes(p,512));return b.subarray(0,b.indexOf(0)).toString('latin1');},cpString=p=>{const b=Buffer.from(memory(c,p,512));return b.subarray(0,b.indexOf(0)).toString('latin1');};
 let texts=[],sounds=[],music=-1,fade=-1,pending=false,scriptBase=0,staffBase=0,native=0;const loaded=new Map();
 m.replace(0x454c60,'ending glyph boundary',()=>{texts.push([m.u32(m.reg('ESI')),stack(1),stack(3),stack(4),m.reg('EDI'),string(stack(5))]);return 0;});
 m.replace(0x44a1e0,'ending sound sink',()=>{sounds.push(m.reg('ESI'));return 0;});
 m.replace(0x42a0d0,'ending BGM',()=>{music=Number(string(stack(2)).match(/_(\d+)\.wav$/)?.[1]??-1);return 0;},2);
 m.replace(0x42a150,'music metadata',()=>{music=stack(2);return 0;},2);m.replace(0x42a270,'music fade',()=>{fade=Math.round(m.f32(m.reg('ESP')+4)*60);return 0;},1);
 m.replace(0x4489e0,'fade rendering boundary',()=>0,5);m.replace(0x40fca0,'loading indicator boundary',()=>0,2);m.replace(0x4549a0,'preloaded ANM retirement boundary',()=>0);
 m.replace(0x4594a0,'preloaded ending ANM completion',()=>{pending=true;return 0;},1);
 m.replace(0x40eb70,'staff script resource boundary',()=>{assert.equal(string(m.reg('EAX')),'staff.msg');scriptBase=staffBase;return staffBase;});
 let cases=0,frames=0,checks=0;
 try{for(const callback of[false,true])for(let index=0;index<12;++index)for(const seen of[0,3]){
  m.heap=heap;o.reset();loaded.clear();const file=`e${String(index).padStart(2,'0')}`;for(const name of[file+'.anm','staff.anm']){const r=o.load(name,true);m.u32(r.original,28);loaded.set(name,r.original);}
  const b=readFileSync(resolve(root,'reference/assets',file+'.msg')),staff=readFileSync(resolve(root,'reference/assets/staff.msg'));scriptBase=m.allocate(b.length);m.write(scriptBase,b);staffBase=m.allocate(staff.length);m.write(staffBase,staff);
  native=m.allocate(0xf0);m.view(native,0xf0).fill(0);const owner=m.allocate(0x30);m.view(owner,0x30).fill(0);m.u32(0x4a8d78,owner);m.u32(owner+0x20,seen);m.u32(owner+0x18,native);m.u32(0x4a5720,index%3+1);
  const a=c.anm_manager_create(),s=c.score_file_create(),p=c.ending_create(a,textResource,s);for(let i=0;i<2;++i){put(c.anm_env_rng(a,i),12345);put(c.anm_env_rng(a,i)+4,0);}
  memory(c,c.score_file_data(s,7)+0x16+index,1)[0]=seen&1?0:1;memory(c,c.score_file_data(s,7)+0x22,1)[0]=seen&2?0:1;
  assert.equal(c.ending_begin(p,source,index%6,index%3+1,index<6?1:0),1,cpString(c.ending_data(p,6,0)));
  m.call(0x40f1d0,{args:[native,scriptBase+b.readUInt32LE(4)]});pending=false;
  const compare=label=>{
   for(const[k,off]of[[0,0x18],[1,0x2c]])assert.deepEqual(Buffer.from(memory(c,c.ending_data(p,k,0),12)),Buffer.from(m.bytes(native+off,12)),label+' timer'+k);
   for(const[k,off]of[[2,0x40],[3,0x90]])assert.deepEqual(Buffer.from(memory(c,c.ending_data(p,k,0),k===2?20:64)),Buffer.from(m.bytes(native+off,k===2?20:64)),label+' handles'+k);
   for(const[k,off]of[[1,0x74],[2,0x78],[3,0x7c]])assert.equal(c.ending_value(p,k)>>>0,m.u32(native+off),label+' state'+k);
   assert.equal(c.ending_value(p,4),m.u32(native+0x54)-scriptBase,label+' instruction');
   assert.deepEqual(Array.from({length:c.ending_value(p,5)},(_,i)=>{const q=c.ending_data(p,4,i);return [read(q),read(q+4),read(q+8),read(q+12),read(q+16),cpString(c.ending_data(p,5,i))];}),texts,label+' glyph requests');
   assert.deepEqual(Array.from({length:c.ending_value(p,8)},(_,i)=>read(c.ending_data(p,7,0)+i*4)),sounds,label+' sounds');assert.equal(c.ending_value(p,6),music,label+' BGM');assert.equal(c.ending_value(p,7),fade,label+' fade');
   o.update(false);assert.equal(c.anm_manager_update(a,0),1);const states=o.states();assert.equal(c.anm_manager_count(a),states.length,label+' ANM count');for(const vm of states){const cp=c.anm_manager_find(a,vm.id);assert.ok(cp);assert.equal(firstDifference(normalizeAnimation(memory(c,cp,0x434),read,read(cp+0x3a4)),normalizeAnimation(vm.bytes,p=>m.u32(p),m.u32(vm.p+0x3a4))),'',label+' VM'+vm.id);++checks;}
  };
  try{let done=false;for(let frame=0;frame<24000;++frame){
   if(pending){const slot=m.u32(native+0xec),name=string(m.u32(native+0x70));assert.ok(loaded.has(name));m.u32(native+0x80+slot*4,loaded.get(name));m.u32(native+0x74,m.u32(native+0x74)&~4);pending=false;}
   texts=[];sounds=[];music=fade=-1;const held=seen?0:512,pressed=frame%17===0?1:0;m.u32(0x4c92a8,held);m.u32(0x4c92b4,pressed);let result;
   if(callback){m.u32(0x4c37d8,0);do{m.reg('EDI',owner);result=m.call(0x40f120);}while(result===6);assert.equal(c.ending_tick(p,held,pressed),1,cpString(c.ending_data(p,6,0)));}
   else{result=m.call(0x40f4a0,{args:[native]});assert.equal(c.ending_update(p,held,pressed),1,cpString(c.ending_data(p,6,0)));}
   if(callback?m.u32(0x4c37d8)!==0:result===0xffffffff){assert.equal(c.ending_value(p,0),0);done=true;break;}compare(`${file} callback${callback} seen${seen} frame${frame}`);++frames;
  }assert.ok(done,`${file} ending did not finish`);++cases;
  }finally{c.ending_delete(p);c.score_file_delete(s);c.anm_manager_delete(a);}
 }report('ending',{passed:true,cases,frames,checks,scope:'All 12 original ending scripts plus staff roll, both interpreter and 40f120 callback repeat/skip timing; waits, text requests, full ANM state and audio cues. ANM loading is a controlled preloaded resource boundary; screen fades and glyph pixels tested separately.'});
 }finally{c.resources_delete(source);c.release(data);c.anm_delete(textResource);m.close();}
});
