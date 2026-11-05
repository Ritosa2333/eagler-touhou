import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync,readdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {oracle,core,memory,report,root} from './helpers.mjs';
test('Every ANM texture retains original upload dimensions, packed pixels and creation format',async()=>{
 const m=await oracle(),c=await core(),f=c.texture_image_fixture(),data=c.allocate(16000000),device=m.allocate(4),deviceTable=m.allocate(128),surface=m.allocate(4),surfaceTable=m.allocate(80),texture=m.allocate(4),textureTable=m.allocate(80),output=m.allocate(4);
 const arg=i=>m.u32(m.reg('ESP')+4+i*4),codes=[0,21,25,23,20,26],storage=[1,1,5,3,0,6],bytesPer=[4,4,2,2,3,2];let w=0,h=0,format=0,pitch=0,pixels=0,created=null,copies=0,checks=0,totalBytes=0,unusual=[];
 m.u32(device,deviceTable);m.u32(surface,surfaceTable);m.u32(texture,textureTable);m.u32(0x4b3108,device);
 const method=(table,offset,count,name,handler)=>m.u32(table+offset,m.registerImport({dll:'texture-boundary',name,argc:count,handler}));
 method(deviceTable,0x6c,5,'temporary-surface',()=>{w=arg(1);h=arg(2);format=arg(3);const index=codes.indexOf(format),row=w*bytesPer[index];assert.ok(index>0);pitch=(row+15)&~15;pixels=m.allocate(pitch*h);m.view(pixels,pitch*h).fill(0xcc);m.u32(arg(4),surface);return 0;});
 method(surfaceTable,0x24,4,'lock',()=>{m.u32(arg(1),pitch);m.u32(arg(1)+4,pixels);return 0;});method(surfaceTable,0x28,1,'unlock',()=>0);method(surfaceTable,8,1,'release',()=>0);method(textureTable,0x3c,3,'level',()=>{assert.equal(arg(1),0);m.u32(arg(2),surface);return 0;});
 m.replace(0x42d2b0,'display-format',()=>0);
 m.replace(0x452390,'GPU-create',()=>{created=Array.from({length:7},(_,i)=>arg(i));m.u32(arg(7),texture);return 0;},8);
 m.replace(0x4524ae,'GPU-upload',()=>{assert.deepEqual(Array.from({length:8},(_,i)=>arg(i)),[surface,0,0,surface,0,0,3,0]);++copies;return 0;},8);
 const heap=m.heap;
 try{for(const file of readdirSync(resolve(root,'reference/assets')).filter(n=>n.endsWith('.anm')).sort()){
  const raw=readFileSync(resolve(root,'reference/assets',file));memory(c,data,raw.length).set(raw);let at=0,index=0;
  for(;;){const embedded=raw[at+52]!==0,offset=raw.readUInt32LE(at+48),desired=raw.readUInt32LE(at+20),width=raw.readUInt32LE(at+12),height=raw.readUInt32LE(at+16);m.heap=heap;created=null;
   assert.equal(c.texture_image_load(f,data,raw.length,index,0),1,file+'/'+index);
   if(embedded){const size=raw.readUInt32LE(at+offset+12)+16,source=m.allocate(size);m.write(source,raw.subarray(at+offset,at+offset+size));assert.equal(m.call(0x43c4c0,{args:[output,source,desired]}),0);
    const sourceFormat=raw.readUInt16LE(at+offset+6);assert.equal(sourceFormat,desired,'shipped pixel and texture encodings match');const row=w*bytesPer[sourceFormat],expected=Buffer.alloc(row*h);for(let y=0;y<h;++y)expected.set(m.bytes(pixels+y*pitch,row),y*row);
    assert.equal(c.texture_image_value(f,0),w);assert.equal(c.texture_image_value(f,1),h);assert.equal(c.texture_image_value(f,2),row);assert.equal(c.texture_image_value(f,3),storage[sourceFormat]);assert.deepEqual(memory(c,c.texture_image_pixels(f),expected.length),new Uint8Array(expected),file+'/'+index+' original upload');totalBytes+=expected.length;
    assert.deepEqual(created,[device,w,h,1,0,codes[desired],1],'native texture request');if(w!==width||h!==height)unusual.push({file,index,declared:[width,height],actual:[w,h]});
   }else{assert.equal(m.call(0x43bba0,{args:[output,width,height,desired]}),0);assert.deepEqual(created,[device,width,height,1,0,codes[desired],1]);assert.equal(c.texture_image_value(f,0),width);assert.equal(c.texture_image_value(f,1),height);assert.ok(memory(c,c.texture_image_pixels(f),c.texture_image_value(f,4)).every(v=>v===0),'empty atlas cleared');}
   ++checks;++index;const next=raw.readUInt32LE(at+56);if(!next)break;at+=next;
  }
 }report('texture-image',{checks,copies,totalBytes,unusual,scope:'All 870 original texture loader requests and 865 packed pixel uploads, including padded locks and four embedded images larger than their ANM declarations. GPU allocation capabilities and color conversion in optional low-color display mode are not exercised by this test.'});
 }finally{c.texture_image_delete(f);c.release(data);m.close();}
});
