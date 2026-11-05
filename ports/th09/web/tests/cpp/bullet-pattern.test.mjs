import test from 'node:test';
import assert from 'node:assert/strict';
import {oracle,core,memory,bits,report} from './helpers.mjs';

test('TH09 bullet patterns match original creation angles, speeds, velocity and RNG',async()=>{
    const m=await oracle(),c=await core(),manager=m.allocate(0x25e200),bullet=manager+0x1a900;
    const params=m.allocate(0x214),sprite=m.allocate(80),p=c.allocate(0x210),rng=c.allocate(8),out=c.allocate(20);
    m.u32(manager+0x25e1b0,bullet);m.u32(manager+0x25e1b4,bullet);m.u32(params+0x210,manager);
    m.u32(manager+0x224,sprite);m.f32(sprite+0x30,16);
    m.replace(0x436ac0,'animation-set-sprite',()=>0,2);
    m.replace(0x4141c0,'bullet-extra-execution',()=>0);
    let checks=0;
    try{
        for(let pattern=0;pattern<9;++pattern)for(let sample=0;sample<80;++sample){
            const b=Buffer.alloc(0x210),count=sample%13+1,layers=sample%5+1,index=sample%count,layer=sample%layers;
            b.writeFloatLE((sample-37)*.127,0x10);b.writeFloatLE((sample-23)*.037,0x14);
            b.writeFloatLE((sample+3)*.113,0x18);b.writeFloatLE((sample-25)*.073,0x1c);
            b.writeInt16LE(count,0x1f4);b.writeInt16LE(layers,0x1f6);b.writeInt16LE(pattern,0x1f8);
            const aim=Math.fround((sample-40)*.17),rate=Math.fround([1,.5,.99][sample%3]);
            m.write(params,b);memory(c,p,b.length).set(b);m.write(bullet+0xdbe,[0,0]);m.f32(0x4b36b8,rate);
            const seed=Buffer.alloc(8);seed.writeUInt16LE((sample*653+5671)&65535);seed.writeUInt32LE(sample*19,4);
            m.write(0x4ace0c,seed);memory(c,rng,8).set(seed);
            assert.equal(m.call(0x412960,{ecx:manager,args:[params,index,layer,bits(aim),1]}),bullet);
            c.bullet_pattern_values(p,index,layer,aim,rate,rng,out);
            const expected=Buffer.concat([m.bytes(bullet+0xd7c,4),m.bytes(bullet+0xd70,4),m.bytes(bullet+0xd58,12)]);
            assert.deepEqual(memory(c,out,20),new Uint8Array(expected),`pattern ${pattern} sample ${sample}`);
            assert.deepEqual(memory(c,rng,8),m.bytes(0x4ace0c,8));++checks;
        }
        report('bullet-patterns',{passed:true,checks,patterns:9,scope:'Creation pattern arithmetic and initial velocity. Animation and bullet-extra execution are boundary stubs.'});
    }finally{for(const a of [p,rng,out])c.release(a);m.close();}
});
