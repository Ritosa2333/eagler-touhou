import {test} from 'vitest';
import assert from 'node:assert/strict';
import {resolve} from 'node:path';
import {portableReactValidationMetadata} from '../lib/react-frontend-artifact.mjs';
test('发布元数据将来源路径、对象键和依赖引用一起转为相对路径',()=>{
 const root=resolve('publication-source'),absolute=root.replaceAll('\\','/')+'/app/shared.css';
 const metadata={manifest:{[absolute]:{src:absolute,file:'assets/shared.css'},entry:{imports:[absolute],file:'assets/entry.js'}},graph:{mountPath:'/'}};
 const result=portableReactValidationMetadata(metadata,root);
 assert.deepEqual(result,{manifest:{'app/shared.css':{src:'app/shared.css',file:'assets/shared.css'},entry:{imports:['app/shared.css'],file:'assets/entry.js'}},graph:{mountPath:'/'}});
 assert.ok(Object.hasOwn(metadata.manifest,absolute));
});
