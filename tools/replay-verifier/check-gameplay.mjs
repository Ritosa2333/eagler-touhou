// Ordinary checks never execute the original, capture or accept goldens.
import fs from 'node:fs';import path from 'node:path';
const suite=process.argv[2];if(!['quick','daily'].includes(suite))throw Error('Select quick or daily');
const corpus=JSON.parse(fs.readFileSync(path.join(import.meta.dirname,'corpus.json')));
const report={suite,status:'incomplete',wholeGameDeterminism:false,fixtures:suite==='quick'?corpus.quick:corpus.daily.fixtures,
  missing:['original full-game golden traces','full candidate state collector'],...(suite==='daily'?{coverage:corpus.daily}:{})};
console.log(JSON.stringify(report,null,2));process.exitCode=2;
