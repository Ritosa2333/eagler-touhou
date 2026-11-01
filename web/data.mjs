export const DATA_MAGIC='UTH05PK1';
export const DATA_SCHEMA='eagler-touhou/uth05-content/1';
export const DATA_LAYOUT_SPEC=Object.freeze({files:[{filename:'/data/kaiki1.dat'},{filename:'/data/kaiki2.dat'},{filename:'/data/zun.com'},...Array.from({length:15},(_,i)=>({filename:`/data/se/s${String(i+1).padStart(2,'0')}.wav`}))]});
const assets=/^(?:kaiki[12]\.dat|zun\.com|anex86\.bmp|practice\.bmp|replay\.bmp|chs\.dat|music\.dat|music-font\.bmp|sfx\/effects\.pcm|se\/s(?:0[1-9]|1[0-5])\.wav)$/;
export function assetPath(path){if(typeof path!=='string'||!assets.test(path))throw Error('Invalid TH05 content path');return path;}
export const digest=async bytes=>Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',bytes)),n=>n.toString(16).padStart(2,'0')).join('');
export async function decodeContent(value){
 const bytes=new Uint8Array(value),text=new TextDecoder();
 if(bytes.length<14||bytes.length>256*1024*1024||text.decode(bytes.subarray(0,8))!==DATA_MAGIC)throw Error('Invalid uth05win content container');
 const length=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength).getUint32(8,true),base=12+length;
 if(length<2||length>1024*1024||base>bytes.length)throw Error('Invalid TH05 content header');
 const manifest=JSON.parse(text.decode(bytes.subarray(12,base)));
 if(manifest.schema!==DATA_SCHEMA||manifest.game!=='th05'||!Array.isArray(manifest.files)||manifest.files.length>64)throw Error('Invalid TH05 content manifest');
 const result=new Map();let end=0;
 for(const file of manifest.files){const path=assetPath(file.path);if(result.has(path)||file.offset!==end||!Number.isSafeInteger(file.bytes)||file.bytes<=0||base+end+file.bytes>bytes.length||!/^[a-f0-9]{64}$/.test(file.sha256))throw Error('Invalid TH05 content entry');const data=bytes.slice(base+end,base+end+file.bytes);if(await digest(data)!==file.sha256)throw Error('TH05 content digest mismatch: '+path);result.set(path,data);end+=file.bytes;}
 if(base+end!==bytes.length||!['kaiki1.dat','kaiki2.dat','zun.com'].every(path=>result.has(path))||(!result.has('sfx/effects.pcm')&&!Array.from({length:15},(_,i)=>`se/s${String(i+1).padStart(2,'0')}.wav`).every(path=>result.has(path))))throw Error('Incomplete TH05 content');
 return result;
}
export async function encodeContent(files){
 const entries=[],bodies=[];let offset=0;
 for(const [path,value] of files){assetPath(path);const bytes=new Uint8Array(value);if(!bytes.length)throw Error('Empty content: '+path);entries.push({path,offset,bytes:bytes.length,sha256:await digest(bytes)});bodies.push(bytes);offset+=bytes.length;}
 const json=new TextEncoder().encode(JSON.stringify({schema:DATA_SCHEMA,game:'th05',files:entries})),result=new Uint8Array(12+json.length+offset);result.set(new TextEncoder().encode(DATA_MAGIC));new DataView(result.buffer).setUint32(8,json.length,true);result.set(json,12);let at=12+json.length;for(const b of bodies){result.set(b,at);at+=b.length;}return result;
}
