import test from 'node:test';
import assert from 'node:assert/strict';
import {resolve} from 'node:path';
import {tmpdir} from 'node:os';
import {isSourceDependency,sourceDependencyTool} from './source-dependencies.mjs';

const root=resolve(tmpdir(),'eagler-dependency-boundary/ports/th10');

test('title inventories include their actual common closure and the inventory tool',()=>{
  for(const path of ['web/cpp/game/GameInput.cpp','../../common/include/eagler/thprac/PracticeInput.hpp',
    '../../common/src/netplay/RollbackJournal.cpp','..named-header.hpp',sourceDependencyTool])
    assert.equal(isSourceDependency(root,path),true,path);
});

test('common access does not admit sibling titles, prefix lookalikes or parent escapes',()=>{
  for(const path of ['../th10-mp/tests/input.cpp','../../common-copy/header.hpp',
    '../../common/../private/header.hpp','../../../private/header.hpp',
    resolve(tmpdir(),'unrelated-source/header.hpp')])
    assert.equal(isSourceDependency(root,path),false,path);
  if(process.platform==='win32')assert.equal(isSourceDependency(root,'Z:/unrelated/header.hpp'),false);
});
