import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtemp,writeFile,rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {resolve} from 'node:path';
import {deflateSync} from 'node:zlib';
import {Th075BaseResources,Th075ThcrapCompiler,cryptTh075,th075QuotedRecords,encodeTh075Pixels,th075ImageVariant} from '../server/th075-thcrap-compiler.mjs';
import {chineseScriptQuotes,chineseStaticResources} from '../server/th075-chinese-static.mjs';
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

test('TH075 replay image variant binds the same pixels to each original edition',()=>{
  const bank=Buffer.alloc(22);bank.writeUInt32LE(1,1);bank.writeUInt32LE(1,5);bank.writeUInt32LE(1,9);bank[13]=32;
  const patch=Buffer.alloc(44);patch.write('E75I');patch.writeUInt32LE(2,4);patch.writeUInt32LE(1,16);patch.writeUInt32LE(1,24);patch.writeUInt32LE(1,28);patch.writeUInt32LE(4,36);
  const variant=th075ImageVariant(patch,bank);assert.equal(variant.readUInt32LE(8),crc32(bank));assert.equal(variant.readUInt32LE(12),bank.length);assert.deepEqual(variant.subarray(20),patch.subarray(20));
  const wrong=Buffer.from(bank);wrong.writeUInt32LE(2,1);assert.throws(()=>th075ImageVariant(patch,wrong),/TH075 bitmap|geometry mismatch/);
  assert.throws(()=>th075ImageVariant(patch.subarray(0,43),bank),/geometry mismatch/);
});

test('TH075 Chinese Story replaces only aligned dialogue, retaining thpatch endings',async()=>{
  const original=cryptTh075(Buffer.from('B0\nP(XX)"Japanese"\nW010\nN()"\\cd0c0c0ending"\n'));
  const translated=cryptTh075(Buffer.from('B0\nP(XX)"\x01hello\x02"\nW010\nN()"ignored ending"\n','latin1'));
  assert.deepEqual(chineseScriptQuotes(original,translated),{'0':'hello'});
  assert.throws(()=>chineseScriptQuotes(original,cryptTh075(Buffer.from('B1\nP(XX)"\x01hello\x02"\nW010\nN()"ending"\n','latin1'))),/position mismatch/);
  const result=await chineseStaticResources([{path:'data/character/reimu/reimu.sce',script:translated,sha256:'test'}],{read:async()=>original},async tables=>tables);
  assert.equal(result.provenance.quotes,1);assert.equal(result.resources[0].path,'th075/character/reimu/reimu.sce.quotes.json');assert.deepEqual(JSON.parse(result.resources[0].bytes),{'0':'hello'});
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
    assert.equal((await source.read('data/system/select.dat','base')).toString(),'base:data/system/select.dat');
    await assert.rejects(source.read('data/system/select.dat','unknown'),/Invalid TH075 resource edition/);
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

test('TH075 menu descriptions use the same validated glyph bank as language text', {skip:!process.env.TH075_TEST_FONT},async()=>{
  for(const language of ['lang_zh-hans','lang_en']){
    const compiler=new Th075ThcrapCompiler({fontFile:process.env.TH075_TEST_FONT});
    compiler.base={read:async()=>cryptTh075(Buffer.from('B0\nN()"\\cd0c0c0ending"\n'))};
    const resources=await compiler.processPack([{game:'th075',path:'th075/character/reimu/scenario.txt.jdiff',bytes:Buffer.from(JSON.stringify({'1':{lines:['localized']}}))}],{language});
    const options=JSON.parse(resources.find(r=>r.path==='localization/options.json').bytes);
    assert.equal(options.optionDescriptions.length,7);
    for(const bytes of options.optionDescriptions){assert.equal(bytes[0],1);assert.equal(bytes.at(-1),2);assert.ok(bytes.every(n=>Number.isInteger(n)&&n>=0&&n<=255));}
    assert.ok(Buffer.from(options.optionDescriptions[0]).includes(Buffer.from('\\n')));
  }
});
