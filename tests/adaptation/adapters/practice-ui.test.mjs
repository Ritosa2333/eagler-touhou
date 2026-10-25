import test from 'node:test';
import assert from 'node:assert/strict';
import {decodePracticeUiSnapshot,decodePracticeApplicationSnapshot} from './practice-ui.mjs';
const bits=value=>{const buffer=new ArrayBuffer(4),view=new DataView(buffer);view.setFloat32(0,value,true);return view.getUint32(0,true);};
function ui(){
  const words=Array(128).fill(0);words[0]=words[1]=1;words[2]=words[3]=10;words[4]=words[7]=words[8]=words[9]=1;
  words.splice(12,8,3,bits(8),bits(8),bits(150),bits(240),100,1,123);
  words[37]=1;words.splice(40,8,112,1,bits(16),bits(50),bits(140),bits(70),10,99);return{available:true,words};
}
test('native UI reader retains actual submitted panel geometry, hotkey identity and input generation',()=>{
  const raw=ui(),before=structuredClone(raw),value=decodePracticeUiSnapshot(raw);assert.deepEqual(raw,before);
  assert.equal(value.panels.cheats.active,true);assert.deepEqual(value.panels.cheats.rect,{x:8,y:8,width:150,height:240});
  assert.deepEqual(value.items,[{key:'F1',value:true,rect:{x:16,y:50,width:124,height:20},generation:10,id:99}]);
  assert.equal(value.panels.advanced.exists,false);assert.equal(value.mobileEnabled,true);
});
test('tagged keyboard extension exposes actual armed state and preserves legacy prefix decoding',()=>{
  const raw=ui();assert.equal(decodePracticeUiSnapshot(raw).keyboard,undefined);
  raw.words[110]=0x50554b31;raw.words[111]=1;raw.words[104]=raw.words[106]=1;raw.words[112+2]=2**26;
  const value=decodePracticeUiSnapshot(raw);assert.equal(value.practiceInput.armed,true);assert.equal(value.practiceInput.confirmHeld,true);
  assert.equal(value.keyboard.down[2],2**26);assert.deepEqual(value.keyboard.pressed,Array(8).fill(0));
  raw.words[111]=2;assert.throws(()=>decodePracticeUiSnapshot(raw));
});
for(const [name,mutate] of Object.entries({
  'uninitialized UI':x=>x.words[1]=0,'unsupported ABI':x=>x.words[0]=2,'invalid bool':x=>x.words[4]=2,
  'overflowed widget count':x=>x.words[37]=9,'unknown key':x=>x.words[40]=120,'missing native item ID':x=>x.words[47]=0,
  'invalid native rectangle':x=>x.words[44]=bits(10),'nonfinite coordinate':x=>x.words[42]=bits(NaN),
  'active absent panel':x=>x.words[12]=2,
}))test(`native UI reader rejects ${name}`,()=>{const raw=ui();mutate(raw);assert.throws(()=>decodePracticeUiSnapshot(raw));});
function application(){const words=Array(80).fill(0);words[0]=words[1]=words[70]=1;words.splice(71,9,2,990,0xa8d8,7,60000,1,-999,0,1);return{available:true,words};}
test('practice application reader uses actual timeline clock and instruction rather than configured frame',()=>{
  assert.deepEqual(decodePracticeApplicationSnapshot(application()),{schema:'eagler-touhou/practice-application-observation/1',timelineCount:2,timelineFrame:990,
    instructionSelected:true,instructionOffset:0xa8d8,opcode:7,programBytes:60000,practiceInstructions:true,previousFrame:-999,fraction:0});
});
test('an unwarped initialization retains its actual not-yet-selected timeline instruction',()=>{
  const raw=application();raw.words[73]=raw.words[74]=-1;raw.words[79]=0;const decoded=decodePracticeApplicationSnapshot(raw);
  assert.equal(decoded.instructionSelected,false);assert.equal(decoded.instructionOffset,null);assert.equal(decoded.opcode,null);
});
for(const [name,mutate] of Object.entries({'old snapshot lacks timeline proof':x=>x.words[70]=0,
  'instruction beyond program':x=>x.words[73]=60000,'native instruction unavailable':x=>x.words[79]=0}))
  test(`practice application reader rejects ${name}`,()=>{const raw=application();mutate(raw);assert.throws(()=>decodePracticeApplicationSnapshot(raw));});
