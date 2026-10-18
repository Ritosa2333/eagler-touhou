// Temporary diagnostic build; always restores the production input host.
import {readFile,writeFile} from 'node:fs/promises';
import {execFileSync} from 'node:child_process';
import {resolve} from 'node:path';
const root=resolve(import.meta.dirname,'..');
const host=resolve(root,'th20_web/cpp/sdl/InputHost.cpp');
const original=await readFile(host,'utf8');
const run=script=>execFileSync(process.execPath,[resolve(root,script),'--th20'],{cwd:root,env:{...process.env,EAGLER_FONT_ROOT:root},windowsHide:true,stdio:'inherit'});
try {
 await writeFile(host,original+'\n'+await readFile(resolve(root,'portable/probes/manual-menu.cpp'),'utf8'));
 run('portable/build.mjs');run('portable/package-eagler.mjs');
 run('portable/test-th20-manual.mjs');
} finally {
 await writeFile(host,original);
 run('portable/build.mjs');run('portable/package-eagler.mjs');
}

