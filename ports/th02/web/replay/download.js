// Download the same Replay that is stored in the browser's Replay library.
export function downloadReplay(blob,name,environment=globalThis){
  if(!(blob instanceof Blob)||!blob.size||!/^th02_[a-zA-Z0-9_-]+\.th02r$/.test(name))throw Error('Replay 文件无效');
  const url=environment.URL.createObjectURL(blob);
  try{const link=environment.document.createElement('a');link.href=url;link.download=name;link.click();}
  finally{environment.setTimeout(()=>environment.URL.revokeObjectURL(url),30000);}
}
