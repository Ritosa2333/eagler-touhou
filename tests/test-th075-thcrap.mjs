import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtemp,writeFile,rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {resolve} from 'node:path';
import {deflateSync} from 'node:zlib';
import {Th075BaseResources,Th075ThcrapCompiler,cryptTh075,th075QuotedRecords,encodeTh075Pixels} from '../server/th075-thcrap-compiler.mjs';
import {crc32} from '../integrations/thcrap.mjs';

test('TH075 pixel runs preserve BGRA words and avoid expanding noisy images',()=>{
  const solid=Buffer.alloc(80);for(let i=0;i<80;i+=4)solid.writeUInt32LE(0x12345678,i);
  const packed=encodeTh075Pixels(solid);assert.equal(packed.packed,8);assert.equal(packed.bytes.readUInt32LE(0),20);assert.equal(packed.bytes.readUInt32LE(4),0x12345678);
  const noisy=Buffer.from([1,2,3,4,5,6,7,8]);assert.deepEqual(encodeTh075Pixels(noisy),{packed:0,bytes:noisy});
});

test('TH075 quote identities exclude comments and preserve numeric controls',()=>{
  const original=Buffer.from('# "comment"\r\nB100\r\nW050\r\nV059\r\nB0\r\nP(XX)"story"\r\nF001\r\nN()"\\cd0c0c0ending"\r\n');
  const encrypted=cryptTh075(original);
  assert.deepEqual(cryptTh075(encrypted),original);
  assert.deepEqual(th075QuotedRecords(encrypted).map(r=>[r.quote,r.label,r.record,r.command,r.text.toString()]),[[0,0,0,'P','story'],[1,0,2,'N','\\cd0c0c0ending']]);
});

test('TH075 original-edition resolver follows native sprite and portrait ownership',async()=>{
  const temp=await mkdtemp(resolve(tmpdir(),'th075-base-test-'));
  try{
    const files=[],parts=[];let offset=0;
    for(const edition of ['base','patched'])for(const path of ['data/character/00.dat','data/character/03.dat','data/character/042.dat','data/system/title.dat','data/system/select.dat']){
      const value=Buffer.from(edition+':'+path),head=Buffer.alloc(4);head.writeUInt32LE(value.length);const packed=Buffer.concat([head,deflateSync(value)]);files.push({path:'/resources/'+edition+'/'+path+'.z',offset,bytes:packed.length});parts.push(packed);offset+=packed.length;
    }
    const table=Buffer.from(JSON.stringify({files})),header=Buffer.alloc(12);header.write('E75DATA1');header.writeUInt32LE(table.length,8);const path=resolve(temp,'th075.data');await writeFile(path,Buffer.concat([header,table,...parts]));
    const source=new Th075BaseResources([path]);
    for(const [path,edition]of [['data/character/00.dat','base'],['data/character/03.dat','patched'],['data/character/042.dat','patched'],['data/system/title.dat','base'],['data/system/select.dat','patched']])assert.equal((await source.read(path)).toString(),edition+':'+path);
  }finally{await rm(temp,{recursive:true,force:true});}
});

test('TH075 compiler emits replacements only and binds text to the original CRC', {skip:!process.env.TH075_TEST_FONT},async()=>{
  const script=cryptTh075(Buffer.from('B0\r\nP(XX)"keep story"\r\nM1\r\nN()"\\cd0c0c0old ending"\r\n'));
  const compiler=new Th075ThcrapCompiler({fontFile:process.env.TH075_TEST_FONT});
  compiler.base={read:async path=>{assert.equal(path,'data/character/reimu/reimu.sce');return script;}};
  const result=await compiler.processPack([{game:'th075',path:'th075/character/reimu/scenario.txt.jdiff',bytes:Buffer.from(JSON.stringify({'1':{lines:['中文，"localized"','second line']}}))}]);
  const patch=result.find(r=>r.targetPath.endsWith('.sce.bin')).bytes;
  assert.equal(patch.subarray(0,4).toString(),'E75T');assert.equal(patch.readUInt32LE(8),crc32(script));assert.equal(patch.readUInt32LE(12),script.length);assert.equal(patch.readUInt32LE(16),1);assert.equal(patch.readUInt32LE(20),1);
  const text=patch.subarray(28);assert.equal(text.subarray(0,8).toString(),'\\cd0c0c0');assert.equal(text[8],1);assert.equal(text.at(-1),2);assert.ok(text.includes(Buffer.from('\\n')));
  assert.ok(!patch.includes(Buffer.from('keep story')),'Language pack must not contain the unchanged Japanese script');
  assert.equal(result.find(r=>r.targetPath.endsWith('glyphs.bin')).bytes.subarray(0,4).toString(),'GG75');
});
