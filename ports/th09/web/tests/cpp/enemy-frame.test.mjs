import test from 'node:test';
import assert from 'node:assert/strict';
import {oracle,core,memory,bits,report} from './helpers.mjs';
const ins=(op,args=[],time=0,mask=0)=>{const b=Buffer.alloc(12+4*args.length);b.writeInt32LE(time);b.writeInt16LE(op,4);b.writeInt16LE(b.length,6);b[9]=255;b.writeUInt16LE(mask,10);args.forEach((v,i)=>b.writeUInt32LE(v>>>0,12+i*4));return b;};
function program(){
    const subs=[[ins(0)],[ins(131,[50]),ins(6,[10000,73],0,1)],[ins(0),ins(1,[],4)]].map(s=>Buffer.concat([...s,ins(-1,[],0x7fffffff)]));
    const tl=(time,op,args=[])=>{const b=Buffer.alloc(8+args.length*4);b.writeInt32LE(time);b.writeUInt16LE(op,4);b[6]=b.length;b[7]=255;args.forEach((v,i)=>b.writeUInt32LE(v>>>0,8+i*4));return b;};
    const timeline=Buffer.concat([tl(0,0,[2,bits(50),bits(-10),10,-1,100]),tl(2,17,[2,bits(90),bits(-10),10,-1,100]),tl(6,6),tl(-1,0)]);
    const h=Buffer.alloc(24);h.writeUInt32LE(0x900);h.writeUInt16LE(subs.length,4);h.writeUInt16LE(1,6);h.writeUInt32LE(h.length,8);let off=h.length+timeline.length;subs.forEach((s,i)=>{h.writeUInt32LE(off,12+i*4);off+=s.length;});return Buffer.concat([h,timeline,...subs]);
}
test('Enemy frame controller matches original scripts, damage, capture, removal, targeting and draw order',async()=>{
    const m=await oracle(),c=await core(),fixture=c.enemy_frame_create(),f=c.enemy_frame_base(fixture);
    const manager=m.allocate(0x2ac450),owner=m.allocate(64),other=m.allocate(64),player=m.allocate(0x30500),otherPlayer=m.allocate(0x30500),scene=m.allocate(0x13000),sprite=m.allocate(0x44),capture=m.allocate(0xd8),position=m.allocate(12),buf=c.allocate(4096),req=c.allocate(36);
    const dv=()=>new DataView(c.memory.buffer),readMap=(p,n,w)=>Array.from({length:n},(_,i)=>Array.from({length:w},(_,j)=>dv().getUint32(p+(i*w+j)*4,true)));
    const fields=readMap(c.ecl_var_fields(),c.ecl_var_field_count(),4),extras=[...readMap(c.ecl_vm_extras(),c.ecl_vm_extra_count(),3),...readMap(c.enemy_frame_extras(),c.enemy_frame_extra_count(),3)],managerFields=readMap(c.enemy_frame_fields(),c.enemy_frame_field_count(),4);
    const data=program();let resource,events=[],shots=[],shotIndex=0,emissions=[],checks=0,seed=0x346105f;
    const next=()=>seed=(Math.imul(seed,1664525)+1013904223)>>>0;
    const enemy=n=>manager+0x5758+n*0x5430,which=p=>(p-enemy(0))/0x5430,arg=n=>m.u32(m.reg('ESP')+4+n*4);
    const event=(...values)=>events.push([...values,...Array(12-values.length).fill(0)].map(v=>v>>>0));
    const vec=p=>m.readWords(p,3),signed16=p=>(m.u32(p)<<16)>>16;
    function anmIdentity(ptr){const delta=ptr-enemy(0)-8,index=Math.floor(delta/0x5430);return [index,(delta-index*0x5430)/0x2a4];}
    m.replace(0x42c970,'frame-ecl-resource',()=>{resource=m.allocate(data.length);m.write(resource,data);return resource;},1);
    m.replace(0x416080,'attack-position',()=>{event(0,arg(0),...vec(arg(1)));return 0;},2);
    m.replace(0x436f30,'animation-step',()=>{const a=arg(0),[index,layer]=anmIdentity(a);event(1,index,layer,signed16(a+0x21a),m.u32(a+0x1f0));m.u32(a+0x1f0,m.u32(a+0x1f0)^0x010203);return Number(signed16(a+0x21a)===7);},1);
    m.replace(0x403e00,'animation-start',()=>{const a=arg(0),[index,layer]=anmIdentity(a);event(14,index,layer,0,arg(1));m.write(a+0x21a,[arg(1)&255,(arg(1)>>>8)&255]);return 0;},2);
    m.replace(0x40f8e0,'player-body-collision',()=>{event(2,...vec(arg(0)),...vec(arg(1)));return 0;},3);
    m.replace(0x41fcd0,'player-shot-damage',()=>{event(3,...vec(arg(0)),...vec(arg(1)),m.u32(arg(3)));const s=shots[shotIndex++%2];m.i32(arg(2),s[1]);m.i32(arg(3),m.i32(arg(3))+1);m.i32(arg(4),s[2]);return s[0];},5);
    m.replace(0x40f860,'score',()=>{event(4,arg(0));return 0;},1);
    m.replace(0x40cc00,'effect',()=>{event(5,arg(0),...vec(arg(1)));return 0;},4);
    m.replace(0x43e380,'sound',()=>{event(6,arg(0),arg(1));return 0;},2);
    m.replace(0x43e2f0,'pan-sound',()=>{event(7,arg(0),arg(1));return 0;},2);
    m.replace(0x4067f0,'boss-position',()=>{event(8,arg(0),...vec(arg(1)));return 0;},2);
    m.replace(0x40f890,'boss-state',()=>{event(9,arg(0),arg(1));return 0;},2);
    m.replace(0x40f560,'release-attachments',()=>{const e=m.reg('ECX');event(10,which(e),m.u32(e+0x5414));m.u32(e+0x5414,0);return 0;},0);
    m.replace(0x40f4c0,'update-attachments',()=>{const e=m.reg('EDI');event(11,which(e),m.u32(e+0x5414));return 0;},0);
    m.replace(0x4102b0,'death-rewards',()=>{event(12,which(m.reg('ECX')),arg(0));return 0;},1);
    m.replace(0x4130f0,'bullet-emission',()=>{event(15);emissions.push(m.bytes(arg(0),0x210));return 0;},1);
    m.call(0x40fa70,{ecx:manager});m.u32(manager+0x320,owner);m.u32(manager+0x324,other);m.u32(owner+4,player);m.u32(other+4,otherPlayer);m.u32(0x4a7e38,scene);m.u32(player+0x368,capture);
    m.f32(sprite+0x34,32);m.f32(sprite+0x30,24);
    function field(n,off){const i=fields.findIndex(v=>v[0]===0&&v[1]===off);if(i>=0)return c.enemy_manager_field(f,n,i);const e=extras.find(v=>v[0]===off);assert.ok(e,'field '+off.toString(16));return c.enemy_manager_actor(f,n)+e[1];}
    function setBytes(n,off,b){memory(c,field(n,off),b.length).set(b);m.write(enemy(n)+off,b);}
    function u32(n,off,v){const b=Buffer.alloc(4);b.writeUInt32LE(v>>>0);setBytes(n,off,b);}
    function f32(n,off,v){u32(n,off,bits(v));}
    function i16(n,off,v){const b=Buffer.alloc(2);b.writeInt16LE(v);setBytes(n,off,b);}
    function u8(n,off,v){setBytes(n,off,Buffer.from([v&255]));}
    function vector(n,off,v){const b=Buffer.alloc(12);v.forEach((v,i)=>b.writeFloatLE(v,i*4));setBytes(n,off,b);}
    function setManager(base,off,value){const i=managerFields.findIndex(v=>v[0]===base&&v[1]===off);assert.ok(i>=0);dv().setInt32(c.enemy_frame_field(fixture,i),value,true);m.i32((base?player:manager)+off,value);}
    function stateCopy(){
        for(let i=0;i<fields.length;++i){const [base,off,,size]=fields[i];if(base<2)continue;const bases=[0,0,manager,player,otherPlayer,0,owner];m.write(bases[base]+off,memory(c,c.enemy_manager_field(f,999,i),size));}
    }
    function spawn(n,sub){
        const b=Buffer.alloc(36);[sub,bits(20+n*32),bits(150+n*43),0,100,-1,150,0,0].forEach((v,i)=>b.writeUInt32LE(v>>>0,i*4));memory(c,req,36).set(b);m.write(position,b.subarray(4,16));
        assert.equal(c.enemy_manager_spawn(f,req,0,1),n);assert.equal(m.call(0x40f1d0,{ecx:manager,args:[sub,position,100,-1,150,0,0]}),enemy(n));
        c.enemy_frame_sprite(fixture,n,1);m.u32(enemy(n)+0x22c,sprite);
    }
    function compare(label,count){
        for(let n=0;n<count;++n){const e=enemy(n),bases=[e,e+0x7f4,manager,player,otherPlayer,0,owner];
            for(let i=0;i<fields.length;++i){const [base,off,,size]=fields[i];assert.deepEqual(memory(c,c.enemy_manager_field(f,n,i),size),m.bytes(bases[base]+off,size),`${label} actor${n} field${base}/${off.toString(16)}`);}
            for(const [off,local,size] of extras)assert.deepEqual(memory(c,c.enemy_manager_actor(f,n)+local,size),m.bytes(e+off,size),`${label} actor${n} extra${off.toString(16)}`);
            assert.deepEqual(memory(c,c.enemy_manager_time(f,n),12),m.bytes(e+0x7fc,12),label+' ECL timer');
            assert.equal(c.enemy_manager_script(f,n,2),m.u32(e+0x7f8)-resource,label+' ECL cursor');
        }
        for(let i=0;i<managerFields.length;++i){const [base,off,,size]=managerFields[i];assert.deepEqual(memory(c,c.enemy_frame_field(fixture,i),size),m.bytes((base?player:manager)+off,size),`${label} manager${base}/${off.toString(16)}`);}
        for(const [whichTarget,addr] of [[0,player+0x3037c],[1,manager+0x2ac444],[2,manager+0x2ac448]])assert.equal(c.enemy_frame_target(fixture,whichTarget),m.u32(addr)?which(m.u32(addr)):-1,label+' target'+whichTarget);
        for(let layer=0;layer<4;++layer){let ptr=m.u32(manager+0x2ac410+layer*4),index=0;while(ptr){assert.ok(index<128);assert.equal(c.enemy_frame_draw(fixture,layer,index++),which(ptr),label+' draw');ptr=m.u32(ptr+4);}assert.equal(c.enemy_frame_draw(fixture,layer,index),-1);}
        const actual=readMap(c.enemy_frame_events(fixture),c.enemy_frame_event_count(fixture),12);assert.equal(actual.length,events.length,label+' call count');actual.forEach((value,i)=>assert.deepEqual(value,events[i],label+' ordered call '+i));
        assert.equal(c.enemy_frame_emission_count(fixture),emissions.length,label+' emitted bullets');for(let n=0;n<emissions.length;++n)assert.deepEqual(memory(c,c.enemy_frame_emissions(fixture)+n*0x210,0x210),emissions[n],label+' bullet parameters');++checks;
    }
    try{
        memory(c,buf,data.length).set(data);assert.equal(c.enemy_manager_load(f,buf,data.length,0),1);m.call(0x405660,{ecx:manager,args:[0]});stateCopy();
        const settings=c.enemy_frame_settings(fixture);dv().setInt32(settings+4,1,true);m.i32(scene+0x1095c,1);
        for(let sample=0;sample<384;++sample){
            for(let n=0;n<128;++n){c.enemy_manager_active(f,n,0);m.u32(enemy(n)+0x337c,m.u32(enemy(n)+0x337c)&~1);}
            c.enemy_frame_set_target(fixture,-1);m.u32(player+0x3037c,0);
            const character=sample%16,side=sample%2,rate=[1,.5,.99][sample%3],flags=sample%19===0?32:0;
            const charField=fields.findIndex(v=>v[0]===6&&v[1]===0x20),positionField=fields.findIndex(v=>v[0]===3&&v[1]===0x1b88);
            dv().setInt32(c.enemy_manager_field(f,0,charField),character,true);memory(c,c.enemy_manager_field(f,0,positionField),12).set(new Uint8Array(new Float32Array([10,210,0]).buffer));stateCopy();m.u32(manager+0x31c,side);
            // Side is not an ECL variable, and is configured through the fixture.
            c.enemy_frame_side(fixture,side);m.u32(player+0x30404,m.u32(0x4a1a50+character*4));m.u32(player+0x30408,m.u32(0x4a1a90+character*4));
            setManager(1,0,sample%13===0?1:0);setManager(1,0x364,0);setManager(0,0x2ac44c,sample%5===0?2:180);
            const nearestIndex=managerFields.findIndex(v=>v[0]===1&&v[1]===0x30364),nearest=c.enemy_frame_field(fixture,nearestIndex);memory(c,nearest,12).set(new Uint8Array(new Float32Array([-999,-999,0]).buffer));m.write(player+0x30364,memory(c,nearest,12));
            const radius=new Float32Array([120,-Math.PI/2,1.75]);memory(c,c.enemy_frame_capture(fixture),12).set(new Uint8Array(radius.buffer));m.write(capture+0x7c,new Uint8Array(radius.buffer));
            m.f32(0x4b36b8,1);m.u32(0x4b36d4,0);m.u32(0x4a7eb0,1);m.f32(0x4a7e7c,1);
            for(let n=0;n<3;++n){spawn(n,sample%17===0?2:0);const behavior=[0,0x10,0x20,0x100,0x20000,0x40000,0x60000,0x8000000,0x4000000,0x10000,0x2000000][(sample+n)%11];
                u32(n,0x337c,0x4d|behavior|(n===1?2:0)|(sample%7===0?0x8000:0));
                const secondary=[0,0x40,0x80,0xc0,0x1c0,0x1040,0x1000,0x200,0x400,0x800,0xc00,0x2000,0x4000,0x10,1,8][(sample+n)%16];u32(n,0x3380,secondary);
                u8(n,0x336b,n);u8(n,0x3387,(sample+n)%4);u32(n,0x2e48,sample%9===0?0:30+next()%70);u32(n,0x2e70,0x9fabcdef);u32(n,0x5414,sample%3);u32(n,0x33d0,720);i16(n,0x2d2e,sample%4===0?1:-1);
                vector(n,0x2d8c,[(sample%2?1:-1)*1.25,1.75,0]);if(sample%23===0)vector(n,0x2d74,[250,150,0]);
                i16(n,0x4c6,3);i16(n,0x76a,7);if(sample%7===0)vector(n,0x2dc8,[50,20,0]);
                const box=Buffer.alloc(16);[-100,60,100,400].forEach((v,i)=>box.writeFloatLE(v,i*4));setBytes(n,0x3398,box);
                const invul=c.allocate(12);c.timer_reset(invul,sample%3);setBytes(n,0x53a8,memory(c,invul,12));c.release(invul);
                if(sample%4===1){u8(n,0x53a0,3);i16(n,0x53a2,20);i16(n,0x53a4,20);}
                c.enemy_manager_set_boss(f,n,n);m.u32(manager+0x2ac388+n*4,enemy(n));
            }
            shots=[[sample%6===0?0:18,sample%8,4],[7,sample%5,3]];const shotBytes=Buffer.alloc(24);shots.flat().forEach((v,i)=>shotBytes.writeInt32LE(v,i*4));memory(c,c.enemy_frame_shots(fixture),24).set(shotBytes);
            setManager(1,0x364,sample%2);c.enemy_frame_field_flags(fixture,sample%11===0?1:0);m.u32(owner+0x34,sample%11===0?1:0);
            for(let frame=0;frame<8;++frame){
                events=[];emissions=[];shotIndex=0;const paused=sample%31===0&&frame<3?0x1800:0;dv().setUint32(settings,paused,true);m.u32(0x4a7ec4,paused);m.f32(0x4b36b8,rate);m.u32(0x4b36d4,flags);
                assert.equal(m.call(0x410730,{ecx:manager,limit:5000000}),1);assert.equal(c.enemy_frame_step(fixture,rate,flags),1,`step success ${sample}/${frame}`);compare(`sample${sample}/frame${frame}`,3);
            }
        }
        for(let n=0;n<128;++n){c.enemy_manager_active(f,n,0);m.u32(enemy(n)+0x337c,m.u32(enemy(n)+0x337c)&~1);}
        m.f32(0x4b36b8,1);m.u32(0x4b36d4,0);for(let n=0;n<8;++n)spawn(n,0);
        for(let n=0;n<8;++n){c.enemy_manager_active(f,n,0);m.u32(enemy(n)+0x337c,m.u32(enemy(n)+0x337c)&~1);}
        c.enemy_frame_field_flags(fixture,0);m.u32(owner+0x34,0);setManager(1,0,0);setManager(1,0x364,0);setManager(0,0x2ac3b0,0);setManager(0,0x2ac44c,180);
        dv().setInt32(settings+4,0,true);m.i32(scene+0x1095c,0);m.u32(manager+0x2ac3fc,manager);
        for(let i=0;i<256;++i){memory(c,settings+12+i,1)[0]=i%2?128:0;m.write(0x4a7c88+i,[i%2?128:0]);}
        for(const rate of [1,.5,.99])for(let frame=0;frame<160;++frame){
            events=[];emissions=[];shotIndex=0;dv().setUint32(settings,0,true);m.u32(0x4a7ec4,0);m.f32(0x4b36b8,rate);m.u32(0x4b36d4,0);
            const dialogue=frame%37<3?0:frame%23===0?-2:-1;dv().setInt32(settings+8,dialogue,true);m.i32(scene+0xe94c,dialogue);setManager(1,0x364,frame%5===0?1:0);
            assert.equal(m.call(0x410730,{ecx:manager,limit:5000000}),1);assert.equal(c.enemy_frame_step(fixture,rate,0),1);const label=`timeline ${rate}/${frame}`;compare(label,8);
            for(let part=0;part<3;++part)assert.deepEqual(memory(c,c.enemy_frame_timeline_part(fixture,part),part<2?12:4),m.bytes(manager+0x2ac3e0+part*12,part<2?12:4),label+' timeline state'+part);
            assert.equal(c.enemy_frame_timeline_cursor(fixture),m.u32(manager+0x2ac400)?m.u32(manager+0x2ac400)-resource:-1,label+' timeline cursor');
        }
        report('enemy-frame',{checks,characters:16,integratedTimelineFrames:480,scope:'Original complete enemy frame callback with ECL and timeline execution. Animation execution, player collision/damage and scene effects observed at explicit service boundaries; full match and rendering not covered.'});
    }finally{c.enemy_frame_delete(fixture);c.release(buf);c.release(req);m.close();}
});
