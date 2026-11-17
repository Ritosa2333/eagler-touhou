import assert from 'node:assert/strict';
import {PauseMenu} from '../web/netplay/pause.js';

const menu=new PauseMenu(0,3);
assert.equal(menu.step([128,0,0]),'paused');
menu.step([0,0,0]);
assert.equal(menu.step([1,0,0]),'paused');
assert.equal(menu.selection,2);
menu.step([0,0,0]);
assert.equal(menu.step([0,0,32]),'resume');
assert.equal(menu.step([128,0,0]),'paused');
menu.step([0,0,0]);
menu.step([1,0,0]);
menu.step([0,0,0]);
assert.equal(menu.step([32,0,0]),'restart');
assert.equal(menu.exited,false);
assert.equal(menu.paused,false);
console.log('TH04 host restart and guest resume passed');
