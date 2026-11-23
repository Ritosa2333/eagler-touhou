import assert from 'node:assert/strict';
import test from 'node:test';
import {createCoopPauseMenu} from '../runtime/coop-pause-menu.mjs';

function fixture(){
  const calls=[],menu=createCoopPauseMenu(action=>calls.push(action));
  const sync=(phase,epoch=1)=>menu.sync({phase,epoch,startup:{canStart:true}});
  const down=(code,repeat=false)=>menu.key({code,down:true,repeat}),up=code=>menu.key({code,down:false});
  const tap=code=>{const consumed=down(code);up(code);return consumed;};
  return {menu,calls,sync,tap,down,up};
}
test('Esc waits for the confirmed fence; duplicate Launcher and physical edges never double-toggle',()=>{
  const {menu,calls,sync,down,up,tap}=fixture();sync('running');
  down('Escape');down('Escape');assert.deepEqual(calls,['pause']);assert.equal(menu.status(),null);
  sync('paused');down('Escape');down('Escape',true);assert.equal(menu.status().selection,'resume');assert.deepEqual(calls,['pause']);
  up('Escape');tap('Enter');tap('Escape');assert.deepEqual(calls,['pause','resume']);assert.ok(menu.status(),'no local dismissal while peers acknowledge');
  sync('running');assert.equal(menu.status(),null);assert.equal(tap('KeyZ'),false);
});
test('paused menu exclusively owns focus; finish/restart require explicit Yes and default No',()=>{
  const {menu,calls,sync,tap,down,up}=fixture();sync('running');down('KeyZ');sync('paused');
  assert.equal(down('KeyZ'),true);assert.deepEqual(calls,[],'held fire cannot select Continue');up('KeyZ');
  tap('ArrowDown');assert.equal(menu.status().selection,'end');tap('KeyZ');assert.equal(menu.status().yes,false);
  tap('Enter');assert.equal(menu.status().confirm,false);assert.deepEqual(calls,[]);
  tap('Enter');tap('ArrowUp');assert.equal(menu.status().yes,true);tap('KeyX');assert.equal(menu.status().yes,false);
  tap('KeyX');assert.equal(menu.status().confirm,false);tap('KeyR');assert.equal(menu.status().selection,'restart');assert.equal(menu.status().confirm,true);
  tap('ArrowDown');tap('Enter');assert.deepEqual(calls,['restart']);sync('voting');assert.equal(menu.status(),null);
  sync('running',2);tap('Escape');sync('paused',2);tap('ArrowDown');tap('Enter');tap('ArrowDown');tap('Enter');assert.deepEqual(calls,['restart','pause','end']);
  sync('ended',2);assert.equal(menu.status(),null);tap('Escape');assert.deepEqual(calls,['restart','pause','end']);
});
test('remote resume, blur/keyboard-clear, focus wrap and held release never leak menu keys into the world',()=>{
  const {menu,calls,sync,tap,down,up}=fixture();sync('paused');tap('ArrowUp');assert.equal(menu.status().selection,'restart');
  down('KeyZ');sync('running');assert.equal(down('KeyZ',true),true);assert.equal(up('KeyZ'),true);assert.equal(tap('KeyZ'),false);
  sync('paused');down('ArrowDown');menu.clearInput();tap('ArrowDown');assert.equal(menu.status().selection,'save');
  tap('Escape');assert.deepEqual(calls,['resume']);sync('running');tap('KeyR');assert.deepEqual(calls,['resume','pause']);
  sync('paused');assert.equal(menu.status().selection,'restart');assert.equal(menu.status().yes,false);assert.equal(menu.status().confirm,true);
});

test('live pause has five authored actions; team-over never offers a continue or resume',()=>{
 const {menu,calls,sync,tap}=fixture();sync('paused');
 for(const expected of ['end','save','manual','restart','resume']){tap('ArrowDown');assert.equal(menu.status().selection,expected);}
 tap('ArrowDown');tap('ArrowDown');tap('Enter');assert.equal(menu.status().confirm,true);tap('ArrowUp');tap('Enter');assert.deepEqual(calls,['save']);
 menu.sync({phase:'ended',epoch:1,result:'team-over',startup:{canStart:true}});
 assert.equal(menu.status().kind,'game-over');assert.equal(menu.status().selection,'end');
 tap('Escape');tap('KeyX');assert.deepEqual(calls,['save']);
 for(const expected of ['save','manual','restart','end']){tap('ArrowDown');assert.equal(menu.status().selection,expected);}
 tap('ArrowDown');tap('Enter');assert.deepEqual(calls,['save','terminal-save']);assert.equal(menu.status().selection,'save');
});

test('plays author menu sounds locally once, after pause and on each accepted edge',()=>{
 const sounds=[],actions=[],menu=createCoopPauseMenu(action=>actions.push(action),sound=>sounds.push(sound));
 const sync=(phase,result)=>menu.sync({phase,result,epoch:1,startup:{canStart:true}});
 const press=(code,repeat=false)=>menu.key({code,down:true,repeat});
 const tap=code=>{press(code);press(code);press(code,true);menu.key({code,down:false});};
 sync('running');tap('Escape');assert.deepEqual(sounds,[]);
 sync('paused');sync('paused');tap('ArrowDown');tap('Enter');tap('ArrowUp');tap('KeyX');tap('KeyX');tap('Escape');
 assert.deepEqual(sounds,[14,10,7,10,9,9,7]);assert.deepEqual(actions,['pause','resume']);
 sync('ended','team-over');tap('Escape');tap('ArrowDown');tap('Enter');assert.deepEqual(sounds.slice(7),[10,7]);
});
