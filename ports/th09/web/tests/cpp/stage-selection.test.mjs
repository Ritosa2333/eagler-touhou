import test from 'node:test';
import assert from 'node:assert/strict';
import {oracle,core,memory,report} from './helpers.mjs';
test('Story route weighting, opponent reuse prevention and versus stage choices match original',async()=>{
    const m=await oracle(),c=await core(),f=c.stage_selection_fixture(),parts=Array.from({length:6},(_,i)=>c.stage_selection_part(f,i)),stats=m.allocate(160),config=m.allocate(0xb0);let dv=new DataView(c.memory.buffer),encounters=[],checks=0;
    const fields=Array.from({length:c.stage_selection_field_count()},(_,i)=>Array.from({length:3},(_,j)=>dv.getUint32(c.stage_selection_fields()+(i*3+j)*4,true)));
    m.u32(0x4a7dac,stats);m.u32(0x4b42d0,config);m.replace(0x4158e0,'encounter',()=>{encounters.push((m.u32(m.reg('ESP')+4)<<24)>>24);return 0;},1);
    function set(address,value){dv=new DataView(c.memory.buffer);const field=fields.find(x=>x[0]===address);assert.ok(field);dv.setInt32(parts[0]+field[1],value,true);}
    function seed(n){dv.setUint32(parts[1],n&65535,true);dv.setUint32(parts[1]+4,0,true);}
    function step(label){
        for(const [at,off,size] of fields)m.write(at,memory(c,parts[0]+off,size));m.write(stats,memory(c,parts[2],4));m.write(stats+20,memory(c,parts[3],1));m.u32(config+0xa8,memory(c,parts[4],1)[0]);m.write(0x4ace0c,memory(c,parts[1],8));encounters=[];
        m.call(0x415910,{ecx:0x4a7d90});assert.equal(c.stage_selection_step(f),1,label+' valid');dv=new DataView(c.memory.buffer);
        for(const [at,off,size] of fields)assert.deepEqual(memory(c,parts[0]+off,size),m.bytes(at,size),label+' field '+at.toString(16));
        assert.deepEqual(memory(c,parts[5],16),m.bytes(m.u32(0x4a7e80),16),label+' route');assert.deepEqual(memory(c,parts[1],8),m.bytes(0x4ace0c,8),label+' RNG');encounters.forEach((n,i)=>assert.equal(c.stage_selection_encounter(f,i),n));assert.equal(c.stage_selection_encounter(f,encounters.length),-1);++checks;
    }
    try{
        for(let character=0;character<14;++character)for(let difficulty=0;difficulty<5;++difficulty)for(let run=0;run<24;++run){
            set(0x4a7ea8,run%2);set(0x4a7eac,difficulty);set(0x4a7db0,character);set(0x4a7e90,run%7);dv.setFloat32(parts[2],[0,1,2.5][run%3],true);memory(c,parts[3],1)[0]=run%4===0?1:0;const visited=fields.find(x=>x[0]===0x4a7e68);memory(c,parts[0]+visited[1],16).fill(0);seed(character*733+run*439+difficulty*41);
            for(let stage=0;stage<9;++stage){set(0x4a7e8c,stage);step(`story ${character}/${difficulty}/${run}/${stage}`);}
        }
        for(let difficulty=0;difficulty<5;++difficulty)for(let selector=-2;selector<17;++selector)for(let run=0;run<48;++run){
            set(0x4a7ea8,2);set(0x4a7eac,difficulty);set(0x4a7db0,run%16);set(0x4a7de8,(run*7+3)%16);set(0x4a7ec8,selector);set(0x4a7e8c,0);memory(c,parts[4],1)[0]=run%3===0?1:0;const unlocked=fields.find(x=>x[0]===0x4a81cc);for(let i=0;i<16;++i)memory(c,parts[0]+unlocked[1]+i,1)[0]=(run+i)%4===0?1:0;seed(run*7919+difficulty*41+selector);step(`versus ${difficulty}/${selector}/${run}`);
        }
        report('stage-selection',{checks,characters:14,routes:293,versusProfiles:16,scope:'Original weighted campaign route selection and RNG across complete nine-stage runs, repeated-opponent prevention, continue/CPU policy choices, and fixed/random versus backgrounds. Encounter persistence is recorded.'});
    }finally{c.stage_selection_delete(f);m.close();}
});
