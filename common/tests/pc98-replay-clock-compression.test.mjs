import {test} from 'node:test';
import assert from 'node:assert/strict';
import {gzipSync} from 'node:zlib';
import {createHash} from 'node:crypto';

for(const game of ['02','03','04'])test(`TH${game} preserves a long replay's exact clocks`,async()=>{
  const {encodeExternalReplay,decodeExternalReplay}=await import(`../../ports/th${game}/web/replay/external-codec.js`);
  const count=360000,clocks=new Uint8Array(count*9),view=new DataView(clocks.buffer);
  let performance=0,epoch=1700000000000;
  for(let i=0;i<count;i++){
    const kind=i%3===0?2:1;clocks[i*9]=kind;
    if(kind===1)performance+=(i%113===0?0:16.6666667);else epoch+=i%7===0?17:0;
    view.setFloat64(i*9+1,kind===1?performance:epoch,true);
  }
  const header={game,language:'jp',runtime:'main',identity:'a'.repeat(64),boundaryMode:'sleep',endBoundary:count,
    result:{boundary:count,clockCount:count,extraClockReads:0,endReason:'sleep',memoryHash:'12345678',memorySha256:'b'.repeat(64),memoryBytes:1024}};
  const file=await encodeExternalReplay(header,{clockChunks:[clocks],events:[],disk:new Uint8Array(4096)});
  const replay=await decodeExternalReplay(file,game);
  assert.equal(replay.header.clockEncoding,'delta-varint/3');
  assert.deepEqual(replay.clocks,clocks);
  assert(file.size<clocks.length/10,`compressed size ${file.size}`);

  // The preceding format groups XOR varints by clock kind.
  const mask=new Uint8Array(Math.ceil(count/8)),streams=[[],[],[]],previous=[0n,0n,0n];
  for(let i=0;i<count;i++){
    const kind=clocks[i*9];if(kind===2)mask[i>>3]|=1<<(i&7);
    const value=view.getBigUint64(i*9+1,true);let delta=value^previous[kind];previous[kind]=value;
    while(delta>=128n){streams[kind].push(Number(delta&127n)|128);delta>>=7n;}streams[kind].push(Number(delta));
  }
  const packed=Buffer.concat([mask,Buffer.from(streams[1]),Buffer.from(streams[2])]),clock=gzipSync(packed);
  const bytes=new Uint8Array(await file.arrayBuffer()),headerLength=new DataView(bytes.buffer).getUint32(8,true);
  const oldHeader=JSON.parse(new TextDecoder().decode(bytes.subarray(12,12+headerLength)));
  const tail=bytes.subarray(12+headerLength+oldHeader.clockPayloadBytes);
  const payload=Buffer.concat([clock,tail]);
  Object.assign(oldHeader,{clockEncoding:'xor-varint/2',clockPackedBytes:packed.length,clockPayloadBytes:clock.length,payloadSha256:createHash('sha256').update(payload).digest('hex')});
  const json=Buffer.from(JSON.stringify(oldHeader)),prefix=Buffer.alloc(12);prefix.write('P98EXT02');prefix.writeUInt32LE(json.length,8);
  const legacy=await decodeExternalReplay(new Blob([prefix,json,payload]),game);
  assert.deepEqual(legacy.clocks,clocks);
});
