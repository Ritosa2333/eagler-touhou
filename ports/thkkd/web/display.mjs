// 画布保留原作的 640×480；最终缩放在设备像素上保持 4:3 并对齐边界。
export function displayRect(width,height,dpr=1){
  const scale=Number.isFinite(dpr)&&dpr>0?dpr:1,w=Math.max(4,Math.floor(width*scale)),h=Math.max(3,Math.floor(height*scale));
  const unit=Math.max(1,Math.floor(Math.min(w/4,h/3))),pixels={width:unit*4,height:unit*3};
  return {left:Math.round((w-pixels.width)/2)/scale,top:Math.round((h-pixels.height)/2)/scale,width:pixels.width/scale,height:pixels.height/scale};
}
export function installDisplay(canvas,onFit=()=>{}){
  let media,lastDpr,lastWidth,lastHeight;
  const fit=()=>{
    if(lastDpr!==devicePixelRatio){media?.removeEventListener('change',fit);lastDpr=devicePixelRatio;media=matchMedia(`(resolution: ${lastDpr}dppx)`);media.addEventListener('change',fit);}
    lastWidth=innerWidth;lastHeight=innerHeight;const rect=displayRect(lastWidth,lastHeight,devicePixelRatio);for(const name of ['left','top','width','height'])canvas.style[name]=rect[name]+'px';onFit();
  };
  addEventListener('resize',fit);globalThis.visualViewport?.addEventListener('resize',fit);fit();
  return {check(){if(lastDpr!==devicePixelRatio||lastWidth!==innerWidth||lastHeight!==innerHeight)fit();},dispose(){removeEventListener('resize',fit);globalThis.visualViewport?.removeEventListener('resize',fit);media?.removeEventListener('change',fit);}};
}
