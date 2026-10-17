import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {oracle,core,memory,bits,report,root} from './helpers.mjs';
const ins=(op,args=[],time=0,mask=255)=>{const b=Buffer.alloc(8+4*args.length);b.writeInt32LE(time);b.writeUInt16LE(op,4);b[6]=b.length;b[7]=mask;args.forEach((a,i)=>b.writeUInt32LE(a>>>0,8+i*4));return b;};
function resource(commands){const head=Buffer.alloc(12);head.writeUInt32LE(0x900);head.writeUInt16LE(1,6);head.writeUInt32LE(12,8);return Buffer.concat([head,...commands,ins(0,[],-1)]);}
test('Enemy timelines: every original command, waits, difficulty, mirrors and all eight real schedules',async()=>{
    const m=await oracle(),c=await core(),state=m.allocate(64),manager=m.allocate(0x2ac450),scene=m.allocate(0x11000),created=m.allocate(0x5430),bosses=m.allocate(4*0x5430),raw=m.allocate(65536),buffer=c.allocate(65536);
    const events=[];let f=0,comparisons=0,spawnCount=0;
    m.u32(0x4a7e38,scene);m.u32(state+0x1c,manager);
    m.replace(0x40f1d0,'spawn-enemy-boundary',()=>{
        const args=m.readWords(m.reg('ESP')+4,7),b=Buffer.alloc(36);b.writeInt32LE((args[0]<<16)>>16);b.set(m.bytes(args[1],12),4);
        for(let i=2;i<7;++i)b.writeUInt32LE(i===3?((args[i]<<24)>>24)>>>0:args[i],8+i*4);events.push(b);return created;
    },7);
    function setup(data,index=0,mirror=0){
        if(f)c.timeline_delete(f);f=c.timeline_create();memory(c,buffer,data.length).set(data);assert.equal(c.timeline_load(f,buffer,data.length,index,mirror),1);
        m.write(raw,data);m.u32(state+0x20,raw+data.readUInt32LE(8+index*4));m.i32(state+0x18,mirror);
        m.write(state,memory(c,c.timeline_part(f,0),12));m.write(state+12,memory(c,c.timeline_part(f,1),12));
        m.view(created,0x5430).fill(0);m.view(bosses,4*0x5430).fill(0);m.view(0x4ace0c,8).fill(0);m.u32(0x4ace0c,0x618a);memory(c,c.timeline_part(f,2),8).set(m.bytes(0x4ace0c,8));
        for(let n=0;n<4;++n){m.i32(manager+0x2ac430+n*4,-1);m.u32(manager+0x2ac388+n*4,bosses+n*0x5430);m.write(bosses+n*0x5430+0x2d70,[255,255]);}
        m.write(0x4a7ecd,[0]);
    }
    function event(n,v){new DataView(c.memory.buffer).setInt32(c.timeline_part(f,3)+4*n,v,true);m.i32(manager+0x2ac430+n*4,v);}
    function boss(n,active,exists=1){c.timeline_set_boss(f,n,active,exists);m.u32(manager+0x2ac388+n*4,exists?bosses+n*0x5430:0);m.u32(bosses+n*0x5430+0x337c,active);}
    function timer(part,current,time=current){c.timer_reset(c.timeline_part(f,part),current);new DataView(c.memory.buffer).setFloat32(c.timeline_part(f,part)+4,time,true);m.write(state+12*part,memory(c,c.timeline_part(f,part),12));}
    function step(label,{rate=1,amount=1,flags=0,mask=1,dialogue=-1}={}){
        events.length=0;m.f32(0x4b36b8,rate);m.u32(0x4b36d4,flags);m.f32(0x4a7e7c,amount);m.u32(0x4a7eb0,mask);m.i32(scene+0xe94c,dialogue);
        const expected=m.call(0x40fcc0,{ecx:state}),actual=c.timeline_step(f,rate,amount,flags,mask,dialogue);assert.equal(actual,expected,label+' return');
        for(const [part,a,n] of [[0,state,12],[1,state+12,12],[2,0x4ace0c,8],[3,manager+0x2ac430,16]])assert.deepEqual(memory(c,c.timeline_part(f,part),n),m.bytes(a,n),label+' state '+part);
        const meta=[m.u32(state+0x20)?m.u32(state+0x20)-raw:-1,m.i32(state+0x18),m.bytes(0x4a7ecd,1)[0],m.i32(created+0x3360),m.i32(created+0x3364),m.i32(created+0x3380)];
        meta.forEach((v,i)=>assert.equal(c.timeline_meta(f,i),v,label+' meta '+i));
        for(let i=0;i<4;++i)assert.equal(c.timeline_boss(f,i),new DataView(m.bytes(bosses+i*0x5430+0x2d70,2).buffer).getInt16(0,true),label+' boss '+i);
        assert.equal(c.timeline_spawn_count(f),events.length,label+' spawns');assert.deepEqual(Buffer.from(memory(c,c.timeline_spawns(f),events.length*36)),Buffer.concat(events),label+' spawn parameters');
        ++comparisons;spawnCount+=events.length;return expected;
    }
    try{
        for(const rate of [.5,.99,1])for(const flags of [0,32])for(const op of [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,65535]){
            const args=op===2||op===4?[0x12345,bits(-55.7),bits(104.3),bits(21.8),77,0xf8,321]:op===3||op===5?[2,bits(-7.8),123,2,500]:op===8?[2,0xfffd]:op===10?[1]:op===13||op===14?[55]:op===11||op===12?[3,bits(-128.1),bits(3.3),35,7,2,800]:[3,bits(95.3),bits(-24.9),40,0xff,100];
            setup(resource([ins(op,args),ins(0,[1,bits(5),bits(7),23,0,32],8),ins(16,[],20)]));
            if(op===10)boss(1,1);event(0,-8);event(1,21);if(op===13)event(2,55);
            timer(1,2,2.5);
            for(let frame=0;frame<50;++frame){if(frame===12)boss(1,0);if(frame===18)event(3,55);if(step(`synthetic op${op} rate${rate} flags${flags} f${frame}`,{rate,flags,dialogue:frame===0?-2:frame===3?0:-1}))break;}
        }
        for(const mask of [1,2,4,8,16,128,255])for(const initial of [0,1,8]){
            setup(resource([ins(3,[7,bits(21),32,-1,90],0,2),ins(2,[2,bits(-99),bits(99),bits(-20),34,2,100],3,5),ins(15,[8,bits(40),bits(-5),15,0,40],7,128),ins(16,[],11,255)]));timer(0,initial);
            for(let frame=0;frame<20;++frame)if(step(`filter ${mask}/${initial}/${frame}`,{mask}))break;
        }
        const data=readFileSync(resolve(root,'reference/assets/enemy.ecl'));
        for(let index=0;index<8;++index)for(const rate of [.5,.99,1])for(const mirror of [0,1]){
            setup(data,index,mirror);let finished=false;
            for(let frame=0;frame<400;++frame){if(step(`real ${index} rate${rate} mirror${mirror} f${frame}`,{rate})){finished=true;break;}}
            assert.ok(finished,'real timeline terminates');
        }
        report('enemy-timeline',{comparisons,spawns:spawnCount,realTimelines:8,scope:'Original timeline controller with enemy creation captured at the manager boundary; not full match simulation'});
    }finally{if(f)c.timeline_delete(f);c.release(buffer);m.close();}
});
