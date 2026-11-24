import {fetchResource} from '../resource-fetch.mjs';
import {argbColor, quadVertices, nativeRecordPriority,applyWorldShake,triangleStripVertices,triangleStripCorners,circleFanVertices,circleLineVertices,ringStripVertices,ringStripCorners,decodeDrawBuffer,nativeQuadColors,selector3QuadVertices} from './draw.mjs';
import {safeAssetUrl} from '../inspector.mjs';
const scalar=Math.fround;
const finite=value=>Number.isFinite(value)&&Math.abs(value)<=1048576;
const unboundedSpriteClamp=Object.freeze([-1048576,-1048576,1048576,1048576]);
const unboundedVerticalClamp=Object.freeze([-1048576,1048576]);
const maxIndexedVertices=65535; // 0xffff is WebGL 2's fixed primitive restart index.
function writeQuadIndices(indices,offset,first){
  indices[offset]=first;indices[offset+1]=first+1;indices[offset+2]=first+2;
  indices[offset+3]=first;indices[offset+4]=first+2;indices[offset+5]=first+3;
}
function roundQuadEdge(value){const lower=Math.floor(value);return value-lower===.5?lower+(Math.abs(lower%2)===1?1:0):Math.round(value);}
function appendQuad(renderer,record,texture,colors){
  const positions=renderer.quadPositions??=new Float64Array(8),mode=record.mode;
  if(mode!==0&&mode!==1){
    const points=quadVertices(record);for(let i=0;i<4;i++){positions[i*2]=points[i][0];positions[i*2+1]=points[i][1];}
  }else{
    const rotation=record.rotation??0;
    if(!Number.isFinite(rotation))throw new Error('Non-finite draw rotation');
    const x=Math.fround(record.x),y=Math.fround(record.y),width=Math.fround(record.width*record.sx),height=Math.fround(record.height*record.sy);
    if(!Number.isFinite(x)||!Number.isFinite(y)||!Number.isFinite(width)||!Number.isFinite(height))throw new Error('Non-finite draw geometry');
    if(mode===0){
      const interpolated=record.presentationInterpolated===true;
      let left=record.anchorX===1?x:record.anchorX===2?Math.fround(x-width):Math.fround(x-width/2);
      let top=record.anchorY===1?y:record.anchorY===2?Math.fround(y-height):Math.fround(y-height/2);
      if(!interpolated){if(record.anchorX!==1&&record.anchorX!==2)left=Math.floor(left);if(record.anchorY!==1&&record.anchorY!==2)top=Math.floor(top);}
      let right=record.anchorX===2?x:Math.fround(left+width),bottom=record.anchorY===2?y:Math.fround(top+height);
      if(!interpolated){
        left=roundQuadEdge(left);top=roundQuadEdge(top);right=roundQuadEdge(right);bottom=roundQuadEdge(bottom);
      }
      positions[0]=left;positions[1]=top;positions[2]=right;positions[3]=top;
      positions[4]=right;positions[5]=bottom;positions[6]=left;positions[7]=bottom;
    }else{
      const left=record.anchorX===1?0:record.anchorX===2?-width:Math.fround(-width/2);
      const top=record.anchorY===1?0:record.anchorY===2?-height:Math.fround(-height/2);
      const right=Math.fround(left+width),bottom=Math.fround(top+height),cosine=Math.fround(Math.cos(rotation)),sine=Math.fround(Math.sin(rotation));
      for(let i=0;i<4;i++){
        const horizontal=i===0||i===3?left:right,vertical=i<2?top:bottom;
        const px=Math.fround(x+horizontal*cosine-vertical*sine),py=Math.fround(y+horizontal*sine+vertical*cosine);
        if(!Number.isFinite(px)||!Number.isFinite(py))throw new Error('Non-finite projected draw coordinates');
        positions[i*2]=px;positions[i*2+1]=py;
      }
    }
  }
  const left=record.u0+record.u,right=record.u0+(record.u1-record.u0)*record.uvScaleX+record.u;
  const top=record.v0+record.v,bottom=record.v0+(record.v1-record.v0)*record.uvScaleY+record.v;
  let vertices,at;
  const indexed=renderer.spriteStream?.indexed;
  if(indexed){at=renderer.reserveIndexedVertices(4);vertices=renderer.extendSpriteStream(32);renderer.appendQuadIndices(at/8);}
  else if(renderer.spriteStream){at=renderer.spriteStream.length;vertices=renderer.extendSpriteStream(48);}
  else{vertices=renderer.vertices;at=vertices.length;vertices.length=at+48;}
  for(let n=0;n<(indexed?4:6);n++){
    const i=indexed||n<3?n:n===3?0:n===4?2:3,color=colors[i];
    vertices[at++]=positions[i*2];vertices[at++]=positions[i*2+1];
    vertices[at++]=i===0||i===3?left:right;
    const v=i<2?top:bottom;vertices[at++]=texture.flipY?1-v:v;
    vertices[at++]=color[0];vertices[at++]=color[1];vertices[at++]=color[2];vertices[at++]=color[3];
  }
}
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
    const aspect=422/480,vertical=1/Math.tan(camera.fov/2),near=30,far=1800,result=[];
    for(const [instanceIndex,instance] of this.data.instances.entries())for(const item of this.quads){
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
      result.push({presentationId:instanceIndex+':'+item.key,bank:this.bank,entry:pose.entry,blend:pose.blend,layer:this.layers.get(item.object),fog:camera.fog,vertices:[...vertices[0],...vertices[1],...vertices[2],...vertices[0],...vertices[2],...vertices[3]]});
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
    this.indexedSprites=true;
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
      this.textureBytes=Math.max(0,(this.textureBytes??0)-(surface.originalPauseCapture?512*512*2:(surface.captureWidth??this.canvas.width)*(surface.captureHeight??this.canvas.height)*3));
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
    const bytes=previous.originalPauseCapture
      ?previous.initialized?512*512*2:0:previous.dynamic
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
      if(entry.texture||entry.name==='@R'||bank===6&&entry.index===1&&entry.name==='@')activeKeys.add(`${bank}:${entry.index}`);
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
      if(!entry.texture&&entry.name==='@'&&bank===6&&entry.index===1){
        if(entry.width!==512||entry.height!==512||entry.format!==3)throw new Error('Original pause capture texture descriptor differs');
        const key='6:1',previous=this.textures.get(key);
        if(previous&&!previous.originalPauseCapture)throw new Error('Original pause capture owner changed');
        if(previous)this.releaseTexture(key);
        this.textures.set(key,{texture:null,width:512,height:512,dynamic:true,flipY:true,initialized:false,originalPauseCapture:true});
        continue;
      }
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
      const response = await fetchResource(sourceUrl,{signal});
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

  async uploadOriginalMusicText(request,bitmap,expectedSHA256){
    if(this.disposed||!request||![[24,22],[6,0]].some(pair=>pair[0]===request.bank&&pair[1]===request.entry)
      ||request.format!==5||request.texture?.[0]!==512||request.texture?.[1]!==512
      ||!Number.isInteger(request.epoch)||request.epoch<=0||!Array.isArray(request.rect)||request.rect.length!==4
      ||request.rect.some(value=>!Number.isInteger(value))||request.rect[0]!==0||request.rect[2]!==512
      ||request.rect[1]<0||![17,18].includes(request.rect[3])||request.rect[1]+request.rect[3]>512
      ||bitmap?.width!==512||bitmap?.height!==request.rect[3]||!/^[a-f0-9]{64}$/.test(expectedSHA256))
      throw new Error('Original Music text upload descriptor rejected');
    this.flush();
    const gl=this.gl,key=request.bank+':'+request.entry,owner=this.bankLoadOwners?.get(request.bank);
    if(!owner)throw new Error('Original Music text bank has no load owner');
    let surface=this.textures.get(key);
    if(surface&&(!surface.originalMusicText||surface.epoch!==request.epoch||surface.owner!==owner)){
      this.releaseTexture(key);surface=null;
    }
    const previousFramebuffer=gl.getParameter(gl.FRAMEBUFFER_BINDING),previousFlip=gl.getParameter(gl.UNPACK_FLIP_Y_WEBGL),
      previousPremultiply=gl.getParameter(gl.UNPACK_PREMULTIPLY_ALPHA_WEBGL);
    let framebuffer=null;
    try{
      if(!surface){
        const texture=gl.createTexture();if(!texture)throw new Error('Music text texture allocation failed');
        gl.bindTexture(gl.TEXTURE_2D,texture);
        gl.texImage2D(gl.TEXTURE_2D,0,gl.RGBA,512,512,0,gl.RGBA,gl.UNSIGNED_BYTE,new Uint8Array(512*512*4));
        gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_WRAP_S,gl.CLAMP_TO_EDGE);gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_WRAP_T,gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_MIN_FILTER,gl.LINEAR);gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_MAG_FILTER,gl.LINEAR);
        surface={texture,width:512,height:512,allocationBytes:512*512*4,originalMusicText:true,epoch:request.epoch,owner,flipY:false};
        this.textures.set(key,surface);this.textureBytes=(this.textureBytes??0)+surface.allocationBytes;
      }
      gl.bindTexture(gl.TEXTURE_2D,surface.texture);
      gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL,false);gl.pixelStorei(gl.UNPACK_PREMULTIPLY_ALPHA_WEBGL,false);
      if(bitmap.pixels instanceof Uint8Array&&bitmap.pixels.length===512*request.rect[3]*4)
        gl.texSubImage2D(gl.TEXTURE_2D,0,0,request.rect[1],512,request.rect[3],gl.RGBA,gl.UNSIGNED_BYTE,bitmap.pixels);
      else gl.texSubImage2D(gl.TEXTURE_2D,0,0,request.rect[1],gl.RGBA,gl.UNSIGNED_BYTE,bitmap);
      framebuffer=gl.createFramebuffer();if(!framebuffer)throw new Error('Music text readback framebuffer allocation failed');
      gl.bindFramebuffer(gl.FRAMEBUFFER,framebuffer);
      gl.framebufferTexture2D(gl.FRAMEBUFFER,gl.COLOR_ATTACHMENT0,gl.TEXTURE_2D,surface.texture,0);
      if(gl.checkFramebufferStatus(gl.FRAMEBUFFER)!==gl.FRAMEBUFFER_COMPLETE)throw new Error('Music text readback framebuffer incomplete');
      const pixels=new Uint8Array(512*request.rect[3]*4);
      gl.readPixels(0,request.rect[1],512,request.rect[3],gl.RGBA,gl.UNSIGNED_BYTE,pixels);
      if(gl.getError()!==gl.NO_ERROR)throw new Error('Original Music text upload/readback rejected by WebGL');
      const digest=Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',pixels)),value=>value.toString(16).padStart(2,'0')).join('');
      if(this.disposed||this.bankLoadOwners.get(request.bank)!==owner||this.textures.get(key)!==surface)
        throw new DOMException('Music text upload ownership changed','AbortError');
      if(digest!==expectedSHA256)throw new Error('Original Music texture full-region readback differs');
      return {bank:request.bank,entry:request.entry,epoch:request.epoch,rect:request.rect.slice(),readbackSHA256:digest};
    }finally{
      gl.bindFramebuffer(gl.FRAMEBUFFER,previousFramebuffer);if(framebuffer)gl.deleteFramebuffer(framebuffer);
      gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL,previousFlip);gl.pixelStorei(gl.UNPACK_PREMULTIPLY_ALPHA_WEBGL,previousPremultiply);
      this.current=null;this.point=null;
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
    if(this.staticQuadBuffer)graphics.deleteBuffer(this.staticQuadBuffer);
    if(this.mixedIndexBuffer)graphics.deleteBuffer(this.mixedIndexBuffer);
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
    const viewport=this.stageViewport();graphics.viewport(218,0,844,960);graphics.enable(graphics.SCISSOR_TEST);graphics.scissor(...viewport);
    graphics.depthMask(true);graphics.clearDepth(1);graphics.clear(graphics.DEPTH_BUFFER_BIT);graphics.enable(graphics.DEPTH_TEST);graphics.depthFunc(graphics.LEQUAL);
    const needed=meshes.length*66;
    if(!this.stageVertices||this.stageVertices.length<needed)this.stageVertices=new Float32Array(needed);
    const groups=[];
    for(let index=0;index<meshes.length;++index){
      const mesh=meshes[index];
      const texture=this.textures.get(`${mesh.bank}:${mesh.entry}`);if(!texture)throw new Error('Original background texture unavailable');
      this.stageVertices.set(mesh.vertices,index*66);
      const fog=[mesh.fog[2]/255,mesh.fog[1]/255,mesh.fog[0]/255],last=groups.at(-1);
      if(last&&last.texture===texture&&last.blend===mesh.blend&&last.fog.every((value,i)=>value===fog[i]))last.count+=6;
      else groups.push({texture,blend:mesh.blend,fog,start:index*6,count:6});
    }
    if(needed)graphics.bufferData(graphics.ARRAY_BUFFER,this.stageVertices.subarray(0,needed),graphics.STREAM_DRAW);
    for(const group of groups){
      this.material(group.blend);graphics.depthMask(group.blend===0);
      graphics.uniform3fv(this.stageFog,group.fog);
      graphics.bindTexture(graphics.TEXTURE_2D,group.texture.texture);
      graphics.drawArrays(graphics.TRIANGLES,group.start,group.count);
    }
    graphics.depthMask(true);graphics.disable(graphics.DEPTH_TEST);graphics.disable(graphics.SCISSOR_TEST);
    graphics.useProgram(this.program);graphics.bindVertexArray(this.array);graphics.bindBuffer(graphics.ARRAY_BUFFER,this.buffer);
    graphics.viewport(0,0,this.canvas.width,this.canvas.height);this.current=null;this.blend=-1;this.point=null;
  }

  flush() {
    const stream=this.spriteStream;
    if(stream){
      const length=stream.indexed?stream.indexLength:stream.length/8,count=length-stream.first;
      if(count){
        stream.ranges.push({first:stream.first,count,texture:this.current,point:this.point,
          vertical:this.verticalClamp,sprite:this.spriteClamp,blend:this.blend});
        stream.first=length;
      }
      return;
    }
    if (!this.vertices.length) return;
    const graphics = this.gl;
    graphics.uniform2fv(this.verticalClampUniform,this.verticalClamp);
    graphics.uniform4fv(this.spriteClampUniform,this.spriteClamp);
    graphics.bindTexture(graphics.TEXTURE_2D,this.current.texture);
    const length=this.vertices.length;
    if(!this.batchUpload||this.batchUpload.length<length)
      this.batchUpload=new Float32Array(Math.max(length,(this.batchUpload?.length??0)*2,256));
    this.batchUpload.set(this.vertices);
    graphics.bufferData(graphics.ARRAY_BUFFER,this.batchUpload.subarray(0,length),graphics.STREAM_DRAW);
    graphics.drawArrays(graphics.TRIANGLES,0,this.vertices.length/8);
    this.vertices.length = 0;
  }

  extendSpriteStream(count){
    const stream=this.spriteStream,end=stream.length+count;
    if(!this.spriteStreamUpload||this.spriteStreamUpload.length<end){
      const previous=this.spriteStreamUpload;
      this.spriteStreamUpload=new Float32Array(Math.max(end,(previous?.length??0)*2,4096));
      if(previous&&stream.length)this.spriteStreamUpload.set(previous.subarray(0,stream.length));
    }
    stream.length=end;return this.spriteStreamUpload;
  }
  reserveIndexedVertices(count){
    const stream=this.spriteStream;
    if(stream.length/8+count>maxIndexedVertices){this.flush();this.submitSpriteStream();}
    return stream.length;
  }
  quadIndexTable(count){
    if(!this.quadIndices||this.quadIndices.length<count){
      const quads=Math.min(maxIndexedVertices>>2,Math.max(count/6,(this.quadIndices?.length??0)/3,256));
      this.quadIndices=new Uint16Array(quads*6);
      for(let i=0;i<quads;++i)writeQuadIndices(this.quadIndices,i*6,i*4);
    }
    return this.quadIndices;
  }
  mixedIndices(count){
    if(!this.spriteMixedIndices||this.spriteMixedIndices.length<count){
      const old=this.spriteMixedIndices;
      this.spriteMixedIndices=new Uint16Array(Math.max(count,(old?.length??0)*2,1024));
      if(old&&this.spriteStream.mixed)this.spriteMixedIndices.set(old.subarray(0,this.spriteStream.indexLength));
    }
    return this.spriteMixedIndices;
  }
  appendQuadIndices(first){
    const stream=this.spriteStream,offset=stream.indexLength;
    if(stream.mixed)writeQuadIndices(this.mixedIndices(offset+6),offset,first);
    stream.indexLength+=6;
  }
  appendIndexedStrip(corners){
    const count=corners.length/8,at=this.reserveIndexedVertices(count),first=at/8;
    this.extendSpriteStream(corners.length).set(corners,at);
    const stream=this.spriteStream,offset=stream.indexLength,length=offset+(count-2)*3;
    const indices=this.mixedIndices(length);
    if(!stream.mixed&&offset)indices.set(this.quadIndexTable(offset).subarray(0,offset));
    stream.mixed=true;stream.indexLength=length;
    let out=offset;
    for(let i=2;i<count;++i){indices[out++]=first+(i&1?i-1:i-2);indices[out++]=first+(i&1?i-2:i-1);indices[out++]=first+i;}
  }

  // Submit one upload for each consecutive stretch of textured draws. Draws
  // retain their authored order and separate WebGL blend/depth settings.
  submitSpriteStream(){
    const stream=this.spriteStream;if(!stream?.ranges.length)return;
    const graphics=this.gl;
    graphics.bufferData(graphics.ARRAY_BUFFER,this.spriteStreamUpload.subarray(0,stream.length),graphics.STREAM_DRAW);
    if(stream.indexed){
      if(stream.mixed){
        const buf=this.mixedIndexBuffer??=graphics.createBuffer();if(!buf)throw new Error('Cannot allocate sprite indices');
        graphics.bindBuffer(graphics.ELEMENT_ARRAY_BUFFER,buf);
        graphics.bufferData(graphics.ELEMENT_ARRAY_BUFFER,this.spriteMixedIndices.subarray(0,stream.indexLength),graphics.STREAM_DRAW);
      }else{
        const buf=this.staticQuadBuffer??=graphics.createBuffer();if(!buf)throw new Error('Cannot allocate quad indices');
        graphics.bindBuffer(graphics.ELEMENT_ARRAY_BUFFER,buf);
        const table=this.quadIndexTable(stream.indexLength);
        if(this.uploadedQuadIndices!==table){graphics.bufferData(graphics.ELEMENT_ARRAY_BUFFER,table,graphics.STATIC_DRAW);this.uploadedQuadIndices=table;}
      }
    }
    let texture=null,point=null,vertical=null,sprite=null;
    const filterByTexture=new Map();
    for(const draw of stream.ranges){
      if(stream.gpuBlend!==draw.blend){this.applyBlend(draw.blend);stream.gpuBlend=draw.blend;}
      if(draw.texture!==texture){texture=draw.texture;graphics.bindTexture(graphics.TEXTURE_2D,texture.texture);point=filterByTexture.get(texture);}
      if(draw.point!==point){
        point=draw.point;filterByTexture.set(texture,point);
        const filter=point?graphics.NEAREST:graphics.LINEAR;
        graphics.texParameteri(graphics.TEXTURE_2D,graphics.TEXTURE_MIN_FILTER,filter);
        graphics.texParameteri(graphics.TEXTURE_2D,graphics.TEXTURE_MAG_FILTER,filter);
      }
      if(vertical!==draw.vertical){vertical=draw.vertical;graphics.uniform2fv(this.verticalClampUniform,vertical);}
      if(sprite!==draw.sprite){sprite=draw.sprite;graphics.uniform4fv(this.spriteClampUniform,sprite);}
      if(stream.indexed)graphics.drawElements(graphics.TRIANGLES,draw.count,graphics.UNSIGNED_SHORT,draw.first*2);
      else graphics.drawArrays(graphics.TRIANGLES,draw.first,draw.count);
    }
    stream.ranges.length=0;stream.length=0;stream.first=0;stream.indexLength=0;stream.mixed=false;
  }

  material(mode) {
    if(!Number.isInteger(mode)||mode<0||mode>6)throw new Error(`Unimplemented blend mode ${mode}`);
    if(this.blend===mode)return;
    this.flush();
    if(!this.spriteStream)this.applyBlend(mode);
    this.blend=mode;
  }

  applyBlend(mode){
    const graphics = this.gl;
    const source=mode<=2?graphics.SRC_ALPHA:mode===3?graphics.ONE:mode===4?graphics.ONE_MINUS_DST_COLOR:mode===5?graphics.DST_COLOR:graphics.ONE_MINUS_SRC_COLOR;
    const destination=mode===1||mode===2?graphics.ONE:mode===3||mode===5?graphics.ZERO:graphics.ONE_MINUS_SRC_ALPHA;
    graphics.blendEquation(mode===2?graphics.FUNC_REVERSE_SUBTRACT:graphics.FUNC_ADD);
    graphics.blendFunc(source,destination);
  }

  captureStage() {
    const graphics=this.gl,surface=this.textures.get('6:2');
    const viewport=this.stageViewport(),width=this.canvas.width,height=this.canvas.height;
    if(!surface?.dynamic)throw new Error('Actual text@R stage surface is unavailable');
    this.flush();
    this.submitSpriteStream();
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

  releaseEndingPixels(ownerToken){
    const surface=this.textures.get('64:0');
    if(!surface?.originalEndingPixels||surface.endingOwnerToken!==ownerToken)return false;
    this.releaseTexture('64:0');return true;
  }

  renderEndingPixels(pixels,ownerToken){
    if(this.disposed||typeof ownerToken!=='string'||!ownerToken.startsWith('th128-owned-ending-')||!(pixels instanceof Uint8Array)||pixels.length!==640*480*4)
      throw new Error('Original ending pixel frame rejected');
    const graphics=this.gl,key='64:0';
    let surface=this.textures.get(key);
    if(surface&&(!surface.originalEndingPixels||surface.endingOwnerToken!==ownerToken))throw new Error('Original ending texture owner collision');
    this.flush();
    if(!surface){
      if((this.textureBytes??0)+pixels.length>256*1024*1024)throw new Error('Original ending texture memory budget exceeded');
      const texture=graphics.createTexture();if(!texture)throw new Error('Cannot allocate original ending pixel texture');
      surface={texture,width:640,height:480,allocationBytes:pixels.length,originalEndingPixels:true,endingOwnerToken:ownerToken};
      this.textures.set(key,surface);this.textureBytes=(this.textureBytes??0)+pixels.length;
      graphics.bindTexture(graphics.TEXTURE_2D,texture);
      graphics.texImage2D(graphics.TEXTURE_2D,0,graphics.RGBA,640,480,0,graphics.RGBA,graphics.UNSIGNED_BYTE,pixels);
    }else{
      graphics.bindTexture(graphics.TEXTURE_2D,surface.texture);
      graphics.texSubImage2D(graphics.TEXTURE_2D,0,0,0,640,480,graphics.RGBA,graphics.UNSIGNED_BYTE,pixels);
    }
    for(const parameter of [graphics.TEXTURE_WRAP_S,graphics.TEXTURE_WRAP_T])
      graphics.texParameteri(graphics.TEXTURE_2D,parameter,graphics.CLAMP_TO_EDGE);
    const failure=graphics.getError();if(failure!==graphics.NO_ERROR)throw new Error('Original ending texture upload failed: '+failure);
    this.render([{bank:64,entry:0,layer:0,mode:0,blend:3,flags:0x40000000,color:0xffffffff,
      width:640,height:480,x:0,y:0,sx:1,sy:1,anchorX:1,anchorY:1,rotation:0,
      u:0,v:0,u0:0,v0:0,u1:1,v1:1,uvScaleX:1,uvScaleY:1}]);
  }

  captureOriginalPauseFrame(descriptor){
    const graphics=this.gl,surface=this.textures.get('6:1');
    if(!surface?.originalPauseCapture||!Number.isSafeInteger(descriptor?.epoch)||descriptor.epoch<1||!Number.isSafeInteger(descriptor.sourceFrame)||descriptor.sourceFrame<0)
      throw new Error('Original paused frame capture request unavailable');
    if(surface.initialized&&surface.pauseEpoch===descriptor.epoch){
      if(surface.sourceFrame!==descriptor.sourceFrame)throw new Error('Original pause epoch reused for another source frame');
      return;
    }
    if(surface.initialized&&descriptor.epoch<surface.pauseEpoch)throw new Error('Stale original pause capture epoch');
    this.flush();
    const texture=graphics.createTexture(),framebuffer=graphics.createFramebuffer();
    if(!texture||!framebuffer){if(texture)graphics.deleteTexture(texture);if(framebuffer)graphics.deleteFramebuffer(framebuffer);throw new Error('Original pause capture allocation failed');}
    let adopted=false;
    try{
      graphics.bindTexture(graphics.TEXTURE_2D,texture);
      graphics.texImage2D(graphics.TEXTURE_2D,0,graphics.RGB565,512,512,0,graphics.RGB,graphics.UNSIGNED_SHORT_5_6_5,null);
      for(const parameter of [graphics.TEXTURE_WRAP_S,graphics.TEXTURE_WRAP_T])graphics.texParameteri(graphics.TEXTURE_2D,parameter,graphics.CLAMP_TO_EDGE);
      for(const parameter of [graphics.TEXTURE_MIN_FILTER,graphics.TEXTURE_MAG_FILTER])graphics.texParameteri(graphics.TEXTURE_2D,parameter,graphics.NEAREST);
      graphics.bindFramebuffer(graphics.DRAW_FRAMEBUFFER,framebuffer);
      graphics.framebufferTexture2D(graphics.DRAW_FRAMEBUFFER,graphics.COLOR_ATTACHMENT0,graphics.TEXTURE_2D,texture,0);
      if(graphics.checkFramebufferStatus(graphics.DRAW_FRAMEBUFFER)!==graphics.FRAMEBUFFER_COMPLETE)throw new Error('Original pause capture framebuffer incomplete');
      graphics.disable(graphics.SCISSOR_TEST);graphics.clearColor(0,0,0,1);graphics.clear(graphics.COLOR_BUFFER_BIT);
      graphics.bindFramebuffer(graphics.READ_FRAMEBUFFER,null);
      const [left,bottom,width,height]=this.stageViewport();
      graphics.blitFramebuffer(left,bottom,left+width,bottom+height,0,384,128,512,graphics.COLOR_BUFFER_BIT,graphics.NEAREST);
      if(graphics.getError()!==graphics.NO_ERROR)throw new Error('Original pause captured pixels unavailable');
      if(surface.texture)graphics.deleteTexture(surface.texture);
      if(!surface.initialized)this.textureBytes=(this.textureBytes??0)+512*512*2;
      Object.assign(surface,{texture,initialized:true,pauseEpoch:descriptor.epoch,sourceFrame:descriptor.sourceFrame,allocationBytes:512*512*2});
      adopted=true;
    }finally{
      graphics.bindFramebuffer(graphics.READ_FRAMEBUFFER,null);graphics.bindFramebuffer(graphics.DRAW_FRAMEBUFFER,null);graphics.deleteFramebuffer(framebuffer);
      if(!adopted)graphics.deleteTexture(texture);
      this.current=null;this.point=null;this.blend=-1;
    }
  }
  renderOriginalPauseFrame(records,snapshot){
    if(!Array.isArray(records)||!snapshot?.backdrop||!Number.isSafeInteger(snapshot.serial)||snapshot.serial<1
      ||snapshot.backdrop.epoch!==snapshot.serial)throw new Error('Original pause presentation owner missing');
    if(!records.some(record=>record.drawPass==='original-pause-shade'))throw new Error('Original separate full-stage shade82 missing');
    this.captureOriginalPauseFrame(snapshot.backdrop);
    this.captureStage();
    this.pauseRenderingEpoch=snapshot.backdrop.epoch;
    try{this.render(records.slice().sort((first,second)=>nativeRecordPriority(first)-nativeRecordPriority(second)),{overlay:true});}
    finally{this.pauseRenderingEpoch=null;}
  }
  renderOriginalTerminalFade(world,textContext=null){
    const terminal=world?.normalTerminal;
    if(!terminal?.configured||!terminal.begun)return false;
    const record=world.normalRecord,flow=world.flow,task=terminal.fadeTask;
    const detached=terminal.sessionRetired===true||terminal.endingStarted===true;
    if(this.disposed||this.canvas.width!==1280||this.canvas.height!==960
        ||terminal.ownerBound!==true||terminal.ownerClaimed!==true||terminal.ownerRecorded!==true||terminal.ownerMatches!==true
        ||terminal.debugTainted!==false||terminal.replayPlayback!==false||terminal.normalClear!==false
        ||world.debug?.runTainted!==false||world.debug?.infiniteRespawn===true
        ||terminal.generation!==flow?.worldGeneration||!Number.isSafeInteger(terminal.generation)||terminal.generation<1
        ||!Number.isInteger(terminal.index)||terminal.index<0||terminal.index>=6||[3,5,8,10,13,15][terminal.index]!==terminal.stage
        ||flow.currentId!==terminal.stage||record?.bound!==true||record.started!==true||record.tainted!==false
        ||record.provenance!=='ordinary-native-postrun'||record.generation!==terminal.generation
        ||record.route!==terminal.index||record.rank!==flow.difficulty
        ||!Number.isInteger(terminal.counter)||terminal.counter<0||terminal.counter>380
        ||!Number.isInteger(terminal.fadeAlpha)||terminal.fadeAlpha<0||terminal.fadeAlpha>255
        ||!Number.isInteger(terminal.fadeElapsed)||terminal.fadeElapsed< -1||terminal.fadeElapsed>0x7fffffff
        ||terminal.counter<=180&&terminal.fadeAlpha!==0
        ||detached&&(terminal.sessionRetired!==true||terminal.endingStarted!==true||world.stageControl?.current!==15)
        ||terminal.counter>=180&&(task?.bound!==true||task.generation!==terminal.generation||task.kind!==5||task.duration!==200
          ||task.current!==terminal.fadeElapsed||task.alpha!==terminal.fadeAlpha||typeof task.active!=='boolean'
          ||task.provenance!=='ordinary-global-kind5'||task.clockSource!==0x4b6f58||task.updatePriority!==15||task.drawPriority!==75
          ||task.returnCode!==(task.active?1:7)||!task.active&&task.alpha!==0||detached&&task.ending!==true))
      throw new Error('Original ordinary terminal fade owner/alpha rejected');
    const alpha=terminal.fadeAlpha/255;if(alpha===0)return false;
    if(textContext&&(!textContext.canvas||textContext.canvas.width!==this.canvas.width||textContext.canvas.height!==this.canvas.height))
      throw new Error('Original terminal text compositor extent changed');
    const graphics=this.gl;
    this.flush();this.material(0);this.flush();
    if(!this.solidTexture){
      const texture=graphics.createTexture();if(!texture)throw new Error('Cannot allocate original terminal fade material');
      graphics.bindTexture(graphics.TEXTURE_2D,texture);
      graphics.texImage2D(graphics.TEXTURE_2D,0,graphics.RGBA,1,1,0,graphics.RGBA,graphics.UNSIGNED_BYTE,new Uint8Array([255,255,255,255]));
      graphics.texParameteri(graphics.TEXTURE_2D,graphics.TEXTURE_MIN_FILTER,graphics.NEAREST);
      graphics.texParameteri(graphics.TEXTURE_2D,graphics.TEXTURE_MAG_FILTER,graphics.NEAREST);this.solidTexture=texture;
    }
    graphics.bindFramebuffer(graphics.FRAMEBUFFER,null);graphics.viewport(0,0,this.canvas.width,this.canvas.height);
    graphics.disable(graphics.SCISSOR_TEST);graphics.bindVertexArray(this.array);graphics.bindBuffer(graphics.ARRAY_BUFFER,this.buffer);
    graphics.bindTexture(graphics.TEXTURE_2D,this.solidTexture);
    this.current={texture:this.solidTexture};this.point=true;this.verticalClamp=[-1048576,1048576];this.spriteClamp=[-1048576,-1048576,1048576,1048576];
    const corners=[[0,0],[640,0],[640,480],[0,480]];
    for(const index of [0,1,2,0,2,3])this.vertices.push(...corners[index],0,0,0,0,0,alpha);
    this.flush();this.current=null;this.point=null;
    if(graphics.getError()!==graphics.NO_ERROR)throw new Error('Original terminal fade GL submission failed');
    if(textContext){
      textContext.save();
      try{
        textContext.setTransform(1,0,0,1,0,0);textContext.globalCompositeOperation='source-atop';
        textContext.globalAlpha=alpha;textContext.fillStyle='#000';textContext.fillRect(0,0,textContext.canvas.width,textContext.canvas.height);
      }finally{textContext.restore();}
    }
    return true;
  }

  shadeForMenu(alpha=.55){
    const graphics=this.gl;
    if(!Number.isFinite(alpha)||alpha<0||alpha>1)throw new Error('Invalid paused frame alpha');
    this.flush();this.material(0);this.flush();
    if(!this.solidTexture){
      const texture=graphics.createTexture();if(!texture)throw new Error('Cannot allocate menu background shade');
      graphics.bindTexture(graphics.TEXTURE_2D,texture);
      graphics.texImage2D(graphics.TEXTURE_2D,0,graphics.RGBA,1,1,0,graphics.RGBA,graphics.UNSIGNED_BYTE,new Uint8Array([255,255,255,255]));
      graphics.texParameteri(graphics.TEXTURE_2D,graphics.TEXTURE_MIN_FILTER,graphics.NEAREST);
      graphics.texParameteri(graphics.TEXTURE_2D,graphics.TEXTURE_MAG_FILTER,graphics.NEAREST);
      this.solidTexture=texture;
    }
    graphics.disable(graphics.SCISSOR_TEST);graphics.bindVertexArray(this.array);graphics.bindBuffer(graphics.ARRAY_BUFFER,this.buffer);
    graphics.bindTexture(graphics.TEXTURE_2D,this.solidTexture);
    this.current={texture:this.solidTexture};this.point=true;this.verticalClamp=[-1048576,1048576];
    const corners=[[128,16],[512,16],[512,464],[128,464]];
    for(const index of [0,1,2,0,2,3])this.vertices.push(...corners[index],0,0,0,0,0,alpha);
    this.flush();this.current=null;this.point=null;
  }
  render(records,{world=false,background=null,shake=[0,0],overlay=false,nativeMenu=false}={}) {
    if(!Array.isArray(records)||records.length>65536)throw new Error('Invalid draw-record budget');
    if(nativeMenu&&(world||background||overlay||this.canvas.width!==1280||this.canvas.height!==960))throw new Error('Invalid native menu raster context');
    if(world&&!(Array.isArray(shake)&&shake.length===2&&shake[0]===0&&shake[1]===0))records=records.map(record=>applyWorldShake(record,shake));
    const streamSprites=world&&records.length>=128,indexed=streamSprites&&this.indexedSprites===true;
    const meshes=new Map(),fans=new Map(),lines=new Map(),laserDepths=new Map();
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
          meshes.set(record,indexed?triangleStripCorners(record,{flipY:true}):triangleStripVertices(record,{flipY:true}));
        }else if(record.geometry.kind==='circle-fan'){
          if(!world||record.mode!==16||![6,16].includes(record.layer)||record.width<=0||record.height<=0)throw new Error('Invalid original mode16 geometry context');
          fans.set(record,circleFanVertices(record));
        }else if(record.geometry.kind==='circle-line'){
          if(!world||record.mode!==17||record.layer!==6)throw new Error('Invalid original level-up circle-line context');lines.set(record,circleLineVertices(record));
        }else if(record.geometry.kind==='anm-ring'){
          if(!world||!((record.bank===5&&record.layer===17&&(record.mode===9||record.mode===14))||(record.bank===0&&record.layer===4&&record.mode===9)))throw new Error('Invalid original Boss/spell ring context');
          meshes.set(record,indexed?ringStripCorners(record):ringStripVertices(record));
        }else if(record.geometry.kind==='laser-selector3-quad'){
          if(!world)throw new Error('Original selector3 quad requires its actual World draw pass');
          fans.set(record,selector3QuadVertices(record));
          const z=record.geometry.vertices[0].z;
          if(z<0||z>1||record.geometry.vertices.some(vertex=>vertex.z!==z))throw new Error('Invalid original pretransformed laser depth');
          laserDepths.set(record,z);
        }else throw new Error('Unimplemented original custom geometry');
        meshPoints+=record.geometry.vertices.length;
        if(meshPoints>65536)throw new Error('World mesh vertex budget exceeded');
      }
    }
    if(mainCount>1)throw new Error('Ambiguous Main82 stage compositor');
    const graphics = this.gl;
    if(nativeMenu)this.bindNativeMenuRaster();
    // Original selector3 draw 4327f0 uses ZWRITE + LESS across its columns, then ALWAYS + writes off.
    // Keep the stage depth buffer; return to the ordinary 2D pass before drawing any other owner.
    let laserDepth=null;
    const leaveLaserDepth=()=>{
      if(laserDepth===null)return;
      this.flush();this.submitSpriteStream();
      graphics.depthRange(0,1);graphics.depthFunc(graphics.ALWAYS);graphics.depthMask(false);
      graphics.disable(graphics.DEPTH_TEST);laserDepth=null;
    };
    try{
    const renderWidth=nativeMenu?640:this.canvas.width,renderHeight=nativeMenu?480:this.canvas.height;
    const nativeScale=renderWidth/640,integerHudScale=nativeScale>1&&Number.isInteger(nativeScale)&&this.canvas.height/480===nativeScale;
    graphics.bindVertexArray(this.array);
    graphics.bindBuffer(graphics.ARRAY_BUFFER,this.buffer);
    graphics.viewport(0,0,renderWidth,renderHeight);
    graphics.disable(graphics.SCISSOR_TEST);
    if(!overlay){
      graphics.clearColor(0,0,0,1);graphics.clear(graphics.COLOR_BUFFER_BIT);
      if(!background&&laserDepths.size){graphics.depthMask(true);graphics.clearDepth(1);graphics.clear(graphics.DEPTH_BUFFER_BIT);}
    }
    this.current = null;
    this.blend = -1;
    this.vertices.length = 0;
    if(background&&!overlay)this.drawBackground(background);
    if(streamSprites)this.spriteStream={ranges:[],length:0,first:0,gpuBlend:-1,indexed,indexLength:0,mixed:false};
    let captured=false,previousBank=-1,previousEntry=-1,previousTexture=null;
    let lastColors=null,lastFlags,lastColor,lastSecondary;
    const quadColors=this.quadColors??={first:[0,0,0,0],second:[0,0,0,0],corners:new Array(4)};
    for (const record of records) {
      if(mainCount&&!captured&&nativeRecordPriority(record)>=12){this.captureStage();captured=true;}
      const geometry=record.geometry,z=geometry?laserDepths.get(record):undefined;
      if(z===undefined){if(laserDepth!==null)leaveLaserDepth();}
      else if(z!==laserDepth){
        this.flush();this.submitSpriteStream();graphics.enable(graphics.DEPTH_TEST);graphics.depthMask(true);graphics.depthFunc(graphics.LESS);
        graphics.depthRange(z,z);laserDepth=z;
      }
      const line=geometry?lines.get(record):undefined;
      const fan=line??(geometry?fans.get(record):undefined);
      if(fan){
        if(geometry.vertices.every(vertex=>(vertex.color>>>24)===0))continue;
        this.material(record.blend);this.flush();this.submitSpriteStream();
        if(this.spriteStream&&this.spriteStream.gpuBlend!==record.blend){this.applyBlend(record.blend);this.spriteStream.gpuBlend=record.blend;}
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
        graphics.bufferData(graphics.ARRAY_BUFFER,fan,graphics.STREAM_DRAW);if(line)graphics.lineWidth(this.canvas.width/640);graphics.drawArrays(line?graphics.LINE_STRIP:graphics.TRIANGLES,0,fan.length/8);
        this.current=null;this.point=null;continue;
      }
      const mesh=geometry?meshes.get(record):undefined;
      let colors=null;
      if(!mesh){
        if(!lastColors||record.flags!==lastFlags||record.color!==lastColor||record.secondary!==lastSecondary){
          lastColors=nativeQuadColors(record,quadColors);
          lastFlags=record.flags;lastColor=record.color;lastSecondary=record.secondary;
        }
        colors=lastColors;
      }
      if ((colors?colors[0][3]===0&&colors[1][3]===0&&colors[2][3]===0&&colors[3][3]===0:(record.color>>>24)===0) || record.width === 0 || record.height === 0) continue;
      if (!Number.isFinite(record.u)||!Number.isFinite(record.v)||!Number.isFinite(record.uvScaleX)||!Number.isFinite(record.uvScaleY)
          ||!Number.isFinite(record.u0)||!Number.isFinite(record.v0)||!Number.isFinite(record.u1)||!Number.isFinite(record.v1)) {
        throw new Error('Non-finite texture coordinates');
      }
      if(record.bank!==previousBank||record.entry!==previousEntry){
        previousBank=record.bank;previousEntry=record.entry;previousTexture=this.textures.get(`${previousBank}:${previousEntry}`);
      }
      const texture=previousTexture;
      if (!texture) throw new Error(`没有绘制纹理：bank=${record.bank}, entry=${record.entry}`);
      if(texture.dynamic&&(!texture.initialized||record.bank!==6||record.entry!==2&&!(record.entry===1&&texture.originalPauseCapture&&this.pauseRenderingEpoch===texture.pauseEpoch)))throw new Error('Dynamic texture has no established current-frame capture');
      this.material(record.blend);
      // Original static HUD texels are replicated at integer display scale (case212).
      const unitHudTexels=integerHudScale&&(record.bank===5||record.bank===11)&&!geometry&&record.mode===0
        &&record.sx===1&&record.sy===1&&record.rotation===0&&(record.rx??0)===0&&(record.ry??0)===0
        &&record.u===0&&record.v===0&&record.uvScaleX===1&&record.uvScaleY===1&&!texture.dynamic
        &&Math.abs(Math.abs(record.u1-record.u0)*texture.width-record.width)<1e-5
        &&Math.abs(Math.abs(record.v1-record.v0)*texture.height-record.height)<1e-5;
      const point = Boolean(record.flags2 & 4)||unitHudTexels;
      const staticAtlas=!geometry&&((record.bank===5&&record.entry===0&&record.sprite>=0&&record.sprite<=56)||(record.bank===11&&record.entry===0))
        &&record.u===0&&record.v===0&&record.uvScaleX===1&&record.uvScaleY===1;
      const spriteClamp=staticAtlas
        ?[Math.min(record.u0,record.u1)+.5/texture.width,Math.min(record.v0,record.v1)+.5/texture.height,Math.max(record.u0,record.u1)-.5/texture.width,Math.max(record.v0,record.v1)-.5/texture.height]
        :unboundedSpriteClamp;
      const verticalClamp=texture.balloon&&record.script>=138&&record.script<=155
        ?[Math.min(record.v0,record.v1)+0.5/texture.height,Math.max(record.v0,record.v1)-0.5/texture.height]:unboundedVerticalClamp;
      if (this.current !== texture || point !== this.point || verticalClamp[0]!==this.verticalClamp[0] || verticalClamp[1]!==this.verticalClamp[1]
          ||spriteClamp!==this.spriteClamp&&spriteClamp.some((value,index)=>value!==this.spriteClamp?.[index])) {
        this.flush();
        this.current = texture;
        this.point = point;
        this.verticalClamp=verticalClamp;
        this.spriteClamp=spriteClamp;
        if(!this.spriteStream){
          graphics.bindTexture(graphics.TEXTURE_2D,texture.texture);
          graphics.texParameteri(graphics.TEXTURE_2D,graphics.TEXTURE_MIN_FILTER,point?graphics.NEAREST:graphics.LINEAR);
          graphics.texParameteri(graphics.TEXTURE_2D,graphics.TEXTURE_MAG_FILTER,point?graphics.NEAREST:graphics.LINEAR);
        }
      }
      if(mesh){
        if(indexed)this.appendIndexedStrip(mesh);
        else if(this.spriteStream){const first=this.spriteStream.length;this.extendSpriteStream(mesh.length).set(mesh,first);}
        else this.vertices.push(...mesh);
        continue;
      }
      appendQuad(this,record,texture,colors);
    }
    this.flush();
    this.submitSpriteStream();
    if(nativeMenu){
      graphics.bindFramebuffer(graphics.READ_FRAMEBUFFER,this.nativeMenuRasterSurface.framebuffer);
      graphics.bindFramebuffer(graphics.DRAW_FRAMEBUFFER,null);
      graphics.blitFramebuffer(0,0,640,480,0,0,this.canvas.width,this.canvas.height,graphics.COLOR_BUFFER_BIT,graphics.NEAREST);
    }
    const error = graphics.getError();
    if (error !== graphics.NO_ERROR) throw new Error(`WebGL 错误 ${error}`);
    }finally{this.spriteStream=null;this.vertices.length=0;leaveLaserDepth();if(nativeMenu){graphics.bindFramebuffer(graphics.FRAMEBUFFER,null);graphics.viewport(0,0,this.canvas.width,this.canvas.height);this.current=null;this.point=null;}}
  }
}
