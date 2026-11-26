import assert from 'node:assert/strict';
import {execFileSync} from 'node:child_process';
import {readFileSync,writeFileSync} from 'node:fs';
import {isDeepStrictEqual} from 'node:util';
import {Renderer as Current} from '../web/renderer.mjs';

const args=Object.fromEntries(process.argv.slice(2).map(a=>{const at=a.indexOf('=');return [a.slice(2,at),a.slice(at+1)];}));
const referenceCommit=args.commit||'a1da2e1e23a5d853d0ed06e195b9b91d6eb6656a';
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
 const r=Object.create(Class.prototype),events=[];
 const constants={ARRAY_BUFFER:34962,STREAM_DRAW:35040,TRIANGLES:4,TEXTURE_2D:3553,FUNC_REVERSE_SUBTRACT:32779,FUNC_ADD:32774,ZERO:0,SRC_COLOR:768,SRC_ALPHA:770,ONE:1,ONE_MINUS_SRC_ALPHA:771,COLOR_BUFFER_BIT:16384};
 r.gl=new Proxy({...constants,bufferData(_target,data){events.push(['buffer',...data]);}},{get(o,k){if(k in o)return o[k];return(...values)=>events.push([k,...values]);}});
 Object.assign(r,{commands,previous,opaque:9,vertices:new Float32Array(60*256)});r.present(alpha);return events;
}
const snapshot=JSON.stringify(commands),alphas=[0,.25,.5,.99,1,1.5,-1];
for(const alpha of alphas){
 const actual=draw(Current,alpha),expected=draw(Before,alpha);
 assert.equal(actual.length,expected.length,'批次数量');
 for(let i=0;i<actual.length;i++)if(!isDeepStrictEqual(actual[i],expected[i]))throw Error(`首个 GPU 提交差异：alpha=${alpha} event=${i} kind=${actual[i][0]}`);
}
assert.equal(JSON.stringify(commands),snapshot);
const report={result:'passed',commands:1800,alphas,checks:['exact Float32 vertex streams','batch order and state','buffer growth past 256 quads','perspective and per-vertex colors','snapshot immutability','identity and teleport interpolation boundaries'],referenceCommit};
if(args.output)writeFileSync(args.output,JSON.stringify(report,null,2)+'\n');console.log(JSON.stringify(report));
