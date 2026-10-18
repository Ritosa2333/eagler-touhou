import test from 'node:test';import assert from 'node:assert/strict';import{core,oracle,memory,report}from'./helpers.mjs';
test('TH11 controller buttons and deadzone boundaries match native 457270',async()=>{
 const c=await core(),m=await oracle(),device=m.allocate(4),vtable=m.allocate(128),buttons=new Uint8Array(128),cfg=Buffer.alloc(60),bp=c.allocate(128),cp=c.allocate(60);let x=0,y=0,checks=0;
 m.u32(device,vtable);m.u32(0x4c32a4,device);m.u32(0x4c3810,0x800);
 m.u32(vtable+0x64,m.registerImport({dll:'controller',name:'Poll',handler:()=>0,argc:1}));
 m.u32(vtable+0x24,m.registerImport({dll:'controller',name:'GetDeviceState',argc:3,handler:()=>{const p=m.u32(m.reg('ESP')+12);m.i32(p,x);m.i32(p+4,y);m.write(p+0x30,buttons);return 0;}}));
 try{for(const mapping of[[0,1,2,3,4],[31,63,100,127,5],[-1,-1,-1,-1,-1],[2,2,2,2,2]]){
  for(let i=0;i<5;++i)cfg.writeInt16LE(mapping[i],4+[0,1,2,3,8][i]*2);m.write(0x4c93dc,cfg.subarray(4,22));
  for(const deadzone of[0,600,999]){cfg.writeInt16LE(deadzone,22);cfg.writeInt16LE(deadzone,24);m.write(0x4c345e,cfg.subarray(22,26));memory(c,cp,60).set(cfg);
   for(let bits=0;bits<32;++bits){buttons.fill(0);for(let i=0;i<5;++i)if(mapping[i]>=0&&(bits&(1<<i)))buttons[mapping[i]]=128;memory(c,bp,128).set(buttons);
    for(const axis of[-1000,-deadzone-1,-deadzone,0,deadzone,deadzone+1,1000]){x=axis;y=-axis;const held=bits%2?0x10000:0,expected=m.call(0x457270,{ecx:held});assert.equal(c.controller_input(held,bp,128,x,y,cp),expected,`${mapping} threshold${deadzone} buttons${bits} axis${axis}`);++checks;}
   }
  }
 }
 report('controller-input',{passed:true,checks,scope:'Native 457270 DirectInput device boundary: configurable buttons including disabled/duplicate mappings and exact positive/negative axis thresholds.'});
 }finally{c.release(bp);c.release(cp);m.close();}
});
