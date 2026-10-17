import {execFileSync} from 'node:child_process';
import {existsSync,readFileSync,realpathSync} from 'node:fs';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';

// A monorepo branch identifies the integrated checkout. Its declared component
// path, rather than that branch's name, owns ordinary versus MP game source.
export function runtimeSourceOwner({source,game,multiplayer=false}) {
  source=realpathSync(source);
  const git=(...args)=>execFileSync('git',['-c',`safe.directory=${source}`,'-C',source,...args],{encoding:'utf8',windowsHide:true}).trim();
  const root=git('rev-parse','--show-toplevel');
  const branch=git('branch','--show-current');
  const owner={repository:source,branch,commit:git('rev-parse','HEAD'),dirty:!!git('status','--porcelain','--','.'),layout:'standalone'};
  const layoutPath=resolve(root,'launcher/config/workspace.json');
  if(existsSync(layoutPath)) {
    const layout=JSON.parse(readFileSync(layoutPath,'utf8'));
    const builds=JSON.parse(readFileSync(resolve(root,'launcher/config/runtime-builds.json'),'utf8'));
    const variant=multiplayer?'multiplayer':'normal';
    const entry=builds.games[game],definition=entry?.variants[variant];
    if(!definition)throw Error(`Unknown Runtime source: ${game}/${variant}`);
    const component=definition.workspaceRepository??entry.workspaceRepository;
    const expected=realpathSync(resolve(root,layout.repositories[component]));
    if(source!==expected)throw Error(`${game}/${variant} requires declared source ${expected}; received ${source}`);
    return {...owner,layout:'monorepo',component:layout.repositories[component],common:resolve(root,layout.repositories.common)};
  }
  if(['th08','th10','th11','th15'].includes(game)&&branch!==(multiplayer?'eagler-mp':'eagler'))throw Error(`${game} requires ${multiplayer?'eagler-mp':'eagler'}; received ${branch||'detached HEAD'}`);
  return owner;
}
if(process.argv[1]&&import.meta.url===pathToFileURL(resolve(process.argv[1])).href) {
  const options=Object.fromEntries(process.argv.slice(2).map(v=>v.replace(/^--/,'').split('=')));
  console.log(JSON.stringify(runtimeSourceOwner({source:options.source,game:options.game,multiplayer:options.variant==='multiplayer'})));
}
