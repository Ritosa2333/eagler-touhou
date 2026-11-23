// th16.exe（v1.00a）Ghidra headless 分析与导出。产物是分析证据，不参与游戏编译。
// 用法：node ports/th16/web/scripts/cpp/analyze.mjs [--import] [0x地址 ...]
//   TH16_DECOMP_TOOLS  含 Ghidra 12 与 JDK 21 的目录（默认仓库根 toolchains/，接受 ghidra/ 与 jdk/ 或带版本号的目录名）
//   TH16_ORIGINAL      原版 th16.exe 路径（默认 web/target.json 的 executable）
//   TH16_DECOMP_OUTPUT 导出目录（默认仓库根 artifacts/th16/decomp，属忽略目录）
import {spawn} from 'node:child_process';
import {mkdirSync,existsSync,readdirSync,readFileSync,statSync} from 'node:fs';
import {resolve,dirname} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
const root=fileURLToPath(new URL('../../',import.meta.url));
const monorepo=resolve(root,'../../..');
const tools=resolve(process.env.TH16_DECOMP_TOOLS??resolve(monorepo,'toolchains'));
const find=(prefix,marker)=>readdirSync(tools).find(n=>n.startsWith(prefix)&&existsSync(resolve(tools,n,marker)));
const ghidra=find('ghidra','Ghidra/Framework/Utility/lib/Utility.jar');
const jdk=find('jdk','bin/java')??find('jdk','bin/java.exe');
if(!ghidra||!jdk)throw new Error('Set TH16_DECOMP_TOOLS to the directory containing Ghidra 12 and JDK 21.');
const java=existsSync(resolve(tools,jdk,'bin/java'))?resolve(tools,jdk,'bin/java'):resolve(tools,jdk,'bin/java.exe');
const target=JSON.parse(readFileSync(resolve(root,'target.json'),'utf8'));
const executable=resolve(process.env.TH16_ORIGINAL??resolve(root,target.executable));
if(!existsSync(executable))throw new Error('Original th16.exe is missing: '+executable);
const sha256=createHash('sha256').update(readFileSync(executable)).digest('hex');
if(sha256!==target.sha256)throw new Error(`th16.exe SHA-256 ${sha256} differs from target.json ${target.sha256}`);
const output=resolve(process.env.TH16_DECOMP_OUTPUT??resolve(monorepo,'artifacts/th16/decomp'));
const requested=process.argv.slice(2).filter(a=>/^0x[0-9a-f]+$/i.test(a));
for(const d of ['project','settings','cache','temp','jp'])mkdirSync(resolve(output,d),{recursive:true});
const cpus=process.env.TH16_DECOMP_CPUS??'3';
const args=['-Xmx4G','-XX:ParallelGCThreads=2','-XX:CICompilerCount=2',
  '-Djava.awt.headless=true','-Dfile.encoding=UTF-8','-Duser.language=en','-Duser.country=US',
  '-Dlog4j.configurationFile='+resolve(root,'scripts/cpp/decomp/log4j.xml'),
  '-Djava.system.class.loader=ghidra.GhidraClassLoader',
  '-Dapplication.settingsdir='+resolve(output,'settings'),'-Dapplication.cachedir='+resolve(output,'cache'),
  '-Dapplication.tempdir='+resolve(output,'temp'),'-Djava.io.tmpdir='+resolve(output,'temp'),
  '-cp',resolve(tools,ghidra,'Ghidra/Framework/Utility/lib/Utility.jar'),
  'ghidra.Ghidra','ghidra.app.util.headless.AnalyzeHeadless',resolve(output,'project'),'TH16',
  ...(existsSync(resolve(output,'project/TH16.gpr'))&&!process.argv.includes('--import')
    ?['-process','th16.exe','-noanalysis']:['-import',executable]),
  '-max-cpu',cpus,'-analysisTimeoutPerFile','3600','-scriptPath',resolve(root,'scripts/cpp/decomp'),
  '-postScript','ExportTh16.java',resolve(output,'jp'),...requested,
  '-log',resolve(output,'analysis.log'),'-scriptlog',resolve(output,'scripts.log')];
console.log(`Ghidra ${ghidra} · ${java}\n${executable} (${statSync(executable).size} bytes, sha256 ${sha256})\n→ ${output}`);
const child=spawn(java,args,{cwd:root,windowsHide:true,stdio:['ignore','pipe','pipe']});
let tail='';
for(const stream of [child.stdout,child.stderr])stream.on('data',b=>{tail=(tail+b).slice(-1000000);});
child.stdout.pipe(process.stdout);child.stderr.pipe(process.stderr);
child.on('error',error=>{console.error(error);process.exitCode=1;});
child.on('exit',code=>{process.exitCode=code||(!tail.includes('TH16 export complete:')?1:0);});
