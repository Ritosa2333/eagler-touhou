import test from 'node:test';
import assert from 'node:assert/strict';
import {oracle,core,memory,bits,report} from './helpers.mjs';
test('Incoming hazard registration and actual/predicted player hits match the original ordered 128-entry collector',async()=>{
    const m=await oracle(),c=await core(),hazards=c.player_hazards_create(),player=m.allocate(0x31000),sht=m.allocate(64),positions=m.allocate(36),cppPositions=c.allocate(36),cppPlayer=c.allocate(c.player_hazard_state_size());
    const collector=player+0x36c; m.u32(collector,player);m.u32(player+0x30338,sht);let seed=13517,checks=0,samples=0;const random=()=>{seed=(Math.imul(seed,1664525)+1013904223)>>>0;return seed;};
    let dv=new DataView(c.memory.buffer);const entries=c.player_hazards_entries(hazards);
    function hitIndex(p){return p?(p-collector-4)/48:-1;}
    function compare(){assert.equal(c.player_hazards_count(hazards),m.u32(collector+0x1804));assert.deepEqual(memory(c,entries,128*48),m.bytes(collector+4,128*48));++checks;}
    try{
        assert.equal(c.player_hazard_state_size(),68);
        for(let batch=0;batch<32;++batch){m.u32(collector+0x1804,0);c.player_hazards_clear(hazards);
            for(let i=0;i<140;++i){const points=new Float32Array([random()%81-40,random()%81-40,.25,random()%40,random()%40,1,random()%21-10,random()%21-10,0]);m.write(positions,new Uint8Array(points.buffer));memory(c,cppPositions,36).set(new Uint8Array(points.buffer));const radius=i%7===0?0:(random()%160)/8,angle=(random()%17-8)*.19634955,owner=i%2?0x12340000+i*16:0,kind=(i+batch)%3;
                if(kind===0)m.call(0x440d30,{ecx:collector,args:[positions,bits(radius),owner]});else if(kind===1)m.call(0x40f8e0,{ecx:collector,args:[positions,positions+12,owner]});else m.call(0x412830,{ecx:collector,args:[positions,positions+12,positions+24,bits(angle),owner]});
                c.player_hazards_add(hazards,kind,cppPositions,cppPositions+12,cppPositions+24,radius,angle,owner);compare();
            }
            for(let sample=0;sample<180;++sample){
                const state=sample%29===0?3:0,immune=sample%31===0?2:0,pos=[random()%151-75,random()%151-75,.5],size=[(random()%32)/8,(random()%32)/8,1],hitRadius=(random()%24)/8;
                dv.setInt32(cppPlayer,state,true);dv.setInt32(cppPlayer+4,immune,true);dv.setFloat32(cppPlayer+8,immune,true);dv.setInt32(cppPlayer+12,immune,true);[...pos,...size,hitRadius].forEach((x,j)=>dv.setFloat32(cppPlayer+16+j*4,x,true));
                m.i32(player,state);m.i32(player+0x1b74,immune);m.f32(player+0x1b78,immune);m.i32(player+0x1b7c,immune);m.write(player+0x1b88,memory(c,cppPlayer+16,12));m.write(player+0x1ca8,memory(c,cppPlayer+28,12));m.f32(sht+4,hitRadius);
                const predict=sample%2===1,additional=(random()%20)/8;let expected;
                if(predict){const points=new Uint8Array(new Float32Array([random()%151-75,random()%151-75,.75,1.5,2.25,.5]).buffer);m.write(positions,points);memory(c,cppPositions,24).set(points);expected=m.call(0x41d810,{ecx:collector,args:[positions,positions+12,bits(additional)]});}
                else expected=m.call(0x41da70,{ecx:collector});
                const actual=c.player_hazards_hit(hazards,cppPlayer,predict?cppPositions:0,cppPositions+12,additional),want=hitIndex(expected);
                assert.equal(actual,want,`batch${batch} sample${sample} `+JSON.stringify({predict,additional,hitRadius,query:Array.from(new Float32Array(c.memory.buffer,cppPositions,6)),actualHazard:actual<0?null:Array.from(new Float32Array(c.memory.buffer,entries+actual*48,11)),expectedHazard:want<0?null:Array.from(new Float32Array(c.memory.buffer,entries+want*48,11))}));
                assert.deepEqual(memory(c,cppPlayer+16,12),m.bytes(player+0x1b88,12),'position restored');assert.deepEqual(memory(c,cppPlayer+28,12),m.bytes(player+0x1ca8,12),'hit extent restored');assert.deepEqual(memory(c,cppPlayer+44,24),m.bytes(player+0x1c60,24),'last calculated bounds');++samples;
            }
        }
        report('player-hazards',{checks,samples,scope:'All three registration paths, original first-hit order, capacity/reuse, invulnerability gates and predictive collision with temporary player position/extent restored.'});
    }finally{c.player_hazards_delete(hazards);c.release(cppPlayer);c.release(cppPositions);m.close();}
});
