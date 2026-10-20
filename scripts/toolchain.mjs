import fs from 'node:fs';import path from 'node:path';
export function emscripten(root){
  const candidates=[process.env.EMSDK,path.resolve(root,'../tools/emsdk'),path.resolve(root,'../../tools/emsdk')].filter(Boolean)
    .flatMap(candidate=>[candidate,path.join(candidate,'install')]);
  const sdk=candidates.find(p=>['upstream/emscripten/emcc.py','emscripten/emcc.py'].some(name=>fs.existsSync(path.join(p,name))));
  const compiler=process.env.EMCC_PYTHON_SCRIPT||(sdk&&['upstream/emscripten/em++.py','emscripten/em++.py','upstream/emscripten/emcc.py','emscripten/emcc.py'].map(name=>path.join(sdk,name)).find(fs.existsSync));
  if(!compiler||!fs.existsSync(compiler))throw Error('Activate Emscripten 4.0.22 and set EMSDK; see docs/BUILD.md.');
  const bundled=sdk&&path.join(sdk,'python/3.13.3_64bit/python.exe');
  const python=process.env.EMSDK_PYTHON||process.env.PYTHON||(bundled&&fs.existsSync(bundled)?bundled:'python');
  const versionFile=path.join(path.dirname(compiler),'emscripten-version.txt');
  const version=fs.existsSync(versionFile)?fs.readFileSync(versionFile,'utf8').trim().replace(/^"|"$/g,''):'unknown';
  return {python,compiler,sdk,version};
}
