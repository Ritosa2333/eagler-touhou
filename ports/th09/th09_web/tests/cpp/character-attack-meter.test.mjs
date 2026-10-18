import test from 'node:test';
import assert from 'node:assert/strict';
import {oracle,core,memory,report} from './helpers.mjs';
test('All sixteen original character combo callbacks preserve attack thresholds and consumption',async()=>{
    const m=await oracle(),c=await core(),fixture=c.special_create(),state=c.allocate(60),point=c.allocate(12),combo=m.allocate(0x44),player=m.allocate(16),position=m.allocate(12);
    m.u32(combo,player);const pos=Buffer.alloc(12);[-35.25,99.875,.5].forEach((v,i)=>pos.writeFloatLE(v,i*4));m.write(position,pos);memory(c,point,12).set(pos);
    const events=[];m.replace(0x4151f0,'character-attack-request',()=>{const args=m.readWords(m.reg('ESP')+4,4),b=Buffer.alloc(40);b.writeUInt32LE(1);b.writeUInt32LE(args[0],4);b.writeUInt32LE(args[2],8);b.set(m.bytes(args[1],12),12);events.push(b);return 0;},4);
    let checks=0,requests=0;
    try{
        for(let character=0;character<16;++character)for(let rank=0;rank<=22;++rank)for(const side of [0,1])for(const amount of [-1,0,1,10,24,49,70,100,250,1000]){
            const input=Buffer.alloc(60);input.writeInt32LE(43,40);input.writeInt32LE(checks%31,44);input.writeInt32LE(amount,48);memory(c,state,60).set(input);m.write(combo+4,input);
            events.length=0;m.u32(player+8,side);m.i32(0x4a7e44,rank);const target=m.u32(0x4a19d0+character*4);assert.equal(m.call(target,{ecx:combo,edx:position}),0);
            assert.equal(c.character_attack_meter(fixture,state,character,rank,side,point),1);assert.deepEqual(memory(c,state,60),m.bytes(combo+4,60),`meter ${character}/${rank}/${side}/${amount}`);
            assert.equal(c.special_event_count(fixture),events.length,'count');assert.deepEqual(Buffer.from(memory(c,c.special_events(fixture),events.length*40)),Buffer.concat(events),'requests');++checks;requests+=events.length;
        }
        report('character-attack-meter',{checks,requests,characters:16,rankRange:[0,22],scope:'All original per-character combo callbacks with attack creation recorded as a boundary; individual attack simulation is separate.'});
    }finally{c.special_delete(fixture);c.release(state);c.release(point);m.close();}
});
