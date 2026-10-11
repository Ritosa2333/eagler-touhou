import assert from 'node:assert/strict';
import {mkdtemp,readFile,writeFile,rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {SITE_MANIFEST_SCHEMA,validateSiteManifest} from '../lib/contracts/site-manifest.mjs';
import {HOST_PROTOCOL} from '../lib/contracts/product-catalog.mjs';
import {loadCompiledContract} from '../lib/contracts/load-compiled-contract.mjs';
import {readCompatibleSiteManifest,legacyHostManifestResponse,normalizeSiteManifestArguments,
  writeLegacySiteManifestAlias,verifyLegacySiteManifestAlias} from '../compat/site-manifest.mjs';
const {loadCompatibleSiteManifest}=await loadCompiledContract('compat/host-manifest');
const hash='a'.repeat(64);
const current=validateSiteManifest({schema:SITE_MANIFEST_SCHEMA,protocol:HOST_PROTOCOL,profile:'web-validation-site-compat',
  shared:{resourceMode:'hosted',vanillaFont:'shared/msgothic.ttc',unicodeFont:'shared/unifont.otf'},games:{th06:{runtime:'runtime/th06/th06.html?hosted=1',
    gameData:{path:'th06.data',bytes:1,sha256:hash,version:`sha256-${hash}`,layout:`sha256-${hash}`},music:{midi:{files:[]}}}}});
const old=legacyHostManifestResponse(current);
assert.throws(()=>validateSiteManifest(old));
assert.deepEqual(readCompatibleSiteManifest(old),current);
const calls=[];
assert.deepEqual(await loadCompatibleSiteManifest(async file=>{calls.push(file);if(file==='site-manifest.json')throw Object.assign(Error('HTTP 404'),{status:404});return old;}),current);
assert.deepEqual(calls,['site-manifest.json','host-manifest.json']);
for(const value of [{...current,protocol:'wrong'}, {...current,schema:'wrong'}]) {
 let count=0;await assert.rejects(loadCompatibleSiteManifest(async()=>{count++;return value;}));assert.equal(count,1);
}
let count=0;await assert.rejects(loadCompatibleSiteManifest(async()=>{count++;throw Error('network unavailable');}));assert.equal(count,1);
assert.deepEqual(normalizeSiteManifestArguments({'host-manifest':'seed.json'}),{'site-manifest':'seed.json'});
assert.throws(()=>normalizeSiteManifestArguments({'host-manifest':'a.json','site-manifest':'b.json'}),/one site manifest/);
const root=await mkdtemp(join(tmpdir(),'eagler-site-compat-'));
try {
 await writeLegacySiteManifestAlias(root,current);await verifyLegacySiteManifestAlias(root,current);
 const alias=JSON.parse(await readFile(join(root,'host-manifest.json'),'utf8'));alias.profile='tampered';
 await writeFile(join(root,'host-manifest.json'),JSON.stringify(alias));
 await assert.rejects(verifyLegacySiteManifestAlias(root,current),/differs from/);
} finally {await rm(root,{recursive:true,force:true});}
console.log('Site Manifest canonical authority, bounded legacy reads and alias integrity: PASS');
