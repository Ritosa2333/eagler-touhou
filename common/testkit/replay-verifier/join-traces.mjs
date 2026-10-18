import {appendFileSync,writeFileSync,mkdirSync} from 'node:fs';
import {dirname} from 'node:path';
import {compareTraces} from './compare.mjs';
import {readTraceFile} from './golden-files.mjs';

// Join completed, independently captured stage providers without dropping,
// shifting, resynchronizing or synthesizing any gameplay tick.
export async function joinCompletedTraces(paths, output) {
  mkdirSync(dirname(output),{recursive:true});writeFileSync(output,'');
  let identity=null,sequence=0,ticks=0,segments=0;const evidence=[];
  const write=r=>appendFileSync(output,JSON.stringify(r)+'\n');
  for(const path of paths){
    const check=await compareTraces(readTraceFile(path),readTraceFile(path));
    if(check.status!=='PASS')throw Error('Cannot join incomplete source: '+path);
    let active=null;
    for await(const r of readTraceFile(path)){
      if(r.type==='run-start'){
        if(!identity){identity=r.comparisonIdentity;write(r);}
        else if(JSON.stringify(identity)!==JSON.stringify(r.comparisonIdentity))throw Error('Mixed identities in stage join');
      }else if(r.type==='run-end'){evidence.push(r.evidence);}
      else{
        if(r.type==='segment-start')active=`${r.route}#${segments++}`;
        write({...r,sequence:sequence++,segmentId:active});if(r.type==='tick')ticks++;
      }
    }
  }
  if(!segments||!ticks)throw Error('Empty stage join');
  write({type:'run-end',sequence,reason:'all-recorded-stages-cleared',complete:true,
    summary:{segments,ticks},evidence:{stageProviders:evidence}});
}
