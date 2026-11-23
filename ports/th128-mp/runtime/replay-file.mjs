import {readMultiplayerReplay} from './confirmed-history.mjs';

const extension='.th128mp',maxBytes=8*1024*1024,type='application/octet-stream';
const checksum=async data=>Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',data)),byte=>byte.toString(16).padStart(2,'0')).join('');

export async function makeTh128ReplayFile(data,build){
  if(!(data instanceof Uint8Array)||data.length<1||data.length>maxBytes)throw new Error('Invalid multiplayer Replay file size');
  const bytes=data.slice(),tape=await readMultiplayerReplay(bytes,build),{matchId,epoch}=tape.header.rules;
  return new File([bytes],`th128mp-${matchId}-${epoch}${extension}`,{type});
}

export async function readTh128ReplayFile(file,build){
  if(!(file instanceof File)||!file.name.toLowerCase().endsWith(extension))throw new Error('Select a TH128 multiplayer Replay file (.th128mp)');
  if(!Number.isSafeInteger(file.size)||file.size<1||file.size>maxBytes)throw new Error('Invalid multiplayer Replay file size');
  const bytes=new Uint8Array(await file.arrayBuffer());
  if(bytes.byteLength!==file.size)throw new Error('Multiplayer Replay file length changed');
  const tape=await readMultiplayerReplay(bytes,build),sha256=await checksum(bytes);
  return Object.freeze({name:file.name,bytes,tape,sha256});
}
