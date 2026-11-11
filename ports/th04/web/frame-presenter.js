// PC-98 framebuffer updates already follow the emulated refresh. Local play
// uploads immediately; lockstep consumes the latest buffer at a synchronous
// batch boundary, after replay has caught up. No browser callback reads the heap.
export function installFramePresenter(mod,{deferred=false,isReplaying=()=>false}={}){
  const gl=mod.gl;
  const overlays=deferred&&!!mod.worker;
  if(!gl||mod.glfx)throw Error('TH04 画面呈现需要 PC-98 2D WebGL 上下文');
  const metrics={mode:deferred?'batch':'direct',submitted:0,uploaded:0,drawn:0,repeated:0,replaySubmitted:0,coalesced:0,restores:0};
  const gaps=new Float64Array(512);
  let gapCount=0,gapCursor=0,lastDraw=null,latest=null,dirty=false,disposed=false,intervalStats=null;
  let width=0,height=0;
  function shader(type,source){
    const s=gl.createShader(type);gl.shaderSource(s,source);gl.compileShader(s);
    if(!gl.getShaderParameter(s,gl.COMPILE_STATUS)){const message=gl.getShaderInfoLog(s);gl.deleteShader(s);throw Error(message);}
    return s;
  }
  const vs=shader(gl.VERTEX_SHADER,`
    attribute vec2 position; attribute vec2 uv; varying vec2 texcoord;
    void main(){gl_Position=vec4(position,0.0,1.0);texcoord=uv;}`);
  // The cooperative native HUD clears graphics to palette index zero.
  // Orange/Elly deliberately recolor that entry during battle. TRAM black
  // spaces conceal it, so sampling a fixed pixel cannot identify the color
  // behind visible text. Read the bound guest palette instead.
  const sidebarShader=`
    uniform bool normalizeSidebar; uniform vec3 sidebarBackground;
    vec3 sidebarRgb(vec3 rgb){
      if(normalizeSidebar&&texcoord.x>=0.7&&texcoord.x<0.9){
        if(all(lessThan(abs(rgb-sidebarBackground),vec3(0.0001))))return vec3(0.0);
      }
      return rgb;
    }`;
  const fs=shader(gl.FRAGMENT_SHADER,overlays?`
    precision mediump float; varying vec2 texcoord; uniform sampler2D image;
    ${sidebarShader}
    uniform sampler2D personalHud; uniform vec2 frameSize;
    uniform vec2 hitpoint1; uniform vec2 hitpoint2; uniform vec2 hitpoint3;
    vec3 marker(vec3 rgb,vec2 pixel,vec2 point){
      vec2 distance=abs(pixel-point);float edge=max(distance.x,distance.y);
      return edge<=2.0?(edge<=1.0?vec3(1.0):vec3(0.0)):rgb;
    }
    void main(){
      vec3 rgb=sidebarRgb(texture2D(image,texcoord).rgb);vec2 pixel=floor(texcoord*frameSize);
      if(pixel.x>=32.0&&pixel.x<416.0&&pixel.y>=16.0&&pixel.y<384.0){
        rgb=marker(rgb,pixel,hitpoint1);rgb=marker(rgb,pixel,hitpoint2);rgb=marker(rgb,pixel,hitpoint3);
      }
      vec4 hud=vec4(0.0);
      if(frameSize.x==640.0&&frameSize.y==400.0){
        if(pixel.x>=448.0&&pixel.x<576.0)
          hud=texture2D(personalHud,vec2((pixel.x+0.5-448.0)/256.0,texcoord.y));
        else if(pixel.x>=272.0&&pixel.x<400.0)
          hud=texture2D(personalHud,vec2((pixel.x+0.5-272.0+128.0)/256.0,texcoord.y));
      }
      gl_FragColor=vec4(mix(rgb,hud.rgb,hud.a),1.0);
    }`:`
    precision mediump float; varying vec2 texcoord; uniform sampler2D image;
    ${sidebarShader}
    void main(){gl_FragColor=vec4(sidebarRgb(texture2D(image,texcoord).rgb),1.0);}`);
  const program=gl.createProgram();gl.attachShader(program,vs);gl.attachShader(program,fs);gl.linkProgram(program);
  gl.deleteShader(vs);gl.deleteShader(fs);
  if(!gl.getProgramParameter(program,gl.LINK_STATUS)){const message=gl.getProgramInfoLog(program);gl.deleteProgram(program);throw Error(message);}
  const position=gl.getAttribLocation(program,'position'),uv=gl.getAttribLocation(program,'uv');
  const sampler=gl.getUniformLocation(program,'image'),buffer=gl.createBuffer(),texture=gl.createTexture();
  const normalizeSidebar=gl.getUniformLocation(program,'normalizeSidebar');
  const sidebarBackground=gl.getUniformLocation(program,'sidebarBackground');
  gl.bindBuffer(gl.ARRAY_BUFFER,buffer);
  // Match upstream's top-left RGB orientation and nearest-neighbor scaling.
  gl.bufferData(gl.ARRAY_BUFFER,new Float32Array([
    -1,-1,0,1, 1,-1,1,1, 1,1,1,0,
    -1,-1,0,1, 1,1,1,0, -1,1,0,0,
  ]),gl.STATIC_DRAW);
  gl.bindTexture(gl.TEXTURE_2D,texture);
  for(const parameter of [gl.TEXTURE_WRAP_S,gl.TEXTURE_WRAP_T])gl.texParameteri(gl.TEXTURE_2D,parameter,gl.CLAMP_TO_EDGE);
  for(const parameter of [gl.TEXTURE_MIN_FILTER,gl.TEXTURE_MAG_FILTER])gl.texParameteri(gl.TEXTURE_2D,parameter,gl.NEAREST);
  let hudTexture,hudRevision=-1,hudSampler,frameSize,pointUniforms;
  if(overlays){
    hudTexture=gl.createTexture();hudSampler=gl.getUniformLocation(program,'personalHud');
    frameSize=gl.getUniformLocation(program,'frameSize');
    pointUniforms=[1,2,3].map(n=>gl.getUniformLocation(program,'hitpoint'+n));
    gl.bindTexture(gl.TEXTURE_2D,hudTexture);
    for(const parameter of [gl.TEXTURE_WRAP_S,gl.TEXTURE_WRAP_T])gl.texParameteri(gl.TEXTURE_2D,parameter,gl.CLAMP_TO_EDGE);
    for(const parameter of [gl.TEXTURE_MIN_FILTER,gl.TEXTURE_MAG_FILTER])gl.texParameteri(gl.TEXTURE_2D,parameter,gl.NEAREST);
    gl.texImage2D(gl.TEXTURE_2D,0,gl.RGBA,1,1,0,gl.RGBA,gl.UNSIGNED_BYTE,new Uint8Array(4));
  }
  const capabilities=[gl.BLEND,gl.DEPTH_TEST,gl.STENCIL_TEST,gl.SCISSOR_TEST,gl.CULL_FACE];
  function flush({redraw=false}={}){
    if(disposed||!latest||(!dirty&&!redraw))return false;
    if(gl.isContextLost())throw Error('GPU 画面上下文已丢失，请重新开始');
    const {frame,frameWidth,frameHeight}=latest,size=frameWidth*frameHeight*3;
    if(dirty&&(frame<0||size<=0||frame+size>mod.HEAPU8.length))throw Error('TH04 画面缓冲范围无效');
    gl.bindFramebuffer(gl.FRAMEBUFFER,null);gl.viewport(0,0,frameWidth,frameHeight);gl.useProgram(program);
    for(const capability of capabilities)gl.disable(capability);
    gl.colorMask(true,true,true,true);gl.bindBuffer(gl.ARRAY_BUFFER,buffer);
    gl.vertexAttribPointer(position,2,gl.FLOAT,false,16,0);gl.vertexAttribPointer(uv,2,gl.FLOAT,false,16,8);
    gl.enableVertexAttribArray(position);gl.enableVertexAttribArray(uv);
    gl.activeTexture(gl.TEXTURE0);gl.bindTexture(gl.TEXTURE_2D,texture);gl.uniform1i(sampler,0);
    const palette=mod.sidebarPaletteAddress;
    // DOS EXEC can retain OP as the reported program name during solo MAIN.
    // Callers bind this address only while a validated game mailbox is live.
    const normalize=frameWidth===640&&frameHeight===400&&Number.isInteger(palette)&&palette>=0&&palette+3<=mod.HEAPU8.length;
    gl.uniform1i(normalizeSidebar,normalize?1:0);
    if(normalize){
      // DOSBox-X dac_4to6, then the VGA 6-to-8-bit DAC expansion.
      const rgb=Array.from(mod.HEAPU8.subarray(palette,palette+3),value=>{
        const nibble=value>>4,six=(nibble<<2)|(nibble>>2);return ((six<<2)|(six>>4))/255;
      });
      gl.uniform3f(sidebarBackground,...rgb);
    }
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
    if(overlays){
      const points=mod.getHitPoints?.()||[],hud=mod.getPersonalHud?.();
      gl.uniform2f(frameSize,frameWidth,frameHeight);
      pointUniforms.forEach((uniform,n)=>gl.uniform2f(uniform,...(points[n]||[-100,-100])));
      gl.activeTexture(gl.TEXTURE1);gl.bindTexture(gl.TEXTURE_2D,hudTexture);gl.uniform1i(hudSampler,1);
      if(hud&&hud.revision!==hudRevision){
        gl.pixelStorei(gl.UNPACK_PREMULTIPLY_ALPHA_WEBGL,false);
        gl.texImage2D(gl.TEXTURE_2D,0,gl.RGBA,gl.RGBA,gl.UNSIGNED_BYTE,hud.canvas);hudRevision=hud.revision;
      }
      gl.activeTexture(gl.TEXTURE0);
    }
    // transferToImageBitmap consumes the buffer; redraw retained RGB and HUD
    // even when native video did not change at this accepted boundary.
    gl.drawArrays(gl.TRIANGLES,0,6);dirty=false;metrics.drawn++;
    const now=performance.now();
    if(lastDraw!==null){gaps[gapCursor]=now-lastDraw;gapCursor=(gapCursor+1)%gaps.length;gapCount=Math.min(gapCount+1,gaps.length);intervalStats=null;}
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
    resetTiming(){lastDraw=null;gapCount=gapCursor=0;intervalStats=null;},
    info(){
      if(!intervalStats){
        const sorted=Array.from(gaps.subarray(0,gapCount)).sort((a,b)=>a-b);
        const percentile=p=>sorted.length?sorted[Math.ceil(sorted.length*p)-1]:null;
        intervalStats={count:gapCount,p95Ms:percentile(.95),p99Ms:percentile(.99),maxMs:sorted.at(-1)??null};
      }
      return {...metrics,pending:dirty,intervals:{...intervalStats},
        intervalScope:'last 512 WebGL draw intervals; not physical screen refresh'};
    },
    dispose(){if(disposed)return;disposed=true;gl.deleteTexture(texture);if(hudTexture)gl.deleteTexture(hudTexture);gl.deleteBuffer(buffer);gl.deleteProgram(program);},
  };
}
