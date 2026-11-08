import test from 'node:test';
import assert from 'node:assert/strict';
import {Netplay} from '../web/sdl-runtime/netplay.mjs';
const input={keys:0,direct:0,x:0,y:0};
for(const side of [0,1])test(`seat ${side}: only host reports confirmed whole-match win once`,async()=>{
 const reports=[];
 const n=new Netplay({_th19_network_winner:()=>0},{onMatch:m=>reports.push(m)});
 n.side=side;
 for(let f=0;f<600;f++)n.confirm(f,input,input,{played:true,winner:-1});
 assert.deepEqual(reports,side===0?[{start:0,frames:600}]:[]);
 n.confirm(600,input,input,{played:true,winner:0});
 n.confirm(601,input,input,{played:true,winner:0});
 n.finished=n.peerFinished=true;n.complete();await n.replayFlush;
 assert.deepEqual(reports,side===0?[{start:0,frames:600},{start:0,frames:601,winner:0}]:[]);
});
test('selection and spectator frames never report a match',()=>{
 const reports=[],n=new Netplay({},{onMatch:m=>reports.push(m)});n.side=0;
 for(let f=0;f<600;f++)n.confirm(f,input,input,{played:false,winner:-1});
 n.spectator=true;n.confirm(600,input,input,{played:true,winner:1});
 assert.deepEqual(reports,[]);
});
