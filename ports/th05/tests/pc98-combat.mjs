import {readFile} from 'node:fs/promises';
import assert from 'node:assert/strict';

export async function compareCombat(frame, file) {
 const capture=JSON.parse(await readFile(file,'utf8'));
 const result=await frame.evaluate(({damage,bombs,specials})=>{
  const m=Module;
  const damageErrors=[], bombErrors=[], specialErrors=[];
  let cases=0, frames=0, specialFrames=0;
  for(const {stage,character,cases:rows} of damage){
   if(!m._th05_native_combat_start(stage,character,0))throw Error('Combat start');
   for(const [boss,bombing,age,frame,x,y,shots,expected] of rows){
    const packed=shots.reduce((value,shot,i)=>value+(shot<<(8*i)),0);
    const actual=m._th05_native_combat_damage(boss,bombing,age,frame,x,y,packed);
    if(actual!==expected)damageErrors.push({stage,character,boss,bombing,age,frame,x,y,shots,expected,actual});
    cases++;
   }
  }
  for(const {character,phase,ring,cursor,frames:rows} of bombs){
   if(!m._th05_native_combat_start(0,character,phase))throw Error('Bomb start');
   m.HEAPU8.set(ring,m._th05_native_audit_ring());
   m._th05_native_audit_cursor(cursor);
   for(const [frame,[key,...expected]] of rows.entries()){
    const address=m._th05_native_combat_bomb_step(frame,key);
    const actual=Array.from(new Int32Array(m.HEAPU8.buffer,address,expected.length));
    if(expected.some((value,i)=>value!==actual[i])&&bombErrors.length<16)
     bombErrors.push({character,phase,frame,expected,actual});
    frames++;
   }
  }
  for(const {kind,angle,clear,frames:rows} of specials){
   if(!m._th05_native_combat_start(kind===0?4:5,0,0))throw Error('Special projectile start');
   m._th05_native_combat_special_start(kind,angle);
   for(const [frame,expected] of rows.entries()){
    const remaining=frame>=clear?Math.max(0,20-frame+clear):0;
    const address=m._th05_native_combat_special_step(kind,remaining);
    const actual=Array.from(new Int32Array(m.HEAPU8.buffer,address,expected.length));
    if(expected.some((value,i)=>value!==actual[i])&&specialErrors.length<12)
     specialErrors.push({kind,angle,clear,frame,expected,actual});
    specialFrames++;
   }
  }
  return {cases,frames,specialFrames,damageErrors,bombErrors,specialErrors};
 },capture);
 assert.deepEqual(result.damageErrors,[]);
 assert.deepEqual(result.bombErrors,[]);
 assert.deepEqual(result.specialErrors,[]);
 console.log(`PC-98 combat: ${result.cases} damage cases, ${result.frames} Bomb frames, ${result.specialFrames} special projectile frames`);
}

