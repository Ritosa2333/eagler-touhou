import {readFile,writeFile,mkdir,copyFile,cp,access} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath,pathToFileURL} from 'node:url';
import {buildNetplayRelay} from './build.mjs';
const root=fileURLToPath(new URL('../',import.meta.url));
export async function writeRelayPackage(target) {
  const source=JSON.parse(await readFile(resolve(root,'package.json'),'utf8'));
  const lock=JSON.parse(await readFile(resolve(root,'package-lock.json'),'utf8'));
  const ws=lock.packages?.['node_modules/ws'];
  if(!ws?.version||!ws.integrity||!ws.resolved)throw Error('Relay lock must pin ws');
  const packageJson={name:source.name,version:source.version,private:true,type:'module',license:source.license,
    engines:source.engines,scripts:{start:'node server/netplay-relay.mjs'},dependencies:{ws:source.dependencies.ws}};
  const {scripts,...entry}=packageJson;
  await mkdir(target,{recursive:true});
  await writeFile(resolve(target,'package.json'),JSON.stringify(packageJson,null,2)+'\n');
  await writeFile(resolve(target,'package-lock.json'),JSON.stringify({name:source.name,version:source.version,lockfileVersion:3,requires:true,packages:{'':entry,'node_modules/ws':ws}},null,2)+'\n');
  await copyFile(resolve(root,'LICENSE'),resolve(target,'LICENSE'));
}
export async function packageRelay(output) {
  const target=resolve(output);
  try {await access(target);throw Error('Relay output already exists: '+target);}catch(error){if(error.code!=='ENOENT')throw error;}
  await mkdir(target,{recursive:true});
  await buildNetplayRelay(resolve(target,'server'));
  await writeRelayPackage(target);
  await cp(resolve(root,'deploy'),resolve(target,'deploy'),{recursive:true});
  await copyFile(resolve(root,'LICENSE'),resolve(target,'LICENSE'));
  console.log('Standalone Relay package: '+target);
}
if(process.argv[1]&&import.meta.url===pathToFileURL(resolve(process.argv[1])).href){
  if(process.argv.length!==3)throw Error('Usage: node scripts/package.mjs NEW_OUTPUT_DIRECTORY');
  await packageRelay(process.argv[2]);
}
