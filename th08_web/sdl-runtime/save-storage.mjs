const LANGUAGES=Object.freeze(['jp','chs']);

function savePathPattern(game){
 return game==='th08'
  ? /^(?:score\.dat|th08\.cfg|replay\/th8_(?:\d{2}|ud[a-z0-9]{4})\.rpyx?)$/
  : /^(?:scoreth10c?\.dat|th10\.cfg|replay\/th10_(?:\d{2}|ud[a-z0-9]{4})\.rpyx?)$/;
}

export async function initializeSaveStorage({game,runtimeVariant='normal',setCompiledVariant,filesystem,idbfs,sync,beforeMount}){
 if(game!=='th08'&&game!=='th10')throw Error('Unsupported save-storage game: '+game);
 if(runtimeVariant!=='normal'&&runtimeVariant!=='multiplayer')throw Error('Unsupported runtimeVariant: '+runtimeVariant);
 const multiplayerRuntime=runtimeVariant==='multiplayer';
 if(typeof setCompiledVariant!=='function'||setCompiledVariant(multiplayerRuntime?1:0)!==1)
  throw Error('Runtime variant mismatch: URL requests '+runtimeVariant+' but the compiled '+game.toUpperCase()+' runtime has a different storage profile');

 const normalNamespace='/saves'+game,multiplayerNamespace=normalNamespace+'-multiplayer';
 const namespace=multiplayerRuntime?multiplayerNamespace:normalNamespace;
 const languageRoot=game==='th10';
 const profile={
  game,variant:runtimeVariant,multiplayerRuntime,namespace,
  root(language){
   if(!languageRoot)return namespace;
   if(!LANGUAGES.includes(language))throw Error('Invalid save language: '+language);
   return namespace+'/'+language;
  },
  relativeSave(value){
   if(typeof value!=='string'||value.length>200)throw Error('Invalid save path');
   let path=value.replaceAll('\\','/').toLowerCase();
   const selectedPrefix=namespace.toLowerCase()+'/';
   if(path.startsWith(selectedPrefix)){
    path=path.slice(selectedPrefix.length);
    if(languageRoot)path=path.replace(/^(?:jp|chs)\//,'');
   }else if([normalNamespace,multiplayerNamespace].some(other=>path.startsWith(other.toLowerCase()+'/'))){
    throw Error('Save path belongs to a different runtime variant');
   }else path=path.replace(/^\//,'');
   if(!savePathPattern(game).test(path))throw Error('Invalid save path: '+path);
   return path;
  },
 };

 beforeMount?.();
 filesystem.mkdirTree(namespace);
 filesystem.mount(idbfs,{},namespace);
 await sync(true);
 return profile;
}

function openDatabase(indexedDB,name){
 return new Promise((resolve,reject)=>{
  const request=indexedDB.open(name);
  request.onerror=()=>reject(request.error);
  request.onsuccess=()=>resolve(request.result);
 });
}

function readLegacyEntries(database){
 return new Promise((resolve,reject)=>{
  const transaction=database.transaction('files','readonly'),rows=[];
  transaction.objectStore('files').openCursor().onsuccess=event=>{
   const cursor=event.target.result;
   if(cursor){rows.push([cursor.key,cursor.value]);cursor.continue();}
  };
  transaction.oncomplete=()=>resolve(rows);
  transaction.onerror=()=>reject(transaction.error);
 });
}

export async function migrateLegacySaves(profile,{indexedDB,filesystem,sync,importReplayName}){
 // Do not even inspect the legacy IndexedDB databases from the isolated MP profile.
 if(profile.multiplayerRuntime)return false;
 const marker=profile.namespace+'/.migration-v3';
 if(filesystem.analyzePath(marker).exists)return false;
 const stores=profile.game==='th08'
  ? [{name:'th08-native-1.00d',target:path=>profile.namespace+'/'+path}]
  : LANGUAGES.map(language=>({name:'th10-1.00a-'+language,target:path=>profile.namespace+'/'+language+'/'+path}));
 const databases=typeof indexedDB.databases==='function'?await indexedDB.databases():null;
 for(const store of stores){
  if(databases&&!databases.some(database=>database.name===store.name))continue;
  const database=await openDatabase(indexedDB,store.name);
  try{
   if(!database.objectStoreNames.contains('files'))continue;
   const entries=await readLegacyEntries(database);
   for(const [name,value] of entries){
    let path;try{path=profile.relativeSave(name);}catch{continue;}
    const bytes=value instanceof Blob?new Uint8Array(await value.arrayBuffer()):new Uint8Array(value);
    path=importReplayName(path,bytes);
    const target=store.target(path);
    if(!filesystem.analyzePath(target).exists){
     filesystem.mkdirTree(target.slice(0,target.lastIndexOf('/')));
     filesystem.writeFile(target,bytes);
    }
   }
  }finally{database.close();}
 }
 filesystem.writeFile(marker,new Uint8Array([1]));
 await sync(false);
 return true;
}
