import {readFile,writeFile,mkdir} from 'node:fs/promises';
import {existsSync} from 'node:fs';
import {resolve,dirname} from 'node:path';
import {createHash} from 'node:crypto';
import {gzipSync} from 'node:zlib';
import {compareTraces} from './compare.mjs';
import {assetPath,readTraceFile,verifyGoldenFiles} from './golden-files.mjs';

export function options(argv=process.argv.slice(2)) {
  const args={};for(let i=0;i<argv.length;i+=2){if(!argv[i].startsWith('--')||!argv[i+1])throw Error('Expected --option value');args[argv[i].slice(2)]=argv[i+1];}return args;
}
export async function compareFileSuite({corpus,captureRoot,originalRoot=captureRoot,goldenRoot,lane='all',report}) {
  const ids=lane==='all'?Object.values(corpus.suites).flat():corpus.suites[lane];
  if(!ids)throw Error('Unknown suite: '+lane);
  const golden=goldenRoot?await verifyGoldenFiles(goldenRoot,{game:corpus.game,suites:corpus.suites}):null;
  const results=[];
  for(const id of ids){
    const original=golden?assetPath(goldenRoot,golden.assets[id]):resolve(originalRoot,`${id}.original.jsonl`);
    const candidate=resolve(captureRoot,`${id}.candidate.jsonl`);
    try {
      const fixture=corpus.cases.find(f=>f.id===id);if(!fixture?.replaySha256)throw Error('Corpus is missing a fixture hash: '+id);
      for(const path of [original,candidate]){
        const records=readTraceFile(path),first=await records.next();await records.return();
        if(first.value?.comparisonIdentity?.game!==corpus.game||first.value?.comparisonIdentity?.replaySha256!==fixture.replaySha256)throw Error('Trace does not belong to corpus fixture: '+id);
      }
      results.push({id,...await compareTraces(readTraceFile(original),readTraceFile(candidate))});
    }
    catch(error){results.push({id,status:'ERROR',reason:String(error)});}
  }
  const result={schema:'eagler/replay-file-suite/v1',game:corpus.game,lane,
    status:results.every(r=>r.status==='PASS')?'PASS':'FAIL',results,
    scope:'Authoritative fixed-tick selected-state comparison in the title-owned diagnostic provider. Browser presentation, audio and real devices are separate.'};
  if(report){await mkdir(dirname(resolve(report)),{recursive:true});await writeFile(report,JSON.stringify(result,null,2)+'\n');}
  return result;
}
export async function acceptFileSuite({corpus,captureRoot,goldenRoot}) {
  if(existsSync(resolve(goldenRoot,'manifest.json')))throw Error('Refusing to replace an existing golden set');
  const result=await compareFileSuite({corpus,captureRoot});
  if(result.status!=='PASS')throw Error('Cannot accept a failing/incomplete oracle suite');
  const assets={};let identity;
  for(const {id,comparedTicks} of result.results){
    const path=resolve(captureRoot,`${id}.original.jsonl`),bytes=await readFile(path),records=bytes.toString('utf8').trim().split('\n').map(JSON.parse);
    const header=records[0],end=records.at(-1);
    if(!header.provenance.provider.includes('retail-')||!end.complete)throw Error('Expected a complete original-provider trace');
    identity??={executableSha256:header.comparisonIdentity.executableSha256,resourceSha256:header.comparisonIdentity.resourceSha256};
    if(identity.executableSha256!==header.comparisonIdentity.executableSha256||identity.resourceSha256!==header.comparisonIdentity.resourceSha256)throw Error('Mixed original identities');
    const compressed=gzipSync(bytes),sha256=createHash('sha256').update(compressed).digest('hex'),name=`${sha256}.jsonl.gz`;
    await mkdir(goldenRoot,{recursive:true});await writeFile(resolve(goldenRoot,name),compressed);
    assets[id]={path:name,sha256,ticks:comparedTicks,replaySha256:header.comparisonIdentity.replaySha256,
      stateSchema:header.comparisonIdentity.stateSchema,provenance:header.provenance,completion:end.evidence};
  }
  const manifest={schema:'eagler/replay-golden-set/v1',game:corpus.game,identity,suites:corpus.suites,assets};
  await writeFile(resolve(goldenRoot,'manifest.json'),JSON.stringify(manifest,null,2)+'\n');
  await verifyGoldenFiles(goldenRoot,{game:corpus.game,suites:corpus.suites});return manifest;
}
