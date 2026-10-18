import {resolve} from 'node:path';
import {TH10_PRESENTATION_LAB_EXPORTS} from './native-abi.mjs';
export default {
  game:'th10',runtimeEntry:'th10.html',wasm:'th10-sdl.wasm',
  buildRoot:resolve(import.meta.dirname,'../../th10_web/artifacts/presentation-lab'),
  runtimeRoot:resolve(import.meta.dirname,'../../artifacts/presentation-lab/runtime'),
  artifactRoot:resolve(import.meta.dirname,'../../artifacts/presentation-lab'),
  requiredExports:TH10_PRESENTATION_LAB_EXPORTS,
};
