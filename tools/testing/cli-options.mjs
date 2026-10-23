import assert from 'node:assert/strict';

// A misspelled scenario switch must fail before a simpler scenario can pass.
export function assertCliArguments(argv,{values=[],flags=[]}) {
  const valueNames=new Set(values),flagNames=new Set(flags),seen=new Set();
  assert.equal(valueNames.size,values.length,'Duplicate CLI value declaration');
  assert.equal(flagNames.size,flags.length,'Duplicate CLI flag declaration');
  for(const name of flagNames)assert.ok(!valueNames.has(name),'CLI option cannot be both a value and a flag');
  for(const argument of argv){
    const parsed=/^--([a-z][a-z0-9-]*)(?:=([\s\S]*))?$/.exec(argument);
    assert.ok(parsed,`Invalid argument: ${argument}`);
    const [,name,value]=parsed;
    assert.ok(valueNames.has(name)||flagNames.has(name),`Unknown option: --${name}`);
    assert.ok(!seen.has(name),`Duplicate option: --${name}`);seen.add(name);
    if(valueNames.has(name))assert.ok(value!==undefined&&value.length>0,`Option --${name} requires =VALUE`);
    else assert.equal(value,undefined,`Flag --${name} does not accept a value`);
  }
}
