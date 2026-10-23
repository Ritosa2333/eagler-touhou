import test from 'node:test';
import assert from 'node:assert/strict';
import {assertCliArguments} from './cli-options.mjs';
const schema={values:['game','output'],flags:['lifecycle','spectator']};
test('explicit scenario flags and values containing spaces or equals are accepted',()=>{
  assert.doesNotThrow(()=>assertCliArguments(['--game=th08','--lifecycle','--output=C:/reports/name=value with spaces'],schema));
});
test('misspelled scenario flags cannot silently run the baseline scenario',()=>{
  assert.throws(()=>assertCliArguments(['--game=th08','--lifecyle'],schema),/Unknown option/);
});
test('duplicate selections and flag values fail',()=>{
  assert.throws(()=>assertCliArguments(['--game=th08','--game=th11'],schema),/Duplicate option/);
  assert.throws(()=>assertCliArguments(['--spectator=false'],schema),/does not accept a value/);
});
test('empty values and positional arguments fail',()=>{
  assert.throws(()=>assertCliArguments(['--game='],schema),/requires =VALUE/);
  assert.throws(()=>assertCliArguments(['--game','th08'],schema),/requires =VALUE/);
  assert.throws(()=>assertCliArguments(['th08'],schema),/Invalid argument/);
});
