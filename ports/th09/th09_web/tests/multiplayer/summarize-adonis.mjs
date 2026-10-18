// Preserve a small, reviewable index of positive AND negative local evidence.
// Raw measurements remain in artifacts; this script never manufactures PASS.
import {createHash} from 'node:crypto';
import {readFileSync,readdirSync,statSync,writeFileSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
import {resolve} from 'node:path';

const root=fileURLToPath(new URL('../../',import.meta.url));
const directory=resolve(root,'artifacts/multiplayer-tests');
const names=readdirSync(directory).filter(name=>/^adonis-[\w-]+(?:report|failure)\.json$/.test(name)||name==='adonis-report.json').sort();
if(!names.length||names.length>100)throw Error('Unexpected evidence file count');
const records=names.map(name=>{
  // The 12k-frame raw trace is ~3.6 MB; keep an explicit finite read limit.
  const file=resolve(directory,name);if(statSync(file).size>8*1024*1024)throw Error('Oversized evidence '+name);
  const bytes=readFileSync(file),report=JSON.parse(bytes),r=report.result||{};
  return {
    file:'th09_web/artifacts/multiplayer-tests/'+name,
    sha256:createHash('sha256').update(bytes).digest('hex'),
    passed:report.passed===true,
    ...(report.correctnessPassed!==undefined?{correctnessPassed:report.correctnessPassed}:{}),
    ...(report.error?{error:String(report.error).split('\n')[0]}:{}),
    ...(report.parameters?{parameters:report.parameters}:{}),
    ...(report.wasm||report.wasmSha256?{wasm:report.wasm||report.wasmSha256}:{}),
    ...(report.harnessSha256?{harnessSha256:report.harnessSha256}:{}),
    ...(report.scope?{scope:report.scope}:{}),
    result:Object.fromEntries(['frames','summary','adonis','hashes','comparableFinalHashes',
      'framebuffersEqual','replayBytes','phases','pauseCalls','routes','elapsedMs',
      'sustainedCadencePassed','measured','costs'].filter(key=>r[key]!==undefined).map(key=>[key,r[key]])),
  };
});
const builds=['sdl-adonis','sdl-adonis-release'].map(directory=>{
  const build=JSON.parse(readFileSync(resolve(root,'artifacts',directory,'build.json')));
  return {directory:'th09_web/artifacts/'+directory,kind:build.kind,sha256:build.sha256,bytes:build.bytes};
});
const output=resolve(root,'../docs/multiplayer/adonis-evidence.json');
writeFileSync(output,JSON.stringify({
  schema:'th09/adonis-evidence/1',date:'2026-10-02',base:'5b9305c',common:'5669eff',
  scope:'Local desktop evidence only. A summary is not a phone/WAN guarantee; raw file hashes identify each attempt, including failures. See ADONIS-EXPERIMENT.md.',
  builds,records,
},null,2)+'\n');
console.log(JSON.stringify({output,reports:records.length,passed:records.filter(r=>r.passed).length}));
