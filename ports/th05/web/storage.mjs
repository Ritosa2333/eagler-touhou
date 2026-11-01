import {inspectReplay} from './replay.mjs';
export const SAVE_ROOT='/savesth05';
export function userPath(value){
 if(typeof value!=='string'||value.length>240||/[\\\x00-\x1f]/.test(value))throw Error('Invalid user path');
 const path=value.startsWith(SAVE_ROOT+'/')?value.slice(SAVE_ROOT.length+1):value;
 if(!/^(?:APS\.csv|score\.dat|game\.cfg|thprac-uth05\.ini|practice-progress\.dat|replay\/(?:[A-Za-z0-9_-]+\/)?[A-Za-z0-9_.-]+\.rpy)$/.test(path)||path.split('/').some(p=>!p||p==='.'||p==='..'))throw Error('Invalid user path');
 return path;
}
// Reject truncated streams, invalid dimensions and unbounded stage allocations
// before native replay loading; the C++ reader checks the run's mod identity.
export function validateUserFile(path,bytes){
 path=userPath(path);if(!(bytes instanceof Uint8Array)||!bytes.length||bytes.length>32*1024*1024)throw Error('Invalid user file bytes');
 if(path.endsWith('.rpy')){
  inspectReplay(bytes);
 }else if(path==='game.cfg'&&(bytes.length<52||bytes.length>4096))throw Error('Invalid TH05 configuration');
 else if(path==='score.dat'&&bytes.length!==1500)throw Error('Invalid TH05 score file');
 else if(path==='thprac-uth05.ini'&&(bytes.length>65536||bytes.some(n=>n===0)))throw Error('Invalid practice configuration');
 return bytes;
}
export function createStore(module){
 const fs=module.FS;let pending=Promise.resolve();
 const sync=populate=>{const task=pending.then(()=>new Promise((resolve,reject)=>fs.syncfs(populate,error=>error?reject(error):resolve())));pending=task.catch(()=>{});return task;};
 const physical=path=>SAVE_ROOT+'/'+userPath(path);
 return {
  async initialize(){fs.mkdirTree(SAVE_ROOT);fs.mount(module.IDBFS,{},SAVE_ROOT);await sync(true);fs.mkdirTree(SAVE_ROOT+'/replay');},
  sync:()=>sync(false),
  read(path){return fs.readFile(physical(path));},
  write(path,bytes){validateUserFile(path,bytes);const full=physical(path);fs.mkdirTree(full.slice(0,full.lastIndexOf('/')));fs.writeFile(full,bytes);},
  remove(path){fs.unlink(physical(path));},
  list(){const result=[];function walk(prefix,depth){for(const name of fs.readdir(SAVE_ROOT+prefix)){if(name==='.'||name==='..')continue;const path=prefix+'/'+name,stat=fs.lstat(SAVE_ROOT+path);if(fs.isDir(stat.mode)&&depth<2)walk(path,depth+1);else if(fs.isFile(stat.mode)){try{userPath(path.slice(1));result.push({path:path.slice(1),size:stat.size,modifiedAt:stat.mtime.getTime()});}catch{}}}}walk('',0);return result;},
 };
}
