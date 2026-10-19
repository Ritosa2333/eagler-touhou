// Optional Node-only test bootstrap. Reuse an explicitly selected installed
// dependency tree without modifying that repository or downloading packages.
// NODE_OPTIONS=--import=<this file>, TH09_TEST_DEPENDENCIES_ROOT=<repo root>.
import {isBuiltin,registerHooks} from 'node:module';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';

const root=process.env.TH09_TEST_DEPENDENCIES_ROOT;
if(!root)throw Error('Set TH09_TEST_DEPENDENCIES_ROOT for the local test dependency bootstrap');
const parentURL=pathToFileURL(resolve(root,'package.json')).href;
registerHooks({resolve(specifier,context,nextResolve){
 try{return nextResolve(specifier,context);}
 catch(error){
  const bare=!isBuiltin(specifier)&&!specifier.startsWith('.')&&!specifier.startsWith('/')&&!specifier.startsWith('#')&&!/^[a-z]+:/i.test(specifier);
  if(error.code!=='ERR_MODULE_NOT_FOUND'||!bare)throw error;
  return nextResolve(specifier,{...context,parentURL});
 }
}});
