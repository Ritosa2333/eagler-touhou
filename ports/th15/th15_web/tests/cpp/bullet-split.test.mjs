import test from 'node:test';import assert from 'node:assert/strict';import {core,oracle,memory,string,report,enableNativeMath} from './helpers.mjs';import {bulletFields} from './bullet-state-fields.mjs';
const fields=[...bulletFields,[16,0,0xc9c,4],[24,0,0xc8a,2],[25,0,0x20,4],[31,0,0x1468,20],[33,0,0xc7c,4],[40,0,0x4c4,4]];
test('TH15 splitting transform preserves child shooter state, two-slot cursor consumption',async()=>{
 const c=await core(),m=await oracle(),fixture=c.bullet_state_create(),native=m.allocate(0x1500),player=m.allocate(0x620),p=c.allocate(44);enableNativeMath(m);m.u32(0x4e9bb8,player);m.f32(player+0x61c,384);let emissions=0,captured=null,cancels=0,checks=0,cases=0;
 m.replace(0x41c5c0,'split_child_emission_boundary',()=>{emissions++;captured=Buffer.from(m.bytes(m.u32(m.reg('ESP')+4),896));return 0;},1);m.replace(0x41e250,'split_parent_cancellation_boundary',()=>{cancels++;return 0;},1);
 try{for(let sample=0;sample<1024;sample++){
  const seed=Buffer.alloc(0x1500);[-17+sample*.03125,19+sample*.0625,.1,.5,1,0,2,(sample%71-35)*.125].forEach((v,i)=>seed.writeFloatLE(v,0xc38+i*4));seed.writeInt16LE(1,0xc8a);seed.writeInt32LE(sample%7,0xc9c);seed.writeInt32LE(-1,0xc7c);for(const [k,f,o,n]of fields)memory(c,c.bullet_state_field(fixture,k,f),n).set(seed.subarray(o,o+n));m.write(native,seed);const target=Buffer.alloc(12);target.writeFloatLE(384,4);memory(c,c.bullet_state_field(fixture,15,0),12).set(target);const type=8192,slot=sample%7;
  for(let index=0;index<18;index++){
   const t=Buffer.alloc(44);if(index===slot){const angle=[-999999,-999990,999990,999999,(sample-371)*.03125][sample%5];[angle,.0625,angle,.125].forEach((v,i)=>t.writeFloatLE(v,i*4));[sample%13,17,3,2].forEach((v,i)=>t.writeInt32LE(v,16+i*4));t.writeUInt32LE(type,32);}else if(index===slot+1&&type===8192){t.writeInt32LE(sample%44,16);t.writeInt32LE(sample%8,20);t.writeInt32LE(0,24);}memory(c,p,44).set(t);c.bullet_transform_read(fixture,index,p);m.write(native+0xca4+index*44,t);
  }
  emissions=c.bullet_children_count(fixture);captured=null;m.call(0x41b180,{ecx:native});
  assert.equal(c.bullet_activate(fixture),1,string(c,c.bullet_activation_error(fixture)));const label=JSON.stringify({sample,slot});for(const [k,f,o,n]of fields)assert.deepEqual(Buffer.from(memory(c,c.bullet_state_field(fixture,k,f),n)),Buffer.from(m.bytes(native+o,n)),label+' state '+o.toString(16));assert.equal(c.bullet_children_count(fixture),emissions,label+' emissions');assert.deepEqual(Buffer.from(memory(c,c.bullet_children_bytes(fixture),896)),captured,label+' child shooter');checks+=fields.length+3;cases++;
 }report('bullet-splitting-transform',{passed:true,cases,checks,originalFunction:'0x0041b180',scope:'Split child configuration, sentinels, copied transform chain and two-slot cursor advancement. Child emission is compared at its manager boundary; full pool recursion and parent-cancellation effects have separate tests.'});
 }finally{c.bullet_state_delete(fixture);c.release(p);m.close();}
});
