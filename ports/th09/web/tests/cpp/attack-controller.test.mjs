import test from 'node:test';
import assert from 'node:assert/strict';
import {oracle,core,memory,report,bits} from './helpers.mjs';
test('Spell and boss attack controller, counters, freeze and announcements match original',async()=>{
    const m=await oracle(),c=await core(),manager=m.allocate(0x1368),players=[m.allocate(0x31000),m.allocate(0x31000)],backgrounds=[m.allocate(0x2400),m.allocate(0x2400)],huds=[m.allocate(0xabd0),m.allocate(0xabd0)],enemies=[m.allocate(0x3400),m.allocate(0x3400)],enemyManagers=[m.allocate(4),m.allocate(4)],text=m.allocate(128),name=c.allocate(128);
    let f=0,parts=[],side=0,frame=0,notify=true,hasBoss=[0,0],events=[],checks=0,starts=0,draws=0,dv=new DataView(c.memory.buffer);
    const fields=Array.from({length:c.attack_controller_field_count()},(_,i)=>Array.from({length:3},(_,j)=>dv.getUint32(c.attack_controller_fields()+(i*3+j)*4,true))),arg=i=>m.u32(m.reg('ESP')+4+i*4),event=(...a)=>events.push([...a,...Array(8-a.length).fill(0)].map(x=>x|0));
    function index(vm){const i=(vm-manager-0xac)/0x2a4;assert.equal(i,Math.floor(i));return i;}
    m.replace(0x40f7d0,'boss-query',()=>{const s=enemyManagers.indexOf(m.reg('ECX'));assert.equal(arg(0),3);event(0,s);return hasBoss[s]?enemies[s]:0;},1);
    m.replace(0x40f1d0,'attack-enemy',()=>{const s=enemyManagers.indexOf(m.reg('ECX')),script=arg(0);assert.equal(arg(3),0xfffffffe);assert.equal(arg(5),0);assert.equal(arg(6),1);event(1,s,script,arg(2),arg(4));if(script===2)hasBoss[s]=1;if(notify){const at=manager+(script<2?0xa4:0xa8);if(m.u32(at)){m.u32(at,2);if(script===2)event(10,side);}}return enemies[s];},7);
    m.replace(0x403e00,'notice-start',()=>{const vm=arg(0),script=arg(1);event(2,index(vm),script);m.view(vm,0x2a4).fill(0);m.write(vm+0x21a,[script&255,(script>>>8)&255]);m.u32(vm+0x1f8,3);m.u32(vm+0x1f0,0x80706050);m.f32(vm+0x288,script*3-17);m.f32(vm+0x28c,script*2+3);m.f32(vm+0x290,.25);return 0;},2);
    m.replace(0x436ac0,'notice-sprite',()=>{event(3,index(arg(0)),m.reg('ECX')===0x102?1:0,arg(1));m.write(arg(0)+0x214,[arg(1)&255,(arg(1)>>>8)&255]);return 0;},2);
    m.replace(0x436f30,'notice-advance',()=>{event(4,index(arg(0)));return frame%43===42?1:0;},1);
    m.replace(0x43a950,'notice-draw',()=>{event(5,index(arg(0)));return 0;},1);
    m.replace(0x43be50,'notice-text',()=>{event(6,index(arg(1)),arg(2),arg(3));return 0;});
    m.replace(0x43e2f0,'notice-sound',()=>{event(7,arg(0),arg(1));return 0;},2);
    m.replace(0x4011c0,'background-reset',()=>{event(9,backgrounds.indexOf(m.reg('ECX')));return 0;});
    m.replace(0x401560,'attack-portrait',()=>{const s=m.reg('ECX')-0x110,layer=(arg(0)-huds[s]-0xa684)/0x2a4;event(11,s,layer,arg(1));return 0;},2);
    m.replace(0x403d90,'notice-viewport',()=>{event(12,arg(0));m.u32(0x4b3448,0x4b3178+arg(0)*0xf0);return 0;},1);
    m.u32(0x4b36cc,0x102);m.u32(0x4b36d4,0);
    for(let s=0;s<2;++s){m.u32(0x4a7d90+s*0x38,backgrounds[s]);m.u32(0x4a7d94+s*0x38,players[s]);m.u32(0x4a7da0+s*0x38,enemyManagers[s]);m.u32(0x4a7da8+s*0x38,huds[s]);m.u32(huds[s]+0xa678,0x110+s);}
    const heap=m.heap;
    function compare(label){
        dv=new DataView(c.memory.buffer);for(const [original,offset,size] of fields)assert.deepEqual(memory(c,parts[0]+offset,size),m.bytes(manager+original,size),label+' field '+original.toString(16));
        assert.equal(dv.getUint32(parts[2],true),m.u32(0x4a7ec4),label+' flags');assert.equal(dv.getInt32(parts[3],true),m.i32(0x4a7e5c),label+' reward');
        for(let s=0;s<2;++s){assert.deepEqual(memory(c,parts[4]+s*24,8),m.bytes(players[s]+0xa0,8),label+' levels');assert.deepEqual(memory(c,parts[4]+s*24+8,12),m.bytes(players[s]+0xb0,12),label+' counts');assert.deepEqual(memory(c,parts[6]+s*8,8),m.bytes(backgrounds[s]+0x1c,8),label+' background');assert.equal(dv.getUint32(parts[7+s],true),m.u32(enemies[s]+0x337c),label+' enemy flags');assert.equal(dv.getInt32(parts[9]+s*4,true)>=0,m.u32(manager+0x1328+s*12)!==0,label+' active');}
        assert.equal(c.attack_controller_event_count(f),events.length,label+' events '+JSON.stringify(events));events.forEach((e,i)=>assert.deepEqual(Array.from({length:8},(_,j)=>dv.getInt32(c.attack_controller_events(f)+32*i+4*j,true)),e,label+' event'+i));++checks;
    }
    function begin(variant,label){events=[];const level=variant%3;m.call(0x403e50,{ecx:manager,args:[variant,level,11,text]});c.attack_controller_begin(f,variant,level,11,name,notify);compare(label);++starts;}
    try{for(side=0;side<2;++side)for(let variant=0;variant<9;++variant)for(let scenario=0;scenario<8;++scenario){
        f=c.attack_controller_fixture(side);parts=Array.from({length:10},(_,i)=>c.attack_controller_part(f,i));dv=new DataView(c.memory.buffer);m.heap=heap;m.view(manager,0x1368).fill(0);m.u32(manager,side);m.u32(manager+4,0x4a7d90+side*0x38);m.u32(manager+8,0x4a7d90+(1-side)*0x38);m.u32(manager+0x14,0x101);
        const nameBytes=new TextEncoder().encode('Spell '+variant+' / '+scenario+'\0');m.write(text,nameBytes);memory(c,name,nameBytes.length).set(nameBytes);m.i32(0x4a7e5c,scenario*319);dv.setInt32(parts[3],scenario*319,true);m.u32(0x4a7ec4,scenario&1?0x840:0);dv.setUint32(parts[2],m.u32(0x4a7ec4),true);notify=scenario%3!==0;hasBoss=[scenario&1,(scenario>>1)&1];
        for(let s=0;s<2;++s){m.view(players[s],0x31000).fill(0);m.view(backgrounds[s],0x2400).fill(0);m.view(enemies[s],0x3400).fill(0);dv.setUint32(parts[5]+s*4,hasBoss[s],true);for(let i=0;i<2;++i){const n=[0,1,9,15,16,22][(scenario+i+s)%6];dv.setInt32(parts[4]+s*24+i*4,n,true);m.i32(players[s]+0xa0+i*4,n);}dv.setInt32(parts[4]+s*24+20,60+s*7,true);m.i32(players[s]+0x303f0,60+s*7);
            const p=parts[1]+s*20;dv.setUint32(p,32+s*304,true);dv.setUint32(p+4,16,true);dv.setFloat32(p+8,9.375,true);dv.setFloat32(p+12,3.75,true);dv.setFloat32(p+16,288,true);m.u32(0x4b3178+s*0xf0+0xcc,32+s*304);m.u32(0x4b3178+s*0xf0+0xd0,16);}
        m.f32(0x4a80e0,9.375);m.f32(0x4a80e4,3.75);begin(variant,`begin ${side}/${variant}/${scenario}`);
        const rate=[1,.5,.99,1.5][scenario%4];m.f32(0x4b36b8,rate);
        for(frame=0;frame<240;++frame){
            if(frame===51)begin((variant+3)%9,'interrupt '+scenario);
            if(frame===140){hasBoss=[0,0];memory(c,parts[5],8).fill(0);}
            events=[];m.call(0x4041f0,{ecx:manager});c.attack_controller_step(f,rate,frame);compare(`frame ${side}/${variant}/${scenario}/${frame}`);
            if(frame%23===0){events=[];m.call(0x404410,{ecx:manager});c.attack_controller_draw(f);compare('draw '+frame);++draws;}
        }
        c.attack_controller_delete(f);f=0;
    }report('attack-controller',{checks,starts,draws,variants:9,scope:'Original spell/Boss controller, reentrant ECL announcement timing, level/statistic changes, counter-Boss flags, freeze windows, interrupts, timers and ANM draw order. ECL enemy creation, animation execution, text rasterization and graphics are service boundaries.'});
    }finally{if(f)c.attack_controller_delete(f);c.release(name);m.close();}
});
