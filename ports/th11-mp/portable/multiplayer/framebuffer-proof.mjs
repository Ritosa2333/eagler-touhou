// Serialize this function into a browser AFTER the performance window. It
// observes only the renderer's real final blit, never substitutes a draw.
export function installFramebufferProof(){
 const w=document.querySelector('iframe').contentWindow,canvas=w.document.querySelector('canvas');
 const gl=canvas.getContext('webgl2');if(!gl)throw Error('Framebuffer proof requires actual WebGL2');
 if(w.__framebufferProof)return;
 const bind=gl.bindFramebuffer,blit=gl.blitFramebuffer;
 let read=gl.getParameter(gl.READ_FRAMEBUFFER_BINDING),draw=gl.getParameter(gl.DRAW_FRAMEBUFFER_BINDING),last=null;
 gl.bindFramebuffer=function(target,value){
  const result=bind.call(this,target,value);
  if(target===gl.FRAMEBUFFER||target===gl.READ_FRAMEBUFFER)read=value;
  if(target===gl.FRAMEBUFFER||target===gl.DRAW_FRAMEBUFFER)draw=value;
  return result;
 };
 gl.blitFramebuffer=function(...args){
  const result=blit.apply(this,args);
  if(!draw&&read&&(args[8]&gl.COLOR_BUFFER_BIT)&&args[0]===0&&args[1]===0)
   last={framebuffer:read,width:args[2],height:args[3]};
  return result;
 };
 w.__framebufferProof={read(){
  if(!last||last.width!==640||last.height!==480)throw Error('Missing real 640x480 presentation framebuffer');
  if(gl.getParameter(gl.PIXEL_PACK_BUFFER_BINDING))throw Error('Unexpected pixel pack buffer binding');
  const before=read,alignment=gl.getParameter(gl.PACK_ALIGNMENT),bytes=new Uint8Array(last.width*last.height*4);
  try{
   gl.bindFramebuffer(gl.READ_FRAMEBUFFER,last.framebuffer);gl.pixelStorei(gl.PACK_ALIGNMENT,1);
   gl.readPixels(0,0,last.width,last.height,gl.RGBA,gl.UNSIGNED_BYTE,bytes);
   const error=gl.getError();if(error!==gl.NO_ERROR)throw Error('Framebuffer readback GL error '+error);
  }finally{gl.bindFramebuffer(gl.READ_FRAMEBUFFER,before);gl.pixelStorei(gl.PACK_ALIGNMENT,alignment);}
  let binary='';for(let i=0;i<bytes.length;i+=32768)binary+=String.fromCharCode(...bytes.subarray(i,i+32768));
  return {width:last.width,height:last.height,format:'RGBA8 bottom-left',base64:btoa(binary)};
 }};
}

// The playfield is the native 384x448 viewport at (32,16). Keep the complete
// raw images as evidence too. The outside frame includes wall-clock FPS text,
// which is not deterministic gameplay and is explicitly outside this oracle.
export function comparePlayfield(live,replay){
 if(live.length!==640*480*4||replay.length!==live.length)throw Error('Invalid pixel oracle shape');
 let mismatches=0;const first=[],distinct=new Set();
 for(let y=16;y<464;++y)for(let x=32;x<416;++x)for(let c=0;c<4;++c){
  const at=(y*640+x)*4+c;
  if(c===0&&distinct.size<257)distinct.add(live[at]|live[at+1]<<8|live[at+2]<<16|live[at+3]<<24);
  if(live[at]!==replay[at]){++mismatches;if(first.length<12)first.push({x,y,c,live:live[at],replay:replay[at]});}
 }
 if(distinct.size<16)throw Error('Flat/empty playfield cannot satisfy pixel equality');
 return {equal:mismatches===0,mismatches,first,distinctColorsAtLeast:distinct.size,viewport:{x:32,y:16,width:384,height:448},tolerance:0};
}
