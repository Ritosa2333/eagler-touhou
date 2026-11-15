import {spawnSync} from 'node:child_process';
import {existsSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
const source=new URL('../../ports/th05/scripts/prepare-content.mjs',import.meta.url);const script=fileURLToPath(existsSync(source)?source:new URL('../host/content-preparers/th05/scripts/prepare-content.mjs',import.meta.url));
const result=spawnSync(process.execPath,[script,...process.argv.slice(2)],{stdio:'inherit',windowsHide:true});
if(result.error)throw result.error;process.exitCode=result.status??1;
