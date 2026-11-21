import {fetchResource} from '../resource-fetch.mjs';
export const DRAW_STRIDE = 160;
export function circleLineVertices(record){
  const vertices=record?.geometry?.vertices;
  if(record?.mode!==17||record.geometry?.kind!=='circle-line'||!Array.isArray(vertices)||vertices.length<2||vertices.length>4097)throw new Error('Invalid original mode17 circle line');
  const output=new Float32Array(vertices.length*8);let offset=0;
  for(const vertex of vertices){if(!vertex||![vertex.x,vertex.y].every(value=>Number.isFinite(value)&&Math.abs(value)<=65536)||!Number.isInteger(vertex.color)||vertex.color<0||vertex.color>0xffffffff)throw new Error('Invalid original circle line vertex');output.set([vertex.x,vertex.y,0,0,...argbColor(vertex.color)],offset);offset+=8;}
  return output;
}
export function ringStripVertices(record){
  return expandTriangleStrip(ringStripCorners(record));
}
export function ringStripCorners(record){
  const vertices=record?.geometry?.vertices;
  if((record?.mode!==9&&record?.mode!==14)||record?.geometry?.kind!=='anm-ring'||!Array.isArray(vertices)||vertices.length<4||vertices.length>8192||vertices.length%2){
    throw new Error('Invalid original Boss ring geometry');
  }
  return packTriangleStrip(vertices,false,false,'Invalid original Boss ring vertex');
}
const finiteStripValue=value=>Number.isFinite(value)&&Math.abs(value)<=65536;
function packTriangleStrip(vertices,flipY,flat,error){
  const output=new Float32Array(vertices.length*8);let offset=0;
  for(const vertex of vertices){
    if(!vertex||!finiteStripValue(vertex.x)||!finiteStripValue(vertex.y)||!finiteStripValue(vertex.z)||vertex.rhw!==1
      ||flat&&vertex.z!==0||!finiteStripValue(vertex.u)||!finiteStripValue(vertex.v)||!validColorWord(vertex.color))throw new Error(error);
    const {x,y,u,v,color}=vertex;
    output[offset++]=x;output[offset++]=y;output[offset++]=u;output[offset++]=flipY?1-v:v;
    output[offset++]=(color>>>16&255)/255;output[offset++]=(color>>>8&255)/255;output[offset++]=(color&255)/255;output[offset++]=(color>>>24)/255;
  }
  return output;
}
function expandTriangleStrip(corners){
  const count=corners.length/8,output=new Float32Array((count-2)*24);let offset=0;
  const emit=vertex=>{const at=vertex*8;output.set(corners.subarray(at,at+8),offset);offset+=8;};
  for(let i=2;i<count;++i){emit(i&1?i-1:i-2);emit(i&1?i-2:i-1);emit(i);}
  return output;
}
export function messageTextLayout(state,variant,{nativeRasters=false}={}){
  if(!state?.active)return [];
  if(![state.x,state.y].every(value=>Number.isFinite(value)&&Math.abs(value)<=65536)||!Array.isArray(state.lines)||state.lines.length>8||![0,1].includes(state.speaker))throw new Error('Invalid original message text layout');
  const decoder=new TextDecoder(variant==='ja'?'shift_jis':'gbk',{fatal:true});const output=[];let row=0;
  for(const bytes of state.lines){
    if(!Array.isArray(bytes)||bytes.length>252||bytes.some(value=>!Number.isInteger(value)||value<0||value>255))throw new Error('Invalid original message text bytes');
    const raw=decoder.decode(Uint8Array.from(bytes)),match=/^\|(\d+),(\d+),(.*)$/s.exec(raw);
    const text=match?match[3]:raw,x=state.x+(match?Number(match[1]):0),y=state.y+(match?Number(match[2]):27*(row+1));
    const slot=row;
    if(!match)++row;
    const primary=Array.isArray(state.textPoses)?state.textPoses[slot]:null,outline=Array.isArray(state.textPoses)?state.textPoses[slot+2]:null,pose=match?outline:primary;
    if(state.textPoses!==undefined&&(!Array.isArray(state.textPoses)||state.textPoses.length!==4||!pose||!outline||![pose.alpha,pose.sx,pose.sy,pose.offsetY,outline.alpha].every(Number.isFinite)))throw new Error('Invalid original message text animation');
    if(pose&&(!pose.visible||pose.alpha<=0)&&(!outline?.visible||outline.alpha<=0))continue;
    output.push({text,...(nativeRasters?{rawHex:Uint8Array.from(match?bytes.slice(bytes.indexOf(44,bytes.indexOf(44)+1)+1):bytes).reduce((hex,value)=>hex+value.toString(16).padStart(2,'0'),''),rasterKind:match?'ruby':'message'}:{}),x,y:y+(pose?.offsetY??0),align:state.speaker===1?'right':'left',font:match||state.font===6?18:20,alpha:pose?.visible?pose.alpha:pose?0:1,outlineAlpha:match?0:outline?.visible?outline.alpha:outline?0:1,sx:pose?.sx??1,sy:pose?.sy??1});
  }
  return output;
}

const nativeLayerPriorities=[4,6,8,9,11,13,16,17,18,19,20,23,26,27,29,31,36,38,40,43,44,53,54,56,57,68,69,null,null,14,41,71,70];
const nativePassPriorities={'hostile-bullets':32,'hostile-lasers':30,player:25,items:28,'frozen-bullets':24,'original-percent-hud':55,'original-freeze-reward-panel':47,'original-spell-bonus':47,'original-fps':67,'original-menu-font0':74,'original-pause-shade':52,'original-pause-backdrop':70,'original-pause-front':68};
export function nativeDrawPriority(layer) {
  if(!Number.isInteger(layer)||layer<0||layer>=nativeLayerPriorities.length||nativeLayerPriorities[layer]===null)throw new Error(`Unimplemented native ANM layer priority ${layer}`);
  return nativeLayerPriorities[layer];
}
export function nativeRecordPriority(record){
  if(record.drawPass===undefined)return nativeDrawPriority(record.layer);
  if(record.drawPass==='playerShotGroup')return nativeDrawPriority(record.layer);
  if(!Object.hasOwn(nativePassPriorities,record.drawPass))throw new Error(`Unimplemented native draw callback ${record.drawPass}`);
  return nativePassPriorities[record.drawPass];
}
export function applyWorldShake(record,offset){
  if(!Array.isArray(offset)||offset.length!==2||!offset.every(value=>Number.isFinite(value)&&Math.abs(value)<=4096))throw new Error('Invalid original shake offset');
  if(nativeRecordPriority(record)>=44||offset.every(value=>value===0)||record.stageSurface==='main')return record;
  if(record.geometry){return {...record,geometry:{...record.geometry,vertices:record.geometry.vertices.map(vertex=>({...vertex,x:vertex.x+offset[0],y:vertex.y+offset[1]}))}};}
  return {...record,x:record.x+offset[0],y:record.y+offset[1]};
}

export function orderDrawLayers(records,{world=false}={}) {
  if(!Array.isArray(records)||records.length>65536){
    throw new Error('Invalid world ANM layer records');
  }
  // Bounded priorities: evaluate each once and retain submission order for
  // equal priorities (blending and overlapping sprites depend on this).
  const offsets=new Uint32Array(256),keys=new Uint8Array(records.length);
  for(let i=0;i<records.length;i++){
    const record=records[i];
    if(!record||!Number.isInteger(record.layer)||record.layer<0||record.layer>255)throw new Error('Invalid world ANM layer records');
    const key=world?nativeRecordPriority(record):record.layer;keys[i]=key;offsets[key]++;
  }
  let next=0;
  for(let key=0;key<offsets.length;key++){const count=offsets[key];offsets[key]=next;next+=count;}
  const ordered=new Array(records.length);
  for(let i=0;i<records.length;i++)ordered[offsets[keys[i]]++]=records[i];
  return ordered;
}

export function argbColor(value) {
  return [(value >>> 16 & 255) / 255, (value >>> 8 & 255) / 255, (value & 255) / 255, (value >>> 24) / 255];
}

const validColorWord=value=>Number.isInteger(value)&&value>=0&&value<=0xffffffff;
function writeArgbColor(out,value){
  out[0]=(value>>>16&255)/255;out[1]=(value>>>8&255)/255;out[2]=(value&255)/255;out[3]=(value>>>24)/255;
}
export function nativeQuadColors(record,scratch=null){
  const flags=record?.flags??0,primary=record?.color,secondary=record?.secondary??0;
  if(!validColorWord(flags)||!validColorWord(primary)||!validColorWord(secondary))
    throw new Error('Invalid original quad diffuse color domain');
  const mode=(flags>>>16)&3;
  if(scratch){
    const {first,second,corners}=scratch;
    if(mode!==1)writeArgbColor(first,primary);
    if(mode!==0)writeArgbColor(second,secondary);
    corners[0]=mode===1?second:first;corners[1]=mode===1||mode===2?second:first;
    corners[2]=mode===0?first:second;corners[3]=mode===1||mode===3?second:first;
    return corners;
  }
  const first=argbColor(primary),second=argbColor(secondary);
  if(mode===0)return [first,first,first,first];
  if(mode===1)return [second,second,second,second];
  return mode===2?[first,second,second,first]:[first,first,second,second];
}

export function selector3QuadVertices(record){
  const geometry=record?.geometry,vertices=geometry?.vertices;
  if(record?.drawPass!=='hostile-lasers'||record.mode!==0||record.blend!==1||record.layer!==0
      ||geometry?.kind!=='laser-selector3-quad'||!Number.isInteger(geometry.owner)||geometry.owner<=0
      ||!Number.isInteger(geometry.column)||geometry.column<0||geometry.column>=512
      ||!Array.isArray(vertices)||vertices.length!==4)
    throw new Error('Invalid original selector3 diffuse quad domain');
  for(const vertex of vertices)if(!vertex||![vertex.x,vertex.y,vertex.z,vertex.rhw].every(value=>Number.isFinite(value)&&Math.abs(value)<=1048576)
      ||vertex.rhw!==1||!Number.isInteger(vertex.color)||vertex.color<0||vertex.color>0xffffffff)
    throw new Error('Invalid original selector3 diffuse vertex');
  const output=new Float32Array(6*8);
  for(const [index,source]of [0,1,2,2,1,3].entries()){
    const vertex=vertices[source];
    output.set([vertex.x,vertex.y,0,0,...argbColor(vertex.color)],index*8);
  }
  return output;
}

export function triangleStripVertices(record,{flipY=false}={}) {
  return expandTriangleStrip(triangleStripCorners(record,{flipY}));
}
export function triangleStripCorners(record,{flipY=false}={}) {
  const vertices=record?.geometry?.vertices;
  if(record?.mode!==12||record.geometry?.kind!=='triangle-strip'||!Array.isArray(vertices)||vertices.length!==34){
    throw new Error('Invalid original526 triangle-strip geometry');
  }
  return packTriangleStrip(vertices,flipY,true,'Invalid bounded original526 vertex');
}

export function circleFanVertices(record){
  const vertices=record?.geometry?.vertices;
  if(record?.mode!==16||record.geometry?.kind!=='circle-fan'||!Array.isArray(vertices)||vertices.length<6||vertices.length>4098){
    throw new Error('Invalid original mode16 circle-fan geometry');
  }
  for(const vertex of vertices)if(!vertex||![vertex.x,vertex.y].every(value=>Number.isFinite(value)&&Math.abs(value)<=65536)
    ||!Number.isInteger(vertex.color)||vertex.color<0||vertex.color>0xffffffff)throw new Error('Invalid original circle-fan vertex');
  const output=new Float32Array((vertices.length-2)*3*8);let offset=0;
  const emit=vertex=>{output.set([vertex.x,vertex.y,0,0,...argbColor(vertex.color)],offset);offset+=8;};
  for(let index=2;index<vertices.length;++index){emit(vertices[0]);emit(vertices[index-1]);emit(vertices[index]);}
  return output;
}

function roundedPixel(value) {
  const lower = Math.floor(value);
  return value - lower === 0.5 ? lower + (Math.abs(lower % 2) === 1 ? 1 : 0) : Math.round(value);
}

export function quadVertices(record) {
  const spellPortrait=record.bank===3&&(record.script===11
    ||record.script===17&&record.entry===20&&record.sprite===43&&record.layer===6
    ||record.script===36&&record.entry===56&&record.sprite===111&&record.layer===6
    ||record.script===38&&record.entry===58&&record.sprite===113&&record.layer===6
    ||record.script===37&&record.entry===57&&record.sprite===112&&record.layer===6
    ||record.script===39&&record.entry===59&&record.sprite===114&&record.layer===6);
  const mode=record.mode===2&&spellPortrait&&record.rx===0&&record.ry===0&&record.rotation===0&&record.z===0?0:record.mode;
  if (mode !== 0 && mode !== 1) throw new Error(`Unimplemented geometry mode ${record.mode}`);
  if (!Number.isFinite(record.rotation ?? 0)) throw new Error('Non-finite draw rotation');
  const positionX = Math.fround(record.x), positionY = Math.fround(record.y);
  const width = Math.fround(record.width * record.sx), height = Math.fround(record.height * record.sy);
  if (![positionX, positionY, width, height].every(Number.isFinite)) throw new Error('Non-finite draw geometry');
  if (mode === 0) {
    const interpolated=record.presentationInterpolated===true;
    const aligned=value=>interpolated?value:Math.floor(value);
    const left = record.anchorX === 1 ? positionX : record.anchorX === 2 ? Math.fround(positionX - width) : aligned(Math.fround(positionX - width / 2));
    const top = record.anchorY === 1 ? positionY : record.anchorY === 2 ? Math.fround(positionY - height) : aligned(Math.fround(positionY - height / 2));
    const right = record.anchorX === 2 ? positionX : Math.fround(left + width);
    const bottom = record.anchorY === 2 ? positionY : Math.fround(top + height);
    const output=interpolated?value=>value:roundedPixel;
    return [[output(left), output(top)], [output(right), output(top)],
      [output(right), output(bottom)], [output(left), output(bottom)]];
  }
  const left = record.anchorX === 1 ? 0 : record.anchorX === 2 ? -width : Math.fround(-width / 2);
  const top = record.anchorY === 1 ? 0 : record.anchorY === 2 ? -height : Math.fround(-height / 2);
  const cosine = Math.fround(Math.cos(record.rotation ?? 0)), sine = Math.fround(Math.sin(record.rotation ?? 0));
  const result = [[left, top], [Math.fround(left + width), top], [Math.fround(left + width), Math.fround(top + height)], [left, Math.fround(top + height)]]
    .map(([horizontal, vertical]) => [Math.fround(positionX + horizontal * cosine - vertical * sine),
      Math.fround(positionY + horizontal * sine + vertical * cosine)]);
  if (!result.flat().every(Number.isFinite)) throw new Error('Non-finite projected draw coordinates');
  return result;
}

export function decodeDrawBuffer(buffer, pointer, count, stride) {
  if (!(buffer instanceof ArrayBuffer) || stride !== DRAW_STRIDE || !Number.isSafeInteger(pointer)
    || !Number.isSafeInteger(count) || pointer < 0 || count < 0 || count > 65536 || pointer + count * stride > buffer.byteLength) {
    throw new Error('Invalid WASM draw ABI bounds');
  }
  const view = new DataView(buffer);
  return Array.from({ length: count }, (_, index) => {
    const offset = pointer + index * stride;
    const integer = field => view.getInt32(offset + field * 4, true);
    const unsigned = field => view.getUint32(offset + field * 4, true);
    const scalar = field => view.getFloat32(offset + field * 4, true);
    return { id: unsigned(0), parent: unsigned(1), bank: integer(2), sprite: integer(3), entry: integer(4),
      layer: integer(5), mode: integer(6), anchorX: integer(7), anchorY: integer(8), blend: integer(9),
      flags: unsigned(10), flags2: unsigned(11), color: unsigned(12), secondary: unsigned(13),
      width: scalar(14), height: scalar(15), x: scalar(16), y: scalar(17), z: scalar(18),
      rotation: scalar(19), sx: scalar(20), sy: scalar(21), u: scalar(22), v: scalar(23),
      uvScaleX: scalar(24), uvScaleY: scalar(25), u0: scalar(26), v0: scalar(27), u1: scalar(28), v1: scalar(29),
      rx: scalar(30), ry: scalar(31), geometryRadius: scalar(32), geometryWidth: scalar(33),
      geometryCount: integer(34), script: integer(35), clock: integer(36), order: integer(37) };
  });
}

export async function loadOriginalHelpPage(renderer,record,{signal,isCurrent=()=>true}={}){
  const graphics=renderer?.gl;
  if(!graphics||!renderer.textures||!Number.isInteger(record?.page)||record.page<0||record.page>8
    ||typeof record.url!=='string'||record.width!==384||record.height!==448)
    throw new Error('Original help texture page is unavailable');
  const response=await fetchResource(record.url,{signal});
  if(!response.ok)throw new Error('Original help page could not be loaded');
  const full=await response.arrayBuffer();
  if(record.sha256){
    const digest=Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',full)),value=>value.toString(16).padStart(2,'0')).join('');
    if(digest!==record.sha256)throw new Error('Original help page bytes differ');
  }
  const bitmap=await createImageBitmap(new Blob([full]),0,0,384,448,{premultiplyAlpha:'none',colorSpaceConversion:'none'});
  let texture=null;
  try{
    if(signal?.aborted||renderer.disposed||!isCurrent())throw new DOMException('Aborted','AbortError');
    const key='46:1',previous=renderer.textures.get(key);
    if(previous?.helpPage===record.page)return;
    texture=graphics.createTexture();if(!texture)throw new Error('Cannot allocate original help page');
    graphics.bindTexture(graphics.TEXTURE_2D,texture);
    graphics.texImage2D(graphics.TEXTURE_2D,0,graphics.RGBA,384,512,0,graphics.RGBA,graphics.UNSIGNED_BYTE,new Uint8Array(384*512*4));
    graphics.texSubImage2D(graphics.TEXTURE_2D,0,0,0,graphics.RGBA,graphics.UNSIGNED_BYTE,bitmap);
    graphics.texParameteri(graphics.TEXTURE_2D,graphics.TEXTURE_WRAP_S,graphics.CLAMP_TO_EDGE);
    graphics.texParameteri(graphics.TEXTURE_2D,graphics.TEXTURE_WRAP_T,graphics.CLAMP_TO_EDGE);
    graphics.texParameteri(graphics.TEXTURE_2D,graphics.TEXTURE_MIN_FILTER,graphics.LINEAR);
    graphics.texParameteri(graphics.TEXTURE_2D,graphics.TEXTURE_MAG_FILTER,graphics.LINEAR);
    if(graphics.getError()!==graphics.NO_ERROR)throw new Error('Original help page GPU upload rejected');
    if(previous)renderer.releaseTexture(key);
    renderer.textures.set(key,{texture,width:384,height:512,allocationBytes:384*512*4,helpPage:record.page,sourceKey:record.url,flipY:false});
    renderer.textureBytes=(renderer.textureBytes??0)+384*512*4;texture=null;
    renderer.current=null;renderer.point=null;
  }finally{
    bitmap.close();if(texture)graphics.deleteTexture(texture);
  }
}
export function splitOriginalMenuDraws(records){
  const menu=[],battle=[],dialogue=[];
  for(const record of records){
    if(record.id>=47000000&&record.id<48000000)menu.push(record);
    else if(record.id>=15000000&&record.id<20000000)dialogue.push(record);
    else battle.push(record);
  }
  return {menu,battle,dialogue};
}

export const originalHelpPagePins={"ja":[{"bytes":135587,"sha256":"641302205fbc8628fae9d55cb0d04437c2e534c8b841d84b4d73ab6167d555f5","page":0,"width":384,"height":448,"file":"textures/menu-help-page00.png"},{"bytes":73390,"sha256":"13508746203a468e4652a4fb65173b7f737605f7ef49fbae5275c32bc27b4fb5","page":1,"width":384,"height":448,"file":"textures/menu-help-page01.png"},{"bytes":119190,"sha256":"80059ab950c8d716abe2de47d6ad29ead9747ef9b668f1e9c1e24cfcc3436ebf","page":2,"width":384,"height":448,"file":"textures/menu-help-page02.png"},{"bytes":161879,"sha256":"84fd5d1e9b4e8e58f0d749dd15db93f7943d5a0e864b20a056b1aa12ccbad313","page":3,"width":384,"height":448,"file":"textures/menu-help-page03.png"},{"bytes":125751,"sha256":"c7aade9776bd26992cf65813555547b13b5dbcfbc4d160d74be04cc6ee329889","page":4,"width":384,"height":448,"file":"textures/menu-help-page04.png"},{"bytes":90439,"sha256":"c0ad0a51d98145e50c69ea29e7007befbef053c00dba8f39c9124bb1861cf410","page":5,"width":384,"height":448,"file":"textures/menu-help-page05.png"},{"bytes":103651,"sha256":"06dbf645f706dd4336dfa2b5d9bfe413a37d89cdd3eec5ea1268fe0176e309d2","page":6,"width":384,"height":448,"file":"textures/menu-help-page06.png"},{"bytes":92713,"sha256":"10a5e4060b980dab4ffa38fbd0e5280ceec88edc13e1c63943a7d810a2cbc758","page":7,"width":384,"height":448,"file":"textures/menu-help-page07.png"},{"bytes":98018,"sha256":"3ffd0a275deb987d62be702401a31edcfc49690e87495267505f3549d1900de2","page":8,"width":384,"height":448,"file":"textures/menu-help-page08.png"}],"zh":[{"bytes":143675,"sha256":"b654891abf25caa0b448f18430802d47b5a59407f80d79202f04074602b228ec","page":0,"width":384,"height":448,"file":"textures/menu-help-page00.png"},{"bytes":80945,"sha256":"000cb17ce6e18a70e4cf5ffffeb6fc6d00a6be03c50d0089f77c1e38dcac2b09","page":1,"width":384,"height":448,"file":"textures/menu-help-page01.png"},{"bytes":116771,"sha256":"c5f951a6e165c940a8a89bf41d117cf7cc0798ba0f8674b2ca8fd2f0c0c80c0f","page":2,"width":384,"height":448,"file":"textures/menu-help-page02.png"},{"bytes":168215,"sha256":"9874bc3410ec30470f6dd84c7fad2b126396c7fae9b3bb41b3c107896144d44e","page":3,"width":384,"height":448,"file":"textures/menu-help-page03.png"},{"bytes":119573,"sha256":"7ddec60d44970cc81c7495771dd4071afae6609a722fa48fe32418eff624cee1","page":4,"width":384,"height":448,"file":"textures/menu-help-page04.png"},{"bytes":100368,"sha256":"61f4d548deb388154f0b00e1d11827d6c0255e73af5496510530e7fd8c671344","page":5,"width":384,"height":448,"file":"textures/menu-help-page05.png"},{"bytes":103762,"sha256":"910d3428f7da9eea33726de3539afce8e3203171aad2fede0101c95681f88993","page":6,"width":384,"height":448,"file":"textures/menu-help-page06.png"},{"bytes":93217,"sha256":"fa0b25f4359a581fb5e3e45cb0f0287d47b505726f424f3b362d9d54c47cbc0f","page":7,"width":384,"height":448,"file":"textures/menu-help-page07.png"},{"bytes":123482,"sha256":"5a059c5bf9bb69fe338a6a54b3386167b1255ebe1e1f7e732cd0d50b623a64c0","page":8,"width":384,"height":448,"file":"textures/menu-help-page08.png"}]};

export async function loadOriginalStageTextBank(metadata,loadPage,{signal}={}){
  if(metadata?.schema!=='th128-original-stage-text-raster/1'||metadata.variant!=='zh'||metadata.sourceExeSha256!=='f51af65e3311461b83751471e85629620a72e656c91355f877115c7da7eae2da'
    ||!Array.isArray(metadata.pages)||metadata.pages.length<1||metadata.pages.length>16||!Array.isArray(metadata.rows)||metadata.rows.length<1||metadata.rows.length>4096||typeof loadPage!=='function')
    throw new Error('Original stage text raster provenance unavailable');
  const rows=new Map(),pages=[];
  const aborted=()=>{if(signal?.aborted)throw signal.reason??new DOMException('Aborted','AbortError');};
  for(const row of metadata.rows){
    if(!row||!['message','ruby','spell'].includes(row.kind)||typeof row.rawHex!=='string'||!/^(?:[0-9a-f]{2}){1,252}$/.test(row.rawHex)||row.key!==row.kind+':'+row.rawHex||rows.has(row.key)
      ||!Number.isInteger(row.page)||row.page<0||row.page>=metadata.pages.length||![row.x,row.y,row.width,row.height,row.advance].every(Number.isInteger)
      ||row.x<0||row.y<0||row.width!==(row.kind==='spell'?320:512)||row.height!==(row.kind==='spell'?17:20)||row.x+row.width>1024||row.y+row.height>2048||row.advance<0||row.advance>4096)
      throw new Error('Invalid original stage text raster row');
    rows.set(row.key,Object.freeze({...row}));
  }
  try{
    for(const page of metadata.pages){
      aborted();
      if(!/^textures\/original-stage-text(?:-full42-)?[0-9]+\.png$/.test(page.file)||!/^textures\/original-stage-outline(?:-full42-)?[0-9]+\.png$/.test(page.outlineFile)
        ||![page.sha256,page.outlineSha256].every(value=>/^[0-9a-f]{64}$/.test(value)))throw new Error('Invalid original stage text page pin');
      const image=await loadPage(page.file,page.sha256);pages.push({image,outlineImage:null});
      aborted();const outlineImage=await loadPage(page.outlineFile,page.outlineSha256);pages[pages.length-1].outlineImage=outlineImage;
      if(image.width!==1024||image.height!==2048||outlineImage.width!==1024||outlineImage.height!==2048)throw new Error('Original stage text page dimensions differ');
    }
    aborted();return{schema:metadata.schema,variant:metadata.variant,rows,pages};
  }catch(problem){for(const page of pages){page.image?.close();page.outlineImage?.close();}throw problem;}
}
export function drawOriginalStageText(context,bank,request){
  if(bank?.schema!=='th128-original-stage-text-raster/1'||bank.variant!=='zh'||!(bank.rows instanceof Map)||!Array.isArray(bank.pages)
    ||!request||!['message','ruby','spell'].includes(request.kind)||typeof request.rawHex!=='string'||!/^(?:[0-9a-f]{2}){1,252}$/.test(request.rawHex)
    ||![request.x,request.y,request.sx,request.sy,request.alpha,request.outlineAlpha].every(Number.isFinite)
    ||Math.abs(request.x)>65536||Math.abs(request.y)>65536||Math.abs(request.sx)>16||Math.abs(request.sy)>16
    ||request.alpha<0||request.alpha>1||request.outlineAlpha<0||request.outlineAlpha>1||!['left','right'].includes(request.align))
    throw new Error('Original stage text draw context rejected');
  const row=bank.rows.get(request.kind+':'+request.rawHex),page=bank.pages[row?.page];
  if(!row||!page?.image||!page.outlineImage)throw new Error('Original stage text raster owner missing');
  const left=request.kind==='spell'?-row.width:request.align==='right'?-row.advance:0;
  context.save();
  try{
    context.translate(request.x,request.y);context.scale(request.sx,request.sy);
    context.globalAlpha=request.outlineAlpha;
    if(request.outlineAlpha>0)context.drawImage(page.outlineImage,row.x,row.y,row.width,row.height,left,0,row.width,row.height);
    context.globalAlpha=request.alpha;
    if(request.alpha>0)context.drawImage(page.image,row.x,row.y,row.width,row.height,left,0,row.width,row.height);
  }finally{context.restore();}
}


export async function decodeOriginalMainMusicPNG(input){
  const bytes=new Uint8Array(input),view=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength);
  if(bytes.length<60||Array.from(bytes.subarray(0,8)).join(',')!=='137,80,78,71,13,10,26,10')throw new Error('Original Music PNG signature rejected');
  const chunks=[];let offset=8,width=0,height=0,ended=false;
  while(offset+12<=bytes.length){
    const length=view.getUint32(offset),name=String.fromCharCode(...bytes.subarray(offset+4,offset+8));
    if(length>16*1024*1024||offset+12+length>bytes.length)throw new Error('Original Music PNG chunk bounds rejected');
    const body=bytes.subarray(offset+8,offset+8+length);
    if(name==='IHDR'){
      if(width||length!==13||body[8]!==8||body[9]!==6||body[10]||body[11]||body[12])throw new Error('Original Music PNG colour format rejected');
      width=view.getUint32(offset+8);height=view.getUint32(offset+12);
      if(width!==1024||height!==2048)throw new Error('Original Music PNG dimensions rejected');
    }else if(name==='IDAT')chunks.push(body);
    else if(name==='IEND'){ended=true;offset+=12+length;break;}
    offset+=12+length;
  }
  if(!width||!ended||offset!==bytes.length||!chunks.length)throw new Error('Original Music PNG ownership rejected');
  const stream=new Blob(chunks).stream().pipeThrough(new DecompressionStream('deflate'));
  const raw=new Uint8Array(await new Response(stream).arrayBuffer()),stride=width*4;
  if(raw.length!==height*(stride+1))throw new Error('Original Music PNG decoded bounds rejected');
  const pixels=new Uint8Array(width*height*4);
  for(let row=0;row<height;++row){
    const start=row*(stride+1);if(raw[start]!==0)throw new Error('Original Music generated PNG filter differs');
    pixels.set(raw.subarray(start+1,start+1+stride),row*stride);
  }
  return{width,height,pixels,close(){this.pixels=null;}};
}

export async function loadOriginalMainMusicRasters(metadata,loadPage,{signal}={}){
  if(metadata?.schema!=='th128-original-main-music-raster/1'||!Array.isArray(metadata.rows)||metadata.rows.length<1||metadata.rows.length>512
    ||!Array.isArray(metadata.pages)||metadata.pages.length<1||metadata.pages.length>8||typeof loadPage!=='function')throw new Error('Original main raster metadata rejected');
  const rows=new Map(),pages=[];
  const checked=()=>{if(signal?.aborted)throw signal.reason??new DOMException('Aborted','AbortError');};
  for(const row of metadata.rows){
    if(!row||!['zh','ja'].includes(row.variant)||typeof row.rawHex!=='string'||!/^(?:[a-f0-9]{2}){0,252}$/.test(row.rawHex)
      ||row.key!==row.variant+':'+row.rawHex||rows.has(row.key)||!Number.isInteger(row.page)||row.page<0||row.page>=metadata.pages.length
      ||![row.x,row.y].every(Number.isInteger)||row.x<0||row.x+512>1024||row.y<0||row.y+18>2048||row.width!==512||row.height!==18
      ||!/^([a-f0-9]{64})$/.test(row.sha256)||!/^([a-f0-9]{64})$/.test(row.sha256Height17))throw new Error('Original main raster row rejected');
    rows.set(row.key,{...row});
  }
  try{
    for(const page of metadata.pages){
      checked();if(!page||page.width!==1024||page.height!==2048||!/^[a-f0-9]{64}$/.test(page.sha256))throw new Error('Original main raster page rejected');
      const image=await loadPage(page.file,page.sha256);pages.push(image);
      if(image.width!==1024||image.height!==2048)throw new Error('Original main raster page dimensions differ');
    }
    checked();return{schema:metadata.schema,rows,pages};
  }catch(problem){for(const page of pages)page.close();throw problem;}
}
export function disposeOriginalMainMusicRasters(bank){if(bank?.pages)for(const page of bank.pages)page.close();}
export async function uploadOriginalMainMusicRequest(renderer,bank,request,variant,{signal}={}){
  if(!bank||bank.schema!=='th128-original-main-music-raster/1'||!['zh','ja'].includes(variant)||request?.encoding!==(variant==='zh'?'gbk':'shift_jis'))
    throw new Error('Original Music variant/raster ownership rejected');
  const row=bank.rows.get(variant+':'+request.textHex);
  if(!row||![17,18].includes(request.rect?.[3]))throw new Error('Original Music raw byte raster unavailable');
  const page=bank.pages[row.page];
  if(!(page.pixels instanceof Uint8Array))throw new Error('Original Music straight-alpha pixels unavailable');
  const pixels=new Uint8Array(512*request.rect[3]*4);
  for(let line=0;line<request.rect[3];++line)pixels.set(page.pixels.subarray(((row.y+line)*1024+row.x)*4,((row.y+line)*1024+row.x+512)*4),line*512*4);
  const bitmap={width:512,height:request.rect[3],pixels,close(){}};
  try{
    if(signal?.aborted)throw signal.reason??new DOMException('Aborted','AbortError');
    return await renderer.uploadOriginalMusicText(request,bitmap,request.rect[3]===17?row.sha256Height17:row.sha256);
  }finally{bitmap.close();}
}

export class OriginalPresentationFPS {
  constructor(){this.reset();}
  reset(){this.anchor=null;this.last=null;this.count=0;this.value=null;}
  observe(timestamp){
    if(!Number.isFinite(timestamp)||timestamp<0)throw new RangeError('Invalid presentation timestamp');
    if(this.last===null||timestamp<this.last){this.reset();this.anchor=this.last=timestamp;return null;}
    if(timestamp===this.last)return this.value;
    this.last=timestamp;++this.count;
    const elapsed=timestamp-this.anchor;
    if(elapsed>=1000){this.value=Math.fround(this.count*1000/elapsed);this.anchor=timestamp;this.count=0;}
    return this.value;
  }
}
export function originalFPSText(rate){
  if(rate===null)return null;
  if(!Number.isFinite(rate)||rate<0||rate>1000000)throw new RangeError('Invalid measured FPS');
  return (Math.fround(rate)+0.05).toFixed(1)+'fps';
}
export function originalFPSDraws(rate){
  const text=originalFPSText(rate);if(text===null)return [];
  const value=Math.fround(rate),color=value<30?0xff5050ff:value<40?0xffa0a0ff:0xffffffff;
  const left=588-Math.max(0,text.length-7)*7;
  return Array.from(text,(character,index)=>{
    const glyph=character.charCodeAt(0)-32,tx=(glyph%32)*8,ty=128+Math.floor(glyph/32)*8;
    return {id:51000000+index,parent:0,bank:11,sprite:98+glyph,entry:0,layer:0,drawPass:'original-fps',
      mode:0,anchorX:1,anchorY:1,blend:0,flags:3,flags2:0,color,secondary:color,
      width:8,height:8,x:left+index*7,y:470,z:0,rotation:0,sx:1,sy:1,u:0,v:0,
      uvScaleX:1,uvScaleY:1,u0:tx/256,v0:ty/256,u1:(tx+8)/256,v1:(ty+8)/256,
      rx:0,ry:0,script:-1,clock:0,order:index};
  });
}

export function composeOriginalResultFont(bank,{textHex,colorRef}){
  if(!bank||!(bank.glyphs instanceof Map)||typeof textHex!=='string'||!(/^(?:[a-f0-9]{2}){0,128}$/).test(textHex)
    ||![0xffffff,0x808080,0xdfdfdf,0xffffa0,0xffff80].includes(colorRef))throw new Error('Original RESULT font composition descriptor rejected');
  const bytes=Uint8Array.from(textHex.match(/../g)||[],h=>parseInt(h,16)),glyphs=[];let advance=0;
  for(let i=0;i<bytes.length;){const length=bytes[i]<128?1:2;if(i+length>bytes.length)throw new Error('Original RESULT GBK glyph truncated');const key=Array.from(bytes.subarray(i,i+length),b=>b.toString(16).padStart(2,'0')).join(''),g=bank.glyphs.get(key);if(!g||!Number.isInteger(g.advanceSource)||g.advanceSource<1||g.advanceSource>40||!(g.outline instanceof Uint16Array)||g.outline.length!==2560||!(g.foreground?.[colorRef] instanceof Uint16Array)||g.foreground[colorRef].length!==2560)throw new Error('Original RESULT exact glyph plane unavailable');glyphs.push({g,x:advance});advance+=g.advanceSource;i+=length;}
  if(advance>2048)throw new Error('Original RESULT text source exceeds native buffer');
  const raw=new Uint8Array(2048*129),view=new DataView(raw.buffer);for(let y=0;y<40;y++)for(let x=0;x<1024;x++)view.setUint16(y*2048+x*2,0xf000,true);
  const origin=512-bytes.length*8;
  for(let pass=0;pass<2;pass++)for(const {g,x}of glyphs){const pixels=pass?g.foreground[colorRef]:g.outline,left=origin+x-16;for(let y=0;y<40;y++)for(let sx=0;sx<64;sx++){const targetX=left+sx,value=pixels[y*64+sx];if(targetX<0||targetX>=1024||value===0xf000)continue;view.setUint16(y*2048+targetX*2,value,true);}}
  return{width:1024,height:129,pitch:2048,format:26,rows:40,sourceWidth:1024,sourceHeight:36,targetWidth:512,targetHeight:18,raw};
}

export function processOriginalResultFont(surface){
  if(surface?.width!==1024||surface.height!==129||surface.pitch!==2048||surface.format!==26||surface.rows!==40||!(surface.raw instanceof Uint8Array)||surface.raw.length!==264192)throw new Error('Original RESULT font source surface rejected');
  const bytes=surface.raw.slice(),view=new DataView(bytes.buffer);for(let offset=1;offset<2048*40;offset+=2)bytes[offset]^=240;
  for(let row=0;row<40;row++)for(let column=0;column<1024;column++){
    const offset=row*2048+column*2;if(view.getUint16(offset,true)&0xf000)continue;
    const neighbors=[];if(column>0)neighbors.push(offset-2);if(column+1<1024)neighbors.push(offset+2);if(row>0)neighbors.push(offset-2048);if(row+1<129)neighbors.push(offset+2048);let count=0,red=0,green=0,blue=0;
    for(const neighbor of neighbors){const value=view.getUint16(neighbor,true);if(!(value&0xf000))continue;red+=(value>>>8)&15;green+=(value>>>4)&15;blue+=value&15;count++;}if(count>1){red=Math.trunc(red/count);green=Math.trunc(green/count);blue=Math.trunc(blue/count);}view.setUint16(offset,((red>>>1)<<8)|((green>>>1)<<4)|(blue>>>1),true);
  }
  const pixels=new Uint8Array(512*17*4);for(let y=0;y<17;y++)for(let x=0;x<512;x++){const channels=[0,0,0,0];for(let dy=0;dy<2;dy++)for(let dx=0;dx<2;dx++){const value=view.getUint16((y*2+dy)*2048+(x*2+dx)*2,true);channels[0]+=((value>>>8)&15)*4.25;channels[1]+=((value>>>4)&15)*4.25;channels[2]+=(value&15)*4.25;channels[3]+=(value>>>12)*4.25;}for(let c=0;c<4;c++)pixels[(y*512+x)*4+c]=Math.round(channels[c]);}
  return{width:512,height:17,pixels,CPUProjectionAdapter:true,driverBitParity:false,close(){}};
}
export async function loadOriginalResultFont(metadata,loadPage,{signal}={}){
  if(metadata?.schema!=='th128-original-result-glyph-layers/1'||!Array.isArray(metadata.glyphs)||metadata.glyphs.length<96||metadata.glyphs.length>512||!Array.isArray(metadata.pages)||metadata.pages.length!==2||typeof loadPage!=='function')throw new Error('Original RESULT glyph atlas metadata rejected');
  const pages=[],glyphs=new Map();try{
    for(const page of metadata.pages){if(signal?.aborted)throw signal.reason??new DOMException('Aborted','AbortError');if(!/^textures\/original-result-glyph[01]\.png$/.test(page.file)||page.width!==1024||page.height!==2048||!/^[a-f0-9]{64}$/.test(page.sha256))throw new Error('Original RESULT glyph page rejected');const pixels=await loadPage(page.file,page.sha256);if(pixels.width!==1024||pixels.height!==2048||!(pixels.pixels instanceof Uint8Array)||pixels.pixels.length!==8388608)throw new Error('Original RESULT glyph atlas pixel bounds rejected');pages.push(pixels);}
    for(const row of metadata.glyphs){if(!/^(?:[a-f0-9]{2}){1,2}$/.test(row.rawHex)||glyphs.has(row.rawHex)||!Number.isInteger(row.advanceSource)||row.advanceSource<1||row.advanceSource>40)throw new Error('Original RESULT glyph identity rejected');const plane=key=>{const p=row.planes?.[key];if(!p||!Number.isInteger(p.page)||p.page<0||p.page>=2||![p.x,p.y].every(Number.isInteger)||p.width!==64||p.height!==40||p.x<0||p.x+64>1024||p.y<0||p.y+40>2048)throw new Error('Original RESULT glyph plane bounds rejected');const data=pages[p.page].pixels,out=new Uint16Array(2560);for(let y=0;y<40;y++)for(let x=0;x<64;x++){const i=((p.y+y)*1024+p.x+x)*4;if(data[i+2]!==0||data[i+3]!==255)throw new Error('Original RESULT glyph integer plane rejected');out[y*64+x]=data[i]|data[i+1]<<8;}return out;};glyphs.set(row.rawHex,{advanceSource:row.advanceSource,outline:plane('outline'),foreground:Object.fromEntries([0xffffff,0x808080,0xdfdfdf,0xffffa0,0xffff80].map(c=>[c,plane(String(c))]))});}
    if(signal?.aborted)throw signal.reason??new DOMException('Aborted','AbortError');return{glyphs,CPUProjectionAdapter:true,driverBitParity:false};
  }finally{for(const page of pages)page.close();}
}
export async function uploadOriginalResultTextRequest(renderer,bank,request,{signal}={}){
  if(request?.bank!==6||request.entry!==0||request.format!==5||request.encoding!=='gbk'||request.texture?.[0]!==512||request.texture?.[1]!==512||request.rect?.[0]!==0||request.rect?.[2]!==512||request.rect?.[3]!==17)throw new Error('Original RESULT text request rejected');
  if(signal?.aborted)throw signal.reason??new DOMException('Aborted','AbortError');const bitmap=processOriginalResultFont(composeOriginalResultFont(bank,{textHex:request.textHex,colorRef:request.foreground}));
  const digest=Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',bitmap.pixels)),b=>b.toString(16).padStart(2,'0')).join('');if(signal?.aborted)throw signal.reason??new DOMException('Aborted','AbortError');return await renderer.uploadOriginalMusicText(request,bitmap,digest);
}
