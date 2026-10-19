import {execFileSync} from 'node:child_process';
import {existsSync, lstatSync} from 'node:fs';
import {isAbsolute, relative, resolve} from 'node:path';

function repositoryPath(repository, path) {
  const name=relative(repository,path).replaceAll('\\','/');
  if(!name||isAbsolute(name)||name==='..'||name.startsWith('../'))
    throw new Error(`Runtime build dependency must stay inside the workspace: ${path}`);
  return name;
}

function relativeSegments(value,label) {
  if(typeof value!=='string'||!value||/[\\:\0\t\r\n]/.test(value)||value.startsWith('/')||
     value.split('/').some(part=>!part||part==='.'||part==='..'))
    throw new Error(`${label} must be a safe owner-relative path`);
  return value.split('/');
}

function assertOrdinaryDirectory(repository,path,label) {
  const name=repositoryPath(repository,path);
  let current=repository;
  for(const part of name.split('/')) {
    current=resolve(current,part);
    const stat=lstatSync(current,{throwIfNoEntry:false});
    if(!stat?.isDirectory()||stat.isSymbolicLink())
      throw new Error(`${label} must use existing ordinary directories: ${current}`);
  }
}

function runGit(args,cwd,label) {
  try {
    return execFileSync('git',args,{cwd,encoding:'utf8',windowsHide:true,maxBuffer:32*1024*1024}).trim();
  } catch(error) {
    const detail=String(error.stderr??error.message).trim();
    throw new Error(`${label}: ${detail}`);
  }
}

function readGitlink(owner,path) {
  const output=runGit(['ls-files','--stage','-z','--',path],owner,`Cannot read Git index for ${path}`);
  const entries=output.split('\0').filter(Boolean).map(record=>{
    const tab=record.indexOf('\t');
    if(tab<0)return null;
    const [mode,commit,stage]=record.slice(0,tab).split(' ');
    return {mode,commit,stage,path:record.slice(tab+1)};
  }).filter(entry=>entry?.path===path);
  if(entries.length!==1||entries[0].mode!=='160000'||entries[0].stage!=='0'||
     !/^[a-f0-9]{40,64}$/.test(entries[0].commit))
    throw new Error(`Required submodule is not a single stage-0 gitlink: ${resolve(owner,path)}`);
  return entries[0].commit;
}

/** Resolve required submodules from the recipe and prove each checkout matches its parent gitlink. */
export function resolveRuntimeBuildSubmodules({source,repository,declarations=[]}) {
  repository=resolve(repository);
  source=resolve(source);
  assertOrdinaryDirectory(repository,source,'Runtime source root');
  if(!Array.isArray(declarations))throw new Error('requiredSubmodules must be an array');
  const result=[],seen=new Set();
  function visit(owner,declaration) {
    if(!declaration||typeof declaration!=='object'||Array.isArray(declaration))
      throw new Error('requiredSubmodules entries must be objects');
    const path=relativeSegments(declaration.path,'requiredSubmodules.path').join('/');
    const children=declaration.requiredSubmodules??[];
    if(!Array.isArray(children))throw new Error(`requiredSubmodules children must be an array: ${path}`);
    const absolute=resolve(owner,...path.split('/'));
    repositoryPath(repository,absolute);
    const commit=readGitlink(owner,path);
    if(!lstatSync(absolute,{throwIfNoEntry:false})?.isDirectory())
      throw new Error(`Required submodule checkout is missing: ${absolute} (expected ${commit})`);
    assertOrdinaryDirectory(repository,absolute,'Required submodule path');
    if(!existsSync(resolve(absolute,'.git')))
      throw new Error(`Required submodule is not initialized: ${absolute} (expected ${commit})`);
    const head=runGit(['rev-parse','--verify','HEAD'],absolute,`Required submodule is not a readable Git worktree: ${absolute}`);
    if(head!==commit)throw new Error(`Required submodule commit mismatch at ${absolute}: expected ${commit}, found ${head}`);
    runGit(['ls-files','--stage','-z','--','.'],absolute,`Required submodule index is unreadable: ${absolute}`);
    const name=repositoryPath(repository,absolute);
    if(seen.has(name))throw new Error(`Duplicate required submodule declaration: ${name}`);
    seen.add(name);
    result.push({path:name,commit});
    for(const child of children)visit(absolute,child);
  }
  for(const declaration of declarations)visit(source,declaration);
  return result.sort((a,b)=>a.path.localeCompare(b.path,'en'));
}

/** Return ordered, non-recursive Git commands for the recipe's declared dependencies. */
export function runtimeSubmoduleInitCommands({source,declarations=[]}) {
  source=resolve(source);
  if(!Array.isArray(declarations))throw new Error('requiredSubmodules must be an array');
  const commands=[];
  function visit(owner,children) {
    if(!children.length)return;
    const paths=children.map(child=>relativeSegments(child?.path,'requiredSubmodules.path').join('/'));
    commands.push({cwd:owner,args:['submodule','update','--init','--',...paths]});
    for(let index=0;index<children.length;index++) {
      const nested=children[index].requiredSubmodules??[];
      if(!Array.isArray(nested))throw new Error(`requiredSubmodules children must be an array: ${paths[index]}`);
      visit(resolve(owner,...paths[index].split('/')),nested);
    }
  }
  visit(source,declarations);
  return commands;
}
