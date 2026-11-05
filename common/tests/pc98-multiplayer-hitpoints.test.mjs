import test from 'node:test';
import assert from 'node:assert/strict';
import {collisionPoints,drawCollisionPoints} from '../../ports/th04/web/netplay/collision-points.js';
import {collisionPoints as th02Points} from '../../ports/th02/web/netplay/collision-points.js';

for(const slot of [0,1,2])test(`TH02 player ${slot+1} sees only their own marker`,()=>{
  const players=[0,1,2].map(i=>({x:40+i*50,y:100+i*30,hit:false,out:false}));
  const args={state:{players},slot,raw:false,paused:false,preferences:{focus:true,always:false},inputs:[64,64,64]};
  assert.deepEqual(th02Points(args),[[56+slot*50,121+slot*30]]);
  assert.deepEqual(th02Points({...args,raw:true}),[]);
  assert.deepEqual(th02Points({...args,paused:true}),[]);
  const inputs=[64,64,64];inputs[slot]=0;
  assert.deepEqual(th02Points({...args,inputs}),[],'teammates focusing do not enable the local marker');
  assert.deepEqual(th02Points({...args,inputs,preferences:{focus:true,always:true}}),[[56+slot*50,121+slot*30]]);
});

function state(){
  const heap=new Uint8Array(0x50000),view=new DataView(heap.buffer),mailbox=0x10000;
  const patch={mailbox_cs_offset:0x100,data_segment:0x2000,code_segment:0x1000,p3_input_cs_offset:0x300,p3_motion_cs_offset:0x400,touch_status_cs_offset:0x500};
  const code=mailbox-patch.mailbox_cs_offset,data=code+0x10000,motions=[data+0x464c,mailbox+44,code+0x400];
  heap[code+0x503]=1;
  motions.forEach((at,i)=>{view.setInt16(at+2,(50+i*60)*16,true);view.setInt16(at+4,(80+i*40)*16,true);});
  heap[data+0x3976]=heap[mailbox+24]=heap[code+0x302]=1;
  return {heap,mailbox,patch,code,motions};
}

for(const slot of [0,1,2])test(`TH04 player ${slot+1} sees only their own marker`,()=>{
  const s=state(),before=s.heap.slice(),expected=[[82+slot*60,96+slot*40]];
  assert.deepEqual(collisionPoints({...s,slot,always:true}),expected);
  assert.deepEqual(collisionPoints({...s,slot,focus:true}),expected);
  assert.deepEqual(collisionPoints({...s,slot,focus:false,always:false}),[]);
  assert.deepEqual(s.heap,before,'per-view drawing does not modify synchronized memory');
  const calls=[],ctx={save(){},restore(){},beginPath(){},rect(){},clip(){},fillRect(...args){calls.push(args);}};
  drawCollisionPoints(ctx,{...s,slot,always:true});
  assert.equal(calls.length,2,'one marker consists of an outline and center');
  assert.deepEqual(calls[0],[expected[0][0]-2,expected[0][1]-2,5,5]);
});

test('TH04 suppresses the local marker when the local player is inactive',()=>{
  const s=state();s.heap[s.code+0x501]=1;
  assert.deepEqual(collisionPoints({...s,slot:1,always:true}),[]);
  assert.deepEqual(collisionPoints({...s,slot:0,always:true}),[[82,96]]);
});
