import test from 'node:test';
import assert from 'node:assert/strict';
import {readdirSync,readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,oracle,memory,report,root} from './helpers.mjs';
test('ANM v7 chained resources match original loader, sprite UVs and script tables',async()=>{
 const c=await core(),m=await oracle();let source,chunk,checks=0,sprites=0,scripts=0,textures=0;
 m.replace(0x46a720,'memset',()=>{const sp=m.reg('ESP'),p=m.u32(sp+4);m.view(p,m.u32(sp+12)).fill(m.u32(sp+8)&255);return p;});
 m.replace(0x45fce4,'operator new',()=>m.allocate(m.u32(m.reg('ESP')+4)));
 m.replace(0x456ad0,'diagnostic log',()=>0);
 m.replace(0x46025c,'sprintf filename',()=>{const sp=m.reg('ESP'),p=m.u32(sp+4),text=m.string(m.u32(sp+12));m.write(p,Buffer.from(text+'\0'));return text.length;});
 m.replace(0x458400,'read fixture file',()=>{const size=m.u32(m.reg('ESP')+4),p=m.allocate(source.length);m.write(p,source);if(size)m.u32(size,source.length);return p;},2);
 const vtable=m.allocate(0x60),texture=m.allocate(16);m.u32(texture,vtable);
 const desc=m.registerImport({dll:'fixture',name:'texture dimensions',argc:3,handler:()=>{const out=m.u32(m.reg('ESP')+12);m.view(out,32).fill(0);m.u32(out+24,source.readUInt16LE(chunk+10));m.u32(out+28,source.readUInt16LE(chunk+12));return 0;}});m.u32(vtable+0x44,desc);
 m.replace(0x453f80,'embedded texture upload',()=>{m.u32(m.reg('EDI'),texture);return 0;},2);
 m.replace(0x4540a0,'blank texture allocation',()=>{m.u32(m.reg('ESI'),texture);return 0;},1);
 m.replace(0x4540f0,'render target allocation',()=>{m.u32(m.reg('ESI'),texture);return 0;});
 const manager=m.allocate(0x435140),path=m.allocate(256),heap=m.heap;
 try{
  for(const name of readdirSync(resolve(root,'reference/assets')).filter(n=>n.endsWith('.anm')).sort()){
   source=readFileSync(resolve(root,'reference/assets',name));m.heap=heap;m.write(path,Buffer.from(name+'\0'));
   const p=c.allocate(source.length),a=c.anm_create();memory(c,p,source.length).set(source);
   assert.equal(c.anm_open(a,p,source.length),1,name);
   const original=m.call(0x454190,{ecx:path,args:[manager,0]});assert.ok(original,name+' original load');
   assert.equal(c.anm_count(a,0),m.u32(original+0x10c),name+' textures');
   assert.equal(c.anm_count(a,1),m.u32(original+0x114),name+' sprites');
   assert.equal(c.anm_count(a,2),m.u32(original+0x110),name+' scripts');
   let spriteStart=0,scriptStart=0,textureIndex=0;chunk=0;
   while(true){
    const ns=source.readUInt16LE(chunk+4),nc=source.readUInt16LE(chunk+6),nativeBase=m.u32(original+0x108);
    assert.equal(m.call(0x4545a0,{args:[original,textureIndex,spriteStart,scriptStart,nativeBase+chunk]}),1);
    for(let j=0;j<ns;j++){
     const n=spriteStart+j,s=c.anm_sprite(a,n),got=Buffer.from(memory(c,s,40)),expected=m.bytes(m.u32(original+0x118)+n*0x48,0x48),v=new DataView(expected.buffer);
     assert.equal(got.readUInt32LE(4),textureIndex,name+' texture index');
     for(const [g,e] of [[8,12],[12,16],[16,56],[20,52],[24,36],[28,40],[32,44],[36,48]])assert.equal(got.readUInt32LE(g),v.getUint32(e,true),`${name} sprite ${n} field ${g}`);
     ++sprites;checks+=9;
    }
    for(let j=0;j<nc;j++){
     const n=scriptStart+j,off=m.u32(m.u32(original+0x11c)+n*4)-nativeBase;
     assert.equal(c.anm_script_offset(a,n),off,name+' script offset');
     const size=c.anm_script_size(a,n);assert.deepEqual(Buffer.from(memory(c,c.anm_script_bytes(a,n),size)),source.subarray(off,off+size),name+' script bytes');
     ++scripts;checks+=2;
    }
    ++textures;++textureIndex;spriteStart+=ns;scriptStart+=nc;
    const next=source.readUInt32LE(chunk+36);if(!next)break;chunk+=next;
   }
   c.anm_delete(a);c.release(p);
  }
  report('anm-resource',{passed:true,checks,textures,sprites,scripts,graphics:'GPU allocation replaced only with dimension-preserving test doubles; original table and UV routines executed'});
 }finally{m.close();}
});
