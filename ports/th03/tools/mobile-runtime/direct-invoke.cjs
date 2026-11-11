// The SHA-pinned NP21 table is immutable. Specialize only audited invoke sites.
function transform(wat){
  const elements=wat.match(/\(elem[^\n]+/g);
  if(!elements||elements.length!==1)throw Error('Unexpected table segments');
  const entries=elements[0].split(' func ')[1].match(/\$[^\s)]+/g);
  if(!entries||entries.length!==2243)throw Error('Unexpected table size');
  // Element segment starts at table slot 1; slot 0 remains null.
  if(!elements[0].includes('(i32.const 1)'))throw Error('Unexpected table offset');
  if(/\btable\.(set|grow|fill|copy|init)\b/.test(wat))throw Error('Mutable function table');
  const begin=wat.indexOf('  (func $native_invoke_v ');
  if(begin<0)throw Error('Missing native exception bridge');
  const end=wat.indexOf('\n)',begin);
  const handler=wat.slice(begin,end);
  const counts={664:0,666:0};
  wat=wat.replace(/i32.const (664|666)\n(\s*)call \$native_invoke_v/g,(_,slot,indent)=>{
    counts[slot]++;return `call $native_invoke_${slot}`;
  });
  if(counts[664]!==3||counts[666]!==2)throw Error('Invoke call sites changed');
  const functions=Object.keys(counts).map(slot=>handler
    .replace('$native_invoke_v (param $index i32)',`$native_invoke_${slot}`)
    .replace('local.get $index\n      call_indirect (type $t0)',`call ${entries[Number(slot)-1]}`));
  return wat.slice(0,wat.lastIndexOf(')'))+functions.join('\n')+'\n)';
}
module.exports={transform};
