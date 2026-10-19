// Copy only immutable resource names and script indices from the pinned target.
import {readFileSync,writeFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {fileURLToPath} from 'node:url';
import {resolve} from 'node:path';
const root=fileURLToPath(new URL('../../',import.meta.url)),target=JSON.parse(readFileSync(resolve(root,'target.json'),'utf8'));
const b=readFileSync(resolve(root,target.executable));if(createHash('sha256').update(b).digest('hex')!==target.sha256)throw Error('Target executable changed');
const offset=a=>a>=0x4a0000?0x9e200+a-0x4a0000:0x8d000+a-0x48e000,word=a=>b.readUInt32LE(offset(a));
const string=a=>{const p=offset(a),end=b.indexOf(0,p);if(end<0||end-p>128)throw Error('Resource name bounds');return b.toString('ascii',p,end);};
const characters=Array.from({length:16},(_,i)=>[...Array.from({length:6},(_,n)=>string(word(0x4a1238+i*24+n*4))),...[0x4a1bd0,0x4a1b90,0x4a1b50,0x4a1b10,0x4a1a10,0x4a1128,0x4a117c].map(a=>word(a+i*4))]);
const backgrounds=Array.from({length:16},(_,i)=>[string(word(0x4a11b8+i*8)),string(word(0x4a11bc+i*8))]);
const rows=values=>values.map(r=>'    {'+r.map(v=>JSON.stringify(v)).join(',')+'},').join('\n');
writeFileSync(resolve(root,'cpp/game/ResourceData.inc'),'// Immutable TH09 1.50a resources. See extract-resource-data.mjs.\nconstexpr CharacterResources characters[]={\n'+rows(characters)+'\n};\nconstexpr BackgroundResources backgrounds[]={\n'+rows(backgrounds)+'\n};\n');
writeFileSync(resolve(root,'reference/resource-data.json'),JSON.stringify({sha256:target.sha256,characters,backgrounds},null,2)+'\n');
console.log(JSON.stringify({characters:characters.length,backgrounds:backgrounds.length}));
