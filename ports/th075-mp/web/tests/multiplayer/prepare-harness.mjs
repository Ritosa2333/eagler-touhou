// An isolated diagnostic fixture, intentionally outside the release packager.
import {readFileSync,writeFileSync,mkdirSync,copyFileSync} from 'node:fs';import {resolve} from 'node:path';
const root=resolve(import.meta.dirname,'../../..'),source=resolve(root,'web/sdl-runtime'),built=resolve(root,'web/artifacts/sdl3-multiplayer');
const out=resolve(process.env.EAGLER_TH075_MP_RUNTIME||resolve(root,'web/artifacts/rollback-harness'));
const common=resolve(process.env.EAGLER_COMMON_ROOT||resolve(root,'../../common'));mkdirSync(out,{recursive:true});
const put=(name,value)=>writeFileSync(resolve(out,name),value);
put('th075.html',readFileSync(resolve(source,'managed.html'),'utf8').replace('<head>','<head><meta name="eagler-data-provider" content="retail-memory">').replace('src="managed.mjs"','src="shell.mjs"'));
put('shell.mjs',readFileSync(resolve(source,'shell.mjs'),'utf8').replace("/*TH075_EAGLER_BUILD*/{version:'development',completeGame:false}",JSON.stringify({version:'rollback-diagnostic-harness',completeGame:true,multiplayer:true})));
for(const name of ['managed.css','keyboard.mjs','directory-keyboard.mjs','display.mjs','save-sync.mjs','multiplayer.mjs'])copyFileSync(resolve(source,name),resolve(out,name));
copyFileSync(resolve(root,'portable/content-bundle.mjs'),resolve(out,'content-bundle.mjs'));copyFileSync(resolve(common,'browser/adonis-calibration.mjs'),resolve(out,'adonis-calibration.mjs'));
put('th075.mjs',readFileSync(resolve(built,'th075-harness.mjs'),'utf8').replaceAll('th075-harness.wasm','th075.wasm'));copyFileSync(resolve(built,'th075-harness.wasm'),resolve(out,'th075.wasm'));
put('manifest.json',JSON.stringify({game:'th075',profile:'diagnostic-only',multiplayer:true,version:'rollback-diagnostic-harness'}));put('NOT-FOR-PUBLICATION.txt','Diagnostic exports; never deploy or import this directory.\n');console.log(out);
