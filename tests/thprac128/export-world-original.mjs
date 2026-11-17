import fs from'node:fs';import path from'node:path';import{createHash}from'node:crypto';import assert from'node:assert/strict';import{pathToFileURL}from'node:url';
const W='D:/Codex-th128-owned-20261108/menu-lifecycle-fixes-20261113',R='D:/Codex-th128-owned-20261108/github-upload-20261115/th128',out=process.argv[2],sha=b=>createHash('sha256').update(b).digest('hex');
const raw=fs.readFileSync(W+'/Web-delivery-20261116/resource-package-dat-ogg-v2/extracted/games/th128/th128.dat');
assert.equal(sha(raw),'38be25a5f1b0bf5e88dd7f66e34e295338d6bb968794a1ff587337f46b3db045');
const{openTh128Dat}=await import(pathToFileURL(R+'/site/runtime/th128-dat.mjs'));const dat=openTh128Dat(raw),names=dat.names.filter(n=>n.endsWith('.ecl')).sort(),pins=[];
assert(names.length<=64);fs.mkdirSync(out);
for(const name of names){const b=dat.read(name);fs.writeFileSync(path.join(out,name),b,{flag:'wx'});pins.push({name,bytes:b.length,sha256:sha(b)});}
const profile=fs.readFileSync(W+'/Web-delivery-20261116/resource-package-v1/extracted/games/th128/assets/runtime/bullet-profile-8.bin');
assert.equal(profile.length,288);assert.equal(sha(profile),'7427ad4aa6459742ffaa628c7a8e303029a9692b7b3ffbdd5b7f9a3207f0d80e');
fs.writeFileSync(path.join(out,'bullet-profile-8.bin'),profile,{flag:'wx'});
fs.writeFileSync(path.join(out,'files.txt'),names.join('\n')+'\n',{flag:'wx'});fs.writeFileSync(path.join(out,'source-pins.json'),JSON.stringify(pins,null,2)+'\n',{flag:'wx'});
console.log(JSON.stringify({originalECLs:names.length,originalDATBytes:raw.length,originalDATSHA256:sha(raw),Worlds:0}));
