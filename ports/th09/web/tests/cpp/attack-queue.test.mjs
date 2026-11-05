import test from 'node:test';
import assert from 'node:assert/strict';
import {oracle,core,memory,report} from './helpers.mjs';
test('Attack queue and reconstructed character attacks reproduce native lifecycle, movement, collision and allocation limits',async()=>{
    const m=await oracle(),c=await core(),f=c.attack_queue_fixture(),manager=m.allocate(0x4c80),position=m.allocate(12),pos=c.allocate(12);
    const players=[m.allocate(0x30500),m.allocate(0x30500)],effects=[m.allocate(0x300),m.allocate(0x300)],shared=[m.allocate(8),m.allocate(8)],shots=[m.allocate(8),m.allocate(8)],enemies=[m.allocate(8),m.allocate(8)],bullets=[m.allocate(0x260000),m.allocate(0x260000)],emitted=m.allocate(0x10c4);
    let dv=new DataView(c.memory.buffer);const fields=Array.from({length:c.attack_queue_field_count()},(_,i)=>Array.from({length:3},(_,j)=>dv.getUint32(c.attack_queue_fields()+(i*3+j)*4,true)));
    let events=[],bulletEvents=[],drawBytes=[],collision=0,checks=0,creations=0,drawChecks=0,conversionChecks=0,activeKind=0;const kinds=[0,1,2,3,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26];
    const actor=n=>manager+0x1c+n*0x4c,index=p=>(p-actor(0))/0x4c,arg=n=>m.u32(m.reg('ESP')+4+n*4),vec=p=>m.readWords(p,3);
    const event=(...values)=>events.push([...values,...Array(16-values.length).fill(0)].map(v=>v>>>0));
    function animationOwner(ptr){for(let n=0;n<256;++n){const start=m.u32(actor(n)+0x1c),count=m.u32(actor(n)+0x2c);if(start&&ptr>=start&&ptr<start+count*0x2a4)return [n,(ptr-start)/0x2a4];}throw Error('unowned animation '+ptr.toString(16));}
    m.replace(0x43e2f0,'attack-sound',()=>{event(0,arg(0),arg(1));return 0;},2);
    m.replace(0x401560,'attack-animation-start',()=>{const p=arg(0),[n,slot]=animationOwner(p),ref=m.reg('ECX');let side=shared.indexOf(ref),resource=0;if(side<0){side=shots.indexOf(ref);resource=1;}assert.ok(side>=0);event(2,n,slot,side,resource,arg(1));m.view(p,0x2a4).fill(0);m.u32(p+0x1f8,1);m.write(p+0x21a,[arg(1)&255,(arg(1)>>>8)&255]);return 0;},2);
    m.replace(0x436f30,'attack-animation-step',()=>{const p=arg(0),[n,slot]=animationOwner(p);event(1,n,slot,(m.u32(p+0x21a)<<16)>>16);if((activeKind===19||activeKind===21)&&m.i32(actor(n)+24)>330)m.u32(p+0x1f8,m.u32(p+0x1f8)&~1);return 0;},1);
    m.replace(0x41dff0,'attack-shot-collision',()=>{const side=players.indexOf(m.reg('ECX'));assert.ok(side>=0);event(3,side,...vec(arg(0)),...vec(arg(1)));return collision;},3);
    m.replace(0x440d30,'attack-player-collision',()=>{const side=players.findIndex(p=>p+0x36c===m.reg('ECX'));assert.ok(side>=0);const p=vec(arg(0));
        // Medicine's temporary look-ahead vector initializes X/Y only. Z is
        // stack garbage and is unused by the original 2D collision system.
        if(activeKind===18||activeKind===24)p[2]=0;event(4,side,...p,arg(1));return 0;},3);
    m.replace(0x40f8e0,'attack-player-box-collision',()=>{const side=players.findIndex(p=>p+0x36c===m.reg('ECX'));assert.ok(side>=0);event(8,side,...vec(arg(0)),...vec(arg(1)));return 0;},3);
    m.replace(0x4417c0,'attack-enemy-spawn',()=>{const side=enemies.indexOf(m.reg('ECX'));assert.ok(side>=0);event(5,side,(arg(0)<<16)>>16,...vec(arg(1)),arg(2),(arg(3)<<24)>>24,arg(4),m.u32(arg(5)+32));return 0;},6);
    m.replace(0x40cc00,'attack-effect',()=>{const side=effects.indexOf(m.reg('ECX'));assert.ok(side>=0);event(6,side,arg(0),...vec(arg(1)));return 0;},4);
    for(const [address,secondary] of [[0x4130f0,0],[0x4131c0,1]])m.replace(address,'attack-bullet-emission',()=>{const side=bullets.indexOf(m.reg('ECX'));assert.ok(side>=0);event(7,side,secondary);bulletEvents.push(m.bytes(arg(0),0x210));return emitted;},1);
    const device=m.allocate(4),vtable=m.allocate(0x140);m.u32(device,vtable);m.u32(0x4b3108,device);
    for(const [slot,argc] of [[0xfc,4],[200,3],[0x130,2]])m.u32(vtable+slot,m.registerImport({dll:'draw-boundary',name:'unused-state',argc,handler:()=>0}));
    m.u32(vtable+0x120,m.registerImport({dll:'draw-boundary',name:'line-strip',argc:5,handler:()=>{assert.equal(arg(1),3);assert.equal(arg(4),20);const count=arg(2)+1;event(11,count);drawBytes.push(m.bytes(arg(3),count*20));return 0;}}));
    for(const address of [0x4396a0,0x40e380,0x40e390,0x4015f0,0x415d00,0x40e370,0x415d10])m.replace(address,'draw-state-boundary',()=>0);
    for(const [address,kind,stride] of [[0x43b7b0,9,20],[0x43b8f0,10,28]])m.replace(address,'custom-fan-boundary',()=>{event(kind,arg(2));drawBytes.push(m.bytes(arg(1),arg(2)*stride));return 0;},3);
    m.replace(0x403d90,'select-draw-layer',()=>{const layer=arg(0);m.u32(0x4b3448,0x4b3100+0x78+layer*0xf0);m.u32(0x4b344c,layer);event(12,layer);return 0;},1);
    m.replace(0x43ab50,'draw-attack-animation',()=>{const [n,slot]=animationOwner(arg(0));event(13,n,slot);return 0;},1);
    for(let side=0;side<2;++side){m.u32(0x4a7d94+side*0x38,players[side]);m.u32(0x4a7d98+side*0x38,bullets[side]);m.u32(0x4a7d9c+side*0x38,effects[side]);m.u32(0x4a7da0+side*0x38,enemies[side]);m.u32(effects[side]+0x2d4,shared[side]);m.u32(players[side]+0xbc,shots[side]);m.u32(manager+0x14+side*4,256);const p=new Float32Array([side?70:-115,side?350:310,0]);memory(c,c.attack_queue_players(f)+side*12,12).set(new Uint8Array(p.buffer));m.write(players[side]+0x1b88,new Uint8Array(p.buffer));}
    const heap=m.heap;
    const bulletFields=Array.from({length:c.bullet_field_count()},(_,i)=>Array.from({length:3},(_,j)=>dv.getUint32(c.bullet_fields()+(i*3+j)*4,true)));
    const bulletField=offset=>bulletFields.find(([original])=>original===offset);
    const nativeBullet=(side,index)=>bullets[side]+0x1a900+index*0x10c4;
    m.u32(0x4a7e3c,manager);
    function setGeometry(){for(let side=0;side<2;++side){const p=c.attack_queue_part(f,1)+side*20,x=32+side*304,y=16;dv.setUint32(p,x,true);dv.setUint32(p+4,y,true);dv.setFloat32(p+8,9.375,true);dv.setFloat32(p+12,3.75,true);dv.setFloat32(p+16,288,true);m.u32(0x4b3100+0x78+side*0xf0+0xcc,x);m.u32(0x4b3100+0x78+side*0xf0+0xd0,y);}m.f32(0x4a80e0,9.375);m.f32(0x4a80e4,3.75);m.f32(0x4a80e8,288);}
    function compareEvents(label){dv=new DataView(c.memory.buffer);assert.equal(c.attack_queue_event_count(f),events.length,label+' event count');const p=c.attack_queue_events(f);events.forEach((e,i)=>{const actual=Array.from({length:16},(_,j)=>dv.getUint32(p+(i*16+j)*4,true));assert.deepEqual(actual,e,label+' event'+i);});assert.equal(c.attack_queue_bullet_count(f),bulletEvents.length,label+' bullet count');bulletEvents.forEach((b,i)=>assert.deepEqual(memory(c,c.attack_queue_bullets(f)+i*0x210,0x210),b,label+' bullet'+i));}
    function compare(label,count=1,draw=true){
        for(let n=0;n<count;++n){const cpp=c.attack_queue_actor(f,n),native=actor(n);for(const [offset,local,size] of fields)assert.deepEqual(memory(c,cpp+local,size),m.bytes(native+offset,size),`${label} actor${n}/${offset.toString(16)}`);
            const state=c.attack_queue_travel(f,n),size=c.attack_queue_travel_size(f,n);if(state)assert.deepEqual(memory(c,state,size),m.bytes(m.u32(native+0x34),size),label+' travel');
            const orig=m.u32(native+0x1c),count=orig?m.u32(native+0x2c):0;assert.equal(c.attack_queue_animation_count(f,n),count,label+' animation lifetime');for(let slot=0;slot<count;++slot)assert.deepEqual(memory(c,c.attack_queue_animation(f,n,slot),0x2a4),m.bytes(orig+slot*0x2a4,0x2a4),label+' animation'+slot);
        }
        assert.deepEqual(memory(c,c.attack_queue_part(f,0),8),m.bytes(0x4ace0c,8),label+' random');
        assert.deepEqual(memory(c,c.attack_queue_part(f,3),8),m.bytes(manager+12,8),label+' counts');
        for(let side=0;side<2;++side)assert.deepEqual(memory(c,c.attack_queue_effect_scales(f)+side*8,8),m.bytes(players[side]+0x1ce4,8),label+' player effect');
        if(draw)for(let layer=0;layer<3;++layer){let ptr=m.u32(manager+0x4c68+layer*4),n=0;while(ptr){assert.ok(n<256);assert.equal(c.attack_queue_draw(f,layer,n++),index(ptr),label+' draw order');ptr=m.u32(ptr+0x30);}assert.equal(c.attack_queue_draw(f,layer,n),-1);}
        compareEvents(label);++checks;
    }
    function create(kind,side,p=[-32.75,187.125,.25]){
        activeKind=kind;
        const bytes=Buffer.alloc(12);p.forEach((v,i)=>bytes.writeFloatLE(v,i*4));m.write(position,bytes);memory(c,pos,12).set(bytes);m.call(0x401440,{ecx:0x4b3100,args:[side]});events=[];bulletEvents=[];
        const expected=m.call(0x4151f0,{ecx:manager,args:[kind,position,side,kind===22?position:0]}),actual=c.attack_queue_create(f,kind,side,pos,kind===22?pos:0);assert.equal(actual,index(expected));++creations;compareEvents('creation');return actual;
    }
    function step(rate,paused=0,frozen=0){events=[];bulletEvents=[];m.f32(0x4b36b8,rate);m.u32(0x4a7ec4,paused);m.u32(0x4a7dc4,frozen);assert.equal(m.call(0x415340,{ecx:manager}),1);assert.equal(c.attack_queue_step(f,rate,0,paused,frozen,collision),1);}
    function clear(){events=[];bulletEvents=[];m.call(0x4156f0,{ecx:manager});c.attack_queue_clear(f);compareEvents('clear');}
    function draw(kind,side,label){
        events=[];bulletEvents=[];drawBytes=[];m.call(0x401440,{ecx:0x4b3100,args:[1-side]});m.call(kind===14||kind===22?0x444130:0x44b940,{ecx:actor(0)});c.attack_queue_render(f,0);compare(label);
        const expected=Buffer.concat(drawBytes);assert.equal(c.attack_queue_draw_size(f),expected.length,label+' draw size');assert.deepEqual(memory(c,c.attack_queue_draw_bytes(f),expected.length),new Uint8Array(expected),label+' draw vertices');++drawChecks;
    }
    function drawQueue(label,count){
        events=[];bulletEvents=[];drawBytes=[];m.call(0x415460,{ecx:manager});m.call(0x415580,{ecx:manager});c.attack_queue_render_all(f);compare(label,count);
        const expected=Buffer.concat(drawBytes);assert.equal(c.attack_queue_draw_size(f),expected.length);assert.deepEqual(memory(c,c.attack_queue_draw_bytes(f),expected.length),new Uint8Array(expected),label+' vertices');++drawChecks;
    }
    function seedConversion(side){
        // Mix ineligible, boundary, outside and eligible bullets in each pool.
        for(const begin of [0,176])for(let i=0;i<7;++i){const n=begin+i,original=nativeBullet(side,n),local=c.attack_queue_bullet(f,side,n);
            const data=Buffer.alloc(0x10c4);data.writeInt16LE(i===0?0:1,0xdbe);data[0x10be]=i===1?1:0;data.writeInt32LE(i===2?0x14a:7,0xd48);
            data.writeFloatLE(m.f32(actor(0)+32)+(i===3?32:i===4?33:i-5),0xd4c);data.writeFloatLE(m.f32(actor(0)+36),0xd50);data.writeFloatLE(.25*i,0xd70);data.writeFloatLE(-1.5+i*.1,0xd7c);
            m.write(original,data);for(const [offset,target,size] of bulletFields)memory(c,local+target,size).set(data.subarray(offset,offset+size));
        }
    }
    function compareConversion(side){
        const [,offset]=bulletField(0x10be);for(const begin of [0,176])for(let i=0;i<7;++i)assert.equal(memory(c,c.attack_queue_bullet(f,side,begin+i)+offset,1)[0],m.bytes(nativeBullet(side,begin+i)+0x10be,1)[0],'conversion marker');
        assert.equal(memory(c,c.attack_queue_emitted(f)+offset,1)[0],m.bytes(emitted+0x10be,1)[0],'emitted conversion marker');++conversionChecks;
    }
    try{
        setGeometry();
        for(const kind of kinds)for(let side=0;side<2;++side)for(const rate of [1,.5,.99])for(const cancel of [false,true]){
            clear();m.heap=heap;dv.setUint32(c.attack_queue_part(f,0),0x631f+kind*17+side,true);dv.setUint32(c.attack_queue_part(f,0)+4,0,true);m.write(0x4ace0c,memory(c,c.attack_queue_part(f,0),8));
            for(let n=0;n<2;++n){dv.setFloat32(c.attack_queue_effect_scales(f)+n*8,1,true);dv.setFloat32(c.attack_queue_effect_scales(f)+n*8+4,1,true);m.f32(players[n]+0x1ce4,1);m.f32(players[n]+0x1ce8,1);
                const positionBytes=new Uint8Array(new Float32Array([n?70:-115,n?350:310,0]).buffer);m.write(players[n]+0x1b88,positionBytes);memory(c,c.attack_queue_players(f)+n*12,12).set(positionBytes);
            }
            const rank=(kind+side*7+Number(cancel)*11)%23,difficulty=(kind+side+Number(cancel))%5,level=kind+Number(cancel)*5;c.attack_queue_settings(f,rank,difficulty,level);m.i32(0x4a7e44,rank);m.i32(0x4a7eac,difficulty);for(let n=0;n<2;++n){m.i32(players[n]+0xa0,level+n*3);m.i32(players[n]+0xa4,level*2+n);}
            const actors=kind===3?2:1;assert.equal(create(kind,side),0);compare(`init ${kind}/${side}`,actors,false);
            let finished=false;
            for(let frame=0;frame<1300;++frame){
                if([14,19,22].includes(kind)&&m.u32(m.u32(actor(0)+0x34))>0){const p=1-side;for(let axis=0;axis<2;++axis){const value=m.f32(actor(0)+32+axis*4)+(axis?0:32);m.f32(players[p]+0x1b88+axis*4,value);dv.setFloat32(c.attack_queue_players(f)+p*12+axis*4,value,true);}}
                const conversion=kind===21&&m.u32(m.u32(actor(0)+0x34))===1;if(conversion&&frame%37===0)seedConversion(1-side);
                collision=cancel&&frame>50?2:0;step(rate,frame<3?0x800:0,frame>=8&&frame<12?1:0);const label=`kind${kind}/side${side}/rate${rate}/cancel${cancel}/frame${frame}`;compare(label,actors);
                if(conversion)compareConversion(1-side);if(Array.from({length:actors},(_,n)=>m.u32(actor(n)+12)).every(v=>!v)){finished=true;break;}
                if(frame%29===0)drawQueue(label+' full queue rendering',actors);else if([14,19,21,22].includes(kind))draw(kind,side,label+' rendering');}
            assert.ok(finished,`attack eventually expires kind${kind}/side${side}/rate${rate}/cancel${cancel}`);
        }
        clear();m.heap=heap;collision=0;
        for(let side=0;side<2;++side){dv.setInt32(c.attack_queue_part(f,2)+side*4,130,true);m.i32(manager+0x14+side*4,130);}
        for(let n=0;n<256;++n)assert.equal(create(n%3,n%2),n);
        assert.equal(create(0,0),256,'full shared pool');compare('full capacity',257,false);step(1);compare('ordered 256 actors',256);
        for(let side=0;side<2;++side){dv.setInt32(c.attack_queue_part(f,2)+side*4,128,true);m.i32(manager+0x14+side*4,128);assert.equal(create(0,side),256,'per-side limit');assert.equal(events.length,0,'limit rejects before sound');}
        clear();compare('clear active objects',256,false);
        report('attack-queue',{checks,creations,drawChecks,conversionChecks,capacity:256,behaviors:[...kinds,4].sort((a,b)=>a-b),scope:'Original queue allocation/update/clear, all 27 character attack state/RNG callbacks, custom draw vertices and Eiki conversion in both bullet pools. Native animation/shot collision/player collision/enemy spawn/effects/bullet emission/draw submission are explicit recorded boundaries. Medicine collision temporary Z is undefined native stack data and excluded; X/Y and radius are compared.'});
    }finally{c.attack_queue_delete(f);c.release(pos);m.close();}
});
