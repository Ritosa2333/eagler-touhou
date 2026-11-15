import {readFile} from 'node:fs/promises';

export async function comparePlayerShots(frame, path) {
 const input=JSON.parse(await readFile(path,'utf8'));
 const result=await frame.evaluate(({ring,cases})=>{
  const m=Module;
  m.HEAPU8.set(ring,m._th05_native_audit_ring());
  let frames=0;
  for(const fixture of cases){
   if(!m._th05_native_shot_start(fixture.char,fixture.power,+fixture.homing))
    throw Error('Could not start TH05 shot fixture');
   for(let tick=0;tick<fixture.frames.length;tick++){
    // Align the incoming RNG cursor independently of unrelated game events.
    m._th05_native_audit_cursor(fixture.cursors[tick]);
    const addr=m._th05_native_shot_step(tick);
    const data=new Int32Array(m.HEAPU8.buffer,addr);
    const actual=Array.from({length:data[0]},(_,i)=>
     Array.from(data.subarray(1+i*7,8+i*7),(value,field)=>field<4?value/256:value));
    const expected=fixture.frames[tick];
    if(JSON.stringify(actual)!==JSON.stringify(expected)){
     const context={character:fixture.char,power:fixture.power,homing:fixture.homing,tick};
     throw Error('TH05 player shots differ: '+JSON.stringify({context,expected,actual}));
    }
    frames++;
   }
  }
  return {cases:cases.length,frames};
 },input);
 console.log(JSON.stringify(result));
}
