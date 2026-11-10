import {createHash} from 'node:crypto';
import {mkdir, readdir, readFile, writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {promisify} from 'node:util';
import {gzip as gzipCallback, gunzip as gunzipCallback} from 'node:zlib';

const gzip=promisify(gzipCallback),gunzip=promisify(gunzipCallback);
const eligible=/^runtime\/th0[234](?:\/[^/]+)*\/[a-f0-9]{64}\/.*\/wdosbox-x-rollback(?:-tracked)?\.wasm$/;
export function acceptsGzip(value){
  return value.split(',').some(part=>{const [name,...parameters]=part.trim().split(';');if(name.trim().toLowerCase()!=='gzip')return false;const q=parameters.map(x=>x.trim()).find(x=>/^q=/i.test(x));return !q||Number(q.slice(2))>0;});
}
export function pc98CompressedPath(site,pathname){
  const name=pathname.replace(/^\//,'');
  if(!eligible.test(name))return null;
  return resolve(site,'_pc98-wasm-gzip',createHash('sha256').update(name).digest('hex')+'.gz');
}
export async function preparePc98WasmGzip(site){
  const runtime=resolve(site,'runtime');let count=0;
  async function walk(directory,relative='runtime/'){
    let items;try{items=await readdir(directory,{withFileTypes:true});}catch(error){if(error.code==='ENOENT')return;throw error;}
    for(const item of items){
      const path=relative+item.name,full=resolve(directory,item.name);
      if(item.isDirectory())await walk(full,path+'/');
      else if(item.isFile()){
        const target=pc98CompressedPath(site,path);if(!target)continue;
        const bytes=await readFile(full),packed=await gzip(bytes,{level:9});
        if(!(await gunzip(packed)).equals(bytes))throw Error('WASM gzip verification failed: '+path);
        await mkdir(resolve(site,'_pc98-wasm-gzip'),{recursive:true});await writeFile(target,packed);count++;
      }
    }
  }
  await walk(runtime);return count;
}
