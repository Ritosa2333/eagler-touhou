// Development-only callback inventory for the pinned executable and SHT files.
import {readFileSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
const root=fileURLToPath(new URL('../../',import.meta.url)),target=JSON.parse(readFileSync(resolve(root,'target.json'),'utf8'));
const image=readFileSync(resolve(root,target.executable));if(createHash('sha256').update(image).digest('hex')!==target.sha256)throw Error('target mismatch');
const groups=[['create',0x4a1c10],['update',0x4a1c34],['draw',0x4a1c54],['hit',0x4a1c64]],used=groups.map(()=>new Set()),resources=[];
for(let character=0;character<16;++character){const name=`pl${String(character).padStart(2,'0')}.sht`,b=readFileSync(resolve(root,'reference/assets',name)),sets=[];
    for(let group=0;group<b.readUInt16LE(2);++group){let offset=b.readUInt32LE(0x42c+group*8),count=0;const callbacks=groups.map(()=>new Set());
        for(;b.readInt16LE(offset)>=0;offset+=56){for(let i=0;i<4;++i){const n=b.readUInt32LE(offset+40+i*4);used[i].add(n);callbacks[i].add(n);}++count;}
        sets.push({count,callbacks:callbacks.map(s=>[...s].sort((a,b)=>a-b))});
    }resources.push({name,sets});
}
const result={target:target.sha256,tables:groups.map(([role,address],i)=>({role,address:`0x${address.toString(16)}`,callbacks:[...used[i]].sort((a,b)=>a-b).map(id=>({id,address:`0x${image.readUInt32LE(0x9e200+address-0x4a0000+id*4).toString(16)}`}))})),resources};
writeFileSync(resolve(root,'reference/player-shot-callbacks.json'),JSON.stringify(result,null,2)+'\n');console.log(JSON.stringify({tables:result.tables,sets:resources.map(r=>({name:r.name,counts:r.sets.map(s=>s.count)}))},null,2));
