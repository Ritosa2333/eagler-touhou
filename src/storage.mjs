const ROOT='/savesth12/';
const DATABASE='/savesth12mp',STORE='FILE_DATA',VERSION=21,MAX_BYTES=64*1024*1024;
const LEGACY_DATABASE='th12-multiplayer-preview-files';
const MIGRATION=ROOT+'.eagler-preview-v1-migrated';
const FILE_MODE=0o100666,DIRECTORY_MODE=0o40777;
const fileName=/^(?:scoreth12\.dat|th12\.cfg|web-profile\.json|replay\/(?:[^/]+\/)?th12_[a-z0-9_-]+\.rpy)$/i;
function isAllowedName(name){
  // Match the Launcher's relative path and single-folder limits.
  if(name.length>240||/[\\\u0000-\u001f\u007f]/.test(name)||!fileName.test(name))return false;
  const parts=name.split('/');
  if(parts.some(part=>!part||part==='.'||part==='..'))return false;
  if(parts.length===3){
    const folder=parts[1];
    if(folder.length>128||/[<>:"|?*]/.test(folder)||/[. ]$/.test(folder))return false;
  }
  return true;
}
const isAllowed=value=>typeof value==='string'&&value.startsWith(ROOT)&&isAllowedName(value.slice(ROOT.length));
export function allowedPath(value){
  if(typeof value!=='string'||!value.startsWith(ROOT))throw Error('存档路径不允许');
  const name=value.slice(ROOT.length);
  if(!isAllowedName(name))throw Error('存档文件不允许');return value;
}

function transaction(db,storeName,mode,setup){
  return new Promise((resolve,reject)=>{
    let tx,result,failure;
    try{tx=db.transaction(storeName,mode);}catch(error){reject(error);return;}
    const fail=error=>{failure??=error;try{tx.abort();}catch{reject(failure);}};
    const watch=(request,callback)=>{
      request.onsuccess=()=>{try{callback(request.result);}catch(error){fail(error);}};
      return request;
    };
    tx.oncomplete=()=>failure?reject(failure):resolve(result);
    tx.onabort=()=>reject(failure??tx.error??Error('浏览器存档事务已取消'));
    // Request failures retain IndexedDB's default transaction abort behavior.
    tx.onerror=()=>{failure??=tx.error;};
    try{setup(tx.objectStore(storeName),watch,value=>{result=value;});}catch(error){fail(error);}
  });
}

function validateSchema(db,storeName,indexed){
  if(db.version!==(indexed?VERSION:1)||db.objectStoreNames.length!==1||!db.objectStoreNames.contains(storeName))throw Error('无法识别浏览器存档库版本或结构');
  const store=db.transaction(storeName,'readonly').objectStore(storeName);
  if(store.keyPath!==null||store.autoIncrement||store.indexNames.length!==(indexed?1:0))throw Error('无法识别浏览器存档库键结构');
  if(indexed){
    if(!store.indexNames.contains('timestamp'))throw Error('浏览器存档库缺少 IDBFS 时间索引');
    const index=store.index('timestamp');
    if(index.keyPath!=='timestamp'||index.unique||index.multiEntry)throw Error('浏览器存档库时间索引不兼容');
  }
}

function openDatabase(legacy=false,shared=false){
  return new Promise((resolve,reject)=>{
    let settled=false,missing=false,failure;
    const request=indexedDB.open(shared?'/savesth12':legacy?LEGACY_DATABASE:DATABASE,legacy?1:VERSION);
    request.onupgradeneeded=event=>{
      // Probing an absent preview database must never commit an empty old DB.
      if(legacy||shared){missing=event.oldVersion===0;if(!missing)failure=Error('旧版存档库结构不兼容');request.transaction.abort();return;}
      if(settled||event.oldVersion!==0){failure=Error('不允许升级未知的浏览器存档库');request.transaction.abort();return;}
      const store=request.result.createObjectStore(STORE);
      store.createIndex('timestamp','timestamp',{unique:false});
    };
    request.onerror=()=>{
      if(settled)return;settled=true;
      if((legacy||shared)&&missing)resolve(null);else reject(failure??request.error??Error('无法打开浏览器存档库'));
    };
    request.onblocked=()=>{if(!settled){settled=true;reject(Error('浏览器存储被另一个游戏窗口占用，请关闭旧窗口后重新打开'));}};
    request.onsuccess=()=>{
      const db=request.result;if(settled){db.close();return;}
      try{validateSchema(db,legacy?'files':STORE,!legacy);}catch(error){settled=true;db.close();reject(error);return;}
      settled=true;db.onversionchange=()=>db.close();resolve(db);
    };
  });
}

const timestampValid=value=>value instanceof Date?Number.isFinite(value.getTime()):typeof value==='number'&&Number.isFinite(value);
const directoryValid=entry=>entry&&Number.isInteger(entry.mode)&&(entry.mode&0xf000)===0x4000&&timestampValid(entry.timestamp);
const directoryEntry=()=>({timestamp:new Date(),mode:DIRECTORY_MODE});
function fileBytes(entry){
  if(!entry||!Number.isInteger(entry.mode)||(entry.mode&0xf000)!==0x8000||!timestampValid(entry.timestamp)||
    !(entry.contents instanceof Uint8Array||entry.contents instanceof Int8Array)||entry.contents.byteLength>MAX_BYTES)throw Error('浏览器存档文件记录无效');
  return new Uint8Array(entry.contents.buffer,entry.contents.byteOffset,entry.contents.byteLength);
}
function ensureDirectory(store,watch,path){
  watch(store.get(path),entry=>{
    if(entry===undefined)store.put(directoryEntry(),path);
    else if(!directoryValid(entry))throw Error('存档父目录记录无效：'+path);
  });
}
function ensureParents(store,watch,path){
  let directory=ROOT.slice(0,-1);
  ensureDirectory(store,watch,directory);
  for(const part of path.slice(ROOT.length).split('/').slice(0,-1)){
    directory+='/'+part;
    ensureDirectory(store,watch,directory);
  }
}

async function legacyFiles(){
  const legacy=await openDatabase(true);
  if(!legacy)return [];
  try{return await transaction(legacy,'files','readonly',(store,watch,done)=>{
    const files=[];
    watch(store.openCursor(),cursor=>{
      if(!cursor){done(files);return;}
      if(isAllowed(cursor.key)){
        const bytes=cursor.value;
        if(!(bytes instanceof Uint8Array)||bytes.byteLength>MAX_BYTES)throw Error('旧版存档文件记录无效：'+cursor.key);
        files.push([cursor.key,bytes]);
      }
      cursor.continue();
    });
  });}finally{legacy.close();}
}

async function sharedMultiplayerReplays(){
  const db=await openDatabase(false,true);
  if(!db)return [];
  try{return await transaction(db,STORE,'readonly',(store,watch,done)=>{
    const files=[];
    watch(store.openCursor(),cursor=>{
      if(!cursor){done(files);return;}
      if(typeof cursor.key==='string'&&/^\/savesth12\/replay\/(?:[^/]+\/)?th12_mp_[a-z0-9_-]+\.rpy$/i.test(cursor.key))
        files.push([cursor.key,fileBytes(cursor.value).slice()]);
      cursor.continue();
    });
  });}finally{db.close();}
}

async function migrate(db){
  const marker=await transaction(db,STORE,'readonly',(store,watch,done)=>watch(store.get(MIGRATION),done));
  if(marker!==undefined&&!directoryValid(marker))throw Error('存档迁移标记无效');
  const files=marker===undefined?[...await legacyFiles(),...await sharedMultiplayerReplays()]:[];
  await transaction(db,STORE,'readwrite',(store,watch)=>{
    ensureDirectory(store,watch,ROOT.slice(0,-1));ensureDirectory(store,watch,ROOT+'replay');
    // FILE_DATA readwrite transactions serialize across windows. Recheck the
    // marker here, after reading the old DB, so a losing opener cannot restore
    // a legacy Replay deleted after the winning migration committed.
    watch(store.get(MIGRATION),current=>{
      if(current!==undefined){if(!directoryValid(current))throw Error('存档迁移标记无效');return;}
      for(const [path,bytes]of files){
        ensureParents(store,watch,path);
        watch(store.getKey(path),key=>{
          if(key===undefined)store.put({timestamp:new Date(),mode:FILE_MODE,contents:bytes},path);
        });
      }
      // Marker, missing files and parent directories commit or abort together.
      // A hidden directory is a normal IDBFS entry, not a schema/version fork.
      store.put(directoryEntry(),MIGRATION);
    });
  });
}

export async function openStore(){
  const db=await openDatabase();
  try{await migrate(db);}catch(error){db.close();throw error;}
  const run=(mode,setup)=>transaction(db,STORE,mode,setup);
  return {
    list:()=>run('readonly',(store,watch,done)=>{
      const paths=[];
      watch(store.openCursor(),cursor=>{
        if(!cursor){done(paths);return;}
        const path=cursor.key,entry=cursor.value;
        if(isAllowed(path)&&path.toLowerCase()!==ROOT+'web-profile.json'&&(entry?.mode&0xf000)===0x8000){fileBytes(entry);paths.push(path);}
        cursor.continue();
      });
    }),
    read:p=>run('readonly',(store,watch,done)=>watch(store.get(allowedPath(p)),entry=>done(entry===undefined?undefined:fileBytes(entry).slice()))),
    write:(p,b)=>{
      if(!(b instanceof Uint8Array)||b.byteLength>MAX_BYTES)throw Error('存档大小无效');
      return run('readwrite',(store,watch,done)=>{
        const path=allowedPath(p);
        ensureParents(store,watch,path);
        watch(store.put({timestamp:new Date(),mode:FILE_MODE,contents:b},path),done);
      });
    },
    remove:p=>run('readwrite',(store,watch,done)=>watch(store.delete(allowedPath(p)),done)),
    sync:async()=>{await run('readonly',(store,watch,done)=>watch(store.count(),done));},
  };
}
