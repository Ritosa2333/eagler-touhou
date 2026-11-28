import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {SITE_FILES,REQUIRED_ONLINE_FILES} from '../../tools/package-online-site.mjs';
const site=fileURLToPath(new URL('../../site/',import.meta.url));
test('website contains every literal relative JavaScript import used by the latest game',()=>{
 const files=new Set([...SITE_FILES,...REQUIRED_ONLINE_FILES]),missing=[];
 for(const name of files){
  if(!name.endsWith('.mjs'))continue;
  const text=fs.readFileSync(path.join(site,name),'utf8');
  for(const m of text.matchAll(/(?:^import[^;\n]*?from\s*|import\s*\()\s*['"]([^'"]+)['"]/gm)){
   if(!m[1].startsWith('.'))continue;
   const target=path.posix.normalize(path.posix.join(path.posix.dirname(name),m[1]));
   if(!files.has(target))missing.push(name+' -> '+target);
  }
 }
 assert.deepEqual(missing,[]);
});
