import {resolve} from 'node:path';
import {TH11_PRESENTATION_LAB_EXPORTS} from './native-abi.mjs';
export default {
  game:'th11',runtimeEntry:'th11.html',wasm:'th11-sdl.wasm',
  buildRoot:resolve(import.meta.dirname,'../../web/artifacts/presentation-lab'),
  runtimeRoot:resolve(import.meta.dirname,'../../artifacts/presentation-lab/runtime'),
  artifactRoot:resolve(import.meta.dirname,'../../artifacts/presentation-lab'),
  requiredExports:TH11_PRESENTATION_LAB_EXPORTS,
};
