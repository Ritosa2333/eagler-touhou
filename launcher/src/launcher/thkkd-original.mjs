import original from '../../../ports/thkkd/assets.metadata.json' with {type:'json'};
import {readDxArchive} from './thkkd-dxa.mjs';
import {sha256Hex} from '../../package/package-integrity.mjs';
const expectedFiles = new Map(Object.entries(original.files).map(([path,info])=>[path.toLowerCase(),info]));
export const THKKD_DATA_LAYOUT = 'sha256-ccd958101cf8a6f2e84fd0c60e215651eede4690f40f109044cee32973b4f273';
export function selectThkkdArchives(inputs) {
  const archives = new Map(); let directory;
  for (const input of inputs) {
    const path = String(input.archivePath || input.webkitRelativePath || input.name || '').replaceAll('\\','/');
    const parts = path.split('/'), name = parts.at(-1)?.toLowerCase();
    if (!['dat.dxa','bgm.dxa'].includes(name)) continue;
    if (parts.length > 2 || parts.some(part=>!part || part==='.' || part==='..' || /[:\x00-\x1f]/.test(part))) throw Error('海惠堂资源应位于 ZIP 根目录或单层游戏目录');
    const parent = parts.slice(0,-1).join('/').toLowerCase();
    if (directory !== undefined && directory !== parent) throw Error('dat.dxa 和 bgm.dxa 应位于同一目录');
    directory = parent;
    if (archives.has(name)) throw Error('海惠堂资源包包含重复的 '+name);
    archives.set(name,input);
  }
  if (archives.size !== 2) throw Error('请同时选择原版 dat.dxa 和 bgm.dxa，或包含它们的 ZIP');
  return archives;
}
export async function convertThkkdArchives(inputs, {convertImage,convertVideo,onTrack,signal} = {}) {
  const archives = selectThkkdArchives(inputs), entries = new Map();
  for (const [archive,blob] of archives) {
    for (const entry of await readDxArchive(blob,{signal})) {
      const path = archive.slice(0,-4)+'/'+entry.path.toLowerCase();
      if (!expectedFiles.has(path)) continue;
      if (entries.has(path)) throw Error('海惠堂资源重名：'+path);
      entries.set(path,entry);
    }
  }
  if (entries.size !== expectedFiles.size) throw Error('海惠堂原版资源不完整');
  const checked = new Set();
  async function read(name) {
    const bytes = await entries.get(name).read();
    if (!checked.has(name) && await sha256Hex(bytes) !== expectedFiles.get(name).sha256) throw Error('海惠堂资源版本不匹配：'+name);
    checked.add(name); return bytes;
  }
  const files = [], payloads = []; let offset = 0, completed = 0;
  const selected = [...entries.keys()].sort().filter(name=>!/_a\.(?:png|bmp|jpg)$/.test(name));
  for (const name of selected) {
    signal?.throwIfAborted(); onTrack?.({name,completed,total:selected.length});
    const bytes = await read(name); let blob = new Blob([bytes]), file = name, intrinsicAlpha = false;
    if (/\.(?:png|bmp|jpg)$/.test(name)) {
      const mask = name.replace(/(\.[^.]+)$/,'_a$1');
      const result = await convertImage(bytes,name,entries.has(mask) ? await read(mask) : null);
      blob = result.blob; intrinsicAlpha = result.intrinsicAlpha; file += '.png';
    } else if (name.endsWith('.wmv')) {
      blob = await convertVideo(bytes,name,signal,value=>onTrack?.({name:name+' ('+Math.min(100,Math.max(0,Math.floor(value*100)))+'%)',completed,total:selected.length}));
      file = name.slice(0,-4)+'.webm';
    }
    signal?.throwIfAborted();
    files.push({source:name,file,offset,bytes:blob.size,...(/\.(?:png|bmp|jpg)$/.test(name)?{intrinsicAlpha}:{})});
    payloads.push(blob); offset += blob.size; completed++;
  }
  if (checked.size !== entries.size) throw Error('海惠堂资源校验未完成');
  const manifest = new TextEncoder().encode(JSON.stringify({schema:'thkkd/web-assets/1',files})), header = new Uint8Array(12);
  header.set(new TextEncoder().encode('THKKD1\0\0')); new DataView(header.buffer).setUint32(8,manifest.length,true);
  onTrack?.({name:'资源校验完成',completed,total:selected.length});
  return new Blob([header,manifest,...payloads]);
}
export async function thkkdPackage(data) {
  const hash = await sha256Hex(data), id = 'game-data';
  return {descriptor:{schema:'eagler-touhou/package/1',game:'thkkd',gameVersion:'original',revision:'original-'+hash.slice(0,32),
    runtimeRequirement:{protocol:'eagler-touhou/1',target:'thkkd',dataFile:id,dataLayout:THKKD_DATA_LAYOUT},
    files:{[id]:{source:'local/thkkd.data',target:'/thkkd.data',bytes:data.size,sha256:hash,revision:hash.slice(0,24)}},base:{files:[id]},components:{}},payloads:new Map([[id,data]])};
}
