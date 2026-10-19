import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {readFile,writeFile} from 'node:fs/promises';
import {basename,dirname,isAbsolute,resolve} from 'node:path';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {parseStoredZip,readStoredZipEntry} from '../../launcher/package/stored-zip.mjs';
import {validatePackageDescriptor} from '../../launcher/package/package-descriptor.mjs';

const sha256=bytes=>createHash('sha256').update(bytes).digest('hex');
const isObject=value=>!!value&&typeof value==='object'&&!Array.isArray(value);
const hash=value=>typeof value==='string'&&/^[a-f0-9]{64}$/i.test(value);

function validateDataIdentity(value,label){
  assert.ok(isObject(value),`${label} DATA identity must be an object`);
  assert.ok(Number.isSafeInteger(value.bytes)&&value.bytes>0,`${label} DATA bytes must be a positive integer`);
  assert.ok(hash(value.sha256),`${label} DATA SHA-256 is invalid`);
}

function validatePackageIdentity(value,label,{withPath=false}={}){
  assert.ok(isObject(value),`${label} package identity must be an object`);
  assert.ok(typeof value.revision==='string'&&value.revision.length>=8,`${label} descriptor revision is invalid`);
  assert.ok(hash(value.archiveSha256),`${label} archive SHA-256 is invalid`);
  validateDataIdentity(value.data,`${label}`);
  if(withPath)assert.ok(typeof value.path==='string'&&value.path.length>0,`${label} archive path is required`);
}

export function validateOfflineUpdateManifest(value){
  assert.ok(isObject(value),`Update manifest must be an object`);
  assert.equal(value.schema,'eagler-touhou/offline-package-updates/1','Unsupported update manifest schema');
  assert.ok(isObject(value.games)&&Object.keys(value.games).length>0,'Update manifest games must be a nonempty object');
  for(const [game,entry] of Object.entries(value.games)){
    assert.ok(PRODUCT_GAMES[game],`Update manifest contains unsupported game ${game}`);
    assert.ok(isObject(entry),`${game}: update entry must be an object`);
    validatePackageIdentity(entry.from,`${game} base`);
    validatePackageIdentity(entry.to,`${game} update`,{withPath:true});
    assert.notEqual(entry.from.revision,entry.to.revision,`${game}: update descriptor revision must differ from the base`);
    assert.deepEqual(entry.from.data,entry.to.data,`${game}: update lane requires the declared DATA identity to remain stable`);
    assert.ok(Array.isArray(entry.to.ogg)&&entry.to.ogg.length>0,`${game}: update ZIP must declare real OGG files`);
    const names=new Set();
    for(const item of entry.to.ogg){
      assert.ok(isObject(item),`${game}: OGG identity must be an object`);
      assert.ok(typeof item.name==='string'&&/^[A-Za-z0-9_.-]+\.ogg$/i.test(item.name),`${game}: invalid OGG basename`);
      assert.ok(!names.has(item.name),`${game}: duplicate OGG identity ${item.name}`);names.add(item.name);
      assert.ok(Number.isSafeInteger(item.bytes)&&item.bytes>0,`${game}/${item.name}: OGG byte count is invalid`);
      assert.ok(hash(item.sha256),`${game}/${item.name}: OGG SHA-256 is invalid`);
    }
  }
  return value;
}

export async function loadOfflineUpdateManifest(path){
  const absolute=resolve(path),bytes=await readFile(absolute),value=validateOfflineUpdateManifest(JSON.parse(bytes.toString('utf8')));
  const games=Object.fromEntries(Object.entries(value.games).map(([game,entry])=>[game,{
    ...entry,to:{...entry.to,path:isAbsolute(entry.to.path)?resolve(entry.to.path):resolve(dirname(absolute),entry.to.path)},
  }]));
  return{path:absolute,bytes:bytes.length,sha256:sha256(bytes),games};
}

/** Read only package.json from a ZIP. Full entry hashes are checked by room-resource preflight. */
export async function inspectOfflineUpdateDescriptor(path){
  const absolute=resolve(path),archive=await readFile(absolute),blob=new Blob([archive]);
  const entries=await parseStoredZip(blob),descriptorEntry=entries.get('package.json');
  assert.ok(descriptorEntry,`${absolute}: ZIP is missing package.json`);
  const descriptor=JSON.parse(new TextDecoder('utf-8',{fatal:true}).decode(await readStoredZipEntry(blob,descriptorEntry)));
  validatePackageDescriptor(descriptor);
  return{path:absolute,archiveBytes:archive.length,archiveSha256:sha256(archive),descriptor,
    ogg:Object.entries(descriptor.files).filter(([id])=>id.startsWith('ogg:')).map(([id,file])=>({
      fileId:id,name:basename(file.source),bytes:file.bytes,sha256:file.sha256,
    })).sort((a,b)=>a.name.localeCompare(b.name))};
}

/** Make a repeatable failed-import artifact by removing only the ZIP EOCD. */
export async function writeInterruptedOfflineUpdateArchive(sourcePath,destinationPath){
  const archive=await readFile(sourcePath);
  assert.ok(archive.length>22,'Update ZIP is too small to create a partial archive attempt');
  const interrupted=archive.subarray(0,archive.length-22);
  await assert.rejects(parseStoredZip(new Blob([interrupted])),/end-of-central-directory|central directory/i,
    'Interrupted update artifact must be rejected before Package Store staging');
  const destination=resolve(destinationPath);await writeFile(destination,interrupted);
  return{path:destination,bytes:interrupted.length,sha256:sha256(interrupted),sourcePath:resolve(sourcePath),
    sourceBytes:archive.length,sourceSha256:sha256(archive),removedBytes:archive.length-interrupted.length};
}

export function assertOfflineUpdateManifestIdentity({game,manifestEntry,basePath,baseRevision,baseArchiveSha256,baseData,update}){
  assert.ok(manifestEntry,`${game}: --update-manifest has no entry for this selected game`);
  assert.equal(manifestEntry.from.revision,baseRevision,`${game}: update manifest base descriptor differs from --packages ZIP`);
  assert.equal(manifestEntry.from.archiveSha256,baseArchiveSha256,`${game}: update manifest base ZIP identity differs from --packages ZIP`);
  assert.deepEqual(manifestEntry.from.data,baseData,`${game}: update manifest base DATA differs from current private DATA`);
  assert.equal(resolve(update.path),resolve(manifestEntry.to.path),`${game}: update ZIP path differs from manifest`);
  assert.equal(update.archiveSha256,manifestEntry.to.archiveSha256,`${game}: update ZIP archive SHA differs from manifest`);
  assert.equal(update.descriptor.revision,manifestEntry.to.revision,`${game}: update descriptor revision differs from manifest`);
  assert.deepEqual({bytes:update.descriptor.files[PRODUCT_GAMES[game].package.dataFileId]?.bytes,
    sha256:update.descriptor.files[PRODUCT_GAMES[game].package.dataFileId]?.sha256},manifestEntry.to.data,
  `${game}: update descriptor DATA identity differs from manifest`);
  assert.deepEqual(update.ogg.map(({name,bytes,sha256})=>({name,bytes,sha256})),manifestEntry.to.ogg,
    `${game}: update descriptor OGG inventory differs from manifest`);
  return{basePath:resolve(basePath),baseRevision,baseArchiveSha256,updatePath:update.path,
    updateRevision:update.descriptor.revision,updateArchiveBytes:update.archiveBytes,
    updateArchiveSha256:update.archiveSha256,data:manifestEntry.to.data,ogg:manifestEntry.to.ogg};
}
