import {readdirSync,readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';

const names = (directory, predicate) => readdirSync(directory).filter(predicate).sort();

function recursiveFiles(directory, predicate, prefix = '') {
  return readdirSync(directory,{withFileTypes:true}).flatMap(entry => {
    const name=prefix?prefix+'/'+entry.name:entry.name;
    if (entry.isDirectory()) return recursiveFiles(resolve(directory,entry.name),predicate,name);
    return entry.isFile()&&predicate(entry.name)?[name]:[];
  }).sort();
}

/** Keep the native oracle build and its acceptance reader on one input inventory. */
export function oracleInputs(root, {portableSdlHeaders = false} = {}) {
  const sources = [
    ...names(resolve(root, 'cpp/game'), name => name.endsWith('.cpp')).map(name => 'cpp/game/' + name),
    'tests/cpp/core-exports.cpp',
  ].sort();
  const headers = [
    ...names(resolve(root, 'cpp/game'), name => /\.(?:hpp|inc)$/.test(name)).map(name => 'cpp/game/' + name),
    ...names(resolve(root, 'tests/cpp'), name => name.endsWith('.hpp')).map(name => 'tests/cpp/' + name),
    ...recursiveFiles(resolve(root,'../../../common/include'),name => /\.(?:h|hpp|inc)$/.test(name))
      .map(name => '../../../common/include/'+name),
    ...(portableSdlHeaders
      ? names(resolve(root, '../portable/sdl'), name => name.endsWith('.hpp')).map(name => '../portable/sdl/' + name)
      : []),
  ].sort();
  const controlFiles = [
    'scripts/cpp/build.mjs',
    'target.json',
    '../../../tools/testing/oracle-inputs.mjs',
  ].sort();
  return {sources, headers, controlFiles};
}

/** Capture the exact inputs the native comparison builder will compile. */
export function captureOracleInputs(root, options = {}) {
  const inputs = oracleInputs(root, options);
  const paths = [...inputs.sources, ...inputs.headers, ...inputs.controlFiles].sort();
  const sourceHashes = Object.fromEntries(paths.map(name => [name, createHash('sha256').update(readFileSync(resolve(root, name))).digest('hex')]));
  return {...inputs, sourceHashes};
}

/** Refuse to publish build evidence if inventory or input bytes changed while compiling. */
export function assertOracleInputsUnchanged(root, captured, options = {}) {
  const current = captureOracleInputs(root, options);
  for (const field of ['sources', 'headers', 'controlFiles']) {
    if (JSON.stringify(current[field]) !== JSON.stringify(captured[field]))
      throw new Error(`Oracle input inventory changed during build: ${field}`);
  }
  for (const name of Object.keys(captured.sourceHashes)) {
    if (current.sourceHashes[name] !== captured.sourceHashes[name])
      throw new Error(`Oracle input changed during build: ${name}`);
  }
}
