import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {oracle,core,memory,root,report} from './helpers.mjs';
import {installAnm,normalizedAnm} from './anm-oracle.mjs';

test('TH09 lasers match original creation, phases, collision windows and real ANM states',async()=>{
    const m=await oracle(),c=await core(),bytes=readFileSync(resolve(root,'reference/assets/etama.anm')),anm=installAnm(m,bytes),manager=m.allocate(0x25e200),owner=m.allocate(64),player=m.allocate(0x2400),parameters=m.allocate(0x214),p=c.allocate(bytes.length);
    let f=0,checks=0,animationChecks=0;const collisions=[],at=i=>manager+0x24d424+i*0x59c;
    m.replace(0x412830,'laser-collision-submission',()=>{
        const a=m.readWords(m.reg('ESP')+4,5),b=Buffer.alloc(32);b.set(m.bytes(a[0],8));b.set(m.bytes(a[1],8),8);b.set(m.bytes(a[2],12),16);b.writeUInt32LE(a[3],28);collisions.push(b);return 0;
    },5);
    function verify(label){
        for(let i=0;i<48;++i){const actual=c.laser_at(f,i),original=at(i);
            assert.deepEqual(memory(c,actual+0x548,0x52),m.bytes(original+0x548,0x52),`${label} laser ${i} fields`);
            for(let kind=0;kind<2;++kind){
                const a=normalizedAnm(memory(c,actual+kind*0x2a4,0x2a4),c.laser_part(f,0),c.laser_part(f,1),c.laser_part(f,2));
                const b=normalizedAnm(m.bytes(original+kind*0x2a4,0x2a4),anm.file,anm.source,anm.sprites);
                const differences=[];for(let n=0;n<a.length;++n)if(a[n]!==b[n])differences.push(n);
                assert.equal(differences.length,0,`${label} laser ${i} animation ${kind}: ${differences.slice(0,12).map(n=>n.toString(16))}`);++animationChecks;
            }
        }
        assert.deepEqual(memory(c,c.laser_part(f,3),8),m.bytes(0x4ace0c,8),label+' RNG');++checks;
    }
    try{
        for(const rate of [1,.5,.99]){
            if(f)c.laser_delete_fixture(f);f=c.laser_create_fixture();memory(c,p,bytes.length).set(bytes);assert.equal(c.laser_load(f,p,bytes.length),1);
            m.view(manager,0x25e200).fill(0);m.u32(manager+0x25e190,owner);m.u32(manager+0x25e1bc,anm.file);m.u32(owner+4,player);
            m.f32(0x4b36b8,rate);m.u32(0x4b36d4,0);m.u32(0x4ace0c,0x4715);m.u32(0x4ace10,0);memory(c,c.laser_part(f,3),8).set(m.bytes(0x4ace0c,8));
            const pos=Buffer.alloc(12);pos.writeFloatLE(28.25);pos.writeFloatLE(387.75,4);m.write(player+0x1b88,pos);memory(c,c.laser_part(f,4),12).set(pos);
            for(let n=0;n<55;++n){
                const e=Buffer.alloc(0x210);e.writeInt16LE(n%5);e.writeInt16LE(n%4,2);e.writeFloatLE(n*.375-87,4);e.writeFloatLE(n%23+10,8);e.writeFloatLE(.1,12);e.writeFloatLE((n%7-3)*.27,16);e.writeFloatLE((n%7+1)*.137,24);e.writeInt16LE(n%2,0x1f8);e.writeUInt32LE(n%4,0x1fc);
                e.writeFloatLE(n%3===0?-10:0,0x1d0);e.writeFloatLE(17.5+n,0x1d4);e.writeFloatLE(127.5,0x1d8);e.writeFloatLE(n%6+12.75,0x1dc);
                e.writeInt32LE([0,7,31,43][n%4],0x1e0);e.writeInt32LE(n%4+15,0x1e4);e.writeInt32LE([0,3,11][n%3],0x1e8);e.writeInt32LE(4,0x1ec);e.writeInt32LE(5,0x1f0);
                const cancel=n===0?1:0;m.i32(manager+0x25e170,cancel);m.write(parameters,e);memory(c,p,e.length).set(e);
                const original=m.call(0x413290,{ecx:manager,args:[parameters]});assert.equal(c.laser_spawn(f,p,cancel,rate),(original-at(0))/0x59c,`create ${n}`);verify(`create ${rate}/${n}`);
            }
            m.i32(manager+0x25e170,0);
            for(let frame=0;frame<150;++frame){
                const fieldFlags=frame%29===0?1:0,gameFlags=frame%31===0?0x800:0;collisions.length=0;m.u32(owner+0x34,fieldFlags);m.u32(0x4a7ec4,gameFlags);
                assert.equal(m.call(0x4146f0,{ecx:manager}),1);assert.equal(c.laser_step(f,rate,0,fieldFlags,gameFlags),1);verify(`frame ${rate}/${frame}`);
                assert.equal(c.laser_collision_count(f),collisions.length);assert.deepEqual(memory(c,c.laser_collisions(f),collisions.length*32),new Uint8Array(Buffer.concat(collisions)),`collision ${rate}/${frame}`);
            }
        }
        report('lasers',{passed:true,checks,animationChecks,poolCapacity:48,scope:'Original laser creation and whole-manager updates with real ANM state, empty bullet pools and collision-submission events.'});
    }finally{if(f)c.laser_delete_fixture(f);c.release(p);m.close();}
});
