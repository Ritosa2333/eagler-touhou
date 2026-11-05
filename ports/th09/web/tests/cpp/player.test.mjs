import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync,readdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {oracle,core,root,bits,memory,report} from './helpers.mjs';

test('TH09 player movement matches both sides, focus changes, bounds and trailing positions',async()=>{
    const m=await oracle(),c=await core(),speeds=c.allocate(16),limits=c.allocate(16);
    const q=m.allocate(0x31000),owner=m.allocate(256),config=m.allocate(256),shot=m.allocate(256),fx=[m.allocate(1024),m.allocate(1024)],vm=m.allocate(1024);
    m.u32(q+12,owner);m.u32(q+0x30338,shot);m.u32(0x4a7e78,config);m.u32(owner+12,0x2000000);
    let events=[],focusIndex=0,checks=0;
    const arg=i=>m.u32(m.reg('ESP')+4+i*4);
    m.replace(0x40cd70,'focus-create',()=>{events.push(1,arg(0),arg(2));const result=fx[focusIndex++%2];m.u32(result+4,vm);m.view(result+0xc4,1)[0]=1;return result;},4);
    m.replace(0x406790,'focus-end',()=>{events.push(2,0,0);return 0;},1);
    m.replace(0x403e00,'animation-interrupt',()=>{events.push(3,arg(1),0);return 0;},2);
    m.replace(0x436f30,'option-animation',()=>0,1);m.replace(0x401510,'option-timer',()=>0,1);
    const callbacks=Array.from({length:4},(_,i)=>m.registerImport({name:'option-'+i,argc:0,handler:()=>{events.push(4,i,0);return 0;}}));
    const fields=Array.from({length:c.motion_field_count()},(_,i)=>{const v=new DataView(c.memory.buffer,c.motion_fields()+i*12,12);return Array.from({length:3},(_,j)=>v.getUint32(j*4,true));});
    const rawFields=()=>fields.forEach(([a,b,n])=>assert.deepEqual(memory(c,p+b,n),m.bytes(q+a,n),`movement offset 0x${a.toString(16)} frame ${checks}`));
    let p=0;
    try{
        const files=readdirSync(resolve(root,'reference/assets')).filter(n=>/^pl\d\d\.sht$/.test(n)).sort();assert.ok(files.length>=8);
        for(let character=0;character<files.length;++character)for(let player=0;player<2;++player){
            p=c.motion_create();m.view(q,0x31000).fill(0);m.u32(q+12,owner);m.u32(q+0x30338,shot);
            for(const [a,b,n] of fields)m.write(q+a,memory(c,p+b,n));
            const playerField=fields.find(f=>f[0]===8);new DataView(c.memory.buffer).setUint32(p+playerField[1],player,true);m.u32(q+8,player);m.u32(owner+32,character);
            const input=readFileSync(resolve(root,'reference/assets',files[character]));m.write(shot,input.subarray(0,48));memory(c,speeds,16).set(input.subarray(20,36));
            const position=fields.find(f=>f[0]===0x1b88),extents=[0x1ca8,0x1cb4,0x1cc0].map(a=>fields.find(f=>f[0]===a));
            const pos=new Float32Array([player?220:-10,400,0]);memory(c,p+position[1],12).set(new Uint8Array(pos.buffer));m.write(q+position[0],new Uint8Array(pos.buffer));
            for(let i=0;i<3;++i){const data=new Uint8Array(new Float32Array([2+i*4,3+i*4,1]).buffer);memory(c,p+extents[i][1],12).set(data);m.write(q+extents[i][0],data);}
            const field=new Float32Array([-128,16,256,416]);memory(c,limits,16).set(new Uint8Array(field.buffer));m.write(0x4a80f0,new Uint8Array(field.buffer));
            for(let frame=0;frame<256;++frame){
                const autoFocus=frame%64>=32,fireFrames=frame%12,keys=((frame%16)<<4)|(frame%7<3?4:0),rate=[1,1,1,0.5,1/3][frame%5],options=frame%16;
                m.view(config+0xb4+player,1)[0]=Number(autoFocus);new DataView(m.view(0x4ace42+player*0x8e,2).buffer,m.view(0x4ace42+player*0x8e,2).byteOffset,2).setUint16(0,fireFrames,true);
                const keyAddress=0x4ace18+player*0x8e+0x2c;new DataView(m.view(keyAddress,2).buffer,m.view(keyAddress,2).byteOffset,2).setUint16(0,keys,true);m.f32(0x4b36b8,rate);
                for(let i=0;i<4;++i)m.u32(q+0x1cec+i*0x2f4+0x2ec,options&(1<<i)?callbacks[i]:0);
                events=[];m.call(0x41c170,{ecx:q});c.motion_step(p,keys,autoFocus,fireFrames,character,speeds,limits,rate,options);
                assert.deepEqual(Array.from(new Uint32Array(c.memory.buffer,c.motion_events_data(),c.motion_events_size())),events,`events ${character}/${player}/${frame}`);rawFields();checks++;
            }
            c.motion_delete(p);p=0;
        }
        report('player-motion',{passed:true,frames:checks,characters:files,players:2,comparedFields:fields.length,original:'0x0041c170',boundaryStubs:['effect creation/deletion','animation interrupts','option animation/timer; callback order checked']});
    }finally{if(p)c.motion_delete(p);c.release(speeds);c.release(limits);m.close();}
});

test('TH09 charge gain, character bonus, cap and HUD transitions match original',async()=>{
    const m=await oracle(),c=await core(),p=c.allocate(4),q=m.allocate(0x31000),owner=m.allocate(64);m.u32(q+12,owner);let level=-1,checks=0;
    m.replace(0x41a320,'charge-level',()=>{level=m.u32(m.reg('ESP')+4)|0;return 0;},1);
    try{
        for(let character=0;character<16;++character)for(const start of [0,0.1,99.999,100,199.9,299.99,399.99,400]){
            m.u32(owner+32,character);m.f32(q+0x30388,start);new DataView(c.memory.buffer).setFloat32(p,start,true);
            for(const amount of [0,0.001,0.01,0.5,2,6.5,20,50,100,400]){
                level=-1;m.call(0x41bc90,{ecx:q,args:[bits(amount)]});assert.equal(c.charge_add(p,amount,character),level);
                assert.equal(new DataView(c.memory.buffer).getUint32(p,true),m.u32(q+0x30388));checks++;
            }
        }
        report('charge-gauge',{passed:true,checks,characters:16});
    }finally{c.release(p);m.close();}
});

test('TH09 player box, circle and item collision boundaries match original',async()=>{
    const m=await oracle(),c=await core(),p=c.allocate(128),q=m.allocate(0x31000),shot=m.allocate(64),center=m.allocate(16),extent=m.allocate(16);m.u32(q+0x30338,shot);
    let seed=1234567,checks=0;const random=()=>{seed=(Math.imul(seed,1664525)+1013904223)>>>0;return (seed%1024-512)/16;};
    try{
        for(let i=0;i<6000;++i){
            const x=random(),y=random(),rx=Math.abs(random())/8,ry=Math.abs(random())/8,px=random(),py=random(),ex=Math.abs(random())/4,ey=Math.abs(random())/4;
            const b=new Float32Array([x-rx,y-ry,0,x+rx,y+ry,0]),pos=new Float32Array([px,py,0]),size=new Float32Array([ex,ey,0]),player=new Float32Array([x,y,0]);
            memory(c,p,24).set(new Uint8Array(b.buffer));memory(c,p+24,12).set(new Uint8Array(pos.buffer));memory(c,p+40,12).set(new Uint8Array(size.buffer));memory(c,p+56,12).set(new Uint8Array(player.buffer));
            m.write(q+0x1c60,new Uint8Array(b.buffer));m.write(q+0x1c90,new Uint8Array(b.buffer));m.write(q+0x1b88,new Uint8Array(player.buffer));m.write(center,new Uint8Array(pos.buffer));m.write(extent,new Uint8Array(size.buffer));m.f32(shot+4,rx);
            assert.equal(c.collision_box(p,p+24,p+40),m.call(0x41bdf0,{ecx:q,args:[center,extent]}));
            assert.equal(c.collision_circle(p+56,rx,p+24,ex),m.call(0x41be70,{ecx:q,args:[center,bits(ex)]}));
            const life=i%5;m.u32(q,life);assert.equal(c.collision_item(life,p,p+24,p+40),m.call(0x41bee0,{ecx:q,args:[center,extent]}));checks+=3;
        }
        report('player-collision',{passed:true,checks,shapes:['axis-aligned box','circle','item collection box']});
    }finally{c.release(p);m.close();}
});
