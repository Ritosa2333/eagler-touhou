/** Runs before the native module can open its save database. */
export function installRuntimeStorageChannel(target) {
  const address=new URL(target.location.href);
  const requested=address.searchParams.get('storageChannel');
  if(requested&&requested!=='beta')throw Error('Unknown runtime storage channel');
  const beta=requested==='beta'||address.pathname.includes('/beta/');
  if(!beta){target.__eaglerStorageChannel='stable';return;}
  if(target.__eaglerStorageChannel==='beta')return;
  const factory=target.indexedDB;
  if(!factory)throw Error('Test mode requires independent save storage');
  const prefix='beta:';
  const scoped=name=>{
    name=String(name);
    // Game data and music use the origin's shared content store.
    return name==='eagler-touhou-package-store-v1'?name:prefix+name;
  };
  const open=factory.open.bind(factory),remove=factory.deleteDatabase.bind(factory);
  factory.open=(name,...args)=>open(scoped(name),...args);
  factory.deleteDatabase=(name,...args)=>remove(scoped(name),...args);
  if(typeof factory.databases==='function'){
    const list=factory.databases.bind(factory);
    factory.databases=async()=> (await list()).filter(db=>db.name?.startsWith(prefix))
      .map(db=>({...db,name:db.name.slice(prefix.length)}));
  }
  // TH13 reads and writes this key from both its shell and its Wasm bridge.
  const storage = target.Storage?.prototype;
  if (storage) {
    for (const method of ['getItem', 'setItem', 'removeItem']) {
      const original = storage[method];
      storage[method] = function (key, ...args) {
        const scopedKey = this === target.localStorage && key === 'th13.save' ? prefix + key : key;
        return original.call(this, scopedKey, ...args);
      };
    }
  }
  target.__eaglerStorageChannel='beta';
}
