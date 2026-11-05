import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {oracle,core,memory,report,root,bits} from './helpers.mjs';
test('Integrated player frame preserves original component order and combined state for every character',async()=>{
    const m=await oracle(),c=await core(),q=m.allocate(0x31000),op=m.allocate(0x31000),enemy=m.allocate(0x2b0000),scene=m.allocate(0x12000),config=m.allocate(512),effect=m.allocate(512),focus=m.allocate(512),sprite=m.allocate(0x44),name=m.allocate(32),data=c.allocate(65536),tmp=c.allocate(12);
    let f=0,parts=[],dv,events=[],loaded=0,frame=0,checks=0,draws=0,side=0,own=0;
    const arg=i=>m.u32(m.reg('ESP')+4+i*4),event=(...v)=>events.push([...v,...Array(8-v.length).fill(0)].map(n=>n>>>0));
    const nativeShot=i=>q+0xc11c+i*0x484,index=p=>p===q+0xc0?1000:p>=q+0x30454&&p<q+0x30f64?2000+(p-q-0x30474)/708:(p-nativeShot(0))/0x484;
    m.replace(0x42c970,'shot-resource',()=>loaded,1);
    for(const address of [0x403d90,0x401390])m.replace(address,'render-viewport',()=>{m.u32(0x4b3448,0x4b3178+arg(0)*0xf0);m.u32(0x4b344c,arg(0));return 0;},1);
    m.replace(0x403e00,'player-animation-start',()=>{const p=arg(0),script=arg(1);event(0,index(p),script);m.view(p,0x2a4).fill(0);m.u32(p+0x1f8,1);m.u32(p+0x1f0,0xffffffff);m.write(p+0x1fc,[1,0]);m.write(p+0x21a,[script&255,script>>>8]);m.u32(p+0x224,sprite);return 0;},2);
    m.replace(0x436f30,'player-animation-advance',()=>{const i=index(arg(0));event(1,i);return i<128&&(frame+i)%107===106?1:0;},1);
    for(const [address,fading] of [[0x43ab50,0],[0x436d70,1],[0x43a950,0]])m.replace(address,'player-draw',()=>{event(2,index(arg(0)),fading);return 0;},1);
    m.replace(0x43e380,'positioned-sound',()=>{event(3,arg(0),arg(1));return 0;},2);m.replace(0x43e2f0,'sound',()=>{event(4,arg(0),arg(1));return 0;},2);
    m.replace(0x40cd70,'slotted-effect',()=>{event(5,arg(0),...m.readWords(arg(1),3),arg(2),arg(3));return arg(0)===7?focus:effect;},4);m.u32(focus+4,focus+16);
    m.replace(0x445560,'character-effect',()=>effect,1);m.write(effect+0xc4,[0]);
    m.replace(0x403e50,'opponent-attack',()=>{event(6,arg(0),arg(1));return 0;},4);m.replace(0x40f860,'score',()=>{event(7,arg(0));return 0;},1);
    m.replace(0x4346a0,'score-popup',()=>{event(8,...m.readWords(arg(1),3),arg(2),arg(3));return 0;},4);
    const meter=m.registerImport({dll:'test',name:'character-meter',handler:()=>{event(9);return 0;},argc:0});
    m.replace(0x418950,'survival-time',()=>{event(11,arg(0));return 0;},1);m.replace(0x41a3c0,'survival-expired',()=>{event(12);return 0;});
    m.replace(0x41a270,'charge-begin',()=>{event(13);return 0;});m.replace(0x41a2d0,'charge-end',()=>{event(14);return 0;});m.replace(0x40f7d0,'boss-query',()=>{event(15);return 0;},1);
    m.replace(0x40cc00,'protection',()=>{event(16,arg(0),...m.readWords(arg(1),3));return effect;},4);m.replace(0x415d70,'winner',()=>{event(17,arg(0));return 0;},1);m.replace(0x41a380,'critical',()=>{event(18);return 0;},1);
    m.replace(0x406790,'focus-end',()=>{event(19);return 0;},1);m.replace(0x41a320,'charge-level',()=>{event(20,arg(0));return 0;},1);m.replace(0x41a3e0,'critical-time',()=>{event(21,arg(0));return 0;},1);
    let motionFields,controlFields;
    function copyFields(){for(let which=0;which<2;++which)for(const [orig,off,size] of which?controlFields:motionFields)m.write(q+orig,memory(c,parts[which]+off,size));}
    function set(which,orig,n,float=false){const entry=(which?controlFields:motionFields).find(x=>x[0]===orig);dv=new DataView(c.memory.buffer);if(float){dv.setFloat32(parts[which]+entry[1],n,true);m.f32(q+orig,n);}else{dv.setInt32(parts[which]+entry[1],n,true);m.i32(q+orig,n);}}
    function compareAnm(at,native,label){const actual=memory(c,at,0x2a4),expected=m.bytes(native,0x2a4);assert.deepEqual(actual.subarray(0,0x224),expected.subarray(0,0x224),label);assert.deepEqual(actual.subarray(0x228),expected.subarray(0x228),label+' tail');}
    function compare(label,checkShots=true){
        for(let which=0;which<2;++which)for(const [orig,off,size] of which?controlFields:motionFields)assert.deepEqual(memory(c,parts[which]+off,size),m.bytes(q+orig,size),label+' player '+orig.toString(16));
        compareAnm(parts[2],q+0xc0,label+' body');assert.deepEqual(memory(c,parts[4],120),m.bytes(q+36,120),label+' cpu');assert.deepEqual(memory(c,parts[5],60),m.bytes(q+0x30414,60),label+' combo');assert.deepEqual(memory(c,parts[6],8),m.bytes(0x4ace0c,8),label+' RNG');
        assert.deepEqual(memory(c,parts[9],513*68),m.bytes(q+0x28bc,513*68),label+' areas');assert.equal(c.player_frame_area_count(f),m.i32(q+0xc110),label+' area count');
        for(let i=0;i<4;++i){assert.deepEqual(memory(c,parts[10]+i*708,32),m.bytes(q+0x30454+i*708,32),label+' item'+i);compareAnm(parts[10]+i*708+32,q+0x30474+i*708,label+' itemANM'+i);}
        for(const [i,orig,size] of [[11,0x30364,12],[12,0x30370,12],[14,0x30f64,8],[15,0x3031c,12]])assert.deepEqual(memory(c,parts[i],size),m.bytes(q+orig,size),label+' extra'+orig.toString(16));
        if(checkShots)for(let i=0;i<128;++i){const a=c.player_frame_shot(f,i),n=nativeShot(i);compareAnm(a,n,label+' shot'+i);assert.deepEqual(memory(c,a+0x2a4,0x1d0),m.bytes(n+0x2a4,0x1d0),label+' shot motion'+i);}
        assert.equal(c.player_frame_beam(f),m.u32(q+0x30328)?(m.u32(q+0x30328)-nativeShot(0))/0x484:-1,label+' beam');assert.equal(c.player_hazards_count(parts[8]),m.u32(q+0x1b70),label+' hazards');
        assert.equal(c.player_frame_life_value(f,0),m.i32(q+0x30334),label+' display');assert.equal(c.player_frame_life_value(f,1),m.i32(q+0x3032c),label+' hidden');assert.equal(c.player_frame_life_value(f,2),m.u32(q+0x364)?1:0,label+' focus');
        dv=new DataView(c.memory.buffer);for(let j=0;j<3;++j)assert.equal(dv.getInt32(parts[7]+j*4,true),m.i32([0x4a7e48,0x4a7e4c,0x4a7e5c][j]),label+' damage'+j);
        assert.equal(c.player_frame_event_count(f),events.length,label+' event count: '+JSON.stringify(events));const p=c.player_frame_events(f);events.forEach((e,i)=>assert.deepEqual(Array.from({length:8},(_,j)=>dv.getUint32(p+(i*8+j)*4,true)),e,label+' event'+i));++checks;
    }
    try{for(let character=0;character<16;++character)for(side=0;side<2;++side){
        f=c.player_frame_fixture();parts=Array.from({length:16},(_,i)=>c.player_frame_part(f,i));dv=new DataView(c.memory.buffer);const fields=(ptr,count)=>Array.from({length:count},(_,i)=>Array.from({length:3},(_,j)=>dv.getUint32(ptr+(i*3+j)*4,true)));motionFields=fields(c.motion_fields(),c.motion_field_count());controlFields=fields(c.shot_control_fields(),c.shot_control_field_count());
        m.view(q,0x31000).fill(0);m.view(op,0x31000).fill(0);m.view(enemy,0x2b0000).fill(0);m.view(scene,0x12000).fill(0);m.view(config,512).fill(0);
        own=0x4a7d90+side*0x38;const other=0x4a7d90+(1-side)*0x38;m.view(0x4a7d90,0x70).fill(0);m.u32(q+12,own);m.u32(q+16,other);m.u32(own+4,q);m.u32(other+4,op);m.u32(own+12,effect);m.u32(other+12,effect);m.u32(own+16,enemy);m.u32(other+16,enemy);m.u32(own+24,effect);m.u32(other+24,effect);m.u32(own+32,character);m.u32(q+0x36c,q);m.u32(q+0x9c,q);m.u32(q+0x30410,q);m.u32(q+0x30450,meter);m.u32(0x4a7e38,scene);m.u32(0x4a7e78,config);m.u32(0x4a7ea8,0);m.u32(0x4a7ec4,0);m.u32(0x4b36d4,0);m.f32(0x4b36b8,1);
        const raw=readFileSync(resolve(root,'reference/assets',`pl${String(character).padStart(2,'0')}.sht`));memory(c,data,raw.length).set(raw);const cpu=side===1?1:0;assert.equal(c.player_frame_load(f,data,raw.length,side,character,cpu),1);c.player_frame_setup(f,1,5);m.u32(q+32,cpu);m.u32(0x4a7e44,5);m.u32(0x4a7eac,1);
        loaded=m.allocate(raw.length);m.write(loaded,raw);m.reg('ESI',q+0x30338);m.call(0x41bbe0,{ecx:name});copyFields();m.write(q+36,memory(c,parts[4],120));m.write(q+0x30414,memory(c,parts[5],60));m.write(q+0xc0,memory(c,parts[2],0x2a4));m.write(q+0x28bc,memory(c,parts[9],513*68));for(let i=0;i<512;++i)m.u32(q+0xb908+i*4,q+0x28bc+i*68);m.u32(q+0xc114,512);
        for(let i=0;i<128;++i)m.write(nativeShot(i),memory(c,c.player_frame_shot(f,i),0x474));m.write(q+0x30454,memory(c,parts[10],4*708));m.write(q+0x30364,memory(c,parts[11],12));m.write(q+0x30370,memory(c,parts[12],12));m.write(q+0x3031c,memory(c,parts[15],12));m.write(q+0x30f64,memory(c,parts[14],8));
        dv=new DataView(c.memory.buffer);dv.setUint32(parts[6],2345+character*733,true);dv.setUint32(parts[6]+4,0,true);m.write(0x4ace0c,memory(c,parts[6],8));dv.setInt32(parts[7],4,true);dv.setInt32(parts[7]+8,3100,true);m.u32(0x4a7e48,4);m.u32(0x4a7e4c,0);m.u32(0x4a7e5c,3100);
        for(let j=0;j<4;++j)m.f32(0x4a80f0+j*4,[-184,32,368,416][j]);m.f32(0x4a80e0,0);m.f32(0x4a80e4,0);m.f32(0x4a80e8,288);m.f32(0x4a80ec,448);for(let s=0;s<2;++s){m.u32(0x4b3178+s*0xf0+0xcc,32+s*304);m.u32(0x4b3178+s*0xf0+0xd0,16);}m.u32(0x4a8108+side*4,10);m.u32(0x4a7e90,0);
        events=[];m.call(0x41ebc0,{ecx:q});c.player_frame_reset(f,10,1,0);compare(`init ${character}/${side}`);set(1,0,1);
        for(frame=0;frame<140;++frame){
            const gameFlags=frame===37?0x800:0,fieldFlags=frame===72?1:0,dialogue=frame>=40&&frame<44,locked=frame===66?1:0,auto=character%2,rate=frame%47===0?.5:1;
            memory(c,parts[3],88).fill(0);dv=new DataView(c.memory.buffer);const held=[0,0x81,0x45,1,0x22,0x11,0x91][Math.floor(frame/11)%7];dv.setUint16(parts[3]+42,frame%12,true);dv.setUint16(parts[3]+44,held,true);dv.setUint16(parts[3]+50,frame%9===0?held:0,true);m.write(0x4ace18+side*0x8e,memory(c,parts[3],88));
            m.u32(0x4a7ec4,gameFlags);m.u32(own+52,fieldFlags);m.i32(scene+0xe94c,dialogue?0:-1);m.u32(scene+0x1095c,locked);m.write(config+0xb4+side,[auto]);m.f32(0x4b36b8,rate);
            if(frame===51||frame===94){set(1,0x30388,400,true);set(1,0x30384,299,true);}
            if(frame===95&&side===0){set(1,0,0);const shock=controlFields.find(x=>x[0]===0x1b74);memory(c,parts[1]+shock[1],12).fill(0);m.view(q+0x1b74,12).fill(0);const motion=motionFields.find(x=>x[0]===0x1b88);memory(c,tmp,12).set(memory(c,parts[0]+motion[1],12));c.player_hazards_add(parts[8],0,tmp,0,0,8,0,0);}
            const hazards=c.player_hazards_count(parts[8]);m.u32(q+0x1b70,hazards);m.write(q+0x370,memory(c,c.player_hazards_entries(parts[8]),128*48));events=[];m.call(0x41e900,{ecx:q});c.player_frame_step(f,gameFlags,fieldFlags,dialogue,auto,locked,rate,frame);compare(`${character}/${side}/${frame}`);
            if(frame%41===40){for(let fading=0;fading<2;++fading){events=[];m.call(fading?0x41cdc0:0x41eb10,{ecx:q});c.player_frame_draw(f,fading);compare(`draw ${character}/${side}/${frame}/${fading}`);++draws;}}
        }
        c.player_frame_delete(f);f=0;
    }report('player-frame',{checks,draws,characters:16,scope:'Integrated original player round reset and main frame, actual motion, AI, charge control, shots, hazards, damage, recovery, areas and combo timers. ANM execution, sound/effect ownership, HUD and opponent scheduling remain recorded service boundaries.'});
    }finally{if(f)c.player_frame_delete(f);c.release(data);c.release(tmp);m.close();}
});
