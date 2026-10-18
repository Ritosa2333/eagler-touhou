import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath,pathToFileURL} from 'node:url';
const here=fileURLToPath(new URL('./',import.meta.url)),common=resolve(process.env.EAGLER_COMMON_ROOT||fileURLToPath(new URL('../../../eagler-common',import.meta.url)));
const {options,compareFileSuite,acceptFileSuite}=await import(pathToFileURL(resolve(common,'testkit/replay-verifier/file-suite.mjs')));
const args=options(),corpus=JSON.parse(readFileSync(resolve(here,'corpus.json')));
if(!args['capture-root'])throw Error('Use --capture-root directory [--lane quick|daily|all] [--golden-root directory] [--report file]');
if(args['accept-golden'])console.log(JSON.stringify(await acceptFileSuite({corpus,captureRoot:args['capture-root'],goldenRoot:args['accept-golden']}),null,2));
else {const result=await compareFileSuite({corpus,captureRoot:args['capture-root'],originalRoot:args['original-root'],goldenRoot:args['golden-root'],lane:args.lane||'all',report:args.report});console.log(JSON.stringify(result,null,2));if(result.status!=='PASS')process.exitCode=1;}
