import test from 'node:test';
import assert from 'node:assert/strict';
import {oracle,core,memory,report} from './helpers.mjs';

test('TH09 twelve native ECL callbacks preserve attacks, both bullet fields and RNG order',async()=>{
    const m=await oracle(),c=await core(),fixture=c.special_create(),enemy=m.allocate(0x3400),manager=m.allocate(0x400),context=m.allocate(0x230);
    const owners=[m.allocate(64),m.allocate(64)],pools=[m.allocate(0x25e200),m.allocate(0x25e200)],instruction=m.allocate(24),p=c.allocate(24);
    const callbacks=[0x40c3c0,0x40c3f0,0x40c530,0x40c440,0x40c4a0,0x40c470,0x40c770,0x40c7c0,0x40c810,0x40c860,0x40c4d0,0x40c500],events=[];
    const dv=new DataView(c.memory.buffer),fields=Array.from({length:c.special_bullet_field_count()},(_,i)=>Array.from({length:3},(_,j)=>dv.getUint32(c.special_bullet_fields()+i*12+j*4,true)));
    m.u32(enemy,manager);m.u32(enemy+0x2ce0,context);owners.forEach((owner,i)=>m.u32(owner+8,pools[i]));
    const event=(type,kind,side)=>{const e=Buffer.alloc(40);e.writeUInt32LE(type);e.writeInt32LE(kind,4);e.writeInt32LE(side,8);events.push(e);return e;};
    m.replace(0x4151f0,'cross-field-attack',()=>{
        const a=m.readWords(m.reg('ESP')+4,4),e=event(1,a[0],a[2]);e.set(m.bytes(a[1],12),12);
        if(a[3]){e.writeUInt32LE(1,24);e.set(m.bytes(a[3],12),28);}return 0;
    },4);
    m.replace(0x43e2f0,'sound-event',()=>{const [sound,pan]=m.readWords(m.reg('ESP')+4,2);event(2,sound,pan);return 0;},2);
    m.replace(0x436ac0,'animation-sprite-boundary',()=>{
        const [vm,sprite]=m.readWords(m.reg('ESP')+4,2),bullet=vm-4,side=bullet>=pools[1]?1:0,index=(bullet-pools[side]-0x1a900)/0x10c4;
        assert.ok(Number.isInteger(index)&&index>=0&&index<536);event(3,sprite,side*536+index);
        m.f32(bullet+0x30,[16,32,8][sprite%3]);return 0;
    },2);
    let checks=0,bulletChecks=0;
    try{
        for(let callback=0;callback<12;++callback)for(let sample=0;sample<(callback===2?8:32);++sample){
            const side=sample&1,ins=Buffer.alloc(24);ins.writeInt16LE(136,4);ins.writeInt16LE(24,6);ins.writeInt32LE(callback,12);ins.writeInt32LE(sample%4-1,16);
            m.write(instruction,ins);memory(c,p,24).set(ins);
            m.u32(manager+0x31c,side);m.u32(manager+0x320,owners[side]);m.u32(manager+0x324,owners[side^1]);
            for(let n=0;n<2;++n){const flags=(0xabcdef00+sample*7+n)>>>0;m.u32(owners[n]+0x34,flags);c.special_set_flags(fixture,n,flags);}
            const position=Buffer.alloc(12),locals=Buffer.alloc(120),seed=Buffer.alloc(8);
            [sample*.7-110,sample*3.15+17,sample*.0625].forEach((v,i)=>position.writeFloatLE(v,i*4));
            for(let n=0;n<30;++n)locals.writeFloatLE((sample-8)*.375+n,4*n);
            seed.writeUInt16LE((sample*671+21345)&65535);seed.writeUInt32LE(sample*133,4);
            for(const [part,address,bytes] of [[0,enemy+0x2d74,position],[1,context+0x1c,locals],[2,0x4ace0c,seed]]){m.write(address,bytes);memory(c,c.special_part(fixture,part),bytes.length).set(bytes);}
            if(callback===2)for(let s=0;s<2;++s)for(let n=0;n<536;++n){
                const raw=Buffer.alloc(0x10c4);raw.writeUInt32LE(pools[s]);
                for(const off of [0xd4c,0xd50,0xd54,0xd58,0xd5c,0xd60,0xd70,0xd74,0xd78,0xd7c,0xd80,0xd84])raw.writeFloatLE((n+sample)*.071+off*.002,off);
                raw.writeFloatLE([8,16,24,32][(n+sample)%4],0x30);raw.writeInt32LE([0,12,63,342,343,400][(n+sample)%6],0xd48);
                raw.writeInt16LE([0,1,2,5,6][(n+sample)%5],0xdbe);raw.writeInt16LE(17,0x10c0);raw.writeInt16LE(4,0x10c2);
                for(const off of [0xdb4,0xdb8,0xdd4,0xdd0])raw.writeInt32LE(n+sample,off);
                for(let j=0;j<108;++j)raw.writeUInt32LE(((j*14315387)^(n*61319))>>>0,0xdd8+j*4);
                m.write(pools[s]+0x1a900+n*0x10c4,raw);const b=c.special_bullet(fixture,s,n);
                for(const [original,offset,size] of fields)memory(c,b+offset,size).set(raw.subarray(original,original+size));
            }
            events.length=0;m.call(callbacks[callback],{ecx:enemy,edx:instruction});assert.equal(c.special_run(fixture,callback,side,p),1);
            assert.equal(c.special_event_count(fixture),events.length,`callback ${callback}/${sample} events`);
            assert.deepEqual(memory(c,c.special_events(fixture),events.length*40),new Uint8Array(Buffer.concat(events)),`callback ${callback}/${sample} events`);
            assert.deepEqual(memory(c,c.special_part(fixture,2),8),m.bytes(0x4ace0c,8),`callback ${callback}/${sample} RNG`);
            for(let s=0;s<2;++s)assert.equal(c.special_flags(fixture,s)>>>0,m.u32(owners[s]+0x34));
            if(callback===2)for(let s=0;s<2;++s)for(let n=0;n<536;++n){
                const b=c.special_bullet(fixture,s,n),raw=pools[s]+0x1a900+n*0x10c4;
                for(const [original,offset,size] of fields)assert.deepEqual(memory(c,b+offset,size),m.bytes(raw+original,size),`callback 2 sample ${sample} bullet ${s}/${n} field ${original.toString(16)}`);
                ++bulletChecks;
            }
            ++checks;
        }
        report('ecl-native-callbacks',{passed:true,checks,bulletChecks,callbacks,scope:'Native callback logic. Attack allocation, sound and sprite changes use recorded boundary events; sprite widths are supplied by a deterministic animation boundary.'});
    }finally{c.release(p);c.special_delete(fixture);m.close();}
});
