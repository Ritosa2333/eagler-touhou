import test from 'node:test';import assert from 'node:assert/strict';import{core,oracle,memory,report}from'./helpers.mjs';
test('TH11 menu selection, disabled entries, wrap and history match native helpers',async()=>{
 const c=await core(),m=await oracle(),p=c.menu_cursor_create(),np=m.allocate(0xd8);let checks=0;
 const compare=label=>{assert.deepEqual(Buffer.from(memory(c,p,0xd8)),Buffer.from(m.bytes(np,0xd8)),label);++checks;};
 try{for(let sample=0;sample<160;++sample){const state=Buffer.alloc(0xd8),count=sample%12+1;state.writeInt32LE(count,8);state.writeInt32LE(sample%2,0xd0);if(count>2&&sample%2){state.writeInt32LE(1,0x90);state.writeInt32LE(1,0xd4);}memory(c,p,0xd8).set(state);m.write(np,state);
  for(const value of[-13,-1,0,1,count-1,count,count+123]){const actual=c.menu_cursor_select(p,value),expected=m.call(0x40df10,{ecx:value,edx:np})|0;assert.equal(actual,expected);compare('select');}
  for(let step=0;step<50;++step){const delta=step%3?-1:1;m.reg('EAX',np);const expected=m.call(0x459160,{args:[delta]})|0;assert.equal(c.menu_cursor_move(p,delta),expected);compare('move');}
  for(let step=0;step<18;++step){m.reg('EAX',np);m.call(0x4590f0);c.menu_cursor_push(p);compare('push');}
  for(let step=0;step<18;++step){m.reg('EAX',np);m.call(0x459130);c.menu_cursor_pop(p);compare('pop');}
 }report('menu-cursor',{passed:true,checks,scope:'Native 40df10 and 4590f0/459130/459160: clamping, wrap, disabled entries and bounded history for original menus. Invalid all-disabled menu is guarded separately in C++.'});
 }finally{c.menu_cursor_delete(p);m.close();}
});
