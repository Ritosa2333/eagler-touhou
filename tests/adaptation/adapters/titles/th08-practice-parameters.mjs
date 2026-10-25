import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {decodePracticeApplicationSnapshot,assertPracticeMenuInputReady} from '../practice-ui.mjs';

/** Independent representative consumers, anchored in PracticePatches.inc and native UI ranges. */
export function th08PracticeParameterCases(base){
  const common={...base,time:100000,familiar:0};
  const row=(id,params,timeline,patches=[])=>({id,params:{...common,...params},timeline,patches});
  return[
    row('resources',{...base},{frame:0,patched:false}),
    row('resource-minimum',{life:0,bomb:0,power:0,score:0,graze:0,point_total:0,point_stage:0,time:0,value:0,night:0,rank:8},{frame:0,patched:false}),
    row('resource-maximum',{life:8,bomb:8,power:128,score:9999999990,graze:2147483647,point_total:9999,point_stage:9999,value:9999990,night:11,rank:99},{frame:0,patched:false}),
    row('stage-frame',{stage:1,warp:6,frame:731},{frame:731,patched:false}),
    row('stage-chapter',{stage:1,warp:1,section:10202},{frame:990,offset:0xa8d8,patched:true}),
    row('dialogue-disabled',{warp:3,section:3,dlg:false},{frame:4175,offset:0xac84,patched:true}),
    row('dialogue-enabled',{warp:3,section:3,dlg:true},{frame:4175,offset:0xac70,patched:true}),
    row('phase',{stage:6,warp:5,section:66,phase:2},{frame:4083,offset:0xc89c,patched:true},
      [{offset:0xac6c,type:'i32',value:3280},{offset:0xb244,type:'i32',value:260},{offset:0xb250,type:'i32',value:2920},{offset:0xb254,type:'i32',value:120}]),
    row('point-disabled',{stage:1,warp:4,section:12,sp1_pts:false},{frame:4870,offset:0xc020,patched:true}),
    row('point-minimum',{stage:1,warp:4,section:12,sp1_pts:true,bsX:-160,bsY:48,power:127},{frame:4870,offset:0xc020,patched:true},
      [{offset:0x1ed0,type:'f32',value:32},{offset:0x1ed4,type:'f32',value:48}]),
    row('point-maximum',{stage:1,warp:4,section:12,sp1_pts:true,bsX:160,bsY:128,power:127},{frame:4870,offset:0xc020,patched:true},
      [{offset:0x1ed0,type:'f32',value:352},{offset:0x1ed4,type:'f32',value:128}]),
  ];
}

export function assertTh08PracticeParameters(rows,{cases,decodeSnapshot,assessConsumption}){
  assert.deepEqual(rows.map(row=>row.parameterId),cases.map(row=>row.id),'Every parameter consumer case must execute');
  const epochs=new Set(),documents=new Set(),programs=new Map();
  for(const [index,row] of rows.entries()){
    const c=cases[index];assert.equal(row.status,'passed');assert.equal(row.enabled,true);assert.equal(row.locale,'ja-JP');
    const menuInput=assertPracticeMenuInputReady(row.nativePracticeMenu.menuInput,{decodeNative:decodeSnapshot});
    const expectedIdentity=row.parameterConsumption.identityBefore;
    assert.equal(menuInput.identity.app,expectedIdentity.app);assert.equal(menuInput.identity.documentTimeOrigin,expectedIdentity.documentTimeOrigin);
    assert.equal(menuInput.identity.url,expectedIdentity.url);
    assert.deepEqual(row.startup.configuration.options.thprac.params,c.params);
    const native=decodeSnapshot(row.parameterRaw),resources=assessConsumption(native,c.params,row.parameterConsumption);
    assert.equal(resources.status,'observed',JSON.stringify(resources.mismatches??resources.missing));
    const id=row.parameterConsumption.identityBefore;assert.ok(Number.isSafeInteger(id.epoch)&&!epochs.has(id.epoch));epochs.add(id.epoch);
    assert.ok(!documents.has(id.documentTimeOrigin));documents.add(id.documentTimeOrigin);
    const t=decodePracticeApplicationSnapshot(row.parameterConsumption.initialRaw),raw=row.programRaw;
    assert.equal(raw?.available,true);assert.equal(raw.header.length,4);assert.equal(raw.header[0],1);assert.equal(raw.header[1],1);
    assert.ok(raw.header[2]>0&&Array.isArray(raw.bytes)&&raw.bytes.length===raw.header[3]&&raw.bytes.length<=2*1024*1024);
    assert.ok(raw.bytes.every(value=>Number.isInteger(value)&&value>=0&&value<=255));
    const bytes=Buffer.from(raw.bytes);assert.equal(bytes.length,t.programBytes);assert.equal(bytes.readUInt32LE(0),0x800);
    assert.equal(t.timelineCount,bytes.readInt16LE(6));assert.equal(t.timelineFrame,c.timeline.frame);
    assert.equal(t.practiceInstructions,c.timeline.patched);assert.equal(t.instructionSelected,c.timeline.offset!==undefined);
    if(c.timeline.offset!==undefined){assert.equal(t.instructionOffset,c.timeline.offset);assert.equal(t.opcode,bytes.readUInt16LE(t.instructionOffset+4));}
    else{assert.equal(t.instructionOffset,null);assert.equal(t.opcode,null);const start=bytes.readUInt32LE(8);assert.ok(start>=72&&start+8<=bytes.length);}
    for(const patch of c.patches){assert.ok(patch.offset>=0&&patch.offset+4<=bytes.length);
      assert.equal(patch.type==='f32'?bytes.readFloatLE(patch.offset):bytes.readInt32LE(patch.offset),patch.value,`${c.id} actual ECL patch ${patch.offset.toString(16)}`);}
    programs.set(c.id,bytes);
  }
  const original=programs.get('point-disabled');
  for(const id of ['point-minimum','point-maximum']){
    const program=programs.get(id);assert.equal(program.length-original.length,520,'Point setup must append the actual instruction payload');
    assert.equal(program.readInt32LE(0x3e24),original.length-0x3e14,'Point setup jump must reach the appended payload');
    assert.equal(program.readInt32LE(program.length-4),0x3e30-(original.length+125*4),'Point setup payload must return to the original program');
  }
  return{complete:true,cases:cases.map(row=>row.id),programs:rows.map(row=>({id:row.parameterId,bytes:row.programRaw.bytes.length,
    sha256:createHash('sha256').update(Buffer.from(row.programRaw.bytes)).digest('hex')}))};
}
