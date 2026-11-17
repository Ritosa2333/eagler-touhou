import {brotliCompress, brotliDecompress, constants} from 'node:zlib';
import {promisify} from 'node:util';
import {readFile,readdir,stat,writeFile} from 'node:fs/promises';
import {resolve,join} from 'node:path';
const compress=promisify(brotliCompress),decompress=promisify(brotliDecompress);
export function th03BrotliEligible(path){return /(?:^|\/)native-classic\.wasm$/.test(path)&&/(?:^|\/)th03(?:\/|$)/.test(path)||/(?:^|\/)th03(?:\/content)?\/th03\.data$/.test(path);}
export function acceptsBrotli(value){return value.split(',').some(part=>{const [name,...params]=part.trim().split(';');const q=params.map(p=>p.trim()).find(p=>/^q=/i.test(p));return name.trim().toLowerCase()==='br'&&(!q||Number(q.slice(2))>0);});}
export async function compressTh03File(path){const raw=await readFile(path),packed=await compress(raw,{params:{[constants.BROTLI_PARAM_QUALITY]:9}});if(!(await decompress(packed)).equals(raw))throw Error('TH03 Brotli verification failed: '+path);await writeFile(path+'.br',packed);return {raw:raw.length,compressed:packed.length};}
export async function th03BrotliSidecar(path,pathname,accepted){if(!th03BrotliEligible(pathname)||!acceptsBrotli(accepted))return null;try{const [source,sidecar]=await Promise.all([stat(path),stat(path+'.br')]);return sidecar.isFile()&&sidecar.mtimeMs>=source.mtimeMs?sidecar:null;}catch(error){if(error.code==='ENOENT')return null;throw error;}}
export async function prepareTh03Brotli(site){let count=0;async function walk(dir,relative){let entries;try{entries=await readdir(dir,{withFileTypes:true});}catch(e){if(e.code==='ENOENT')return;throw e;}for(const e of entries){const name=relative?relative+'/'+e.name:e.name,full=join(dir,e.name);if(e.isDirectory())await walk(full,name);else if(e.isFile()&&th03BrotliEligible(name)){await compressTh03File(full);count++;}}}await walk(resolve(site,'runtime'),'runtime');await walk(resolve(site,'games','th03'),'games/th03');return count;}
