import test from 'node:test';
import assert from 'node:assert/strict';
import {oracle,core,memory,bits,report} from './helpers.mjs';
test('TH09 enemy defeat preserves chain rewards, charge, items, sounds and cross-field transfer RNG',async()=>{
    const m=await oracle(),c=await core(),f=c.enemy_death_fixture(),enemy=m.allocate(0x5430),manager=m.allocate(0x2ac450),player=m.allocate(0x30500),owner=m.allocate(64),other=m.allocate(64),otherManager=m.allocate(0x2ac450),transfer=m.allocate(0xd8);
    m.u32(enemy,manager);m.u32(manager+0x320,owner);m.u32(manager+0x324,other);m.u32(owner+4,player);m.u32(player+12,owner);m.u32(other+0x10,otherManager);
    const events=[];const event=values=>{const b=Buffer.alloc(48);values.forEach((v,i)=>b.writeUInt32LE(v>>>0,i*4));events.push(b);};const vec=p=>m.readWords(p,3);
    m.hooks.push(m.cpu.hook_add(m.uc.HOOK_CODE,()=>event([0]),null,0x40f8c0,0x40f8c0));
    m.hooks.push(m.cpu.hook_add(m.uc.HOOK_CODE,()=>event([6,m.u32(m.reg('ESP')+4)]),null,0x41bc90,0x41bc90));
    m.replace(0x40f8b0,'chain-reward',()=>{const a=m.readWords(m.reg('ESP')+4,5);event([1,...vec(a[0]),...a.slice(1)]);m.u32(player+0x30448,m.u32(player+0x30448)+1);return 0;},5);
    m.replace(0x43e380,'defeat-sound',()=>{event([2,...m.readWords(m.reg('ESP')+4,2)]);return 0;},2);
    m.replace(0x40cc00,'defeat-effect',()=>{const a=m.readWords(m.reg('ESP')+4,4);event([3,a[0],...vec(a[1])]);return 0;},4);
    m.replace(0x41d110,'defeat-explosion',()=>{const a=m.readWords(m.reg('ESP')+4,6);event([4,...vec(a[0]),...a.slice(1)]);return 0;},6);
    m.replace(0x41db90,'item-drop',()=>{const a=m.readWords(m.reg('ESP')+4,2);event([5,a[0],...vec(a[1])]);return 0;},2);
    m.replace(0x41a320,'charge-level',()=>{event([7,m.u32(m.reg('ESP')+4)]);return 0;},1);
    m.replace(0x40ccc0,'attack-transfer',()=>{const a=m.readWords(m.reg('ESP')+4,5);event([8,a[0],...vec(a[1]),...vec(a[2])]);return transfer;},5);
    let checks=0,transfers=0;const cases=[];
    for(let kind=0;kind<8;++kind)for(const captured of [0,0x1000])for(const source of [0,1,2])for(const side of [0,1])for(const count of [0,9,10,24,25,50,51,100,101,128,129])cases.push({flags:(kind<<6)|captured,source,side,count});
    for(const flags of [0x400,0x800,0xc00,0x2000,0x2440,0x2c80])for(const source of [0,1,2])for(const side of [0,1])cases.push({flags,source,side,count:4});
    try{
        for(const [index,s] of cases.entries()){
            events.length=0;const effect=[0,1,2,3,-1][index%5],opposing=index%4===0?25:24,character=index%3===0?1:0,gauge=[99.25,199.5,299.75,399.125,17.25][index%5],seed=(index*137+5741)&65535;
            c.enemy_death_setup(f,s.flags,index%128,effect,s.count,opposing,s.side,character,gauge,seed);
            m.u32(enemy+0x3380,s.flags);m.u32(enemy+0x2e58,index%128);m.write(enemy+0x3368,[effect&255]);m.u32(enemy+0x335c,3);[71.375,95.625,.5].forEach((v,i)=>m.f32(enemy+0x2d74+i*4,v));[76.875,100.75,0].forEach((v,i)=>m.f32(enemy+0x2dd4+i*4,v));
            m.u32(player+0x30448,s.count);m.f32(player+0x30388,gauge);m.u32(owner+0x20,character);m.u32(otherManager+0x2ac3b8,opposing);m.u32(manager+0x31c,s.side);
            for(let side=0;side<2;++side){m.u32(0x4b3178+side*0xf0+0xcc,32+side*304);m.u32(0x4b3178+side*0xf0+0xd0,16);}
            m.u32(0x4b3448,0x4b3178+s.side*0xf0);m.f32(0x4a80e0,17.25);m.f32(0x4a80e4,13.125);m.f32(0x4a80e8,384);
            m.u32(0x4ace0c,seed);m.u32(0x4ace10,0);m.view(transfer,0xd8).fill(0);
            m.call(0x4102b0,{ecx:enemy,args:[s.source]});assert.equal(c.enemy_death_run(f,s.source),1,'defeat result '+index);
            assert.equal(c.enemy_death_event_count(f),events.length,'events '+index);assert.deepEqual(Buffer.from(memory(c,c.enemy_death_events(f),events.length*48)),Buffer.concat(events),'event parameters '+index);
            for(const [part,p,n] of [[0,0x4ace0c,8],[1,player+0x30388,4],[2,transfer+0xa0,10],[3,player+0x30448,4]])assert.deepEqual(memory(c,c.enemy_death_part(f,part),n),m.bytes(p,n),'state '+index+'/'+part);
            if(events.some(e=>e.readUInt32LE(0)===8))++transfers;++checks;
        }
        report('enemy-death',{checks,transfers,scope:'Original enemy defeat decision path, numeric rewards, actual charge gauge, transfers and RNG; combo, effect and item managers are recorded boundaries.'});
    }finally{c.enemy_death_delete(f);m.close();}
});
