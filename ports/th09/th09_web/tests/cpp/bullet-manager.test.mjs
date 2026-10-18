import test from 'node:test';
import assert from 'node:assert/strict';
import {oracle,core,memory,report} from './helpers.mjs';

test('TH09 bullet manager matches original animation phases, collision order and draw lists',async()=>{
    const m=await oracle(),c=await core(),manager=m.allocate(0x25e200),owner=m.allocate(64),player=m.allocate(0x2400),sprite=m.allocate(80);
    let f=0,testFrame=0;const events=[];
    m.f32(sprite+0x34,16);m.f32(sprite+0x30,24);
    const fields=Array.from({length:c.bullet_field_count()},(_,i)=>Array.from({length:3},(_,j)=>new DataView(c.memory.buffer).getUint32(c.bullet_fields()+i*12+j*4,true)));
    const at=index=>manager+0x1a900+index*0x10c4,indexOf=address=>(address-manager-0x1a900)/0x10c4;
    const event=(kind,a,b=0)=>{const e=Buffer.alloc(16);e.writeUInt32LE(kind);e.writeInt32LE(a,4);e.writeInt32LE(b,8);events.push(e);};
    m.replace(0x436f30,'bullet-animation',()=>{
        const address=m.u32(m.reg('ESP')+4),index=Math.floor(indexOf(address)),offset=address-at(index),kind=[4,0x2a8,0x54c,0x7f0,0xa94].indexOf(offset);
        assert.ok(kind>=0&&index>=0&&index<536);event(6,index,kind);return (testFrame+index+kind)%7===0?1:0;
    },1);
    m.replace(0x41e350,'probe-attacks',()=>{const index=indexOf(m.u32(m.reg('ESP')+12));event(7,index);return (testFrame+index)%3;},3);
    m.replace(0x41dff0,'collide-attacks',()=>{const index=indexOf(m.u32(m.reg('ESP')+12));event(8,index);return (testFrame+index*2)%3;},3);
    m.replace(0x40f8e0,'collide-player',()=>{event(9,indexOf(m.u32(m.reg('ESP')+12)));return 0;},3);
    m.replace(0x43e2f0,'bullet-sound',()=>{const [sound,pan]=m.readWords(m.reg('ESP')+4,2);event(2,sound,pan);return 0;},2);
    let checks=0;
    try{
        for(const rate of [1,.5,.99])for(const timingFlags of [0,32]){
            if(f)c.bullet_manager_delete(f);f=c.bullet_manager_create();m.view(manager,0x25e200).fill(0);
            m.u32(manager+0x25e190,owner);m.u32(owner+4,player);
            const selected=[];
            for(let n=0;n<72;++n){
                const index=(n*17)%536;if(index===175)continue;selected.push(index);
                const raw=Buffer.alloc(0x10c4);raw.writeUInt32LE(manager);raw.writeUInt32LE(sprite,0x228);raw.writeUInt32LE(n%2,0x224);
                raw.writeInt16LE(n%5+1,0xdbe);raw.writeInt32LE(-1,0xdd0);raw[0xd46]=n%6;raw[0x10bc]=n%4===0?1:0;raw[0xdc3]=n%3===0?1:0;raw[0xdc4]=n%2;
                raw.writeInt16LE(n%4,0xdc0);raw.writeUInt32LE(n%3===0?0x1000:0,0xdb8);raw.writeInt32LE(n%7,0xdb0);
                raw.writeFloatLE(n%9===0?170:-80+n*2,0xd4c);raw.writeFloatLE(n%7===0?470:100+n,0xd50);raw.writeFloatLE(.1,0xd54);
                raw.writeFloatLE((n%7-3)*.3,0xd58);raw.writeFloatLE((n%11+1)*.27,0xd5c);
                raw.writeFloatLE(2.75,0xd70);raw.writeFloatLE(.375,0xd7c);raw.writeFloatLE(16,0x30);
                if(n%6===0){
                    raw.writeUInt32LE(raw.readUInt32LE(0xdb8)|0x20,0xdb8);raw.writeFloatLE(.03,0xdd8);raw.writeFloatLE(.0125,0xddc);raw.writeInt32LE(12,0xde0);raw.writeUInt32LE(0x20,0xde8);
                }
                m.write(at(index),raw);const object=c.bullet_manager_at(f,index);for(const [original,local,size] of fields)memory(c,object+local,size).set(raw.subarray(original,original+size));c.bullet_manager_cull_size(f,index,16,24);
            }
            m.write(at(175)+0xdbe,[6,0]);
            for(testFrame=0;testFrame<120;++testFrame){
                const fieldFlags=testFrame%17===0?1:0,gameFlags=testFrame%19===0?0x800:0;
                m.u32(owner+0x34,fieldFlags);m.u32(0x4a7ec4,gameFlags);m.f32(0x4b36b8,rate);m.u32(0x4b36d4,timingFlags);events.length=0;
                assert.equal(m.call(0x4146f0,{ecx:manager}),1);assert.equal(c.bullet_manager_step(f,rate,timingFlags,fieldFlags,gameFlags,testFrame),1);
                const label=`frame ${rate}/${timingFlags}/${testFrame}`;
                for(const index of selected){const object=c.bullet_manager_at(f,index);
                    for(const [original,local,size] of fields)assert.deepEqual(memory(c,object+local,size),m.bytes(at(index)+original,size),`${label} bullet ${index} field ${original.toString(16)}`);
                    const next=m.u32(at(index)+0xdc8);assert.equal(c.bullet_manager_draw_next(f,index),next?indexOf(next):-1,label+' draw next');
                }
                assert.equal(c.bullet_event_count(f),events.length,label+' events');assert.deepEqual(memory(c,c.bullet_events(f),events.length*16),new Uint8Array(Buffer.concat(events)),label+' event order');
                for(const [part,offset] of [[0,0x25e16c],[1,0x25e164],[2,0x25e168],[3,0x25e180],[4,0x25e170]])assert.equal(c.bullet_manager_metadata(f,part),m.i32(manager+offset),label+' manager '+part);
                for(let group=0;group<6;++group){const head=m.u32(manager+0x25e198+group*4);assert.equal(c.bullet_manager_metadata(f,5+group),head?indexOf(head):-1,label+' head '+group);}
                assert.deepEqual(memory(c,c.bullet_manager_timer(f),12),m.bytes(manager+0x25e174,12),label+' manager timer');++checks;
            }
        }
        report('bullet-manager',{passed:true,checks,slotsPerScenario:72,scope:'Complete original bullet-manager frame callback with empty laser pool. Animation, player collision and sound behavior recorded at boundaries.'});
    }finally{if(f)c.bullet_manager_delete(f);m.close();}
});
