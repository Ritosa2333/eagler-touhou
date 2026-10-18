import {readFileSync,writeFileSync} from 'node:fs';
const base=new URL('../browser/',import.meta.url);
const svg=readFileSync(new URL('startup-wordmark.svg',base),'utf8').replaceAll('\r\n','\n');
const module=readFileSync(new URL('startup-branding.mjs',base),'utf8').replaceAll('\r\n','\n');
const generated='// Generated from eagler-common/browser/startup-branding.mjs and startup-wordmark.svg.\n'+
  'const STARTUP_WORDMARK='+JSON.stringify(svg)+';\n'+module;
const args=process.argv.slice(2),check=args.includes('--check');
for(const path of args.filter(a=>a!=='--check')){
  const before=readFileSync(path,'utf8').replaceAll('\r\n','\n');
  let after=generated;
  if(path.endsWith('.html')){
    const start='      // BEGIN GENERATED STARTUP BRANDING',end='      // END GENERATED STARTUP BRANDING';
    const block=start+'\n'+generated.replace('export async function','async function')+end;
    if(before.includes(start))after=before.slice(0,before.indexOf(start))+block+before.slice(before.indexOf(end)+end.length);
    else after=before.replace('      const launch = async () => {',block+'\n      const launch = async () => {');
  }
  if(check){if(before!==after)throw Error('Startup branding drift: '+path);}
  else writeFileSync(path,after);
  console.log((check?'PASS ':'SYNC ')+path);
}
