import test from 'node:test';
import assert from 'node:assert/strict';
import {activateOwnedBrowserWindow,setOwnedBrowserWindowState} from './owned-window.mjs';

test('native activation binds process and title as arguments and never falls back to another window',async()=>{
  const title='th08 fixture $(literal) "title"';let called=0;
  const execute=async(_file,args,options)=>{
    called++;assert.equal(args[args.indexOf('-PageTitle')+1],title);assert.equal(args[args.indexOf('-OwnerProcessId')+1],'123');
    assert.equal(options.windowsHide,true);
    return{stdout:JSON.stringify({status:'incomplete',processId:123,matchingWindows:0})};
  };
  const result=await activateOwnedBrowserWindow({processId:123,title,platform:'win32',execute});
  assert.equal(result.status,'incomplete');assert.equal(called,1);
  await assert.rejects(activateOwnedBrowserWindow({processId:123,title,platform:'win32',
    execute:async()=>({stdout:JSON.stringify({status:'requested',processId:456})})}),/another process/);
  const linux=await activateOwnedBrowserWindow({platform:'linux',execute:async()=>{throw Error('must not spawn');}});
  assert.equal(linux.status,'not-required');
  await setOwnedBrowserWindowState({processId:123,title,state:'minimized',platform:'win32',execute:async(file,args)=>{
    assert.equal(args.at(-1),'minimized');return{stdout:JSON.stringify({status:'requested',processId:123})};
  }});
  await assert.rejects(setOwnedBrowserWindowState({state:'maximized'}),/Unsupported/);
});

test('owned window receipt preserves failed foreground attempts and minimize diagnostics',async()=>{
  const receipt={status:'requested',processId:123,requestedState:'normal',foregroundAttempted:true,
    foregroundCallReturned:false,foregroundObserved:false,showWindowAttempted:false,
    before:{foregroundHandle:'10',foregroundProcessId:8,minimized:false},
    afterShow:{foregroundHandle:'10',foregroundProcessId:8,minimized:false},
    afterForeground:{foregroundHandle:'10',foregroundProcessId:8,minimized:false}};
  assert.deepEqual(await activateOwnedBrowserWindow({processId:123,title:'fixture',platform:'win32',
    execute:async()=>({stdout:JSON.stringify(receipt)})}),receipt);
  const minimized={...receipt,requestedState:'minimized',foregroundAttempted:false,foregroundCallReturned:null,
    showWindowAttempted:true,showWindowCommand:6,minimizedObserved:true};
  assert.deepEqual(await setOwnedBrowserWindowState({processId:123,title:'fixture',state:'minimized',platform:'win32',
    execute:async()=>({stdout:JSON.stringify(minimized)})}),minimized);
});
