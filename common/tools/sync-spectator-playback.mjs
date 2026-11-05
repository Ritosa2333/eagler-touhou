import {readFileSync,writeFileSync} from 'node:fs';
const source=readFileSync(new URL('../browser/spectator-playback.mjs',import.meta.url),'utf8').replaceAll('\r\n','\n').replace(/^export /gm,'');
const start='// BEGIN GENERATED SPECTATOR PLAYBACK',end='// END GENERATED SPECTATOR PLAYBACK';
const generated=start+'\n// Generated from eagler-common/browser/spectator-playback.mjs.\n'+source+end;
const args=process.argv.slice(2),check=args.includes('--check');
for(const path of args.filter(value=>value!=='--check')) {
  const before=readFileSync(path,'utf8').replaceAll('\r\n','\n');let after;
  if(before.includes(start)) {
    const from=before.indexOf(start),to=before.indexOf(end,from);
    if(to<0)throw Error('Missing spectator playback end marker: '+path);
    after=before.slice(0,from)+generated+before.slice(to+end.length);
  } else {
    const anchor=path.endsWith('.html')?'      const launch = async () => {':before.match(/^const (?:runtimeBuild|protocol|game)=.*$/m)?.[0];
    if(!anchor)throw Error('Missing spectator playback insertion anchor: '+path);
    after=before.replace(anchor,generated+'\n'+anchor);
  }
  if(check){if(after!==before)throw Error('Spectator playback bridge drift: '+path);}
  else writeFileSync(path,after);
  console.log((check?'PASS ':'SYNC ')+path);
}
