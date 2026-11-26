import {copyFile,mkdir,readFile,stat} from 'node:fs/promises';
import {resolve} from 'node:path';
const args=new Map(process.argv.slice(2).map(value=>{const match=/^--(original|output)=(.+)$/.exec(value);if(!match)throw Error('Expected --original=DIR --output=DIR');return [match[1],resolve(match[2])];}));
if(args.size!==2||!args.has('original')||!args.has('output'))throw Error('Expected --original=DIR --output=DIR');
const source=resolve(args.get('original'),'thkkd.data'),target=resolve(args.get('output'),'thkkd.data');
if(!(await stat(source)).isFile())throw Error('Missing prepared thkkd.data; run ports/thkkd/scripts/prepare-assets.py');
const bytes=await readFile(source);if(bytes.subarray(0,8).toString()!=='THKKD1\0\0')throw Error('Invalid thkkd.data');
await mkdir(args.get('output'),{recursive:true});if(source!==target)await copyFile(source,target);
console.log(JSON.stringify({game:'thkkd',bytes:bytes.length}));
