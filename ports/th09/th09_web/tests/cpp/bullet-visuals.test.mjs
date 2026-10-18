import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {oracle,core,memory,bits,root,report} from './helpers.mjs';
import {installAnm,normalizedAnm} from './anm-oracle.mjs';

test('TH09 bullet creation integrates all original etama templates and real animations',async()=>{
    const m=await oracle(),c=await core(),f=c.bullet_visuals_create(),bytes=readFileSync(resolve(root,'reference/assets/etama.anm'));
    const anm=installAnm(m,bytes),manager=m.allocate(0x25e200),owner=m.allocate(64),player=m.allocate(0x2400),params=m.allocate(0x214),p=c.allocate(Math.max(bytes.length,0x210));
    const fields=Array.from({length:c.bullet_field_count()},(_,i)=>Array.from({length:3},(_,j)=>new DataView(c.memory.buffer).getUint32(c.bullet_fields()+i*12+j*4,true)));
    m.u32(manager+0x25e190,owner);m.u32(owner+4,player);m.f32(0x4b36b8,1);m.u32(0x4b36d4,0);m.u32(0x4ace0c,0x1234);m.u32(0x4ace10,0);
    memory(c,c.bullet_visuals_part(f,3),8).set(m.bytes(0x4ace0c,8));m.replace(0x401660,'get-etama-resource',()=>anm.file,1);
    let checks=0,animationChecks=0;
    function compareAnimation(actual,expected,label){
        const a=normalizedAnm(memory(c,actual,0x2a4),c.bullet_visuals_part(f,0),c.bullet_visuals_part(f,1),c.bullet_visuals_part(f,2));
        const b=normalizedAnm(m.bytes(expected,0x2a4),anm.file,anm.source,anm.sprites);
        const difference=[];for(let i=0;i<a.length;++i)if(a[i]!==b[i])difference.push(i);
        assert.equal(difference.length,0,label+' offsets '+difference.slice(0,16).map(n=>n.toString(16)));++animationChecks;
    }
    try{
        memory(c,p,bytes.length).set(bytes);assert.equal(c.bullet_visuals_load(f,p,bytes.length),1);assert.equal(m.call(0x413e40,{ecx:manager}),0);m.call(0x412470,{ecx:manager});
        for(let type=0;type<23;++type){
            for(let kind=0;kind<5;++kind)compareAnimation(c.bullet_visuals_template(f,type,kind),manager+type*0xd48+kind*0x2a4,`template ${type}/${kind}`);
            const metadata=c.bullet_visuals_metadata(f,type);assert.deepEqual(memory(c,metadata,15),m.bytes(manager+type*0xd48+0xd34,15));assert.deepEqual(memory(c,metadata+16,4),m.bytes(manager+type*0xd48+0xd44,4));
        }
        const selected=new Set();
        for(let n=0;n<368;++n){
            const type=n%23,color=Math.floor(n/23)%(type===21?3:4),phase=Math.floor(n/92),second=n%2,rate=[1,.5,.99][n%3];
            const payload=Buffer.alloc(0x210);payload.writeInt16LE(type);payload.writeInt16LE(color,2);
            payload.writeFloatLE((n%13-6)*12.25,4);payload.writeFloatLE(n%100+130.5,8);payload.writeFloatLE(.875,16);payload.writeFloatLE(.125,20);payload.writeFloatLE(2.125,24);payload.writeFloatLE(.5,28);
            payload.writeInt16LE(7,0x1f4);payload.writeInt16LE(3,0x1f6);payload.writeInt16LE(n%9,0x1f8);payload.writeUInt32LE([0,2,4,8][phase],0x1fc);payload.writeInt32LE(-1,0x204);payload.writeUInt32LE(n,0x20c);
            m.write(params,payload);m.u32(params+0x210,manager+type*0xd48);memory(c,p,0x210).set(payload);m.f32(0x4b36b8,rate);
            const original=m.call(0x412960,{ecx:manager,args:[params,n%7,n%3,bits(-.25),second]}),slot=(original-manager-0x1a900)/0x10c4;
            assert.equal(c.bullet_visuals_spawn(f,p,n%7,n%3,-.25,second,rate),slot,`creation ${n} type ${type} color ${color}`);selected.add(slot);
            const b=c.bullet_manager_at(f,slot);
            for(const [raw,local,size] of fields){if(raw===0x224){assert.equal(new DataView(c.memory.buffer).getUint32(b+local,true),m.u32(original+raw)?1:0);continue;}
                assert.deepEqual(memory(c,b+local,size),m.bytes(original+raw,size),`creation ${n} field ${raw.toString(16)}`);}
            for(let kind=0;kind<5;++kind)compareAnimation(c.bullet_visuals_instance(f,slot,kind),original+4+kind*0x2a4,`creation ${n}/${kind}`);
            assert.deepEqual(memory(c,c.bullet_visuals_part(f,3),8),m.bytes(0x4ace0c,8));++checks;
        }
        // Run the original whole-manager callback with real ANM state. Collision
        // is disabled here to isolate the render-independent animation lifecycle.
        for(const slot of selected){m.view(manager+0x1a900+slot*0x10c4+0x10bc,1)[0]=1;const field=fields.find(x=>x[0]===0x10bc);memory(c,c.bullet_manager_at(f,slot)+field[1],1)[0]=1;}
        for(let frame=0;frame<45;++frame){
            const rate=frame<20?.5:1;m.f32(0x4b36b8,rate);m.u32(0x4a7ec4,0);assert.equal(m.call(0x4146f0,{ecx:manager}),1);assert.equal(c.bullet_visuals_frame(f,rate,0,0,0,frame),1);
            for(const slot of selected){const original=manager+0x1a900+slot*0x10c4,b=c.bullet_manager_at(f,slot);
                for(const [raw,local,size] of fields){if(raw===0x224){assert.equal(new DataView(c.memory.buffer).getUint32(b+local,true),m.u32(original+raw)?1:0);continue;}
                    assert.deepEqual(memory(c,b+local,size),m.bytes(original+raw,size),`frame ${frame} slot ${slot} field ${raw.toString(16)}`);}
                for(let kind=0;kind<5;++kind)compareAnimation(c.bullet_visuals_instance(f,slot,kind),original+4+kind*0x2a4,`frame ${frame}/${slot}/${kind}`);
            }
            assert.deepEqual(memory(c,c.bullet_visuals_part(f,3),8),m.bytes(0x4ace0c,8));++checks;
        }
        report('bullet-visuals',{passed:true,checks,animationChecks,templates:23,source:'etama.anm',scope:'Real ANM template initialization, full bullet creation including pool exhaustion, and whole-manager animation updates. Player collisions disabled; no GPU rendering.'});
    }finally{c.bullet_visuals_delete(f);c.release(p);m.close();}
});
