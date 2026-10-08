import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import createCore from '../site/runtime/core.mjs';

const native=JSON.parse(fs.readFileSync(new URL('fixtures/full-bullet-clear-native.json',import.meta.url)));
const word=v=>{const b=Buffer.alloc(4);b.writeInt32LE(v);return b;};
const float=v=>{const b=Buffer.alloc(4);b.writeFloatLE(v);return b;};
function instruction(op,args=[],time=0){
 const payload=Buffer.concat(args),b=Buffer.alloc(16+payload.length);
 b.writeInt32LE(time);b.writeUInt16LE(op,4);b.writeUInt16LE(b.length,6);b[10]=63;b[11]=args.length;payload.copy(b,16);return b;
}
function program(commands){
 const header=Buffer.alloc(16);header.write('ECLH');
 const body=Buffer.concat([header,...commands,instruction(83,[word(100000)]),instruction(10)]);
 const b=Buffer.alloc(48+body.length);b.write('SCPT');b.writeUInt16LE(1,4);b.writeUInt16LE(1,16);b.writeUInt32LE(48,36);b.write('main\0',40);body.copy(b,48);return b;
}
function load(c,method,bytes,...leading){const p=c._malloc(bytes.length);c.HEAPU8.set(bytes,p);assert.equal(c[method](...leading,p,bytes.length),1);c._free(p);}
function state(c){const p=c._th12_state(),size=c._th12_state_size();return JSON.parse(new TextDecoder().decode(c.HEAPU8.subarray(p,p+size)));}
async function ready(card=0,inactive=false){
 const c=await createCore();
 for(const [bank,file] of [[0,'bullet'],[11,'ascii']])load(c,'_th12_load_anm',fs.readFileSync(new URL('../local/retail/'+file+'.anm',import.meta.url)),bank);
 const commands=[];
 for(const row of native.initialBullets.filter(r=>r.phase===1||r.phase===2)){
  const type=row.hitbox[0]===14?26:row.hitbox[0]===10?17:0;
  commands.push(instruction(500,[word(0)]),instruction(502,[word(0),word(type),word(row.color&3)]),
   instruction(525,[word(0),...row.position.map(float)]),instruction(505,[word(0),float(0),float(0)]),instruction(501,[word(0)]));
 }
 if(card)commands.push(instruction(422,[word(card),word(2400),word(0),word(0)],1));
 if(inactive)commands.push(instruction(423,[],1));
 commands.push(instruction(510,[],1));
 load(c,'_th12_load_ecl',program(commands));c._th12_start(0,0,1,0x1234);
 c._th12_tick(0,0); // Native input0 clears the main birth guard.
 return c;
}
test('production ECL510 retires matching bullets immediately and converts physical slots in native order',async()=>{
 assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 const original=native.cases.find(c=>c.convert&&!c.card),c=await ready();
 assert.equal(state(c).bullets.filter(b=>!b.friendly).length,8);
 c._th12_tick(0,0);const after=state(c),items=after.items.filter(i=>i[2]===9);
 assert.equal(items.length,original.pointBirths.length);
 for(let i=0;i<items.length;i++){
  assert.deepEqual(items[i].slice(4,6).map(Math.fround),original.pointBirths[i].position.slice(0,2));
  assert.equal(items[i][10],(i&3)-1,'same Item22 visit consumes the original ordinal delay');
 }
 const remaining=after.bullets.filter(b=>!b.friendly);
 assert.equal(remaining.length,1);assert.deepEqual(remaining[0].position,[260,128]);
 assert.equal(after.animations.nodes.filter(n=>n.alive&&n.bank===0&&n.script===96).length,original.effects.length);
});
test('production full clear preserves the native active96..99 spell gate',async()=>{
 for(const [card,inactive] of [[95,false],[96,false],[99,false],[96,true]]){
  const original=native.cases.find(r=>r.convert&&r.card===card&&r.activeSpell===!inactive),c=await ready(card,inactive);
  c._th12_tick(0,0);const after=state(c);
  assert.equal(after.items.filter(i=>i[2]===9).length,original.pointBirths.length,'spell '+card+' active '+!inactive);
  assert.equal(after.animations.nodes.filter(n=>n.alive&&n.bank===0&&n.script===96).length,original.effects.length);
 }
});
