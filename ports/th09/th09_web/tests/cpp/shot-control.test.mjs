import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {oracle,core,memory,report,root} from './helpers.mjs';
test('Original shooting, charge release and quick bombs match for all character resources and both control modes',async()=>{
    const m=await oracle(),c=await core(),f=c.shot_control_fixture(),player=m.allocate(0x31000),own=m.allocate(0x38),opponent=m.allocate(0x38),config=m.allocate(512),scene=m.allocate(0x11000),sht=m.allocate(65536),data=c.allocate(65536);
    let dv=new DataView(c.memory.buffer),events=[],available=true,checks=0;const state=c.shot_control_state(f),fields=Array.from({length:c.shot_control_field_count()},(_,i)=>Array.from({length:3},(_,j)=>dv.getUint32(c.shot_control_fields()+(i*3+j)*4,true)));
    const arg=i=>m.u32(m.reg('ESP')+4+i*4),event=(...a)=>events.push([...a,...Array(8-a.length).fill(0)].map(n=>n>>>0)),field=original=>fields.find(f=>f[0]===original);
    m.u32(player+12,own);m.u32(player+16,opponent);m.u32(own+12,own);m.u32(own+24,own);m.u32(opponent+16,opponent);m.u32(opponent+20,opponent);m.u32(player+0x30338,sht);m.u32(0x4a7e78,config);m.u32(0x4a7e38,scene);
    m.replace(0x43e2f0,'charge-sound',()=>{event(0,arg(0),arg(1));return 0;},2);
    m.replace(0x41a270,'charge-begin',()=>{event(1);return 0;});m.replace(0x41a2d0,'charge-end',()=>{event(2);return 0;});
    m.replace(0x40f7d0,'opponent-boss-slot',()=>{assert.equal(arg(0),3);event(3);return available?0:1;},1);
    m.replace(0x403e50,'charged-opponent-attack',()=>{assert.equal(arg(0),arg(1));assert.equal(arg(2),0);assert.equal(arg(3),sht+44+arg(0)*64);event(4,arg(0));return 0;},4);
    m.replace(0x40cc00,'protection-effect',()=>{assert.equal(arg(2),1);assert.equal(arg(3),0xffffffff);event(5,arg(0),...m.readWords(arg(1),3));return 0;},4);
    m.replace(0x41f4c0,'fire-shot-set',()=>{event(6,arg(0),m.reg('EDX'));return 0;},1);
    function value(original,v,float=false){dv=new DataView(c.memory.buffer);const offset=field(original)[1];if(float){dv.setFloat32(state+offset,v,true);m.f32(player+original,v);}else{dv.setInt32(state+offset,v,true);m.i32(player+original,v);}}
    function timer(original,current,previous=current){const offset=field(original)[1],b=new Uint8Array(new Int32Array([previous,0,current]).buffer);new DataView(b.buffer).setFloat32(4,current,true);memory(c,state+offset,12).set(b);m.write(player+original,b);}
    function resetAreas(){c.shot_control_areas_reset(f);m.write(player+0x28bc,memory(c,c.shot_control_areas(f),513*68));m.view(player+0xb100,0x1018).fill(0);for(let i=0;i<512;++i)m.u32(player+0xb908+i*4,player+0x28bc+i*68);m.u32(player+0xc114,512);}
    function compare(label){dv=new DataView(c.memory.buffer);for(const [original,offset,size] of fields)assert.deepEqual(memory(c,state+offset,size),m.bytes(player+original,size),label+' field '+original.toString(16));assert.equal(c.shot_control_event_count(f),events.length,label+' event count');const p=c.shot_control_events(f);events.forEach((e,i)=>assert.deepEqual(Array.from({length:8},(_,j)=>dv.getUint32(p+(i*8+j)*4,true)),e,label+' event'+i));assert.equal(c.shot_control_area_count(f),m.i32(player+0xc110));assert.deepEqual(memory(c,c.shot_control_areas(f),513*68),m.bytes(player+0x28bc,513*68),label+' protective areas');++checks;}
    function step(held,pressed,auto,side,rate,bomb=false,locked=false){
        m.u32(player+8,side);m.write(config+0xb4+side,[auto?1:0]);m.write(0x4ace18+side*0x8e+0x2c,[held&255,held>>>8]);m.write(0x4ace18+side*0x8e+0x32,[pressed&255,pressed>>>8]);m.f32(0x4b36b8,rate);m.u32(scene+0x1095c,locked?1:0);events=[];
        if(bomb){m.reg('ESI',player);m.call(0x41ca00);}else m.call(0x41f810,{ecx:player});c.shot_control_step(f,held,pressed,auto,side,rate,available,bomb,locked);
    }
    try{
        const p=new Uint8Array(new Float32Array([-37.5,361.125,.49]).buffer);memory(c,c.shot_control_position(f),12).set(p);m.write(player+0x1b88,p);
        for(let character=0;character<16;++character){const bytes=readFileSync(resolve(root,'reference/assets',`pl${String(character).padStart(2,'0')}.sht`));memory(c,data,bytes.length).set(bytes);assert.equal(c.shot_control_resource(f,data,bytes.length),1);m.write(sht,bytes);
            for(let scenario=0;scenario<64;++scenario){resetAreas();for(const [original,offset,size] of fields)memory(c,state+offset,size).fill(0);for(const [original,offset,size] of fields)m.write(player+original,memory(c,state+offset,size));
                const start=[0,99.5,100,199.5,200,299.5,300,399.5,400][scenario%9];value(0x30384,start,true);value(0x30388,[100,200,300,400][scenario%4],true);value(0x1b80,scenario%5===0?4:0);value(0,scenario%6);
                timer(0x3038c,scenario%4===0?299:0);timer(0x30398,scenario%13);timer(0x303a4,[-1,0,4,5,9,10,14][scenario%7]);timer(0x303b0,scenario%4);timer(0x303bc,scenario%12);
                available=scenario%2===0;const auto=(scenario>>1)&1,side=scenario%2;
                for(let frame=0;frame<24;++frame){const held=[0,1,4,5,1,0,2,6][(frame+(scenario>>2))%8],pressed=frame%3===0?held:0,rate=[1,.5,.99][scenario%3];step(held,pressed,auto,side,rate,frame%11===10,frame%7===6);compare(`character${character} scenario${scenario} frame${frame}`);}
            }
        }
        report('shot-control',{checks,characters:16,scope:'Original normal/charged firing, automatic-focus input, level thresholds, full-charge timeout, quick bombs and protective area creation. Shot sets, opponent attack scheduling, HUD, audio and effects are explicit recorded boundaries.'});
    }finally{c.shot_control_delete(f);c.release(data);m.close();}
});
