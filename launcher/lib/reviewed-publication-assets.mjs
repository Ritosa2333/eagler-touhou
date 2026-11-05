import {readFile} from 'node:fs/promises';
import {resolve,relative,isAbsolute,extname} from 'node:path';
import {createHash} from 'node:crypto';

const privateExtensions=new Set(['.dat','.data','.wav','.ogg','.mid','.midi','.rpy','.ttc']);
const privateArtwork=path=>/(?:^|\/)assets\/th\d{2}-(?:card|title)[^/]*\.(?:png|webp|jpe?g)$/i.test(path);
export async function loadReviewedPublicationAssets(root,manifest) {
  const data=JSON.parse(await readFile(manifest,'utf8'));
  if(data.schema!=='eagler-touhou/reviewed-source-assets/1'||!Array.isArray(data.entries))throw Error('Invalid reviewed source asset manifest');
  const safePath=path=>{
    if(typeof path!=='string'||path.includes('\\')||isAbsolute(path)||path.split('/').includes('..'))throw Error('Unsafe review evidence path');
    const file=resolve(root,path),rel=relative(root,file);
    if(rel.startsWith('..')||isAbsolute(rel))throw Error('Review evidence escapes repository');
    return file;
  };
  const entries=new Map();
  for(const entry of data.entries){
    safePath(entry.path);
    if(entries.has(entry.path)||privateExtensions.has(extname(entry.path).toLowerCase())||privateArtwork(entry.path))throw Error('Private, duplicate or unsafe reviewed asset: '+entry.path);
    entries.set(entry.path,entry);
  }
  const matches=async identity=>{
    if(!/^[a-f0-9]{64}$/.test(identity.sha256)||!['bytes','lf-text'].includes(identity.hashMode))return false;
    try {
      const raw=await readFile(safePath(identity.path));
      const bytes=identity.hashMode==='lf-text'?Buffer.from(raw.toString('utf8').replaceAll('\r\n','\n')):raw;
      return createHash('sha256').update(bytes).digest('hex')===identity.sha256;
    }catch{return false;}
  };
  return async path=>{
    const entry=entries.get(path);
    if(!entry||!Array.isArray(entry.evidence)||!entry.evidence.length||!await matches(entry))return false;
    for(const evidence of entry.evidence)if(!await matches(evidence))return false;
    return true;
  };
}
