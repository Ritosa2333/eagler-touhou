import test from 'node:test';import assert from 'node:assert/strict';import {readFileSync} from 'node:fs';import {resolve} from 'node:path';
import {core,oracle,memory,report,root} from './helpers.mjs';import {graphicsOracle} from './graphics-oracle.mjs';
test('TH11 stage bounding-box visibility matches original culling',async()=>{
 const c=await core(),m=await oracle();graphicsOracle(m);const camera=c.allocate(0x118),object=c.allocate(24),instance=c.allocate(12),nc=m.allocate(0x118),no=m.allocate(28),ni=m.allocate(12);let checks=0,visible=0,seed=731912;
 const random=()=>{seed=(Math.imul(seed,1664525)+1013904223)>>>0;return seed/4294967296;};
 let nativePoints=[],nativeProjected=[];
 if(process.env.TH11_CULL_DEBUG){m.hooks.push(m.cpu.hook_add(m.uc.HOOK_CODE,()=>{const p=m.u32(m.reg('ESP')+8);nativePoints=Array.from({length:36},(_,i)=>m.f32(p+i*4));},null,0x403c43,0x403c43));m.hooks.push(m.cpu.hook_add(m.uc.HOOK_CODE,()=>{const p=m.reg('ESP')+0x108;nativeProjected=Array.from({length:36},(_,i)=>m.f32(p+i*4));},null,0x403c48,0x403c48));}
 const compare=(b,p,limit,label)=>{memory(c,object,24).set(b.subarray(4,28));memory(c,instance,12).set(p);m.write(no,b);m.write(ni,p);m.write(nc,memory(c,camera,0x118));
  const actual=c.stage_visible_test(object,object+12,instance,camera,limit),bits=Buffer.alloc(4);bits.writeFloatLE(limit);m.reg('EAX',nc);const expected=m.call(0x403950,{ecx:no,edx:ni,args:[bits.readUInt32LE()],limit:100000});if(actual!==1-expected&&process.env.TH11_CULL_DEBUG)console.log({object:Array.from({length:6},(_,i)=>b.readFloatLE(4+i*4)),instance:Array.from({length:3},(_,i)=>p.readFloatLE(i*4)),nativePoints,nativeProjected});assert.equal(actual,1-expected,label);++checks;visible+=actual;
 };
 try{for(let view=0;view<16;++view){const b=Buffer.alloc(0x118);b.writeFloatLE(view?random()*2000-1000:0,0);b.writeFloatLE(view?random()*2000-1000:0,4);b.writeFloatLE(view?random()*2000-1000:-600,8);
   for(let i=0;i<3;++i){b.writeFloatLE(view?random()*800-400:[0,300,600][i],12+i*4);b.writeFloatLE(view?random()*40-20:0,60+i*4);}b.writeFloatLE(1,28);b.writeFloatLE(view?.2+random():.5235987901687622,72);
   for(const [i,value]of [32,16,384,448].entries())b.writeUInt32LE(value,0xcc+i*4);b.writeFloatLE(1,0xe0);memory(c,camera,b.length).set(b);c.scene_camera_update(camera,0);
   for(let number=1;number<=7;++number){const std=readFileSync(resolve(root,`reference/assets/stage0${number}.std`));
    for(let p=std.readUInt32LE(4);std.readInt16LE(p)>=0;p+=16){const id=std.readInt16LE(p),start=std.readUInt32LE(0x90+id*4);compare(std.subarray(start,start+28),std.subarray(p+4,p+16),9610000,`view ${view} stage ${number} instance ${p}`);}
   }
   for(let sample=0;sample<256;++sample){const o=Buffer.alloc(28),p=Buffer.alloc(12);for(let i=0;i<3;++i){o.writeFloatLE(random()*4000-2000,4+i*4);o.writeFloatLE(random()*1600,16+i*4);p.writeFloatLE(random()*2000-1000,i*4);}compare(o,p,sample%3?9610000:random()*9000000,`view ${view} synthetic ${sample}`);}
  }report('stage-culling',{passed:true,checks,visible,scope:'Original 403950 bounding-box visibility, all seven STD instance sets, sixteen cameras, synthetic boxes and distance limits; native D3DX projection.'});
 }finally{m.close();for(const p of[camera,object,instance])c.release(p);}
});
