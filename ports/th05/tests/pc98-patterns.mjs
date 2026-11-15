// Optional local parity audit; the retail input traces are never checked in.
import {readFile,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';

export async function comparePatterns(frame,directory,{requireGeometry=false,requireAppearance=false,hpSweep=false}={}){
 let mismatches=0;
 for(const kind of ['enemy','midboss','boss','mai','yuki']){
  for(let stage=0;stage<7;stage++){
   if((stage===5&&kind==="midboss")||(["mai","yuki"].includes(kind)&&stage!==3))continue;
   if(hpSweep&&!((kind==='boss'&&stage>=3)||(kind==='midboss'&&[0,1,6].includes(stage))))continue;
   const input=JSON.parse(await readFile(resolve(directory,`pc98-${kind}-${stage}.json`),'utf8'));
   if(hpSweep&&!input.hpSweep)throw Error('Capture with Verify-OriginalPatterns.py --hp-sweep');
   if(requireGeometry&&input.frames.some(row=>!Array.isArray(row[6])))
    throw new Error('Capture with Verify-OriginalPatterns.py --geometry first');
   if(requireAppearance&&input.frames.some(row=>!Array.isArray(row[6])||row[6].some(shot=>shot.length!==9)))
    throw new Error('Capture sprite and motion flags with Verify-OriginalPatterns.py --geometry');
   const trace=await frame.evaluate(input=>{
    const m=Module;
    const mode=['enemy','boss','midboss','mai','yuki'].indexOf(input.kind);
    if(!m._th05_native_audit_start(input.stage,input.difficulty,input.rank,mode))throw Error('Audit start');
    m.HEAPU8.set(input.ring,m._th05_native_audit_ring());
    m._th05_native_audit_cursor(input.frames[0][1]);
    const addr=m._th05_native_audit_data();
    const geometryAddr=m._th05_native_audit_geometry();
    const trajectoryAddr=m._th05_native_audit_trajectory();
    return input.frames.map(([,cursor,,meta,,,geometry,trajectory],i)=>{
     // Visual effects and drops can consume RNG independently of attack logic.
     // Align each boss tick to the retail firing routine's random input.
     if(mode)m._th05_native_audit_cursor(cursor);
     if(!m._th05_native_audit_frame(input.start+i,mode,meta[7]??0,meta[8]??-1,meta[9]??-1,(input.hpSweep?1:0)|(meta[10]?2:0)|(meta[11]?4:0)))throw Error('Missing stage actor');
     const data=new Int32Array(m.HEAPU8.buffer,addr);
     const result=[Array.from(data.subarray(12,12+data[0]),speed=>speed/256).sort((a,b)=>a-b),data[1],...Array.from(data.subarray(2,10)),Array.from(new Int32Array(m.HEAPU8.buffer,m._th05_native_audit_special(),m._th05_native_audit_special_count()),v=>[v>>8,v&255]).sort((a,b)=>(a[0]-b[0])||(a[1]-b[1]))];
     if(geometry){
      const shots=new Int32Array(m.HEAPU8.buffer,geometryAddr,data[0]*9);
      result.push(Array.from({length:data[0]},(_,j)=>Array.from(shots.subarray(j*9,j*9+(geometry[0]?.length??6)),n=>n/256)));
     }
     if(trajectory?.length)result.push(Array.from(new Int32Array(m.HEAPU8.buffer,trajectoryAddr,trajectory.length)));
     return result;
    });
   },input);
   await writeFile(resolve(directory,`port-${kind}-${stage}.json`),JSON.stringify(trace));
   const deltas=[],specials=[],scrolls=[],geometryDeltas=[],appearanceDeltas=[];
   let positionFrames=0,positionDifferences=0;
   const phases=[],trajectories=[];
   for(let i=0;i<trace.length;i++){
    const [scroll,cursor,expected,meta,endCursor,special,geometry,trajectory]=input.frames[i],actual=trace[i];
    if(geometry){
     const order=(rows,fields)=>rows.map(x=>x.slice(0,fields).join(',')).sort();
     const expected=order(geometry,6),observed=order(actual[11],6);
     if(JSON.stringify(expected)!==JSON.stringify(observed))
      geometryDeltas.push({frame:input.start+i,phase:meta[0],expected:expected.slice(0,4),actual:observed.slice(0,4)});
     else if(geometry[0]?.length===9){
      const native=order(geometry,9),port=order(actual[11],9);
      if(JSON.stringify(native)!==JSON.stringify(port))
       appearanceDeltas.push({frame:input.start+i,phase:meta[0],expected:native.filter(row=>!port.includes(row)).slice(0,4),actual:port.filter(row=>!native.includes(row)).slice(0,4)});
     }
    }
    if(trajectory?.length&&trajectory.some((value,index)=>value!==actual[12]?.[index]))
     trajectories.push({frame:input.start+i,phase:meta[0],expected:trajectory,actual:actual[12]});
    if(hpSweep&&(meta[0]!==actual[2]||meta[1]!==actual[3]||meta[3]!==actual[5]))
     phases.push({frame:input.start+i,native:meta,port:actual.slice(2,9)});
    if(kind!=='enemy'&&meta[0]===actual[2]&&meta[1]===actual[3]){
     positionFrames++;
     if(meta[5]!==actual[7]||meta[6]!==actual[8])positionDifferences++;
    }
    if(special&&JSON.stringify(actual?.[10])!==JSON.stringify(special))specials.push({frame:input.start+i,expected:special,actual:actual?.[10],phase:meta[0]});
    if(kind==="enemy"&&actual?.[9]!==scroll)scrolls.push({frame:input.start+i,expected:scroll,actual:actual?.[9]});
    if(!actual||JSON.stringify(actual[0])!==JSON.stringify(expected)){
     deltas.push({frame:input.start+i,expected,actual:actual?.[0],native:meta,port:actual?.slice(2),endCursor,portCursor:actual?.[1]});
    }
   }
   mismatches+=trajectories.length+phases.length+deltas.length+specials.length+scrolls.length+(requireGeometry?geometryDeltas.length+positionDifferences:0)+(requireAppearance?appearanceDeltas.length:0);
   console.log(JSON.stringify({kind,stage,frames:trace.length,...(hpSweep?{phaseDifferences:phases.length,phaseFirst:phases.slice(0,2),trajectoryDifferences:trajectories.length,trajectoryFirst:trajectories.slice(0,1)}:{}),changed:deltas.length,geometry:geometryDeltas.length,geometryFirst:geometryDeltas.slice(0,2),appearance:appearanceDeltas.length,appearanceFirst:appearanceDeltas.slice(0,2),positionFrames,positionDifferences,special:specials.length,specialFirst:specials.slice(0,4),scroll:scrolls.length,scrollFirst:scrolls.slice(0,3),first:deltas.slice(0,2).map(d=>({...d,expected:d.expected.slice(0,16),actual:d.actual?.slice(0,16),n:[d.expected.length,d.actual?.length]}))}));
  }
 }
 if(mismatches)throw new Error('PC-98 pattern audit found '+mismatches+' frame differences');
}
