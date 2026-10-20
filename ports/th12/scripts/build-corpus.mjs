// Manifest preparation only: reads explicitly supplied Replay/resource files.
// This does not execute either game or generate expected completion/trace data.
import fs from 'node:fs';
import path from 'node:path';
import {parseReplay} from '../src/replay.mjs';
import {stageResources} from '../src/stage-loader.mjs';
import {argumentsFor,sha256,titleRoot,jsonFile} from '../tools/replay-verifier/config.mjs';
import {demoOrder} from '../tools/replay-verifier/manifest.mjs';
const {values,positional}=argumentsFor(process.argv.slice(2));
const allowed=new Set(['assets','demos','replays-source','replays','corpus','original-exe','completions']);
if(positional.length||Object.keys(values).some(k=>!allowed.has(k)))throw Error('Unknown corpus option');
if(!values['original-exe'])throw Error('Provide --original-exe=<owned TH12 1.00b EXE>; no original process is executed');
const assets=path.resolve(values.assets||process.env.TH12_ASSET_DIR||path.join(titleRoot,'site/assets'));
const demos=path.resolve(values.demos||assets),destination=path.resolve(values.replays||process.env.TH12_REPLAY_DIR||path.join(titleRoot,'site/replays'));
const output=path.resolve(values.corpus||process.env.TH12_REPLAY_CORPUS||path.join(titleRoot,'tools/replay-verifier/corpus.json'));
const original=fs.readFileSync(path.resolve(values['original-exe'])),exeSha=sha256(original);
if(exeSha!=='99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417')throw Error('Original executable is not pinned TH12 1.00b');
const completions=values.completions?jsonFile(path.resolve(values.completions)):{};
const inputs=demoOrder.map(name=>({name,file:path.join(demos,name),provenance:'built-in-demo'}));
// No historical workspace scan or deleted-replay fallback: user replays enter
// this tool only from a directory the maintainer explicitly supplies now.
if(values['replays-source']){
  const directory=path.resolve(values['replays-source']);
  for(const name of fs.readdirSync(directory).filter(n=>/^[a-z0-9_.-]+\.rpy$/i.test(n)&&!demoOrder.includes(n)).sort())inputs.push({name,file:path.join(directory,name),provenance:'user-supplied'});
}
fs.mkdirSync(destination,{recursive:true});const fixtures=[];
for(const input of inputs){
  const bytes=fs.readFileSync(input.file),replay=parseReplay(bytes);if(replay.extension)throw Error('Retail corpus rejects extended/assisted replay: '+input.name);
  const target=path.join(destination,input.name);if(fs.existsSync(target)&&sha256(fs.readFileSync(target))!==sha256(bytes))throw Error('Refusing to replace a different Replay fixture: '+target);
  if(path.resolve(input.file)!==target&&!fs.existsSync(target))fs.copyFileSync(input.file,target,fs.constants.COPYFILE_EXCL);
  fixtures.push({id:input.name,file:input.name,provenance:input.provenance,sha256:sha256(bytes),bytes:bytes.length,name:replay.name,character:replay.character,shot:replay.shot,difficulty:replay.difficulty,
    stages:replay.stages.map(({number,seed,frames,initial})=>({number,seed,frames,initial})),
    completion:completions[input.name]??{status:'unobserved'}});
}
const optional={};for(let i=1;i<=7;i++){const name=`stgenm${String(i).padStart(2,'0')}m`;if(fs.existsSync(path.join(assets,name+'.anm')))optional[name]=true;}
const required=[...new Set(fixtures.flatMap(f=>f.stages.flatMap(s=>stageResources(s.number,optional).map(r=>r.file))))].sort();
const resources=required.map(file=>{const bytes=fs.readFileSync(path.join(assets,file));return {file,bytes:bytes.length,sha256:sha256(bytes)};});
const daily={standardLoadout:{character:1,shot:1},requiredDifficulties:[3,4],fixtures:fixtures.filter(f=>f.character===1&&f.shot===1&&[3,4].includes(f.difficulty)).map(f=>f.id)};
const corpus={schema:'th12-replay-corpus/2',game:'th12',version:'1.00b',original:{executableSha256:exeSha,resources},fixtures,quick:demoOrder,daily};
fs.mkdirSync(path.dirname(output),{recursive:true});fs.writeFileSync(output,JSON.stringify(corpus,null,2)+'\n');
// The legacy local preview consumes the same fixture metadata; no accepted
// golden index or completion observation is manufactured by this operation.
fs.writeFileSync(path.join(destination,'index.json'),JSON.stringify(corpus,null,2)+'\n');
console.log(JSON.stringify({fixtures:fixtures.length,quick:corpus.quick,daily:daily.fixtures,corpus:output,completionEvidenceGenerated:false}));
