import {readFile,stat} from 'node:fs/promises';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {hostArtworkFiles} from '../../launcher/lib/host-artwork.mjs';

/** The visible Launcher catalog owns its covers independently of selected DATA. */
export async function inspectOfflineArtwork(directory){
  const files=[],missing=[];
  for(const name of hostArtworkFiles(Object.keys(PRODUCT_GAMES))){
    const path=resolve(directory,name);
    try{
      const info=await stat(path);
      if(!info.isFile()||!info.size){missing.push({name,path,reason:'not a nonempty file'});continue;}
      const bytes=await readFile(path);
      files.push({name,path,bytes:bytes.length,sha256:createHash('sha256').update(bytes).digest('hex')});
    }catch(error){if(error.code!=='ENOENT')throw error;missing.push({name,path,reason:'missing'});}
  }
  return{available:missing.length===0,scope:'complete-visible-launcher-catalog',files,missing};
}
