import {readFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
import {gunzipSync} from 'node:zlib';
import {Fat12} from '../../ports/th03/web/disk.js';
import {extractOriginalPmd} from '../../common/pc98/pmd-archives.mjs';

const root=resolve(import.meta.dirname,'../..');
const assets=resolve(process.argv[2]??resolve(root,'..'));
const disks={th02:'th02-coop-lab/web/th02-solo-jp.hdi.gz',th03:'th03-netplay-lab/web/disks/3-jp.hdi.gz',th04:'th04-coop-lab/web/th04-solo-jp.hdi.gz'};
for(const [game,diskPath] of Object.entries(disks)){
  const disk=new Uint8Array(gunzipSync(await readFile(resolve(assets,diskPath))));
  const originals=extractOriginalPmd(game,new Fat12(disk));
  const manifest=JSON.parse(await readFile(resolve(root,'ports',game,'web/bgm/manifest.json'),'utf8'));
  for(const [hash,id] of Object.entries(manifest.headers)){
    const track=originals.get(Number(hash));
    if(!track||createHash('sha256').update(track.bytes).digest('hex')!==manifest.tracks[id].sha256)throw Error(`${game}: 原始曲目 ${id} 不匹配`);
  }
  console.log(`${game}: ${Object.keys(manifest.headers).length} 原始曲目已核对`);
}
