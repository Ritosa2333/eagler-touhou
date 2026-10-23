import {win32 as win32Path} from 'node:path';
import {isFullWindowsImagePath} from './windows-image-path.mjs';

/** Parse the explicitly declared Windows compiler helper images accepted by the task runner. */
export function parseWindowsCompilerHelpers(environment=process.env, platform=process.platform) {
  const raw=environment.EAGLER_TEST_WINDOWS_COMPILER_HELPERS;
  if(raw===undefined||raw===null)return [];
  if(platform!=='win32')throw new Error('EAGLER_TEST_WINDOWS_COMPILER_HELPERS is supported only on Windows');
  let parsed;
  try { parsed=JSON.parse(raw); }
  catch { throw new Error('EAGLER_TEST_WINDOWS_COMPILER_HELPERS must be a JSON array of {path, sha256}'); }
  if(!Array.isArray(parsed))throw new Error('EAGLER_TEST_WINDOWS_COMPILER_HELPERS must be a JSON array of {path, sha256}');
  const seen=new Set();
  return parsed.map((entry,index)=>{
    if(!entry||typeof entry!=='object'||Array.isArray(entry)||Object.keys(entry).sort().join(',')!=='path,sha256')
      throw new Error(`EAGLER_TEST_WINDOWS_COMPILER_HELPERS[${index}] must contain only path and sha256`);
    if(!isFullWindowsImagePath(entry.path))
      throw new Error(`EAGLER_TEST_WINDOWS_COMPILER_HELPERS[${index}].path must be a drive-qualified or UNC-share Windows path`);
    if(typeof entry.sha256!=='string'||!/^[a-f\d]{64}$/i.test(entry.sha256))
      throw new Error(`EAGLER_TEST_WINDOWS_COMPILER_HELPERS[${index}].sha256 must be a 64-digit SHA-256`);
    const path=win32Path.normalize(entry.path),key=path.toLowerCase();
    if(seen.has(key))throw new Error(`EAGLER_TEST_WINDOWS_COMPILER_HELPERS contains duplicate path: ${path}`);
    seen.add(key);
    return {path,sha256:entry.sha256.toLowerCase()};
  });
}
