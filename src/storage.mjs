const ROOT='/savesth12/';
export function allowedPath(value){
  if(typeof value!=='string'||!value.startsWith(ROOT))throw Error('存档路径不允许');
  const name=value.slice(ROOT.length);
  if(!/^(?:scoreth12\.dat|th12\.cfg|replay\/th12_[a-z0-9_-]+\.rpy)$/i.test(name))throw Error('存档文件不允许');return value;
}
export async function openStore(){
  const db=await new Promise((resolve,reject)=>{let blocked=false;const req=indexedDB.open('th12-eagler-preview-files',1);req.onupgradeneeded=()=>req.result.createObjectStore('files');req.onsuccess=()=>{if(blocked)req.result.close();else resolve(req.result);};req.onerror=()=>reject(req.error);req.onblocked=()=>{blocked=true;reject(Error('浏览器存储被另一个游戏窗口占用，请关闭旧窗口后重新打开'));};});
  db.onversionchange=()=>db.close();
  const run=(mode,op)=>new Promise((resolve,reject)=>{const tx=db.transaction('files',mode),store=tx.objectStore('files');let result;const request=op(store);request.onsuccess=()=>{result=request.result;};tx.oncomplete=()=>resolve(result);tx.onerror=()=>reject(tx.error);tx.onabort=()=>reject(tx.error);});
  return {list:()=>run('readonly',s=>s.getAllKeys()),read:p=>run('readonly',s=>s.get(allowedPath(p))),
    write:(p,b)=>{if(!(b instanceof Uint8Array)||b.length>64*1024*1024)throw Error('存档大小无效');return run('readwrite',s=>s.put(b,allowedPath(p)));},
    remove:p=>run('readwrite',s=>s.delete(allowedPath(p))),sync:async()=>{await run('readonly',s=>s.count());}};
}
