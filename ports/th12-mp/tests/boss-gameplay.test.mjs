import test from 'node:test';
import assert from 'node:assert/strict';
import createCore from '../site/runtime/core.mjs';
import fs from 'node:fs';

const word=value=>{const b=Buffer.alloc(4);b.writeInt32LE(value);return b;};
const float=value=>{const b=Buffer.alloc(4);b.writeFloatLE(value);return b;};
const str=value=>{const n=Buffer.byteLength(value)+1,b=Buffer.alloc((n+3)&~3);b.write(value);return Buffer.concat([word(b.length),b]);};
function instruction(op,args=[],time=0,refs=0,count=args.length){
  const payload=Buffer.concat(args),b=Buffer.alloc(16+payload.length);
  b.writeInt32LE(time);b.writeUInt16LE(op,4);b.writeUInt16LE(b.length,6);b.writeUInt16LE(refs,8);b[10]=63;b[11]=count;payload.copy(b,16);return b;
}
function program(routines){
  const entries=Object.entries(routines),names=Buffer.concat(entries.map(([name])=>Buffer.from(name+'\0')));
  const start=(36+entries.length*4+names.length+3)&~3;let offset=start;
  const chunks=entries.map(([_,code])=>{const h=Buffer.alloc(16);h.write('ECLH');return Buffer.concat([h,...code,instruction(10)]);});
  const b=Buffer.alloc(start+chunks.reduce((n,c)=>n+c.length,0));b.write('SCPT');b.writeUInt16LE(1,4);b.writeUInt16LE(entries.length,16);names.copy(b,36+entries.length*4);
  for(let i=0;i<entries.length;i++){b.writeUInt32LE(offset,36+i*4);chunks[i].copy(b,offset);offset+=chunks[i].length;}return b;
}
async function ready(routines,character=0){const c=await createCore(),b=program(routines),p=c._malloc(b.length);c.HEAPU8.set(b,p);assert.equal(c._th12_load_ecl(p,b.length),1);c._free(p);c._th12_start(character,0,1,1234);return c;}
function sprites(c,kind){const n=c._th12_draw(),p=c._th12_draw_ptr(),stride=c._th12_draw_stride(),view=new DataView(c.HEAPU8.buffer);const out=[];for(let i=0;i<n;i++){const at=p+i*stride;if(view.getInt32(at+24,true)!==kind)continue;out.push({x:view.getFloat32(at,true)-224,y:view.getFloat32(at+4,true)-16,angle:view.getFloat32(at+8,true)-Math.fround(Math.PI/2),bank:view.getInt32(at+16,true),animation:view.getInt32(at+20,true),age:view.getInt32(at+32,true)});}return out;}
const hold=()=>instruction(83,[word(100000)]);
const spawn=(op,name,x,y,refs=0)=>instruction(op,[str(name),float(x),float(y),word(100),word(0),word(0)],0,refs,6);
const bind=animation=>instruction(259,[word(0),word(animation)]);
const near=(actual,expected,label)=>assert.ok(Math.abs(actual-expected)<.000002,`${label}: ${actual} != ${expected}`);
const snapshot=c=>{const p=c._th12_state(),size=c._th12_state_size();return JSON.parse(new TextDecoder().decode(c.HEAPU8.subarray(p,p+size)));};

test('ECL spell start applies native sevenfold life to real player shots and end clears it',async()=>{
  const c=await ready({main:[spawn(257,'Boss',0,390),hold()],Boss:[instruction(412,[word(0)]),instruction(411,[word(1500)]),instruction(414,[word(0),word(0),word(2400),str('End')]),instruction(422,[word(3),word(2400),word(0),str('')]),instruction(423,[],35),hold()],End:[instruction(423),hold()]});
  const bytes=fs.readFileSync(new URL('../site/assets/pl00a.sht',import.meta.url)),ptr=c._malloc(bytes.length);c.HEAPU8.set(bytes,ptr);assert.equal(c._th12_load_sht(0,ptr,bytes.length),1);c._free(ptr);
  const birth=snapshot(c).enemies.find(e=>e.boss);assert.equal(birth.lifeRaw,10500);assert.equal(birth.lifeFlags&1,1);
  for(let i=0;i<30;i++)c._th12_tick(1,0);
  const boss=snapshot(c).enemies.find(e=>e.boss);assert.ok(boss.lifeRaw<10500);assert.equal(boss.life,Math.trunc(boss.lifeRaw/7));assert.ok(boss.life>1000,'ordinary shots cannot remove the spell in one volley');
  for(let i=0;i<6;i++)c._th12_tick(0,0);assert.equal(snapshot(c).enemies.find(e=>e.boss).lifeFlags&1,0);
});

test('actual MSG19 selects each native Boss track at its scripted point and restart selects the road',async()=>{
  for(let stage=1;stage<=7;stage++){
    const c=await ready({main:[instruction(418,[word(0)]),hold()]});c._th12_select_stage(stage);
    const ecl=program({main:[instruction(418,[word(0)]),hold()]}),msg=fs.readFileSync(new URL(`../site/assets/st${String(stage).padStart(2,'0')}_00a.msg`,import.meta.url));
    for(const [bytes,method]of [[ecl,'_th12_load_ecl'],[msg,'_th12_load_msg']]){const ptr=c._malloc(bytes.length);c.HEAPU8.set(bytes,ptr);assert.equal(method==='_th12_load_msg'?c[method](0,ptr,bytes.length):c[method](ptr,bytes.length),1);c._free(ptr);}
    c._th12_start(0,0,stage===7?4:1,1);assert.equal(c._th12_music_track(),stage*2-1);let switched=false;
    for(let i=0;i<600;i++){c._th12_tick(0x200,0x80001);const s=snapshot(c),event=s.message.events?.some(e=>e[0]===19);if(event){assert.equal(c._th12_music_track(),stage*2);assert.ok(c._th12_music_serial()>0);switched=true;break;}assert.equal(c._th12_music_track(),stage*2-1);}
    assert.ok(switched,`stage${stage} reaches MSG19`);c._th12_start(0,0,stage===7?4:1,1);assert.equal(c._th12_music_track(),stage*2-1);assert.equal(c._th12_music_serial(),0);
  }
});
