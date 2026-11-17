import test from 'node:test';import assert from 'node:assert/strict';import {readFileSync,readdirSync} from 'node:fs';import {resolve} from 'node:path';
import {core,oracle,memory,string,root,report} from './helpers.mjs';
test('TH17 seven stage configurations and every dialogue instruction and text byte match original data and decoder',async()=>{
 const c=await core(),m=await oracle(),p=c.message_create();let instructions=0,texts=0,entries=0,checks=0;
 try{
  for(let stage=1;stage<=7;stage++){
   const table=0x4b23e8+stage*0xe8;
   for(let field=0;field<14;field++){const name=string(c,c.stage_config_name(stage,field));assert.equal(name,m.string(m.u32(table+4+field*4)),`${stage} name ${field}`);if(field!==2&&field!==3)assert.ok(readFileSync(resolve(root,'reference/assets',name)).length);checks++;}
   for(const [field,offset]of [[0,0],[1,0x3c],[2,0x40],[3,0x44]]){assert.equal(c.stage_config_value(stage,field),m.i32(table+offset));checks++;}
   assert.deepEqual(Buffer.from(memory(c,c.stage_config_visuals(stage),160)),Buffer.from(m.bytes(table+0x48,160)));checks++;
  }
  assert.equal(c.stage_config_visuals(0),0);assert.equal(c.stage_config_visuals(8),0);
  const files=readdirSync(resolve(root,'reference/assets')).filter(n=>/^st0[1-7][a-i]\.msg$/.test(n));assert.equal(files.length,63);
  for(const name of files){const b=readFileSync(resolve(root,'reference/assets',name)),input=c.allocate(b.length);memory(c,input,b.length).set(b);assert.equal(c.message_open(p,input,b.length),1,name+' '+string(c,c.message_error(p)));const count=b.readUInt32LE();assert.equal(c.message_count(p),count);entries+=count;
   for(let entry=0;entry<count;entry++){const start=b.readUInt32LE(4+entry*8),end=entry+1<count?b.readUInt32LE(12+entry*8):b.length;assert.equal(c.message_entry(p,entry,0),start);assert.equal(c.message_entry(p,entry,1),b.readUInt32LE(8+entry*8));let at=start,index=0;
    while(at<end){const v=c.message_instruction(p,entry,index++),time=b.readUInt16LE(at),opcode=b[at+2],size=b[at+3];assert.ok(v);assert.deepEqual([0,1,2,3].map(k=>c.message_instruction_field(v,k)),[at,time,opcode,size]);assert.deepEqual(Buffer.from(memory(c,c.message_instruction_data(v),size)),b.subarray(at+4,at+4+size));checks+=3;instructions++;
     if(opcode>=15&&opcode<=17){const native=m.allocate(size+4);m.write(native,b.subarray(at+4,at+4+size));const output=m.call(0x42f840,{ecx:native}),n=c.message_instruction_field(v,4);assert.equal(m.bytes(output+n,1)[0],0);assert.deepEqual(Buffer.from(memory(c,c.message_instruction_text(v),n)),Buffer.from(m.bytes(output,n)),name+' text '+at);texts++;checks+=2;}
     at+=4+size;
    }assert.equal(c.message_entry(p,entry,2),index);checks++;
   }
   // Truncated entry tables and arguments must fail before any interpreter runs.
   for(const length of [3,7,b.length-1]){assert.equal(c.message_open(p,input,length),0,name+' truncated '+length);checks++;}
   c.deallocate(input);
  }
  report('stage-messages',{passed:true,stages:7,files:files.length,entries,instructions,texts,checks,scope:'All seven stage data rows, original resource names and four visual sets; all 63 MSG entry tables and instruction payloads; each text byte compared with the unchanged native Shift-JIS decoder including ASCII underscore handling. Dialogue execution and text rasterization remain separate.'});
 }finally{c.message_delete(p);m.close();}
});
