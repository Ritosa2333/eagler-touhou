import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';import {resolve} from 'node:path';
import {core,oracle,memory,report,root} from './helpers.mjs';

test('TH11 HUD score, power, graze, communication and timer text matches original',async()=>{
 const c=await core(),m=await oracle(),f=c.hud_create(),hud=m.allocate(0x4460),supervisor=m.allocate(0x18500),enemy=m.allocate(0x100),records=m.allocate(0x30000);
 m.u32(0x4a8d84,hud);m.u32(0x4a8d58,supervisor);m.u32(0x4a8d7c,enemy);m.u32(0x4a8ebc,records);
 const overlay=m.allocate(0x434),spell=m.allocate(0x920),front=readFileSync(resolve(root,'reference/assets/front.anm')),fp=c.allocate(front.length);memory(c,fp,front.length).set(front);assert.equal(c.hud_load(f,0,fp,front.length),1);
 m.u32(0x4a8d6c,spell);
 m.replace(0x4561e0,'spell overlay lookup only; native text layout retained',()=>m.u32(m.reg('ESP')+4)===1?overlay:0,1);
 m.replace(0x451ef0,'embedded HUD sprite draw boundary',()=>0);
 m.replace(0x45fc2c,'security cookie check',()=>0);
 // Keep all native request generation, including integer divisions and alpha.
 // Isolate the platform CRT formatter; argument reads come from native va_list.
 const format=(fmt,p)=>{let q=p;return fmt.replace(/%([ 0]?)(\d*)(?:\.(\d+))?([df])/g,(_,pad,width,precision,type)=>{
   if(type==='f'){const v=Buffer.from(m.bytes(q,8)).readDoubleLE();q+=8;return v.toFixed(precision===undefined?6:Number(precision)).padStart(Number(width)||0,pad==='0'?'0':' ');}
   const v=m.i32(q);q+=4;const neg=v<0,body=String(Math.abs(v)).padStart(Number(precision)||0,'0');return ((neg?'-':'')+body).padStart(Number(width)||0,pad==='0'?'0':' ');
 });};
 m.replace(0x45fdd2,'vsprintf',()=>{const sp=m.reg('ESP'),out=m.u32(sp+4),value=format(m.string(m.u32(sp+8)),m.u32(sp+12));m.write(out,Buffer.from(value+'\0'));return value.length;});
 const dv=()=>new DataView(c.memory.buffer),set=(p,v)=>dv().setInt32(p,v,true),get=p=>dv().getInt32(p,true),flt=p=>dv().getFloat32(p,true);
 const string=p=>{const b=memory(c,p,256),n=b.indexOf(0);return Buffer.from(b.subarray(0,n)).toString();};
 const e=c.hud_data(f,0),input=c.hud_data(f,1),score=c.hud_score(f);
 let checks=0;
 try{for(let sample=0;sample<180;++sample){
  const powerStep=sample%3===0?20:100,power=sample%5*powerStep+sample%powerStep,max=powerStep*(sample%3===0?8:4);
  const displayed=[0,9,12345678,99999999,100000000,999999999][sample%6],high=displayed+13,cont=sample%10,alpha=sample*7%256,practice=sample%3===1;
  const graze=[0,999,90000,99999999][sample%4],comm=[0,9999,10000,12000][sample%4],points=5000000+sample*17;
  const frames=[0,1,59,60,5999,60000][sample%6],encoded=[0,0x6ae9c24,2206633,3708958][sample%4],overlayColor=((255-alpha)<<24)|0xffffff;
  assert.equal(c.hud_spell_overlay(f,overlayColor,frames,encoded),1);m.u32(hud+0x43bc,1);m.u32(hud+0x43c0,1);m.u32(overlay+0x374,overlayColor);m.i32(spell+0x8f8,frames);m.i32(spell+0x90c,encoded);
  for(const [off,value,address]of [[4,power,0x4a56e8],[8,points,0x4a56f0],[12,comm,0x4a56f4],[32,max,0x4a5748],[36,powerStep,0x4a574c],[40,graze,0x4a5754]]){set(e+off,value);m.i32(address,value);}
  for(const [off,value,address]of [[0,displayed,hud+0x43e4],[8,high,0x4a56e0],[12,cont,0x4a573c],[16,cont,0x4a5740]]){set(score+off,value);m.i32(address,value);}
  const active=sample%2; c.hud_boss(f,active);m.u32(enemy+0x1c,active?enemy:0);
  set(input+24,3);set(input+28,sample%100);m.i32(hud+0x4440,3);m.i32(hud+0x4444,sample%100);
  set(input+32,12345678);memory(c,input+36,1)[0]=+practice;
  m.u32(0x4a5758,practice?0x10:0);m.u32(0x4a5710,0);m.u32(0x4a5714,0);m.u32(0x4a5728,1);m.u32(0x4a5720,0);m.i32(records+0x5ac,12345678);
  for(const [ptr,address,value]of [[c.hud_data(f,4)+0x374,hud+0x384,(alpha<<24)|0xffffff],[c.hud_data(f,6)+0x374,hud+0x31c0,((255-alpha)<<24)|0xffffff],[c.hud_data(f,5)+0x374,hud+0x2958,0x80908070]]){set(ptr,value);m.u32(address,value);}
  m.u32(supervisor+0x1847c,0);m.u32(supervisor+0x18480,0xffffffff);m.f32(supervisor+0x18484,1);m.f32(supervisor+0x18488,1);m.u32(supervisor+0x18498,0);m.u32(supervisor+0x1849c,0);
  m.reg('ESI',hud);m.call(0x41c660);
  assert.equal(c.hud_queue_text(f),1);
  assert.equal(c.hud_text_count(f),m.u32(supervisor+0x1847c),`sample${sample} request count`);
  for(let i=0;i<c.hud_text_count(f);++i){
   const q=supervisor+0x87c+i*0x130,p=c.hud_text_position(f,i),s=c.hud_text_style(f,i),label=`sample${sample} request${i}`;
   assert.equal(string(c.hud_text_string(f,i)),m.string(q),label+' text');
   for(let k=0;k<3;++k)assert.equal(flt(p+k*4),m.f32(q+0x100+k*4),label+' position');
   assert.equal(get(s)>>>0,m.u32(q+0x10c),label+' color');
   for(let k=0;k<2;++k)assert.equal(flt(s+4+k*4),m.f32(q+0x110+k*4),label+' scale');
   assert.equal(get(s+12),m.i32(q+0x120),label+' font');assert.equal(get(s+20),m.i32(q+0x128),label+' pass');++checks;
  }
 }
 report('hud-text',{passed:true,checks,scope:'Original 41c660 HUD ASCII request generation with native arithmetic: regular/practice score and continues, both power scales, point/graze communication text, alpha, countdown hundredths, spell-result gameplay/wall times including invalid replay checksums, coordinates, request order and rendering passes.'});
 }finally{c.release(fp);c.hud_delete(f);m.close();}
});
