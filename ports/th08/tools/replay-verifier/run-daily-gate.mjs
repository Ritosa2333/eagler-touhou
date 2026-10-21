#!/usr/bin/env node
import {writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {runTh08ReplaySuite} from '../../../../tests/adaptation/adapters/titles/th08-replay.mjs';

function options(argv){
  const result={};
  for(let index=0;index<argv.length;index++){
    const token=argv[index];
    if(!token.startsWith('--')||index+1>=argv.length)throw Error(`Invalid argument ${token}`);
    result[token.slice(2)]=argv[++index];
  }
  return result;
}
const args=options(process.argv.slice(2));
if(!args['capture-root'])throw Error('usage: node run-daily-gate.mjs --capture-root <lunatic-and-extra-directory> [--replay-lunatic <file>] [--replay-extra <file>] [--golden-root <published-golden>] [--common-root <eagler-common>] [--report <file>]');
const result=await runTh08ReplaySuite({lane:'daily',captureRoot:resolve(args['capture-root']),
  replayFiles:{lunatic:args['replay-lunatic']?resolve(args['replay-lunatic']):undefined,
    extra:args['replay-extra']?resolve(args['replay-extra']):undefined},
  goldenRoot:args['golden-root']?resolve(args['golden-root']):undefined,
  commonRoot:args['common-root']?resolve(args['common-root']):undefined});
const report={...result,suite:'daily',scope:'The declared Lunatic and Extra original Replay inputs compared through shared per-gameplay-tick state and natural-return checks.'};
if(args.report)await writeFile(resolve(args.report),`${JSON.stringify(report,null,2)}\n`);
for(const item of result.results)process.stdout.write(`${item.id}: ${item.status}${item.comparison?` (${item.comparison.comparedTicks} ticks)`:''}${item.reason?` - ${item.reason}`:''}\n`);
process.exitCode=result.complete?0:1;
