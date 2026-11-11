// PC-98 framebuffer updates already follow the emulated refresh. Local play
// uploads immediately; lockstep consumes the latest buffer at a synchronous
// batch boundary, after replay has caught up. No browser callback reads the heap.
export function installFramePresenter(mod,{deferred=false,isReplaying=()=>false}={}){
  const gl=mod.gl;
  if(!gl||mod.glfx)throw Error('TH03 画面呈现需要 PC-98 2D WebGL 上下文');
  const metrics={mode:deferred?'batch':'direct',submitted:0,uploaded:0,drawn:0,repeated:0,replaySubmitted:0,coalesced:0,restores:0};
  const gaps=new Float64Array(512);
  let gapCount=0,gapCursor=0,lastDraw=null,latest=null,dirty=false,disposed=false;
  let width=0,height=0;
  function shader(type,source){
    const s=gl.createShader(type);gl.shaderSource(s,source);gl.compileShader(s);
    if(!gl.getShaderParameter(s,gl.COMPILE_STATUS)){const message=gl.getShaderInfoLog(s);gl.deleteShader(s);throw Error(message);}
    return s;
  }
  const vs=shader(gl.VERTEX_SHADER,`
    attribute vec2 position; attribute vec2 uv; varying vec2 texcoord;
    void main(){gl_Position=vec4(position,0.0,1.0);texcoord=uv;}`);
  const fs=shader(gl.FRAGMENT_SHADER,`
    precision mediump float; varying vec2 texcoord; uniform sampler2D image;
    void main(){gl_FragColor=vec4(texture2D(image,texcoord).rgb,1.0);}`);
  const program=gl.createProgram();gl.attachShader(program,vs);gl.attachShader(program,fs);gl.linkProgram(program);
  gl.deleteShader(vs);gl.deleteShader(fs);
  if(!gl.getProgramParameter(program,gl.LINK_STATUS)){const message=gl.getProgramInfoLog(program);gl.deleteProgram(program);throw Error(message);}
  const position=gl.getAttribLocation(program,'position'),uv=gl.getAttribLocation(program,'uv');
  const sampler=gl.getUniformLocation(program,'image'),buffer=gl.createBuffer(),texture=gl.createTexture();
  gl.bindBuffer(gl.ARRAY_BUFFER,buffer);
  // Match upstream's top-left RGB orientation and nearest-neighbor scaling.
  gl.bufferData(gl.ARRAY_BUFFER,new Float32Array([
    -1,-1,0,1, 1,-1,1,1, 1,1,1,0,
    -1,-1,0,1, 1,1,1,0, -1,1,0,0,
  ]),gl.STATIC_DRAW);
  gl.bindTexture(gl.TEXTURE_2D,texture);
  for(const parameter of [gl.TEXTURE_WRAP_S,gl.TEXTURE_WRAP_T])gl.texParameteri(gl.TEXTURE_2D,parameter,gl.CLAMP_TO_EDGE);
  for(const parameter of [gl.TEXTURE_MIN_FILTER,gl.TEXTURE_MAG_FILTER])gl.texParameteri(gl.TEXTURE_2D,parameter,gl.NEAREST);
  const capabilities=[gl.BLEND,gl.DEPTH_TEST,gl.STENCIL_TEST,gl.SCISSOR_TEST,gl.CULL_FACE];
  function flush({redraw=false}={}){
    if(disposed||!latest||(!dirty&&!redraw))return false;
    if(gl.isContextLost())throw Error('GPU 画面上下文已丢失，请重新开始');
    const {frame,frameWidth,frameHeight}=latest,size=frameWidth*frameHeight*3;
    if(dirty&&(frame<0||size<=0||frame+size>mod.HEAPU8.length))throw Error('TH03 画面缓冲范围无效');
    gl.bindFramebuffer(gl.FRAMEBUFFER,null);gl.viewport(0,0,frameWidth,frameHeight);gl.useProgram(program);
    for(const capability of capabilities)gl.disable(capability);
    gl.colorMask(true,true,true,true);gl.bindBuffer(gl.ARRAY_BUFFER,buffer);
    gl.vertexAttribPointer(position,2,gl.FLOAT,false,16,0);gl.vertexAttribPointer(uv,2,gl.FLOAT,false,16,8);
    gl.enableVertexAttribArray(position);gl.enableVertexAttribArray(uv);
    gl.activeTexture(gl.TEXTURE0);gl.bindTexture(gl.TEXTURE_2D,texture);gl.uniform1i(sampler,0);
    gl.pixelStorei(gl.UNPACK_ALIGNMENT,1);
    // WebGL consumes this view during the call. Reacquire the heap on every
    // upload, including after memory growth or a rollback restore.
    if(dirty){
      const pixels=mod.HEAPU8.subarray(frame,frame+size);
      if(width!==frameWidth||height!==frameHeight){
        width=frameWidth;height=frameHeight;
        gl.texImage2D(gl.TEXTURE_2D,0,gl.RGB,width,height,0,gl.RGB,gl.UNSIGNED_BYTE,pixels);
      }else gl.texSubImage2D(gl.TEXTURE_2D,0,0,0,width,height,gl.RGB,gl.UNSIGNED_BYTE,pixels);
      metrics.uploaded++;
    }else metrics.repeated++;
    // The upstream context has preserveDrawingBuffer=false. When only HUD
    // changes, redraw the retained texture before drawImage in this same task.
    gl.drawArrays(gl.TRIANGLES,0,6);dirty=false;metrics.drawn++;
    const now=performance.now();
    if(lastDraw!==null){gaps[gapCursor]=now-lastDraw;gapCursor=(gapCursor+1)%gaps.length;gapCount=Math.min(gapCount+1,gaps.length);}
    lastDraw=now;mod.observeFrameDraw?.(now);
    return true;
  }
  mod.updateTexture=(frame,frameWidth,frameHeight)=>{
    if(disposed)return;
    metrics.submitted++;mod.frameUploads=(mod.frameUploads||0)+1;
    if(isReplaying())metrics.replaySubmitted++;
    if(dirty)metrics.coalesced++;
    latest={frame,frameWidth,frameHeight};dirty=true;
    if(!deferred)flush();
  };
  return {
    flush,
    capture:()=>latest?{...latest}:null,
    restore(saved){latest=saved?{...saved}:null;dirty=!!latest;metrics.restores++;},
    resetTiming(){lastDraw=null;gapCount=gapCursor=0;},
    info(){
      const sorted=Array.from(gaps.subarray(0,gapCount)).sort((a,b)=>a-b);
      const percentile=p=>sorted.length?sorted[Math.ceil(sorted.length*p)-1]:null;
      return {...metrics,pending:dirty,intervals:{count:gapCount,p95Ms:percentile(.95),p99Ms:percentile(.99),maxMs:sorted.at(-1)??null},
        intervalScope:'last 512 WebGL draw intervals; not physical screen refresh'};
    },
    dispose(){if(disposed)return;disposed=true;gl.deleteTexture(texture);gl.deleteBuffer(buffer);gl.deleteProgram(program);},
  };
}
