import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,oracle,memory,string,root,report,enableNativeMath} from './helpers.mjs';
import {resourceLoader,nativePool,nativeInstances,prepareBank,compareAnm} from './anm-oracle.mjs';

test('TH17 original item allocation, delayed births, cancellation density and spirit routing',async()=>{
 const c=await core(),m=await oracle(),manager=c.anm_manager_create(),nm=nativePool(m),loader=resourceLoader(m);
 const native=m.allocate(0xe4b988),slots=m.allocate(24),effects=m.allocate(0x1100),text=m.allocate(0x19270),player=m.allocate(0x19100),position=m.allocate(12);
 enableNativeMath(m);for(const [at,p]of [[0x4b76b8,native],[0x4b768c,slots-0x71b538],[0x4b7698,effects],[0x4b7678,text],[0x4b77d0,player],[0x4a0430,0x4b5918]])m.u32(at,p);m.f32(0x4b5918,1);
 let fixture,sounds=[],spirits=[],checks=0,requests=0;
 m.replace(0x465450,'observe item birth queued audio',()=>{sounds.push(m.i32(m.reg('ESP')+4));return 0;},2);
 const direction=m.allocate(4),spiritBoundary=m.registerImport({dll:'fixture',name:'observe original item-to-animal-spirit boundary',argc:2,handler:()=>{const p=m.u32(m.reg('ESP')+4);spirits.push(m.u32(m.reg('ESP')+8),m.u32(p),m.u32(p+4),m.u32(p+8),m.u32(direction));return 0;}}),capture=m.allocate(16),code=Buffer.from([0xf3,0x0f,0x11,0x15,0,0,0,0,0xe9,0,0,0,0]);code.writeUInt32LE(direction,4);code.writeInt32LE(spiritBoundary-capture-code.length,9);m.write(capture,code);const jump=Buffer.from([0xe9,0,0,0,0]);jump.writeInt32LE(capture-0x410380-5,1);m.write(0x410380,jump);
 const bits=f=>{const b=Buffer.alloc(4);b.writeFloatLE(f);return b.readUInt32LE();},item=id=>native+0x14+(id-1)*0xc78,idOf=p=>p?(p-native-0x14)/0xc78+1:0;
 const equal=(a,b,label)=>{if(!a.equals(b)){let i=0;while(i<a.length&&a[i]===b[i])i++;assert.fail(label+' byte '+i.toString(16)+' actual='+a.subarray(Math.max(0,i-4),i+12).toString('hex')+' original='+b.subarray(Math.max(0,i-4),i+12).toString('hex'));}};
 const load=(name,id)=>{const b=readFileSync(resolve(root,'reference/assets',name)),p=c.allocate(b.length);memory(c,p,b.length).set(b);assert.equal(c.anm_manager_load(manager,id,p,b.length),1,string(c,c.anm_manager_error(manager)));c.deallocate(p);return prepareBank(m,loader,name,b,nm,id);};
 const check=label=>{const dv=new DataView(c.memory.buffer);for(const [f,o]of [[0,0xe4b97c],[1,0xe4b980],[2,0xe4b984],[4,0xe4b978],[5,0xe4b974]])assert.equal(dv.getUint32(c.item_manager_field(fixture,f),true),m.u32(native+o),label+' counter '+f);for(let cancel=0;cancel<2;cancel++)assert.equal(c.item_manager_free(fixture,cancel),idOf(m.u32(native+(cancel?0xe4b968:0xe4b958))),label+' free '+cancel);for(let v=0;v<2;v++)equal(Buffer.from(memory(c,c.item_manager_field(fixture,6+v),8)),Buffer.from(m.bytes(v?0x4b7660:0x4b7668,8)),label+' RNG '+v);assert.deepEqual(Array.from(new Int32Array(c.memory.buffer,c.item_manager_sounds(fixture),c.item_manager_sound_count(fixture))),sounds,label+' sounds');assert.deepEqual(Array.from(new Uint32Array(c.memory.buffer,c.item_manager_spirits(fixture),c.item_manager_spirit_count(fixture))),spirits,label+' spirit routing');checks++;};
 const spawn=(kind,sample)=>{
  const pos=[Math.fround((sample%43-21)*17.25),Math.fround((sample%17)*24.125),Math.fround((sample%3-1)*.0625)],angle=Math.fround((sample%33-16)*.125),speed=Math.fround((sample%13)*.25),density=[0,255,256,511,512,1023,1024,4096][sample%8],delay=sample%5?0:sample%19,sound=sample%4? -1:sample%75;
  const dv=new DataView(c.memory.buffer);dv.setInt32(c.item_manager_field(fixture,1),density,true);dv.setInt32(c.item_manager_field(fixture,2),sample,true);m.i32(native+0xe4b980,density);m.i32(native+0xe4b984,sample);
  for(let i=0;i<3;i++){m.f32(position+i*4,pos[i]);m.f32(player+0x610+i*4,[12,400,0][i]);dv.setFloat32(c.item_manager_field(fixture,8)+i*4,[12,400,0][i],true);}
  const expected=idOf(m.call(0x434740,{ecx:native,args:[kind,position,0,bits(angle),bits(speed),delay,0,sound]})),actual=c.item_manager_spawn(fixture,kind,...pos,angle,speed,delay,sound);
  assert.equal(actual,expected,'spawn '+kind+'/'+sample+' '+string(c,c.item_manager_error(fixture)));assert.equal(string(c,c.item_manager_error(fixture)),'');
  if(actual){equal(Buffer.from(memory(c,c.item_manager_state(fixture,actual),104)),Buffer.from(m.bytes(item(actual)+0xc10,104)),'item '+kind+'/'+sample);if(kind<9||kind===15){for(let arrow=0;arrow<2;arrow++)compareAnm(c,m,c.item_manager_vm(fixture,actual,arrow),item(actual)+(arrow?0x610:0x10),'VM '+kind+'/'+sample+'/'+arrow);}}
  check('spawn '+kind+'/'+sample);requests++;return actual;
 };
 try{
  m.u32(text+0x19248,load('ascii.anm',2));assert.equal(c.anm_manager_fallback(manager,2),1);m.u32(effects+12,load('effect.anm',8));m.u32(slots+16,load('bullet.anm',9));fixture=c.item_manager_create(manager,9);m.call(0x41b3c0,{ecx:native});
  for(let v=0;v<2;v++){const b=Buffer.alloc(8);b.writeUInt16LE(3717+v*137);m.write(v?0x4b7660:0x4b7668,b);memory(c,c.item_manager_field(fixture,6+v),8).set(b);}check('reset');
  for(let i=0;i<600;i++)spawn([1,2,3,4,5,6,7,8,15][i%9],i);assert.equal(spawn(1,600),0);
  for(let i=0;i<4096;i++)spawn(9+i%6,1000+i);assert.equal(spawn(9,6000),0);
  for(let i=0;i<256;i++)spawn(16+i%18,7000+i);
  for(let alt=0;alt<2;alt++){const list=nativeInstances(m,nm,alt);assert.equal(c.anm_manager_count(manager,alt),list.length);for(const p of list)compareAnm(c,m,c.anm_manager_find(manager,m.u32(p+0x538)),p,'birth effect '+p);}
  for(let cycle=0;cycle<3;cycle++){
   const dv=new DataView(c.memory.buffer);for(const [f,o,v,float]of [[1,0xe4b980,321+cycle,false],[4,0xe4b978,111+cycle,false],[5,0xe4b974,2.5+cycle,true]]){if(float){dv.setFloat32(c.item_manager_field(fixture,f),v,true);m.f32(native+o,v);}else{dv.setInt32(c.item_manager_field(fixture,f),v,true);m.i32(native+o,v);}}
   m.call(0x41b3c0,{ecx:native});assert.equal(c.item_manager_reset(fixture),1,string(c,c.item_manager_error(fixture)));check('scene reset '+cycle);
   for(let id=1;id<=4696;id++){equal(Buffer.from(memory(c,c.item_manager_state(fixture,id),104)),Buffer.from(m.bytes(item(id)+0xc10,104)),'reset '+cycle+'/'+id);assert.equal(c.item_manager_vm(fixture,id,0),0);assert.equal(c.item_manager_vm(fixture,id,1),0);}
   for(let i=0;i<24;i++)spawn([1,2,3,4,6,15,9,10,11,12,13,14][i%12],cycle*100+i);
  }
  report('item-pool-generation',{passed:true,ordinarySlots:600,cancelSlots:4096,requests,checks,resets:3,originalFunctions:['0x41b3c0','0x434740','0x434b80','0x434cf0'],scope:'Exact native item states, fixed allocation order, full pool rejection, all ordinary/cancellation kinds, alias 15, spawn delays/custom audio, 16..33 animal-spirit routing and consumed RNG, original embedded/pooled animations, repeated resets. Spirit lifecycle and item collection/frame movement are separate unfinished integration.'});
 }finally{if(fixture)c.item_manager_delete(fixture);c.anm_manager_delete(manager);m.close();}
});
