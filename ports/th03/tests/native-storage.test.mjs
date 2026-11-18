import test from 'node:test';
import assert from 'node:assert/strict';
import {createRequire} from 'node:module';
import {openUserFiles} from '../web/managed-storage.mjs';
const require=createRequire(new URL('../../../launcher/package.json',import.meta.url));
test('native settings survive closing and reopening the solo store',async()=>{
 const {IDBFactory,IDBKeyRange}=require('fake-indexeddb');globalThis.indexedDB=new IDBFactory();globalThis.IDBKeyRange=IDBKeyRange;
 const first=await openUserFiles();await first.write('PODD.CFG',Uint8Array.of(1,2,3));first.close();
 const second=await openUserFiles();assert.deepEqual([...second.memory.get('PODD.CFG')],[1,2,3]);second.close();
});
