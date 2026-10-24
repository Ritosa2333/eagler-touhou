import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {runInNewContext} from 'node:vm';
import {createSenderDelayQueue} from './performance-sender-delay.mjs';

// Exercise the actual serialized init-script function, including the same
// prototype hooks used by Chromium, without loading or mutating a game.
const source=readFileSync(new URL('./performance-browser.mjs',import.meta.url),'utf8');
const probe=source.slice(source.indexOf('function installProbe('),source.indexOf('\nfunction host('));
function fixture(){
  const clock={at:0},pending=[],heap=new Uint8Array(1024),calls=[];
  const context={performance:{timeOrigin:10000,now:()=>clock.at},setTimeout,clearTimeout,TextDecoder,
    requestAnimationFrame:callback=>(pending.push(callback),pending.length),advance:ms=>clock.at+=ms,
    document:{querySelector:()=>({textContent:''}),addEventListener(){}},__eaglerPeerTransport:{route:'relay'},
    Module:{HEAPU8:heap,HEAPU32:new Uint32Array(heap.buffer),_th15_mp_status:()=>4,
      _th15_mp_rollback_status:()=>128,_th15_phase:()=>1,onGameFrame:()=>{}},
    WebSocket:class {send(){}},WebGL2RenderingContext:class {
      constructor(){this.canvas={id:'canvas'};}bindFramebuffer(...args){calls.push(['bind',...args]);}
      blitFramebuffer(...args){calls.push(['blit',...args]);}
    },
  };
  runInNewContext('('+probe+')({seat:0,duration:1},('+createSenderDelayQueue.toString()+'));'+
    'globalThis.gl=new WebGL2RenderingContext();globalThis.notify=true;globalThis.draw=false;'+
    'globalThis.nativeTick=function tick(time){advance(5);if(draw){gl.bindFramebuffer(36009,null);gl.blitFramebuffer(0,0,640,480,0,0,640,480,16384,9728);}'+
    'if(notify)Module.onGameFrame(1,4,1);requestAnimationFrame(tick);};'+
    '__th15Performance.attach();requestAnimationFrame(nativeTick);',context);
  const step=at=>{clock.at=at;const callback=pending.shift();assert.ok(callback);callback(at);};
  return {context,clock,pending,calls,step};
}

test('stable native rAF is learned before the measured correction-only callback',()=>{
  const f=fixture();f.step(10);f.context.__th15Performance.start();f.context.notify=false;f.step(30);
  const data=f.context.__th15Performance.result();assert.equal(data.nativeTokens.length,1);assert.equal(data.rows.length,1);
  assert.equal(data.rows[0].native,true);assert.equal(data.rows[0].nativeMs,null);assert.equal(data.rows[0].exit-data.rows[0].entry,5);
});

test('only color blits into the game canvas default draw framebuffer are submissions',()=>{
  const f=fixture();f.step(10);f.context.__th15Performance.start();f.context.draw=true;f.step(30);
  const {gl}=f.context;gl.bindFramebuffer(36009,{});gl.blitFramebuffer(0,0,1,1,0,0,1,1,16384,9728);
  gl.bindFramebuffer(36160,null);gl.blitFramebuffer(0,0,1,1,0,0,1,1,256,9728);
  gl.canvas={id:'another-canvas'};gl.blitFramebuffer(0,0,1,1,0,0,1,1,16384,9728);
  const data=f.context.__th15Performance.result();assert.equal(data.presentations.length,1);assert.equal(data.presentationsOutsideNativeCallback,0);
});

test('post-callback diagnostic getter cost is outside native callback wall',()=>{
  const f=fixture();f.step(10);f.context.__th15Performance.start();
  f.context.Module._th15_mp_rollback_status=()=>{f.clock.at+=20;return 128;};f.step(30);
  const row=f.context.__th15Performance.result().rows[0];assert.equal(row.exit-row.entry,5);assert.equal(row.observerMs,20);
});
