import assert from 'node:assert/strict';
import {execFileSync} from 'node:child_process';
import {readFileSync,writeFileSync} from 'node:fs';
import {Renderer as Current} from '../web/renderer.mjs';

const args=Object.fromEntries(process.argv.slice(2).map(a=>{const at=a.indexOf('=');return [a.slice(2,at),a.slice(at+1)];}));
const referenceCommit=args.commit||'497969450a26f0d809d6afd8d96d41b5d0f90cea';
const source=args.reference?readFileSync(args.reference,'utf8'):execFileSync('git',['show',referenceCommit+':ports/thkkd/web/renderer.mjs'],{encoding:'utf8'});
const {Renderer:Before}=await import('data:text/javascript;base64,'+Buffer.from(source).toString('base64'));
let seed=918273;
const random=()=>((seed=Math.imul(seed,1664525)+1013904223>>>0)/4294967296);
const textures=[1,2,3].map(texture=>({texture})),commands=[],previous=new Map();
for(let i=0;i<1800;i++){
 const x=random()*640,y=random()*480,texture=i<600?textures[0]:textures[i%3],identity='object:'+i;
 const c={texture,identity,points:[x,y,x+32,y,x+32,y+16,x,y+16],uv:[0,0,1,0,1,1,0,1],tint:[random(),random(),random(),random()],depth:[1,.5,.7,.3,.8,.6,1,.5],mode:i<600?1:i%5,opaque:i<600?false:!!(i%2),area:i<600?[0,0,640,480]:[i%3,2,630,470]};
 if(i%4===0)c.vertexColors=Array.from({length:16},random);
 const prior={...c,points:c.points.map((v,j)=>v-(i%13===0?80:j%2?2:4)),uv:[...c.uv]};
 if(i%7===0)prior.uv[0]=.25;if(i%11===0)prior.texture=textures[2];
 commands.push(c);previous.set(identity,prior);
}
function draw(Class,alpha){
 const r=Object.create(Class.prototype),events=[];let buffer,texture,opaque,equation,blend,clip,uploads=0,fraction=0;
 const constants={ARRAY_BUFFER:34962,STREAM_DRAW:35040,TRIANGLES:4,TEXTURE_2D:3553,FUNC_REVERSE_SUBTRACT:32779,FUNC_ADD:32774,ZERO:0,SRC_COLOR:768,SRC_ALPHA:770,ONE:1,ONE_MINUS_SRC_ALPHA:771,COLOR_BUFFER_BIT:16384};
 r.gl=new Proxy({...constants,
  bufferData(_target,data){buffer=Float32Array.from(data);uploads++;},bindTexture(_target,value){texture=value;},uniform1i(_where,value){opaque=value;},
  uniform1f(_where,value){fraction=value;},
  blendEquation(value){equation=value;},blendFunc(...value){blend=value;},scissor(...value){clip=value;},
  drawArrays(kind,first,count){const stride=r.vertexStride||10,vertices=[];for(let vertex=first;vertex<first+count;vertex++){const offset=vertex*stride;for(let field=0;field<10;field++)vertices.push(stride>10&&field<2?Math.fround(buffer[offset+field]+buffer[offset+10+field]*fraction):buffer[offset+field]);}events.push(['draw',kind,texture,opaque,equation,blend,clip,...vertices]);},
 },{get(o,k){if(k in o)return o[k];return()=>{};}});
 Object.assign(r,{commands,previous,opaque:9,vertices:new Float32Array(60*256)});r.present(alpha);return {events,uploads};
}
const snapshot=JSON.stringify(commands),alphas=[0,.25,.5,.99,1,1.5,-1];
let uploads;
for(const alpha of alphas){
 const next=draw(Current,alpha),old=draw(Before,alpha),actual=next.events,expected=old.events;uploads={before:old.uploads,after:next.uploads};
 assert.equal(actual.length,expected.length,'批次数量');
 for(let i=0;i<actual.length;i++){assert.deepEqual(actual[i].slice(0,7),expected[i].slice(0,7),'绘制状态与顺序');assert.equal(actual[i].length,expected[i].length);for(let j=7;j<actual[i].length;j++)if(!Object.is(actual[i][j],expected[i][j])&&Math.abs(actual[i][j]-expected[i][j])>(alpha>0&&(j-7)%10<2?.0002:0))throw Error(`首个 GPU 提交差异：alpha=${alpha} event=${i} field=${j}`);}
}
assert.equal(JSON.stringify(commands),snapshot);
function sprites(Class){
 const r=Object.assign(Object.create(Class.prototype),{commands:[],current:[],identities:new Map(),previous:new Map(),brightness:[.3,.7,.9],alpha:.6,area:[4,8,632,468]});
 for(let frame=0;frame<4;frame++){
  r.clear();r.tick=frame;r.scene=frame<3?'game':'menu';
  for(let i=0;i<1200;i++){r.identity='sprite:'+i;r.mode=i%5;r.sprite({texture:1,width:257,height:255,x:7,y:11,w:32,h:16},i%640,i%480,(i%9-4)/3,i/100,i%2===0,i%3===0,i%5!==0);}
  r.commit();
 }
 const geometry=({transform,topology,...command})=>command;
 return {commands:r.commands.map(geometry),previous:[...r.previous].map(([key,command])=>[key,geometry(command)])};
}
assert.deepEqual(sprites(Current),sprites(Before),'精灵几何、透明度、UV 与切面后的历史映射');
assert.equal(uploads.after,1);
const report={result:'passed',commands:1800,alphas,uploads,spriteCommands:4800,positionTolerance:.0002,checks:['exact current-frame attributes','predicted positions within Float32 rounding','draw order and effective GPU state','buffer growth past 256 quads','perspective and per-vertex colors','snapshot immutability','identity and teleport interpolation boundaries','sprite geometry and scene transitions'],referenceCommit};
if(args.output)writeFileSync(args.output,JSON.stringify(report,null,2)+'\n');console.log(JSON.stringify(report));
