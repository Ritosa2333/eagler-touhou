// Optional local parity audit; the retail input traces are never checked in.
import {readFile,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';

export async function comparePatterns(frame,directory,{requireGeometry=false}={}){
 let mismatches=0;
 for(const kind of ['enemy','midboss','boss','mai','yuki']){
  for(let stage=0;stage<7;stage++){
   if((stage===5&&kind==="midboss")||(["mai","yuki"].includes(kind)&&stage!==3))continue;
   const input=JSON.parse(await readFile(resolve(directory,`pc98-${kind}-${stage}.json`),'utf8'));
   if(requireGeometry&&input.frames.some(row=>!Array.isArray(row[6])))
    throw new Error('Capture with Verify-OriginalPatterns.py --geometry first');
   const trace=await frame.evaluate(input=>{
    const m=Module;
    const mode=['enemy','boss','midboss','mai','yuki'].indexOf(input.kind);
    if(!m._th05_native_audit_start(input.stage,input.difficulty,input.rank,mode))throw Error('Audit start');
    m.HEAPU8.set(input.ring,m._th05_native_audit_ring());
    m._th05_native_audit_cursor(input.frames[0][1]);
    const addr=m._th05_native_audit_data();
    const geometryAddr=m._th05_native_audit_geometry();
    return input.frames.map(([,cursor,,meta,,,geometry],i)=>{
     // Visual effects and drops can consume RNG independently of attack logic.
     // Align each boss tick to the retail firing routine's random input.
     if(mode)m._th05_native_audit_cursor(cursor);
     if(!m._th05_native_audit_frame(input.start+i,mode,meta[7]??0))throw Error('Missing stage actor');
     const data=new Int32Array(m.HEAPU8.buffer,addr);
     const result=[Array.from(data.subarray(12,12+data[0]),speed=>speed/256).sort((a,b)=>a-b),data[1],...Array.from(data.subarray(2,10)),Array.from(new Int32Array(m.HEAPU8.buffer,m._th05_native_audit_special(),m._th05_native_audit_special_count()),v=>[v>>8,v&255]).sort((a,b)=>(a[0]-b[0])||(a[1]-b[1]))];
     if(geometry){
      const shots=new Int32Array(m.HEAPU8.buffer,geometryAddr,data[0]*6);
      result.push(Array.from({length:data[0]},(_,j)=>Array.from(shots.subarray(j*6,j*6+6),n=>n/256)));
     }
     return result;
    });
   },input);
   await writeFile(resolve(directory,`port-${kind}-${stage}.json`),JSON.stringify(trace));
   const deltas=[],specials=[],scrolls=[],geometryDeltas=[];
   for(let i=0;i<trace.length;i++){
    const [scroll,cursor,expected,meta,endCursor,special,geometry]=input.frames[i],actual=trace[i];
    if(geometry){
     const expected=geometry.map(x=>x.join(',')).sort();
     const observed=actual?.[11]?.map(x=>x.join(',')).sort();
     if(JSON.stringify(expected)!==JSON.stringify(observed))
      geometryDeltas.push({frame:input.start+i,phase:meta[0],expected:expected.slice(0,4),actual:observed?.slice(0,4)});
    }
    if(special&&JSON.stringify(actual?.[10])!==JSON.stringify(special))specials.push({frame:input.start+i,expected:special,actual:actual?.[10],phase:meta[0]});
    if(kind==="enemy"&&actual?.[9]!==scroll)scrolls.push({frame:input.start+i,expected:scroll,actual:actual?.[9]});
    if(!actual||JSON.stringify(actual[0])!==JSON.stringify(expected)){
     deltas.push({frame:input.start+i,expected,actual:actual?.[0],native:meta,port:actual?.slice(2),endCursor,portCursor:actual?.[1]});
    }
   }
   mismatches+=deltas.length+specials.length+scrolls.length+(requireGeometry?geometryDeltas.length:0);
   console.log(JSON.stringify({kind,stage,frames:trace.length,changed:deltas.length,geometry:geometryDeltas.length,geometryFirst:geometryDeltas.slice(0,2),special:specials.length,specialFirst:specials.slice(0,4),scroll:scrolls.length,scrollFirst:scrolls.slice(0,3),first:deltas.slice(0,2).map(d=>({...d,expected:d.expected.slice(0,16),actual:d.actual?.slice(0,16),n:[d.expected.length,d.actual?.length]}))}));
  }
 }
 if(mismatches)throw new Error('PC-98 pattern audit found '+mismatches+' frame differences');
}
