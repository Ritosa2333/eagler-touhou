import fs from 'node:fs';
import path from 'node:path';
import crypto from 'node:crypto';
import {fileURLToPath} from 'node:url';
export const titleRoot=path.resolve(fileURLToPath(new URL('../..',import.meta.url)));
export const sha256=bytes=>crypto.createHash('sha256').update(bytes).digest('hex');
export const jsonFile=file=>JSON.parse(fs.readFileSync(file,'utf8'));
export function argumentsFor(argv){
  const values={},positional=[];
  for(const argument of argv){
    if(!argument.startsWith('--')){positional.push(argument);continue;}
    const equal=argument.indexOf('=');if(equal<3)throw Error('Use --name=value: '+argument);
    const key=argument.slice(2,equal);if(key in values)throw Error('Duplicate option: '+key);
    values[key]=argument.slice(equal+1);
  }
  return {values,positional};
}
export function verifierPaths(values={}){
  const allowed=new Set(['assets','runtime','replays','corpus','goldens','output','index','proposal','subsystem-index','reviewed-original','replace']);
  for(const key of Object.keys(values))if(!allowed.has(key))throw Error('Unknown verifier option: '+key);
  const resolve=(key,environment,fallback)=>path.resolve(values[key]||process.env[environment]||path.join(titleRoot,fallback));
  return {
    assets:resolve('assets','TH12_ASSET_DIR','site/assets'),
    runtime:resolve('runtime','TH12_DIAGNOSTIC_RUNTIME','artifacts/diagnostic-runtime/core.mjs'),
    replays:resolve('replays','TH12_REPLAY_DIR','site/replays'),
    corpus:resolve('corpus','TH12_REPLAY_CORPUS','tools/replay-verifier/corpus.json'),
    goldens:resolve('goldens','TH12_GAMEPLAY_GOLDENS','tools/replay-verifier/gameplay-golden-index.json'),
    output:resolve('output','TH12_VERIFIER_OUTPUT','artifacts/replay-verifier'),
    subsystemIndex:resolve('subsystem-index','TH12_SUBSYSTEM_GOLDENS','tools/replay-verifier/subsystem-golden.json')
  };
}
export function prerequisite(file,label){if(!fs.existsSync(file))throw Error('Missing prerequisite: '+label+' ('+file+')');return file;}
export function containedFile(directory,name){
  if(typeof name!=='string'||!name||name.includes('\\')||path.isAbsolute(name)||name.split('/').some(s=>!s||s==='.'||s==='..'))throw Error('Unsafe manifest path');
  const result=path.resolve(directory,name);if(!result.startsWith(path.resolve(directory)+path.sep))throw Error('Manifest path escapes root');return result;
}
export function assertHash(value,label){if(!/^[a-f0-9]{64}$/.test(value??''))throw Error('Missing or invalid SHA256: '+label);return value;}
