import {writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';

// Emscripten reads @file with Python shlex, including on Windows. Use unique
// content identities because compilation can launch several processes at once.
export function compilerArguments(args,{compiler,output,platform=process.platform}) {
  if(platform!=='win32'||args.reduce((n,v)=>n+v.length+3,compiler.length)<20000)return args;
  const contents=args.map(v=>'"'+v.replaceAll('\\','\\\\').replaceAll('"','\\"')+'"').join('\n')+'\n';
  const file=resolve(output,`arguments-${createHash('sha256').update(contents).digest('hex').slice(0,20)}.rsp.utf-8`);
  writeFileSync(file,contents);
  return ['@'+file];
}
