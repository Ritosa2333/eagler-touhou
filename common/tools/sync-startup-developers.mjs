import {readFileSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
const base=new URL('../browser/',import.meta.url);
const svg=readFileSync(new URL('startup-developers.svg',base),'utf8');
const module=readFileSync(new URL('startup-developers.mjs',base),'utf8');
const header=readFileSync(new URL('StartupBranding.hpp',base),'utf8');
const args=process.argv.slice(2),check=args.includes('--check');
for(const root of args.filter(x=>x!=='--check')){
  const game=root.match(/th\d+/)?.[0];if(!game)throw Error('Expected title repository');
  for(const [file,content] of [
    [`${game}_web/sdl-runtime/startup-branding.mjs`,'// Generated from eagler-common/browser/startup-developers.{mjs,svg}.\nconst STARTUP_DEVELOPERS='+JSON.stringify(svg)+';\n'+module],
    ['portable/sdl/StartupBranding.hpp','// Generated from eagler-common/browser/StartupBranding.hpp.\n'+header],
  ]){
    const path=resolve(root,file);
    if(check){if(readFileSync(path,'utf8')!==content)throw Error('Startup credit drift: '+path);}
    else writeFileSync(path,content);
  }
  console.log((check?'PASS ':'SYNC ')+root);
}
