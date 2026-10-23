import assert from 'node:assert/strict';
import test from 'node:test';
import {parseWindowsCompilerHelpers} from './compiler-helpers.mjs';

test('Windows compiler helper configuration validates identity and rejects ambiguity',()=>{
  const helper={path:'C:\\VS\\VC\\Tools\\vctip.exe',sha256:'A'.repeat(64)};
  const uncHelper={path:'\\\\build-host\\tool-share\\vctip.exe',sha256:'B'.repeat(64)};
  const environment={EAGLER_TEST_WINDOWS_COMPILER_HELPERS:JSON.stringify([helper,uncHelper])};
  assert.deepEqual(parseWindowsCompilerHelpers(environment,'win32'),[
    {path:helper.path,sha256:'a'.repeat(64)},{path:uncHelper.path,sha256:'b'.repeat(64)},
  ]);
  assert.deepEqual(parseWindowsCompilerHelpers({},'linux'),[]);
  assert.throws(()=>parseWindowsCompilerHelpers(environment,'linux'),/supported only on Windows/);
  for(const invalid of [
    '{',
    JSON.stringify({path:helper.path,sha256:helper.sha256}),
    JSON.stringify([{path:'relative\\vctip.exe',sha256:helper.sha256}]),
    JSON.stringify([{path:'\\vctip.exe',sha256:helper.sha256}]),
    JSON.stringify([{path:'/vctip.exe',sha256:helper.sha256}]),
    JSON.stringify([{path:'C:vctip.exe',sha256:helper.sha256}]),
    JSON.stringify([{path:'\\\\build-host',sha256:helper.sha256}]),
    JSON.stringify([{path:helper.path,sha256:'bad'}]),
    JSON.stringify([{...helper,extra:true}]),
    JSON.stringify([helper,helper]),
  ]) assert.throws(()=>parseWindowsCompilerHelpers({EAGLER_TEST_WINDOWS_COMPILER_HELPERS:invalid},'win32'),
    /EAGLER_TEST_WINDOWS_COMPILER_HELPERS/);
});
