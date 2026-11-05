import test from 'node:test';
import assert from 'node:assert/strict';
import {oracle,core,memory,report} from './helpers.mjs';
test('Original four-slot item spawning, movement, pickup rewards, transfer scheduling and drawing match C++',async()=>{
    const m=await oracle(),c=await core(),f=c.player_items_fixture(),q=m.allocate(0x31000),sht=m.allocate(64),field=m.allocate(0x38),bm=m.allocate(0x25e200),scene=m.allocate(0x11000),pos=m.allocate(12),cppPos=c.allocate(12),transferBase=m.allocate(64*0x100);
    let dv=new DataView(c.memory.buffer),events=[],transfers=0,checks=0,totalTransfers=0;
    const arg=i=>m.u32(m.reg('ESP')+4+i*4),event=(...a)=>events.push([...a,...Array(10-a.length).fill(0)].map(n=>n>>>0)),item=i=>q+0x30454+i*0x2c4,index=p=>(p-item(0)-32)/0x2c4;
    m.u32(q+12,field);m.u32(field+8,bm);m.u32(q+0x30338,sht);m.u32(0x4a7e38,scene);
    m.replace(0x401560,'item-animation',()=>{event(0,index(arg(0)),arg(1));m.view(arg(0),0x2a4).fill(0);m.u32(arg(0)+0x1f8,1);m.write(arg(0)+0x21a,[arg(1)&255,arg(1)>>>8]);return 0;},2);
    m.replace(0x43a950,'item-draw',()=>{event(1,index(arg(0)));return 0;},1);
    m.replace(0x41bc90,'item-charge',()=>{event(2,arg(0));return 0;},1);
    m.replace(0x40ccc0,'item-transfer',()=>{event(3,arg(0),...m.readWords(arg(1),3),...m.readWords(arg(2),3));const p=transferBase+(transfers++)*0x100;assert.ok(transfers<=64);m.view(p,0x100).fill(0);return p;},5);
    m.replace(0x41d150,'item-combo',()=>{event(4,...m.readWords(arg(0),3),arg(1),arg(2),arg(3),arg(4));return 0;},5);
    m.replace(0x43e2f0,'item-sound',()=>{event(5,arg(0),arg(1));return 0;},2);
    const base=c.player_items_part(f,0),context=c.player_items_part(f,2);m.write(item(0),memory(c,base,4*0x2c4));
    function compare(label){dv=new DataView(c.memory.buffer);assert.deepEqual(memory(c,base,4*0x2c4),m.bytes(item(0),4*0x2c4),label+' item state');assert.deepEqual(memory(c,c.player_items_part(f,1),8),m.bytes(0x4ace0c,8),label+' RNG');assert.deepEqual(memory(c,c.player_items_part(f,3),8),m.bytes(q+0x30f64,8),label+' target');assert.equal(c.player_items_event_count(f),events.length,label+' event count');const p=c.player_items_events(f);events.forEach((e,i)=>assert.deepEqual(Array.from({length:10},(_,j)=>dv.getUint32(p+(i*10+j)*4,true)),e,label+' event'+i));assert.equal(c.player_items_transfer_count(f),transfers);for(let i=0;i<transfers;++i){const t=c.player_items_transfers(f)+i*16;assert.deepEqual(memory(c,t,4),m.bytes(transferBase+i*0x100+0x98,4));assert.deepEqual(memory(c,t+4,12),m.bytes(transferBase+i*0x100+0xa0,12));}totalTransfers+=transfers;++checks;}
    function spawn(kind,p,locked){events=[];transfers=0;const b=new Uint8Array(new Float32Array(p).buffer);m.write(pos,b);memory(c,cppPos,12).set(b);m.u32(scene+0x1095c,locked);m.call(0x41db90,{ecx:q,args:[kind,pos]});c.player_items_spawn(f,kind,cppPos,locked);compare('spawn');}
    try{
        for(let scenario=0;scenario<100;++scenario){const side=scenario%2,rank=scenario%23,difficulty=scenario%5;dv.setUint32(c.player_items_part(f,1),4567+scenario,true);dv.setUint32(c.player_items_part(f,1)+4,0,true);m.write(0x4ace0c,memory(c,c.player_items_part(f,1),8));[0,side,rank,difficulty].forEach((v,i)=>dv.setInt32(context+i*4,v,true));dv.setFloat32(context+16,16,true);m.i32(q+8,side);m.i32(0x4a7e44,rank);m.i32(0x4a7eac,difficulty);m.f32(sht+16,16);
            for(let n=0;n<2;++n){const g=context+56+n*20;dv.setUint32(g,32+n*304,true);dv.setUint32(g+4,16,true);dv.setFloat32(g+8,3.5,true);dv.setFloat32(g+12,2.75,true);dv.setFloat32(g+16,288,true);m.u32(0x4b3100+0x78+n*0xf0+0xcc,32+n*304);m.u32(0x4b3100+0x78+n*0xf0+0xd0,16);}m.f32(0x4a80e0,3.5);m.f32(0x4a80e4,2.75);m.f32(0x4a80e8,288);
            for(let n=0;n<4;++n){dv.setInt32(base+n*0x2c4+28,0,true);m.i32(item(n)+28,0);}
            for(let n=0;n<6;++n)spawn((n+scenario)%4,[-30+n*20,scenario%2?460:300+n*10,.125],n===1?1:0);
            for(let frame=0;frame<220;++frame){const state=frame%37===0?4:frame%5===0?3:0;dv.setInt32(context,state,true);m.i32(q,state);
                const p=new Uint8Array(new Float32Array([scenario%3===0?0:-30+(frame%4)*20,frame<15?0:300,.49]).buffer),bounds=new Uint8Array(new Float32Array([-70,frame<15?-10:180,0,70,frame<15?10:430,0]).buffer);memory(c,context+20,12).set(p);memory(c,context+32,24).set(bounds);m.write(q+0x1b88,p);m.write(q+0x1c90,bounds);
                events=[];transfers=0;m.call(0x41dc20,{ecx:q});c.player_items_step(f);compare(`scenario${scenario} frame${frame}`);
                if(frame%13===0){events=[];transfers=0;m.call(0x401440,{ecx:0x4b3100,args:[side]});m.call(0x41df80,{ecx:q});c.player_items_draw(f);compare('draw');}
            }
        }
        report('player-items',{checks,totalTransfers,scope:'Four-slot original item pool, launch/gravity/culling, all rewards, deterministic rank/difficulty transfer payloads and ordering, item-1 loop-counter behavior and draw positions. ANM start/draw, charge, combo, transfers and audio are recorded boundaries.'});
    }finally{c.player_items_delete(f);c.release(cppPos);m.close();}
});
