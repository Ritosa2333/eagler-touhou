import test from 'node:test';
import assert from 'node:assert/strict';
import {oracle,core,memory,report,bits} from './helpers.mjs';
test('Effect pools, cross-field transfers and all character effect states and CPU geometry match original',async()=>{
    const m=await oracle(),c=await core(),manager=m.allocate(0x2dc),pool=m.allocate(41*0xd8),position=m.allocate(12),extra=m.allocate(12),pos=c.allocate(12),args=c.allocate(12),scene=m.allocate(0x12000),sprite=m.allocate(0x44),enemy=m.allocate(0x3400),players=[m.allocate(0x31000),m.allocate(0x31000)],enemyManagers=[m.allocate(0x2b0000),m.allocate(0x2b0000)],bulletManagers=[m.allocate(16),m.allocate(16)];
    let f=0,parts=[],dv=new DataView(c.memory.buffer),events=[],bullets=[],draws=[],frame=0,checks=0,drawChecks=0,creations=0,side=0;
    const fields=Array.from({length:c.effects_field_count()},(_,i)=>Array.from({length:3},(_,j)=>dv.getUint32(c.effects_fields()+(i*3+j)*4,true))),actor=i=>pool+i*0xd8,arg=i=>m.u32(m.reg('ESP')+4+i*4);
    const drawFns=[0,0,0,0,0,0,0x40dc70,0,0x40dc70,0,0x4414a0,0,0,0x40dc70,0x40dc70,0x40dc70,0x441da0,0x443010,0x4437f0,0,0,0,0,0,0x4453a0,0x447480,0x444700,0x445a40,0x447ce0,0x448750,0x449a10,0x44a780,0x44b090,0x44baf0,0x444af0,0,0x44c090,0x44c790];
    const event=(...a)=>events.push([...a,...Array(8-a.length).fill(0)].map(n=>n>>>0));
    function owner(vm){for(let i=0;i<41;++i)if(m.u32(actor(i)+4)===vm)return i;throw Error('unknown effect VM '+vm.toString(16));}
    m.replace(0x403e00,'effect-animation-start',()=>{const p=arg(0),script=arg(1),i=owner(p),character=m.reg('ECX')===0x102;event(0,i,side,character?1:0,script);m.view(p,0x2a4).fill(0);m.u32(p+0x1f8,1);m.u32(p+0x1f0,0xffffffff);m.write(p+0x21a,[script&255,script>>>8]);m.u32(p+0x224,sprite);m.u32(p+0x100,24);m.f32(p+24,8);m.f32(p+28,8);return 0;},2);
    m.replace(0x436f30,'effect-animation-advance',()=>{event(1,owner(arg(0)));return frame>185?1:0;},1);m.replace(0x43ab50,'effect-animation-draw',()=>{event(2,owner(arg(0)));return 0;},1);
    m.replace(0x43e2f0,'effect-audio',()=>{event(3,arg(0),arg(1));return 0;},2);
    m.replace(0x4130f0,'transfer-bullet',()=>{event(4,bulletManagers.indexOf(m.reg('ECX')));bullets.push(m.bytes(arg(0),0x210));return 0;},1);
    m.replace(0x40f1d0,'transfer-spirit',()=>{event(5,enemyManagers.indexOf(m.reg('ECX')),(arg(0)<<16)>>16,...m.readWords(arg(1),3));m.write(enemy+0x2d74,m.bytes(arg(1),12));m.f32(enemy+0x2dd4,m.f32(arg(1))+1.25);m.f32(enemy+0x2dd8,m.f32(arg(1)+4)-2.5);m.f32(enemy+0x2ddc,m.f32(arg(1)+8));m.f32(enemy+0x2df8,.003);m.u32(enemy+0x3380,0x2020);m.f32(enemy+0x2de0,-.5);return enemy;},7);
    m.replace(0x41f4c0,'lyrica-effect-shot',()=>{event(6,players.indexOf(m.reg('ECX')),arg(0),m.reg('EDX'));return 0;},1);
    for(const [address,type,stride] of [[0x43b7b0,8,20],[0x43b4d0,9,28],[0x43b670,11,20]])m.replace(address,'effect-geometry',()=>{event(type,arg(2));draws.push(m.bytes(arg(1),arg(2)*stride));return 0;},3);
    const device=m.allocate(4),vt=m.allocate(0x140);m.u32(device,vt);m.u32(0x4b3108,device);for(const [slot,argc] of [[0xfc,4],[200,3],[0x130,2]])m.u32(vt+slot,m.registerImport({dll:'draw',name:'state',argc,handler:()=>0}));
    m.u32(vt+0x120,m.registerImport({dll:'draw',name:'lines',argc:5,handler:()=>{assert.equal(arg(1),3);const count=arg(2)+1;event(10,count);draws.push(m.bytes(arg(3),count*20));return 0;}}));
    for(const address of [0x4396a0,0x40e380,0x40e390,0x4015f0,0x415d00,0x40e370,0x415d10])m.replace(address,'render-state',()=>0);
    m.replace(0x401390,'effect-viewport',()=>{m.u32(0x4b3448,0x4b3178+arg(0)*0xf0);return 0;},1);
    for(let i=0;i<2;++i){m.view(players[i],0x31000).fill(0);m.view(enemyManagers[i],0x2b0000).fill(0);m.u32(0x4a7d94+i*0x38,players[i]);m.u32(0x4a7da0+i*0x38,enemyManagers[i]);m.u32(0x4a7d98+i*0x38,bulletManagers[i]);m.u32(players[i]+0xbc,0x102);}m.u32(0x4a7e38,scene);m.view(scene,0x12000).fill(0);m.u32(0x4b36d4,0);
    const originalHeap=m.heap;
    const enemyFields=Array.from({length:c.ecl_var_field_count()},(_,i)=>Array.from({length:4},(_,j)=>dv.getUint32(c.ecl_var_fields()+(i*4+j)*4,true))).filter(x=>x[0]===0&&[0x2d74,0x2dd4,0x2df8,0x3380,0x2de0].includes(x[1]));
    function eventsMatch(label){dv=new DataView(c.memory.buffer);assert.equal(c.effects_event_count(f),events.length,label+' events '+JSON.stringify(events));const ptr=c.effects_events(f);events.forEach((e,i)=>assert.deepEqual(Array.from({length:8},(_,j)=>dv.getUint32(ptr+(i*8+j)*4,true)),e,label+' event'+i));assert.equal(c.effects_bullet_count(f),bullets.length,label+' bullets');bullets.forEach((b,i)=>assert.deepEqual(memory(c,c.effects_bullets(f)+i*0x210,0x210),b,label+' emission'));}
    function compare(label,checkDrawList=true){
        for(let i=0;i<40;++i){const local=c.effects_actor(f,i),original=actor(i);for(const [off,at,size] of fields)assert.deepEqual(memory(c,local+at,size),m.bytes(original+off,size),label+' actor'+i+' field'+off.toString(16));const vm=c.effects_animation(f,i),native=m.u32(original+4);assert.equal(Boolean(vm),Boolean(native),label+' animation lifetime'+i);if(vm){assert.deepEqual(memory(c,vm,0x224),m.bytes(native,0x224),label+' animation'+i);assert.deepEqual(memory(c,vm+0x228,0x7c),m.bytes(native+0x228,0x7c),label+' animation tail'+i);}}
        assert.deepEqual(memory(c,parts[0],8),m.bytes(0x4ace0c,8),label+' RNG');for(let i=0;i<3;++i)assert.equal(c.effects_values(f,i),m.u32(manager+[0,8,0x2d0][i]),label+' manager'+i);
        for(const [,orig,off,size] of enemyFields)assert.deepEqual(memory(c,parts[7]+off,size),m.bytes(enemy+orig,size),label+' spawned spirit '+orig.toString(16));
        const burst=c.effects_burst(f,0);if(burst){const native=m.u32(actor(0)+0xcc);assert.deepEqual(memory(c,burst,4),m.bytes(native,4),label+' burst phase');assert.deepEqual(memory(c,burst+4,4),m.bytes(native+0x50,4),label+' burst radius');assert.deepEqual(memory(c,c.effects_burst_jitter(f,0),4*33*4),m.bytes(native+0xce8,4*33*4),label+' burst jitter');}
        if(checkDrawList)for(let layer=0;layer<3;++layer){let a=m.u32(manager+0x110+layer*0xd8),count=0;while(a){assert.equal(c.effects_draw_index(f,layer,count++),(a-pool)/0xd8,label+' draw order');a=m.u32(a+0xd4);}assert.equal(c.effects_draw_index(f,layer,count),-1);}
        eventsMatch(label);++checks;
    }
    function render(kind,label){if(!drawFns[kind]||m.bytes(actor(0)+0xc4,1)[0]===0)return;events=[];bullets=[];draws=[];m.call(drawFns[kind],{ecx:actor(0)});c.effects_render(f,0);compare(label);const bytes=Buffer.concat(draws);assert.equal(c.effects_vertex_count(f),bytes.length,label+' vertex bytes');assert.deepEqual(memory(c,c.effects_vertices(f),bytes.length),new Uint8Array(bytes),label+' CPU geometry');++drawChecks;}
    try{for(let kind=0;kind<38;++kind)for(let scenario=0;scenario<6;++scenario){
        const transfer=kind>=1&&kind<=4;side=transfer?2:scenario%2;f=c.effects_fixture(side,32);parts=Array.from({length:8},(_,i)=>c.effects_part(f,i));dv=new DataView(c.memory.buffer);m.heap=originalHeap;m.view(manager,0x2dc).fill(0);m.view(pool,41*0xd8).fill(0);m.view(enemy,0x3400).fill(0);m.u32(manager+12,side);m.u32(manager+16,0x4a7d90+Math.min(side,1)*0x38);m.u32(manager+0x30,pool);m.u32(manager+0x34,32);m.u32(manager+0x38,8);m.u32(manager+0x2d4,0x101);m.u32(0x4a7ec4,0);m.u32(0x4a7dc4,0);
        for(let s=0;s<3;++s){const g=parts[1]+20*s;dv.setUint32(g,s<2?32+s*304:0,true);dv.setUint32(g+4,s<2?16:0,true);dv.setFloat32(g+8,9.375,true);dv.setFloat32(g+12,3.75,true);dv.setFloat32(g+16,288,true);m.u32(0x4b3178+s*0xf0+0xcc,s<2?32+s*304:0);m.u32(0x4b3178+s*0xf0+0xd0,s<2?16:0);}
        m.f32(0x4a80e0,9.375);m.f32(0x4a80e4,3.75);m.f32(0x4a80e8,288);const coordinates=transfer?(kind%2?1:0):side;m.call(0x401440,{ecx:0x4b3100,args:[coordinates]});
        for(let s=0;s<2;++s){const p=new Uint8Array(new Float32Array([s?70:-115,350-s*40,.25]).buffer);memory(c,parts[2]+s*12,12).set(p);m.write(players[s]+0x1b88,p);dv.setFloat32(parts[3]+s*4,-1.5+s,true);m.f32(players[s]+0x1cd8,-1.5+s);dv.setFloat32(parts[4]+s*4,s?2:-2,true);m.f32(players[s]+0x1ccc,s?2:-2);dv.setInt32(parts[5]+s*4,[7,10,0][(s+scenario)%3],true);m.i32(0x4a7db0+s*0x38,[7,10,0][(s+scenario)%3]);dv.setInt32(parts[6]+s*4,scenario===5?10:0,true);m.i32(enemyManagers[s]+0x2ac3b8,scenario===5?10:0);}
        const pp=new Uint8Array(new Float32Array([21.75,180.25,.375]).buffer),xx=new Uint8Array(new Float32Array([.45,83.25,11.5]).buffer);memory(c,pos,12).set(pp);memory(c,args,12).set(xx);m.write(position,pp);m.write(extra,xx);for(let j=0;j<4;++j)m.f32(sprite+0x20+j*4,[.125,.25,.625,.875][j]);
        dv.setUint32(parts[0],12345+kind*431+scenario*17,true);dv.setUint32(parts[0]+4,0,true);m.write(0x4ace0c,memory(c,parts[0],8));events=[];bullets=[];m.call(0x40ccc0,{ecx:manager,args:[kind,position,extra,1,0xffffffff]});assert.equal(c.effects_create(f,kind,pos,args,-1,coordinates),0);++creations;compare(`init ${kind}/${scenario}`,false);
        if(transfer){const base=c.effects_actor(f,0),map=fields[0][1],params=new Uint8Array(new ArrayBuffer(12)),pd=new DataView(params.buffer);pd.setFloat32(0,scenario*.7+.8,true);pd.setInt16(4,scenario%4===0?3:0,true);pd.setInt16(6,4,true);pd.setInt16(8,kind%2?0:1,true);pd.setInt16(10,scenario%4,true);memory(c,base+map+0xa0-12,12).set(params);m.write(actor(0)+0xa0,params);if(kind<3){dv.setFloat32(base+map+0x98-12,scenario*3,true);m.f32(actor(0)+0x98,scenario*3);}}
        const rate=[1,.5,.99][scenario%3];
        for(frame=0;frame<205;++frame){
            const vm=c.effects_animation(f,0),native=m.u32(actor(0)+4);if(vm&&(kind===6||kind===8||(kind>=13&&kind<=15))){dv=new DataView(c.memory.buffer);for(const [off,n] of [[0x100,16+8*(scenario%3)],[0x104,scenario%3<2?0:3]]){dv.setInt32(vm+off,n,true);m.i32(native+off,n);}for(const [off,value] of [[0x288,35+frame*.4],[0x28c,scenario%3?9:0],[0x18,3.5],[4,.25],[8,.75]]){dv.setFloat32(vm+off,value,true);m.f32(native+off,value);}}
            events=[];bullets=[];m.f32(0x4b36b8,rate);m.i32(0x4a7e44,scenario*4);m.u32(scene+0x11ea8,scenario===4?1:0);const paused=frame===61?0x800:0,frozen=frame===75?1:0;m.u32(0x4a7ec4,paused);m.u32(0x4a7dc4,frozen);m.call(0x40cdd0,{ecx:manager});c.effects_step(f,rate,paused,frozen,frame,scenario*4,scenario===4);compare(`${kind}/${scenario}/${frame}`);if(frame%17===0)render(kind,`draw ${kind}/${scenario}/${frame}`);
            if(transfer&&frame%17===0){events=[];bullets=[];event(7,2,0);m.call(0x40d070,{ecx:manager});c.effects_draw_layer(f,0);compare(`transfer draw ${kind}/${scenario}/${frame}`);++drawChecks;}
        }
        if(kind===0&&scenario===0){
            // Exhaustion, reserved actors, partial multi-spawns and recycling must
            // retain cursor and draw order even when creation returns the sentinel.
            frame=0;
            for(let round=0;round<110;++round){
                const count=[1,2,33,0][round%4],type=[0,5,9,20,21,22,23][round%7],color=(0x40201008+round*0x01020103)>>>0;
                events=[];bullets=[];const original=m.call(0x40ccc0,{ecx:manager,args:[type,position,extra,count,color]});
                assert.equal(c.effects_create_many(f,type,pos,args,count,color),(original-pool)/0xd8);++creations;compare('pool create '+round);
                if(round%9===0){events=[];bullets=[];const slot=round%8,original=m.call(0x40cd70,{ecx:manager,args:[9,position,slot,0xffffffff]});assert.equal(c.effects_create(f,9,pos,args,slot,0),(original-pool)/0xd8);++creations;compare('reserved '+round);}
                if(round%3===0){
                    for(let i=0;i<40;++i){const at=c.effects_actor(f,i)+fields[1][1];if(i%7===round%7){memory(c,at,1)[0]=0;m.write(actor(i)+0xc4,[0]);}for(const [off,value] of [[4,i%2],[6,i%5===0?1:0]]){memory(c,at+off,1)[0]=value;m.write(actor(i)+0xc4+off,[value]);}memory(c,c.effects_actor(f,i)+fields[2][1],1)[0]=i%11===0?1:0;m.write(actor(i)+0xd0,[i%11===0?1:0]);}
                }
                events=[];bullets=[];m.call(0x40cdd0,{ecx:manager});c.effects_step(f,1,0,0,0,0,0);compare('pool update '+round);
                for(let layer=0;layer<2;++layer){events=[];bullets=[];event(7,0,layer);m.call([0x40cfb0,0x40d110][layer],{ecx:manager});c.effects_draw_layer(f,layer);compare('pool draw '+round+'/'+layer);++drawChecks;}
            }
        }
        c.effects_delete(f);f=0;
    }report('effects',{checks,creations,drawChecks,kinds:38,scope:'Original effect pool lifecycle, delayed Hermite transfers, bullet/spirit arrival and character effect state, plus generated CPU draw geometry. ANM advancement and final GPU submission are explicit boundaries.'});
    }finally{if(f)c.effects_delete(f);c.release(pos);c.release(args);m.close();}
});
