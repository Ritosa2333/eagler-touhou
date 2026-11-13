// WebGPU object host for WebGpuRenderer.hpp. Vertex processing, packed pixel
// expansion, matrix composition and batching are supplied by the C++ owner.
export class Th14WebGpuHost {
  static async create({canvas, gpu = globalThis.navigator?.gpu, shader, resampleShader, onError} = {}) {
    if (!gpu) throw new Error('WebGPU is unavailable');
    const adapter = await gpu.requestAdapter({powerPreference:'high-performance'});
    if (!adapter) throw new Error('No WebGPU adapter');
    const device = await adapter.requestDevice({requiredFeatures:[]});
    const load = async name => {
      const response = await fetch(new URL(name, import.meta.url));
      if (!response.ok) throw new Error(`Unable to load ${name}: ${response.status}`);
      return response.text();
    };
    try {
      const sources = await Promise.all([shader ?? load('Th14WebGpu.wgsl'), resampleShader ?? load('Th14WebGpuResample.wgsl')]);
      const host = new Th14WebGpuHost(device, sources[0], sources[1], {canvas, gpu, onError});
      await host.initialize();
      return host;
    } catch (error) { device.destroy(); throw error; }
  }

  constructor(device, shader, resampleShader, {canvas, gpu, onError} = {}) {
    this.device = device;
    this.resources = new Map(); this.depths = new Map(); this.pipelines = new Map(); this.bindings = new Map();
    this.encoder = null; this.buffers = []; this.closed = false; this.failure = null;
    this.shader = device.createShaderModule({code:shader});
    this.resampleShader = device.createShaderModule({code:resampleShader});
    this.clearShader = device.createShaderModule({code:`
struct Uniform{color:vec4f,depth:vec4f};
@group(0) @binding(0) var<uniform> u:Uniform;
@vertex fn vs(@builtin(vertex_index) id:u32)->@builtin(position) vec4f{let p=vec2f(f32((id<<1u)&2u),f32(id&2u));return vec4f(p*2.0-1.0,0,1);}
struct Output{@location(0) color:vec4f,@builtin(frag_depth) depth:f32};
@fragment fn fs()->Output{return Output(u.color,u.depth.x);}
@fragment fn fsColor()->@location(0) vec4f{return u.color;}`});
    this.clearPipelines = new Map();
    this.resamplePipeline = device.createRenderPipeline({layout:'auto', vertex:{module:this.resampleShader,entryPoint:'vs'},
      fragment:{module:this.resampleShader,entryPoint:'fs',targets:[{format:'bgra8unorm'}]}});
    this.canvasContext = canvas?.getContext('webgpu');
    if (canvas && !this.canvasContext) throw new Error('Unable to create WebGPU canvas context');
    if (this.canvasContext) {
      this.canvasFormat = gpu.getPreferredCanvasFormat();
      this.canvasContext.configure({device,format:this.canvasFormat,alphaMode:'opaque'});
      const module = device.createShaderModule({code:`
@group(0) @binding(0) var image:texture_2d<f32>;
@vertex fn vs(@builtin(vertex_index) id:u32)->@builtin(position) vec4f{let p=vec2f(f32((id<<1u)&2u),f32(id&2u));return vec4f(p*2.0-1.0,0,1);}
@fragment fn fs(@builtin(position) at:vec4f)->@location(0) vec4f{return textureLoad(image,vec2i(at.xy),0);}`});
      this.presentPipeline = device.createRenderPipeline({layout:'auto',vertex:{module,entryPoint:'vs'},
        fragment:{module,entryPoint:'fs',targets:[{format:this.canvasFormat}]}});
    }
    device.addEventListener('uncapturederror', event => {
      this.failure = event.error; onError?.(event.error);
    });
    device.lost.then(info => {if (!this.closed) {this.failure = new Error(`WebGPU device lost: ${info.message}`);onError?.(this.failure);}});
  }

  assertHealthy() {if (this.closed) throw new Error('WebGPU host closed'); if (this.failure) throw this.failure;}
  commands() {this.assertHealthy(); return this.encoder ??= this.device.createCommandEncoder();}
  flush() {
    if (!this.encoder) return;
    this.device.queue.submit([this.encoder.finish()]); this.encoder = null;
    const retired = this.buffers; this.buffers = [];
    if (retired.length) this.device.queue.onSubmittedWorkDone().then(() => {for (const buffer of retired) buffer.destroy();}, () => {for (const buffer of retired) buffer.destroy();});
  }
  buffer(raw, usage) {
    const bytes = new Uint8Array(raw.buffer,raw.byteOffset,raw.byteLength);
    const buffer = this.device.createBuffer({size:(bytes.length+3)&~3,usage,mappedAtCreation:true});
    new Uint8Array(buffer.getMappedRange()).set(bytes);buffer.unmap();this.buffers.push(buffer);return buffer;
  }

  async initialize() {
    for (const shader of [this.shader,this.resampleShader,this.clearShader]) {
      const info = await shader.getCompilationInfo();
      const errors = info.messages.filter(message => message.type === 'error');
      if (errors.length) throw new Error(errors.map(error => error.message).join('\n'));
    }
    this.alphaBoundaries = await this.measureAlphaBoundaries();
    this.upload(0,1,1,new Uint8Array([255,255,255,255]));this.assertHealthy();
  }

  upload(handle,width,height,bytes) {
    this.assertHealthy();this.flush();this.release(handle);
    const texture = this.device.createTexture({size:[width,height,1],format:'bgra8unorm',
      usage:GPUTextureUsage.COPY_SRC|GPUTextureUsage.COPY_DST|GPUTextureUsage.TEXTURE_BINDING});
    this.device.queue.writeTexture({texture},bytes,{bytesPerRow:width*4},[width,height,1]);
    this.resources.set(handle,{width,height,sample:texture,target:null,dirty:false});
  }
  target(handle) {
    const item = this.resources.get(handle);if (!item) throw new Error(`Missing target ${handle}`);
    if (!item.target) {
      item.target = this.device.createTexture({size:[item.width+1,item.height+1,1],format:'bgra8unorm',
        usage:GPUTextureUsage.COPY_SRC|GPUTextureUsage.COPY_DST|GPUTextureUsage.RENDER_ATTACHMENT});
      this.commands().copyTextureToTexture({texture:item.sample},{texture:item.target},[item.width,item.height,1]);
    }
    return item.target;
  }
  sample(handle) {
    const item = this.resources.get(handle);if (!item) throw new Error(`Missing texture ${handle}`);
    if (item.dirty) {
      this.commands().copyTextureToTexture({texture:item.target},{texture:item.sample},[item.width,item.height,1]);item.dirty=false;
    }
    return item.sample;
  }
  depth(handle,width,height,formatCode) {
    const format = {10:'depth16unorm',11:'depth24plus',12:'depth24plus-stencil8'}[formatCode];
    if (!format) throw new Error('Invalid depth format');
    const prior = this.depths.get(handle);
    if (prior) {if (prior.width!==width||prior.height!==height||prior.format!==format) throw new Error('Depth size/format changed without release');return;}
    const texture = this.device.createTexture({size:[width+1,height+1,1],format,usage:GPUTextureUsage.RENDER_ATTACHMENT});
    this.depths.set(handle,{texture,width,height,format});
  }
  depthAttachment(handle) {
    if (!handle) return undefined;
    const item = this.depths.get(handle);if (!item) throw new Error(`Missing depth ${handle}`);
    return {view:item.texture.createView(),depthLoadOp:'load',depthStoreOp:'store',
      ...(item.format.endsWith('stencil8')?{stencilLoadOp:'load',stencilStoreOp:'store'}:{})};
  }

  pipeline(p,instanced=false) {
    const format = p[1]?this.depths.get(p[1]).format:null;
    const key = JSON.stringify([p[3],...p.slice(4,13),p[19],format,instanced]);
    if (this.pipelines.has(key)) return this.pipelines.get(key);
    const factors = ['zero','one','src','one-minus-src','src-alpha','one-minus-src-alpha','dst-alpha','one-minus-dst-alpha','dst','one-minus-dst','src-alpha-saturated'];
    const operations = ['add','subtract','reverse-subtract','min','max'];
    const compares = ['never','less','equal','less-equal','greater','not-equal','greater-equal','always'];
    const target = {format:'bgra8unorm',writeMask:p[12]};
    if (p[6]) {
      const factor = (value,alpha) => alpha && (value===2||value===3)?factors[value+2]:
        alpha&&(value===8||value===9)?factors[value-2]:alpha&&value===10?'one':factors[value];
      const component = alpha => {
        const separate=alpha&&!!(p[19]&1),src=separate?(p[19]>>>1)&15:p[7],dst=separate?(p[19]>>>5)&15:p[8],op=separate?(p[19]>>>9)&7:p[9];
        return {srcFactor:op>=3?'one':factor(src,alpha),dstFactor:op>=3?'one':factor(dst,alpha),operation:operations[op]};
      };
      target.blend={color:component(false),alpha:component(true)};
    }
    const descriptor = {layout:'auto',
      vertex:{module:this.shader,entryPoint:instanced?'vsInstanced':'vs',buffers:[{arrayStride:40,attributes:[
        {shaderLocation:0,offset:0,format:'float32x4'},{shaderLocation:1,offset:16,format:'unorm8x4'},
        {shaderLocation:2,offset:20,format:'float32x2'},{shaderLocation:3,offset:28,format:'float32'},
        {shaderLocation:4,offset:32,format:'float32'},{shaderLocation:5,offset:36,format:'unorm8x4'}]}]},
      fragment:{module:this.shader,entryPoint:format?'fs':'fsColor',targets:[target]},
      primitive:{topology:{0:'point-list',1:'line-list',3:'triangle-list'}[p[3]],frontFace:'cw',cullMode:['none','back','front'][p[11]]},
      ...(format?{depthStencil:{format,depthWriteEnabled:!!(p[4]&&p[5]),depthCompare:p[4]?compares[p[10]]:'always'}}:{})};
    if(instanced) descriptor.vertex.buffers.push({arrayStride:148,stepMode:'instance',attributes:[
      ...Array.from({length:8},(_,i)=>({shaderLocation:6+i,offset:16*i,format:'float32x4'})),
      {shaderLocation:14,offset:128,format:'float32x4'},
      {shaderLocation:15,offset:144,format:'float32'}]});
    const pipeline=this.device.createRenderPipeline(descriptor);
    const entry={pipeline,key};this.pipelines.set(key,entry);return entry;
  }

  draw(packet,vertices,count,instances=null) {
    this.assertHealthy();if (packet.byteLength!==448||vertices.byteLength!==count*40) throw new Error('Invalid C++ draw packet');
    const p = new Uint32Array(packet.buffer,packet.byteOffset,20);
    const uniform = packet.subarray(80);const floats = new Float32Array(uniform.buffer,uniform.byteOffset,92);
    const viewport = floats.subarray(80,84);const range = new Float32Array(p.buffer,p.byteOffset+68,2);
    const target=this.target(p[0]),texture=this.sample(p[2]),entry=this.pipeline(p,instances!==null),pipeline=entry.pipeline;
    const key=JSON.stringify([entry.key,p[2],...p.slice(13,17)]);
    if (!this.bindings.has(key)) {
      const address=['repeat','mirror-repeat','clamp-to-edge'];
      const sampler=this.device.createSampler({addressModeU:address[p[15]],addressModeV:address[p[16]],minFilter:['nearest','linear'][p[13]],magFilter:['nearest','linear'][p[14]]});
      this.bindings.set(key,this.device.createBindGroup({layout:pipeline.getBindGroupLayout(1),entries:[{binding:0,resource:texture.createView()},{binding:1,resource:sampler}]}));
    }
    const vb=this.buffer(vertices,GPUBufferUsage.VERTEX),ub=this.buffer(uniform,GPUBufferUsage.UNIFORM);
    const binding=this.device.createBindGroup({layout:pipeline.getBindGroupLayout(0),entries:[{binding:0,resource:{buffer:ub,size:368}}]});
    const pass=this.commands().beginRenderPass({colorAttachments:[{view:target.createView(),loadOp:'load',storeOp:'store'}],depthStencilAttachment:this.depthAttachment(p[1])});
    const item=this.resources.get(p[0]);pass.setScissorRect(0,0,item.width,item.height);
    pass.setViewport(viewport[0]+.5,viewport[1]+.5,viewport[2],viewport[3],range[0],range[1]);
    pass.setPipeline(pipeline);pass.setBindGroup(0,binding);pass.setBindGroup(1,this.bindings.get(key));pass.setVertexBuffer(0,vb);
    if(instances!==null)pass.setVertexBuffer(1,this.buffer(instances,GPUBufferUsage.VERTEX));
    pass.draw(count,instances===null?1:instances.byteLength/148);pass.end();item.dirty=true;
  }

  drawInstances(packet,vertices,count,instances,instanceCount) {
    if(!Number.isInteger(instanceCount)||instanceCount<2||instanceCount>1024||instances.byteLength!==instanceCount*148)
      throw new Error('Invalid C++ instance packet');
    return this.draw(packet,vertices,count,instances);
  }
  clear(target,depth,flags,color,value,stencil,rects) {
    const item=this.resources.get(target),output=this.target(target);
    const rgba=[16,8,0,24].map(shift=>((color>>>shift)&255)/255);
    const full=rects.length===4&&rects[0]===0&&rects[1]===0&&rects[2]===item.width&&rects[3]===item.height;
    const z=this.depthAttachment(depth);
    if ((flags&4)&&(!z||!this.depths.get(depth).format.endsWith('stencil8'))) throw new Error('Stencil clear requires stencil storage');
    if (full) {
      if (z&&(flags&2)) Object.assign(z,{depthLoadOp:'clear',depthClearValue:value});
      if (flags&4) Object.assign(z,{stencilLoadOp:'clear',stencilClearValue:stencil&255});
      this.commands().beginRenderPass({colorAttachments:[{view:output.createView(),loadOp:flags&1?'clear':'load',storeOp:'store',clearValue:rgba}],depthStencilAttachment:z}).end();
    } else {
      const format=depth?this.depths.get(depth).format:null,key=JSON.stringify([format,flags&7]);
      if (!this.clearPipelines.has(key)) this.clearPipelines.set(key,this.device.createRenderPipeline({layout:'auto',vertex:{module:this.clearShader,entryPoint:'vs'},
        fragment:{module:this.clearShader,entryPoint:format?'fs':'fsColor',targets:[{format:'bgra8unorm',writeMask:flags&1?15:0}]},
        ...(format?{depthStencil:{format,depthWriteEnabled:!!(flags&2),depthCompare:'always',
          ...(flags&4?{stencilFront:{compare:'always',passOp:'replace'},stencilBack:{compare:'always',passOp:'replace'},stencilReadMask:255,stencilWriteMask:255}:{})}}:{})}));
      const pipeline=this.clearPipelines.get(key),buffer=this.buffer(new Float32Array([...rgba,value,0,0,0]),GPUBufferUsage.UNIFORM);
      const bind=this.device.createBindGroup({layout:pipeline.getBindGroupLayout(0),entries:[{binding:0,resource:{buffer}}]});
      const pass=this.commands().beginRenderPass({colorAttachments:[{view:output.createView(),loadOp:'load',storeOp:'store'}],depthStencilAttachment:z});
      pass.setPipeline(pipeline);pass.setBindGroup(0,bind);
      if (flags&4) pass.setStencilReference(stencil&255);
      for (let i=0;i<rects.length;i+=4) {
        const left=Math.max(0,rects[i]),top=Math.max(0,rects[i+1]),right=Math.min(item.width,rects[i+2]),bottom=Math.min(item.height,rects[i+3]);
        if (right>left&&bottom>top) {pass.setScissorRect(left,top,right-left,bottom-top);pass.draw(3);}
      }
      pass.end();
    }
    item.dirty=true;
  }

  resample(source,rect,target,to,weights,width,height,sourceMask,targetMask) {
    const texture=this.sample(source),output=this.target(target),buffer=new ArrayBuffer(80),ints=new Int32Array(buffer),words=new Uint32Array(buffer);
    ints.set([rect[0],rect[1],rect[2]-rect[0],rect[3]-rect[1],to[0],to[1],to[2]-to[0],to[3]-to[1]]);
    words.set(sourceMask,8);words.set(targetMask,12);words.set([+(weights!==null),width,0,0],16);
    const ub=this.buffer(new Uint8Array(buffer),GPUBufferUsage.UNIFORM),wb=this.buffer(weights??new Uint8Array(8),GPUBufferUsage.STORAGE);
    const pipeline=this.resamplePipeline,bind=this.device.createBindGroup({layout:pipeline.getBindGroupLayout(0),entries:[{binding:0,resource:{buffer:ub}},{binding:1,resource:texture.createView()},{binding:2,resource:{buffer:wb}}]});
    const pass=this.commands().beginRenderPass({colorAttachments:[{view:output.createView(),loadOp:'load',storeOp:'store'}]});
    pass.setPipeline(pipeline);pass.setBindGroup(0,bind);pass.setViewport(to[0],to[1],to[2]-to[0],to[3]-to[1],0,1);pass.setScissorRect(to[0],to[1],to[2]-to[0],to[3]-to[1]);pass.draw(3);pass.end();this.resources.get(target).dirty=true;return 1;
  }
  copy(source,rect,target,point) {
    this.commands().copyTextureToTexture({texture:this.sample(source),origin:[rect[0],rect[1],0]},
      {texture:this.target(target),origin:[point[0],point[1],0]},[rect[2]-rect[0],rect[3]-rect[1],1]);this.resources.get(target).dirty=true;
  }
  async readTexture(texture,width,height) {
    const stride=(width*4+255)&~255;
    const buffer=this.device.createBuffer({size:stride*height,usage:GPUBufferUsage.COPY_DST|GPUBufferUsage.MAP_READ});
    this.commands().copyTextureToBuffer({texture},{buffer,bytesPerRow:stride,rowsPerImage:height},[width,height,1]);this.flush();
    try {
      await buffer.mapAsync(GPUMapMode.READ);this.assertHealthy();
      const mapped=new Uint8Array(buffer.getMappedRange()),raw=new Uint8Array(width*height*4);
      for (let y=0;y<height;++y) raw.set(mapped.subarray(y*stride,y*stride+width*4),y*width*4);
      buffer.unmap();return raw;
    } finally {buffer.destroy();}
  }
  async read(handle) {const item=this.resources.get(handle);return this.readTexture(this.target(handle),item.width,item.height);}
  present(handle) {
    this.assertHealthy();
    if (this.canvasContext) {
      const source=this.sample(handle),output=this.canvasContext.getCurrentTexture();
      const bind=this.device.createBindGroup({layout:this.presentPipeline.getBindGroupLayout(0),entries:[{binding:0,resource:source.createView()}]});
      const pass=this.commands().beginRenderPass({colorAttachments:[{view:output.createView(),loadOp:'clear',storeOp:'store',clearValue:[0,0,0,1]}]});
      pass.setPipeline(this.presentPipeline);pass.setBindGroup(0,bind);pass.draw(3);pass.end();
    }
    this.flush();
  }
  release(handle) {
    this.flush();const item=this.resources.get(handle);if (item) {item.sample.destroy();item.target?.destroy();this.resources.delete(handle);}
    const depth=this.depths.get(handle);if (depth) {depth.texture.destroy();this.depths.delete(handle);}
    this.bindings.clear();
  }
  close() {if (this.closed) return;this.flush();this.closed=true;for (const handle of [...this.resources.keys(),...this.depths.keys()]) this.release(handle);this.canvasContext?.unconfigure();this.device.destroy();}

  async measureAlphaBoundaries() {
    const shader=this.device.createShaderModule({code:`
@group(0) @binding(0) var<storage,read> values:array<vec4f>;
@vertex fn vs(@builtin(vertex_index) id:u32)->@builtin(position) vec4f{let p=vec2f(f32((id<<1u)&2u),f32(id&2u));return vec4f(p*2.0-1.0,0,1);}
@fragment fn fs(@builtin(position) p:vec4f)->@location(0) vec4f{return vec4f(0,0,0,values[u32(p.x)].x);}`});
    const pipeline=this.device.createRenderPipeline({layout:'auto',vertex:{module:shader,entryPoint:'vs'},fragment:{module:shader,entryPoint:'fs',targets:[{format:'bgra8unorm'}]}});
    const values=this.device.createBuffer({size:4096,usage:GPUBufferUsage.STORAGE|GPUBufferUsage.COPY_DST});
    const bind=this.device.createBindGroup({layout:pipeline.getBindGroupLayout(0),entries:[{binding:0,resource:{buffer:values}}]});
    const target=this.device.createTexture({size:[256,1,1],format:'bgra8unorm',usage:GPUTextureUsage.RENDER_ATTACHMENT|GPUTextureUsage.COPY_SRC});
    const sample=async words=>{
      const data=new Uint32Array(1024);for (let i=0;i<256;++i) data[i*4]=words[i];this.device.queue.writeBuffer(values,0,data);
      const pass=this.commands().beginRenderPass({colorAttachments:[{view:target.createView(),loadOp:'clear',storeOp:'store',clearValue:[0,0,0,0]}]});
      pass.setPipeline(pipeline);pass.setBindGroup(0,bind);pass.draw(3);pass.end();
      const raw=await this.readTexture(target,256,1);return Uint8Array.from({length:256},(_,i)=>raw[i*4+3]);
    };
    const bits=new ArrayBuffer(4),float=new Float32Array(bits),word=new Uint32Array(bits),encode=value=>{float[0]=value;return word[0];};
    const lo=Uint32Array.from({length:256},(_,i)=>i?encode((i-1)/255):0),hi=Uint32Array.from({length:256},(_,i)=>i?encode(i/255):0);
    try {
      for (let step=0;step<32;++step) {
        if (hi.every((v,i)=>!i||v-lo[i]<=1)) break;
        const mid=Uint32Array.from(hi,(v,i)=>lo[i]+Math.floor((v-lo[i])/2)),result=await sample(mid);
        for (let i=1;i<256;++i) if (hi[i]-lo[i]>1) {if (result[i]>=i) hi[i]=mid[i];else lo[i]=mid[i];}
      }
      const lower=await sample(lo),upper=await sample(hi);
      for (let i=1;i<256;++i) if (hi[i]-lo[i]!==1||lower[i]>=i||upper[i]<i) throw new Error('RGBA8 alpha calibration failed');
      return new Float32Array(hi.buffer.slice(0));
    } finally {values.destroy();target.destroy();}
  }
}
