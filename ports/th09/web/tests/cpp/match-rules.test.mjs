import test from 'node:test';
import assert from 'node:assert/strict';
import {oracle,core,memory,report} from './helpers.mjs';
test('Match difficulty, time rewards, score smoothing and extends match the original game manager',async()=>{
    const m=await oracle(),c=await core(),f=c.match_rules_fixture(),parts=Array.from({length:4},(_,i)=>c.match_rules_part(f,i));
    let dv=new DataView(c.memory.buffer),events=[],checks=0,rewards=0;
    const fields=Array.from({length:c.match_rules_field_count()},(_,i)=>[dv.getUint32(c.match_rules_fields()+8*i,true),dv.getUint32(c.match_rules_fields()+8*i+4,true)]);
    const scene=m.allocate(0x1293c),players=[m.allocate(0x31000),m.allocate(0x31000)],scores=[m.allocate(160),m.allocate(160)],enemies=[m.allocate(0x3400),m.allocate(0x3400)],dummy=m.allocate(1024),arg=i=>m.u32(m.reg('ESP')+4+i*4);
    const importHandler=m.onImport;m.onImport=e=>{if(e.name==='timeGetTime'){m.ret(1000,0);return;}importHandler(e);};
    m.replace(0x47b24e,'new',()=>m.allocate(arg(0)));m.replace(0x47b249,'free',()=>0);
    for(const a of [0x41a796,0x41a8a2,0x415910,0x4217e0,0x4307c0,0x41cde0,0x4316b0,0x4304a0,0x4343e0,0x4181e0,0x4156f0,0x4011c0,0x41ebc0,0x412470,0x40c990,0x411fd0,0x403d80,0x41a230])m.replace(a,'match-service',()=>0);
    m.replace(0x418840,'scene-create',()=>scene);m.replace(0x4157d0,'attack-create',()=>dummy);
    m.replace(0x41f170,'player-create',()=>players[m.reg('EDX')],1);
    for(const a of [0x403ae0,0x4150d0,0x412240,0x404600,0x41a5d0])m.replace(a,'field-create',()=>dummy+m.reg('ECX')*128);
    m.replace(0x40d320,'effects-create',()=>dummy+m.reg('ECX')*128,1);
    m.replace(0x43e2f0,'sound',()=>{events.push([1,arg(0),arg(1)]);return 0;},2);
    m.replace(0x40f1d0,'reward-enemy',()=>{const side=(m.reg('ECX')-dummy)/128;assert.equal(arg(0),19);assert.deepEqual(m.readWords(arg(1),3),[0,0xc2000000,0]);assert.deepEqual([arg(2),arg(3),arg(4),arg(5),arg(6)],[830,0xfffffffe,10000,0,0]);m.view(enemies[side],0x3400).fill(0);events.push(()=>[0,side,m.i32(enemies[side]+0x335c)]);++rewards;return enemies[side];},7);
    m.replace(0x41a380,'reward-notification',()=>{assert.equal(arg(0),1);events.push([2,(m.reg('ECX')-dummy)/128,0]);return 0;},1);
    function writeField(address,value){const field=fields.find(p=>p[0]===address);assert.ok(field);dv=new DataView(c.memory.buffer);dv.setInt32(parts[1]+field[1],value,true);m.i32(address,value);}
    function compare(label,withScores=true){
        dv=new DataView(c.memory.buffer);for(const [original,off] of fields)assert.equal(dv.getUint32(parts[1]+off,true),m.u32(original),label+' progress '+original.toString(16));
        assert.deepEqual(memory(c,parts[0],8),m.bytes(0x4ace0c,8),label+' RNG');assert.equal(dv.getInt32(parts[0]+8,true),m.i32(0x4a7eac));assert.equal(dv.getInt32(parts[0]+12,true),m.i32(0x4a7e44),label+' rank');
        for(let s=0;s<2;++s){assert.deepEqual(memory(c,parts[3]+s*8,8),m.bytes(players[s]+0xa0,8),label+' attack levels');if(withScores)assert.deepEqual(memory(c,parts[2]+20*s,20),m.bytes(scores[s],20),label+' score '+s);}
        const expected=events.map(e=>typeof e==='function'?e():e);assert.equal(c.match_rules_event_count(f),expected.length,label+' events');expected.forEach((e,i)=>assert.deepEqual(Array.from({length:3},(_,j)=>dv.getInt32(c.match_rules_events(f)+12*i+4*j,true)),e,label+' event '+i));++checks;
    }
    try{
        for(let mode=0;mode<3;++mode)for(let difficulty=0;difficulty<5;++difficulty)for(let stage=0;stage<10;++stage){
            m.view(0x4a7d90,0x400).fill(0);m.view(scene,0x1293c).fill(0);m.i32(scene+0xe94c,-1);m.u32(0x4b36d4,0);m.u32(0x4a7dac,scores[0]);m.u32(0x4a7de4,scores[1]);for(const p of scores)m.view(p,160).fill(0);
            m.i32(0x4a7ea8,mode);m.i32(0x4a7eac,difficulty);m.i32(0x4a7e8c,stage-(mode===2?0:1));m.u32(0x4ace0c,0x9137+stage);m.u32(0x4ace10,0);memory(c,parts[0],8).set(m.bytes(0x4ace0c,8));memory(c,parts[2],40).fill(0);
            events=[];m.call(0x41af2d);c.match_rules_initialize(f,difficulty,mode,stage);compare(`initialize ${mode}/${difficulty}/${stage}`);
            // Every retry has a different rank interval and floor; other services
            // are explicit boundaries while the original game reset remains real.
            for(let round=0;round<5;++round){
                writeField(0x4a7e90,round);writeField(0x4a7e5c,round*713);writeField(0x4a7e40,2117);writeField(0x4a7e50,1000);writeField(0x4a7e60,451);
                events=[];m.call(0x41b5c6);c.match_rules_restart(f,round);compare(`restart ${mode}/${difficulty}/${stage}/${round}`);
            }
        }
        for(let scenario=0;scenario<72;++scenario){
            const mode=scenario%3,difficulty=scenario%5,stage=scenario%10;
            c.match_rules_initialize(f,difficulty,mode,stage);dv=new DataView(c.memory.buffer);for(const [original,off] of fields)m.u32(original,dv.getUint32(parts[1]+off,true));m.i32(0x4a7ea8,mode);m.i32(0x4a7eac,difficulty);m.i32(0x4a7e44,dv.getInt32(parts[0]+12,true));
            for(let s=0;s<2;++s)m.write(players[s]+0xa0,memory(c,parts[3]+s*8,8));
            for(const address of [0x4a811c,0x4a7eb4,0x4a7ecc,0x4a7ec4,0x4a7ed0,0x4acf3a])m.u32(address,0);
            writeField(0x4a7e40,[718,1798,3598,5398,100000,0x7ffffffe][scenario%6]);writeField(0x4a7e50,718);writeField(0x4a7e5c,scenario%2?9995:10001);
            for(let s=0;s<2;++s){const p=parts[2]+s*20;dv.setFloat32(p,[0,2.5,6,7,8][(scenario+s)%5],true);dv.setUint32(p+4,[0,999995,499998,999999995][(scenario+s)%4],true);dv.setUint32(p+8,[19,500000,4000000,1000000001][(scenario*3+s)%4],true);dv.setUint32(p+12,[0,1,578910,0xffffffff][scenario%4],true);dv.setInt32(p+16,[0,4,7,99999][(scenario+s)%4],true);m.write(scores[s],memory(c,p,20));}
            for(let frame=0;frame<96;++frame){
                const dialogue=[-1,0,-2,-3][(frame+scenario)%4],flags=frame%7===0?0x1800:0,frozen=frame%13===0?1:0;m.i32(scene+0xe94c,dialogue);m.u32(0x4a7ec4,flags);m.u32(0x4a7dc4,frozen);events=[];
                const result=m.call(0x41aa5f,{ecx:0x4a7d90});assert.equal(result,1);c.match_rules_step(f,dialogue>=0||dialogue===-2,flags,frozen);compare(`step ${scenario}/${frame}`);
                for(const e of events)if(typeof e==='function'){const s=e()[1];assert.equal(m.u32(enemies[s]+0x3380),0x2000);}
            }
        }
        report('match-rules',{checks,rewards,modes:3,difficulties:5,scope:'Original game manager rank initialization, round reset, active frame rules, time reward order/RNG, score smoothing and life extends. Scene/resource creation, input/pause gates and reward enemy construction are recorded service boundaries.'});
    }finally{c.match_rules_delete(f);m.close();}
});
