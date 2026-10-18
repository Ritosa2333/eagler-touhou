// Analysis evidence only; these generated candidates are never compiled into the game.
import {spawn} from 'node:child_process';
import {mkdirSync,existsSync,readdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
const root=fileURLToPath(new URL('../../',import.meta.url));
const tools=resolve(process.env.TH09_DECOMP_TOOLS??resolve(root,'../th10_web/tools/decomp'));
const ghidra=readdirSync(tools).find(n=>n.startsWith('ghidra_12.')&&existsSync(resolve(tools,n,'Ghidra/Framework/Utility/lib/Utility.jar')));
const jdk=readdirSync(tools).find(n=>n.startsWith('jdk-21.')&&existsSync(resolve(tools,n,'bin/java.exe')));
if(!ghidra||!jdk)throw new Error('Set TH09_DECOMP_TOOLS to the directory containing Ghidra 12 and JDK 21.');
const output=resolve(root,'artifacts/cpp/analysis');
const requested=process.argv.slice(2).filter(a=>/^0x[0-9a-f]+$/i.test(a));
for(const d of ['project','settings','cache','temp','jp'])mkdirSync(resolve(output,d),{recursive:true});
const args=['-Xmx3G','-XX:ParallelGCThreads=2','-XX:CICompilerCount=2',
  '-Djava.awt.headless=true','-Dfile.encoding=UTF-8','-Duser.language=en','-Duser.country=US',
  '-Dlog4j.configurationFile='+resolve(root,'scripts/cpp/decomp/log4j.xml'),
  '-Djava.system.class.loader=ghidra.GhidraClassLoader',
  '-Dapplication.settingsdir='+resolve(output,'settings'),'-Dapplication.cachedir='+resolve(output,'cache'),
  '-Dapplication.tempdir='+resolve(output,'temp'),'-Djava.io.tmpdir='+resolve(output,'temp'),
  '-cp',resolve(tools,ghidra,'Ghidra/Framework/Utility/lib/Utility.jar'),
  'ghidra.Ghidra','ghidra.app.util.headless.AnalyzeHeadless',resolve(output,'project'),'TH09',
  ...(existsSync(resolve(output,'project/TH09.gpr'))&&!process.argv.includes('--import')
    ?['-process','th09.exe','-noanalysis']:['-import',resolve(root,'../[th09] 东方花映塚 (日文版)/th09.exe')]),
  '-max-cpu','2','-analysisTimeoutPerFile','1200','-scriptPath',resolve(root,'scripts/cpp/decomp'),
  '-postScript','ExportTh09.java',resolve(output,'jp'),...requested,
  '-log',resolve(output,'analysis.log'),'-scriptlog',resolve(output,'scripts.log')];
const child=spawn(resolve(tools,jdk,'bin/java.exe'),args,{cwd:root,windowsHide:true,stdio:['ignore','pipe','pipe']});
let tail='';
for(const stream of [child.stdout,child.stderr])stream.on('data',b=>{tail=(tail+b).slice(-1000000);});
child.stdout.pipe(process.stdout);child.stderr.pipe(process.stderr);
child.on('error',error=>{console.error(error);process.exitCode=1;});
child.on('exit',code=>{process.exitCode=code||(!tail.includes('TH09 export complete:')?1:0);});
