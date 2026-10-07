// Ordered WebGL2 sprite batches. Presentation is a reader of core draw state.
import {AnimationSampler} from './animation.mjs';
import {StageBackground} from './stage-background.mjs';
export function readCoreHud(core){const size=core._th12_hud_size?.()??20;if(!Number.isInteger(size)||size<20||size>256)throw Error('状态记录长度无效');return new Float32Array(core.HEAPU8.buffer,core._th12_hud(),size);}
export function animationBankName(bank,stage=1){const suffix=String(stage).padStart(2,'0');return ['bullet','enemy','stgenm'+suffix,'stgenm'+suffix+'m','pl00','pl01','pl02','front','text','st'+suffix+'logo','stage'+suffix,'ascii'][bank]??null;}
export function decodeMessageHex(hex){if(typeof hex!=='string'||hex.length%2||!/^[0-9a-f]*$/i.test(hex))throw Error('对话字符数据无效');return new TextDecoder('shift-jis').decode(Uint8Array.from(hex.match(/../g)??[],v=>parseInt(v,16)));}
export function readCoreMessage(core){if(!core._th12_message||!core._th12_message_size)return null;const ptr=core._th12_message(),size=core._th12_message_size();if(!Number.isInteger(size)||size<0||size>1024*1024||ptr<0||ptr+size>core.HEAPU8.byteLength)throw Error('对话状态记录无效');if(!size)return null;return JSON.parse(new TextDecoder().decode(core.HEAPU8.subarray(ptr,ptr+size)).replace(/\0+$/,''));}
export function readCoreBackground(core){
  if(!core._th12_background||!core._th12_background_size)return null;
  const ptr=core._th12_background(),size=core._th12_background_size();
  if(!Number.isInteger(ptr)||!Number.isInteger(size)||ptr<0||size<=0||size>4*1024*1024||ptr+size>core.HEAPU8.byteLength)throw Error('背景状态记录无效');
  const state=JSON.parse(new TextDecoder().decode(core.HEAPU8.subarray(ptr,ptr+size)).replace(/\0+$/,''));
  if(!state||![false,true,0,1].includes(state.loaded))throw Error('背景状态记录无效');
  if(state.loaded){const c=state.camera;if(!c||!['position','direction','up'].every(k=>Array.isArray(c[k])&&c[k].length===3&&c[k].every(Number.isFinite))||!Number.isFinite(c.fov)||!Array.isArray(c.fog)||c.fog.length!==5||!c.fog.every(Number.isFinite)||!Array.isArray(state.poses)||state.poses.length>65536||!state.poses.every(p=>Array.isArray(p)&&p.length>=21&&p.slice(0,21).every(Number.isFinite)))throw Error('背景相机或姿态记录无效');}
  return state;
}
const argbRgba=(argb,alpha=1)=>[((argb>>>16)&255)/255,((argb>>>8)&255)/255,(argb&255)/255,((argb>>>24)&255)/255*alpha];
export function beamSegmentVertices(x,y,angle,length,width,start,end){const dx=Math.cos(angle)*length/2,dy=Math.sin(angle)*length/2,nx=-Math.sin(angle)*width/2,ny=Math.cos(angle)*width/2;return [{x:x-dx-nx,y:y-dy-ny,color:start},{x:x-dx+nx,y:y-dy+ny,color:start},{x:x+dx+nx,y:y+dy+ny,color:end},{x:x+dx-nx,y:y+dy-ny,color:end}];}
export function dialogueMetrics(width=354,height=17){return {width,height,sourceWidth:Math.min(1024,2*width+22),sourceHeight:36};}
export function messageTextPoses(message,logicalPoses,atlases){
  const roots=logicalPoses.filter(p=>p.a.name==='text'&&p.a.sprite>=0&&p.a.sprite<4),colors=[0xf8f08f,0x8088ff,0xd8d8d8],records=message?.textRecords??(message?.textHex??[]).map(textHex=>({textHex,offset:0,spacing:0,style:(message.flags>>1)&1,color:colors[message.speaker]??colors[2]}));
  const make=(p,slot)=>{const record=records[slot];if(!record?.textHex||!p.a.visible)return null;const text=decodeMessageHex(record.textHex),sprite=atlases.text.sprites[slot];return {...p,dialogue:true,text,record,metrics:dialogueMetrics(sprite.w,sprite.h)};};
  if(roots.length)return roots.map(p=>make(p,p.a.sprite)).filter(Boolean);
  if(!message?.active)return [];
  // Older cores have no logical text VMs. Coordinates below come from the
  // retail text scripts and MSG engine offset; modern cores use Pose above.
  return records.map((record,slot)=>make({a:{name:'text',sprite:slot,layer:20,visible:true,alpha:1,sx:1,sy:1,anchorX:1,anchorY:1},x:56,y:(slot<2?373:367)+22*(slot%2),alpha:1,order:logicalPoses.length+slot},slot)).filter(Boolean);
}
export function readCoreAnimationPoses(core,stage=1){
  if(!core._th12_anm_draw||!core._th12_anm_draw_ptr)return [];
  const count=core._th12_anm_draw(),ptr=core._th12_anm_draw_ptr(),stride=core._th12_anm_draw_stride?.()??128;
  if(!Number.isInteger(count)||count<0||count>65536||stride!==128||ptr<0||ptr+count*stride>core.HEAPU8.byteLength)throw Error('逻辑动画姿态记录无效');
  const view=new DataView(core.HEAPU8.buffer),rgba=argb=>[((argb>>>16)&255)/255,((argb>>>8)&255)/255,(argb&255)/255,((argb>>>24)&255)/255],poses=[];
  for(let i=0;i<count;i++){const offset=ptr+i*stride,F=n=>view.getFloat32(offset+n*4,true),I=n=>view.getInt32(offset+n*4,true),U=n=>view.getUint32(offset+n*4,true),flags=U(24),flags2=U(25),primary=rgba(U(14)),secondary=rgba(U(15)),color=flags&0x10000?secondary:primary;
    const a={name:animationBankName(I(16),stage),sprite:I(17),layer:I(18),mode:I(19),anchorX:I(20),anchorY:I(21),blend:I(22),geometryCount:I(23),
      x:0,y:0,z:F(2),rx:F(3),ry:F(4),rotation:F(5),sx:F(6),sy:F(7),u:F(8),v:F(9),uvScaleX:F(10),uvScaleY:F(11),geometryRadius:F(12),geometryWidth:F(13),
      r:color[0],g:color[1],b:color[2],alpha:color[3],r2:secondary[0],g2:secondary[1],b2:secondary[2],alpha2:secondary[3],point:!!(flags2&2),visible:!!(flags&1),rotate:I(19)===1||I(19)===3};
    if(U(31)===1){a.nativeWidth=F(29);a.nativeHeight=F(30);}
    const owner=U(27);a.visible=(flags&3)===3;
    poses.push({a,x:F(0),y:F(1),angle:0,scale:1,alpha:F(28),id:U(26),owner,logical:true,order:i});
  }return poses;
}
export function readCoreLaserSegments(core,stage=1){
  if(!core._th12_laser_draw)return [];
  const count=core._th12_laser_draw(),ptr=core._th12_laser_draw_ptr(),stride=core._th12_laser_draw_stride();
  if(!Number.isInteger(count)||count<0||count>65536||stride!==64||ptr<0||ptr+count*stride>core.HEAPU8.byteLength)throw Error('激光绘制记录无效');
  const view=new DataView(core.HEAPU8.buffer),segments=[];
  for(let i=0;i<count;i++){
    const at=ptr+i*stride,F=n=>view.getFloat32(at+n*4,true),I=n=>view.getInt32(at+n*4,true),U=n=>view.getUint32(at+n*4,true),color=U(8);
    segments.push({laser:true,x:F(0),y:F(1),angle:F(2),length:F(3),width:F(4),alpha:F(5)*((color>>>24)/255),u:F(6),v:F(7),tint:[((color>>>16)&255)/255,((color>>>8)&255)/255,(color&255)/255],textureLength:F(13),headOffset:F(14),owner:U(15),a:{name:animationBankName(I(9),stage),sprite:I(10),layer:I(11),blend:I(12)}});
  }return segments;
}
export function readCoreMeshes(core,stage=1){
  if(!core._th12_mesh_draw)return [];
  const count=core._th12_mesh_draw(),ptr=core._th12_mesh_draw_ptr(),stride=core._th12_mesh_draw_stride(),vertexPtr=core._th12_mesh_vertices_ptr(),vertexCount=core._th12_mesh_vertices_count(),vertexStride=core._th12_mesh_vertex_stride(),limit=core.HEAPU8.byteLength;
  if(!Number.isInteger(count)||count<0||count>65536||stride!==56||vertexStride!==28||!Number.isInteger(vertexCount)||vertexCount<0||vertexCount>4000000||ptr<0||ptr+count*stride>limit||vertexPtr<0||vertexPtr+vertexCount*vertexStride>limit)throw Error('网格绘制记录无效');
  const view=new DataView(core.HEAPU8.buffer),meshes=[];
  for(let i=0;i<count;i++){
    const at=ptr+i*stride,I=n=>view.getInt32(at+n*4,true),U=n=>view.getUint32(at+n*4,true),offset=U(10),length=U(11),primitive=U(5),pass=U(6);
    if(offset+length>vertexCount||![4,5].includes(primitive)||pass>2)throw Error('网格顶点范围无效');
    const vertices=[];
    for(let j=0;j<length;j++){const a=vertexPtr+(offset+j)*vertexStride,F=n=>view.getFloat32(a+n*4,true),color=view.getUint32(a+16,true);vertices.push({x:F(0),y:F(1),z:F(2),rhw:F(3),color,u:F(5),v:F(6)});}
    meshes.push({mesh:true,vertices,primitive,renderPass:pass,drawPriority:I(7),owner:U(8),id:U(9),order:U(9),filter:U(12),addressFlags:U(13),a:{name:animationBankName(I(0),stage),sprite:I(1),entry:I(2),layer:I(3),blend:I(4)}});
  }return meshes;
}
export function fragmentInterrupt(index,whole,fragments=0){return index<whole?2:index===whole?7+Math.max(0,Math.min(5,fragments|0)):3;}
export function groupedNumber(n){return String(Math.max(0,Math.round(n))).replace(/\B(?=(\d{3})+(?!\d))/g,',');}
// Original ASCII font 3 (0x401d97): 16px sprites, 12px numeral advance,
// 4px comma advance, and comma shifted down 3px. HUD calls: 0x41f473..
export function hudGlyphs(text,x,y,scale=1,right=false){text=String(text);const advance=c=>(c===','?4:12)*scale;if(right)x-=[...text].reduce((w,c)=>w+advance(c),0);return [...text].map(c=>{const id=c===','?257:c==='/'?253:c==='.'?254:c==='s'?255:c==='*'?256:c.charCodeAt(0)+195,pose={id,x,y:y+(c===','?3:0),scale};x+=advance(c);return pose;});}
// Native mode15 circle helper 0x45d430 writes a triangle fan: primary color
// at its center, secondary color at count+1 circumference vertices.
export function circleVertices(x,y,radius,angle,segments,centerColor,edgeColor){const f=Math.fround,pi=3.1415927410125732,tau=6.2831854820251465,step=f(6.283185307179586/segments),vertices=[{x,y,color:centerColor}];angle=f(angle);for(let i=0;i<=segments;i++){vertices.push({x:f(x+f(Math.cos(angle)*radius)),y:f(y+f(Math.sin(angle)*radius)),color:edgeColor});angle=f(angle+step);while(angle>pi)angle=f(angle-tau);while(angle<-pi)angle=f(angle+tau);}return vertices;}
export function quadCorners(x,y,w,h,angle=0,anchorX=0,anchorY=0,pixel=false){
  const left=anchorX===1?0:anchorX===2?-w:-w/2,top=anchorY===1?0:anchorY===2?-h:-h/2,c=Math.cos(angle),s=Math.sin(angle);
  return [[left,top],[left+w,top],[left+w,top+h],[left,top+h]].map(([a,b])=>{const px=x+a*c-b*s,py=y+a*s+b*c;return [pixel?Math.round(px):px,pixel?Math.round(py):py];});
}
export class Renderer {
  constructor(canvas,atlases){
    this.canvas=canvas;this.atlases=atlases;this.textures=new Map();this.dialogueTextures=new Map();this.batch=[];this.current=null;this.blend=0;this.animations=new AnimationSampler(atlases);this.stageBackgrounds=new Map();this.stageJobs=new Map();this.requestedStage=1;
    const gl=this.gl=canvas.getContext('webgl2',{alpha:false,antialias:false});if(!gl)throw Error('浏览器需要支持 WebGL 2');
    const compile=(type,source)=>{const s=gl.createShader(type);gl.shaderSource(s,source);gl.compileShader(s);if(!gl.getShaderParameter(s,gl.COMPILE_STATUS))throw Error(gl.getShaderInfoLog(s));return s;};
    const p=gl.createProgram();gl.attachShader(p,compile(gl.VERTEX_SHADER,`#version 300 es
      precision highp float;layout(location=0)in vec2 position;layout(location=1)in vec2 uv;layout(location=2)in vec4 color;layout(location=3)in vec2 depthFog;out vec2 texcoord;out vec4 tint;out float fog;
      void main(){gl_Position=vec4((position.x/320.0-1.0)*depthFog.x,(1.0-position.y/240.0)*depthFog.x,0,depthFog.x);texcoord=uv;tint=color;fog=depthFog.y;}`));
    gl.attachShader(p,compile(gl.FRAGMENT_SHADER,`#version 300 es
      precision mediump float;uniform sampler2D atlas;uniform vec3 fogColor;in vec2 texcoord;in vec4 tint;in float fog;out vec4 pixel;
      void main(){pixel=texture(atlas,texcoord)*tint;pixel.rgb=mix(pixel.rgb,fogColor,clamp(fog,0.0,1.0));if(pixel.a<0.01)discard;}`));
    gl.linkProgram(p);if(!gl.getProgramParameter(p,gl.LINK_STATUS))throw Error(gl.getProgramInfoLog(p));gl.useProgram(p);
    this.buffer=gl.createBuffer();gl.bindBuffer(gl.ARRAY_BUFFER,this.buffer);this.vao=gl.createVertexArray();gl.bindVertexArray(this.vao);
    for(const [slot,count,offset]of [[0,2,0],[1,2,8],[2,4,16],[3,2,32]]){gl.enableVertexAttribArray(slot);gl.vertexAttribPointer(slot,count,gl.FLOAT,false,40,offset);}
    this.fogColor=gl.getUniformLocation(p,'fogColor');
    const solid=gl.createTexture();gl.bindTexture(gl.TEXTURE_2D,solid);gl.texImage2D(gl.TEXTURE_2D,0,gl.RGBA,1,1,0,gl.RGBA,gl.UNSIGNED_BYTE,new Uint8Array([255,255,255,255]));gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_MIN_FILTER,gl.NEAREST);gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_MAG_FILTER,gl.NEAREST);this.solid={texture:solid,w:1,h:1};
    gl.enable(gl.BLEND);gl.blendFunc(gl.SRC_ALPHA,gl.ONE_MINUS_SRC_ALPHA);
    this.uploadCount=0;this.drawCount=0;this.lastFrame=-1;this.point=null;
  }
  async prepare(){
    await this.loadAtlases(['pl00','pl01','pl02','bullet','enemy','front','ascii','text']);await this.prepareStage(1);
  }
  async prepareStage(stage){
    if(!Number.isInteger(stage)||stage<1||stage>7)throw Error('关卡编号无效');
    this.requestedStage=stage;
    if(!this.stageJobs.has(stage)){const name='stage'+String(stage).padStart(2,'0'),enemy='stgenm'+String(stage).padStart(2,'0'),logo='st'+String(stage).padStart(2,'0')+'logo';
      const job=(async()=>{const response=await fetch('/assets/'+name+'-background.json');if(!response.ok)throw Error('关卡背景数据加载失败 '+stage);const background=new StageBackground(await response.json(),this.atlases);await this.loadAtlases([name,enemy,enemy+'m',logo].filter(n=>this.atlases[n]));this.stageBackgrounds.set(stage,background);return background;})();this.stageJobs.set(stage,job);}
    const background=await this.stageJobs.get(stage);if(this.requestedStage===stage)this.background=background;return background;
  }
  async loadAtlases(names){
    const gl=this.gl;
    // Atlas metadata may include virtual duplicate names; each physical entry
    // gets its own generated image, so images never silently overwrite others.
    const entries=[];for(const name of names){const atlas=this.atlases[name];if(!atlas)throw Error('动画资源不存在 '+name);for(let index=0;index<atlas.entries.length;index++){const e=atlas.entries[index];if(e.image&&!this.textures.has(name+':'+index))entries.push({name,index,e});}}
    for(let start=0;start<entries.length;start+=8)await Promise.all(entries.slice(start,start+8).map(async({name,index,e})=>{
      const response=await fetch('/assets/'+e.image);if(!response.ok)throw Error('贴图加载失败 '+e.image);
      const bitmap=await createImageBitmap(await response.blob(),{premultiplyAlpha:'none',colorSpaceConversion:'none'}),texture=gl.createTexture();
      gl.bindTexture(gl.TEXTURE_2D,texture);gl.pixelStorei(gl.UNPACK_PREMULTIPLY_ALPHA_WEBGL,false);gl.texImage2D(gl.TEXTURE_2D,0,gl.RGBA,gl.RGBA,gl.UNSIGNED_BYTE,bitmap);
      gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_MIN_FILTER,gl.LINEAR);gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_MAG_FILTER,gl.LINEAR);const wrap=gl.REPEAT; // Native4514c5/4514db defaults both texture axes to WRAP1.
      gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_WRAP_S,wrap);gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_WRAP_T,wrap);
      this.textures.set(name+':'+index,{texture,w:bitmap.width,h:bitmap.height,defaultWrap:wrap});bitmap.close();
    }));
  }
  begin(){const g=this.gl;g.viewport(0,0,640,480);g.clearColor(.035,.035,.055,1);g.clear(g.COLOR_BUFFER_BIT);g.bindVertexArray(this.vao);g.bindBuffer(g.ARRAY_BUFFER,this.buffer);this.drawCount=0;this.uploadCount=0;this.current=null;this.batch=[];this.batchMode=g.TRIANGLES;this.blend=-1;this.material(0);}
  flush(){if(!this.batch.length)return;const g=this.gl;g.bindTexture(g.TEXTURE_2D,this.current.texture);g.bufferData(g.ARRAY_BUFFER,new Float32Array(this.batch),g.STREAM_DRAW);g.drawArrays(this.batchMode,0,this.batch.length/10);this.drawCount++;this.uploadCount++;this.batch.length=0;}
  primitive(mode){if(this.batchMode!==mode){this.flush();this.batchMode=mode;}}
  material(blend){if(this.blend===blend)return;this.flush();const g=this.gl;const factors=[[g.SRC_ALPHA,g.ONE_MINUS_SRC_ALPHA],[g.SRC_ALPHA,g.ONE],[g.ZERO,g.ONE_MINUS_SRC_COLOR],[g.ONE,g.ZERO],[g.ONE_MINUS_DST_COLOR,g.ONE_MINUS_SRC_ALPHA],[g.DST_COLOR,g.ZERO],[g.ONE_MINUS_SRC_COLOR,g.ONE_MINUS_SRC_ALPHA]];g.blendFunc(...(factors[blend]||factors[0]));this.blend=blend;}
  region(name,entry,sx,sy,sw,sh,x,y,w,h,angle=0,alpha=1,tint=[1,1,1],blend=0,options={}){
    const texture=this.textures.get(name+':'+entry);if(!texture)return;
    this.material(blend);this.texture(texture,!!options.point);this.primitive(this.gl.TRIANGLES);
    const corners=quadCorners(x,y,w,h,angle,options.anchorX||0,options.anchorY||0,!!options.pixel),du=options.u||0,dv=options.v||0;
    const endX=sx+sw*(options.uvScaleX??1),endY=sy+sh*(options.uvScaleY??1);
    const uv=[[sx/texture.w+du,sy/texture.h+dv],[endX/texture.w+du,sy/texture.h+dv],[endX/texture.w+du,endY/texture.h+dv],[sx/texture.w+du,endY/texture.h+dv]];
    for(const i of [0,1,2,0,2,3])this.batch.push(...corners[i],...uv[i],...tint,alpha,1,0);
  }
  texture(texture,point=false,addressFlags=null){const g=this.gl,wrapU=addressFlags===null?(texture.defaultWrap??g.CLAMP_TO_EDGE):addressFlags&1?g.REPEAT:g.CLAMP_TO_EDGE,wrapV=addressFlags===null?(texture.defaultWrap??g.CLAMP_TO_EDGE):addressFlags&2?g.REPEAT:g.CLAMP_TO_EDGE;if(this.current!==texture||this.point!==point||this.wrapU!==wrapU||this.wrapV!==wrapV){this.flush();this.current=texture;this.point=point;this.wrapU=wrapU;this.wrapV=wrapV;g.bindTexture(g.TEXTURE_2D,texture.texture);g.texParameteri(g.TEXTURE_2D,g.TEXTURE_MIN_FILTER,point?g.NEAREST:g.LINEAR);g.texParameteri(g.TEXTURE_2D,g.TEXTURE_MAG_FILTER,point?g.NEAREST:g.LINEAR);g.texParameteri(g.TEXTURE_2D,g.TEXTURE_WRAP_S,wrapU);g.texParameteri(g.TEXTURE_2D,g.TEXTURE_WRAP_T,wrapV);}}
  mesh(p){
    const texture=this.textures.get(p.a.name+':'+p.a.entry);if(!texture)return;
    this.material(p.a.blend);this.texture(texture,p.filter===1,p.addressFlags);this.primitive(this.gl.TRIANGLES);
    const emit=v=>this.batch.push(v.x,v.y,v.u,v.v,...argbRgba(v.color),v.rhw?1/v.rhw:1,0);
    if(p.primitive===4)for(const v of p.vertices)emit(v);
    else for(let i=2;i<p.vertices.length;i++)for(const index of i&1?[i-1,i-2,i]:[i-2,i-1,i])emit(p.vertices[index]);
  }
  sprite(name,id,x,y,angle=0,scale=1,alpha=1){const s=this.atlases[name]?.sprites[id];if(s)this.region(name,s.entry,s.x,s.y,s.w,s.h,x,y,s.w*scale,s.h*scale,angle,alpha);}
  solidTriangles(vertices,indices,blend){this.material(blend);this.texture(this.solid,true);this.primitive(this.gl.TRIANGLES);for(const i of indices){const p=vertices[i];this.batch.push(p.x,p.y,.5,.5,...p.color,1,0);}}
  trail(x,y,angle,length,start,end,alpha){const dx=Math.cos(angle)*length/2,dy=Math.sin(angle)*length/2;this.material(1);this.texture(this.solid,true);this.primitive(this.gl.LINES);this.batch.push(x-dx,y-dy,.5,.5,...argbRgba(start,alpha),1,0,x+dx,y+dy,.5,.5,...argbRgba(end,alpha),1,0);}
  beamTrail(x,y,angle,length,width,start,end){
    // Native profile3/script210 uses sprite405, blend1/layer12. The custom
    // vertex update 46a0ee..46a153 offsets both edges by12px (full width24).
    // The current segment ABI does not expose native longitudinal UV phase.
    const sprite=this.atlases.bullet.sprites[405],texture=this.textures.get('bullet:'+sprite.entry);if(!texture)return;
    this.material(1);this.texture(texture,false);this.primitive(this.gl.TRIANGLES);
    const vertices=beamSegmentVertices(x,y,angle,length,width,argbRgba(start),argbRgba(end)),uv=[[sprite.x/texture.w,0],[(sprite.x+sprite.w)/texture.w,0],[(sprite.x+sprite.w)/texture.w,1/32],[sprite.x/texture.w,1/32]];
    for(const i of [0,1,2,0,2,3])this.batch.push(vertices[i].x,vertices[i].y,...uv[i],...vertices[i].color,1,0);
  }
  laserSegment(p){
    const s=this.atlases[p.a.name]?.sprites[p.a.sprite];if(!s||p.alpha<=0)return;
    const x=p.x+Math.cos(p.angle)*p.length/2,y=p.y+Math.sin(p.angle)*p.length/2;
    this.region(p.a.name,s.entry,s.x,s.y,s.w,s.h,x,y,p.width,p.length,p.angle+Math.PI/2,p.alpha,p.tint,p.a.blend,{u:p.u,v:p.v});
  }
  state(a,x,y,angle=0,scale=1,alpha=1){
    if(!a.visible)return;
    if(a.mode===14){if(a.alpha<=0)return;const color=[a.r,a.g,a.b,a.alpha*alpha],corners=quadCorners(x+a.x,y+a.y,a.geometryRadius*a.sx*scale,a.geometryWidth*a.sy*scale,a.rotation+angle);this.solidTriangles(corners.map(([x,y])=>({x,y,color})),[0,1,2,0,2,3],a.blend);return;}
    if(a.mode===15||a.mode===16){const count=a.geometryCount;if(count<3||count>512)return;const center=[a.r,a.g,a.b,a.alpha*alpha],edge=a.mode===15?[a.r2,a.g2,a.b2,a.alpha2*alpha]:center;if(center[3]<=0&&edge[3]<=0)return;const vertices=circleVertices(x+a.x,y+a.y,a.geometryRadius*a.sx*scale,a.rotation+angle,count,center,edge),indices=[];for(let i=1;i<=count;i++)indices.push(0,i,i+1);this.solidTriangles(vertices,indices,a.blend);return;}
    const s=this.atlases[a.name]?.sprites[a.sprite];if(!s||a.alpha<=0)return;this.region(a.name,s.entry,s.x,s.y,s.w,s.h,x+a.x,y+a.y,(a.nativeWidth!==undefined&&a.nativeWidth!==-1?a.nativeWidth:s.w)*scale*a.sx,(a.nativeHeight!==undefined&&a.nativeHeight!==-1?a.nativeHeight:s.h)*scale*a.sy,a.rotate?a.rotation+angle:0,a.alpha*alpha,[a.r,a.g,a.b],a.blend,{anchorX:a.anchorX,anchorY:a.anchorY,u:a.u,v:a.v,uvScaleX:a.uvScaleX,uvScaleY:a.uvScaleY,pixel:a.mode===0||a.mode===1&&a.rotation+angle===0,point:a.point});
  }
  animation(name,id,age,x,y,angle=0,scale=1,variant=null,options={}){const {alpha=1,...sampling}=options;for(const a of this.animations.sampleAll(name,id,age,variant,sampling))this.state(a,x,y,angle,scale,alpha);}
  text(text,x,y,scale=1,tint=[1,1,1]){for(const c of String(text)){const s=this.atlases.ascii.sprites[c.charCodeAt(0)-32];if(s)this.region('ascii',s.entry,s.x,s.y,s.w,s.h,x+s.w*scale/2,y+s.h*scale/2,s.w*scale,s.h*scale,0,1,tint);x+=14*scale;}}
  hudText(text,x,y,scale=1,right=false){for(const p of hudGlyphs(text,x,y,scale,right)){const s=this.atlases.ascii.sprites[p.id];if(s)this.region('ascii',s.entry,s.x,s.y,s.w,s.h,p.x,p.y,s.w*scale,s.h*scale,0,1,[1,1,1],0,{anchorX:1,anchorY:1,point:true});}}
  dialogueTexture(text,record,metrics){
    // Original 44e56f..44e5c6 selects 32px GDI MS Gothic (400) / MS Mincho
    // (600), then text rendering reduces by two. Browser rasterization remains
    // a presentation fallback until the original GDI texture oracle is sampled.
    const key=JSON.stringify([text,record,metrics]);if(this.dialogueTextures.has(key))return this.dialogueTextures.get(key);
    const {style=0,color=0xd8d8d8,offset=0,spacing=0}=record,canvas=new OffscreenCanvas(metrics.sourceWidth,metrics.sourceHeight),ctx=canvas.getContext('2d'),serif=style===1;ctx.font=`${style===2?700:serif?600:400} ${style===2?15:32}px "${serif?'ＭＳ 明朝':'ＭＳ ゴシック'}", ${serif?'serif':'monospace'}`;ctx.textBaseline='top';
    const foreground='#'+[color&255,(color>>>8)&255,(color>>>16)&255].map(v=>v.toString(16).padStart(2,'0')).join(''),draw=(text,x)=>{ctx.fillStyle='#000';for(const [dx,dy]of [[2,4],[-2,4],[2,0],[-2,0]])ctx.fillText(text,x+dx,dy);ctx.fillStyle=foreground;ctx.fillText(text,x,2);};
    if(spacing){const bytes=Uint8Array.from(record.textHex.match(/../g)??[],v=>parseInt(v,16));for(let i=0;i<bytes.length;i+=2)draw(new TextDecoder('shift-jis').decode(bytes.slice(i,i+2)),2*offset+i*spacing);}
    else draw(text,2*offset+2);
    const g=this.gl,texture=g.createTexture();this.flush();g.bindTexture(g.TEXTURE_2D,texture);g.texImage2D(g.TEXTURE_2D,0,g.RGBA,g.RGBA,g.UNSIGNED_BYTE,canvas);g.texParameteri(g.TEXTURE_2D,g.TEXTURE_MIN_FILTER,g.LINEAR);g.texParameteri(g.TEXTURE_2D,g.TEXTURE_MAG_FILTER,g.LINEAR);g.texParameteri(g.TEXTURE_2D,g.TEXTURE_WRAP_S,g.CLAMP_TO_EDGE);g.texParameteri(g.TEXTURE_2D,g.TEXTURE_WRAP_T,g.CLAMP_TO_EDGE);
    const result={texture,w:canvas.width,h:canvas.height};this.dialogueTextures.set(key,result);if(this.dialogueTextures.size>16){const oldest=this.dialogueTextures.keys().next().value,old=this.dialogueTextures.get(oldest);this.dialogueTextures.delete(oldest);g.deleteTexture(old.texture);}return result;
  }
  dialogueLine(p){if(!p.text||p.a.alpha<=0)return;const texture=this.dialogueTexture(p.text,p.record,p.metrics);this.material(p.a.blend??0);this.texture(texture,!!p.a.point);this.primitive(this.gl.TRIANGLES);const points=quadCorners(p.x,p.y,p.metrics.width*p.a.sx,p.metrics.height*p.a.sy,p.a.rotation??0,p.a.anchorX,p.a.anchorY),uv=[[0,0],[1,0],[1,1],[0,1]];for(const i of [0,1,2,0,2,3])this.batch.push(...points[i],...uv[i],p.a.r??1,p.a.g??1,p.a.b??1,p.a.alpha*p.alpha,1,0);}
  render(core){
    this.begin();const h=readCoreHud(core),frame=h[0];if(frame<this.lastFrame)this.animations=new AnimationSampler(this.atlases);this.lastFrame=frame;this.animations.beginFrame();
    const stage=Math.max(1,Math.min(7,h[39]||1)),stageSuffix=String(stage).padStart(2,'0');if(this.stageBackgrounds.has(stage))this.background=this.stageBackgrounds.get(stage);else if(!this.stageJobs.has(stage))this.prepareStage(stage).catch(error=>this.stageError=error);
    const g=this.gl;g.enable(g.SCISSOR_TEST);g.scissor(32,16,384,448);
    const logicalBackground=readCoreBackground(core),background=this.background.geometry(logicalBackground?.frame??core._th12_background_frame?.()??frame,logicalBackground);g.uniform3fv(this.fogColor,background.camera.fog.slice(0,3));g.clearColor(...background.camera.fog.slice(0,3),1);g.clear(g.COLOR_BUFFER_BIT);
    this.primitive(g.TRIANGLES);if(background.authoritative||(core._th12_background_visible?.()??true))for(const triangle of background.triangles){const texture=this.textures.get(triangle.bank+':'+triangle.entry);if(!texture)continue;this.material(triangle.blend);this.texture(texture,triangle.point);for(const p of triangle.vertices)this.batch.push(p.x,p.y,p.u/texture.w,p.v/texture.h,p.r,p.g,p.b,p.alpha,p.depth,p.fog);}
    this.flush();
    const logicalPoses=readCoreAnimationPoses(core,stage),meshes=readCoreMeshes(core,stage),logicalOwners=new Set([...logicalPoses,...meshes].map(p=>p.owner)),message=readCoreMessage(core);
    const count=core._th12_draw(),ptr=core._th12_draw_ptr(),stride=(core._th12_draw_stride?.()||36)/4;if(stride<9||stride>64||!Number.isInteger(stride))throw Error('绘制记录长度无效');
    const f=new Float32Array(core.HEAPU8.buffer,ptr,count*stride),v=new Int32Array(core.HEAPU8.buffer,ptr,count*stride);
    const player='pl0'+h[17],poses=[...logicalPoses.filter(p=>!(p.a.name==='text'&&p.a.sprite>=0&&p.a.sprite<4)).map(p=>({...p,order:p.id})),...messageTextPoses(message,logicalPoses,this.atlases),...meshes,...(core._th12_mesh_draw?[]:readCoreLaserSegments(core,stage))];
    const add=(name,id,age,x,y,angle,scale,variant,options)=>{const {alpha,...sampling}=options;for(const a of this.animations.sampleAll(name,id,age,variant,sampling))poses.push({a,x,y,angle,scale,alpha,order:poses.length});};
    for(let i=0;i<count;i++){
      const k=i*stride,x=f[k],y=f[k+1],angle=f[k+2],scale=f[k+3],animation=v[k+5],kind=v[k+6],color=v[k+7],age=v[k+8],entityId=stride>=12?v[k+9]>>>0:0,interrupt=stride>=12?v[k+10]:0,alpha=stride>=12?f[k+11]:1;
      if(stride>=12&&logicalOwners.has(entityId))continue;
      const options={entityId,seed:entityId?entityId&65535:0x1234,alpha,...(interrupt?{interrupt}: {})};
      if(kind===12)poses.push({a:{layer:12},x,y,angle,scale,width:alpha,start:color>>>0,end:animation>>>0,beamTrail:true,order:poses.length});
      else if(kind===13)poses.push({a:{layer:12},x,y,angle,scale,alpha:1,start:color>>>0,end:animation>>>0,trail:true,order:poses.length});
      else if(kind===4||kind===5||kind===2||kind===7)add(player,animation,age,x,y,angle,scale,null,options);
      else if(kind===6){const {alpha:opacity,...sampling}=options,a=this.animations.sample(player,animation,age,null,sampling);if(a)poses.push({a,x,y,angle,scale,alpha:opacity,beam:true,order:poses.length});}
      else if(kind===1){const type=this.atlases.bullet.types?.[Math.floor(color/16)],variant=type?.colors[color%16];if(type)add('bullet',stride>=12?animation:type.script,age,x,y,angle,scale,variant,options);}
      else if(kind===3)add('bullet',animation>=165?animation:166+color,age,x,y,angle,scale,null,options);
      else {const bank=v[k+4],name=bank===0?'bullet':bank===1?'enemy':bank===2?'stgenm'+stageSuffix:bank===3?'stgenm'+stageSuffix+'m':null;if(name&&this.atlases[name])add(name,animation,age,x,y,angle,scale,null,options);}
    }
    if(message?.active){
      const events=message.events??[],windowEvent=[...events].reverse().find(e=>e[0]===3);
      // Native MSG opcode3 creates front script52. Use that original animation
      // while the message layer has not yet been registered as a core VM.
      if(windowEvent&&!logicalPoses.some(p=>p.a.name==='front'&&p.a.layer===19))add('front',52,Math.max(0,frame-windowEvent[1]),0,0,0,1,null,{alpha:1,entityId:'message-window'});
    }
    // Native ANM manager draws layers, so bullets/items/player child slots
    // cannot simply follow the core's entity array insertion order.
    poses.sort((a,b)=>(a.renderPass??0)-(b.renderPass??0)||((a.renderPass??0)>0?a.order-b.order:a.a.layer-b.a.layer||a.order-b.order));
    let playfieldClip=true;
    for(const p of poses){
      const clipped=!(p.logical&&p.a.name==='front'&&p.a.layer>=17);
      if(clipped!==playfieldClip){this.flush();if(clipped)g.enable(g.SCISSOR_TEST);else g.disable(g.SCISSOR_TEST);playfieldClip=clipped;}
      if(p.mesh)this.mesh(p);else if(p.laser)this.laserSegment(p);else if(p.dialogue)this.dialogueLine(p);else if(p.beamTrail)this.beamTrail(p.x,p.y,p.angle,p.scale,p.width,p.start,p.end);else if(p.trail)this.trail(p.x,p.y,p.angle,p.scale,p.start,p.end,p.alpha);else if(p.beam){const s=this.atlases[p.a.name].sprites[p.a.sprite];if(s)this.region(p.a.name,s.entry,s.x,s.y,s.w,s.h,p.x,p.y-448*p.scale/2,448*p.scale,14*p.a.sy,-Math.PI/2,p.a.alpha*p.alpha,[p.a.r,p.a.g,p.a.b],p.a.blend);}else this.state(p.a,p.x,p.y,p.angle,p.scale,p.alpha);
    }
    this.flush();g.disable(g.SCISSOR_TEST);
    this.animation('front',0,frame,0,0);
    this.hudText(groupedNumber(h[3]),620,72,1,true);this.hudText(groupedNumber(h[40]??h[3]),620,48,1,true);
    for(let i=0;i<8;i++){this.animation('front',13+i,frame,0,0,0,1,null,{interrupt:fragmentInterrupt(i,h[5],h[21])});this.animation('front',21+i,frame,0,0,0,1,null,{interrupt:fragmentInterrupt(i,h[6],h[22])});}
    this.hudText(Math.floor(h[4]/100)+'.',540,152);this.hudText(String(Math.round(h[4])%100).padStart(2,'0'),560,159,.6);this.hudText('/4.',574,152);this.hudText('00',606,159,.6);
    if(h.length>20)this.hudText(groupedNumber(Math.floor(h[20]/100/10)*10),620,176,1,true);this.hudText(groupedNumber(h[7]),620,200,1,true);
    if(h.length>25&&!logicalPoses.some(p=>p.a.name==='front'&&(p.owner>>>16)===0xfffe))for(let i=0;i<3;i++){const color=h[23+i]|0;this.animation('front',80,frame,0,0,0,1,null,{interrupts:[7+i,color?9+color:13,color?2:3]});}
    this.animation('front',69+h[19],frame,0,0);
    this.flush();this.animations.endFrame();return [...h];
  }
}
