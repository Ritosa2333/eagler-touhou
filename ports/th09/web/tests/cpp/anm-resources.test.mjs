import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync,readdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {oracle,core,root,report} from './helpers.mjs';
const view=(c,p,n)=>new Uint8Array(c.memory.buffer,p,n);
function normalized(bytes,file,script,sprites){const copy=bytes.slice(),v=new DataView(copy.buffer);for(const [offset,base] of [[0x204,file],[0x21c,script],[0x220,script],[0x224,sprites],[0x234,script]]){const p=v.getUint32(offset,true);v.setUint32(offset,p?p-base+1:0,true);}return copy;}
function metadata(bytes){
    const v=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength),scripts=[],sprites=[];let base=0;
    for(;;){const ns=v.getUint32(base,true),nt=v.getUint32(base+4,true),width=v.getUint32(base+12,true),height=v.getUint32(base+16,true);
        for(let i=0;i<ns;i++){const p=base+v.getUint32(base+64+i*4,true);sprites.push({x:v.getFloat32(p+4,true),y:v.getFloat32(p+8,true),w:v.getFloat32(p+12,true),h:v.getFloat32(p+16,true),width,height});}
        for(let i=0;i<nt;i++)scripts.push(base+v.getUint32(base+64+ns*4+i*8+4,true));const next=v.getUint32(base+56,true);if(!next)break;base+=next;
    }return {scripts,sprites};
}
test('C++ parses and executes every original ANM script against original code',async()=>{
    const m=await oracle(),c=await core(),f=c.anm_create(),p=c.allocate(16*1024*1024),vm=m.allocate(0x2a4),manager=m.allocate(64),file=m.allocate(28),sprites=m.allocate(2*1024*1024),sourceSprite=m.allocate(68),script=m.allocate(16*1024*1024);let checks=0,totalScripts=0,totalSprites=0;const files=[];
    m.u32(0x4dc550,manager);m.u32(file,7);m.u32(file+4,script);m.u32(file+12,sprites);
    try{for(const name of readdirSync(resolve(root,'reference/assets')).filter(n=>n.endsWith('.anm')).sort()){
        const bytes=readFileSync(resolve(root,'reference/assets',name)),meta=metadata(bytes);view(c,p,bytes.length).set(bytes);m.reg('FPCW',0x7f);assert.equal(c.anm_load(f,p,bytes.length),1,'load '+name);assert.equal(c.anm_script_count(f),meta.scripts.length);assert.equal(c.anm_sprite_count(f),meta.sprites.length);m.write(script,bytes);
        for(let i=0;i<meta.sprites.length;i++){
            const s=meta.sprites[i],data=new Uint8Array(68),v=new DataView(data.buffer);v.setInt32(0,7,true);v.setFloat32(8,s.x,true);v.setFloat32(12,s.y,true);v.setFloat32(16,Math.fround(s.x+s.w),true);v.setFloat32(20,Math.fround(s.y+s.h),true);v.setFloat32(24,s.height,true);v.setFloat32(28,s.width,true);v.setFloat32(56,1,true);v.setFloat32(60,1,true);m.write(sourceSprite,data);m.call(0x43bcb0,{ecx:file,args:[i,sourceSprite]});
            assert.deepEqual(view(c,c.anm_sprites(f)+i*68,68),m.bytes(sprites+i*68,68),`${name} sprite ${i}`);totalSprites++;
        }
        for(let index=0;index<meta.scripts.length;index++){
            const offset=meta.scripts[index];assert.equal(c.anm_script_offset(f,index),offset);m.view(manager,64).fill(0);m.u32(0x4ace0c,0x1234);m.u32(0x4ace10,0);m.f32(0x4b36b8,1);m.u32(0x4b36d4,0);m.reg('FPCW',0x7f);m.call(0x403a10,{ecx:vm});
            try{assert.equal(c.anm_start_index(f,index,1),1);m.call(0x4395a0,{ecx:file,args:[vm,script+offset]});
                const compare=frame=>{assert.equal(c.anm_invalid(f),0,'invalid C++ script');const actual=normalized(view(c,c.anm_vm(f),0x2a4),c.anm_file(f),c.anm_raw(f),c.anm_sprites(f)),expected=normalized(m.bytes(vm,0x2a4),file,script,sprites);const differences=[];for(let k=0;k<actual.length;k++)if(actual[k]!==expected[k])differences.push(k);assert.equal(differences.length,0,`${name} script ${index} frame ${frame} offsets ${differences.slice(0,12).map(n=>n.toString(16))}`);assert.deepEqual(view(c,c.anm_rng(f),8),m.bytes(0x4ace0c,8));assert.equal(c.anm_count(f),m.u32(manager+16));checks++;};
                compare(-1);
                for(let frame=0;frame<90;frame++){const interrupt=frame===12?1:frame===35?2:0;if(interrupt)m.view(vm+0x1fe,2).set(new Uint8Array(new Int16Array([interrupt]).buffer));const actual=c.anm_step(f,interrupt),expected=m.call(0x436f30,{ecx:manager,args:[vm]});assert.equal(actual,expected);compare(frame);if(actual)break;}
                totalScripts++;
            }catch(error){error.message=`${name} script ${index}: ${error.message}`;throw error;}
        }
        files.push({name,scripts:meta.scripts.length,sprites:meta.sprites.length});
    }report('anm-resources',{passed:true,checks,totalScripts,totalSprites,files,framesPerScript:90,interrupts:[1,2],precision:32});}finally{c.anm_delete(f);c.release(p);m.close();}
});
