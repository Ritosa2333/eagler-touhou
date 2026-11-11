// Audited NP21 mobile build. See README.md; never patch the upstream binary.
const fs=require('node:fs'),crypto=require('node:crypto');
const path=require('node:path');
const mode=process.argv[2],vendor=process.argv[3];
if(!['local','net'].includes(mode)||!vendor)throw Error('Usage: node build.cjs local|net VENDOR');
(async()=>{
  const wabt=await require('wabt')();
  const input=fs.readFileSync(path.join(vendor,'np21.wasm'));
  if(crypto.createHash('sha256').update(input).digest('hex')!=='d64bbe39549a48686b1ed04fbadec68d8b33643d220395a9c09b8d54834f36e4')throw Error('Unaudited WASM');
  const old=wabt.readWasm(input,{});old.generateNames();old.applyNames();let wat=old.toText({foldExprs:false,inlineExport:false});
  const setThrew=wat.match(/\(export "Vd" \(func (\$[^)]+)\)\)/)[1];
  const count=(wat.match(/call \$a\.d\s/g)||[]).length;
  const native=true;
  if(native)wat=wat.replace(/call \$a\.d(?=\s)/g,'call $native_invoke_v');
  wat=require('./idle-wait.cjs').transform(wat);
  wat=require('./idle-wait.cjs').transform(wat,'04');
  wat=require('./frame-wait.cjs').transform(wat);
  // Diagnostic counters must not introduce unsnapshotted mutable globals.
  for(const game of ['03','04']){
    wat=wat.replace(`  (global $idle_yields${game} (mut i32) (i32.const 0))`,'')
      .replace(`  (export "__idle_yields${game}" (global $idle_yields${game}))`,'')
      .replace(`      (global.set $idle_yields${game} (i32.add (global.get $idle_yields${game}) (i32.const 1)))`,'');
  }
  if(mode==='local')wat=require('./present565.cjs').transform(wat);
  if(mode==='net')wat=wat.slice(0,wat.lastIndexOf(')'))+'\n  (export "__rollback_g0" (global $g0))\n  (export "__rollback_g1" (global $g1))\n)';
  if(mode==='local'){
    const cap=2;if(![1,2,3,4].includes(cap))throw Error('Bad batch cap');
    const begin=wat.indexOf('  (func $f1703 '),end=wat.indexOf('\n  (func ',begin+1);
    let main=wat.slice(begin,end);
    const prefix='    i32.const 0\n    call $f1129\n    call $f918';
    if(!main.includes(prefix))throw Error('Main loop anchor changed');
    main=main.replace(prefix,`    local.get $l0\n    i32.const 12\n    i32.gt_u\n    if\n      i32.const 12\n      local.set $l0\n    end\n    local.get $l0\n    i32.const ${cap}\n    i32.gt_u\n    if (result i32)\n      local.get $l0\n      i32.const ${cap}\n      i32.sub\n    else\n      i32.const 0\n    end\n    call $f1129\n    call $f918`);
    const limit='      i32.const 12\n      local.get $l0\n      local.get $l0\n      i32.const 12\n      i32.ge_u';
    if(!main.includes(limit))throw Error('Batch limit anchor changed');
    main=main.replace(limit,limit.replaceAll('i32.const 12',`i32.const ${cap}`));
    wat=wat.slice(0,begin)+main+wat.slice(end);
  }
  if(native)wat=wat.replace('(module','(module\n  (import "a" "__js_tag" (tag $js_tag (param externref)))\n  (import "a" "__check_exception" (func $check_exception (param externref)))');
  const handler=`
  (func $native_invoke_v (param $index i32) (local $sp i32)
    global.get $g0
    local.set $sp
    try
      local.get $index
      call_indirect (type $t0)
    catch $js_tag
      local.get $sp
      global.set $g0
      call $check_exception
      i32.const 1
      i32.const 0
      call ${setThrew}
    catch_all
      local.get $sp
      global.set $g0
      rethrow 0
    end)
`;
  if(native)wat=wat.slice(0,wat.lastIndexOf(')'))+handler+')';
  wat=require('./direct-invoke.cjs').transform(wat);
  const mod=wabt.parseWat('native.wat',wat,{exceptions:true,reference_types:true});mod.resolveNames();mod.validate({exceptions:true,reference_types:true});
  const bytes=mod.toBinary({write_debug_names:false}).buffer;
  fs.writeFileSync(path.join(vendor,mode==='local'?'np21-mobile.wasm':'np21-rollback-mobile.wasm'),bytes);
  console.log({rewrittenCalls:count,bytes:bytes.length,valid:WebAssembly.validate(bytes)});
})().catch(e=>{console.error(String(e));process.exitCode=1;});
