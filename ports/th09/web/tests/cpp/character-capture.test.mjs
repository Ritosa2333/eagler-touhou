import test from 'node:test';
import assert from 'node:assert/strict';
import {oracle,core,memory,report} from './helpers.mjs';
test('All original character capture shapes preserve geometric boundaries',async()=>{
    const m=await oracle(),c=await core(),player=m.allocate(0x2000),effect=m.allocate(0xd8),target=m.allocate(12),p=c.allocate(36);m.u32(player+0x368,effect);
    let seed=0x7845183,checks=0;const next=()=>seed=(Math.imul(seed,1664525)+1013904223)>>>0;const random=()=>next()/0x100000000;
    try{
        for(let character=0;character<16;++character){const address=m.u32(0x4a1a50+character*4);
            for(let sample=0;sample<2048;++sample){
                const px=15+random()*300,py=50+random()*340,r=8+random()*160,angle=(random()*2-1)*Math.PI,spread=.15+random()*3;
                const tx=px+(random()*4-2)*r,ty=py+(random()*4-2)*r;
                const values=[px,py,.25,tx,ty,0,r,angle,spread],b=Buffer.alloc(36);values.forEach((v,i)=>b.writeFloatLE(v,i*4));memory(c,p,36).set(b);m.write(player+0x1b88,b.subarray(0,12));m.write(target,b.subarray(12,24));m.write(effect+0x7c,b.subarray(24,36));
                const expected=m.call(address,{ecx:player,edx:target}),actual=c.capture_contains(character,p,p+12,p+24);assert.equal(actual,expected,`capture ${character}/${sample} ${JSON.stringify(values)}`);++checks;
            }
            for(const radius of [0,14,32,64,96])for(const dx of [-96,-64,-32,-14,0,14,32,64,96])for(const dy of [-96,-64,-32,-14,0,14,32,64,96]){
                const b=Buffer.alloc(36);[128,320,0,128+dx,320+dy,0,radius,-Math.PI/2,Math.PI/2].forEach((v,i)=>b.writeFloatLE(v,i*4));memory(c,p,36).set(b);m.write(player+0x1b88,b.subarray(0,12));m.write(target,b.subarray(12,24));m.write(effect+0x7c,b.subarray(24,36));
                assert.equal(c.capture_contains(character,p,p+12,p+24),m.call(address,{ecx:player,edx:target}),`boundary ${character}/${radius}/${dx}/${dy}`);++checks;
            }
        }
        report('character-capture-shapes',{checks,characters:16,scope:'Exact original capture predicates at sampled interior, exterior and boundary positions; all character shape callbacks.'});
    }finally{c.release(p);m.close();}
});
test('Captured spirit velocities, timers and enemy integration match original float state',async()=>{
    const m=await oracle(),c=await core(),f=c.ecl_vm_create(),enemy=m.allocate(0x6000);const base=c.ecl_vm_base(f),dv=new DataView(c.memory.buffer);
    const fields=Array.from({length:c.ecl_var_field_count()},(_,i)=>Array.from({length:4},(_,k)=>dv.getUint32(c.ecl_var_fields()+i*16+k*4,true))),extras=Array.from({length:c.ecl_vm_extra_count()},(_,i)=>Array.from({length:3},(_,k)=>dv.getUint32(c.ecl_vm_extras()+i*12+k*4,true)));
    const field=offset=>{const n=fields.findIndex(v=>v[0]===0&&v[1]===offset);assert.ok(n>=0);return c.ecl_vm_field(f,n);};
    const extra=offset=>{const n=extras.find(v=>v[0]===offset);assert.ok(n);return base+n[1];};
    const rngIndex=fields.findIndex(v=>v[0]===5&&v[1]===0x4ace0c),rng=c.ecl_vm_field(f,rngIndex);let checks=0;
    function setU(offset,value){dv.setUint32(field(offset),value>>>0,true);m.u32(enemy+offset,value);}
    try{
        for(let character=0;character<16;++character)for(const rate of [1,.5,.99])for(const captured of [false,true]){
            for(const off of [0x2d74,0x2db0]){memory(c,field(off),12).fill(0);m.view(enemy+off,12).fill(0);}
            for(const off of [0x2d98,0x2da4,0x5424]){memory(c,extra(off),12).fill(0);m.view(enemy+off,12).fill(0);}
            const initial=Buffer.alloc(12);[127.25,345.875,.5].forEach((v,i)=>initial.writeFloatLE(v,i*4));memory(c,field(0x2d74),12).set(initial);m.write(enemy+0x2d74,initial);
            setU(0x3380,captured?0x1000:0);c.ecl_vm_set_flags(f,character%2?0x8000:0,0);m.u32(enemy+0x337c,character%2?0x8000:0);
            const speed=Buffer.alloc(12);[1.125,-.375,.25].forEach((v,i)=>speed.writeFloatLE(v,i*4));memory(c,c.ecl_vm_motion(f,1),12).set(speed);m.write(enemy+0x2d8c,speed);
            dv.setUint32(rng,0xb361,true);dv.setUint32(rng+4,0,true);m.write(0x4ace0c,memory(c,rng,8));m.f32(0x4b36b8,rate);
            for(let frame=0;frame<256;++frame){
                if(frame===100){dv.setFloat32(extra(0x5424)+4,120.375,true);dv.setInt32(extra(0x5424)+8,120,true);m.write(enemy+0x5424,memory(c,extra(0x5424),12));}
                m.call(m.u32(0x4a1a90+character*4),{edx:enemy});assert.equal(c.capture_motion(f,character,rate),1);
                m.call(0x40f9b0,{ecx:enemy});c.enemy_integrate(f,rate);
                for(const off of [0x2d74,0x2db0])assert.deepEqual(memory(c,field(off),12),m.bytes(enemy+off,12),`motion ${character}/${rate}/${captured}/${frame}/${off.toString(16)}`);
                for(const off of [0x2d98,0x2da4,0x5424])assert.deepEqual(memory(c,extra(off),12),m.bytes(enemy+off,12),'motion extra '+off.toString(16));
                assert.deepEqual(memory(c,rng,8),m.bytes(0x4ace0c,8),'motion RNG');++checks;
            }
        }
        report('character-capture-motion',{checks,characters:16,scope:'All original captured-spirit velocity callbacks and position integration, including fractional timer boundary and mirrored normal movement.'});
    }finally{c.ecl_vm_delete(f);m.close();}
});
