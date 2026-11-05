import test from 'node:test';import assert from 'node:assert/strict';
import {readdirSync} from 'node:fs';import {resolve} from 'node:path';
import {core,oracle,memory,report,root} from './helpers.mjs';import {animationOracle} from './anm-oracle.mjs';
test('TH11 shipped animation scripts match original manager frames and random streams',async()=>{
 const c=await core(),m=await oracle(),o=animationOracle(m);let checks=0,scripts=0,files=0;
 function normalize(bytes,read,base){const b=Buffer.from(bytes);for(const off of [0x68,0xcc,0x118,0x144,0x190,0x1cc,0x218,0x244,0x270,0x29c,0x38c,0x3b0])b.writeUInt32LE(0,off);
  for(const off of [4,16,0x1c]){const p=b.readUInt32LE(off);if(p)b.writeUInt32LE(read(p),off);}
  for(const off of [8,12,20,24]){const p=b.readUInt32LE(off);if(p)b.writeUInt32LE(read(read(p)),off);}
  for(const off of [0x394,0x3a4,0x3a8]){const p=b.readUInt32LE(off);if(p)b.writeUInt32LE((p-base)>>>0,off);}
  for(const off of [0x3ac,0x400,0x410,0x428,0x42c])b.writeUInt32LE(b.readUInt32LE(off)?1:0,off);
  // Native ripple draw callback becomes a semantic draw-mode dispatch.
  b.writeUInt32LE(0,0x414);return b;
 }
 try{for(const name of readdirSync(resolve(root,'reference/assets')).filter(n=>n.endsWith('.anm')&&(!process.env.TH11_ANM_FILE||n===process.env.TH11_ANM_FILE)).sort()){
  const {original,source,heap}=o.load(name),data=c.allocate(source.length),resource=c.anm_create();memory(c,data,source.length).set(source);assert.equal(c.anm_open(resource,data,source.length),1,name);
  for(let script=0;script<c.anm_count(resource,2);++script){if(process.env.TH11_ANM_SCRIPT&&script!==+process.env.TH11_ANM_SCRIPT)continue;
   let rootMode=0;const scriptBytes=Buffer.from(memory(c,c.anm_script_bytes(resource,script),c.anm_script_size(resource,script)));for(let off=0;off+8<=scriptBytes.length;){const op=scriptBytes.readInt16LE(off),size=scriptBytes.readUInt16LE(off+2);if(op===68&&scriptBytes[off+8]>=29)rootMode=1;if(op===-1||size<8)break;off+=size;}
   m.heap=heap;m.view(heap,Math.min(8000000,0x07000000-heap-16)).fill(0);o.reset();const manager=c.anm_manager_create();
   for(const v of [0,1]){const r=c.anm_env_rng(manager,v);memory(c,r,8).fill(0);new DataView(c.memory.buffer).setUint16(r,12345,true);}
   const context=()=>`${name} script=${script}`;
   try{const p=c.anm_manager_spawn(manager,resource,script,0,rootMode);assert.ok(p,context()+' creation failed opcode='+c.anm_manager_error(manager));o.spawn(original,script,0,rootMode);
    for(let frame=-1;frame<+(process.env.TH11_ANM_FRAMES||120);++frame){
     if(frame>=0){for(const overlay of [0,1]){assert.equal(c.anm_manager_update(manager,overlay),1,context()+' update opcode='+c.anm_manager_error(manager));try{o.update(overlay);}catch(e){e.message=context()+` frame=${frame} layers=${o.states().map(v=>m.u32(v.p+0x20))}: `+e.message;throw e;}}}
     const native=o.states();assert.equal(c.anm_manager_count(manager),native.length,context()+` frame=${frame} active count`);
     const dv=()=>new DataView(c.memory.buffer),read=p=>dv().getUint32(p,true);
     for(const item of native){const actual=c.anm_manager_find(manager,item.id);assert.ok(actual,context()+' missing id '+item.id);
      const a=normalize(memory(c,actual,0x434),read,read(actual+0x3a4)),b=normalize(item.bytes,p=>m.u32(p),m.u32(item.p+0x3a4));
      let first=-1;for(let i=0;i<a.length;i+=4)if(a.readUInt32LE(i)!==b.readUInt32LE(i)){first=i;break;}
      assert.equal(first,-1,context()+` frame=${frame} id=${item.id} offset=0x${first.toString(16)} actual=${first<0?'':a.readUInt32LE(first).toString(16)} expected=${first<0?'':b.readUInt32LE(first).toString(16)}`);
      const geometry=read(actual+0x400);if(geometry){const mode=(read(actual+0x404)>>>22)&15,n=mode===10?0x4b0:read(actual+0x3b4)*56;assert.deepEqual(Buffer.from(memory(c,geometry,n)),Buffer.from(m.bytes(m.u32(item.p+0x400),n)),context()+` geometry frame=${frame}`);}
      ++checks;
     }
     for(const v of [0,1])assert.deepEqual(Buffer.from(memory(c,c.anm_env_rng(manager,v),8)),Buffer.from(m.bytes(v?0x4c2ef8:0x4c2f00,8)),context()+` RNG frame=${frame}`);
     if(native.length===0)break;
    }
   }finally{c.anm_manager_delete(manager);}++scripts;
  }
  c.anm_delete(resource);c.release(data);++files;console.log(name+' verified');
 }report('anm-shipped-scripts',{passed:true,checks,scripts,files,framesPerScript:+(process.env.TH11_ANM_FRAMES||120),scope:'original manager + interpreter, per-object state and geometry; GPU output tested separately'});
 }finally{m.close();}
});
