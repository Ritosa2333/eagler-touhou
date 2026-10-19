import test from 'node:test';
import assert from 'node:assert/strict';
import {oracle,core,bits,floatWrapper,memory,report} from './helpers.mjs';
test('TH09 angle wrapping, rotation and interpolation preserve original arithmetic',async()=>{
    const m=await oracle(),c=await core(),p=c.allocate(32),q=m.allocate(32);let checks=0;
    try{
        const angle=floatWrapper(m,0x42aed0,2),curve=floatWrapper(m,0x401000,5);
        const values=[0,-0,Math.PI,-Math.PI,Math.PI*2,Math.PI*18,-Math.PI*18,0.00001,-0.1,0.1,-1000,1000];
        for(const a of values)for(const d of values){assert.equal(bits(c.angle_add(a,d)),m.call(angle,{args:[bits(a),bits(d)]}));checks++;}
        for(let i=0;i<8192;++i){
            const a=Math.fround((i-4096)*0.031337),x=Math.fround(((i*127)%1024)-512.01),y=Math.fround(((i*51)%1024)-512.001);
            const data=new Uint8Array(new Float32Array([x,y]).buffer);memory(c,p,8).set(data);m.write(q,data);
            c.vector_rotate(p+8,p,a);m.call(0x42af40,{ecx:q+8,edx:q,args:[bits(a)]});assert.deepEqual(memory(c,p+8,8),m.bytes(q+8,8),`rotation ${i}`);
            const parameters=[x,y,x/3,y/7,Math.fround((i%100)/99)];assert.equal(bits(c.curve_hermite(...parameters)),m.call(curve,{args:parameters.map(bits)}),`curve ${i}`);checks+=2;
        }
        report('game-math',{passed:true,checks,fpuControl:0x7f,rotationRange:[-129,129]});
    }finally{c.release(p);m.close();}
});
test('TH09 rotated player hit and near-miss results match original',async()=>{
    const m=await oracle(),c=await core(),p=c.allocate(80),q=m.allocate(0x31000),points=m.allocate(80);let checks=0,seed=0x935ab67;
    const random=()=>{seed=(Math.imul(seed,1664525)+1013904223)>>>0;return (seed%2048-1024)/8;};
    try{
        for(let i=0;i<8192;++i){
            const values=new Float32Array([random(),random(),0,Math.abs(random())/32,Math.abs(random())/32,0,random(),random(),0,Math.abs(random())/4,Math.abs(random())/4,0,random(),random(),0]);
            memory(c,p,values.byteLength).set(new Uint8Array(values.buffer));m.write(points,new Uint8Array(values.buffer));m.write(q+0x1b88,new Uint8Array(values.buffer,0,12));m.write(q+0x1ca8,new Uint8Array(values.buffer,12,12));
            const angle=Math.fround(random()/10),near=i%2;
            const expected=m.call(0x41bfc0,{ecx:q,args:[points+24,points+36,points+48,bits(angle),near]});
            assert.equal(c.collision_rotated(p,p+12,p+24,p+36,p+48,angle,near),expected,`rotated collision ${i}`);checks++;
        }
        report('rotated-collision',{passed:true,checks});
    }finally{c.release(p);m.close();}
});
