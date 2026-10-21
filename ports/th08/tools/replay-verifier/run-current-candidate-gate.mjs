#!/usr/bin/env node
import {writeFile} from 'node:fs/promises';
import {dirname, join, resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {runTh08ReplaySuite} from '../../../../tests/adaptation/adapters/titles/th08-replay.mjs';

const here=dirname(fileURLToPath(import.meta.url));
function options(argv){
  const result={};
  for(let index=0;index<argv.length;index++){
    const token=argv[index];
    if(!token.startsWith('--')||index+1>=argv.length)throw new Error(`Invalid argument ${token}`);
    result[token.slice(2)]=argv[++index];
  }
  return result;
}

const args=options(process.argv.slice(2));
if(!args['capture-root'])throw new Error('usage: node run-current-candidate-gate.mjs --capture-root <capture-directory> [--input-root <prepared-demo-input>] [--golden-root <published-golden>] [--oracle-root <advanced-source>] [--common-root <eagler-common>] [--report <file>]');
const result=await runTh08ReplaySuite({lane:'quick',captureRoot:resolve(args['capture-root']),
  inputRoot:resolve(args['input-root']??join(here,'..','..','artifacts','presentation-lab','input')),
  goldenRoot:args['golden-root']?resolve(args['golden-root']):undefined,
  commonRoot:args['common-root']?resolve(args['common-root']):undefined,
  oracleRoot:args['oracle-root']?resolve(args['oracle-root']):undefined});
const report={...result,suite:'quick',scope:'Four immutable original demo captures compared through shared per-tick state, identity, and lifecycle checks.'};
if(args.report)await writeFile(resolve(args.report),`${JSON.stringify(report,null,2)}\n`);
for(const item of result.results)process.stdout.write(`${item.id}: ${item.status}${item.comparison?` (${item.comparison.comparedTicks} ticks)`:''}${item.reason?` - ${item.reason}`:''}\n`);
process.exitCode=result.complete?0:1;
