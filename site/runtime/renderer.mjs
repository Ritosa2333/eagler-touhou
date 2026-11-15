import {argbColor, quadVertices, nativeRecordPriority,applyWorldShake,triangleStripVertices,circleFanVertices,circleLineVertices,ringStripVertices,decodeDrawBuffer} from './draw.mjs';
import {safeAssetUrl} from '../inspector.mjs';
const scalar=Math.fround;
const finite=value=>Number.isFinite(value)&&Math.abs(value)<=1048576;
export function parseStd(input){
  if(!(input instanceof Uint8Array)||input.length<156||input.length>8*1024*1024)throw new Error('Invalid STD byte range');
  const view=new DataView(input.buffer,input.byteOffset,input.byteLength);
  const requireRange=(offset,size)=>{if(!Number.isSafeInteger(offset)||offset<0||size<0||offset+size>input.length)throw new Error('Truncated original STD');};
  const word=offset=>{requireRange(offset,4);return view.getUint32(offset,true);};
  const half=offset=>{requireRange(offset,2);return view.getUint16(offset,true);};
  const floats=(offset,count)=>{requireRange(offset,count*4);const values=Array.from({length:count},(_,index)=>view.getFloat32(offset+index*4,true));if(!values.every(finite))throw new Error('Non-finite STD data');return values;};
  const count=half(0),faceOffset=word(4),scriptOffset=word(8);
  if(count<1||count>1024||faceOffset>=scriptOffset||scriptOffset+8>input.length)throw new Error('Invalid original STD tables');
  requireRange(16,128);const end=input.indexOf(0,16);
  const anm=new TextDecoder('ascii',{fatal:true}).decode(input.subarray(16,end));
  if(!/^stage_[a-z0-9]+\.anm$/.test(anm))throw new Error('Invalid original STD ANM name');
  const objects=[];
  for(let index=0;index<count;++index){
    const offset=word(144+index*4);requireRange(offset,28);let cursor=offset+28;
    const quads=[];
    while(word(cursor)!==0x0004ffff){
      requireRange(cursor,28);if(half(cursor)!==0||half(cursor+2)!==28||quads.length>1024)throw new Error('Unsupported STD primitive');
      quads.push({script:half(cursor+4),position:floats(cursor+8,3),size:floats(cursor+20,2)});cursor+=28;
      if(cursor>=faceOffset)throw new Error('STD primitive outside object section');
    }
    objects.push({id:half(offset),layer:half(offset+2),position:floats(offset+4,3),quads});
  }
  const instances=[];let cursor=faceOffset;
  while(half(cursor)!==65535){
    if(cursor+16>scriptOffset||instances.length>4096)throw new Error('Invalid STD instance extent');
    const object=half(cursor);if(object>=objects.length)throw new Error('STD instance object missing');
    instances.push({object,position:floats(cursor+4,3)});cursor+=16;
  }
  const commands=[];cursor=scriptOffset;
  while(cursor+8<=input.length){
    const size=half(cursor+6);if(size===65535)break;
    const opcode=half(cursor+4);requireRange(cursor,size);
    if(size<8||size%4||commands.length>65536)throw new Error('Invalid STD command extent');
    const integers=Array.from({length:(size-8)/4},(_,index)=>view.getInt32(cursor+8+index*4,true));
    const values=Array.from({length:(size-8)/4},(_,index)=>view.getFloat32(cursor+8+index*4,true));
    const indices=opcode===8?[1,2]:opcode===9?[3,4]:[3,5,10].includes(opcode)?Array.from({length:values.length-2},(_,index)=>index+2)
      :[2,4,6,7].includes(opcode)?values.map((_,index)=>index):[];
    if(indices.some(index=>!finite(values[index])))throw new Error('Non-finite original STD operand');
    commands.push({offset:cursor-scriptOffset,time:view.getInt32(cursor,true),opcode,integers,floats:values});
    cursor+=size;
  }
  if(cursor+8>input.length||half(cursor+6)!==65535||!commands.length)throw new Error('Missing STD terminator');
  return {anm,objects,instances,commands};
}

function ease(ratio,mode){
  if(mode===0)return ratio;
  if(mode===4)return scalar(1-(1-ratio)*(1-ratio));
  if(mode===8)return ratio;
  throw new Error(`Unimplemented original STD interpolation mode ${mode}`);
}
export class StageCamera {
  constructor(data){
    this.data=data;this.offsets=new Map(data.commands.map((command,index)=>[command.offset,index]));this.pc=0;this.clock=0;this.frame=-1;this.tracks=new Map();
    this.effect={mode:0,current:-1,frame:0,time:0,offset:[0,0,0],upX:0};
    this.camera={position:[0,0,-400],direction:[0,0,1],up:[0,1,0],fov:scalar(Math.PI/5),fog:[32,32,32,255,200,1100]};
    this.tick();
  }
  tick(){
    const state=this.camera;
    for(let budget=0;this.data.commands[this.pc]?.time<=this.clock;++budget){
      if(budget>=4096)throw new Error('STD instruction budget exceeded');
      const command=this.data.commands[this.pc++],integers=command.integers,values=command.floats;
      const vector=index=>values.slice(index,index+3);
      const fog=index=>{const color=integers[index]>>>0;return [color&255,(color>>>8)&255,(color>>>16)&255,color>>>24,values[index+1],values[index+2]];};
      if(command.opcode===0)continue;
      if(command.opcode===1){
        const target=this.offsets.get(integers[0]);if(target===undefined||integers[1]<0)throw new Error('Invalid original STD loop');
        this.pc=target;this.clock=integers[1];continue;
      }
      if([2,4,6].includes(command.opcode)){
        const key=command.opcode===2?'position':command.opcode===4?'direction':'up';state[key]=vector(0);this.tracks.delete(key);continue;
      }
      if(command.opcode===7){if(values[0]<=0||values[0]>=Math.PI)throw new Error('Invalid STD field of view');state.fov=values[0];continue;}
      if(command.opcode===8){state.fog=fog(0);this.tracks.delete('fog');continue;}
      if([3,5,9,10].includes(command.opcode)){
        const key=command.opcode===9?'fog':command.opcode===5?'direction':'position',hermite=command.opcode===10;
        if(integers[0]<=0||integers[0]>1000000)throw new Error('Invalid STD interpolation duration');
        this.tracks.set(key,{from:[...state[key]],to:key==='fog'?fog(2):vector(hermite?5:2),a:vector(2),b:vector(8),age:0,
          duration:integers[0],mode:integers[1],hermite});continue;
      }
      if(command.opcode===12){
        if(integers.length!==1||!Number.isInteger(integers[0])||integers[0]<0||integers[0]>255)
          throw new Error('Invalid original STD effect mode');
        this.effect.mode=integers[0];
        if(this.effect.mode===0)this.effect.offset=[0,0,0];
        this.effect.current=-1;this.effect.frame=0;this.effect.time=0;
        if([1,4,5,7].includes(this.effect.mode))
          throw new Error(`STD12 mode${this.effect.mode} consumer pending`);
        continue;
      }
      throw new Error(`Unimplemented actual STD opcode ${command.opcode}`);
    }
    for(const [key,track] of this.tracks){
      const ratio=scalar(Math.min(1,++track.age/track.duration)),blend=ease(ratio,track.mode);
      state[key]=track.from.map((from,index)=>{
        const value=track.hermite?(2*ratio**3-3*ratio**2+1)*from+(-2*ratio**3+3*ratio**2)*track.to[index]
          +(ratio**3-2*ratio**2+ratio)*track.a[index]+(ratio**3-ratio**2)*track.b[index]
          :scalar(from+scalar(scalar(track.to[index]-from)*blend));
        return key==='fog'&&index<4?Math.trunc(value):scalar(value);
      });
      if(track.age>=track.duration)this.tracks.delete(key);
    }
    this.applyEffect();
    ++this.clock;++this.frame;
  }
  applyEffect(){
    if(this.effect.mode!==6)return;
    const time=this.effect.time,phase=Math.PI-2*time*Math.PI/512,sine=Math.sin(phase);
    this.effect.offset[0]=scalar(-15*sine);
    this.effect.upX=scalar(-0.01*sine);
    this.effect.current=this.effect.frame;
    ++this.effect.frame;
    this.effect.time=scalar(this.effect.time+1);
    if(this.effect.frame>=512){this.effect.current=-1;this.effect.frame=0;this.effect.time=0;}
  }
  cameraView(){
    const view=structuredClone(this.camera);
    view.position=view.position.map((value,index)=>scalar(value+this.effect.offset[index]));
    if(this.effect.mode===6)view.up[0]=this.effect.upX;
    return view;
  }
  snapshot(){return {frame:this.frame,clock:this.clock,...structuredClone(this.camera),effect:structuredClone(this.effect),view:this.cameraView()};}
}
const cross=(first,second)=>[first[1]*second[2]-first[2]*second[1],first[2]*second[0]-first[0]*second[2],first[0]*second[1]-first[1]*second[0]];
const unit=vector=>{const length=Math.hypot(...vector);if(!length)throw new Error('Degenerate STD camera');return vector.map(value=>value/length);};
const dot=(first,second)=>first.reduce((total,value,index)=>total+value*second[index],0);
export class StageBackground {
  constructor(data,module,bank=8){
    this.data=data;this.module=module;this.bank=bank;this.camera=new StageCamera(data);this.frame=0;this.quads=[];
    this.layers=new Map(data.objects.map(object=>[object.id,object.layer]));
    for(const object of data.objects)for(const quad of object.quads){
      const key=this.quads.length+1;
      if(!module._th128_bind(bank,quad.script,key))throw new Error(module.UTF8ToString(module._th128_error()));
      this.quads.push({object:object.id,quad,key});
    }
  }
  tick(){
    if(!this.module._th128_tick())throw new Error(this.module.UTF8ToString(this.module._th128_error()));
    this.camera.tick();++this.frame;
  }
  snapshot(){return {frame:this.frame,camera:this.camera.snapshot(),objects:this.data.objects.length,instances:this.data.instances.length};}
  geometry(){
    const count=this.module._th128_draw_count();
    const error=this.module.UTF8ToString(this.module._th128_error());if(error)throw new Error(error);
    const poses=new Map(decodeDrawBuffer(this.module.HEAPU8.buffer,this.module._th128_draw_ptr(),count,this.module._th128_draw_stride()).map(record=>[record.id,record]));
    const camera=this.camera.cameraView(),forward=unit(camera.direction),right=unit(cross(camera.up,forward)),up=cross(forward,right);
    const aspect=384/448,vertical=1/Math.tan(camera.fov/2),near=30,far=1800,result=[];
    for(const instance of this.data.instances)for(const item of this.quads){
      if(item.object!==instance.object)continue;
      const pose=poses.get(item.key);if(!pose||!(pose.flags&3)||!pose.color)continue;
      if(pose.mode!==8)throw new Error('Unsupported original background ANM geometry');
      const cosine=Math.cos(pose.rotation),sine=Math.sin(pose.rotation),cosineX=Math.cos(pose.rx),sineX=Math.sin(pose.rx);
      const cosineY=Math.cos(pose.ry),sineY=Math.sin(pose.ry),vertices=[];
      const color=argbColor(pose.color);
      for(const [horizontal,vert,coordinateU,coordinateV] of [[-.5,-.5,0,0],[.5,-.5,1,0],[.5,.5,1,1],[-.5,.5,0,1]]){
        const localX=horizontal*item.quad.size[0],localY=vert*item.quad.size[1];
        const rotatedX=localX*cosine-localY*sine,rotatedY=localX*sine+localY*cosine;
        const tiltedY=rotatedY*cosineX,tiltedZ=rotatedY*sineX;
        const world=[rotatedX*cosineY+tiltedZ*sineY+instance.position[0]+item.quad.position[0]+pose.x,
          tiltedY+instance.position[1]+item.quad.position[1]+pose.y,
          -rotatedX*sineY+tiltedZ*cosineY+instance.position[2]+item.quad.position[2]+pose.z];
        const relative=world.map((value,index)=>value-camera.position[index]);
        const depth=dot(relative,forward),clipX=dot(relative,right)*vertical/aspect,clipY=dot(relative,up)*vertical;
        const fog=Math.min(1,Math.max(0,(depth-camera.fog[4])/(camera.fog[5]-camera.fog[4])));
        vertices.push([clipX,clipY,(far+near)/(far-near)*depth-2*far*near/(far-near),depth,
          pose.u0+(pose.u1-pose.u0)*coordinateU*pose.uvScaleX+pose.u,pose.v0+(pose.v1-pose.v0)*coordinateV*pose.uvScaleY+pose.v,
          ...color,fog]);
      }
      result.push({bank:this.bank,entry:pose.entry,blend:pose.blend,layer:this.layers.get(item.object),fog:camera.fog,vertices:[...vertices[0],...vertices[1],...vertices[2],...vertices[0],...vertices[2],...vertices[3]]});
    }
    result.sort((first,second)=>first.layer-second.layer);
    return result;
  }
}


export function portraitTextureAddressMode(bank,entry){
  if(bank===5&&entry?.name==='face/balloon.png')return 'balloon';
  return (bank===3||bank===7)&&typeof entry?.name==='string'&&entry.name.startsWith('face/')?'clamp':'repeat';
}

export class WebGlRenderer {
  constructor(canvas) {
    this.canvas = canvas;
    this.gl = canvas.getContext('webgl2', {alpha:false, antialias:false, premultipliedAlpha:false});
    if (!this.gl) throw new Error('需要支持 WebGL 2 的浏览器');
    this.textures = new Map();
    const shaders=[];
    const graphics = this.gl;
    const compile = (kind, source) => {
      const shader = graphics.createShader(kind);
      if(!shader){for(const item of shaders)graphics.deleteShader(item);graphics.deleteProgram(this.program);throw new Error('Cannot allocate shader');}
      shaders.push(shader);
      graphics.shaderSource(shader, source);
      graphics.compileShader(shader);
      if (!graphics.getShaderParameter(shader, graphics.COMPILE_STATUS)) {
        const reason=graphics.getShaderInfoLog(shader);
        for(const item of shaders)graphics.deleteShader(item);
        graphics.deleteProgram(this.program);
        throw new Error(reason);
      }
      return shader;
    };
    const program = graphics.createProgram();
    if(!program)throw new Error('Cannot allocate shader program');
    this.program=program;
    graphics.attachShader(program, compile(graphics.VERTEX_SHADER, `#version 300 es
      precision highp float;
      layout(location=0) in vec2 position;
      layout(location=1) in vec2 coordinate;
      layout(location=2) in vec4 color;
      out vec2 textureCoordinate;
      out vec4 vertexColor;
      void main(){gl_Position=vec4(position.x/320.0-1.0,1.0-position.y/240.0,0.0,1.0);textureCoordinate=coordinate;vertexColor=color;}`));
    graphics.attachShader(program, compile(graphics.FRAGMENT_SHADER, `#version 300 es
      precision highp float;
      uniform sampler2D image;
      uniform vec2 verticalClamp;
      uniform vec4 spriteClamp;
      in vec2 textureCoordinate;
      in vec4 vertexColor;
      out vec4 fragment;
      void main(){vec2 uv=clamp(textureCoordinate,spriteClamp.xy,spriteClamp.zw);fragment=texture(image,vec2(uv.x,clamp(uv.y,verticalClamp.x,verticalClamp.y)))*vertexColor;if(fragment.a<1.0/255.0)discard;}`));
    graphics.linkProgram(program);
    if (!graphics.getProgramParameter(program, graphics.LINK_STATUS)) {
      const reason=graphics.getProgramInfoLog(program);
      for(const shader of shaders)graphics.deleteShader(shader);
      graphics.deleteProgram(program);
      throw new Error(reason);
    }
    graphics.useProgram(program);
    this.program = program;
    for (const shader of graphics.getAttachedShaders(program)) graphics.deleteShader(shader);
    this.buffer = graphics.createBuffer();
    this.array = graphics.createVertexArray();
    graphics.bindVertexArray(this.array);
    graphics.bindBuffer(graphics.ARRAY_BUFFER, this.buffer);
    for (const [location, size, offset] of [[0,2,0],[1,2,8],[2,4,16]]) {
      graphics.enableVertexAttribArray(location);
      graphics.vertexAttribPointer(location,size,graphics.FLOAT,false,32,offset);
    }
    graphics.enable(graphics.BLEND);
    graphics.pixelStorei(graphics.UNPACK_PREMULTIPLY_ALPHA_WEBGL, false);
    graphics.pixelStorei(graphics.UNPACK_COLORSPACE_CONVERSION_WEBGL, graphics.NONE);
    this.verticalClampUniform=graphics.getUniformLocation(this.program,'verticalClamp');
    this.spriteClampUniform=graphics.getUniformLocation(this.program,'spriteClamp');
    this.spriteClamp=[-1048576,-1048576,1048576,1048576];
    this.verticalClamp=[-1048576,1048576];
    this.vertices = [];
    this.current = null;
    this.blend = -1;
    this.point = null;
  }

  setResolution(width,height){
    if(width!==1280||height!==960||this.disposed)throw new Error('Render output is fixed at1280x960');
    if(this.canvas.width===width&&this.canvas.height===height)return;
    for(const surface of this.textures.values())if(surface.dynamic&&surface.initialized){
      this.gl.deleteTexture(surface.texture);
      this.textureBytes=Math.max(0,(this.textureBytes??0)-(surface.captureWidth??this.canvas.width)*(surface.captureHeight??this.canvas.height)*3);
      surface.texture=null;surface.initialized=false;surface.captureWidth=surface.captureHeight=0;
    }
    this.canvas.width=width;this.canvas.height=height;this.current=null;this.point=null;
  }

  stageViewport(){
    if(this.canvas.width!==1280||this.canvas.height!==960)throw new Error('Physical output must be1280x960');
    return [256,32,768,896];
  }

  releaseTexture(key) {
    const previous=this.textures.get(key);
    if(!previous)return;
    if(this.current===previous){
      if(this.vertices?.length)this.flush();
      this.current=null;this.point=null;
    }
    if(previous.texture)this.gl.deleteTexture(previous.texture);
    const bytes=previous.dynamic
      ?previous.initialized?(previous.captureWidth??this.canvas.width)*(previous.captureHeight??this.canvas.height)*3:0
      :previous.texture?previous.allocationBytes??previous.width*previous.height*4:0;
    this.textureBytes=Math.max(0,(this.textureBytes??0)-bytes);
    this.textures.delete(key);
  }

  async loadBank(bank, metadata, base, {signal} = {}) {
    const graphics = this.gl;
    if(this.disposed||signal?.aborted)throw new DOMException('Aborted','AbortError');
    if(!Number.isSafeInteger(bank)||bank<0||!Array.isArray(metadata?.entries))throw new Error('Invalid texture bank');
    const activeKeys=new Set(),indices=new Set();
    for(const entry of metadata.entries){
      if(!Number.isSafeInteger(entry.index)||entry.index<0||indices.has(entry.index))throw new Error('Invalid texture entry identity');
      indices.add(entry.index);
      if(entry.texture||entry.name==='@R')activeKeys.add(`${bank}:${entry.index}`);
    }
    this.bankLoadOwners??=new Map();
    const loadOwner={};this.bankLoadOwners.set(bank,loadOwner);
    const requireLoadOwner=()=>{
      if(this.disposed||signal?.aborted||this.bankLoadOwners.get(bank)!==loadOwner)
        throw new DOMException('Aborted','AbortError');
    };
    for(const key of this.textures.keys())if(key.startsWith(`${bank}:`)&&!activeKeys.has(key))this.releaseTexture(key);
    for (const entry of metadata.entries) {
      requireLoadOwner();
      if(!entry.texture&&entry.name==='@R'){
        if(!Number.isInteger(entry.index)||entry.index<0||entry.width!==640||entry.height!==480)throw new Error('Unsupported dynamic render-target dimensions');
        if(this.disposed||signal?.aborted)throw new DOMException('Aborted','AbortError');
        const key=`${bank}:${entry.index}`,previous=this.textures.get(key);
        if(previous&&(!previous.dynamic||previous.width!==entry.width||previous.height!==entry.height))throw new Error('Dynamic render-target binding changed');
        if(!previous)this.textures.set(key,{texture:null,width:entry.width,height:entry.height,dynamic:true,flipY:true,initialized:false});
        continue;
      }
      if (!entry.texture) continue;
      const dimensions=[entry.width,entry.height,entry.texture.width,entry.texture.height];
      if(!dimensions.every(value=>Number.isInteger(value)&&value>0&&value<=8192))throw new Error('Invalid texture dimensions');
      if(this.disposed||signal?.aborted)throw new DOMException('Aborted','AbortError');
      const sourceUrl=safeAssetUrl(base,entry.texture.file),key=`${bank}:${entry.index}`;
      const addressMode=portraitTextureAddressMode(bank,entry);
      const sourceKey=JSON.stringify([sourceUrl,...dimensions,entry.texture.format,addressMode]);
      const previous=this.textures.get(key);
      if(previous?.sourceKey===sourceKey&&previous.texture)continue;
      const logicalBytes=Math.max(entry.width,entry.texture.width)*Math.max(entry.height,entry.texture.height)*4;
      if(logicalBytes>64*1024*1024)throw new Error('Texture memory budget exceeded');
      const maximum=graphics.getParameter(graphics.MAX_TEXTURE_SIZE);
      if(Math.max(entry.width,entry.texture.width)>maximum||Math.max(entry.height,entry.texture.height)>maximum)throw new Error('Texture exceeds GPU dimension limit');
      if(previous)this.releaseTexture(key);
      if((this.textureBytes??0)+logicalBytes>256*1024*1024)throw new Error('Texture memory budget exceeded');
      if (this.disposed || signal?.aborted) throw new DOMException('Aborted','AbortError');
      const response = await fetch(sourceUrl,{signal});
      if (!response.ok) throw new Error(`纹理读取失败：${entry.name}`);
      const bitmap = await createImageBitmap(await response.blob(), {premultiplyAlpha:'none',colorSpaceConversion:'none'});
      let texture=null;
      try {
      requireLoadOwner();
      if (bitmap.width !== entry.texture.width || bitmap.height !== entry.texture.height) throw new Error('THTX/PNG 尺寸不一致');
      if((this.textureBytes??0)+logicalBytes>256*1024*1024)throw new Error('Texture memory budget exceeded');
      texture = graphics.createTexture();
      if(!texture)throw new Error('Cannot allocate texture');
      graphics.bindTexture(graphics.TEXTURE_2D,texture);
      const width = Math.max(entry.width,bitmap.width), height = Math.max(entry.height,bitmap.height);
      const padding = new Uint8Array(width*height*4);
      if (entry.texture.format === 3) for (let offset=3;offset<padding.length;offset+=4) padding[offset]=255;
      graphics.texImage2D(graphics.TEXTURE_2D,0,graphics.RGBA,width,height,0,graphics.RGBA,graphics.UNSIGNED_BYTE,padding);
      graphics.texSubImage2D(graphics.TEXTURE_2D,0,0,0,graphics.RGBA,graphics.UNSIGNED_BYTE,bitmap);
      const wrapS=addressMode==='clamp'||(addressMode==='balloon'&&entry.texture.width!==16)?graphics.CLAMP_TO_EDGE:graphics.REPEAT;
      graphics.texParameteri(graphics.TEXTURE_2D,graphics.TEXTURE_WRAP_S,wrapS);
      graphics.texParameteri(graphics.TEXTURE_2D,graphics.TEXTURE_WRAP_T,addressMode==='clamp'?graphics.CLAMP_TO_EDGE:graphics.REPEAT);
      graphics.texParameteri(graphics.TEXTURE_2D,graphics.TEXTURE_MIN_FILTER,graphics.LINEAR);
      graphics.texParameteri(graphics.TEXTURE_2D,graphics.TEXTURE_MAG_FILTER,graphics.LINEAR);
      this.textures.set(key,{texture,width,height,sourceKey,allocationBytes:logicalBytes,balloon:addressMode==='balloon'});
      this.textureBytes=(this.textureBytes??0)+logicalBytes;
      texture=null;
      } finally {
        bitmap.close();
        if(texture)graphics.deleteTexture(texture);
      }
    }
  }

  bindNativeMenuRaster(){
    if(this.disposed)throw new Error('Native menu render target is disposed');
    const graphics=this.gl;
    if(this.nativeMenuRasterSurface){graphics.bindFramebuffer(graphics.FRAMEBUFFER,this.nativeMenuRasterSurface.framebuffer);return;}
    const texture=graphics.createTexture(),framebuffer=graphics.createFramebuffer();
    if(!texture||!framebuffer){if(texture)graphics.deleteTexture(texture);if(framebuffer)graphics.deleteFramebuffer(framebuffer);throw new Error('Cannot allocate native menu framebuffer');}
    try{
      graphics.bindTexture(graphics.TEXTURE_2D,texture);
      for(const parameter of [graphics.TEXTURE_WRAP_S,graphics.TEXTURE_WRAP_T])graphics.texParameteri(graphics.TEXTURE_2D,parameter,graphics.CLAMP_TO_EDGE);
      for(const parameter of [graphics.TEXTURE_MIN_FILTER,graphics.TEXTURE_MAG_FILTER])graphics.texParameteri(graphics.TEXTURE_2D,parameter,graphics.NEAREST);
      graphics.texImage2D(graphics.TEXTURE_2D,0,graphics.RGBA8,640,480,0,graphics.RGBA,graphics.UNSIGNED_BYTE,null);
      graphics.bindFramebuffer(graphics.FRAMEBUFFER,framebuffer);
      graphics.framebufferTexture2D(graphics.FRAMEBUFFER,graphics.COLOR_ATTACHMENT0,graphics.TEXTURE_2D,texture,0);
      if(graphics.checkFramebufferStatus(graphics.FRAMEBUFFER)!==graphics.FRAMEBUFFER_COMPLETE)throw new Error('Native menu framebuffer is incomplete');
      this.nativeMenuRasterSurface={texture,framebuffer};this.textureBytes=(this.textureBytes??0)+640*480*4;
    }catch(problem){graphics.bindFramebuffer(graphics.FRAMEBUFFER,null);graphics.deleteFramebuffer(framebuffer);graphics.deleteTexture(texture);throw problem;}
    finally{this.current=null;this.point=null;}
  }

  releaseNativeMenuRaster(){
    const surface=this.nativeMenuRasterSurface;if(!surface)return;
    this.gl.deleteFramebuffer(surface.framebuffer);this.gl.deleteTexture(surface.texture);
    this.nativeMenuRasterSurface=null;this.textureBytes=Math.max(0,(this.textureBytes??0)-640*480*4);
  }

  dispose() {
    this.disposed = true;
    this.releaseNativeMenuRaster();
    const graphics = this.gl;
    for (const item of this.textures.values()) if(item.texture)graphics.deleteTexture(item.texture);
    this.textures.clear();
    this.bankLoadOwners?.clear();
    this.textureBytes=0;
      if(this.solidTexture)graphics.deleteTexture(this.solidTexture);
      this.solidTexture=null;
    graphics.deleteBuffer(this.buffer);
    graphics.deleteVertexArray(this.array);
    graphics.deleteProgram(this.program);
    if(this.stageProgram){graphics.deleteProgram(this.stageProgram);graphics.deleteBuffer(this.stageBuffer);graphics.deleteVertexArray(this.stageArray);}
  }

  drawBackground(background){
    const graphics=this.gl;
    if(!this.stageProgram){
      const program=graphics.createProgram(),shaders=[];
      try{
        for(const [kind,source] of [[graphics.VERTEX_SHADER,`#version 300 es
          precision highp float;layout(location=0) in vec4 clipPosition;layout(location=1) in vec2 uv;
          layout(location=2) in vec4 tint;layout(location=3) in float fogAmount;
          out vec2 coordinate;out vec4 color;out float fog;
          void main(){gl_Position=clipPosition;coordinate=uv;color=tint;fog=fogAmount;}`],
          [graphics.FRAGMENT_SHADER,`#version 300 es
          precision highp float;uniform sampler2D image;uniform vec3 fogColor;
          in vec2 coordinate;in vec4 color;in float fog;out vec4 fragment;
          void main(){fragment=texture(image,coordinate)*color;if(fragment.a<1.0/255.0)discard;fragment.rgb=mix(fragment.rgb,fogColor,clamp(fog,0.0,1.0));}`]]){
          const shader=graphics.createShader(kind);shaders.push(shader);graphics.shaderSource(shader,source);graphics.compileShader(shader);
          if(!graphics.getShaderParameter(shader,graphics.COMPILE_STATUS))throw new Error(graphics.getShaderInfoLog(shader));
          graphics.attachShader(program,shader);
        }
        graphics.linkProgram(program);if(!graphics.getProgramParameter(program,graphics.LINK_STATUS))throw new Error(graphics.getProgramInfoLog(program));
        this.stageProgram=program;this.stageFog=graphics.getUniformLocation(program,'fogColor');
      }catch(problem){graphics.deleteProgram(program);throw problem;}
      finally{for(const shader of shaders)graphics.deleteShader(shader);}
      this.stageBuffer=graphics.createBuffer();this.stageArray=graphics.createVertexArray();
      graphics.bindVertexArray(this.stageArray);graphics.bindBuffer(graphics.ARRAY_BUFFER,this.stageBuffer);
      for(const [location,size,offset] of [[0,4,0],[1,2,16],[2,4,24],[3,1,40]]){
        graphics.enableVertexAttribArray(location);graphics.vertexAttribPointer(location,size,graphics.FLOAT,false,44,offset);
      }
    }
    const meshes=background.geometry();
    if(meshes.length>4096||meshes.some(mesh=>mesh.vertices.length!==66||!mesh.vertices.every(Number.isFinite)))throw new Error('Invalid original STD geometry');
    graphics.useProgram(this.stageProgram);graphics.bindVertexArray(this.stageArray);graphics.bindBuffer(graphics.ARRAY_BUFFER,this.stageBuffer);
    const viewport=this.stageViewport();graphics.viewport(...viewport);graphics.enable(graphics.SCISSOR_TEST);graphics.scissor(...viewport);
    graphics.clearDepth(1);graphics.clear(graphics.DEPTH_BUFFER_BIT);graphics.enable(graphics.DEPTH_TEST);graphics.depthFunc(graphics.LEQUAL);
    for(const mesh of meshes){
      const texture=this.textures.get(`${mesh.bank}:${mesh.entry}`);if(!texture)throw new Error('Original background texture unavailable');
      this.material(mesh.blend);graphics.depthMask(mesh.blend===0);
      graphics.uniform3fv(this.stageFog,[mesh.fog[2]/255,mesh.fog[1]/255,mesh.fog[0]/255]);
      graphics.bindTexture(graphics.TEXTURE_2D,texture.texture);graphics.bufferData(graphics.ARRAY_BUFFER,new Float32Array(mesh.vertices),graphics.STREAM_DRAW);
      graphics.drawArrays(graphics.TRIANGLES,0,6);
    }
    graphics.depthMask(true);graphics.disable(graphics.DEPTH_TEST);graphics.disable(graphics.SCISSOR_TEST);
    graphics.useProgram(this.program);graphics.bindVertexArray(this.array);graphics.bindBuffer(graphics.ARRAY_BUFFER,this.buffer);
    graphics.viewport(0,0,this.canvas.width,this.canvas.height);this.current=null;this.blend=-1;this.point=null;
  }

  flush() {
    if (!this.vertices.length) return;
    const graphics = this.gl;
    graphics.uniform2fv(this.verticalClampUniform,this.verticalClamp);
    graphics.uniform4fv(this.spriteClampUniform,this.spriteClamp);
    graphics.bindTexture(graphics.TEXTURE_2D,this.current.texture);
    graphics.bufferData(graphics.ARRAY_BUFFER,new Float32Array(this.vertices),graphics.STREAM_DRAW);
    graphics.drawArrays(graphics.TRIANGLES,0,this.vertices.length/8);
    this.vertices.length = 0;
  }

  material(mode) {
    const graphics = this.gl;
    const factors = [[graphics.SRC_ALPHA,graphics.ONE_MINUS_SRC_ALPHA],[graphics.SRC_ALPHA,graphics.ONE],
      [graphics.SRC_ALPHA,graphics.ONE],[graphics.ONE,graphics.ZERO],
      [graphics.ONE_MINUS_DST_COLOR,graphics.ONE_MINUS_SRC_ALPHA],[graphics.DST_COLOR,graphics.ZERO],
      [graphics.ONE_MINUS_SRC_COLOR,graphics.ONE_MINUS_SRC_ALPHA]];
    if (!factors[mode]) throw new Error(`Unimplemented blend mode ${mode}`);
    if (this.blend !== mode) {
      this.flush();
      graphics.blendEquation(mode === 2 ? graphics.FUNC_REVERSE_SUBTRACT : graphics.FUNC_ADD);
      graphics.blendFunc(...factors[mode]);
      this.blend = mode;
    }
  }

  captureStage() {
    const graphics=this.gl,surface=this.textures.get('6:2');
    const viewport=this.stageViewport(),width=this.canvas.width,height=this.canvas.height;
    if(!surface?.dynamic)throw new Error('Actual text@R stage surface is unavailable');
    this.flush();
    let provisional=null;
    try{
      const texture=surface.texture??(provisional=graphics.createTexture());
      if(!texture)throw new Error('Cannot allocate dynamic stage texture');
      graphics.bindTexture(graphics.TEXTURE_2D,texture);
      if(surface.initialized)graphics.copyTexSubImage2D(graphics.TEXTURE_2D,0,0,0,0,0,width,height);
      else{
        const required=width*height*3;
        if((this.textureBytes??0)+required>256*1024*1024)throw new Error('Texture memory budget exceeded');
        graphics.copyTexImage2D(graphics.TEXTURE_2D,0,graphics.RGB,0,0,width,height,0);
      }
      const failure=graphics.getError();
      if(failure!==graphics.NO_ERROR)throw new Error(`Dynamic stage capture failed: WebGL ${failure}`);
      for(const parameter of [graphics.TEXTURE_WRAP_S,graphics.TEXTURE_WRAP_T])graphics.texParameteri(graphics.TEXTURE_2D,parameter,graphics.REPEAT);
      for(const parameter of [graphics.TEXTURE_MIN_FILTER,graphics.TEXTURE_MAG_FILTER])graphics.texParameteri(graphics.TEXTURE_2D,parameter,graphics.LINEAR);
      if(!surface.initialized)this.textureBytes=(this.textureBytes??0)+width*height*3;
      surface.captureWidth=width;surface.captureHeight=height;
      surface.texture=texture;surface.initialized=true;provisional=null;
    }finally{if(provisional)graphics.deleteTexture(provisional);}
    this.current=null;this.point=null;
    graphics.enable(graphics.SCISSOR_TEST);graphics.scissor(...viewport);
    graphics.clearColor(0,0,0,1);graphics.clear(graphics.COLOR_BUFFER_BIT);graphics.disable(graphics.SCISSOR_TEST);
  }

  render(records,{world=false,background=null,shake=[0,0],overlay=false,nativeMenu=false}={}) {
    if(!Array.isArray(records)||records.length>65536)throw new Error('Invalid draw-record budget');
    if(nativeMenu&&(world||background||overlay||this.canvas.width!==1280||this.canvas.height!==960))throw new Error('Invalid native menu raster context');
    if(world)records=records.map(record=>applyWorldShake(record,shake));
    const meshes=new Map(),fans=new Map(),lines=new Map();
    let meshPoints=0,mainCount=0,priority=-1,mainSeen=false;
    for(const record of records){
      if(world){
        const next=nativeRecordPriority(record);
        if(next<priority)throw new Error('World records must follow native draw priorities');
        priority=next;
      }
      if(record.stageSurface==='main'){
        if(!world||record.bank!==6||record.entry!==2||record.script!==82||record.sprite!==83||record.mode!==0||record.blend!==3||record.layer!==29)
          throw new Error('Invalid actual Main82 stage compositor');
        ++mainCount;mainSeen=true;
      }
      if(record.geometry){
        if(record.geometry.kind==='triangle-strip'){
          if(!world||!mainSeen||record.bank!==6||record.entry!==2||record.layer!==29||record.blend!==0)
            throw new Error('Original526 rows require the actual preceding Main82 stage compositor');
          meshes.set(record,triangleStripVertices(record,{flipY:true}));
        }else if(record.geometry.kind==='circle-fan'){
          if(!world||record.mode!==16||![6,16].includes(record.layer)||record.width<=0||record.height<=0)throw new Error('Invalid original mode16 geometry context');
          fans.set(record,circleFanVertices(record));
        }else if(record.geometry.kind==='circle-line'){
          if(!world||record.mode!==17||record.layer!==6)throw new Error('Invalid original level-up circle-line context');lines.set(record,circleLineVertices(record));
        }else if(record.geometry.kind==='anm-ring'){
          if(!world||!((record.bank===5&&record.layer===17&&(record.mode===9||record.mode===14))||(record.bank===0&&record.layer===4&&record.mode===9)))throw new Error('Invalid original Boss/spell ring context');
          meshes.set(record,ringStripVertices(record));
        }else throw new Error('Unimplemented original custom geometry');
        meshPoints+=record.geometry.vertices.length;
        if(meshPoints>65536)throw new Error('World mesh vertex budget exceeded');
      }
    }
    if(mainCount>1)throw new Error('Ambiguous Main82 stage compositor');
    const graphics = this.gl;
    if(nativeMenu)this.bindNativeMenuRaster();
    try{
    const renderWidth=nativeMenu?640:this.canvas.width,renderHeight=nativeMenu?480:this.canvas.height;
    graphics.bindVertexArray(this.array);
    graphics.bindBuffer(graphics.ARRAY_BUFFER,this.buffer);
    graphics.viewport(0,0,renderWidth,renderHeight);
    graphics.disable(graphics.SCISSOR_TEST);
    graphics.clearColor(0,0,0,1);
    graphics.clear(graphics.COLOR_BUFFER_BIT);
    this.current = null;
    this.blend = -1;
    this.vertices.length = 0;
    if(background)this.drawBackground(background);
    let captured=false;
    for (const record of records) {
      if(mainCount&&!captured&&nativeRecordPriority(record)>=12){this.captureStage();captured=true;}
      if(fans.has(record)||lines.has(record)){
        const fan=fans.get(record)??lines.get(record);if(record.geometry.vertices.every(vertex=>(vertex.color>>>24)===0))continue;
        this.material(record.blend);this.flush();
        if(!this.solidTexture){
          const texture=graphics.createTexture();if(!texture)throw new Error('Cannot allocate original untextured ANM material');
          graphics.bindTexture(graphics.TEXTURE_2D,texture);graphics.texParameteri(graphics.TEXTURE_2D,graphics.TEXTURE_WRAP_S,graphics.CLAMP_TO_EDGE);
          graphics.texParameteri(graphics.TEXTURE_2D,graphics.TEXTURE_WRAP_T,graphics.CLAMP_TO_EDGE);graphics.texParameteri(graphics.TEXTURE_2D,graphics.TEXTURE_MIN_FILTER,graphics.NEAREST);
          graphics.texParameteri(graphics.TEXTURE_2D,graphics.TEXTURE_MAG_FILTER,graphics.NEAREST);
          graphics.texImage2D(graphics.TEXTURE_2D,0,graphics.RGBA,1,1,0,graphics.RGBA,graphics.UNSIGNED_BYTE,new Uint8Array([255,255,255,255]));
          const failure=graphics.getError();if(failure!==graphics.NO_ERROR){graphics.deleteTexture(texture);throw new Error(`Untextured ANM material allocation failed: WebGL ${failure}`);}
          this.solidTexture=texture;
        }
        graphics.bindVertexArray(this.array);graphics.bindBuffer(graphics.ARRAY_BUFFER,this.buffer);graphics.bindTexture(graphics.TEXTURE_2D,this.solidTexture);
        graphics.texParameteri(graphics.TEXTURE_2D,graphics.TEXTURE_MIN_FILTER,graphics.NEAREST);graphics.texParameteri(graphics.TEXTURE_2D,graphics.TEXTURE_MAG_FILTER,graphics.NEAREST);
        graphics.bufferData(graphics.ARRAY_BUFFER,fan,graphics.STREAM_DRAW);if(lines.has(record))graphics.lineWidth(this.canvas.width/640);graphics.drawArrays(lines.has(record)?graphics.LINE_STRIP:graphics.TRIANGLES,0,fan.length/8);
        this.current=null;this.point=null;continue;
      }
      if ((record.color>>>24) === 0 || record.width === 0 || record.height === 0) continue;
      if (![record.u,record.v,record.uvScaleX,record.uvScaleY,record.u0,record.v0,record.u1,record.v1].every(Number.isFinite)) {
        throw new Error('Non-finite texture coordinates');
      }
      const texture = this.textures.get(`${record.bank}:${record.entry}`);
      if (!texture) throw new Error(`没有绘制纹理：bank=${record.bank}, entry=${record.entry}`);
      if(texture.dynamic&&(!texture.initialized||record.bank!==6||record.entry!==2))throw new Error('Dynamic texture has no established current-frame capture');
      this.material(record.blend);
      // Original static HUD texels are replicated at integer display scale (case212).
      const nativeScale=renderWidth/640;
      const unitHudTexels=(record.bank===5||record.bank===11)&&!record.geometry&&record.mode===0
        &&nativeScale>1&&Number.isInteger(nativeScale)&&this.canvas.height/480===nativeScale
        &&record.sx===1&&record.sy===1&&record.rotation===0&&(record.rx??0)===0&&(record.ry??0)===0
        &&record.u===0&&record.v===0&&record.uvScaleX===1&&record.uvScaleY===1&&!texture.dynamic
        &&Math.abs(Math.abs(record.u1-record.u0)*texture.width-record.width)<1e-5
        &&Math.abs(Math.abs(record.v1-record.v0)*texture.height-record.height)<1e-5;
      const point = Boolean(record.flags2 & 4)||unitHudTexels;
      const staticAtlas=!record.geometry&&((record.bank===5&&record.entry===0&&record.sprite>=0&&record.sprite<=56)||(record.bank===11&&record.entry===0))
        &&record.u===0&&record.v===0&&record.uvScaleX===1&&record.uvScaleY===1;
      const spriteClamp=staticAtlas
        ?[Math.min(record.u0,record.u1)+.5/texture.width,Math.min(record.v0,record.v1)+.5/texture.height,Math.max(record.u0,record.u1)-.5/texture.width,Math.max(record.v0,record.v1)-.5/texture.height]
        :[-1048576,-1048576,1048576,1048576];
      const verticalClamp=texture.balloon&&record.script>=138&&record.script<=155
        ?[Math.min(record.v0,record.v1)+0.5/texture.height,Math.max(record.v0,record.v1)-0.5/texture.height]:[-1048576,1048576];
      if (this.current !== texture || point !== this.point || verticalClamp[0]!==this.verticalClamp[0] || verticalClamp[1]!==this.verticalClamp[1]||spriteClamp.some((value,index)=>value!==this.spriteClamp?.[index])) {
        this.flush();
        this.current = texture;
        this.point = point;
        this.verticalClamp=verticalClamp;
        this.spriteClamp=spriteClamp;
        graphics.bindTexture(graphics.TEXTURE_2D,texture.texture);
        graphics.texParameteri(graphics.TEXTURE_2D,graphics.TEXTURE_MIN_FILTER,point?graphics.NEAREST:graphics.LINEAR);
        graphics.texParameteri(graphics.TEXTURE_2D,graphics.TEXTURE_MAG_FILTER,point?graphics.NEAREST:graphics.LINEAR);
      }
      if(meshes.has(record)){this.vertices.push(...meshes.get(record));continue;}
      const positions = quadVertices(record), color = argbColor(record.color);
      const left = record.u0 + record.u, right = record.u0+(record.u1-record.u0)*record.uvScaleX+record.u;
      const top = record.v0 + record.v, bottom = record.v0+(record.v1-record.v0)*record.uvScaleY+record.v;
      const coordinates = [[left,texture.flipY?1-top:top],[right,texture.flipY?1-top:top],
        [right,texture.flipY?1-bottom:bottom],[left,texture.flipY?1-bottom:bottom]];
      for (const index of [0,1,2,0,2,3]) this.vertices.push(...positions[index],...coordinates[index],...color);
    }
    this.flush();
    if(nativeMenu){
      graphics.bindFramebuffer(graphics.READ_FRAMEBUFFER,this.nativeMenuRasterSurface.framebuffer);
      graphics.bindFramebuffer(graphics.DRAW_FRAMEBUFFER,null);
      graphics.blitFramebuffer(0,0,640,480,0,0,this.canvas.width,this.canvas.height,graphics.COLOR_BUFFER_BIT,graphics.NEAREST);
    }
    const error = graphics.getError();
    if (error !== graphics.NO_ERROR) throw new Error(`WebGL 错误 ${error}`);
    }finally{if(nativeMenu){graphics.bindFramebuffer(graphics.FRAMEBUFFER,null);graphics.viewport(0,0,this.canvas.width,this.canvas.height);this.current=null;this.point=null;}}
  }
}
