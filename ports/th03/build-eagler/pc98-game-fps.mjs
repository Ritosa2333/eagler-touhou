// Counts the game's own frame counter, not browser redraws or network ticks.
export function mountGameFps(){
  const node=document.createElement('div');node.id='pc98-game-fps';node.textContent='FPS: --';
  node.style.cssText='position:fixed;right:8px;bottom:8px;z-index:2147483647;pointer-events:none;color:#fff;background:#000b;padding:2px 5px;font:12px/1.2 monospace;text-shadow:0 1px 2px #000';
  document.body.append(node);let previous=null,at=0;
  return {update(frame,now){
    if(!Number.isInteger(frame)){previous=null;node.textContent='FPS: --';return;}
    if(previous===null){previous=frame;at=now;return;}
    const elapsed=now-at;if(elapsed<1000)return;
    const count=(frame-previous)>>>0;
    node.textContent=count>1000?'FPS: --':`FPS: ${Math.round(count*1000/elapsed)}`;
    previous=frame;at=now;
  },reset(){previous=null;node.textContent='FPS: --';}};
}
