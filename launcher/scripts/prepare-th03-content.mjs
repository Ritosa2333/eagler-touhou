import {mkdir,readFile,copyFile,cp} from 'node:fs/promises';
import {existsSync} from 'node:fs';
import {resolve} from 'node:path';
import {validateNativeTH03Data} from '../lib/pc98-static-publication.mjs';
const arg=name=>process.argv.find(value=>value.startsWith('--'+name+'='))?.slice(name.length+3);
if(!arg('original')||!arg('output'))throw Error('Use --original=PREPARED_CONTENT_ROOT --output=DIR');
const original=resolve(arg('original')),output=resolve(arg('output'));
const prepared=existsSync(resolve(original,'th03.data'))?original:resolve(original,'content');
validateNativeTH03Data(await readFile(resolve(prepared,'th03.data')));
await mkdir(output,{recursive:true});await copyFile(resolve(prepared,'th03.data'),resolve(output,'th03.data'));
// The native runtime carries its optional soundtrack itself. Preserve a
// complete legacy music directory when supplied, but do not require it.
if(existsSync(resolve(prepared,'music')))await cp(resolve(prepared,'music'),resolve(output,'music'),{recursive:true});
if(existsSync(resolve(prepared,'source.json')))await copyFile(resolve(prepared,'source.json'),resolve(output,'source.json'));
console.log('Prepared verified TH03 content: '+output);
