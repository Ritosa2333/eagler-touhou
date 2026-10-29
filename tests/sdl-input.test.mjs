import test from 'node:test';
import assert from 'node:assert/strict';
import {sdlPointerPosition} from '../src/sdl-input.mjs';
test('portrait hosted drag uses the fitted canvas instead of the full viewport',()=>{
  const rect={left:0,top:250,width:400,height:300},viewport={width:400,height:800};
  const down=sdlPointerPosition({x:.5,y:.5},rect,viewport,true);
  const move=sdlPointerPosition({x:.5,y:.375},rect,viewport,true);
  assert.equal(down.x,320);assert.equal(down.y,240);assert.equal(move.y-down.y,-160);
});
test('THPrac mouse CSS pixels map exactly like a native canvas mouse',()=>{
  const rect={left:80,top:100,width:800,height:600},viewport={width:960,height:800};
  const point=sdlPointerPosition({type:'down',x:120,y:150},rect,viewport);
  assert.equal(point.x,32);assert.equal(point.y,40);assert.equal(point.type,'down');
  const full=sdlPointerPosition({x:.5,y:.5},{left:0,top:0,width:640,height:480},{width:640,height:480},true);
  assert.equal(full.x,320);assert.equal(full.y,240);
});
