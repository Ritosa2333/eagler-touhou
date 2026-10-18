import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,string,report,enableNativeSine} from './helpers.mjs';
function ins(op,args=[],mask=0,time=0){const b=Buffer.alloc(8+args.length*4);b.writeInt16LE(op);b.writeUInt16LE(b.length,2);b.writeInt16LE(time,4);b.writeUInt16LE(mask,6);args.forEach((a,i)=>typeof a==='object'?b.writeFloatLE(a.f,8+i*4):b.writeInt32LE(a|0,8+i*4));return b;}
test('TH15 animation interpolation commands reproduce original frame properties and interpolator state',async()=>{
 const c=await core(),m=await oracle(),vm=c.anm_vm_create(),rng=c.rng_create(),native=m.allocate(0x608),bank=m.allocate(0x13c),table=m.allocate(4),manager=m.allocate(0x187f600),source=m.allocate(4096),p=c.allocate(4096);enableNativeSine(m);let checks=0,frames=0;
 const opKinds=new Map([[407,0],[408,1],[409,2],[410,3],[411,4],[412,5],[413,8],[414,9],[417,2],[420,0],[427,10],[428,11],[430,7],[435,6]]),nativeInterpolation=[[0x98,88],[0xf0,88],[0x148,48],[0x178,88],[0x1d0,48],[0x200,68],[0x244,68],[0x288,68],[0x2cc,88],[0x324,48],[0x354,48],[0x384,48]];
 const fields=[[0,0x18,8],[8,0x24,4],[16,0x4ec,12],[28,0x50,12],[40,0x5c,8],[48,0x64,8],[56,0x6c,8],[64,0x74,8],[72,0x84,8],[80,0x3d4,8],[88,0x8c,8],[96,0x538,8]];
 m.u32(0x503c18,manager);m.u32(manager+0x187f4d8,bank);m.u32(bank+0x120,table);m.u32(table,source);m.u32(0x4ca620,0x4e73e8);
 try{for(const [op,kind]of opKinds)for(let mode=0;mode<=31;mode++)for(let sample=0;sample<6;sample++){
 const duration=[0,-1,1,7,19,90][sample],f={f:(sample-3)*13.375},g={f:(sample-2)*17.625},h={f:(sample-1)*11.875};let args;
 if([407,410].includes(op))args=[duration,mode,f,g,h];else if([412,430,435].includes(op))args=[duration,mode,f,g];else if([408,413].includes(op))args=[duration,mode,sample*73-7,sample*59+67,sample*101+11];else if(op===417)args=[mode,duration];else if(op===420)args=[duration,{f:-1.25},{f:2.75},{f:-3.5},f,g,h,{f:4.5},{f:-5.25},{f:6.5}];else args=[duration,mode,op===409||op===414?sample*73-7:f];
 const bytes=Buffer.concat([ins(400,[{f:13.25},{f:-21.375},{f:1.5}]),ins(401,[{f:-1.125},{f:2.375},{f:2.875}]),ins(403,[190]),ins(404,[210,100,70]),ins(405,[93]),ins(406,[50,30,10]),ins(425,[{f:-.25}]),ins(426,[{f:.375}]),ins(op,args),ins(3,[],0,80),ins(-1)]);
 memory(c,p,bytes.length).set(bytes);const resource=c.anm_fixture_create(p,bytes.length);assert.equal(c.anm_vm_bind(vm,resource,0),1);m.view(native,0x608).fill(0);m.write(source,bytes);m.call(0x4773e0,{ecx:bank,args:[native,0]});if(sample&1){m.u32(native+0x18,m.u32(native+0x18)|0x400);new DataView(c.memory.buffer,c.anm_vm_visual(vm),4).setUint32(0,6|0x400,true);}
 const rate=[.5,1,1.5][sample%3];m.f32(0x4e73e8,rate);
 for(let frame=0;frame<45;frame++){
 const label=JSON.stringify({op,mode,sample,frame});assert.equal(c.anm_vm_tick(vm,rng,rate),0,label+' '+string(c,c.anm_vm_error(vm)));m.call(0x477e10,{ecx:native,limit:10000000});const vp=c.anm_vm_visual(vm),vars=c.anm_vm_variables(vm);
 for(const [co,no,n]of fields){assert.deepEqual(Buffer.from(memory(c,vp+co,n)),Buffer.from(m.bytes(native+no,n)),label+' field '+no.toString(16));checks++;}
 for(const [co,no,n]of [[64,0x38,12],[76,0x44,12]]){assert.deepEqual(Buffer.from(memory(c,vars+co,n)),Buffer.from(m.bytes(native+no,n)),label+' variable '+no.toString(16));checks++;}
 const [no,size]=nativeInterpolation[kind];assert.deepEqual(Buffer.from(memory(c,c.anm_vm_interpolation(vm,kind),size)),Buffer.from(m.bytes(native+no,size)),label+' interpolator');assert.equal(c.anm_vm_offset(vm),m.i32(native+0x34));assert.deepEqual(Buffer.from(memory(c,c.anm_vm_timer(vm),20)),Buffer.from(m.bytes(native+0x54c,20)),label+' timer');checks+=3;frames++;
 }c.anm_delete(resource);
 }report('anm-interpolation',{passed:true,checks,frames,coveredOpcodes:[...opKinds.keys()],coveredModes:32,originalFunction:'0x477e10'});
 }finally{c.anm_vm_delete(vm);c.rng_delete(rng);c.release(p);m.close();}
});
