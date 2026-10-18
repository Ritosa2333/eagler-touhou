import test from 'node:test';
import assert from 'node:assert/strict';
import {oracle,core,memory,report} from './helpers.mjs';

test('TH09 bullet transformations match original activation and frame helpers',async()=>{
    const m=await oracle(),c=await core(),f=c.bullet_create(),bullet=m.allocate(0x10c4),manager=m.allocate(0x25e200),owner=m.allocate(64),player=m.allocate(0x2000),sprite=m.allocate(80);
    const fields=Array.from({length:c.bullet_field_count()},(_,i)=>Array.from({length:3},(_,j)=>new DataView(c.memory.buffer).getUint32(c.bullet_fields()+i*12+j*4,true)));
    const events=[],children=[];let checks=0;
    m.u32(manager+0x25e190,owner);m.u32(owner+4,player);m.f32(sprite+0x34,16);m.f32(sprite+0x30,24);c.bullet_cull_size(f,16,24);
    for(let type=0;type<5;++type){m.u32(manager+type*0xd48+0x224,sprite);m.i32(manager+type*0xd48+0xd44,type*23+7);}
    const event=(kind,a=0,b=0,x=0)=>{const e=Buffer.alloc(16);e.writeUInt32LE(kind);e.writeInt32LE(a,4);e.writeInt32LE(b,8);e.writeFloatLE(x,12);events.push(e);};
    m.replace(0x43e2f0,'bullet-sound',()=>{const a=m.readWords(m.reg('ESP')+4,2);event(2,a[0],a[1]);return 0;},2);
    m.replace(0x43e380,'positioned-bullet-sound',()=>{const p=m.reg('ESP')+4;event(3,m.i32(p),0,m.f32(p+4));return 0;},2);
    m.replace(0x436ac0,'bullet-type-animation',()=>{const [vm,color]=m.readWords(m.reg('ESP')+4,2),type=(m.i32(vm+0xd44)-7)/23;event(4,type,color);m.f32(vm+0x2c,8*(color%4+1));return 0;},2);
    m.replace(0x4130f0,'bullet-children',()=>{children.push(m.bytes(m.u32(m.reg('ESP')+4),0x210));event(5);return 0;},1);
    const handlers=[[1,0x413460],[0x10,0x4134d0],[0x20,0x413590],[0x40,0x413630],[0x100,0x413700],[0x80,0x4137d0],[0xc00,0x4138c0],[0x400000,0x4139f0],[0x800000,0x413a70]].map(([flag,address])=>{
        const stub=m.allocate(32),code=Buffer.alloc(10);code.set([0x56,0x89,0xce,0xe8]);code.writeInt32LE(address-(stub+8),4);code.set([0x5e,0xc3],8);m.write(stub,code);return [flag,stub];
    });
    function verify(label){
        for(const [original,offset,size] of fields)assert.deepEqual(memory(c,c.bullet_state(f)+offset,size),m.bytes(bullet+original,size),label+' field '+original.toString(16));
        assert.equal(c.bullet_event_count(f),events.length,label+' events');assert.deepEqual(memory(c,c.bullet_events(f),events.length*16),new Uint8Array(Buffer.concat(events)),label+' event payloads');
        assert.equal(c.bullet_children_count(f),children.length);children.forEach((e,i)=>assert.deepEqual(memory(c,c.bullet_children(f)+i*0x210,0x210),e,label+' child '+i));++checks;
    }
    function run(rate,flags,update,label){
        events.length=0;children.length=0;m.f32(0x4b36b8,rate);m.u32(0x4b36d4,flags);
        if(!update)m.call(0x4141c0,{ecx:bullet});
        else {
            for(const [flag,stub] of handlers)if(m.u32(bullet+0xdb4)&flag)m.call(stub,{ecx:bullet});
            if(m.u32(bullet+0xdb4)&0x20000){if(m.i32(bullet+0x106c)<=0)m.u32(bullet+0xdb4,m.u32(bullet+0xdb4)^0x20000);else m.call(0x406670,{ecx:bullet+0x1064});}
        }
        assert.equal(c.bullet_extra_run(f,rate,flags,update),1);verify(label);
    }
    try{
        for(const flag of [1,2,0x10,0x20,0x40,0x80,0x100,0x400,0x800,0x2000,0x4000,0x20000,0x40000,0x80000,0x400000,0x800000,0x1000000])for(let sample=0;sample<18;++sample){
            const raw=Buffer.alloc(0x10c4),start=sample%3,offset=0xdd8+start*24;
            raw.writeUInt32LE(manager);raw.writeUInt32LE(sprite,0x228);raw.writeFloatLE([8,16,32][sample%3],0x30);raw.writeInt32LE(7,0xd48);
            raw.writeFloatLE([-200,0,160,400][sample%4],0xd4c);raw.writeFloatLE([-40,200,500][sample%3],0xd50);raw.writeFloatLE(.1,0xd54);
            for(const [off,value] of [[0xd58,1.5],[0xd5c,-.375],[0xd60,.03125],[0xd70,2.75],[0xd7c,-.875],[0xfcc,.125],[0xfd0,.0625]])raw.writeFloatLE(value,off);
            raw.writeInt16LE(1,0xdbe);raw.writeInt32LE(sample%2?31:-1,0xdd0);raw.writeInt32LE(start,0xdd4);raw.writeUInt32LE(sample%5?0xffffffff:0,0xdb8);
            raw.writeFloatLE(sample%2?1.625:-1,offset);raw.writeFloatLE(sample%3?-.25:-999,offset+4);raw.writeInt32LE(3,offset+8);raw.writeInt32LE(2,offset+12);raw.writeUInt32LE(flag,offset+16);raw.writeInt32LE(sample%2,offset+20);
            if(flag===0x1000000){
                raw.writeUInt32LE(((sample%2?0x82030400:0x02030400)|7)>>>0,offset+8);raw.writeInt32LE(9,offset+12);
                raw.writeFloatLE(.875,offset+24);raw.writeFloatLE(.0625,offset+28);raw.writeInt32LE(3,offset+32);raw.writeInt32LE(0x200,offset+36);
            }
            m.write(bullet,raw);for(const [original,local,size] of fields)memory(c,c.bullet_state(f)+local,size).set(raw.subarray(original,original+size));
            const pos=Buffer.alloc(12);pos.writeFloatLE(67.5);pos.writeFloatLE(370.25,4);m.write(player+0x1b88,pos);memory(c,c.bullet_player(f),12).set(pos);
            const rate=[1,.5,.99][sample%3],flags=sample%4===0?32:0;
            for(let frame=0;frame<28;++frame){run(rate,flags,0,`activate ${flag.toString(16)}/${sample}/${frame}`);run(rate,flags,1,`update ${flag.toString(16)}/${sample}/${frame}`);}
        }
        report('bullet-extras',{passed:true,checks,scope:'Original extra-program activation plus all numeric update helpers. Child allocation, animation type changes and sounds are explicit boundaries; full manager collision lifecycle is separate.'});
    }finally{c.bullet_delete(f);m.close();}
});
