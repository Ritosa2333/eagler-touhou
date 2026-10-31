// Real retail HUD scripts at late presentation ages; no gameplay simulation.
import assert from 'node:assert/strict';
import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {execFileSync} from 'node:child_process';
import {createHash} from 'node:crypto';

const root=resolve(import.meta.dirname,'../..');
const assets=resolve(process.env.TH12_PROFILE_ASSETS??resolve(root,'ports/th12/site/assets'));
const output=resolve(process.env.TH12_HUD_PROFILE_OUTPUT??resolve(root,'artifacts/th12-hud-sampling'));
const baseline=execFileSync('git',['rev-parse','--verify','--end-of-options',(process.env.TH12_PROFILE_BASELINE_REF??'HEAD')+'^{commit}'],{cwd:root,encoding:'utf8'}).trim();
const bytes=await readFile(resolve(assets,'atlases.json')),atlases=JSON.parse(bytes);
const hash=value=>createHash('sha256').update(value).digest('hex');
await mkdir(output,{recursive:true});
const stats=values=>{values.sort((a,b)=>a-b);return {mean:values.reduce((a,b)=>a+b,0)/values.length,p50:values[values.length>>1],max:values.at(-1)};};
const cases=[
  {name:'frame-rebind',id:0,options:{}},
  {name:'bomb-stock-change',id:21,before:{interrupt:2},options:{interrupt:3}},
  {name:'life-fragment-change',id:13,before:{interrupt:2},options:{interrupt:7}},
  {name:'ufo-slot-change',id:80,before:{interrupts:[7,10,2]},options:{interrupts:[8,11,2]}},
  {name:'hud-reset',all:true},
];
const report={schema:'eagler-touhou/th12-hud-sampling/1',baselineCommit:baseline,node:process.version,
  scope:'Retail atlas sampler only; cold HUD binding, stock/fragment interrupts and a full HUD reset',atlasesSha256:hash(bytes),rounds:5,cases:[],sources:{}};
function sample(Sampler,scenario,age){
  const sampler=new Sampler(atlases);
  if(scenario.before)sampler.sampleAll('front',scenario.id,age-1,null,scenario.before);
  const start=performance.now();let poses;
  if(scenario.all){
    poses=[sampler.sampleAll('front',0,age)];
    for(let i=0;i<8;i++)for(const offset of [13,21])poses.push(sampler.sampleAll('front',offset+i,age,null,{interrupt:i<2?2:3}));
    for(let i=0;i<3;i++)poses.push(sampler.sampleAll('front',80,age,null,{interrupts:[7+i,10+i,2]}));
  }else poses=sampler.sampleAll('front',scenario.id,age,null,scenario.options);
  const ms=performance.now()-start;
  return {ms,poses,diagnostics:sampler.diagnostics()};
}
for(const variant of ['th12','th12-mp']){
  const file=`ports/${variant}/src/animation.mjs`,source=execFileSync('git',['show',baseline+':'+file],{cwd:root,encoding:'utf8'});
  const baselinePath=resolve(output,variant+'-baseline.mjs');
  await writeFile(baselinePath,source.replace("'./easing.mjs'",JSON.stringify(pathToFileURL(resolve(root,`ports/${variant}/src/easing.mjs`)).href)));
  const {AnimationSampler:Baseline}=await import(pathToFileURL(baselinePath));
  const {AnimationSampler:Candidate}=await import(pathToFileURL(resolve(root,file)));
  report.sources[variant]={baseline:hash(source),candidate:hash(await readFile(resolve(root,file)))};
  for(const age of [18000,60000])for(const scenario of cases){
    const times={baseline:[],candidate:[]};
    sample(Baseline,scenario,120);sample(Candidate,scenario,120);
    for(let round=0;round<report.rounds;round++){
      const results={};
      for(const name of round%2?['candidate','baseline']:['baseline','candidate']){
        const result=sample(name==='baseline'?Baseline:Candidate,scenario,age);times[name].push(result.ms);results[name]=result;
      }
      assert.deepEqual(results.candidate.poses,results.baseline.poses,`${variant}/${scenario.name}/${age}`);
      assert.deepEqual(results.candidate.diagnostics,results.baseline.diagnostics);
    }
    report.cases.push({variant,age,scenario:scenario.name,baseline:stats(times.baseline),candidate:stats(times.candidate),identical:true});
  }
}
await writeFile(resolve(output,'report.json'),JSON.stringify(report,null,2)+'\n');
console.log(JSON.stringify(report));
