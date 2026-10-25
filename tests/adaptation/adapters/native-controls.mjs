import assert from 'node:assert/strict';

/** Submitted native widgets. Reading never moves, scrolls or activates them. */
export function decodeNativeControls(raw){
  assert.equal(raw?.available,true,raw?.reason);const w=raw.words;
  assert.ok(Array.isArray(w)&&w.length===1024&&w.every(Number.isFinite));assert.ok(w[0]===1||w[0]===2);assert.equal(w[1],1);
  const stride=w[0]===1?12:16,capacity=w[0]===1?64:56;
  for(const i of [4])assert.ok(w[i]===0||w[i]===1);assert.equal(w[7],0,'Native widget observation overflow');
  assert.ok(Number.isSafeInteger(w[6])&&w[6]>=0&&w[6]<=capacity);
  const controls=Array.from({length:w[6]},(_,i)=>{
    const p=128+i*stride;for(const j of [0,1,9,10])assert.ok(Number.isSafeInteger(w[p+j])&&w[p+j]>=0);
    assert.ok(w[p]>0&&w[p]<64&&w[p+1]>0&&w[p+4]>0&&w[p+5]>0,`Malformed native widget: ${JSON.stringify(w.slice(p,p+12))}`);
    for(const j of [7,8])assert.ok(w[p+j]===0||w[p+j]===1);
    assert.equal(w[p+9],w[2]);if(w[0]===1)assert.equal(w[p+11],0);else assert.ok(w[p+11]===0||w[p+11]===1);
    const clip=w[0]===2?{left:w[p+12],top:w[p+13],right:w[p+14],bottom:w[p+15]}:null;
    if(clip)assert.ok(clip.right>=clip.left&&clip.bottom>=clip.top);
    return{semantic:w[p],id:w[p+1],rect:{x:w[p+2],y:w[p+3],width:w[p+4],height:w[p+5]},value:w[p+6],
      visible:!!w[p+7],enabled:!!w[p+8],generation:w[p+9],activations:w[p+10],popup:!!w[p+11],clip};
  });
  assert.equal(new Set(controls.map(c=>c.semantic)).size,controls.length,'Duplicate native widget semantic');
  return{generation:w[2],renderedGeneration:w[3],open:!!w[4],locale:w[5],controls,
    scroll:{left:w[8],top:w[9],right:w[10],bottom:w[11],offset:w[12],maximum:w[13],id:w[14],barWidth:w[15]},values:w.slice(16,64)};
}

export function nativeControlPoint(state,semantic,{width=640,height=480,allowDisabled=false}={}){
  assert.equal(state.generation,state.renderedGeneration,'Native widgets have not completed drawing');
  const item=state.controls.find(c=>c.semantic===semantic);assert.ok(item,'Native widget is not submitted');
  assert.equal(item.visible,true,'Native widget is clipped');if(!allowDisabled)assert.equal(item.enabled,true,'Native widget is disabled');
  const c=item.clip??state.scroll,r=item.rect,left=Math.max(0,c.left,r.x),top=Math.max(0,c.top,r.y),right=Math.min(width,c.right,r.x+r.width),bottom=Math.min(height,c.bottom,r.y+r.height);
  assert.ok(right-left>2&&bottom-top>2,'Native widget has no visible clipped target');
  return{x:(left+right)/2,y:(top+bottom)/2};
}
