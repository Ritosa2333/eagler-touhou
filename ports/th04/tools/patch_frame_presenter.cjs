// Install before ws-ready / the first framebuffer submission, so no upstream
// rAF remains queued when the adapter starts. Both generated cores use this.
function patch(source){
  const anchor='requestAnimationFrameId=null})}}})()';
  if(source.split(anchor).length!==2)throw Error('DOSBox-X framebuffer initialization anchor changed');
  return source.replace(anchor,'requestAnimationFrameId=null})}};Module.installFramePresenter?.()})()');
}
module.exports={patch};
