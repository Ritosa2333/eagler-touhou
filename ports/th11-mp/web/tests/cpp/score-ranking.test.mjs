import test from 'node:test';import assert from 'node:assert/strict';import{core,oracle,memory,report}from'./helpers.mjs';
test('TH11 high-score insertion preserves original ranking and row bytes',async()=>{
 const c=await core(),m=await oracle(),s=c.score_file_create(),native=m.allocate(0x68d4),clock=m.allocate(0x40);let timestamp=0,checks=0;m.u32(0x4a8d80,clock);
 m.replace(0x4608b7,'deterministic score timestamp',()=>{const p=m.u32(m.reg('ESP')+4);m.u32(p,timestamp);m.u32(p+4,0);m.reg('EDX',0);return timestamp;});
 try{for(let sample=0;sample<300;++sample){const selection=sample%7,difficulty=sample%5,stage=sample%9,continues=sample%256,score=sample%13===0?-1:((sample%13)*100000),row=Buffer.alloc(0x68d4);timestamp=1600000000+sample*1000;
  for(let i=0;i<row.length;++i)row[i]=(i*97+sample*13)&255;for(let d=0;d<5;++d)for(let r=0;r<10;++r)row.writeInt32LE(1000000-r*100000,0x10+d*0x118+r*28);
  memory(c,c.score_file_data(s,selection),row.length).set(row);m.write(native,row);
  m.i32(0x4a5720,difficulty);m.i32(0x4a56e4,score);m.i32(0x4a5728,stage);m.i32(0x4a573c,continues);
  const elapsed=600+sample,drawn=sample%7?elapsed:Math.floor(elapsed*.97),time=Buffer.alloc(16);time.writeDoubleLE(drawn);time.writeDoubleLE(elapsed,8);m.write(clock+0x24,time);
  const slow=Math.fround(100-Math.fround(drawn/elapsed)*100),expected=m.call(0x42ba80,{args:[native]})|0;
  assert.equal(c.score_insert(s,selection,difficulty,score,stage,continues,timestamp,slow),expected,'rank');assert.deepEqual(Buffer.from(memory(c,c.score_file_data(s,selection),row.length)),Buffer.from(m.bytes(native,row.length)),'row bytes sample'+sample);++checks;
 }report('score-ranking',{passed:true,checks,scope:'Native 42ba80 rank ties, full-table shifting, score/stage/continue/timestamp/slowdown fields and preservation of unrelated record bytes. Name entry and full result-screen integration are separate.'});
 }finally{c.score_file_delete(s);m.close();}
});
