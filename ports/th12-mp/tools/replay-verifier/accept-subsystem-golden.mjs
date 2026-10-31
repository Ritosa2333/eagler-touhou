// Explicit maintainer operation. Candidate collection and ordinary tests have
// no write access to this path. Existing content-addressed objects are immutable.
import fs from 'node:fs';import path from 'node:path';import crypto from 'node:crypto';
import {argumentsFor,verifierPaths} from './config.mjs';
const {values,positional}=argumentsFor(process.argv.slice(2)),paths=verifierPaths(values);
const filename=values.proposal??positional[0];if(!filename||positional.length>1)throw Error('Provide a reviewed original-only proposal path.');
const bytes=fs.readFileSync(filename),proposal=JSON.parse(bytes);
if(proposal.schema!=='th12-subsystem-oracle/1'||proposal.provider.name!=='original-x86-instructions'||proposal.wholeGameDeterminism!==false||proposal.completion.rngSeeds!==65536)throw Error('Incompatible subsystem golden proposal');
const digest=crypto.createHash('sha256').update(bytes).digest('hex'),root=path.join(path.dirname(paths.subsystemIndex),'golden');fs.mkdirSync(root,{recursive:true});
const dest=path.join(root,digest+'.json');if(fs.existsSync(dest)){if(!fs.readFileSync(dest).equals(bytes))throw Error('Immutable object conflict');}else fs.writeFileSync(dest,bytes,{flag:'wx'});
fs.writeFileSync(paths.subsystemIndex,JSON.stringify({schema:'th12-subsystem-golden-index/1',sha256:digest,file:'golden/'+digest+'.json',scope:proposal.scope,wholeGameDeterminism:false},null,2)+'\n');
console.log('Accepted original subsystem trace:',digest);
