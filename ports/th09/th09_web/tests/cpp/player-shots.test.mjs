import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {oracle,core,memory,report,root,bits} from './helpers.mjs';
test('All 16 SHT resources reproduce native shot spawning, special callbacks, movement, damage and draw state',async()=>{
    const m=await oracle(),c=await core(),q=m.allocate(0x31000),field=m.allocate(0x38),effect=m.allocate(0x200),sprite=m.allocate(0x44),name=m.allocate(32),pos=m.allocate(12),extent=m.allocate(12),results=m.allocate(12);
    const data=c.allocate(65536),cppPos=c.allocate(12),cppExtent=c.allocate(12),cppResults=c.allocate(12),cppProtection=c.allocate(12);
    const metadata=JSON.parse(readFileSync(resolve(root,'reference/player-shot-callbacks.json'),'utf8'));
    let f=0,loaded=0,frame=0,events=[],drawn=[],checks=0,damageChecks=0,drawChecks=0,fired=0,activeCharacter=0;
    const arg=i=>m.u32(m.reg('ESP')+4+i*4),nativeShot=i=>q+0xc11c+i*0x484,index=p=>(p-nativeShot(0))/0x484,vec=p=>m.readWords(p,3);
    const event=(...e)=>events.push([...e,...Array(8-e.length).fill(0)].map(n=>n>>>0));
    m.replace(0x42c970,'sht-resource',()=>loaded,1);
    m.replace(0x403e00,'shot-animation',()=>{const p=arg(0),script=arg(1);event(0,index(p),script);m.view(p,0x2a4).fill(0);m.u32(p+0x1f8,1);m.u32(p+0x1f0,0xffffffff);m.write(p+0x1fc,[1,0]);m.write(p+0x21a,[script&255,script>>>8]);m.u32(p+0x224,sprite);return 0;},2);
    m.replace(0x436f30,'shot-animation-update',()=>{const i=index(arg(0));event(1,i);return (frame+i)%113===112?1:0;},1);
    for(const [address,fading] of [[0x43ab50,0],[0x436d70,1]])m.replace(address,'shot-draw',()=>{event(2,index(arg(0)),fading);drawn.push(m.bytes(arg(0),0x2a4));return 0;},1);
    m.replace(0x43e380,'shot-positioned-sound',()=>{event(3,arg(0),arg(1));return 0;},2);
    m.replace(0x43e2f0,'shot-sound',()=>{event(4,arg(0),arg(1));return 0;},2);
    m.replace(0x40cd70,'shot-effect',()=>{event(5,arg(0),...vec(arg(1)),arg(2));return effect;},4);
    m.replace(0x445560,'shot-effect-get',()=>{event(6,arg(0));return effect;},1);
    m.f32(sprite+0x30,32);m.f32(sprite+0x34,24);
    let dv=new DataView(c.memory.buffer);
    function compareEvents(label){dv=new DataView(c.memory.buffer);const p=c.player_shots_events(f);assert.equal(c.player_shots_event_count(f),events.length,label+' event count');events.forEach((e,i)=>assert.deepEqual(Array.from({length:8},(_,j)=>dv.getUint32(p+(i*8+j)*4,true)),e,label+' event'+i));}
    function compareAnm(actual,expected,label){assert.deepEqual(actual.subarray(0,0x224),expected.subarray(0,0x224),label+' ANM');assert.deepEqual(actual.subarray(0x228,0x2a4),expected.subarray(0x228,0x2a4),label+' ANM tail');}
    function compare(label){
        for(let i=0;i<128;++i){const p=c.player_shots_shot(f,i),original=nativeShot(i);compareAnm(memory(c,p,0x2a4),m.bytes(original,0x2a4),label+' shot'+i);assert.deepEqual(memory(c,p+0x2a4,0x1d0),m.bytes(original+0x2a4,0x1d0),label+' shot values'+i);}
        assert.deepEqual(memory(c,c.player_shots_part(f,3),8),m.bytes(q+0x1cdc,8),label+' player scale');assert.deepEqual(memory(c,c.player_shots_part(f,4),12),m.bytes(q+0x3031c,12),label+' beam timer');assert.equal(c.player_shots_beam(f),m.u32(q+0x30328)?index(m.u32(q+0x30328)):-1,label+' beam');
        assert.equal(c.player_shots_area_count(f),m.i32(q+0xc110),label+' area count');assert.deepEqual(memory(c,c.player_shots_area(f,0),513*68),m.bytes(q+0x28bc,513*68),label+' areas');compareEvents(label);++checks;
    }
    function setPosition(frame){
        const p=new Uint8Array(new Float32Array([((frame*7)%200-100)*.875,330+(frame%19)*.375,.49]).buffer);m.write(q+0x1b88,p);memory(c,c.player_shots_part(f,0),12).set(p);
        for(let i=0;i<4;++i){const point=new Uint8Array(new Float32Array([i*20-30,310+i*2,.49]).buffer);m.write(q+0x1cec+i*0x2f4+0x2a4,point);memory(c,c.player_shots_part(f,1)+i*12,12).set(point);}
        const target=new Uint8Array(new Float32Array([frame%61<20?-999:((frame*13)%200-100),130+(frame%29),0]).buffer);m.write(q+0x30364,target);memory(c,c.player_shots_part(f,2),12).set(target);
    }
    function fire(set,t){events=[];m.call(0x41f4c0,{ecx:q,edx:t,args:[set]});c.player_shots_fire(f,set,t,frame%37<18?1:0);compare(`char${activeCharacter} frame${frame} fire${set}/${t}`);++fired;}
    try{
        for(let character=0;character<16;++character)for(let side=0;side<2;++side){
            activeCharacter=character;f=c.player_shots_fixture();dv=new DataView(c.memory.buffer);m.view(q,0x31000).fill(0);m.u32(q+12,field);m.u32(field+12,effect);m.u32(q+8,side);
            // No address-bearing state is imported into production C++.
            const input=readFileSync(resolve(root,'reference/assets',`pl${String(character).padStart(2,'0')}.sht`));assert.ok(input.length<65536);memory(c,data,input.length).set(input);assert.equal(c.player_shots_load(f,data,input.length),1);
            loaded=m.allocate(input.length);m.write(loaded,input);m.reg('ESI',q+0x30338);m.call(0x41bbe0,{ecx:name});assert.deepEqual(memory(c,c.player_shots_part(f,7),16),m.bytes(loaded+20,16),'SHT movement coefficients');
            for(let i=0;i<128;++i){m.write(nativeShot(i),memory(c,c.player_shots_shot(f,i),0x474));m.u32(nativeShot(i)+0x224,0);}
            for(let i=0;i<513;++i)m.write(q+0x28bc+i*68,memory(c,c.player_shots_area(f,i),68));for(let i=0;i<512;++i)m.u32(q+0xb908+i*4,q+0x28bc+i*68);m.u32(q+0xc114,512);
            m.write(q+0x1cdc,memory(c,c.player_shots_part(f,3),8));m.write(q+0x3031c,memory(c,c.player_shots_part(f,4),12));m.write(effect+12,memory(c,c.player_shots_part(f,6),12));
            const g=c.player_shots_part(f,5);dv.setUint32(g,32+side*304,true);dv.setUint32(g+4,16,true);dv.setFloat32(g+8,5.25,true);dv.setFloat32(g+12,2.375,true);dv.setFloat32(g+16,288,true);m.u32(0x4b3100+0x78+side*0xf0+0xcc,32+side*304);m.u32(0x4b3100+0x78+side*0xf0+0xd0,16);m.f32(0x4a80e0,5.25);m.f32(0x4a80e4,2.375);m.call(0x401440,{ecx:0x4b3100,args:[side]});
            // Fixture constructor uses side zero; the other fields are explicit.
            c.player_shots_side(f,side);
            for(frame=0;frame<180;++frame){
                setPosition(frame);m.write(effect+0xc4,[frame%37<18?1:0]);
                fire(0,frame%15);fire(1,frame%120);if(character===6||character===15)fire(2,frame%90);
                const rate=[1,1,.5,.99][frame%4],frozen=frame%47===11?1:0;events=[];m.f32(0x4b36b8,rate);m.u32(field+0x34,frozen);m.call(0x41f580,{ecx:q});c.player_shots_update(f,rate,frozen,frame);compare(`char${character} side${side} frame${frame} update`);
                if(frame%3===0){
                    const shot=Array.from({length:128},(_,i)=>i).find(i=>m.bytes(nativeShot(i)+0x462,2)[0]===1);const p=shot===undefined?new Uint8Array(new Float32Array([0,240,0]).buffer):m.bytes(nativeShot(shot)+0x2a4,12),e=new Uint8Array(new Float32Array([32,24,0]).buffer);
                    m.write(pos,p);memory(c,cppPos,12).set(p);m.write(extent,e);memory(c,cppExtent,12).set(e);const protection=new Uint8Array(new Int32Array([0,0,frame%23===0?0:1]).buffer);m.write(q+0x303c8,protection);memory(c,cppProtection,12).set(protection);
                    const outputs=new Uint8Array(new Int32Array([765,432,17]).buffer);m.write(results,outputs);memory(c,cppResults,12).set(outputs);events=[];const expected=m.call(0x41fcd0,{ecx:q,args:[pos,extent,results,results+4,results+8]});assert.equal(c.player_shots_hit(f,cppPos,cppExtent,cppProtection,cppResults),expected|0);assert.deepEqual(memory(c,cppResults,12),m.bytes(results,12));compare(`char${character} frame${frame} damage`);++damageChecks;
                }
                if(frame%9===0)for(const fading of [0,1]){events=[];drawn=[];m.call(fading?0x41f760:0x41f680,{ecx:q});c.player_shots_draw(f,fading);compare(`char${character} frame${frame} draw${fading}`);assert.equal(c.player_shots_draw_count(f),drawn.length);drawn.forEach((b,i)=>compareAnm(memory(c,c.player_shots_draw_data(f)+i*0x2a4,0x2a4),b,'draw submission'));++drawChecks;}
                m.reg('EDI',q);m.call(0x41c8e0);c.player_shots_area_update(f);
            }
            c.player_shots_delete(f);f=0;
        }
        report('player-shots',{checks,fired,damageChecks,drawChecks,characters:16,callbackTables:metadata.tables,scope:'All local SHT files, authored shot initialization/update/hit/draw behaviors, 128-slot manager and integrated attack areas. Native ANM start/update/draw, sound and effects are recorded boundaries; this is not a complete player or match test.'});
    }finally{if(f)c.player_shots_delete(f);for(const p of [data,cppPos,cppExtent,cppResults,cppProtection])c.release(p);m.close();}
});
