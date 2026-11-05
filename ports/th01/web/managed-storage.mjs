const root = '/savesth01/';
export function userPath(value) {
  if (typeof value !== 'string') throw Error('用户文件路径无效');
  const path = value.replace(/^\/savesth01(?:mp)?\//,'');
  if (!['scores.dat'].includes(path) && !(/^replay\/[A-Za-z0-9_(). -]+\.th01r$/i.test(path) && !path.includes('..') && path.length <= 207)) throw Error('不允许访问此文件');
  return path;
}
export async function openUserFiles({multiplayer=false}={}) {
  // Match the main Replay reader's IDBFS namespace and record format. The
  // Runtime remains its sole writer; normal and multiplayer files are separate.
  const databaseRoot=root.slice(0,-1)+(multiplayer?'mp':''),prefix=databaseRoot+'/';
  const db = await new Promise((resolve, reject) => {
    const request = indexedDB.open(databaseRoot, 21);
    request.onupgradeneeded = () => {
      const store=request.result.createObjectStore('FILE_DATA');store.createIndex('timestamp','timestamp',{unique:false});
      for(const path of [databaseRoot,prefix+'replay'])store.put({mode:16877,timestamp:new Date()},path);
    };
    request.onerror = () => reject(request.error); request.onblocked = () => reject(Error('请关闭旧的游戏文件管理页面'));
    request.onsuccess = () => {request.result.onversionchange=()=>request.result.close();resolve(request.result);};
  });
  const memory = new Map();
  await new Promise((resolve, reject) => {
    const tx = db.transaction('FILE_DATA'), request = tx.objectStore('FILE_DATA').openCursor();
    request.onsuccess = () => { const item = request.result; if (item) { if (String(item.key).startsWith(prefix)&&(item.value?.mode&0xf000)===0x8000) { try { memory.set(userPath(String(item.key).slice(prefix.length)), new Uint8Array(item.value.contents)); } catch {} } item.continue(); } };
    tx.oncomplete = resolve; tx.onerror = tx.onabort = () => reject(tx.error);
  });
  const mutate = (path, value) => new Promise((resolve, reject) => {
    const tx = db.transaction('FILE_DATA', 'readwrite'), store = tx.objectStore('FILE_DATA');
    if (value === null) store.delete(prefix + path); else store.put({mode:33188,timestamp:new Date(),contents:value}, prefix + path);
    tx.oncomplete = () => { if (value === null) memory.delete(path); else memory.set(path, value); resolve(); };
    tx.onerror = tx.onabort = () => reject(tx.error);
  });
  return {memory, async write(name, value) { const path=userPath(name), bytes=new Uint8Array(value); if (!bytes.length || bytes.length > 64 * 1024 * 1024) throw Error('用户文件大小无效'); await mutate(path, bytes); },
    async remove(name) { await mutate(userPath(name), null); }, close() { db.close(); }};
}
