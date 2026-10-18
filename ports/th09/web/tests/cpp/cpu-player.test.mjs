import test from 'node:test';
import assert from 'node:assert/strict';
import {oracle,core,memory,report,bits} from './helpers.mjs';
test('Computer opponent input, evasion, timers and attack decisions match original 1.50a',async()=>{
    const m=await oracle(),c=await core(),f=c.cpu_player_fixture(),p=m.allocate(0x31000),own=m.allocate(0x38),enemy=m.allocate(0x2b0000),scene=m.allocate(0x12000),op=m.allocate(0x31000),other=m.allocate(0x38),sht=m.allocate(0x1000),priority=m.allocate(0x3400),first=m.allocate(0x3400),tmp=c.allocate(48);
    let dv=new DataView(c.memory.buffer),events=[],checks=0;const parts=Array.from({length:9},(_,i)=>c.cpu_player_part(f,i)),bases=[p,own,enemy,scene,op,0,sht,priority,first];
    const fields=Array.from({length:c.cpu_player_field_count()},(_,i)=>Array.from({length:4},(_,j)=>dv.getUint32(c.cpu_player_fields()+(i*4+j)*4,true))),shotFields=Array.from({length:c.shot_control_field_count()},(_,i)=>Array.from({length:3},(_,j)=>dv.getUint32(c.shot_control_fields()+(i*3+j)*4,true)));
    for(const [address,size] of [[p,0x31000],[own,0x38],[enemy,0x2b0000],[scene,0x12000],[op,0x31000],[other,0x38],[sht,0x1000],[priority,0x3400],[first,0x3400]])m.view(address,size).fill(0);
    m.u32(p+12,own);m.u32(p+16,other);m.u32(own+16,enemy);m.u32(other+4,op);m.u32(other+24,other);m.u32(p+0x30338,sht);m.u32(p+0x9c,p);m.u32(p+0x36c,p);m.u32(0x4a7e38,scene);
    const arg=i=>m.u32(m.reg('ESP')+4+i*4);m.replace(0x418950,'opponent-survival-display',()=>{events.push([0,arg(0)|0]);return 0;},1);m.replace(0x41a3c0,'opponent-survival-expired',()=>{events.push([1,0]);return 0;});
    function put(base,orig,values,float=false){const entry=fields.find(x=>x[0]===base&&x[1]===orig);assert.ok(entry,[base,orig]);dv=new DataView(c.memory.buffer);for(let j=0;j<values.length;++j){if(float){dv.setFloat32(parts[1]+entry[2]+j*4,values[j],true);m.f32(bases[base]+orig+j*4,values[j]);}else{dv.setInt32(parts[1]+entry[2]+j*4,values[j],true);m.i32(bases[base]+orig+j*4,values[j]);}}}
    function shot(orig,n,float=false){const entry=shotFields.find(x=>x[0]===orig);if(float){dv.setFloat32(parts[2]+entry[1],n,true);m.f32(p+orig,n);}else{dv.setInt32(parts[2]+entry[1],n,true);m.i32(p+orig,n);}}
    function timer(at,n){dv.setInt32(at,n-1,true);dv.setFloat32(at+4,n,true);dv.setInt32(at+8,n,true);}
    function reset(scenario){
        dv=new DataView(c.memory.buffer);memory(c,parts[0],120).fill(0);timer(parts[0],scenario%60);dv.setInt32(parts[0]+12,scenario%2,true);for(let j=0;j<16;++j)dv.setInt32(parts[0]+16+j*4,scenario%4===0?[16,32][j%2]:scenario%3===0?0:(scenario+j)%9,true);
        dv.setInt32(parts[0]+80,scenario%7===0?3:0,true);dv.setInt32(parts[0]+84,scenario%9,true);dv.setFloat32(parts[0]+88,[100,175,200,300,400][scenario%5],true);timer(parts[0]+92,[0,299,599,1199,7199][scenario%5]);timer(parts[0]+104,[0,299,599,1199,7199][Math.floor(scenario/5)%5]);dv.setInt32(parts[0]+116,scenario%4,true);m.write(p+36,memory(c,parts[0],120));
        for(const [orig,off,size] of shotFields){memory(c,parts[2]+off,size).fill(0);m.view(p+orig,size).fill(0);}for(const [base,orig,off,size] of fields){memory(c,parts[1]+off,size).fill(0);m.view(bases[base]+orig,size).fill(0);}
        const side=scenario%2;m.u32(p+8,side);put(0,0,[scenario%6===0?scenario%5:0]);shot(0,scenario%6===0?scenario%5:0);put(0,0x1b74,[0,bits(0),0]);
        const pos=[[-170,-100,-70,0,95,130,180][scenario%7],[60,180,230,300,380,448][Math.floor(scenario/7)%6],.49];put(0,0x1b88,pos,true);put(0,0x1ca8,[2,2,5],true);put(0,0x1cdc,[1,1],true);put(0,0x1ce4,[1,scenario%11===0?.75:1],true);put(0,0xa8,[[10,5,1][scenario%3]]);put(1,0x2c,[scenario%71]);put(4,0x3044c,[[0,300,360,500][scenario%4]]);put(2,0x2ac3b8,[scenario%9]);put(2,0x2ac3ac,[scenario%5]);put(0,0xc110,[scenario%3]);put(1,0x34,[scenario%13===0?1:0]);
        put(0,0x30f64,scenario%3===0?[90,370,0]:[-1000,0,0],true);put(7,0x2d74,[30,200,.3],true);put(8,0x2d74,[-60,110,.3],true);put(7,0x3380,[[0,0xc00,0x2000][scenario%3]]);
        shot(0x30384,[0,99,100,199,200,300,400][scenario%7],true);shot(0x30388,[100,200,300,400][scenario%4],true);
        const modes=(scenario%23===0?1:0)|(scenario%29===0?2:0)|(scenario%4===0?4:0)|(scenario%3===1?8:0)|(scenario%3===2?16:0);c.cpu_player_modes(f,modes);m.u32(scene+0x1095c,modes&1);m.i32(scene+0xe94c,modes&2?0:-1);m.u32(0x4a7ea8,modes&4?1:0);m.u32(enemy+0x2ac444,modes&8?priority:0);m.u32(enemy+0x2ac448,modes&16?first:0);
        for(let j=0;j<4;++j){dv.setFloat32(parts[7]+j*4,[4.5,2,3.1819806,1.4142135][j],true);m.f32(sht+20+j*4,[4.5,2,3.1819806,1.4142135][j]);dv.setFloat32(parts[8]+j*4,[-184,32,368,416][j],true);m.f32(0x4a80f0+j*4,[-184,32,368,416][j]);}c.cpu_player_radius(f,4);m.f32(sht+4,4);
        dv.setUint32(parts[4],(scenario*3777)&65535,true);dv.setUint32(parts[4]+4,0,true);m.write(0x4ace0c,memory(c,parts[4],8));
        c.player_hazards_clear(parts[5]);const count=scenario%8===0?0:scenario%8===1?1:32;
        for(let j=0;j<count;++j){const angle=j*2.399963;const radius=scenario%8===1?90:4+(j%7)*9;const position=[pos[0]+Math.cos(angle)*radius,pos[1]+Math.sin(angle)*radius,.49];for(let k=0;k<3;++k)dv.setFloat32(tmp+k*4,position[k],true);c.player_hazards_add(parts[5],0,tmp,0,0,scenario%8===1?300:4+(j%5)*3,0,0);}
        m.write(p+0x370,memory(c,c.player_hazards_entries(parts[5]),128*48));m.u32(p+0x1b70,count);
    }
    function compare(label){dv=new DataView(c.memory.buffer);assert.deepEqual(memory(c,parts[0],120),m.bytes(p+36,120),label+' AI state');assert.deepEqual(memory(c,parts[3],88),m.bytes(0x4ace18+m.u32(p+8)*0x8e,88),label+' input');assert.deepEqual(memory(c,parts[4],8),m.bytes(0x4ace0c,8),label+' RNG');
        for(const [base,orig,off,size] of fields)assert.deepEqual(memory(c,parts[1]+off,size),m.bytes(bases[base]+orig,size),label+' context '+base+':'+orig.toString(16));for(const [orig,off,size] of shotFields)assert.deepEqual(memory(c,parts[2]+off,size),m.bytes(p+orig,size),label+' shot '+orig.toString(16));
        assert.equal(c.cpu_player_event_count(f),events.length,label+' events');const at=c.cpu_player_events(f);events.forEach((e,i)=>assert.deepEqual([dv.getInt32(at+i*8,true),dv.getInt32(at+i*8+4,true)],e,label+' event'));++checks;
    }
    try{for(let scenario=0;scenario<1704;++scenario){reset(scenario);for(let frame=0;frame<8;++frame){const rate=[1,.5,.99][scenario%3];events=[];m.f32(0x4b36b8,rate);m.call(0x4049a0,{ecx:p+36});c.cpu_player_step(f,rate);compare(`${scenario}/${frame}`);}}
        report('cpu-player',{checks,levels:71,scope:'Original CPU input and evasion against actual hazard collisions, survival/error policy, charge release, movement history, target tracking and RNG. HUD time display/expiry are recorded boundaries.'});
    }finally{c.cpu_player_delete(f);c.release(tmp);m.close();}
});
