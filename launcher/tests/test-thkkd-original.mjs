import {test} from 'vitest';
import assert from 'node:assert/strict';
import UPNG from 'upng-js';
import {selectThkkdArchives} from '../src/launcher/thkkd-original.mjs';
import {unpackDxLz,readDxArchive} from '../src/launcher/thkkd-dxa.mjs';
import {decodeThkkdBmp,convertThkkdImage} from '../src/launcher/thkkd-image.mjs';

const inputs = paths => paths.map(archivePath=>Object.assign(new Blob(['fixture']),{archivePath}));
test('同时导入两份独立 DXA 文件，支持交换选择顺序',()=>{
  const files=['bgm.dxa','dat.dxa'].map(name=>Object.assign(new Blob(['fixture']),{name}));
  assert.deepEqual([...selectThkkdArchives(files).keys()],['bgm.dxa','dat.dxa']);
});
test('原版 ZIP 可以使用根目录或同一个单层游戏目录',()=>{
  for(const paths of [['dat.dxa','bgm.dxa'],['海惠堂/DAT.DXA','海惠堂/BGM.DXA']])
    assert.equal(selectThkkdArchives(inputs([...paths,'readme.txt'])).size,2);
});
test('两份原版档案必须完整、唯一，且目录层级一致',()=>{
  for(const paths of [['dat.dxa'],['dat.dxa','game/bgm.dxa'],['a/b/dat.dxa','a/b/bgm.dxa'],['../dat.dxa','../bgm.dxa'],['dat.dxa','DAT.DXA','bgm.dxa']])
    assert.throws(()=>selectThkkdArchives(inputs(paths)));
});
function packed(payload,size){
  const bytes=new Uint8Array(9+payload.length),view=new DataView(bytes.buffer);
  view.setUint32(0,size,true);view.setUint32(4,bytes.length,true);bytes[8]=255;bytes.set(payload,9);return bytes;
}
test('DXA 解压支持重叠回溯和控制字符转义',()=>{
  assert.deepEqual([...unpackDxLz(packed([65,255,0,0],5),5)],[65,65,65,65,65]);
  assert.deepEqual([...unpackDxLz(packed([255,255],1),1)],[255]);
});
test('DXA 解压检查损坏长度、回溯距离和索引',async()=>{
  assert.throws(()=>unpackDxLz(packed([255,0,0],4),4),/回溯/);
  assert.throws(()=>unpackDxLz(packed([65],2),2),/长度/);
  assert.throws(()=>unpackDxLz(packed([65],1),3),/长度/);
  await assert.rejects(readDxArchive(new Blob([new Uint8Array(28)])),/DXA/);
});
test('BMP 按 BGR 顺序和自底向上的行序解码，保留不透明 alpha',()=>{
  const bytes=new Uint8Array(62),v=new DataView(bytes.buffer);
  v.setUint16(0,0x4d42,true);v.setUint32(10,54,true);v.setUint32(14,40,true);
  v.setInt32(18,1,true);v.setInt32(22,2,true);v.setUint16(28,24,true);
  bytes.set([30,20,10,0,60,50,40,0],54);
  assert.deepEqual([...decodeThkkdBmp(bytes).rgba],[40,50,60,255,10,20,30,255]);
});
test('PNG 的独立遮罩红通道与原生 alpha 相乘，RGB 保持原字节',async()=>{
  const png=rgba=>new Uint8Array(UPNG.encode([new Uint8Array(rgba).buffer],1,1,0,undefined,true));
  const result=await convertThkkdImage(png([70,90,110,128]),'sample.png',png([128,0,255,255]));
  const decoded=UPNG.decode(await result.blob.arrayBuffer());
  assert.deepEqual([...new Uint8Array(UPNG.toRGBA8(decoded)[0])],[70,90,110,64]);
  assert.equal(result.intrinsicAlpha,true);
});
test('图片内容决定解码器，兼容原版资源扩展名与内容不一致的情况',async()=>{
  const bytes=new Uint8Array(UPNG.encode([new Uint8Array([70,90,110,255]).buffer],1,1,0,undefined,true));
  const result=await convertThkkdImage(bytes,'image.jpg',null);
  assert.deepEqual([...new Uint8Array(UPNG.toRGBA8(UPNG.decode(await result.blob.arrayBuffer()))[0])],[70,90,110,255]);
});
