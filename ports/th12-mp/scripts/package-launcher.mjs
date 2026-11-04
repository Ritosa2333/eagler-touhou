// Both Runtime variants enter the same Launcher/Package Store assembly.
import {existsSync} from 'node:fs';
import {resolve} from 'node:path';
import {spawnSync} from 'node:child_process';
const root=resolve(import.meta.dirname,'..');
const entry=resolve(root,'../th12/scripts/package-launcher.mjs');
if(!existsSync(entry))throw Error('站点组装需要同仓库的 ports/th12 普通版源码');
const args=process.argv.slice(2);
if(!args.some(arg=>arg.startsWith('--runtime=')))args.push('--runtime='+resolve(root,'../th12/build-eagler'));
if(!args.some(arg=>arg.startsWith('--multiplayer-runtime=')))args.push('--multiplayer-runtime='+resolve(root,'build-eagler-multiplayer'));
if(!args.some(arg=>arg.startsWith('--output=')))args.push('--output='+resolve(root,'artifacts/launcher-site'));
const result=spawnSync(process.execPath,[entry,...args],{stdio:'inherit',windowsHide:true});
if(result.error)throw result.error;
process.exitCode=result.status??1;
