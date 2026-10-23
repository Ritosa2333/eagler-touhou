import test from 'node:test';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {deflateSync} from 'node:zlib';
import {canvasPixelEvidence,assessFirstFrameEvidence} from './presentation-first-frame.mjs';
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
const crc32=bytes=>{let crc=0xffffffff;for(const byte of bytes){crc^=byte;for(let i=0;i<8;i++)crc=(crc>>>1)^((crc&1)?0xedb88320:0);}return (crc^0xffffffff)>>>0;};
function png(blank=false){
  const chunk=(name,data)=>{const type=Buffer.from(name),length=Buffer.alloc(4),crc=Buffer.alloc(4);length.writeUInt32BE(data.length);crc.writeUInt32BE(crc32(Buffer.concat([type,data])));return Buffer.concat([length,type,data,crc]);};
  const header=Buffer.alloc(13);header.writeUInt32BE(2);header.writeUInt32BE(2,4);header[8]=8;header[9]=2;
  const pixels=Buffer.alloc(14);if(!blank)pixels[4]=255;
  return Buffer.concat([Buffer.from('89504e470d0a1a0a','hex'),chunk('IHDR',header),chunk('IDAT',deflateSync(pixels)),chunk('IEND',Buffer.alloc(0))]);
}
const scope={game:'th08',variant:'normal'},expectedBuild={packageSha256:'a'.repeat(64),wasmSha256:'b'.repeat(64),manifestSha256:'c'.repeat(64)},expectedData={bytes:123,sha256:'d'.repeat(64)};
function fixture(){return {scope,expectedBuild,expectedData,runs:['cold','reload'].map((kind,index)=>{
  const epoch=index+1,bytes=png();return {schema:'eagler-presentation/first-frame-navigation/1',kind,scope,epoch,navigationId:kind,
    origin:'https://localhost:8443',outerUrl:'https://localhost:8443/host',runtimeUrl:`https://localhost:8443/runtime/th08?runtimeEpoch=${epoch}`,
    identity:{build:expectedBuild,data:expectedData},launchEpochMs:1000,canvasRect:{x:0,y:0,width:2,height:2},
    receipt:{protocol:'eagler-touhou/1',game:'th08',epoch,event:'first-frame',presentation:{schema:'eagler-runtime/first-frame/1',nativePresentCount:1,sentAtEpochMs:1020}},
    frames:[{timestampEpochMs:1010,metadata:{timestamp:1.01,deviceWidth:2,deviceHeight:2,offsetTop:0,pageScaleFactor:1},png:bytes.toString('base64'),sha256:hash(bytes)}]};
  })};}
test('actual PNG canvas pixels and compositor swap precede both protocol sends',()=>assert.equal(assessFirstFrameEvidence(fixture()).status,'passed'));
for(const [label,change] of [
  ['late swap',run=>{run.frames[0].timestampEpochMs=1030;run.frames[0].metadata.timestamp=1.03;}],
  ['blank canvas',run=>{const bytes=png(true);Object.assign(run.frames[0],{png:bytes.toString('base64'),sha256:hash(bytes)});}],
  ['counter without pixels',run=>{run.frames=[];}],
  ['foreign epoch',run=>{run.receipt.epoch=100;}],
  ['foreign build',run=>{run.identity={...run.identity,build:{...expectedBuild,wasmSha256:'e'.repeat(64)}};}],
  ['stale navigation',run=>{run.launchEpochMs=1011;}],
  ['tampered PNG',run=>{run.frames[0].png=png(true).toString('base64');}],
  ['receive clock replacing send',run=>{delete run.receipt.presentation.sentAtEpochMs;run.receipt.receivedAtEpochMs=1020;}],
  ['different viewport scale',run=>{run.frames[0].metadata.pageScaleFactor=2;}]
])test(`reject ${label}`,()=>{const input=fixture();change(input.runs[0]);assert.equal(assessFirstFrameEvidence(input).status,'failed');});
test('cold capture cannot stand for reload',()=>{const input=fixture();input.runs[1]=structuredClone(input.runs[0]);assert.equal(assessFirstFrameEvidence(input).status,'failed');});
test('missing cold/reload input stays incomplete',()=>assert.equal(assessFirstFrameEvidence({}).status,'incomplete'));
test('PNG truncation and checksum corruption are rejected',()=>{
  const bytes=png();assert.throws(()=>canvasPixelEvidence(bytes.subarray(0,30),{x:0,y:0,width:2,height:2}));
  bytes[40]^=1;assert.throws(()=>canvasPixelEvidence(bytes,{x:0,y:0,width:2,height:2}),/checksum/);
});
