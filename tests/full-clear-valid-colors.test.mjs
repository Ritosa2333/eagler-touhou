import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import crypto from 'node:crypto';
import createCore from '../site/runtime/core.mjs';

const native=JSON.parse(fs.readFileSync(new URL('fixtures/full-clear-valid-colors-native.json',import.meta.url)));
const word=v=>{const b=Buffer.alloc(4);b.writeInt32LE(v);return b;};
const float=v=>{const b=Buffer.alloc(4);b.writeFloatLE(v);return b;};
function instruction(op,args=[],time=0){
 const payload=Buffer.concat(args),b=Buffer.alloc(16+payload.length);
 b.writeInt32LE(time);b.writeUInt16LE(op,4);b.writeUInt16LE(b.length,6);b[10]=63;b[11]=args.length;payload.copy(b,16);return b;
}
function program(commands){
 const header=Buffer.alloc(16);header.write('ECLH');const body=Buffer.concat([header,...commands,instruction(83,[word(100000)]),instruction(10)]);
 const b=Buffer.alloc(48+body.length);b.write('SCPT');b.writeUInt16LE(1,4);b.writeUInt16LE(1,16);b.writeUInt32LE(48,36);b.write('main\0',40);body.copy(b,48);return b;
}
function load(c,method,bytes,...leading){const p=c._malloc(bytes.length);c.HEAPU8.set(bytes,p);assert.equal(c[method](...leading,p,bytes.length),1);c._free(p);}
function state(c){const p=c._th12_state(),size=c._th12_state_size();return JSON.parse(new TextDecoder().decode(c.HEAPU8.subarray(p,p+size)));}
async function ready(row){
 const c=await createCore();
 for(const [bank,file] of [[0,'bullet'],[11,'ascii']]){
  const raw=fs.readFileSync(new URL('../local/retail/'+file+'.anm',import.meta.url));
  assert.equal(crypto.createHash('sha256').update(raw).digest('hex'),file==='bullet'?native.bulletAnmSha256:native.asciiAnmSha256);
  load(c,'_th12_load_anm',raw,bank);
 }
 const commands=[instruction(500,[word(0)]),instruction(502,[word(0),word(row.type),word(row.color)]),
  instruction(525,[word(0),...row.origin.map(float)]),instruction(505,[word(0),float(row.speed),float(0)]),
  instruction(507,[word(0),word(row.mode)]),instruction(508,[word(0),word(row.fireSound),word(row.transformSound)]),
  instruction(501,[word(0)]),instruction(510)];
 load(c,'_th12_load_ecl',program(commands));c._th12_start(0,0,1,row.seed);return c;
}
function compareEffect(snapshot,expected,label){
 const effects=snapshot.animations.nodes.filter(n=>n.alive&&n.bank===0&&n.script===96);
 assert.equal(effects.length,expected.effects.length,label+' effect count');
 for(let i=0;i<effects.length;i++){
  assert.equal(effects[i].render[0],expected.effects[i].primaryColor,label+' native ARGB');
  assert.equal(effects[i].render[4],expected.effects[i].sprite,label+' native effect sprite');
  assert.equal(effects[i].render[5],expected.effects[i].layer,label+' native effect layer');
  assert.equal(effects[i].clock,expected.effects[i].clock,label+' native ANM clock');
 }
 assert.deepEqual({seed:snapshot.rng.seed,calls:snapshot.rng.calls},expected.rng,label+' main RNG');
}
test('valid16/8/4-color bullet families preserve actual native full-clear tint and RNG through Primary27',async()=>{
 assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 assert.deepEqual(native.cases.map(c=>c.bulletBirth.spriteWidth),[8,32,64]);
 for(const row of native.cases){
  const c=await ready(row),birth=state(c),label='type'+row.type+'/color'+row.color;
  compareEffect(birth,row.afterClear,label+' clear');
  const stars=birth.items.filter(i=>i[2]===9);assert.equal(stars.length,row.afterClear.pointBirths.length);
  assert.deepEqual(stars[0].slice(4,6).map(Math.fround),row.afterClear.pointBirths[0].position.slice(0,2));
  c._th12_tick(0,0);compareEffect(state(c),row.afterPrimary27,label+' Primary27');
 }
});
