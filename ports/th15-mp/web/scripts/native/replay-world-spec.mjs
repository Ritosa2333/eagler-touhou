import {core,memory,root} from '../../tests/cpp/helpers.mjs';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
const c=await core(),specs=[];for(let i=0;i<3;i++){const bytes=readFileSync(resolve(root,'reference/assets/demo'+i+'.rpy')),p=c.allocate(bytes.length),file=c.replay_create();memory(c,p,bytes.length).set(bytes);if(!c.replay_open(file,p,bytes.length))throw Error('Replay open');const d=Buffer.from(memory(c,c.replay_data(file),c.replay_size(file)));specs.push({index:i,stage:d.readInt32LE(0xa4),frames:d.readUInt32LE(0xa8),character:d.readInt32LE(0x8c),difficulty:d.readInt32LE(0x94),mode:d[10]});c.release(p);c.replay_delete(file);}console.log(JSON.stringify(specs));
