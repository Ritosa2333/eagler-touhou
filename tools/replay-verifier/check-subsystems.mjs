import fs from 'node:fs';import path from 'node:path';import crypto from 'node:crypto';
import createCore from '../../site/runtime/core.mjs';import {decryptBytes,parseReplay} from '../../src/replay.mjs';
const title=path.resolve(import.meta.dirname,'../..'),index=JSON.parse(fs.readFileSync(path.join(import.meta.dirname,'subsystem-golden.json')));
const raw=fs.readFileSync(path.join(import.meta.dirname,index.file));if(crypto.createHash('sha256').update(raw).digest('hex')!==index.sha256)throw Error('Golden trace hash mismatch');
const golden=JSON.parse(raw),core=await createCore();if(golden.completion.rngSeeds!==65536)throw Error('Incomplete original RNG corpus');
const next16=Buffer.from(golden.next16,'base64'),next32=Buffer.from(golden.next32,'base64');
for(let seed=0;seed<65536;seed++){
  if(core._th12_rng16(seed)!==next16.readUInt16LE(seed*2))throw Error('RNG16 first divergence at seed '+seed);
  if((core._th12_rng32(seed)>>>0)!==next32.readUInt32LE(seed*4))throw Error('RNG32 first divergence at seed '+seed);
}
for(const f of golden.replayDecryption){
  const file=path.join(title,'site/replays',f.id),bytes=fs.readFileSync(file);if(crypto.createHash('sha256').update(bytes).digest('hex')!==f.replaySha256)throw Error('Replay fixture identity differs: '+f.id);
  const packed=bytes.readUInt32LE(28),decoded=decryptBytes(decryptBytes(bytes.subarray(36,36+packed),0x5e,0xe1,0x800),0x7d,0x3a,0x40);
  if(crypto.createHash('sha256').update(decoded).digest('hex')!==f.decryptedSha256)throw Error('Replay decrypt divergence: '+f.id);
  parseReplay(bytes);
}
const result={schema:'th12-subsystem-check/1',status:'pass',goldenSha256:index.sha256,rngSeeds:65536,replays:golden.replayDecryption.length,wholeGameDeterminism:false};
fs.mkdirSync(path.join(title,'artifacts'),{recursive:true});fs.writeFileSync(path.join(title,'artifacts/subsystems-report.json'),JSON.stringify(result,null,2)+'\n');console.log(JSON.stringify(result));
