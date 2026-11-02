// Ordered WebGL2 sprite batches. Presentation is a reader of core draw state.
import {AnimationSampler} from './animation.mjs';
import {StageBackground} from './stage-background.mjs';
import {readCoreSpell,spellTextDescriptors,bossTimerDescriptors,spellResultDescriptors} from './spell-presentation.mjs';
import {readCoreUfo,ufoTextPoses,ufoTextRenderPriority} from './ufo-presentation.mjs';
import {asciiGlyphs} from './ascii-presentation.mjs';
import {fpsTextDescriptors} from './fps-presentation.mjs';
import {stageClearText,stageTransitionPose} from './stage-clear-presentation.mjs';
import {readCoreDrawSchedule,orderPresentation,animationLayerPriority} from './draw-scheduler.mjs';
import {localizedHex,localizedFontFamily} from './localization/loader.mjs';
import {thcrapLayout} from './localization/layout.mjs';
import {capturePresentationOwners,interpolatePresentation,readCorePresentationMotion} from './presentation-interpolation.mjs';
import {VertexBatch} from './vertex-batch.mjs';
import {copyMeshPose,meshVertexCount,meshCoordinate,translateMesh} from './mesh-vertices.mjs';
export function readCoreHud(core,{highScore=0,scoreLowDigit=0}={}){const size=core._th12_hud_size?.()??20;if(!Number.isInteger(size)||size<20||size>256)throw Error('状态记录长度无效');const h=new Float32Array(core.HEAPU8.buffer,core._th12_hud(),size);if(typeof core._th12_score_stored!=='function')return h;const score=core._th12_score_stored();if(!Number.isInteger(score)||score< -0x80000000||score>0x7fffffff)throw Error('整数分数记录无效');const exact=[...h];exact[3]=score*10+scoreLowDigit;if(size>40)exact[40]=Math.max(exact[3],Number.isSafeInteger(highScore)&&highScore>=0?highScore:0);return exact;}
export function readCoreBossHud(core){
  if(!core._th12_boss_hud||!core._th12_boss_hud_size)return null;
  const ptr=core._th12_boss_hud(),size=core._th12_boss_hud_size();
  if(!Number.isInteger(ptr)||!Number.isInteger(size)||ptr<0||size<2||size>4096||ptr+size>core.HEAPU8.byteLength)throw Error('Boss 血条记录无效');
  const s=JSON.parse(new TextDecoder().decode(core.HEAPU8.subarray(ptr,ptr+size)));
  if(!Number.isFinite(s.fill)||!Number.isFinite(s.target)||!Array.isArray(s.markers)||s.markers.length!==4||!s.markers.every(m=>Array.isArray(m)&&m.length===2&&m.every(Number.isFinite)))throw Error('Boss 血条状态无效');
  return s;
}
export function bossHealthRectangles(s){
  if(!s||s.fill<=0)return [];
  // Native41f350..41f439 uses solid rectangles, not an ANM texture.
  const right=Math.fround(330*s.fill+41),rectangles=[{bounds:[41,23,right,25],argb:0xff000000},{bounds:[40,22,Math.fround(right-1),24],argb:0xffffffff}];
  for(const [fraction,argb]of s.markers)if(fraction!==0)rectangles.push({bounds:[40,22,Math.fround(330*Math.min(fraction,s.fill)+40),24],argb});
  return rectangles;
}
const animationBanks=new Map();
function animationBankNames(stage){let names=animationBanks.get(stage);if(!names){const suffix=String(stage).padStart(2,'0');names=['bullet','enemy','stgenm'+suffix,'stgenm'+suffix+'m','pl00','pl01','pl02','front','text','st'+suffix+'logo','stage'+suffix,'ascii'];animationBanks.set(stage,names);}return names;}
export function animationBankName(bank,stage=1){return animationBankNames(stage)[bank]??null;}
export function alwaysHitboxPoses(sampler,frame,x,y){
  // Use the complete settled77 ->76 marker, not the shrinking birth frame's
  // first quad. This display-only track never creates core VMs or consumes RNG.
  return sampler.sampleAll('bullet',77,Math.max(18,frame),null,{entityId:'always-hitbox',seed:0x1234})
    .filter(a=>a.visible&&a.sprite>=0&&a.alpha>0).map((a,index)=>({
      owner:0xffff0000,presentationKey:'player-hitbox:'+index,x,y,angle:0,scale:1,alpha:1,
      drawPriority:animationLayerPriority(16),clip:2,order:index,a}));
}
function originalMessageHex(hex){if(typeof hex!=='string'||hex.length%2||!/^[0-9a-f]*$/i.test(hex))throw Error('对话字符数据无效');return new TextDecoder('shift-jis').decode(Uint8Array.from(hex.match(/../g)??[],v=>parseInt(v,16)));}
export function decodeMessageHex(hex){return localizedHex('dialogue',hex)?.text??originalMessageHex(hex);}
export function readCoreMessage(core){if(!core._th12_message||!core._th12_message_size)return null;const ptr=core._th12_message(),size=core._th12_message_size();if(!Number.isInteger(size)||size<0||size>1024*1024||ptr<0||ptr+size>core.HEAPU8.byteLength)throw Error('对话状态记录无效');if(!size)return null;return JSON.parse(new TextDecoder().decode(core.HEAPU8.subarray(ptr,ptr+size)).replace(/\0+$/,''));}
export function readCoreBackground(core){
  if(!core._th12_background||!core._th12_background_size)return null;
  const ptr=core._th12_background(),size=core._th12_background_size();
  if(!Number.isInteger(ptr)||!Number.isInteger(size)||ptr<0||size<=0||size>4*1024*1024||ptr+size>core.HEAPU8.byteLength)throw Error('背景状态记录无效');
  const state=JSON.parse(new TextDecoder().decode(core.HEAPU8.subarray(ptr,ptr+size)).replace(/\0+$/,''));
  if(!state||![false,true,0,1].includes(state.loaded))throw Error('背景状态记录无效');
  if(state.camera?.screenShake!==undefined&&(!Array.isArray(state.camera.screenShake)||state.camera.screenShake.length!==2||!state.camera.screenShake.every(Number.isFinite)))throw Error('背景画面震动记录无效');
  if(state.loaded){const c=state.camera;if(!c||!['position','direction','up'].every(k=>Array.isArray(c[k])&&c[k].length===3&&c[k].every(Number.isFinite))||!Number.isFinite(c.fov)||!Array.isArray(c.fog)||c.fog.length!==5||!c.fog.every(Number.isFinite)||!Array.isArray(state.poses)||state.poses.length>65536||!state.poses.every(p=>Array.isArray(p)&&p.length>=21&&p.slice(0,21).every(Number.isFinite)))throw Error('背景相机或姿态记录无效');}
  return state;
}
const argbRgba=(argb,alpha=1)=>[((argb>>>16)&255)/255,((argb>>>8)&255)/255,(argb&255)/255,((argb>>>24)&255)/255*alpha];
const quadIndices=[0,1,2,0,2,3];
const zeroOffset=Object.freeze([0,0]);
export function beamSegmentVertices(x,y,angle,length,width,start,end){const dx=Math.cos(angle)*length/2,dy=Math.sin(angle)*length/2,nx=-Math.sin(angle)*width/2,ny=Math.cos(angle)*width/2;return [{x:x-dx-nx,y:y-dy-ny,color:start},{x:x-dx+nx,y:y-dy+ny,color:start},{x:x+dx+nx,y:y+dy+ny,color:end},{x:x+dx-nx,y:y+dy-ny,color:end}];}
export function dialogueMetrics(width=354,height=17){return {width,height,sourceWidth:Math.min(1024,2*width+22),sourceHeight:36};}
export function messageTextPoses(message,logicalPoses,atlases,{logicalText=false}={}){
  const roots=logicalPoses.filter(p=>p.a.name==='text'&&p.a.sprite>=0&&p.a.sprite<4),colors=[0xf8f08f,0x8088ff,0xd8d8d8],records=message?.textRecords??(message?.textHex??[]).map(textHex=>({textHex,offset:0,spacing:0,style:(message.flags>>1)&1,color:colors[message.speaker]??colors[2]}));
  const make=(p,slot)=>{let record=records[slot];if(!record?.textHex||!p.a.visible)return null;const translation=localizedHex('dialogue',record.textHex,{entry:message?.entry,offset:record.offset,spacing:record.spacing}),text=translation?.text??originalMessageHex(record.textHex),sprite=atlases.text.sprites[slot];if(translation)record={...record,offset:translation.offset??record.offset,spacing:translation.spacing??record.spacing,localizedWidths:translation.byteWidths,localizedRuby:translation.ruby};return {...p,dialogue:true,text,record,metrics:dialogueMetrics(sprite.w,sprite.h)};};
  if(roots.length||logicalText)return roots.map(p=>make(p,p.a.sprite)).filter(Boolean);
  if(!message?.active)return [];
  // Older cores have no logical text VMs. Coordinates below come from the
  // retail text scripts and MSG engine offset; modern cores use Pose above.
  return records.map((record,slot)=>make({a:{name:'text',sprite:slot,layer:20,visible:true,alpha:1,sx:1,sy:1,anchorX:1,anchorY:1},x:56,y:(slot<2?373:367)+22*(slot%2),alpha:1,order:logicalPoses.length+slot},slot)).filter(Boolean);
}
export function readCoreAnimationPoses(core,stage=1){
  if(!core._th12_anm_draw||!core._th12_anm_draw_ptr)return [];
  const count=core._th12_anm_draw(),ptr=core._th12_anm_draw_ptr(),stride=core._th12_anm_draw_stride?.()??128;
  if(!Number.isInteger(count)||count<0||count>65536||stride!==128||!Number.isInteger(ptr)||ptr<0||ptr%4!==0||ptr+count*stride>core.HEAPU8.byteLength)throw Error('逻辑动画姿态记录无效');
  const f=new Float32Array(core.HEAPU8.buffer),v=new Int32Array(core.HEAPU8.buffer),u=new Uint32Array(core.HEAPU8.buffer),banks=animationBankNames(stage),rgba=argb=>[((argb>>>16)&255)/255,((argb>>>8)&255)/255,(argb&255)/255,((argb>>>24)&255)/255],poses=[];
  for(let i=0;i<count;i++){const k=(ptr>>2)+i*32,flags=u[k+24],flags2=u[k+25],primary=rgba(u[k+14]),secondary=rgba(u[k+15]),color=flags&0x10000?secondary:primary;
    const a={name:banks[v[k+16]]??null,sprite:v[k+17],layer:v[k+18],mode:v[k+19],anchorX:v[k+20],anchorY:v[k+21],blend:v[k+22],geometryCount:v[k+23],
      x:0,y:0,z:f[k+2],rx:f[k+3],ry:f[k+4],rotation:f[k+5],sx:f[k+6],sy:f[k+7],u:f[k+8],v:f[k+9],uvScaleX:f[k+10],uvScaleY:f[k+11],geometryRadius:f[k+12],geometryWidth:f[k+13],
      r:color[0],g:color[1],b:color[2],alpha:color[3],primaryColor:primary,r2:secondary[0],g2:secondary[1],b2:secondary[2],alpha2:secondary[3],point:!!(flags2&2),visible:!!(flags&1),rotate:v[k+19]===1||v[k+19]===3};
    if(u[k+31]===1){a.nativeWidth=f[k+29];a.nativeHeight=f[k+30];}
    const owner=u[k+27];a.visible=(flags&3)===3;
    // Original45c93e/45c95b tests primary alpha even when flag10000 selects
    // secondary color. A secondary alpha cannot revive a faded mode0/1 quad.
    if((a.mode===0||a.mode===1)&&primary[3]===0)a.visible=false;
    poses.push({a,x:f[k+0],y:f[k+1],angle:0,scale:1,alpha:f[k+28],id:u[k+26],owner,logical:true,order:i,drawPriority:undefined,index:undefined,clip:undefined,clipped:undefined});
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
  const heap=core.HEAPU8,view=new DataView(heap.buffer,heap.byteOffset,heap.byteLength),meshes=[];
  // One owned copy before another export can grow WASM memory or replace
  // the draw queue. All meshes share views of this immutable snapshot.
  const bytes=heap.slice(vertexPtr,vertexPtr+vertexCount*vertexStride),floats=new Float32Array(bytes.buffer,bytes.byteOffset,vertexCount*7),colors=new Uint32Array(bytes.buffer,bytes.byteOffset,vertexCount*7);
  for(let i=0;i<count;i++){
    const at=ptr+i*stride,I=n=>view.getInt32(at+n*4,true),U=n=>view.getUint32(at+n*4,true),offset=U(10),length=U(11),primitive=U(5),pass=U(6);
    if(offset+length>vertexCount||![3,4,5].includes(primitive)||pass>2)throw Error('网格顶点范围无效');
    meshes.push(copyMeshPose({mesh:true,vertexData:{floats,colors,offset,count:length},primitive,untextured:I(0)===-1&&I(1)===-1,renderPass:pass,drawPriority:I(7),owner:U(8),id:U(9),order:i,filter:U(12),addressFlags:U(13),a:{name:animationBankName(I(0),stage),sprite:I(1),entry:I(2),layer:I(3),blend:I(4)}}));
  }return meshes;
}
// Stage draw2/5 installs camera2's screen offset; draw42 clears it before
// layer20 and the HUD. These are drawing coordinates, never game positions.
export function shakePresentation(pose,offset=zeroOffset){
  offset=offset.map(Math.fround); // Snapshot JSON transports float32 globals.
  if(pose.drawPriority>=42||(!offset[0]&&!offset[1]))return pose;
  const shift=(x,y)=>[Math.fround(x+offset[0]),Math.fround(y+offset[1])];
  // 459e50 shifts flat textured quads. 45c3a0/45d5d0 submits already formed
  // XYZRHW strips/lines without camera translation; solid14..20 do likewise.
  if(pose.mesh)return pose.primitive===4&&!pose.untextured?translateMesh(pose,offset[0],offset[1],true):pose;
  if(pose.a?.mode>=14)return pose;
  // Ordinary quads construct their native corners before camera translation.
  // Pre-shifting a center would change mode0's floor/nearest-even boundaries.
  if(pose.nativeText)return {...pose,screenShake:offset};
  if(pose.a&&(pose.a.mode===0||pose.a.mode===1))return {...pose,screenShake:offset};
  if(Number.isFinite(pose.x)&&Number.isFinite(pose.y)){const [x,y]=shift(pose.x,pose.y);return {...pose,x,y};}
  return pose;
}
export function fragmentInterrupt(index,whole,fragments=0){return index<whole?2:index===whole?7+Math.max(0,Math.min(5,fragments|0)):3;}
export function groupedNumber(n){return String(Math.max(0,Math.round(n))).replace(/\B(?=(\d{3})+(?!\d))/g,',');}
// Original ASCII font 3 (0x401d97): 16px sprites, 12px numeral advance,
// 4px comma advance, and comma shifted down 3px. HUD calls: 0x41f473..
export function hudGlyphs(text,x,y,scale=1,right=false){text=String(text);const advance=c=>(c===','?4:12)*scale;if(right)x-=[...text].reduce((w,c)=>w+advance(c),0);return [...text].map(c=>{const id=c===','?257:c==='/'?253:c==='.'?254:c==='s'?255:c==='*'?256:c.charCodeAt(0)+195,pose={id,x,y:y+(c===','?3:0),scale};x+=advance(c);return pose;});}
// Native mode15 circle helper 0x45d430 writes a triangle fan: primary color
// at its center, secondary color at count+1 circumference vertices.
export function circleVertices(x,y,radius,angle,segments,centerColor,edgeColor){const f=Math.fround,pi=3.1415927410125732,tau=6.2831854820251465,step=f(6.283185307179586/segments),vertices=[{x,y,color:centerColor}];angle=f(angle);for(let i=0;i<=segments;i++){vertices.push({x:f(x+f(Math.cos(angle)*radius)),y:f(y+f(Math.sin(angle)*radius)),color:edgeColor});angle=f(angle+step);while(angle>pi)angle=f(angle-tau);while(angle<-pi)angle=f(angle+tau);}return vertices;}
// Native45d710 is a diffuse line strip with count+1 perimeter points. Mode16
// is used by the small UFO completion hint and the two large UFO circles.
export function outlineVertices(x,y,radius,angle,segments,color){
  const f=Math.fround,pi=3.1415927410125732,tau=6.2831854820251465,step=f(tau/segments),vertices=[];
  [x,y,radius,angle]=[x,y,radius,angle].map(f);
  for(let i=0;i<=segments;i++){
    vertices.push({x:f(x+f(Math.cos(angle)*radius)),y:f(y+f(Math.sin(angle)*radius)),color});
    angle=f(angle+step);while(angle>pi)angle=f(angle-tau);while(angle<-pi)angle=f(angle+tau);
  }
  return vertices;
}
// 459e50 uses x87 nearest-even before the D3D half-pixel adjustment. WebGL
// has no D3D half-pixel offset, but must retain that rounding at exact halves.
export function nativePixel(n){const floor=Math.floor(n),fraction=n-floor;return fraction===.5?floor+(Math.abs(floor%2)===1?1:0):Math.round(n);}
export function quadCorners(x,y,w,h,angle=0,anchorX=0,anchorY=0,pixel=false,out=[[0,0],[0,0],[0,0],[0,0]]){
  const left=anchorX===1?0:anchorX===2?-w:-w/2,top=anchorY===1?0:anchorY===2?-h:-h/2,c=Math.cos(angle),s=Math.sin(angle);
  for(let i=0;i<4;i++){const a=i===1||i===2?left+w:left,b=i>=2?top+h:top,px=x+a*c-b*s,py=y+a*s+b*c;out[i][0]=pixel?nativePixel(px):px;out[i][1]=pixel?nativePixel(py):py;}
  return out;
}
// Original mode0:45a570 floors centered left/top, then459e50 adds screen
// offset and rounds nearest-even. Mode1:45af10 rotates first and never rounds,
// including zero rotation. WebGL omits D3D's final half-pixel subtraction.
export function nativeQuadCorners(x,y,w,h,angle=0,anchorX=0,anchorY=0,mode=0,offset=zeroOffset,out=[[0,0],[0,0],[0,0],[0,0]]){
  const f=Math.fround;x=f(x);y=f(y);w=f(w);h=f(h);angle=f(angle);const ox=f(offset[0]),oy=f(offset[1]);
  if(mode===0){
    const left=anchorX===1?x:anchorX===2?f(x-w):Math.floor(f(x-w/2)),
      top=anchorY===1?y:anchorY===2?f(y-h):Math.floor(f(y-h/2)),
      right=anchorX===2?x:f(left+w),bottom=anchorY===2?y:f(top+h);
    const l=nativePixel(f(left+ox)),r=nativePixel(f(right+ox)),t=nativePixel(f(top+oy)),b=nativePixel(f(bottom+oy));
    out[0][0]=out[3][0]=l;out[1][0]=out[2][0]=r;out[0][1]=out[1][1]=t;out[2][1]=out[3][1]=b;
    return out;
  }
  const left=anchorX===1?0:anchorX===2?-w:f(-w/2),
    top=anchorY===1?0:anchorY===2?-h:f(-h/2),c=f(Math.cos(angle)),s=f(Math.sin(angle));
  const right=f(left+w),bottom=f(top+h);
  for(let i=0;i<4;i++){const a=i===1||i===2?right:left,b=i>=2?bottom:top;out[i][0]=f(f(x+a*c-b*s)+ox);out[i][1]=f(f(y+a*s+b*c)+oy);}
  return out;
}
export class Renderer {
  constructor(canvas,atlases,{fetcher=(...args)=>globalThis.fetch(...args)}={}){
    this.fetch=fetcher;this.canvas=canvas;this.atlases=atlases;this.textures=new Map();this.dialogueTextures=new Map();this.batch=new VertexBatch();this.current=null;this.blend=0;this.animations=new AnimationSampler(atlases);this.stageBackgrounds=new Map();this.stageJobs=new Map();this.requestedStage=1;
    const gl=this.gl=canvas.getContext('webgl2',{alpha:false,antialias:false});if(!gl)throw Error('浏览器需要支持 WebGL 2');
    const compile=(type,source)=>{const s=gl.createShader(type);gl.shaderSource(s,source);gl.compileShader(s);if(!gl.getShaderParameter(s,gl.COMPILE_STATUS))throw Error(gl.getShaderInfoLog(s));return s;};
    const p=gl.createProgram();gl.attachShader(p,compile(gl.VERTEX_SHADER,`#version 300 es
      precision highp float;layout(location=0)in vec2 position;layout(location=1)in vec2 uv;layout(location=2)in vec4 color;layout(location=3)in vec2 depthFog;out vec2 texcoord;out vec4 tint;out float fog;
      void main(){gl_Position=vec4((position.x/320.0-1.0)*depthFog.x,(1.0-position.y/240.0)*depthFog.x,0,depthFog.x);texcoord=uv;tint=color;fog=depthFog.y;}`));
    gl.attachShader(p,compile(gl.FRAGMENT_SHADER,`#version 300 es
      precision mediump float;uniform sampler2D atlas;uniform vec3 fogColor;in vec2 texcoord;in vec4 tint;in float fog;out vec4 pixel;
      // Native4512c8 sets ALPHAREF=1/255 and ALPHAFUNC=GREATEREQUAL.
      void main(){pixel=texture(atlas,texcoord)*tint;pixel.rgb=mix(pixel.rgb,fogColor,clamp(fog,0.0,1.0));if(pixel.a<1.0/255.0)discard;}`));
    gl.linkProgram(p);if(!gl.getProgramParameter(p,gl.LINK_STATUS))throw Error(gl.getProgramInfoLog(p));gl.useProgram(p);
    this.buffer=gl.createBuffer();gl.bindBuffer(gl.ARRAY_BUFFER,this.buffer);this.vao=gl.createVertexArray();gl.bindVertexArray(this.vao);
    for(const [slot,count,offset]of [[0,2,0],[1,2,8],[2,4,16],[3,2,32]]){gl.enableVertexAttribArray(slot);gl.vertexAttribPointer(slot,count,gl.FLOAT,false,40,offset);}
    this.fogColor=gl.getUniformLocation(p,'fogColor');
    const solid=gl.createTexture();gl.bindTexture(gl.TEXTURE_2D,solid);gl.texImage2D(gl.TEXTURE_2D,0,gl.RGBA,1,1,0,gl.RGBA,gl.UNSIGNED_BYTE,new Uint8Array([255,255,255,255]));gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_MIN_FILTER,gl.NEAREST);gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_MAG_FILTER,gl.NEAREST);this.solid={texture:solid,w:1,h:1};
    gl.enable(gl.BLEND);gl.blendFunc(gl.SRC_ALPHA,gl.ONE_MINUS_SRC_ALPHA);
    this.uploadCount=0;this.drawCount=0;this.lastFrame=-1;this.presentationEpoch=0;this.point=null;this.stageSurfaceCopied=false;this.stageSurfaceComposited=false;this.stageSurfaceAvailable=false;this.surfaceCopyError=0;
  }
  async prepare(){
    await this.loadAtlases(['pl00','pl01','pl02','bullet','enemy','front','ascii','text']);await this.prepareStage(1);
  }
  async prepareStage(stage,{activate=true}={}){
    if(!Number.isInteger(stage)||stage<1||stage>7)throw Error('关卡编号无效');
    if(activate)this.requestedStage=stage;
    if(!this.stageJobs.has(stage)){const name='stage'+String(stage).padStart(2,'0'),enemy='stgenm'+String(stage).padStart(2,'0'),logo='st'+String(stage).padStart(2,'0')+'logo';
      const job=(async()=>{const response=await this.fetch('/assets/'+name+'-background.json');if(!response.ok)throw Error('关卡背景数据加载失败 '+stage);const background=new StageBackground(await response.json(),this.atlases);await this.loadAtlases([name,enemy,enemy+'m',logo].filter(n=>this.atlases[n]));this.stageBackgrounds.set(stage,background);return background;})();this.stageJobs.set(stage,job);}
    const background=await this.stageJobs.get(stage);if(activate&&this.requestedStage===stage)this.background=background;return background;
  }
  async loadAtlases(names){
    const gl=this.gl;
    // Atlas metadata may include virtual duplicate names; each physical entry
    // gets its own generated image, so images never silently overwrite others.
    const entries=[];for(const name of names){const atlas=this.atlases[name];if(!atlas)throw Error('动画资源不存在 '+name);for(let index=0;index<atlas.entries.length;index++){const e=atlas.entries[index];if(e.image&&!this.textures.has(name+':'+index))entries.push({name,index,e});}}
    for(let start=0;start<entries.length;start+=8)await Promise.all(entries.slice(start,start+8).map(async({name,index,e})=>{
      const response=await this.fetch('/assets/'+e.image);if(!response.ok)throw Error('贴图加载失败 '+e.image);
      const bitmap=await createImageBitmap(await response.blob(),{premultiplyAlpha:'none',colorSpaceConversion:'none'}),texture=gl.createTexture();
      gl.bindTexture(gl.TEXTURE_2D,texture);gl.pixelStorei(gl.UNPACK_PREMULTIPLY_ALPHA_WEBGL,false);gl.texImage2D(gl.TEXTURE_2D,0,gl.RGBA,gl.RGBA,gl.UNSIGNED_BYTE,bitmap);
      gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_MIN_FILTER,gl.LINEAR);gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_MAG_FILTER,gl.LINEAR);const wrap=gl.REPEAT; // Native4514c5/4514db defaults both texture axes to WRAP1.
      gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_WRAP_S,wrap);gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_WRAP_T,wrap);
      this.textures.set(name+':'+index,{texture,w:bitmap.width,h:bitmap.height,defaultWrap:wrap});bitmap.close();
    }));
  }
  unloadAtlases(names){
    this.flush();const prefixes=names.map(name=>name+':');
    for(const [key,texture]of this.textures)if(prefixes.some(prefix=>key.startsWith(prefix))){if(this.current===texture)this.current=null;this.gl.deleteTexture(texture.texture);this.textures.delete(key);}
  }
  begin(clearColor=[.035,.035,.055,1]){const g=this.gl;g.viewport(0,0,640,480);g.clearColor(...clearColor);g.clear(g.COLOR_BUFFER_BIT);g.bindVertexArray(this.vao);g.bindBuffer(g.ARRAY_BUFFER,this.buffer);this.drawCount=0;this.uploadCount=0;this.current=null;this.batch.length=0;this.batchMode=g.TRIANGLES;this.blend=-1;this.stageSurfaceCopied=false;this.stageSurfaceComposited=false;this.stageSurfaceAvailable=false;this.surfaceCopyError=0;this.material(0);}
  flush(){if(!this.batch.length)return;const g=this.gl;g.bindTexture(g.TEXTURE_2D,this.current.texture);g.bufferData(g.ARRAY_BUFFER,this.batch.data,g.STREAM_DRAW,0,this.batch.length);g.drawArrays(this.batchMode,0,this.batch.length/10);this.drawCount++;this.uploadCount++;this.batch.length=0;}
  primitive(mode){if(this.batchMode!==mode){this.flush();this.batchMode=mode;}}
  material(blend){if(this.blend===blend)return;this.flush();const g=this.gl;const factors=[[g.SRC_ALPHA,g.ONE_MINUS_SRC_ALPHA],[g.SRC_ALPHA,g.ONE],[g.ZERO,g.ONE_MINUS_SRC_COLOR],[g.ONE,g.ZERO],[g.ONE_MINUS_DST_COLOR,g.ONE_MINUS_SRC_ALPHA],[g.DST_COLOR,g.ZERO],[g.ONE_MINUS_SRC_COLOR,g.ONE_MINUS_SRC_ALPHA]];g.blendFunc(...(factors[blend]||factors[0]));this.blend=blend;}
  region(name,entry,sx,sy,sw,sh,x,y,w,h,angle=0,alpha=1,tint=[1,1,1],blend=0,options={},screenShake=options.screenShake,presentationOffset=options.presentationOffset){
    const texture=this.textures.get(name+':'+entry);if(!texture||texture.available===false)return;
    this.material(blend);this.texture(texture,!!options.point);this.primitive(this.gl.TRIANGLES);
    const scratch=this.quadScratch??=[[0,0],[0,0],[0,0],[0,0]],mode=options.nativeMode??options.mode;
    const corners=mode===0||mode===1
      ?nativeQuadCorners(x,y,w,h,angle,options.anchorX||0,options.anchorY||0,mode,screenShake,scratch)
      :quadCorners(x,y,w,h,angle,options.anchorX||0,options.anchorY||0,!!options.pixel,scratch),du=options.u||0,dv=options.v||0;
    const endX=sx+sw*(options.uvScaleX??1),endY=sy+sh*(options.uvScaleY??1);
    const u0=sx/texture.w+du,u1=endX/texture.w+du,v0=sy/texture.h+dv,v1=endY/texture.h+dv;
    // Native45af10 mode1 keeps D3D9 integer pixel centers. Convert at
    // submission to WebGL half-integer centers; otherwise a 1px warning
    // samples only the transparent edges of its vertically compressed sprite.
    // Mode0 already removes native459e50's half-pixel subtraction above.
    const rasterOffset=mode===1?.5:0,ox=(presentationOffset?.[0]??0)+rasterOffset,oy=(presentationOffset?.[1]??0)+rasterOffset,r=tint[0]??tint.r,g=tint[1]??tint.g,b=tint[2]??tint.b;
    if(this.batch instanceof VertexBatch)this.batch.quad(corners,u0,u1,v0,v1,texture.flipY,r,g,b,alpha,ox,oy);
    else for(const i of quadIndices){const u=i===1||i===2?u1:u0,v=i>=2?v1:v0;this.batch.push(corners[i][0]+ox,corners[i][1]+oy,u,texture.flipY?1-v:v,r,g,b,alpha,1,0);}
  }
  texture(texture,point=false,addressFlags=null){const g=this.gl,wrapU=addressFlags===null?(texture.defaultWrap??g.CLAMP_TO_EDGE):addressFlags&1?g.REPEAT:g.CLAMP_TO_EDGE,wrapV=addressFlags===null?(texture.defaultWrap??g.CLAMP_TO_EDGE):addressFlags&2?g.REPEAT:g.CLAMP_TO_EDGE;if(this.current!==texture||this.point!==point||this.wrapU!==wrapU||this.wrapV!==wrapV){this.flush();this.current=texture;this.point=point;this.wrapU=wrapU;this.wrapV=wrapV;g.bindTexture(g.TEXTURE_2D,texture.texture);this.textureParameters(texture,point,wrapU,wrapV);}}
  textureParameters(texture,point,wrapU,wrapV){
    // Sampling belongs to each texture, not to the last bound texture. Keep
    // render-target writes on this path too so a capture cannot stale the cache.
    const states=this.textureStates??=new WeakMap(),state=states.get(texture),g=this.gl;
    if(!state||state.point!==point){
      g.texParameteri(g.TEXTURE_2D,g.TEXTURE_MIN_FILTER,point?g.NEAREST:g.LINEAR);
      g.texParameteri(g.TEXTURE_2D,g.TEXTURE_MAG_FILTER,point?g.NEAREST:g.LINEAR);
    }
    if(!state||state.wrapU!==wrapU)g.texParameteri(g.TEXTURE_2D,g.TEXTURE_WRAP_S,wrapU);
    if(!state||state.wrapV!==wrapV)g.texParameteri(g.TEXTURE_2D,g.TEXTURE_WRAP_T,wrapV);
    if(state){state.point=point;state.wrapU=wrapU;state.wrapV=wrapV;}
    else states.set(texture,{point,wrapU,wrapV});
  }
  mesh(p,screenShake=zeroOffset){
    const texture=p.untextured?this.solid:this.textures.get(p.a.name+':'+p.a.entry);if(!texture||texture.available===false)return;
    this.material(p.a.blend);this.texture(texture,p.untextured||p.filter===1,p.addressFlags);
    this.primitive(p.primitive===3?this.gl.LINE_STRIP:this.gl.TRIANGLES);
    const ox=Math.fround(screenShake[0]),oy=Math.fround(screenShake[1]),shake=p.primitive===4&&!p.untextured&&(ox||oy);
    const data=p.vertexData,count=meshVertexCount(p);
    const emit=data?i=>{
      const at=(data.offset+i)*7,f=data.floats,color=data.colors[at+4],x=meshCoordinate(p,i,0),y=meshCoordinate(p,i,1);
      this.batch.push(shake?Math.fround(x+ox):x,shake?Math.fround(y+oy):y,p.untextured ? .5 : f[at+5],p.untextured ? .5 : texture.flipY?1-f[at+6]:f[at+6],((color>>>16)&255)/255,((color>>>8)&255)/255,(color&255)/255,(color>>>24)/255,f[at+3]?1/f[at+3]:1,0);
    }:i=>{
      const v=p.vertices[i];this.batch.push(shake?Math.fround(v.x+ox):v.x,shake?Math.fround(v.y+oy):v.y,p.untextured ? .5 : v.u,p.untextured ? .5 : texture.flipY?1-v.v:v.v,((v.color>>>16)&255)/255,((v.color>>>8)&255)/255,(v.color&255)/255,(v.color>>>24)/255,v.rhw?1/v.rhw:1,0);
    };
    // Native45d5d0 submits each trail separately; batching two line strips
    // together would draw a spurious segment between their endpoints.
    if(p.primitive===3){this.flush();for(let i=0;i<count;i++)emit(i);this.flush();return;}
    if(p.primitive===4)for(let i=0;i<count;i++)emit(i);
    else for(let i=2;i<count;i++){emit(i&1?i-1:i-2);emit(i&1?i-2:i-1);emit(i);}
  }
  stageSurface(priority,screenShake=zeroOffset,clip=1){
    //431630 aliases text@R entry2 to the first native target.42f200 ends
    //that same-frame background pass at draw12;42f3b0 copies it at14 before
    //the layer28 strips. This texture never includes later Player/enemy/HUD.
    const g=this.gl;
    if(priority>=12&&!this.stageSurfaceCopied){
      this.flush();let texture=this.textures.get('text:2');
      if(!texture){texture={texture:g.createTexture(),w:640,h:480,defaultWrap:g.REPEAT,flipY:true};this.textures.set('text:2',texture);}
      //The renderer's alpha:false back buffer is RGB. Copying it to RGBA
      //attempts to add an absent component and can fail INVALID_OPERATION.
      //The original @R uses the opaque display format; RGB also samples A=1.
      g.bindTexture(g.TEXTURE_2D,texture.texture);
      if(texture.surfaceInitialized&&g.copyTexSubImage2D){
        // Reuse verified640x480 RGB storage. Reallocation and synchronous
        // getError on every frame force avoidable mobile driver round trips.
        g.copyTexSubImage2D(g.TEXTURE_2D,0,0,0,0,0,640,480);this.surfaceCopyError=0;
      }else{
        g.copyTexImage2D(g.TEXTURE_2D,0,g.RGB,0,0,640,480,0);
        this.surfaceCopyError=g.getError?.()??0;
        texture.surfaceInitialized=this.surfaceCopyError===(g.NO_ERROR??0);
      }
      texture.available=this.surfaceCopyError===(g.NO_ERROR??0);
      this.current=null;this.point=null;this.stageSurfaceCopied=true;
      this.stageSurfaceAvailable=texture.available;
      if(!texture.available)return; //Retain the already rendered background.
      this.textureParameters(texture,false,g.REPEAT,g.REPEAT);
      //Native42f200 clears the destination target before any layer5/prio13
      //draw. Keeping the previous background here would leave ghost strips
      //at the exposed edges when Main81 moves during Camera shake.
      g.enable(g.SCISSOR_TEST);g.scissor(13,0,422,480);g.clearColor(0,0,0,1);g.clear(g.COLOR_BUFFER_BIT);
      if(clip===0)g.disable(g.SCISSOR_TEST);else if(clip===1)g.scissor(32,16,384,448);
    }
    if(priority>=14&&!this.stageSurfaceComposited){
      if(!this.stageSurfaceAvailable){this.stageSurfaceComposited=true;return;}
      //The original main script81's ordinary held pose copies sprite75 using
      //blend3. Preserve that copy even when layer5/prio13 drew into the next
      //target. A later typed Main-VM interrupt can override this normal pose.
      this.region('text',2,32,16,384,448,32,16,384,448,0,1,[1,1,1],3,
        {nativeMode:0,anchorX:1,anchorY:1,screenShake});
      this.flush();this.stageSurfaceComposited=true;
    }
  }
  sprite(name,id,x,y,angle=0,scale=1,alpha=1){const s=this.atlases[name]?.sprites[id];if(s)this.region(name,s.entry,s.x,s.y,s.w,s.h,x,y,s.w*scale,s.h*scale,angle,alpha);}
  solidTriangles(vertices,indices,blend){this.material(blend);this.texture(this.solid,true);this.primitive(this.gl.TRIANGLES);for(const i of indices){const p=vertices[i];this.batch.push(p.x,p.y,.5,.5,...p.color,1,0);}}
  solidLineStrip(vertices,blend){this.material(blend);this.texture(this.solid,true);this.primitive(this.gl.LINE_STRIP);this.flush();for(const p of vertices)this.batch.push(p.x,p.y,.5,.5,...p.color,1,0);this.flush();}
  trail(x,y,angle,length,start,end,alpha){const dx=Math.cos(angle)*length/2,dy=Math.sin(angle)*length/2;this.material(0);this.texture(this.solid,true);this.primitive(this.gl.LINES);this.batch.push(x-dx,y-dy,.5,.5,...argbRgba(start,alpha),1,0);this.batch.push(x+dx,y+dy,.5,.5,...argbRgba(end,alpha),1,0);}
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
  state(a,x,y,angle=0,scale=1,alpha=1,screenShake=zeroOffset,presentationOffset=zeroOffset){
    if(!a.visible)return;
    // Native mode0 floors/rounds its authored current corners. Apply the
    // owner offset after that boundary to retain high-refresh subpixels.
    if(a.mode!==0&&a.mode!==1){x+=presentationOffset[0];y+=presentationOffset[1];}
    if(a.mode===18||a.mode===20){
      // Native mode18 (opcode108),45cbaa ->45d0a0, uses an untextured
      // gradient quad for the two halves of the selection highlight.
      // The right vertices carry primary color, the left secondary color;
      // the gradient rotates together with the quad.
      const right=a.primaryColor??[a.r,a.g,a.b,a.alpha],left=[a.r2,a.g2,a.b2,a.alpha2],points=quadCorners(x+a.x,y+a.y,a.geometryRadius*a.sx*scale,a.geometryWidth*a.sy*scale,a.rotation+angle);
      const colors=[left,right,right,left].map(c=>[c[0],c[1],c[2],c[3]*alpha]);
      this.solidTriangles(points.map(([x,y],i)=>({x,y,color:colors[i]})),[0,1,2,0,2,3],a.blend);return;
    }
    if(a.mode===14){if(a.alpha<=0)return;const color=[a.r,a.g,a.b,a.alpha*alpha],corners=quadCorners(x+a.x,y+a.y,a.geometryRadius*a.sx*scale,a.geometryWidth*a.sy*scale,a.rotation+angle);this.solidTriangles(corners.map(([x,y])=>({x,y,color})),[0,1,2,0,2,3],a.blend);return;}
    if(a.mode===15||a.mode===16){const count=a.geometryCount;if(count<3||count>512)return;const primary=a.primaryColor??[a.r,a.g,a.b,a.alpha],center=[primary[0],primary[1],primary[2],primary[3]*alpha],edge=[a.r2,a.g2,a.b2,a.alpha2*alpha];if(a.mode===16){if(center[3]>0)this.solidLineStrip(outlineVertices(x+a.x,y+a.y,Math.fround(a.geometryRadius*a.sx*scale),a.rotation+angle,count,center),a.blend);return;}if(center[3]<=0&&edge[3]<=0)return;const vertices=circleVertices(x+a.x,y+a.y,a.geometryRadius*a.sx*scale,a.rotation+angle,count,center,edge),indices=[];for(let i=1;i<=count;i++)indices.push(0,i,i+1);this.solidTriangles(vertices,indices,a.blend);return;}
    const s=this.atlases[a.name]?.sprites[a.sprite];if(!s||a.alpha<=0)return;this.region(a.name,s.entry,s.x,s.y,s.w,s.h,x+a.x,y+a.y,(a.nativeWidth!==undefined&&a.nativeWidth!==-1?a.nativeWidth:s.w)*scale*a.sx,(a.nativeHeight!==undefined&&a.nativeHeight!==-1?a.nativeHeight:s.h)*scale*a.sy,a.rotate?a.rotation+angle:0,a.alpha*alpha,a,a.blend,a,screenShake,a.mode===0||a.mode===1?presentationOffset:undefined);
  }
  animation(name,id,age,x,y,angle=0,scale=1,variant=null,options={}){const {alpha=1,...sampling}=options;for(const a of this.animations.sampleAll(name,id,age,variant,sampling))this.state(a,x,y,angle,scale,alpha);}
  text(text,x,y,scale=1,tint=[1,1,1]){for(const c of String(text)){const s=this.atlases.ascii.sprites[c.charCodeAt(0)-32];if(s)this.region('ascii',s.entry,s.x,s.y,s.w,s.h,x+s.w*scale/2,y+s.h*scale/2,s.w*scale,s.h*scale,0,1,tint);x+=14*scale;}}
  hudText(text,x,y,scale=1,right=false){for(const p of hudGlyphs(text,x,y,scale,right)){const s=this.atlases.ascii.sprites[p.id];if(s)this.region('ascii',s.entry,s.x,s.y,s.w,s.h,p.x,p.y,s.w*scale,s.h*scale,0,1,[1,1,1],0,{anchorX:1,anchorY:1,point:true});}}
  nativeText(descriptor,screenShake=zeroOffset){for(const p of asciiGlyphs(descriptor)){const s=this.atlases.ascii.sprites[p.id];if(!s)continue;const color=argbRgba(p.colorARGB);this.region('ascii',s.entry,s.x,s.y,s.w,s.h,p.x,p.y,s.w*p.scaleX,s.h*p.scaleY,0,color[3],color.slice(0,3),0,{anchorX:p.anchorX,anchorY:p.anchorY,point:p.point,pixel:true,nativeMode:0,screenShake});}}
  hudCounters(h){
    this.hudText(groupedNumber(h[3]),620,72,1,true);this.hudText(groupedNumber(h[40]??h[3]),620,48,1,true);
    this.hudText(Math.floor(h[4]/100)+'.',540,152);this.hudText(String(Math.round(h[4])%100).padStart(2,'0'),560,159,.6);this.hudText('/4.',574,152);this.hudText('00',606,159,.6);
    if(h.length>20)this.hudText(groupedNumber(Math.floor(h[20]/100/10)*10),620,176,1,true);this.hudText(groupedNumber(h[7]),620,200,1,true);
  }
  dialogueTexture(text,record,metrics,raster=null){
    // Original 44e56f..44e5c6 selects 32px GDI MS Gothic (400) / MS Mincho
    // (600), then text rendering reduces by two. Browser rasterization remains
    // a presentation fallback until the original GDI texture oracle is sampled.
    const cache=raster?.menu?(this.menuTextTextures??=new Map()):this.dialogueTextures;
    const family=localizedFontFamily(),key=JSON.stringify([text,record,metrics,raster,family,family?this.localizationTabs??[]:null]);
    if(cache.has(key)){const result=cache.get(key);if(family)this.localizationTabs=[...result.tabs];return result;}
    const {style=0,color=0xd8d8d8,offset=0,spacing=0}=record,canvas=new OffscreenCanvas(metrics.sourceWidth,metrics.sourceHeight),ctx=canvas.getContext('2d'),serif=style===1;ctx.font=`${raster?.fontWeight??(style===2?700:serif?600:400)} ${raster?.fontPixelHeight??(style===2?15:32)}px "${localizedFontFamily()??raster?.fontFamily??(serif?'ＭＳ 明朝':'ＭＳ ゴシック')}", "Eagler Unicode", ${serif?'serif':'monospace'}`;ctx.textBaseline='top';ctx.textAlign=raster?.align??'left';
    const foreground=raster?.foregroundColor!==undefined?'#'+(raster.foregroundColor>>>0).toString(16).padStart(6,'0'):'#'+[color&255,(color>>>8)&255,(color>>>16)&255].map(v=>v.toString(16).padStart(2,'0')).join(''),draw=(text,x)=>{
      const y=raster?.sourceY??2;
      // Trace one contour at the glyph baseline. Four displaced copies of
      // the thin shared font form separate strokes instead of an outline;
      // the old y-2 adjustment also raised ending outlines above their fill.
      if(raster?.outlineOffsets?.length!==0){
        ctx.strokeStyle='#'+((raster?.outlineColor??record.outlineColor??0)>>>0).toString(16).padStart(6,'0');
        ctx.lineWidth=raster?.outlineWidth??2;ctx.lineJoin='round';ctx.strokeText(text,x,y);
      }
      ctx.fillStyle=foreground;ctx.fillText(text,x,y);
    };
    if(family){
      ctx.textAlign='left';
      const font=ctx.font,measure=(value,commands='')=>{ctx.font=(commands.includes('b')?'bold ':'')+(commands.includes('i')?'italic ':'')+font;return ctx.measureText(value).width;};
      const layout=thcrapLayout(text,{measure,width:canvas.width,tabs:this.localizationTabs??=[]});ctx.font=font;
      let x=raster?.sourceX??2*offset+2;
      if(raster?.localizedAlign==='right')x=Math.max(0,canvas.width-26-layout.width);
      else if(raster?.align==='right')x=Math.max(0,x-layout.width);
      else if(raster?.align==='center')x=Math.max(0,x-layout.width/2);
      if(record.localizedRuby){
        const baseFont=font.replace(/\d+px/,'32px'),rubyMeasure=(value,commands='')=>{ctx.font=(commands.includes('b')?'bold ':'')+(commands.includes('i')?'italic ':'')+baseFont;return ctx.measureText(value).width;};
        const base=thcrapLayout(record.localizedRuby.base,{measure:rubyMeasure}).width,span=thcrapLayout(record.localizedRuby.span,{measure:rubyMeasure}).width;
        ctx.font=font;x=2+base+(span-layout.width)/2;
      }
      const available=Math.max(1,canvas.width-(raster?.localizedAlign==='right'?26:x)),scale=Math.min(1,available/Math.max(1,layout.width));
      ctx.save();ctx.translate(x,0);ctx.scale(scale,1);
      for(const run of layout.runs){measure('',run.commands);draw(run.text,run.x);}ctx.restore();ctx.font=font;
    }
    else if(spacing&&record.localizedWidths){let advance=0;[...text].forEach((character,i)=>{draw(character,2*offset+advance*spacing);advance+=record.localizedWidths[i];});}
    else if(spacing){const bytes=Uint8Array.from(record.textHex.match(/../g)??[],v=>parseInt(v,16));for(let i=0;i<bytes.length;i+=2)draw(new TextDecoder('shift-jis').decode(bytes.slice(i,i+2)),2*offset+i*spacing);}
    else draw(text,raster?.sourceX??2*offset+2);
    const g=this.gl,texture=g.createTexture();this.flush();g.bindTexture(g.TEXTURE_2D,texture);g.texImage2D(g.TEXTURE_2D,0,g.RGBA,g.RGBA,g.UNSIGNED_BYTE,canvas);g.texParameteri(g.TEXTURE_2D,g.TEXTURE_MIN_FILTER,g.LINEAR);g.texParameteri(g.TEXTURE_2D,g.TEXTURE_MAG_FILTER,g.LINEAR);g.texParameteri(g.TEXTURE_2D,g.TEXTURE_WRAP_S,g.CLAMP_TO_EDGE);g.texParameteri(g.TEXTURE_2D,g.TEXTURE_WRAP_T,g.CLAMP_TO_EDGE);
    const result={texture,w:canvas.width,h:canvas.height,tabs:[...this.localizationTabs??[]]};cache.set(key,result);if(cache.size>(raster?.menu?96:16)){const oldest=cache.keys().next().value,old=cache.get(oldest);cache.delete(oldest);g.deleteTexture(old.texture);}return result;
  }
  dialogueLine(p){if(!p.text||p.a.alpha<=0)return;const texture=this.dialogueTexture(p.text,p.record,p.metrics,p.raster);this.material(p.a.blend??0);this.texture(texture,!!p.a.point);this.primitive(this.gl.TRIANGLES);const points=p.a.mode===0||p.a.mode===1?nativeQuadCorners(p.x,p.y,p.metrics.width*p.a.sx,p.metrics.height*p.a.sy,p.a.rotation??0,p.a.anchorX,p.a.anchorY,p.a.mode,p.screenShake):quadCorners(p.x,p.y,p.metrics.width*p.a.sx,p.metrics.height*p.a.sy,p.a.rotation??0,p.a.anchorX,p.a.anchorY),uv=[[0,0],[1,0],[1,1],[0,1]];for(const i of [0,1,2,0,2,3])this.batch.push(...points[i],...uv[i],p.a.r??1,p.a.g??1,p.a.b??1,p.a.alpha*p.alpha,1,0);}
  capturePauseField(){
    //4356d0 /4609d0 downsamples only the384x448 field to text sprite71's
    //128x128 RGB565 rectangle. HUD is never part of the paused blur.
    const g=this.gl;this.flush();g.disable(g.SCISSOR_TEST);
    let target=this.textures.get('text:1');
    if(!this.pauseFramebuffer){
      const texture=g.createTexture();g.bindTexture(g.TEXTURE_2D,texture);g.texStorage2D(g.TEXTURE_2D,1,g.RGB565,512,512);
      g.texParameteri(g.TEXTURE_2D,g.TEXTURE_MIN_FILTER,g.LINEAR);g.texParameteri(g.TEXTURE_2D,g.TEXTURE_MAG_FILTER,g.LINEAR);g.texParameteri(g.TEXTURE_2D,g.TEXTURE_WRAP_S,g.CLAMP_TO_EDGE);g.texParameteri(g.TEXTURE_2D,g.TEXTURE_WRAP_T,g.CLAMP_TO_EDGE);
      target={texture,w:512,h:512,defaultWrap:g.CLAMP_TO_EDGE,flipY:true};this.textures.set('text:1',target);
      this.pauseFramebuffer=g.createFramebuffer();g.bindFramebuffer(g.DRAW_FRAMEBUFFER,this.pauseFramebuffer);g.framebufferTexture2D(g.DRAW_FRAMEBUFFER,g.COLOR_ATTACHMENT0,g.TEXTURE_2D,texture,0);
    }
    g.bindFramebuffer(g.READ_FRAMEBUFFER,null);g.bindFramebuffer(g.DRAW_FRAMEBUFFER,this.pauseFramebuffer);
    g.blitFramebuffer(32,16,416,464,0,384,128,512,g.COLOR_BUFFER_BIT,g.LINEAR);
    g.bindFramebuffer(g.FRAMEBUFFER,null);this.current=null;this.point=null;
  }
  renderPauseBackdrop(poses,capture=false){
    const g=this.gl;this.flush();if(capture)this.capturePauseField();g.disable(g.SCISSOR_TEST);g.uniform3fv(this.fogColor,[0,0,0]);
    //432260 copies the invisible79 VM's RGB to text81's captured-field draw.
    //Its20-frame tint changes only the playfield, leaving the HUD untouched.
    const tint=poses.find(a=>a.script===79);
    if(tint)this.solidTriangles([[32,16],[416,16],[416,464],[32,464]].map(([x,y])=>({x,y,color:[tint.r,tint.g,tint.b,1]})),[0,1,2,0,2,3],5);
    for(const a of poses)if(a.script!==79)this.state(a,0,0);
    this.flush();
  }
  renderMenu(poses,texts=[],fps=null,{overlay=false,shade=0,shadeBounds=[0,0,640,480],clearColor}={}) {
    const g=this.gl;
    if(!overlay)this.begin(clearColor);
    g.disable(g.SCISSOR_TEST);g.uniform3fv(this.fogColor,[0,0,0]);
    if(shade>0){const [left,top,right,bottom]=shadeBounds;this.solidTriangles([[left,top],[right,top],[right,bottom],[left,bottom]].map(([x,y])=>({x,y,color:[0,0,0,shade]})),[0,1,2,0,2,3],0);}
    for(const pose of orderPresentation(poses)){
      const a=pose.a??pose,payload=a.menuText;
      if(!payload){this.state(a,pose.x??0,pose.y??0,0,1,pose.alpha??1);continue;}
      if(!a.visible||a.alpha<=0)continue;
      const sprite=this.atlases[a.name]?.sprites[a.sprite],width=payload.width??sprite?.w??512,height=payload.height??sprite?.h??17;
      this.dialogueLine({text:payload.text,x:(pose.x??0)+a.x,y:(pose.y??0)+a.y,alpha:pose.alpha??1,a,
        metrics:{width,height,sourceWidth:width*2,sourceHeight:height*2},record:{style:0,color:0xffffff,offset:0,spacing:0},
        raster:{menu:true,fontFamily:'ＭＳ ゴシック',fontWeight:400,fontPixelHeight:32,sourceX:0,sourceY:0,foregroundColor:payload.colorRGB??0xffffff,outlineOffsets:[[2,2],[-2,2],[2,-2],[-2,-2]],align:'left'}});
    }
    for(const text of texts) {
      if(text.font!==undefined)this.nativeText(text);
      else {
        const {x=0,y=0,width=600,height=20,color=0xffffff,alpha=1,size=16,align='left'}=text;
        this.dialogueLine({text:text.text,x,y,alpha:1,a:{sx:1,sy:1,alpha,blend:0,anchorX:1,anchorY:1},
          metrics:{width,height,sourceWidth:width*2,sourceHeight:height*2},record:{style:0,color,outlineColor:0,offset:0,spacing:0},
          raster:{fontFamily:'ＭＳ ゴシック',fontPixelHeight:size*2,fontWeight:400,sourceX:align==='right'?width*2:align==='center'?width:0,sourceY:0,foregroundColor:color,outlineColor:0,outlineOffsets:[[2,2],[-2,2],[2,-2],[-2,-2]],align}});
      }
    }
    for(const text of fpsTextDescriptors({currentFps:fps}))this.nativeText(text);
    this.flush();
  }
  resetPresentation(){
    // Call on start/restart, stage load, replay seek, pause/resume, focus
    // loss or any host discontinuity; the host also discards its old pair.
    if(this.drawBuckets)for(const bucket of this.drawBuckets)if(bucket)bucket.length=0;
    this.presentationEpoch++;this.lastFrame=-1;this.animations=new AnimationSampler(this.atlases);
  }
  capturePresentation(core,fps={}){
    const h=[...readCoreHud(core,fps)],frame=h[0];if(frame<this.lastFrame){this.resetPresentation();}this.lastFrame=frame;this.animations.beginFrame();
    const stage=Math.max(1,Math.min(7,h[39]||1)),stageSuffix=String(stage).padStart(2,'0');if(this.stageBackgrounds.has(stage))this.background=this.stageBackgrounds.get(stage);else if(!this.stageJobs.has(stage))this.prepareStage(stage).catch(error=>this.stageError=error);
    const logicalBackground=readCoreBackground(core),background=this.background.geometry(logicalBackground?.frame??core._th12_background_frame?.()??frame,logicalBackground),screenShake=logicalBackground?.camera?.screenShake??[0,0],backgroundVisible=background.authoritative||(core._th12_background_visible?.()??true);
    const logicalPoses=readCoreAnimationPoses(core,stage),schedule=readCoreDrawSchedule(core,logicalPoses),meshes=readCoreMeshes(core,stage),logicalOwners=(this.logicalOwners??=new Set()),message=readCoreMessage(core),spell=readCoreSpell(core),bossHud=readCoreBossHud(core);
    logicalOwners.clear();for(const p of logicalPoses)logicalOwners.add(p.owner);for(const p of meshes)logicalOwners.add(p.owner);
    let bulletCount=0;
    const player='pl0'+h[17],poses=[...logicalPoses.filter(p=>!(p.a.name==='text'&&(p.a.sprite>=0&&p.a.sprite<4||p.a.sprite===7))),...messageTextPoses(message,logicalPoses,this.atlases,{logicalText:!!core._th12_anm_draw}),...meshes,...(core._th12_mesh_draw?[]:readCoreLaserSegments(core,stage))];
    for(const text of spellTextDescriptors(spell,logicalPoses,{disableMaster:fps.disableMaster}))poses.push(text.kind==='spell-title'?{...text.pose,dialogue:true,text:text.text,record:text.record,metrics:text.metrics,raster:text.raster}:{nativeText:text,drawPriority:text.drawFlag===0?69:45,clipped:false,order:poses.length});
    for(const text of [...bossTimerDescriptors(bossHud),...spellResultDescriptors(spell,logicalPoses)])poses.push({nativeText:text,drawPriority:44,clip:2,order:poses.length});
    const ufo=readCoreUfo(core);
    for(const text of ufoTextPoses(ufo)){const priority=ufoTextRenderPriority(text),attached=!!ufo?.enemyId&&text.font===2&&text.drawFlag===1&&text.colorARGB===0xc080ffc0;
      poses.push({nativeText:text,drawPriority:priority,clip:priority===22?2:0,owner:attached?ufo.enemyId:0,presentationKey:attached?'ufo-countdown:'+ufo.enemyId:undefined,order:poses.length});}
    for(const text of fpsTextDescriptors(fps))poses.push({nativeText:text,drawPriority:text.drawPriority,clip:0,order:poses.length});
    if(fps.stageClear){const text=stageClearText(h,logicalPoses,fps.practice);if(text)poses.push({nativeText:text,drawPriority:69,clip:0,order:poses.length});}
    const transition=stageTransitionPose(logicalBackground?.transition);if(transition)poses.push(transition);
    const add=(name,id,age,x,y,angle,scale,variant,options)=>{const {alpha,drawPriority,clipped,...sampling}=options,owner=Number.isInteger(sampling.entityId)?sampling.entityId:0;let child=0;for(const a of this.animations.sampleAll(name,id,age,variant,sampling))poses.push({a,x,y,angle,scale,alpha,drawPriority,clipped,owner,presentationKey:owner?'sample:'+owner+':'+id+':'+child++:undefined,order:poses.length});};
    // Consume the draw queue after all metadata exports. No Wasm calls occur
    // inside this loop; endpoint records own their scalars before the next export.
    const count=core._th12_draw(),ptr=core._th12_draw_ptr(),stride=(core._th12_draw_stride?.()||36)/4;if(stride<9||stride>64||!Number.isInteger(stride))throw Error('绘制记录长度无效');
    if(!Number.isInteger(count)||count<0||count>1000000||!Number.isInteger(ptr)||ptr<0||ptr%4!==0||ptr+count*stride*4>core.HEAPU8.byteLength)throw Error('绘制记录范围无效');
    const f=new Float32Array(core.HEAPU8.buffer,ptr,count*stride),v=new Int32Array(core.HEAPU8.buffer,ptr,count*stride),records=[];
    for(let i=0;i<count;i++){
      const k=i*stride,x=f[k],y=f[k+1],angle=f[k+2],scale=f[k+3],animation=v[k+5],kind=v[k+6],color=v[k+7],age=v[k+8],entityId=stride>=12?v[k+9]>>>0:0,interrupt=stride>=12?v[k+10]:0,alpha=stride>=12?f[k+11]:1;
      records.push({x,y,owner:entityId,bank:v[k+4],animation,kind,color,age,interrupt});
      if(kind===1)bulletCount++;
      if(stride>=12&&logicalOwners.has(entityId))continue;
      const options={entityId,seed:entityId?entityId&65535:0x1234,alpha,...(interrupt?{interrupt}: {}),...(kind===1?{drawPriority:31,clipped:true}:kind===3?{drawPriority:27,clipped:true}:kind===4?{drawPriority:24,clipped:true}:{})};
      if(kind===12)poses.push({a:{layer:12},x,y,angle,scale,width:alpha,start:color>>>0,end:animation>>>0,beamTrail:true,order:poses.length});
      else if(kind===13)poses.push({a:{layer:13},x,y,angle,scale,alpha:1,start:color>>>0,end:animation>>>0,trail:true,order:poses.length});
      else if(kind===4||kind===5||kind===2||kind===7)add(player,animation,age,x,y,angle,scale,null,options);
      else if(kind===6){const {alpha:opacity,...sampling}=options,a=this.animations.sample(player,animation,age,null,sampling);if(a)poses.push({a,x,y,angle,scale,alpha:opacity,beam:true,owner:entityId,presentationKey:'beam:'+entityId,order:poses.length});}
      else if(kind===1){const type=this.atlases.bullet.types?.[Math.floor(color/16)],variant=type?.colors[color%16];if(type)add('bullet',stride>=12?animation:type.script,age,x,y,angle,scale,variant,options);}
      else if(kind===3)add('bullet',animation>=165?animation:166+color,age,x,y,angle,scale,null,options);
      else {const bank=v[k+4],name=bank===0?'bullet':bank===1?'enemy':bank===2?'stgenm'+stageSuffix:bank===3?'stgenm'+stageSuffix+'m':null;if(name&&this.atlases[name])add(name,animation,age,x,y,angle,scale,null,options);}
    }
    if(message?.active&&!core._th12_anm_draw){
      const events=message.events??[],windowEvent=[...events].reverse().find(e=>e[0]===3);
      // Native MSG opcode3 creates front script52. Use that original animation
      // while the message layer has not yet been registered as a core VM.
      if(windowEvent&&!logicalPoses.some(p=>p.a.name==='front'&&p.a.layer===19))add('front',52,Math.max(0,frame-windowEvent[1]),0,0,0,1,null,{alpha:1,entityId:'message-window'});
    }
    // The frame itself belongs at layer21/23. Painting it after every VM
    // hides the UFO panel and FontManager's right-side counters.
    //41d560 reinitializes stage HUD slots, but the front0 frame survives.
    add('front',0,fps.hudAge??frame,0,0,0,1,null,{alpha:1,clipped:false});
    for(const rectangle of bossHealthRectangles(bossHud))poses.push({rectangle,drawPriority:44,clip:2,order:poses.length});
    poses.push({hudCounters:true,drawPriority:69,clipped:false,order:poses.length});
    for(let i=0;i<8;i++){add('front',13+i,frame,0,0,0,1,null,{alpha:1,drawPriority:59,clipped:false,interrupt:fragmentInterrupt(i,h[5],h[21])});add('front',21+i,frame,0,0,0,1,null,{alpha:1,drawPriority:59,clipped:false,interrupt:fragmentInterrupt(i,h[6],h[22])});}
    if(h.length>25&&!logicalPoses.some(p=>p.a.name==='front'&&(p.owner>>>16)===0xfffe))for(let i=0;i<3;i++){const color=h[23+i]|0;add('front',80,frame,0,0,0,1,null,{alpha:1,drawPriority:44,clipped:true,interrupts:[7+i,color?9+color:13,color?2:3]});}
    add('front',69+h[19],frame,0,0,0,1,null,{alpha:1,clipped:false});
    const nativeMotion=fps.presentationContinuity===false?null:readCorePresentationMotion(core),playerState=core._th12_player_state?.()??nativeMotion?.playerState;
    if(fps.alwaysHitbox&&playerState===1&&!logicalPoses.some(p=>p.owner===0xffff0020)){
      poses.push(...alwaysHitboxPoses(this.animations,frame,h[1]+224,h[2]+16));
    }
    // These pose wrappers were created for this capture. Annotate them before
    // publishing the immutable endpoint; older endpoints and appearances stay intact.
    const ordered=orderPresentation(poses,schedule,{reusePoses:true,buckets:this.drawBuckets??=[]});
    this.animations.endFrame();
    // Modern cores already publish authoritative owner/lineage data. Do not
    // build a second legacy owner graph that their interpolation never uses;
    // a locked60 Hz endpoint needs neither graph.
    const motion=fps.presentationContinuity===false||nativeMotion?null:capturePresentationOwners(h,records,ordered);
    return {epoch:this.presentationEpoch,frame,stage,hud:h,background,logicalBackground,nativeMotion,backgroundVisible,screenShake,hasStageSurface:!!logicalBackground?.loaded,poses:ordered,records,bulletCount,motion};
  }
  render(core,fps={}){
    // Ordinary 60 Hz callers retain current-endpoint rendering.
    return this.renderPresentation(null,this.capturePresentation(core,{...fps,presentationContinuity:false}),1);
  }
  renderPresentation(previous,current,interpolationAlpha=1,live={}){
    const endpoint=interpolatePresentation(previous,current,interpolationAlpha),{hud:h,backgroundVisible,screenShake,hasStageSurface}=endpoint,
      source=this.stageBackgrounds.get(endpoint.stage),background=source&&endpoint.logicalBackground!==current.logicalBackground&&endpoint.logicalBackground?.loaded
        ?source.geometry(endpoint.logicalBackground.frame,endpoint.logicalBackground):endpoint.background;
    this.begin();const g=this.gl;g.enable(g.SCISSOR_TEST);g.scissor(32,16,384,448);
    g.uniform3fv(this.fogColor,background.camera.fog.slice(0,3));g.clearColor(...background.camera.fog.slice(0,3),1);g.clear(g.COLOR_BUFFER_BIT);
    this.primitive(g.TRIANGLES);if(backgroundVisible)for(const triangle of background.triangles){const texture=this.textures.get(triangle.bank+':'+triangle.entry);if(!texture)continue;this.material(triangle.blend);this.texture(texture,triangle.point);for(const p of triangle.vertices)this.batch.push(p.x,p.y,p.u/texture.w,p.v/texture.h,p.r,p.g,p.b,p.alpha,p.depth,p.fog);}
    this.flush();
    let currentClip=1;const roundedShake=screenShake.map(Math.fround),shaking=!!(roundedShake[0]||roundedShake[1]);
    for(const originalPose of endpoint.poses){
      if(hasStageSurface)this.stageSurface(originalPose.drawPriority,screenShake,currentClip);
      // Native sprite corners apply shake at submission. Keep their immutable
      // pose and appearance instead of cloning every sprite during a Bomb.
      let p=originalPose,poseShake=p.screenShake;
      if(shaking&&p.drawPriority<42&&!p.mesh){
        if(!p.mesh&&!p.dialogue&&(p.nativeText||p.a&&(p.a.mode===0||p.a.mode===1)))poseShake=roundedShake;
        else{p=shakePresentation(p,roundedShake);poseShake=p.screenShake;}
      }
      if(p.clip!==currentClip){this.flush();if(p.clip){g.enable(g.SCISSOR_TEST);if(p.clip===2)g.scissor(13,0,422,480);else g.scissor(32,16,384,448);}else g.disable(g.SCISSOR_TEST);currentClip=p.clip;}
      if(p.nativeText){if(p.nativeText.kind!=='current-fps'||!Number.isFinite(live.currentFps))this.nativeText(p.nativeText,poseShake);}else if(p.hudCounters)this.hudCounters(h);else if(p.rectangle){const {bounds:[left,top,right,bottom],argb}=p.rectangle,color=argbRgba(argb);this.solidTriangles([[left,top],[right,top],[right,bottom],[left,bottom]].map(([x,y])=>({x,y,color})),[0,1,2,0,2,3],0);}else if(p.mesh)this.mesh(p,p.drawPriority<42?roundedShake:zeroOffset);else if(p.laser)this.laserSegment(p);else if(p.dialogue)this.dialogueLine(p);else if(p.beamTrail)this.beamTrail(p.x,p.y,p.angle,p.scale,p.width,p.start,p.end);else if(p.trail)this.trail(p.x,p.y,p.angle,p.scale,p.start,p.end,p.alpha);else if(p.beam){const s=this.atlases[p.a.name].sprites[p.a.sprite];if(s)this.region(p.a.name,s.entry,s.x,s.y,s.w,s.h,p.x,p.y-448*p.scale/2,448*p.scale,14*p.a.sy,-Math.PI/2,p.a.alpha*p.alpha,[p.a.r,p.a.g,p.a.b],p.a.blend,{nativeMode:1,screenShake:poseShake,presentationOffset:p.presentationOffset});}else this.state(p.a,p.x,p.y,p.angle,p.scale,p.alpha,poseShake,p.presentationOffset);
    }
    this.flush();g.disable(g.SCISSOR_TEST);
    for(const text of fpsTextDescriptors({currentFps:live.currentFps}))this.nativeText(text);
    this.flush();return [...h];
  }
}
