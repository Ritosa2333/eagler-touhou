import {spawn} from 'node:child_process';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {readTraceFile} from './golden-files.mjs';
import {validateAdapter} from './contracts.mjs';
import {validateCorpus} from './corpus.mjs';

export function createFileAdapter({repository,game,inspectReplay,originalScript,candidateScript}) {
  const corpus=validateCorpus(JSON.parse(readFileSync(resolve(repository,'tools/replay-verifier/corpus.json'))));
  if(corpus.game!==game)throw Error('Adapter corpus identity mismatch');
  const open=(side,script,spec,{signal}={})=>{
    const id=spec?.caseId;if(!corpus.cases.some(f=>f.id===id))throw Error('Unknown adapter case');
    signal?.throwIfAborted();
    const output=resolve(spec.outputDirectory||resolve(repository,'artifacts/replay-verifier/adapter',side));
    const child=spawn(process.execPath,[resolve(repository,script),'--case',id,'--output',output],{cwd:repository,windowsHide:true,stdio:['ignore','ignore','pipe']});
    let log='',settled=false;
    child.stderr.on('data',bytes=>{log=(log+bytes).slice(-12000);});
    const cancel=()=>{if(!settled)child.kill();};signal?.addEventListener('abort',cancel,{once:true});
    const done=new Promise((accept,reject)=>{child.on('error',reject);child.on('exit',code=>{settled=true;signal?.removeEventListener('abort',cancel);code===0?accept():reject(Error('Provider failed: '+code+' '+log));});});
    // A provider may fail before its consumer requests records. Keep that
    // failure handled while preserving rejection for the consumer.
    done.catch(()=>{});
    return {records:(async function*(){await done;yield* readTraceFile(resolve(output,`${id}.${side}.jsonl`));})(),cancel,
      async close(){cancel();try{await done;}catch{}}};
  };
  const adapter={describe:()=>({adapterApiVersion:1,game,adapterVersion:'file-provider/v1',features:{original:true,candidate:true,quick:true,daily:true}}),
    inspectReplay,async openCandidate(spec,options){return open('candidate',candidateScript,spec,options);},
    async openOriginal(spec,options){return open('original',originalScript,spec,options);}};
  validateAdapter(adapter);return adapter;
}
