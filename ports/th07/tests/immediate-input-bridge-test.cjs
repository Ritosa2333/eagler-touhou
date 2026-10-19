const assert = require('node:assert/strict');
const fs = require('node:fs');
const vm = require('node:vm');
const path = require('node:path');
const source = fs.readFileSync(path.join(__dirname, '../resources/shell.html'), 'utf8')
  .split('// BEGIN IMMEDIATE INPUT BRIDGE')[1].split('// END IMMEDIATE INPUT BRIDGE')[0];
const calls = [];
const context = vm.createContext({hosted:true, launched:false, protocol:'eagler-touhou/1', game:'th07', epoch:17, validEpoch:true,
  location:{origin:'https://launcher.invalid'},
  window:{innerWidth:800,innerHeight:600},
  Module:{eaglerControls:{},eaglerOptions:{netplayMode:'lan',netplayInputDelayFrames:0,netplayRollbackPolicy:'full'}, canvas:{getBoundingClientRect:() => ({left:80,top:60,width:640,height:480}),focus:()=>{}},
    _TouhouAuxTouchDown:(...args)=>calls.push(['down',...args]),
    _TouhouAuxTouchMotion:(...args)=>calls.push(['move',...args]),
    _TouhouAuxTouchUp:(...args)=>calls.push(['up',...args])},
  applyBrowserKeyboardEvent:(...args)=>calls.push(['key',...args]),
  clearBrowserKeyboard:()=>calls.push(['key-clear']), cancelAllTouches:()=>calls.push(['cancel']),
});
vm.runInContext(source.slice(source.indexOf('function applyRuntimeInputEvent')),context);
const bridge=context.__eaglerDirectInputBridge;
const message={protocol:context.protocol,game:'th07',epoch:context.epoch,command:'direct-touch',type:'move',id:1,x:.5,y:.5};
assert.equal(bridge.submit(message),false);
context.launched=true;
assert.equal(bridge.submit({...message,game:'th06'}),false);
assert.equal(bridge.submit({...message,epoch:context.epoch-1}),false);
assert.equal(bridge.submit({...message,request:'reply-required'}),false);
assert.equal(bridge.submit({...message,command:'write'}),false);
assert.equal(calls.length,0);
assert.equal(bridge.submit(message),true);
assert.deepEqual(calls.pop(),['move',1,.5,.5]);
context.Module.eaglerOptions.netplayInputDelayFrames=6;
assert.equal(bridge.submit(message),false,'balanced mode preserves queued transport');
context.Module.eaglerOptions.netplayInputDelayFrames=0;
context.Module.eaglerOptions.netplayMode=null;
assert.equal(bridge.submit(message),false,'ordinary gameplay is unchanged');
context.Module.eaglerOptions.netplayMode='lan';
context.applyRuntimeInputEvent(message);
assert.deepEqual(calls.pop(),['move',1,.5,.5],'postMessage path uses exactly the same consumer');
assert.throws(()=>bridge.submit({...message,x:NaN}),/invalid direct/);
assert.equal(calls.length,0);
const controls={...message,command:'touch-controls',fireEnabled:true,focusEnabled:false,
  bombSerial:2,escapeSerial:3,joystickX:0,joystickY:0,touchSensitivity:100};
assert.equal(bridge.submit(controls),true);
assert.equal(context.Module.eaglerControls.bombSerial,2);
assert.equal(bridge.submit({...message,command:'touch-cancel'}),true);
assert.deepEqual(calls.pop(),['cancel']);
console.log('PASS immediate input validation, lifecycle, exact coordinate conversion and shared queued consumer');
