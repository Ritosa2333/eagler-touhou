"""Read-only address/call evidence for the TH12 UFO collection map."""
import hashlib,json,pathlib,struct,sys
TITLE=pathlib.Path(__file__).resolve().parents[2];WORKSPACE=TITLE.parent
sys.path.insert(0,str(WORKSPACE/'tools/python'))
import capstone,pefile
exe=WORKSPACE/'th12/th12.exe';pe=pefile.PE(str(exe));base=pe.OPTIONAL_HEADER.ImageBase
cs=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32);cs.detail=True;cs.skipdata=True
ranges={
 'fragment_append':(0x422e80,0x422f19),'fragment_collected':(0x4270b0,0x4271b6),
 'fragment_collect_callers':(0x426bf2,0x426da8),'ufo_reset':(0x449f50,0x449f7e),
 'ufo_constructor':(0x449f80,0x449f9d),'ufo_register_callbacks':(0x449fa0,0x44a0e9),
 'ufo_tick':(0x44a2d0,0x44a4e9),'ufo_spawn':(0x44a8a0,0x44ab5f),
 'ufo_absorb_gate':(0x4271c0,0x427214),'ufo_item_absorb':(0x426870,0x426b00),
 'ufo_item_account':(0x44aca0,0x44aea0),'ufo_defeated':(0x44af00,0x44b5e0),
 'ufo_ecl_accessors':(0x425850,0x425889),'fragment_hud_rotate':(0x421a60,0x421b50),
 'fragment_hud_update':(0x4214b0,0x421687),
 'fragment_item_collected_checks':(0x4260af,0x426189),'fragment_cycle_player_proximity':(0x426236,0x42658a),
 'replay_save_fragment_slots':(0x43c643,0x43c673),'replay_restore_fragment_slots':(0x43c869,0x43c8bc),
 'fragment_item_spawn':(0x4273f0,0x4275e1),
}
out=TITLE/'local/reverse/ufo-capstone';out.mkdir(parents=True,exist_ok=True)
manifest={'exeSha256':hashlib.sha256(exe.read_bytes()).hexdigest(),'imageBase':hex(base),'ranges':{}}
for name,(start,end) in ranges.items():
    text='\n'.join(f'{i.address:08x} {i.mnemonic:9} {i.op_str}' for i in cs.disasm(pe.get_data(start-base,end-start),start))+'\n'
    file=out/f'{name}.asm';file.write_text(text);manifest['ranges'][name]={'start':hex(start),'endExclusive':hex(end),'path':str(file.relative_to(TITLE))}
refs={f'0x{a:x}':[] for a in [0x422e80,0x4270b0,0x44a8a0,0x44aca0,0x44af00,0x4b0c4c,0x4b0c50,0x4b0c54,0x4b0c58,0x4b0c5c,0x4b0c60,0x4b0c64,0x4b4534]}
section=pe.sections[0]
for i in cs.disasm(section.get_data(),base+section.VirtualAddress):
    if not i.id:continue
    for operand in i.operands:
        key=f'0x{operand.imm:x}' if operand.type==capstone.x86.X86_OP_IMM else f'0x{operand.mem.disp:x}' if operand.type==capstone.x86.X86_OP_MEM else ''
        if key in refs:refs[key].append({'address':hex(i.address),'instruction':f'{i.mnemonic} {i.op_str}'})
manifest['references']=refs
read=lambda address,n:pe.get_data(address-base,n)
scripts=struct.unpack('<4I',read(0x4b33e0,16))
manifest['tables']={
 'ufoEclScripts':{'address':'0x4b33e0','entries':[{'pointer':hex(p),'script':read(p,80).split(bytes([0]))[0].decode('ascii')} for p in scripts]},
 'spawnParameterByPowerLevel':{'address':'0x4b33f0','entries':struct.unpack('<5i',read(0x4b33f0,20))},
 'pickupItemTypeTargets':{'address':'0x426f7c','entries':{str(i+1):hex(p) for i,p in enumerate(struct.unpack('<18I',read(0x426f7c,72)))}},
 'absorbEligibilityTargets':{'address':'0x427214','entries':{str(i-1):hex(p) for i,p in enumerate(struct.unpack('<5I',read(0x427214,20)))}},
 'cyclingFragmentPlayerFreezeDistanceSquared':{'address':'0x4a4410','value':struct.unpack('<d',read(0x4a4410,8))[0]},
 'itemToUfoAbsorbDistanceSquared':{'address':'0x4a43e0','value':struct.unpack('<f',read(0x4a43e0,4))[0]},
}
(out/'manifest.json').write_text(json.dumps(manifest,indent=2)+'\n')
print(out)
