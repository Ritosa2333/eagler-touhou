import fs from 'node:fs';import path from 'node:path';
export function emscripten(root){
  const candidates=[process.env.EMSDK,path.resolve(root,'../tools/emsdk'),path.resolve(root,'../../tools/emsdk')].filter(Boolean);
  const sdk=candidates.find(p=>fs.existsSync(path.join(p,'upstream/emscripten/emcc.py')));
  const compiler=process.env.EMCC_PYTHON_SCRIPT||(sdk&&path.join(sdk,'upstream/emscripten/emcc.py'));
  if(!compiler||!fs.existsSync(compiler))throw Error('Activate Emscripten 4.0.22 and set EMSDK; see docs/BUILD.md.');
  const bundled=sdk&&path.join(sdk,'python/3.13.3_64bit/python.exe');
  const python=process.env.EMSDK_PYTHON||process.env.PYTHON||(bundled&&fs.existsSync(bundled)?bundled:'python');
  return {python,compiler};
}
