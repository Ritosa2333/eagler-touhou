// Ordinary suites only read accepted golden objects. They never invoke the
// original, change corpus rules, or create/accept expected values.
import fs from 'node:fs';
import path from 'node:path';
import {argumentsFor,verifierPaths,jsonFile,prerequisite,containedFile} from './config.mjs';
import {loadCorpus,selectSuite,loadFixture,resourceIdentity,goldenEntry} from './manifest.mjs';
import {fileIdentity,validateGameplayFile} from './trace-stream.mjs';
import {captureAndCompare} from './capture-candidate.mjs';
const {values,positional}=argumentsFor(process.argv.slice(2));
const paths=verifierPaths(values),suite=positional[0];
let report={schema:'th12-gameplay-suite-report/1',suite,status:'missing-prerequisite',scope:'declared-complete-replay-routes',suiteDeterminism:false,wholeGameDeterminism:false,fixtures:[]};
try{
  if(positional.length!==1)throw Error('Select exactly quick or daily');
  const corpus=loadCorpus(paths),fixtures=selectSuite(corpus,suite),common=resourceIdentity(corpus,paths,fixtures);
  const index=jsonFile(prerequisite(paths.goldens,'accepted full-state original golden index'));
  // Validate every complete immutable expected object before candidate ticks.
  const planned=[];
  for(const fixture of fixtures){
    const loaded=loadFixture(corpus,fixture,paths),entry=goldenEntry(index,fixture.id),file=containedFile(path.dirname(paths.goldens),entry.file),identity={replaySha256:fixture.sha256,...common};
    const actual=await fileIdentity(prerequisite(file,'accepted original trace '+fixture.id));
    if(actual.sha256!==entry.sha256||actual.bytes!==entry.bytes)throw Error('Golden content-address/length mismatch: '+fixture.id);
    await validateGameplayFile(file,{identity,fixture:loaded.identity,completion:loaded.completion,original:true});
    planned.push({loaded,file,entry});
  }
  fs.mkdirSync(paths.output,{recursive:true});report.status='running';
  for(const {loaded,file,entry} of planned){
    const candidate=path.join(paths.output,loaded.fixture.sha256+'-'+Date.now()+'.candidate.jsonl');
    try{
      const result=await captureAndCompare(loaded,paths,common,candidate,file);
      const unchanged=await fileIdentity(file);if(unchanged.sha256!==entry.sha256||unchanged.bytes!==entry.bytes)throw Error('Golden changed during comparison');
      report.fixtures.push({id:loaded.fixture.id,goldenSha256:entry.sha256,candidate,...result});
      if(result.status!=='pass'){report.status='diverged';break;}
    }catch(error){report.fixtures.push({id:loaded.fixture.id,status:'failed',error:error.message,candidate});report.status='failed';break;}
  }
  if(report.status==='running'){report.status='pass';report.suiteDeterminism=true;}
}catch(error){report.error=error.message;report.status='missing-prerequisite';}
fs.mkdirSync(paths.output,{recursive:true});fs.writeFileSync(path.join(paths.output,suite==='daily'?'daily-report.json':'quick-report.json'),JSON.stringify(report,null,2)+'\n');
console.log(JSON.stringify(report,null,2));if(report.status!=='pass')process.exitCode=report.status==='diverged'?1:2;
