import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {oracle,core,memory,report,root} from './helpers.mjs';
test('Original damage, Tewi defence, recovery, entrance and invulnerability transitions match',async()=>{
    const m=await oracle(),c=await core(),f=c.player_life_fixture(),p=m.allocate(0x31000),own=m.allocate(0x38),other=m.allocate(0x38),scene=m.allocate(0x12000),sht=m.allocate(65536),focus=m.allocate(512),shield=m.allocate(512),data=c.allocate(65536);
    let dv=new DataView(c.memory.buffer),events=[],boss=true,checks=0;
    const parts=Array.from({length:10},(_,i)=>c.player_life_part(f,i));
    const fields=(fn,count)=>Array.from({length:count},(_,i)=>Array.from({length:3},(_,j)=>dv.getUint32(fn()+(i*3+j)*4,true)));
    const motion=fields(c.motion_fields,c.motion_field_count()),control=fields(c.shot_control_fields,c.shot_control_field_count());
    const arg=i=>m.u32(m.reg('ESP')+4+i*4),event=(...a)=>events.push([...a,...Array(8-a.length).fill(0)].map(n=>n>>>0));
    m.view(p,0x31000).fill(0);m.view(scene,0x12000).fill(0);m.view(own,0x38).fill(0);m.view(other,0x38).fill(0);
    m.u32(p+12,own);m.u32(p+16,other);m.u32(own+12,own);m.u32(own+24,own);m.u32(other+16,other);m.u32(other+20,other);m.u32(p+0x30338,sht);m.u32(0x4a7e38,scene);m.u32(focus+4,focus+16);
    m.replace(0x43e2f0,'sound-pan',()=>{event(0,arg(0),arg(1));return 0;},2);
    m.replace(0x40f7d0,'opponent-boss-slot',()=>{assert.equal(arg(0),3);event(3);return boss?0:1;},1);
    m.replace(0x403e50,'defence-attack',()=>{assert.equal(arg(0),arg(1));assert.equal(arg(2),0);assert.equal(arg(3),sht+44+arg(0)*64);event(4,arg(0));return 0;},4);
    m.replace(0x40cc00,'protection-effect',()=>{event(5,arg(0),...m.readWords(arg(1),3));return 0;},4);
    m.replace(0x43e380,'sound-position',()=>{event(7,arg(0),arg(1));return 0;},2);
    m.replace(0x415d70,'opponent-wins',()=>{event(8,arg(0));return 0;},1);
    m.replace(0x40cd70,'slotted-effect',()=>{event(9,arg(0),...m.readWords(arg(1),3),arg(2),arg(3));return 0;},4);
    m.replace(0x41a380,'critical-health',()=>{assert.equal(arg(0),0);event(10);return 0;},1);
    m.replace(0x406790,'focus-interrupt',()=>{assert.equal(m.reg('ECX'),focus+16);assert.equal(arg(0),1);event(11);return 0;},1);
    m.replace(0x41d7e0,'flush-combo',()=>{event(12);return 0;});
    m.replace(0x41bdc0,'reset-ai',()=>{event(13);return 0;});
    m.replace(0x41bc90,'charge-recovery',()=>{event(14,arg(0));return 0;},1);
    function set(which,original,value,float=false){dv=new DataView(c.memory.buffer);const list=which?control:motion,entry=list.find(x=>x[0]===original),at=parts[which]+entry[1];if(float){dv.setFloat32(at,value,true);m.f32(p+original,value);}else{dv.setInt32(at,value,true);m.i32(p+original,value);}}
    function timer(original,value){const b=new Uint8Array(new ArrayBuffer(12)),v=new DataView(b.buffer);v.setInt32(0,Math.trunc(value)-1,true);v.setFloat32(4,value,true);v.setInt32(8,Math.trunc(value),true);const entry=control.find(x=>x[0]===original);memory(c,parts[1]+entry[1],12).set(b);m.write(p+original,b);}
    function reset(character,scenario){
        for(let which=0;which<2;++which)for(const [orig,off,size] of which?control:motion){memory(c,parts[which]+off,size).fill(0);m.view(p+orig,size).fill(0);}
        memory(c,parts[2],0x2a4).fill(0);m.view(p+0xc0,0x2a4).fill(0);
        c.player_life_areas_reset(f);m.write(p+0x28bc,memory(c,parts[6],513*68));m.view(p+0xb100,0x1018).fill(0);for(let i=0;i<512;++i)m.u32(p+0xb908+i*4,p+0x28bc+i*68);m.u32(p+0xc114,512);
        dv=new DataView(c.memory.buffer);const side=scenario%2,health=[0,1,2,3,5,8,10][scenario%7],controller=scenario%3,show=scenario%5===0?-1:scenario%16,hidden=scenario%2,hasFocus=scenario%2,hasShield=scenario%3!==0,z=(scenario%5-2)*.125;
        boss=scenario%3===0;c.player_life_settings(f,character,controller,hasFocus,show,hidden,hasShield,boss,z);
        m.u32(own+32,character);m.u32(p+32,controller);m.u32(p+0x364,hasFocus?focus:0);m.u32(p+0x368,hasFocus?shield:0);m.u32(p+0x30334,show);m.u32(p+0x3032c,hidden);m.u32(p+0x303e0,hasShield?shield:0);m.f32(p+0x1cd4,z);m.write(shield+0xc4,[1]);
        set(0,8,side);set(0,0xa8,health);set(1,0,scenario%6);set(1,0x30384,73,true);set(1,0x30388,[0,100,199,200,299,300,399,400][scenario%8],true);
        const pos=[(scenario%9-4)*30.25,100+scenario*6,.25],step=[.6,-.8];for(let j=0;j<3;++j){dv.setFloat32(parts[0]+motion.find(x=>x[0]===0x1b88)[1]+j*4,pos[j],true);m.f32(p+0x1b88+j*4,pos[j]);dv.setFloat32(parts[7]+j*4,17+j,true);m.f32(shield+12+j*4,17+j);}
        for(let j=0;j<2;++j){dv.setFloat32(parts[0]+motion.find(x=>x[0]===0x1ccc)[1]+j*4,step[j],true);m.f32(p+0x1ccc+j*4,step[j]);}
        timer(0x303c8,[-2,0,.5,1,7,29,30,59,59.5,60,80][scenario%11]);timer(0x1b74,scenario%9);
        const rules=[scenario%8,scenario%4,scenario*90];for(let j=0;j<3;++j){dv.setInt32(parts[3]+j*4,rules[j],true);m.i32([0x4a7e48,0x4a7e4c,0x4a7e5c][j],rules[j]);}dv.setUint8(parts[3]+12,scenario%17===0?1:0);m.u32(scene+0x11ea8,scenario%17===0?1:0);
        for(let j=0;j<4;++j){dv.setFloat32(parts[4]+j*4,[-184,32,368,416][j],true);m.f32(0x4a80f0+j*4,[-184,32,368,416][j]);}
        dv.setUint32(parts[5],(scenario*733+character*279)&65535,true);dv.setUint32(parts[5]+4,7,true);m.write(0x4ace0c,memory(c,parts[5],8));
        memory(c,parts[8],8).fill(0);memory(c,parts[9],8).fill(0);m.view(scene+0xe934,16).fill(0);
    }
    function compare(label){
        for(let which=0;which<2;++which)for(const [original,offset,size] of which?control:motion)assert.deepEqual(memory(c,parts[which]+offset,size),m.bytes(p+original,size),label+' field '+original.toString(16));
        assert.deepEqual(memory(c,parts[2],0x2a4),m.bytes(p+0xc0,0x2a4),label+' body');assert.deepEqual(memory(c,parts[5],8),m.bytes(0x4ace0c,8),label+' rng');
        dv=new DataView(c.memory.buffer);for(let j=0;j<3;++j)assert.equal(dv.getInt32(parts[3]+j*4,true),m.i32([0x4a7e48,0x4a7e4c,0x4a7e5c][j]),label+' damage rules');
        assert.equal(c.player_life_value(f,0),m.u32(p+0x364)?1:0,label+' focus');assert.equal(c.player_life_value(f,1),m.i32(p+0x30334),label+' display');assert.equal(c.player_life_value(f,2),m.i32(p+0x3032c),label+' hidden');assert.equal(c.player_life_value(f,3),m.u32(p+0x303e0)?1:0,label+' shield');
        assert.deepEqual(memory(c,parts[7],12),m.bytes(shield+12,12),label+' shield position');assert.deepEqual(memory(c,parts[8],8),m.bytes(scene+0xe93c,8),label+' flash duration');assert.deepEqual(memory(c,parts[9],8),m.bytes(scene+0xe934,8),label+' flash color');
        assert.equal(c.player_life_area_count(f),m.i32(p+0xc110),label+' area count');assert.deepEqual(memory(c,parts[6],513*68),m.bytes(p+0x28bc,513*68),label+' areas');
        assert.equal(c.player_life_event_count(f),events.length,label+' events');const out=c.player_life_events(f);events.forEach((e,i)=>assert.deepEqual(Array.from({length:8},(_,j)=>dv.getUint32(out+(i*8+j)*4,true)),e,label+' event'+i));++checks;
    }
    function step(which,rate,label){events=[];m.f32(0x4b36b8,rate);m.reg('ESI',p);m.call([0x41e420,0x41e720,0x41cb50,0x41cc80][which],{ecx:p});c.player_life_step(f,which,rate);compare(label);}
    try{
        for(let character=0;character<16;++character){const bytes=readFileSync(resolve(root,'reference/assets',`pl${String(character).padStart(2,'0')}.sht`));memory(c,data,bytes.length).set(bytes);assert.equal(c.player_life_resource(f,data,bytes.length),1);m.write(sht,bytes);
            for(let scenario=0;scenario<88;++scenario)for(let which=0;which<4;++which){reset(character,scenario);step(which,1,`${character}/${scenario}/${which}`);if(which===3)for(let frame=0;frame<10;++frame)step(3,[1,.5,.99][scenario%3],`${character}/${scenario}/timer${frame}`);}
        }
        report('player-life',{checks,characters:16,scope:'Original health loss, fatal hit, Tewi automatic defence, knockback and recovery, entrance, invulnerability and shock timers. Opponent scheduling, HUD, effects, charge and combo/AI resets are recorded service boundaries.'});
    }finally{c.player_life_delete(f);c.release(data);m.close();}
});
