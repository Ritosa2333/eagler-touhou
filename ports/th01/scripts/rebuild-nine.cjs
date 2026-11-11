// npm ci --prefix ports/th01/scripts
// node ports/th01/scripts/patch-launch.cjs <original.wasm> <baseline.wasm>
// node ports/th01/scripts/rebuild-nine.cjs <baseline.wasm> <output.wasm>
const fs=require('fs');
const crypto=require('crypto');
const [inputPath,outputPath]=process.argv.slice(2);
if(!inputPath||!outputPath)throw Error('usage: node rebuild-nine.cjs <baseline.wasm> <candidate.wasm>');
const input=fs.readFileSync(inputPath);
if(crypto.createHash('sha256').update(input).digest('hex')!=='56a9d57b518997a9cd7c062711d75e3e6a5b705890cc4171f58fb9e177358e61')throw Error('unexpected baseline');
const wabt=require('wabt');
wabt().then(w=>{
 const mod=w.readWasm(input,{readDebugNames:true});mod.generateNames();mod.applyNames();let wat=mod.toText({foldExprs:false,inlineExport:false});
 const data=wat.indexOf('  (data ');let code=wat.slice(0,data),tail=wat.slice(data);
 code=code.replace(/(offset=|i32\.const )(\d+)\b/g,(all,p,n)=>{let x=+n;if(x>=2537808&&x<2538576)return p+(4171008+x-2537808);if(x>=2539184&&x<2545584)return p+(4173000+x-2539184);if(x>=2538928&&x<2539184)return p+(4172200+x-2538928);return all});
 function edit(name,fn){const a=code.indexOf('  (func $'+name+' ');if(a<0)throw Error(name);const b=code.indexOf('\n  (func ',a+1);code=code.slice(0,a)+fn(code.slice(a,b))+code.slice(b)}
 function once(s,a,b){if(s.split(a).length!==2)throw Error('pattern '+a);return s.replace(a,b)}
 edit('th_init',s=>once(s,'        i32.const 8\n        i32.const 1\n        local.get $p1\n        i32.const 8\n        i32.eq\n        select','        local.get $p1\n        i32.const 1\n        local.get $p1\n        i32.const 8\n        i32.eq\n        local.get $p1\n        i32.const 9\n        i32.eq\n        i32.or\n        select'));
 edit('th01::orbs_reset__',s=>{
  const start=s.indexOf('    block $B0');const end=s.indexOf('    i32.const 0\n    i32.const 0\n    i32.store offset=2538920',start);
  const old=s.slice(start,end);if(!old.includes('local.set $l1'))throw Error('reset');
  s=s.slice(0,start)+'    i32.const 0\n    i32.load offset=2537004\n    i32.const 9\n    i32.eq\n    if\n      i32.const 9\n      local.set $l1\n    else\n'+old+'    end\n'+s.slice(end);
  s=once(s,'    i32.const -256\n    local.set $l0\n    loop $L3','    i32.const 4172200\n    i32.const 0\n    i32.const 324\n    memory.fill\n    i32.const 4173000\n    i32.const 0\n    i32.const 7200\n    memory.fill\n    i32.const -256\n    local.set $l0\n    loop $L3');
  s=s.replaceAll('local.get $l1\n        i32.const 6\n        i32.eq\n        if (result i32)', 'local.get $l1\n        i32.const 6\n        i32.eq\n        local.get $l1\n        i32.const 9\n        i32.eq\n        i32.or\n        if (result i32)');
  const grid='      local.get $l0\n      i32.const 48\n      i32.add';
  s=once(s,grid, `        local.get $l1\n        i32.const 9\n        i32.eq\n        if\n          local.get $l0\n          i32.const 600\n          local.get $l5\n          i32.const 3\n          i32.rem_u\n          i32.const 32\n          i32.mul\n          i32.sub\n          local.tee $l8\n          i32.store\n          local.get $l0\n          i32.const 4\n          i32.add\n          local.get $l8\n          i32.store\n          local.get $l0\n          i32.const 8\n          i32.add\n          i32.const 264\n          local.get $l5\n          i32.const 3\n          i32.div_u\n          i32.const 32\n          i32.mul\n          i32.add\n          local.tee $l8\n          i32.store\n          local.get $l0\n          i32.const 12\n          i32.add\n          local.get $l8\n          i32.store\n          local.get $l6\n          local.get $l5\n          i32.const 3\n          i32.rem_u\n          i32.const 1\n          i32.add\n          i32.const -512\n          i32.mul\n          local.tee $l8\n          i32.store\n          local.get $l7\n          i32.const 3\n          i32.const 1\n          local.get $l8\n          i32.const -1536\n          i32.lt_s\n          select\n          i32.store\n        end\n`+grid);
  return s;
 });
 edit('th01::init_stage_bool_',s=>once(s,'    local.get $l1\n    i32.mul\n    local.tee $l1\n    i32.store offset=3601936', `    local.get $l1\n    i32.mul\n    i32.const 0\n    i32.load offset=2537004\n    i32.const 9\n    i32.eq\n    if (result i32)\n      i32.const 0\n      i32.load offset=2536992\n      i32.const 24\n      i32.mul\n      i32.const 88844\n      i32.add\n      i32.load\n      i32.const 0\n      i32.load offset=2536992\n      i32.const 24\n      i32.mul\n      i32.const 88840\n      i32.add\n      i32.load\n      i32.sub\n      i32.const 2\n      i32.div_s\n      local.get $l1\n      i32.mul\n    else\n      i32.const 0\n    end\n    i32.add\n    local.tee $l1\n    i32.store offset=3601936`));
 edit('th01::move_orbs__',s=>once(s,'            local.get $l9\n            i32.const 36\n            i32.add','            local.get $l9\n            i32.const 40\n            i32.add'));
 edit('th01::checksum__',s=>once(s,'        local.get $l6\n        i32.const 32\n        i32.add','        local.get $l6\n        i32.const 36\n        i32.add'));
 edit('th_menu_input',s=>once(s,'    block $B0', `    i32.const 0
    i32.load offset=2577536
    i32.const 2
    i32.eq
    i32.const 0
    i32.load offset=2577824
    i32.const 2
    i32.eq
    i32.and
    if
      block $cycle_done
        i32.const 0
        i32.load offset=3601864
        local.set $l1
        local.get $p0
        i32.const 3
        i32.eq
        if
          local.get $l1
          i32.const 8
          i32.eq
          if
            i32.const 0
            i32.const 9
            i32.store offset=3601864
            i32.const 0
            return
          end
          local.get $l1
          i32.const 9
          i32.eq
          if
            i32.const 0
            i32.const 1
            i32.store offset=3601864
            i32.const 0
            return
          end
        end
        local.get $p0
        i32.const 2
        i32.eq
        if
          local.get $l1
          i32.const 9
          i32.eq
          if
            i32.const 0
            i32.const 8
            i32.store offset=3601864
            i32.const 0
            return
          end
          local.get $l1
          i32.const 1
          i32.eq
          if
            i32.const 0
            i32.const 9
            i32.store offset=3601864
            i32.const 0
            return
          end
        end
      end
    end
    block $B0`));
 edit('th_get',s=>once(s,'      local.get $p1\n      i32.const 7\n      i32.gt_u','      local.get $p1\n      i32.const 8\n      i32.gt_u'));
 const m=w.parseWat('nine.wat',code+tail,{sat_float_to_int:true,bulk_memory:true});m.resolveNames();m.validate();fs.writeFileSync(outputPath,Buffer.from(m.toBinary({write_debug_names:true}).buffer));console.log('built prototype');
}).catch(e=>{console.error(e);process.exitCode=1});
