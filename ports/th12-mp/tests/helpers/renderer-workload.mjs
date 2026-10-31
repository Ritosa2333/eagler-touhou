// Deterministic renderer-only workload. No retail assets or game tick required.
export function createRendererFixture(Renderer,{record=false}={}){
  let nextTexture=0,boundTexture=null,upload=null,blend=null,scissor=null;
  const textureParameters=new Map(),buffers=new WeakSet(),draws=[];
  const stats={uploads:0,uploadBytes:0,uploadBuffers:0,draws:0,parameters:0};
  const gl={};
  for(const [i,key]of ['VERTEX_SHADER','FRAGMENT_SHADER','COMPILE_STATUS','LINK_STATUS',
    'ARRAY_BUFFER','STREAM_DRAW','FLOAT','TEXTURE_2D','RGBA','RGB','RGB565','UNSIGNED_BYTE',
    'TEXTURE_MIN_FILTER','TEXTURE_MAG_FILTER','TEXTURE_WRAP_S','TEXTURE_WRAP_T',
    'NEAREST','LINEAR','REPEAT','CLAMP_TO_EDGE','BLEND','SRC_ALPHA','ONE_MINUS_SRC_ALPHA',
    'ONE','ZERO','ONE_MINUS_SRC_COLOR','ONE_MINUS_DST_COLOR','DST_COLOR','TRIANGLES',
    'LINE_STRIP','LINES','COLOR_BUFFER_BIT','SCISSOR_TEST','FRAMEBUFFER','READ_FRAMEBUFFER',
    'DRAW_FRAMEBUFFER','COLOR_ATTACHMENT0'].entries())gl[key]=i+1;
  for(const key of ['shaderSource','compileShader','attachShader','linkProgram','useProgram',
    'bindBuffer','bindVertexArray','enableVertexAttribArray','vertexAttribPointer','texImage2D',
    'viewport','clearColor','clear','enable','disable','uniform3fv','copyTexImage2D',
    'copyTexSubImage2D','texStorage2D','bindFramebuffer','framebufferTexture2D','blitFramebuffer'])gl[key]=()=>{};
  for(const key of ['createShader','createProgram','createBuffer','createVertexArray','getUniformLocation','createFramebuffer'])gl[key]=()=>({});
  gl.getShaderParameter=gl.getProgramParameter=()=>true;
  gl.createTexture=()=>({id:nextTexture++});
  gl.bindTexture=(_target,texture)=>{boundTexture=texture;};
  gl.texParameteri=(_target,name,value)=>{
    stats.parameters++;
    if(record){
      if(!textureParameters.has(boundTexture))textureParameters.set(boundTexture,new Map());
      textureParameters.get(boundTexture).set(name,value);
    }
  };
  gl.blendFunc=(...values)=>{blend=values;};
  gl.scissor=(...values)=>{scissor=values;};
  gl.bufferData=(_target,data,_usage,offset=0,length=data.length-offset)=>{
    stats.uploads++;stats.uploadBytes+=length*4;
    if(!buffers.has(data.buffer)){buffers.add(data.buffer);stats.uploadBuffers++;}
    if(record)upload=Array.from(data.subarray(offset,offset+length));
  };
  gl.drawArrays=(mode,first,count)=>{
    stats.draws++;
    if(record)draws.push({mode,first,count,texture:boundTexture.id,
      parameters:Object.fromEntries(textureParameters.get(boundTexture)),blend,scissor,vertices:upload});
  };
  const renderer=new Renderer({getContext:()=>gl},{bullet:{sprites:{405:{entry:0,x:0,y:0,w:16,h:16}}}});
  for(let i=0;i<2;i++)renderer.textures.set('bullet:'+i,{texture:gl.createTexture(),w:256,h:256,defaultWrap:gl.REPEAT});
  const reset=()=>{for(const key of Object.keys(stats))stats[key]=0;draws.length=0;};
  reset();return {renderer,gl,stats,draws,reset};
}

export const rendererWorkloads={
  dense:{sprites:4096,group:64},
  switches:{sprites:2048,group:1},
  large:{sprites:8192,group:8192},
};

export function drawRendererWorkload(renderer,{sprites,group}){
  renderer.begin();
  for(let i=0;i<sprites;i++)renderer.region('bullet',Math.floor(i/group)%2,
    16,32,16,16,32+(i%64)*6,16+Math.floor(i/64)%70*6,16,16,
    (i%16)*.125,.75,[.5,.75,1]);
  renderer.flush();
}
