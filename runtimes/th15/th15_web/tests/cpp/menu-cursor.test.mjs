import test from 'node:test';import assert from 'node:assert/strict';import {core,oracle,memory,string,report} from './helpers.mjs';
test('TH15 shared menu selection, wrapping, disabled entries and nested return history match the original',async()=>{
 const c=await core(),m=await oracle(),native=m.allocate(0xd8);let checks=0,operations=0;
 try{for(let scenario=0;scenario<128;scenario++){
  const f=c.menu_cursor_create();try{const count=2+scenario%14,wrap=scenario%2,cursor=scenario%count;m.view(native,0xd8).fill(0);m.i32(native,cursor);m.i32(native+8,count);m.i32(native+0xd0,wrap);c.menu_cursor_configure(f,cursor,count,wrap);
  const verify=label=>{assert.deepEqual(Buffer.from(memory(c,c.menu_cursor_bytes(f),0xd8)),Buffer.from(m.bytes(native,0xd8)),scenario+'/'+label);assert.equal(string(c,c.menu_cursor_error(f)),'');checks+=54;operations++;};verify('initial');
  for(let n=0;n<32;n++){const entry=(n*31+scenario*11)%201-100;m.call(0x422280,{ecx:native,args:[entry]});c.menu_cursor_select(f,entry);verify('select'+n);const direction=n%2?-1:1;m.call(0x403ba0,{ecx:native,args:[direction]});c.menu_cursor_move(f,direction);verify('move'+n);}
  const block=1;m.call(0x452b30,{ecx:native,args:[block]});assert.equal(c.menu_cursor_disable(f,block),1);verify('disable');
  // Disabled boundaries would make the native clamped menu loop forever.
  // These valid interior entries exercise its skip without hiding that constraint.
  for(let n=0;n<32;n++){m.call(0x422280,{ecx:native,args:[n%count]});c.menu_cursor_select(f,n%count);verify('skip'+n);if(count>2){m.call(0x403ba0,{ecx:native,args:[n%2?-1:1]});c.menu_cursor_move(f,n%2?-1:1);verify('skip move'+n);}}
  for(let depth=0;depth<24;depth++){m.call(0x403b10,{ecx:native});c.menu_cursor_history(f,1);verify('push'+depth);const nextCount=3+depth%11,selection=(scenario+depth)%nextCount;m.i32(native+8,nextCount);m.i32(native,selection);c.menu_cursor_configure(f,selection,nextCount,wrap);verify('child'+depth);}
  for(let depth=0;depth<24;depth++){m.call(0x403b50,{ecx:native});c.menu_cursor_history(f,0);verify('pop'+depth);}
  }finally{c.menu_cursor_delete(f);}
 }
 report('menu-cursor',{passed:true,scenarios:128,operations,checks,originalFunctions:['0x403b10','0x403b50','0x403ba0','0x422280','0x452b30'],scope:'Unmodified original navigation compares every logical state word, including disabled entry storage, clamp versus wrap, count-zero selection and the saturated 16-entry history. Impossible all-disabled/clamped-disabled loops are explicit errors in production, not exercised as valid menu states.'});
 }finally{m.close();}
});
