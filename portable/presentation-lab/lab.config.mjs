import {resolve} from 'node:path';
import {TH11_PRESENTATION_LAB_EXPORTS} from './native-abi.mjs';
const artifactRoot=resolve(process.env.TH11_LAB_OUTPUT||resolve(import.meta.dirname,'../../artifacts/presentation-lab'));
export default {
  game:'th11',runtimeEntry:'th11.html',wasm:'th11-sdl.wasm',
  buildRoot:resolve(import.meta.dirname,'../../web',process.env.TH11_OUTPUT||'artifacts/presentation-lab'),
  runtimeRoot:resolve(artifactRoot,'runtime'),
  artifactRoot,
  requiredExports:TH11_PRESENTATION_LAB_EXPORTS,
};
