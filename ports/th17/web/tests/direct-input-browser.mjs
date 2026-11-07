import assert from 'node:assert/strict';

export async function checkDirectInput(frame,epoch=7){
 const result=await frame.evaluate(async epoch=>{
  const bridge=window.__eaglerDirectInputBridge,canvas=Module.canvas;
  const original={touch:Module._th17_touch,cancel:Module._th17_touch_cancel,
   controls:Module._th17_touch_controls,stick:Module._th17_touch_stick,rect:canvas.getBoundingClientRect};
  const calls=[],controls=[];let reads=0;
  const message={protocol:'eagler-touhou/1',game:'th17',epoch,command:'direct-touch',id:91,x:.5,y:.5};
  try{
   Module._th17_touch=(...args)=>calls.push(args);
   Module._th17_touch_cancel=()=>calls.push(['cancel']);
   Module._th17_touch_controls=(...args)=>controls.push(args);
   Module._th17_touch_stick=()=>{};
   canvas.getBoundingClientRect=()=>{++reads;return {left:11,top:13,width:500,height:400};};
   window.dispatchEvent(new Event('resize'));
   const before=Module._th17_frame();
   const rejected=[{...message,epoch:epoch-1},{...message,request:'rpc'},{...message,game:'th15'}]
    .map(value=>bridge.submit({...value,type:'down'}));
   bridge.submit({...message,type:'down'});
   for(let n=0;n<120;++n)bridge.submit({...message,type:'move',x:.5+n/1000});
   bridge.submit({...message,type:'up'});
   const burstReads=reads;
   bridge.submit({protocol:message.protocol,game:message.game,epoch,command:'touch-controls',bombSerial:42});
   bridge.submit({...message,command:'touch-cancel'});
   // Native canvas delivery shares geometry with the hosted carrier.
   const pointer=type=>new PointerEvent(type,{pointerId:92,pointerType:'touch',clientX:261,clientY:213,bubbles:true,cancelable:true});
   canvas.dispatchEvent(pointer('pointerdown'));
   window.dispatchEvent(pointer('pointermove'));
   window.dispatchEvent(pointer('pointerup'));
   const nativeReads=reads;
   window.dispatchEvent(new Event('scroll'));
   bridge.submit({...message,type:'move'});
   const finalReads=reads,frameAdvanced=Module._th17_frame()!==before;
   await new Promise(done=>requestAnimationFrame(()=>requestAnimationFrame(done)));
   const acrossFrames=reads;
   bridge.submit({...message,type:'move'});
   return {schema:bridge.schema,rejected,calls,controls,burstReads,nativeReads,
    finalReads,acrossFrameReads:reads-acrossFrames,frameAdvanced,first:[(.5*innerWidth-11)/500,(.5*innerHeight-13)/400]};
  }finally{
   Module._th17_touch=original.touch;Module._th17_touch_cancel=original.cancel;
   Module._th17_touch_controls=original.controls;Module._th17_touch_stick=original.stick;
   canvas.getBoundingClientRect=original.rect;
   window.dispatchEvent(new Event('resize'));
  }
 },epoch);
 assert.equal(result.schema,'eagler-touhou/direct-input/1');
 assert.deepEqual(result.rejected,[false,false,false]);
 assert.equal(result.frameAdvanced,false,'input delivery never advances the simulation');
 assert.equal(result.burstReads,1,'a motion burst measures the canvas once');
 assert.equal(result.finalReads,result.nativeReads+1,'scroll invalidates geometry before the next input');
 assert.equal(result.acrossFrameReads,0,'continuous motion reuses geometry across frames');
 assert.equal(result.calls.length,128,'each accepted edge is delivered once');
 assert.deepEqual(result.calls[0],[0,91,...result.first]);
 assert.deepEqual(result.calls[121],[2,91,...result.first]);
 assert.deepEqual(result.calls[122],['cancel']);
 assert.deepEqual(result.calls.slice(123,126),[[0,92,.5,.5],[1,92,.5,.5],[2,92,.5,.5]]);
 assert.equal(result.controls.length,1);
 assert.equal(result.controls[0][3],42,'Bomb serial crosses the carrier once');
}
