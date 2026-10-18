import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync,readdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,oracle,root,memory,bits,floatWrapper,report} from './helpers.mjs';

test('TH09 ECL resolver maps locals, shared values and each playfield exactly',async()=>{
    const m=await oracle(),c=await core(),p=c.ecl_vars_create(),enemy=m.allocate(0x6000),context=m.allocate(0x300),manager=m.allocate(0x400),player=m.allocate(0x2000),otherPlayer=m.allocate(0x2000),owner=m.allocate(64),otherOwner=m.allocate(64),argument=m.allocate(4);
    m.u32(enemy,manager);m.u32(enemy+0x2ce0,context);m.u32(manager+0x320,owner);m.u32(manager+0x324,otherOwner);m.u32(owner+4,player);m.u32(otherOwner+4,otherPlayer);
    const bases=[enemy,context,manager,player,otherPlayer,0,owner],fp=floatWrapper(m,0x4068a0,1);
    const dv=new DataView(c.memory.buffer),fields=Array.from({length:c.ecl_var_field_count()},(_,i)=>Array.from({length:4},(_,k)=>dv.getUint32(c.ecl_var_fields()+i*16+k*4,true)));
    let seed=0x349812af,checks=0;const next=()=>seed=(Math.imul(seed,1664525)+1013904223)>>>0;
    const nativeAddress=q=>{for(const [kind,off,local,n] of fields)if(q>=p+local&&q<p+local+n)return bases[kind]+off+(q-p-local);return 0;};
    try{
        for(let sample=0;sample<180;++sample){
            for(const [kind,off,local,n] of fields){
                const bytes=new Uint8Array(n),v=new DataView(bytes.buffer);
                for(let i=0;i+4<=n;i+=4){const b=next();v.setFloat32(i,(b%20000000-10000000)/(sample%7===0?1:32768),true);}
                if(n%4)bytes[n-1]=next()&255;
                memory(c,p+local,n).set(bytes);m.write(bases[kind]+off,bytes);
            }
            for(let id=9995;id<=10110;++id){
                const rng=fields.find(f=>f[0]===5&&f[1]===0x4ace0c),rngp=p+rng[2];
                let start=next();dv.setUint16(rngp,start&65535,true);dv.setUint32(rngp+4,start>>>16,true);m.write(0x4ace0c,memory(c,rngp,8));
                assert.equal(c.ecl_read_int(p,id)>>>0,m.call(0x405760,{ecx:enemy,edx:id}),`integer ${sample}/${id}`);
                assert.deepEqual(memory(c,rngp,8),m.bytes(0x4ace0c,8));
                start=next();dv.setUint16(rngp,start&65535,true);dv.setUint32(rngp+4,start>>>16,true);m.write(0x4ace0c,memory(c,rngp,8));
                const value=Math.fround(id+(sample%2?0.25:0));
                assert.equal(bits(c.ecl_read_float(p,value)),m.call(fp,{ecx:enemy,args:[bits(value)]}),`float ${sample}/${id}`);
                assert.deepEqual(memory(c,rngp,8),m.bytes(0x4ace0c,8));
                for(let real=0;real<2;++real){
                    m.u32(argument,real?bits(value):id);
                    const actual=c.ecl_write_target(p,id,real),expected=m.call(real?0x406210:0x405f20,{ecx:enemy,edx:argument,args:[1,0]});
                    assert.equal(actual?nativeAddress(actual):argument,expected,`target ${sample}/${id}/${real}`);
                }
                checks+=4;
            }
        }
        report('ecl-variables',{passed:true,checks,samples:180,variableRange:[9995,10110],originalResolvers:['0x405760','0x4068a0','0x405f20','0x406210']});
    }finally{c.ecl_vars_delete(p);m.close();}
});

test('TH09 all original ECL resources match original subroutine and timeline tables',async()=>{
    const m=await oracle(),c=await core(),program=c.ecl_program_create(),p=c.allocate(65536),manager=m.allocate(0x400);let current;const results=[];
    m.replace(0x42c970,'read-ecl',()=>{const q=m.allocate(current.length);m.write(q,current);return q;},1);
    try{
        for(const name of readdirSync(resolve(root,'reference/assets')).filter(n=>n.endsWith('.ecl'))){
            current=readFileSync(resolve(root,'reference/assets',name));memory(c,p,current.length).set(current);
            assert.equal(c.ecl_program_load(program,p,current.length),1,name);assert.equal(m.call(0x405660,{ecx:manager,args:[0x2020000]}),0);
            const base=m.u32(manager),counts=[current.readUInt16LE(4),current.readUInt16LE(6)];
            for(let timeline=0;timeline<2;++timeline){
                assert.equal(c.ecl_program_count(program,timeline),counts[timeline]);
                for(let i=0;i<counts[timeline];++i){
                    const table=timeline?base+8:m.u32(manager+4);
                    assert.equal(c.ecl_program_offset(program,i,timeline),m.u32(table+i*4)-base,`${name}/${timeline}/${i}`);
                }
            }
            results.push({name,subroutines:counts[0],timelines:counts[1]});
            assert.equal(c.ecl_program_load(program,p,7),0);
            memory(c,p,current.length).set(current);new DataView(c.memory.buffer).setUint32(p+8,0xffffffff,true);
            assert.equal(c.ecl_program_load(program,p,current.length),0);
        }
        assert.equal(results.length,17);report('ecl-programs',{passed:true,resources:results,invalidFilesRejected:results.length*2});
    }finally{c.ecl_program_delete(program);c.release(p);m.close();}
});
