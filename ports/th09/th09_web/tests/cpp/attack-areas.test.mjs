import test from 'node:test';
import assert from 'node:assert/strict';
import {oracle,core,memory,report,bits} from './helpers.mjs';
test('Attack areas match original allocation, delayed growth, cancellation, reflection and enemy damage',async()=>{
    const m=await oracle(),c=await core(),f=c.attack_areas_fixture(),player=m.allocate(0x31000),nativePos=m.allocate(12),nativeExtent=m.allocate(12),nativeResult=m.allocate(12),pos=c.allocate(12),extent=c.allocate(12),result=c.allocate(4);
    const transfer=m.allocate(0x100),bullet=m.allocate(0x10c4),bulletManager=m.allocate(0x25e200),field=m.allocate(0x38);
    const dv=new DataView(c.memory.buffer),base=c.attack_areas_part(f,0),area=n=>player+0x28bc+n*68,idx=p=>p?(p-area(0))/68:-1;
    const arg=i=>m.u32(m.reg('ESP')+4+i*4),vec=p=>m.readWords(p,3);let events=[],checks=0,creates=0,queries=0,seed=14791;
    const rand=()=>{seed=(Math.imul(seed,1664525)+1013904223)>>>0;return seed;};
    const event=(...a)=>events.push([...a,...Array(10-a.length).fill(0)].map(n=>n>>>0));
    m.replace(0x40ccc0,'area-transfer',()=>{event(0,arg(0),...vec(arg(1)),...vec(arg(2)));return transfer;},5);
    m.replace(0x41bc90,'area-charge',()=>{event(1,arg(0));return 0;},1);
    m.replace(0x43e2f0,'area-sound',()=>{event(2,arg(0),arg(1));return 0;},2);
    m.replace(0x41d150,'area-combo',()=>{event(3,...vec(arg(0)),arg(1),arg(2),arg(3),arg(4));return 0;},5);
    m.replace(0x40f860,'area-score',()=>{event(4,arg(0));return 0;},1);
    m.u32(player+12,field);m.u32(bullet,bulletManager);m.u32(player+0x303c8,0);m.u32(player+0x303d0,1);
    for(let n=0;n<513;++n)m.write(area(n),memory(c,base+n*68,68));for(let n=0;n<512;++n)m.u32(player+0xb908+n*4,area(n));m.u32(player+0xc114,512);
    const bulletFields=Array.from({length:c.bullet_field_count()},(_,i)=>Array.from({length:3},(_,j)=>dv.getUint32(c.bullet_fields()+(i*3+j)*4,true)));
    function compare(label,lists=true){
        assert.deepEqual(memory(c,base,513*68),m.bytes(area(0),513*68),label+' areas');
        for(let which=0;which<2;++which){assert.equal(c.attack_areas_count(f,which),m.i32(player+0xc110+which*4),label+' count');if(lists)for(let n=0;n<(which?512:514);++n)assert.equal(c.attack_areas_list(f,which,n),idx(m.u32(player+(which?0xb908:0xb100)+n*4)),label+' list '+which+'/'+n);}
        ++checks;
    }
    function create(lifetime,delay){const p=new Uint8Array(new Float32Array([rand()%180-90,rand()%420,0]).buffer);memory(c,pos,12).set(p);m.write(nativePos,p);const actual=c.attack_areas_create(f,pos,lifetime,delay),expected=idx(m.call(0x41cf30,{ecx:player,args:[nativePos,lifetime,delay]}));assert.equal(actual,expected);++creates;return actual;}
    function edit(n,offset,value,float=false){if(float){dv.setFloat32(base+n*68+offset,value,true);m.f32(area(n)+offset,value);}else{dv.setInt32(base+n*68+offset,value,true);m.i32(area(n)+offset,value);}}
    function update(){m.reg('EDI',player);m.call(0x41c8e0);c.attack_areas_update(f);}
    function compareEvents(label){assert.equal(c.attack_areas_event_count(f),events.length,label);const p=c.attack_areas_events(f);events.forEach((e,i)=>assert.deepEqual(Array.from({length:10},(_,j)=>dv.getUint32(p+(i*10+j)*4,true)),e,label+' event'+i));assert.deepEqual(memory(c,c.attack_areas_part(f,6),12),m.bytes(transfer+0xa0,12),label+' transfer');assert.deepEqual(memory(c,c.attack_areas_part(f,1),8),m.bytes(0x4ace0c,8),label+' RNG');}
    try{
        compare('initial');
        for(let n=0;n<520;++n){const i=create(3+n%7,n%4);edit(i,8,n*.25,true);edit(i,12,.375,true);edit(i,16,10,true);edit(i,20,15,true);edit(i,24,-.25,true);edit(i,28,.5,true);}
        compare('exhausted pool');for(let frame=0;frame<16;++frame){update();compare('expiry '+frame);}
        for(let frame=0;frame<360;++frame){for(let j=0;j<frame%4;++j)create(1+rand()%19,rand()%7);update();compare('recycling '+frame);}
        for(let frame=0;frame<30;++frame)update();compare('empty');
        for(let n=0;n<9;++n)create(100000,0);
        for(let scenario=0;scenario<3600;++scenario){
            const side=scenario%2,mode=scenario%4,seed=rand();dv.setUint32(c.attack_areas_part(f,1),seed,true);dv.setUint32(c.attack_areas_part(f,1)+4,0,true);m.write(0x4ace0c,memory(c,c.attack_areas_part(f,1),8));
            const setting=c.attack_areas_part(f,3);[side,scenario%5,scenario%120,scenario%16].forEach((n,i)=>dv.setInt32(setting+i*4,n,true));m.i32(player+8,side);m.i32(0x4a7eac,scenario%5);m.i32(player+0x30448,scenario%120);m.i32(bulletManager+0x25e18c,scenario%16);
            for(let n=0;n<2;++n){const g=c.attack_areas_part(f,2)+n*20;dv.setUint32(g,n*304+32,true);dv.setUint32(g+4,16,true);dv.setFloat32(g+8,7.125,true);dv.setFloat32(g+12,3.5,true);dv.setFloat32(g+16,288,true);m.u32(0x4b3100+0x78+n*0xf0+0xcc,n*304+32);m.u32(0x4b3100+0x78+n*0xf0+0xd0,16);}
            m.f32(0x4a80e0,7.125);m.f32(0x4a80e4,3.5);m.f32(0x4a80e8,288);m.call(0x401440,{ecx:0x4b3100,args:[side]});
            for(let n=0;n<9;++n){const i=c.attack_areas_list(f,0,n);edit(i,0,rand()%65-32,true);edit(i,4,rand()%65-32,true);edit(i,8,n%3===0?10+rand()%50:0,true);edit(i,16,8+rand()%32,true);edit(i,20,8+rand()%32,true);edit(i,32,n%3===1?((scenario%9)-4)*.3926991:0,true);edit(i,36,scenario%20);edit(i,40,1+rand()%40);edit(i,44,rand()%25);edit(i,48,n%3?0:15);edit(i,52,1+n%3);edit(i,56,(n+scenario)%5);edit(i,64,n===0?scenario%3:0);}
            const p=new Uint8Array(new Float32Array([rand()%81-40,rand()%81-40,0]).buffer),e=new Uint8Array(new Float32Array([rand()%32+1,rand()%32+1,0]).buffer);memory(c,pos,12).set(p);memory(c,extent,12).set(e);m.write(nativePos,p);m.write(nativeExtent,e);
            const bp=Buffer.alloc(0x10c4);bp.writeInt16LE(scenario%3,0x10c0);bp.writeInt16LE(scenario%8,0x10c2);bp.writeFloatLE(1.75,0xd70);bp[0x10bd]=scenario%4;m.write(bullet+4,bp.subarray(4));for(const [offset,local,size] of bulletFields)memory(c,c.attack_areas_part(f,4)+local,size).set(bp.subarray(offset,offset+size));
            const bounds=new Uint8Array(new Float32Array([-6,-5,0,6,5,0]).buffer);memory(c,c.attack_areas_part(f,5),24).set(bounds);m.write(player+0x1c78,bounds);
            events=[];const expected=m.call(mode<2?0x41dff0:0x41e350,{ecx:player,args:[nativePos,nativeExtent,mode&1?bullet:0]});assert.equal(c.attack_areas_cancel(f,pos,extent,mode),expected,'cancellation '+scenario);compareEvents('cancellation '+scenario);
            m.i32(nativeResult,987);m.i32(nativeResult+4,333);m.i32(nativeResult+8,17);dv.setInt32(result,17,true);
            const nativeDamage=m.call(0x41fcd0,{ecx:player,args:[nativePos,nativeExtent,nativeResult,nativeResult+4,nativeResult+8]});assert.equal(c.attack_areas_damage(f,pos,extent,result),nativeDamage|0,'damage '+scenario);assert.equal(dv.getInt32(result,true),m.i32(nativeResult+8),'special damage');assert.equal(m.i32(nativeResult),0);assert.equal(m.i32(nativeResult+4),0);compare('collision '+scenario,false);++queries;
        }
        report('attack-areas',{checks,creates,queries,scope:'Original 512-slot area allocation and overflow slot, delayed growth/recycling, box/circle/rotated cancellation, reflection RNG and recorded rewards, area damage timing/limits. Physical player shots are absent from damage fixtures; transfer, charge, combo and score are recorded boundaries.'});
    }finally{c.attack_areas_delete(f);c.release(pos);c.release(extent);c.release(result);m.close();}
});
