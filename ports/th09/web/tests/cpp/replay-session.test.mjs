import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,oracle,root,memory,report} from './helpers.mjs';
const inputBase=0x4ace18,inputStride=0x8e,inputSize=88;
const u16=(m,p,v)=>v===undefined?new DataView(m.view(p,2).buffer,m.view(p,2).byteOffset,2).getUint16(0,true):m.write(p,new Uint8Array(new Uint16Array([v]).buffer));
const inputsEqual=(m,c,p,label)=>{for(let side=0;side<3;++side)assert.deepEqual(memory(c,p+side*inputSize,inputSize),m.bytes(inputBase+side*inputStride,inputSize),label+' side '+side);};

test('TH09 simulation input and capture match the original, including repeat and counter wrap',async()=>{
    const m=await oracle(),c=await core(),p=c.allocate(inputSize*3),capture=c.allocate(4),rng=c.allocate(8),event=c.allocate(4),manager=m.allocate(0x164),config=m.allocate(204);
    m.u32(0x4a7e78,config);let random=0x62be7091,checks=0;
    const next=()=>random=(Math.imul(random,1664525)+1013904223)>>>0;
    try{
        for(let frame=0;frame<10000;++frame){
            const focus=frame%4;
            for(let side=0;side<3;++side){
                const bytes=Uint8Array.from({length:inputSize},()=>next()>>>24);
                // Include long holds, alternating edges, and unsigned-duration overflow.
                if(frame%3===0)bytes.fill(0xff,56);
                memory(c,p+side*inputSize,inputSize).set(bytes);m.write(inputBase+side*inputStride,bytes);
                if(side<2)m.write(config+0xb4+side,[Number(Boolean(focus&(1<<side)))]);
            }
            const seed=next()&0xffff,callCount=next(),ev=(next()&3)-1;
            u16(m,0x4ace0c,seed);m.u32(0x4ace10,callCount);m.i32(0x4a7ec0,ev);
            const view=new DataView(c.memory.buffer);view.setUint16(rng,seed,true);view.setUint32(rng+4,callCount,true);view.setInt32(event,ev,true);
            c.input_capture(capture,p,focus,rng,event);assert.equal(m.call(0x420190,{ecx:manager}),1);
            inputsEqual(m,c,p,'capture '+frame);assert.deepEqual(memory(c,capture,4),m.bytes(manager+0x160,4));
            assert.equal(view.getUint32(rng+4,true),m.u32(0x4ace10));assert.equal(view.getInt32(event,true),m.i32(0x4a7ec0));++checks;
        }
        report('game-input',{passed:true,checks,originalCallbacks:['0x4200f0','0x420190'],comparisonBytes:inputSize*3});
    }finally{for(const x of [p,capture,rng,event])c.release(x);m.close();}
});

test('TH09 all demonstration input streams advance frame-for-frame like the original',async()=>{
    const m=await oracle(),c=await core(),r=c.replay_create(),player=c.replay_play_create(),p=c.allocate(1000000),inputs=c.allocate(inputSize*3),settings=c.allocate(256);
    const manager=m.allocate(0x164),config=m.allocate(204),p1=m.allocate(32),p2=m.allocate(32),dialogue=m.allocate(0xe950);
    m.u32(0x4a7e78,config);m.u32(0x4a7dac,p1);m.u32(0x4a7de4,p2);m.u32(0x4a7e38,dialogue);
    m.u32(0x4a7ea8,2);m.u32(0x4a7e8c,9);const results=[];
    try{
        for(let index=0;index<3;++index){
            const name=`demorpy${index}.rpy`,source=readFileSync(resolve(root,'reference/assets',name));
            memory(c,p,source.length).set(source);assert.equal(c.replay_decode(r,p,source.length),1);
            assert.equal(c.replay_play_begin(player,r,9,settings),1);const count=c.replay_play_field(player,1);
            const raw=m.allocate(source.length);m.write(raw,source);const decoded=m.call(0x4205e0,{ecx:raw,edx:source.length,limit:200000000});
            m.u32(manager+8,decoded);m.call(0x420840,{ecx:manager});
            assert.deepEqual(memory(c,settings+40,204),m.bytes(config,204));
            const sv=new DataView(c.memory.buffer);
            for(let side=0;side<2;++side){
                const at=settings+side*20,global=0x4a7db0+side*0x38,state=side?p2:p1;
                assert.equal(sv.getUint32(at,true),m.u32(state+8));assert.equal(sv.getUint8(at+6),m.u32(global));
                assert.equal(sv.getUint8(at+7),m.u32(global+8));assert.equal(sv.getUint16(at+10,true),m.u32(global+12));
                assert.equal(sv.getUint8(at+8),m.f32(state));
            }
            assert.equal(sv.getUint16(settings+4,true),u16(m,0x4ace0c));
            memory(c,inputs,inputSize*3).fill(0);for(let side=0;side<3;++side)m.view(inputBase+side*inputStride,inputSize).fill(0);
            u16(m,0x4b36c8,0);m.i32(0x4b3880,0);let ticks=0;
            while(c.replay_play_field(player,0)<count){
                const flags=ticks%103===0?0:ticks%107===0?0x204:4,focus=(ticks>>>7)&3;
                for(let side=0;side<2;++side)m.write(config+0xb4+side,[Number(Boolean(focus&(1<<side)))]);
                m.u32(0x4a7ec4,flags);c.replay_play_step(player,flags,inputs,focus);assert.equal(m.call(0x4204b0,{ecx:manager}),1);
                inputsEqual(m,c,inputs,name+' tick '+ticks);
                assert.equal(c.replay_play_field(player,0),m.u32(manager));
                assert.equal(c.replay_play_field(player,3),u16(m,0x4b36c8));assert.equal(c.replay_play_field(player,4),m.i32(0x4b3880));
                const state=(ticks%6)-3;m.i32(dialogue+0xe94c,state);
                assert.equal(c.replay_play_after(player,flags,state),m.call(0x420470,{ecx:manager}));++ticks;
            }
            assert.equal(c.replay_play_step(player,4,inputs,3),2,'bounded end of replay');
            results.push({name,frames:count,callbackTicks:ticks,frameRateSamples:c.replay_play_field(player,2),allThreeInputsEqual:true});
        }
        report('replay-playback',{passed:true,files:results,scope:'Input playback callbacks and initialization; full battle replay is not yet integrated.'});
    }finally{c.replay_delete(r);c.replay_play_delete(player);for(const x of [p,inputs,settings])c.release(x);m.close();}
});

test('TH09 recording preserves original frame order across allocation chunks and match ending',async()=>{
    const m=await oracle(),c=await core(),r=c.record_create(),inputs=c.allocate(inputSize*3),out=c.allocate(100000),manager=m.allocate(0x164);
    m.replace(0x47b24e,'new-chunk',()=>m.allocate(m.u32(m.reg('ESP')+4)));
    const chunk=manager+0x1c;m.u32(manager+0x10c,chunk);
    for(let group=0;group<4;++group){const q=m.allocate(group===3?121:7200);m.u32(manager+12+group*4,q);m.u32(chunk+group*4,q);}
    let ticks=0,random=0x891045e1;
    try{
        for(;ticks<11420;++ticks){
            const flags=ticks>=11410?0x204:ticks%211===0?0:4,paused=ticks%59===0,cpu=(ticks>>>9)&3,rate=(ticks*17)&255;
            m.u32(0x4a7ec4,flags);m.u32(0x4b36d4,paused?8:0);u16(m,0x4b36c8,rate);
            for(let side=0;side<3;++side){
                random=(Math.imul(random,1103515245)+12345)>>>0;const keys=random>>>16;
                new DataView(c.memory.buffer).setUint16(inputs+side*inputSize+44,keys,true);u16(m,inputBase+side*inputStride+44,keys);
                if(side<2)m.u32(0x4a7db8+side*0x38,Number(Boolean(cpu&(1<<side))));
            }
            c.record_step(r,flags,paused,inputs,cpu,rate);assert.equal(m.call(0x4202a0,{ecx:manager}),1);
            assert.equal(c.record_field(r,0),m.u32(manager));assert.equal(c.record_field(r,1),m.u32(manager+4));
        }
        let chunks=0;for(let q=chunk;q;q=m.u32(q+20))++chunks;assert.equal(c.record_field(r,2),chunks);assert.ok(chunks>=4);
        const lengths=[];
        for(let group=0;group<4;++group){
            const expected=[];for(let q=chunk;q;q=m.u32(q+20))expected.push(m.bytes(m.u32(q+4*group),group===3?Math.trunc(m.u32(q+16)/30):m.u32(q+16)*2));
            const native=Buffer.concat(expected),size=c.record_stream(r,group,out,100000);assert.equal(size,native.length);
            assert.deepEqual(memory(c,out,size),new Uint8Array(native),'group '+group);lengths.push(size);
        }
        report('replay-recording',{passed:true,ticks,frames:c.record_field(r,0),chunks,endingFrames:c.record_field(r,1),streamBytes:lengths,byteExact:true});
    }finally{c.record_delete(r);c.release(inputs);c.release(out);m.close();}
});
