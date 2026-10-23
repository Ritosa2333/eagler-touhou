import test from 'node:test';
import assert from 'node:assert/strict';
import {installFramebufferProof,comparePlayfield} from './framebuffer-proof.mjs';

function withFramebuffer(body){
  const previous=globalThis.document,source={bytes:new Uint8Array(640*480*4)};
  let read=source,draw=null,alignment=4,readbacks=0;
  const gl={READ_FRAMEBUFFER_BINDING:1,DRAW_FRAMEBUFFER_BINDING:2,PIXEL_PACK_BUFFER_BINDING:3,
    PACK_ALIGNMENT:4,READ_FRAMEBUFFER:5,DRAW_FRAMEBUFFER:6,FRAMEBUFFER:7,COLOR_BUFFER_BIT:8,
    RGBA:9,UNSIGNED_BYTE:10,NO_ERROR:0,
    getParameter(parameter){return parameter===1?read:parameter===2?draw:parameter===4?alignment:null;},
    bindFramebuffer(target,value){if(target===5||target===7)read=value;if(target===6||target===7)draw=value;},
    blitFramebuffer(){},pixelStorei(parameter,value){if(parameter===4)alignment=value;},
    readPixels(x,y,width,height,format,type,bytes){++readbacks;bytes.set(read.bytes);},getError(){return 0;}};
  const runtime={document:{querySelector:()=>({getContext:()=>gl})}};
  globalThis.document={querySelector:()=>({contentWindow:runtime})};
  try{
    installFramebufferProof();
    const present=value=>{source.bytes.fill(value);gl.blitFramebuffer(0,0,640,480,0,0,640,480,gl.COLOR_BUFFER_BIT,0);};
    body({proof:runtime.__framebufferProof,present,source,readbacks:()=>readbacks});
  }finally{globalThis.document=previous;}
}

test('confirmed-frame pixels remain owned after the renderer reuses its framebuffer',()=>withFramebuffer(({proof,present,readbacks})=>{
  present(17);const captured=proof.capture('2:928');
  assert.deepEqual(captured,{key:'2:928',presentation:1});
  assert.equal(proof.capture('2:929'),null,'an old presentation cannot label a new native frame');
  present(91);
  assert.deepEqual(proof.capture('2:928'),captured,'the first exact sample remains authoritative');
  const stored=proof.readSample('2:928');
  assert.equal(stored.key,'2:928');assert.equal(stored.presentation,1);
  assert(Buffer.from(stored.base64,'base64').every(value=>value===17));
  assert.equal(readbacks(),1,'transfer and duplicate lookups do not perform another GPU readback');
  assert(Buffer.from(proof.read().base64,'base64').every(value=>value===91));
  const stopped=proof.stopSampling();assert.equal(stopped.sampleReadbacks,1);
  assert.deepEqual(proof.sampleKeys(),[]);assert.equal(proof.readSample('2:928'),null);
  present(42);assert.equal(proof.capture('2:930'),null,'sampling stays off during pause/resume checks');
}));

test('post-measurement sampling has fixed retention and readback budgets',()=>withFramebuffer(({proof,present,readbacks})=>{
  for(let frame=0;frame<80;++frame){present(frame);proof.capture('0:'+frame);}
  assert.equal(readbacks(),64);assert.equal(proof.sampleKeys().length,16);
  assert.equal(proof.readSample('0:47'),null);assert.equal(proof.readSample('0:48').presentation,49);
  assert.equal(proof.readSample('0:64'),null);
  assert.deepEqual(proof.stopSampling(),{sampleReadbacks:64,retainedSamples:16,sampleLimit:16,readbackLimit:64});
}));

test('the entire native playfield retains zero-tolerance RGBA comparison',()=>{
  const live=Buffer.alloc(640*480*4),replay=Buffer.alloc(live.length);
  for(let y=0;y<480;++y)for(let x=0;x<640;++x){const at=(y*640+x)*4;live[at]=x%256;live[at+1]=y%256;live[at+3]=255;}
  live.copy(replay);assert.equal(comparePlayfield(live,replay).equal,true);
  replay[0]^=1;assert.equal(comparePlayfield(live,replay).equal,true,'outside-frame FPS pixels stay outside the declared viewport');
  replay[(463*640+415)*4+3]^=1;
  const result=comparePlayfield(live,replay);
  assert.equal(result.equal,false);assert.equal(result.mismatches,1);assert.equal(result.tolerance,0);
  assert.deepEqual(result.first,[{x:415,y:463,c:3,live:255,replay:254}]);
});
