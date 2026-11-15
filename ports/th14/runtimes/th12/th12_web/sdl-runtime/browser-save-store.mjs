// The mount name also identifies the IDBFS database. Keep each title isolated.
export async function openBrowserSaveStore(core,title){
 if(title!==14)throw Error('存档作品不受支持');
 const directory=`/savesth${title}`,fs=core.FS;
 let queue=Promise.resolve(),active=false;
 let lastConfig;
 const report=phase=>{
  if(!fs.readFile)return;
  let state='absent';
  try{const bytes=fs.readFile(directory+'/th14.cfg');state=`${bytes.length} bytes, BGM=${bytes[0x22]}, SE=${bytes[0x23]}`;}catch(e){if(e.errno!==44)throw e;}
  if(phase==='load'||state!==lastConfig)console.info(`[save14] ${phase}: ${state}`);
  lastConfig=state;
 };
 const synchronize=populate=>new Promise((resolve,reject)=>fs.syncfs(populate,error=>error?reject(error):resolve()));
 fs.mkdirTree(directory);fs.mount(core.IDBFS,{},directory);
 await synchronize(true);
 report('load');
 fs.mkdirTree(directory+'/replay');fs.symlink(directory,'/save');
 return {
  activate(){active=true;},
  save(){
   if(!active)return Promise.resolve();
   const pending=queue.then(async()=>{
    if(!core._th11_save_scores())throw Error('无法写入游戏存档');
    await synchronize(false);
    report('saved');
   });
   // Report a failed flush to its caller while allowing the next flush to retry.
   queue=pending.catch(()=>{});return pending;
  }
 };
}
