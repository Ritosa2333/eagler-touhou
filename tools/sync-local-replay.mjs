import {readFileSync,writeFileSync} from 'node:fs';
const sources=['local-replay.mjs','local-replay-native.mjs'].map(name=>readFileSync(new URL('../browser/'+name,import.meta.url),'utf8').replaceAll('\r\n','\n').replace(/^import .*\n/gm,'').replace(/^export /gm,'')).join('\n');
const start='// BEGIN GENERATED LOCAL REPLAY',end='// END GENERATED LOCAL REPLAY';
const block=start+'\n// Generated from eagler-common/browser/local-replay*.mjs.\n'+sources+end;
const check=process.argv.includes('--check');
for(const path of process.argv.slice(2).filter(arg=>arg!=='--check')){
  const before=readFileSync(path,'utf8').replaceAll('\r\n','\n');let after;
  if(before.includes(start)){const a=before.indexOf(start),b=before.indexOf(end,a);if(b<0)throw Error('Missing local Replay end marker');after=before.slice(0,a)+block+before.slice(b+end.length);}
  else {const anchor=path.endsWith('.html')?'      const launch = async () => {':before.match(/^const (?:runtimeBuild|protocol|game)=.*$/m)?.[0];if(!anchor||!before.includes(anchor))throw Error('Missing shell insertion anchor');after=before.replace(anchor,block+'\n'+anchor);}
  if(check){if(before!==after)throw Error('Stale local Replay shell: '+path);console.log('CHECK '+path);}
  else if(before!==after){writeFileSync(path,after);console.log('SYNC '+path);}
}
