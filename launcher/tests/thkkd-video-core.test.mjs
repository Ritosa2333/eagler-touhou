import {test} from 'vitest';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {gzipSync} from 'node:zlib';
import {isOnDemandFrontendAsset} from '../lib/frontend-static-manifest.mjs';

const directory=new URL('../third_party/thkkd-video/',import.meta.url);
const source=JSON.parse(readFileSync(new URL('SOURCE.json',directory),'utf8'));
test('海恵堂视频内核满足原始和传输体积预算，并与登记身份一致',()=>{
  for(const [name,hash]of Object.entries(source.recipe))assert.equal(createHash('sha256').update(readFileSync(new URL(name,directory))).digest('hex'),hash);
  for(const [name,record]of Object.entries(source.files)){
    const bytes=readFileSync(new URL(name,directory));
    assert.equal(bytes.length,record.bytes);assert.equal(createHash('sha256').update(bytes).digest('hex'),record.sha256);
    assert.equal(gzipSync(bytes,{level:9}).length,record.gzipBytes);
    if(name.endsWith('.wasm')){assert.ok(bytes.length<=8*1024*1024);assert.ok(record.gzipBytes<=3*1024*1024);assert.equal(WebAssembly.validate(bytes),true);}
  }
});
test('视频内核的实际编译组件限定为 WMV3 到 VP8 所需的依赖闭包',()=>{
  assert.deepEqual(source.components,{
    decoder:['H263_DECODER','VC1_DECODER','WMV3_DECODER'],encoder:['LIBVPX_VP8_ENCODER'],demuxer:['ASF_DEMUXER'],muxer:['WEBM_MUXER'],
    parser:['H263_PARSER','VC1_PARSER'],filter:['FORMAT_FILTER','NULL_FILTER','SCALE_FILTER'],protocol:['FILE_PROTOCOL'],
  });
  assert.deepEqual(source.libvpx,{vp8Encoder:true,vp8Decoder:false,vp9Encoder:false,vp9Decoder:false,multithread:false});
  assert.equal(source.singleThread,true);assert.equal(source.license,'LGPL-2.1-or-later');
});
test('专用内核继续按需获取，前端依赖清单使用轻量 Worker 包装层',()=>{
  assert.equal(isOnDemandFrontendAsset('assets/ffmpeg-core-test123.wasm'),true);
  const manifest=JSON.parse(readFileSync(new URL('../package.json',import.meta.url),'utf8'));
  assert.equal(manifest.dependencies['@ffmpeg/core'],undefined);assert.equal(manifest.dependencies['@ffmpeg/ffmpeg'],'0.12.15');
});
