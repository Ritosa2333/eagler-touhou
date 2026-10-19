import test from 'node:test';
import assert from 'node:assert/strict';
import {oracle,core,bits,floatWrapper,memory,report} from './helpers.mjs';

test('TH09 RNG streams and float results match original 1.50a',async()=>{
    const m=await oracle(),c=await core(),p=c.allocate(8),q=m.allocate(8);let checks=0;
    try{
        const floats=[0x42ae70,0x42aea0].map(a=>floatWrapper(m,a));
        for(const seed of [0,1,0x1234,0x9630,0xffff]){
            const state=new Uint8Array(new Uint32Array([seed|0xabcd0000,0xfffffff0]).buffer);
            m.write(q,state);memory(c,p,8).set(state);
            for(let i=0;i<4096;++i){
                const kind=i%3,max=[0,1,17,32768,65535][i%5];
                const expect=m.call([0x42ae20,0x42ae50,0x4048b0][kind],{ecx:q,args:kind===2?[max]:[]});
                assert.equal(c.rng_word(p,kind,max)>>>0,kind===0?expect&65535:expect,`rng ${seed}/${i}`);
                assert.deepEqual(memory(c,p,8),m.bytes(q,8));checks++;
            }
            for(let i=0;i<1024;++i){const kind=i%2;assert.equal(bits(c.rng_float(p,kind)),m.call(floats[kind],{ecx:q}));checks++;}
        }
        report('rng',{passed:true,checks,fpuControl:0x7f});
    }finally{c.release(p);m.close();}
});

test('TH09 timer matches original full-time storage and forced transitions',async()=>{
    const m=await oracle(),c=await core(),p=c.allocate(12),q=m.allocate(12);let checks=0;
    try{
        for(const start of [-199,-1,0,1,999999,2147483520,-2147483648])for(const rate of [0,0.01,1/3,0.5,0.99,1,1.5])for(const flags of [0,32]){
            c.timer_reset(p,start);m.call(0x4014d0,{ecx:q,args:[start]});assert.deepEqual(memory(c,p,12),m.bytes(q,12));
            m.f32(0x4b36b8,rate);m.u32(0x4b36d4,flags);
            for(let i=0;i<96;++i){
                const amount=[0,1,2,-1,0.01,-2.75,10.33][i%7];
                c.timer_advance(p,amount,rate,flags);m.call(0x42f490,{ecx:q,args:[bits(amount)]});
                assert.deepEqual(memory(c,p,12),m.bytes(q,12),`timer ${start}/${rate}/${flags}/${i}`);checks++;
            }
        }
        report('timer',{passed:true,checks,fpuControl:0x7f});
    }finally{c.release(p);m.close();}
});

test('TH09 two-player input edges and repeat cadence match the original',async()=>{
    const m=await oracle(),c=await core(),p=c.allocate(84);let checks=0;
    try{
        for(let frame=0;frame<8192;++frame)for(let player=0;player<2;++player){
            const keys=frame<256?(player?0xaaaa:0xffff):((frame*19+player*97)^(frame>>4))&0xffff;
            c.input_update(p+player*42,keys);m.call(0x42ace0,{ecx:player,edx:keys});
            assert.deepEqual(memory(c,p+player*42,42),m.bytes(0x4ace18+player*0x8e,42),`input ${frame}/${player}`);checks++;
        }
        // The 16-bit repeat counter wraps before its threshold comparison.
        memory(c,p,42).fill(255);m.write(0x4ace18,memory(c,p,42));
        c.input_update(p,65535);m.call(0x42ace0,{ecx:0,edx:65535});assert.deepEqual(memory(c,p,42),m.bytes(0x4ace18,42));checks++;
        report('input',{passed:true,checks,players:2});
    }finally{c.release(p);m.close();}
});

test('TH09 frame timer preserves the original inclusive slowdown threshold',async()=>{
    const m=await oracle(),c=await core(),p=c.allocate(12),q=m.allocate(12);let checks=0;
    try{
        for(const start of [-100,0,2,16777216,2147483520])for(const rate of [0,.5,.98999995,.99,.9900001,1,1.5]){
            c.timer_reset(p,start);m.call(0x4014d0,{ecx:q,args:[start]});m.f32(0x4b36b8,rate);
            for(let frame=0;frame<100;++frame){
                c.timer_tick(p,rate);m.call(0x4014b0,{ecx:q});
                assert.deepEqual(memory(c,p,12),m.bytes(q,12),`tick ${start}/${rate}/${frame}`);++checks;
            }
        }
        report('timer-tick',{passed:true,checks});
    }finally{c.release(p);m.close();}
});
