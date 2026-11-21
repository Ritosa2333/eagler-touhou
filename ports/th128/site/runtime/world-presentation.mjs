const fields=['id','parent','bank','sprite','entry','layer','mode','anchorX','anchorY','blend','flags','flags2','color','secondary','script','clock','visualTicks','width','height','x','y','z','rotation','sx','sy','u','v','uvScaleX','uvScaleY','u0','v0','u1','v1','rx','ry'];
const passes=[null,'hostile-bullets','hostile-lasers','player','items','frozen-bullets','original-percent-hud','original-freeze-reward-panel','original-fps','original-menu-font0','original-pause-shade','original-pause-backdrop','original-pause-front','playerShotGroup','original-spell-bonus'];
const surfaces=[null,'stage','main'],utf8=new TextDecoder('utf-8',{fatal:true}),canonicalValues=new Map();
const pendingOwnedDraws=new WeakMap();
// One-use handoff for the fresh packet consumer. Legacy JSON sidecars still
// go through the complete display validation with the packed vertex count.
export function takeOwnedDrawValidation(records){
 const validation=pendingOwnedDraws.get(records);pendingOwnedDraws.delete(records);return validation;
}
function canonicalNumber(value){
 if(Number.isInteger(value))return value;
 const cached=canonicalValues.get(value);if(cached!==undefined)return cached;
 const result=general9(value);canonicalValues.set(value,result);return result;
}
const rawNumber=value=>value;
function general9(value){
 if(Number.isInteger(value))return value;
 const scale=10**(8-Math.floor(Math.log10(Math.abs(value)))),scaled=Math.abs(value)*scale,lower=Math.floor(scaled);
 if(scaled-lower===.5&&lower%2===0){
  const exact=Math.abs(value).toExponential(20),[digits,exponent]=exact.split('e'),all=digits.replace('.','');
  if(all[9]==='5'&&/^0*$/.test(all.slice(10)))return (value<0?-1:1)*Number(all.slice(0,9)+'e'+(Number(exponent)-8));
 }
 return Number(value.toPrecision(9));
}
function bounds(pointer,bytes,heap){return Number.isSafeInteger(pointer)&&Number.isSafeInteger(bytes)&&pointer>=0&&bytes>=0&&pointer+bytes<=heap.byteLength;}
function fault(module){const value=module.UTF8ToString(module._th128_world_bullet_draw_error());if(value)throw new Error(value);}
function unpackDrawGeometry(ref,rows,seen,number){
 const [,kind,start,count,owner,column]=ref;
 if(ref.length!==6||!Number.isSafeInteger(start)||start!==seen||!Number.isSafeInteger(count)||start+count>rows.length/7
  ||!(kind===1&&count===34&&Number.isSafeInteger(owner)&&owner>0&&owner<=0xffffffff&&Number.isInteger(column)&&column>=0&&column<16
   ||kind===2&&count>=4&&count<=8192&&count%2===0&&owner===0&&column===0))throw new Error('Invalid World draw packet vertex reference');
 const vertices=new Array(count);
 for(let i=0;i<count;++i){
  const base=(start+i)*7,x=rows[base],y=rows[base+1],z=rows[base+2],rhw=rows[base+3],u=rows[base+4],v=rows[base+5],color=rows[base+6];
  if(!Number.isFinite(x)||!Number.isFinite(y)||!Number.isFinite(z)||!Number.isFinite(rhw)||!Number.isFinite(u)||!Number.isFinite(v)
   ||!Number.isInteger(color)||color<0||color>0xffffffff)throw new Error('Invalid World draw packet vertex value');
  vertices[i]={x:number(x),y:number(y),z:number(z),rhw:number(rhw),color,u:number(u),v:number(v)};
 }
 return kind===1?{kind:'triangle-strip',owner,column,vertices}:{kind:'anm-ring',vertices};
}
export function readWorldDrawRecords(module,{rawNumbers=false,fastNumbers=false}={}){
 if(canonicalValues.size>65536)canonicalValues.clear();
 const number=rawNumbers?rawNumber:canonicalNumber;
 if(typeof module?._th128_world_draw_packet!=='function'){
  const value=module.UTF8ToString(module._th128_world_bullet_draws());fault(module);return JSON.parse(value);
 }
 const pointer=module._th128_world_draw_packet();fault(module);
 const heap=module.HEAPU8;
 if(!(heap instanceof Uint8Array)||!pointer||pointer%8||!bounds(pointer,64,heap))throw new Error('Invalid World draw packet header bounds');
 const header=new Float64Array(heap.buffer,heap.byteOffset+pointer,8),[magic,version,count,stride,rowsPointer,textPointer,textBytes,reserved]=header;
 if(magic!==0x54483131||version!==1&&version!==2||stride!==39||reserved!==0||!Number.isSafeInteger(count)||count<0||count>65536
  ||rowsPointer%8||!bounds(rowsPointer,count*stride*8,heap)||!bounds(textPointer,textBytes,heap)||textBytes>32*1024*1024)
  throw new Error('Invalid World draw packet schema, stride or bounds');
 let vertexRows=null,seenVertices=0;
 if(version===2){
  if(!bounds(pointer,80,heap))throw new Error('Invalid World draw packet vertex header');
  const [start,size]=new Float64Array(heap.buffer,heap.byteOffset+pointer+64,2);
  if(!Number.isSafeInteger(size)||size<0||size>65536||start%8||!bounds(start,size*7*8,heap))throw new Error('Invalid World draw packet vertex bounds');
  vertexRows=new Float64Array(heap.buffer,heap.byteOffset+start,size*7);
 }
 const refs=JSON.parse(utf8.decode(heap.subarray(textPointer,textPointer+textBytes)));
 if(!Array.isArray(refs)||refs.length>65536)throw new Error('Invalid World draw packet record budget');
 const rows=new Float64Array(heap.buffer,heap.byteOffset+rowsPointer,count*stride),used=new Uint8Array(count),output=new Array(refs.length),unchecked=[];let seen=0;
 for(let index=0;index<refs.length;++index){
  let reference=refs[index],geometry=null;
  if(vertexRows&&Array.isArray(reference)){
   geometry=unpackDrawGeometry(reference,vertexRows,seenVertices,number);seenVertices+=geometry.vertices.length;
   reference=reference[0];
   if(typeof reference!=='number')throw new Error('Invalid World draw packet geometry row reference');
  }
  if(typeof reference!=='number'){
   if(!reference||typeof reference!=='object'||Array.isArray(reference))throw new Error('Invalid World draw packet JSON fallback');
   unchecked.push(reference);output[index]=reference;continue;
  }
  if(!Number.isSafeInteger(reference)||reference<0||reference>=count||used[reference])throw new Error('Invalid World draw packet reference');
  used[reference]=1;++seen;const base=reference*stride;
  const effect=rows[base+35],surface=rows[base+36],pass=rows[base+37],kind=rows[base+38];
  if(effect!==0&&effect!==1||!Number.isInteger(surface)||surface<0||surface>=surfaces.length
   ||!Number.isInteger(pass)||pass<0||pass>=passes.length||kind!==0&&kind!==1)throw new Error('Invalid World draw packet enum');
  if(rawNumbers||fastNumbers){
   for(let k=0;k<17;++k)if(!Number.isSafeInteger(rows[base+k]))throw new Error('Non-finite World draw packet field');
   for(let k=17;k<35;++k)if(!Number.isFinite(rows[base+k]))throw new Error('Non-finite World draw packet field');
   const record=kind===1?{id:rows[base+0],parent:rows[base+1],bank:rows[base+2],sprite:rows[base+3],entry:rows[base+4],layer:rows[base+5],mode:rows[base+6],anchorX:rows[base+7],anchorY:rows[base+8],blend:rows[base+9],flags:rows[base+10],flags2:rows[base+11],color:rows[base+12],secondary:rows[base+13],width:number(rows[base+17]),height:number(rows[base+18]),x:number(rows[base+19]),y:number(rows[base+20]),z:number(rows[base+21]),rotation:number(rows[base+22]),sx:number(rows[base+23]),sy:number(rows[base+24]),u:number(rows[base+25]),v:number(rows[base+26]),uvScaleX:number(rows[base+27]),uvScaleY:number(rows[base+28]),u0:number(rows[base+29]),v0:number(rows[base+30]),u1:number(rows[base+31]),v1:number(rows[base+32]),rx:number(rows[base+33]),ry:number(rows[base+34])}:{id:rows[base+0],parent:rows[base+1],bank:rows[base+2],sprite:rows[base+3],entry:rows[base+4],layer:rows[base+5],mode:rows[base+6],anchorX:rows[base+7],anchorY:rows[base+8],blend:rows[base+9],flags:rows[base+10],flags2:rows[base+11],color:rows[base+12],secondary:rows[base+13],script:rows[base+14],clock:rows[base+15],visualTicks:rows[base+16],width:number(rows[base+17]),height:number(rows[base+18]),x:number(rows[base+19]),y:number(rows[base+20]),z:number(rows[base+21]),rotation:number(rows[base+22]),sx:number(rows[base+23]),sy:number(rows[base+24]),u:number(rows[base+25]),v:number(rows[base+26]),uvScaleX:number(rows[base+27]),uvScaleY:number(rows[base+28]),u0:number(rows[base+29]),v0:number(rows[base+30]),u1:number(rows[base+31]),v1:number(rows[base+32]),rx:number(rows[base+33]),ry:number(rows[base+34])};
   if(kind===0)record.effectSystem=effect?'unified-primary':'other';if(surface)record.stageSurface=surfaces[surface];if(pass)record.drawPass=passes[pass];
   if(geometry)record.geometry=geometry;output[index]=record;continue;
  }
  const record={};
  for(let k=0;k<fields.length;++k){
   const value=rows[base+k];
   if(!Number.isFinite(value)||k<17&&!Number.isSafeInteger(value))throw new Error('Non-finite World draw packet field');
   if(kind===1&&(k===14||k===15||k===16))continue;
   // Preserve the old JSON writer's general9 numeric contract, including signed zero.
   record[fields[k]]=k>=17?general9(value):value;
  }
  if(kind===0)record.effectSystem=effect?'unified-primary':'other';
  if(surface)record.stageSurface=surfaces[surface];if(pass)record.drawPass=passes[pass];if(geometry)record.geometry=geometry;output[index]=record;
 }
 if(seen!==count)throw new Error('Unreferenced World draw packet row');
 if(vertexRows&&seenVertices*7!==vertexRows.length)throw new Error('Unreferenced World draw packet vertices');
 if(fastNumbers)pendingOwnedDraws.set(output,{unchecked,vertices:seenVertices});
 return output;
}
