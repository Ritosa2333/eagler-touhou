// Explicit maintainer review step. Never imported by quick/daily/candidate.
import fs from 'node:fs';
import path from 'node:path';
import {argumentsFor,verifierPaths,prerequisite,jsonFile} from './config.mjs';
import {fileIdentity,validateGameplayFile,records} from './trace-stream.mjs';
import {loadCorpus,loadFixture,resourceIdentity} from './manifest.mjs';
const {values,positional}=argumentsFor(process.argv.slice(2)),paths=verifierPaths(values);
if(positional.length||values['reviewed-original']!=='1'||!values.proposal)throw Error('Explicit reviewed-original=1 and proposal path are required; this command does not capture an Oracle');
const proposal=path.resolve(values.proposal);prerequisite(proposal,'reviewed original full-state proposal');
const iterator=records(proposal)[Symbol.asyncIterator]();let header;try{header=(await iterator.next()).value;}finally{await iterator.return?.();}
const corpus=loadCorpus(paths),fixture=corpus.fixtures.find(f=>f.id===header?.fixture?.id);
if(!fixture)throw Error('Proposal fixture is not declared in the corpus');
const loaded=loadFixture(corpus,fixture,paths),identity={replaySha256:fixture.sha256,...resourceIdentity(corpus,paths,[fixture])};
await validateGameplayFile(proposal,{identity,fixture:loaded.identity,completion:loaded.completion,original:true});
const digest=await fileIdentity(proposal),directory=path.dirname(paths.goldens),objectName='golden/'+digest.sha256+'.jsonl',object=path.join(directory,objectName);
const index=fs.existsSync(paths.goldens)?jsonFile(paths.goldens):{schema:'th12-gameplay-golden-index/1',fixtures:[]};
if(index.schema!=='th12-gameplay-golden-index/1'||!Array.isArray(index.fixtures))throw Error('Invalid golden index');
const previous=index.fixtures.find(f=>f.id===fixture.id);
if(previous&&previous.sha256!==digest.sha256&&values.replace!==previous.sha256)throw Error('Golden replacement requires --replace=<previous SHA256>; existing objects remain immutable');
fs.mkdirSync(path.dirname(object),{recursive:true});
if(fs.existsSync(object)){const existing=await fileIdentity(object);if(existing.sha256!==digest.sha256||existing.bytes!==digest.bytes)throw Error('Immutable golden object conflict');}
else fs.copyFileSync(proposal,object,fs.constants.COPYFILE_EXCL);
// Re-read after copying to bind the actual immutable object, not a stale proposal.
const accepted=await fileIdentity(object);if(accepted.sha256!==digest.sha256||accepted.bytes!==digest.bytes)throw Error('Original proposal changed during acceptance');
index.fixtures=index.fixtures.filter(f=>f.id!==fixture.id);
index.fixtures.push({id:fixture.id,file:objectName,...digest,reviewedOriginal:true,...(previous&&previous.sha256!==digest.sha256?{replaces:previous.sha256}:{})});
fs.mkdirSync(directory,{recursive:true});fs.writeFileSync(paths.goldens,JSON.stringify(index,null,2)+'\n');
console.log(JSON.stringify({status:'accepted-original',fixture:fixture.id,...digest,index:paths.goldens}));
