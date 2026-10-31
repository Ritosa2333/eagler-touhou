"""Explicit read-only TH12 1.00b UFO address/table/disassembly inspection.

This is diagnostic material, not a Runtime dependency, game execution or Oracle
capture. Linear .text xrefs are candidates and require CFG confirmation.
"""
import pathlib,sys,struct,json,hashlib
TITLE=pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0,str(TITLE.parent/'tools/python'))
import pefile,capstone
from capstone.x86_const import X86_OP_IMM,X86_OP_MEM
exe=TITLE.parent/'th12/th12.exe'
identity=hashlib.sha256(exe.read_bytes()).hexdigest()
if identity!='99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417':raise ValueError('UFO addresses require pinned TH12 1.00b executable')
pe=pefile.PE(str(exe));base=pe.OPTIONAL_HEADER.ImageBase
def data(va,n):return pe.get_data(va-base,n)
def values(va,fmt,n):return list(struct.unpack('<'+fmt*n,data(va,struct.calcsize('<'+fmt)*n)))
def string(va):return data(va,128).split(b'\0')[0].decode('ascii',errors='replace')
cs=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32);cs.detail=True;cs.skipdata=True
windows={
 'boss_slots_nonempty':(0x449f20,0x449f47),'clear_active':(0x449f50,0x449f7e),
 'manager_update':(0x44a2d0,0x44a4e9),'spawn':(0x44a8a0,0x44ab5f),
 'item_absorbed':(0x44aca0,0x44ae9e),'enemy_death_rewards':(0x44af00,0x44b58e),
 'bonus_item_collection':(0x426c6f,0x426dda),'token_collection':(0x4270b0,0x4273f0),
 'ufo_attraction_and_absorption':(0x426890,0x426997),
 'may_absorb_item':(0x4271c0,0x427213),'resource_item_rewards':(0x422ce0,0x422e80),
 'maximum_point_value':(0x421880,0x4218a5),'stage_index_advance':(0x4217e0,0x4217fe),
}
tables={
 'spawn_script_names':{'address':'0x4b33e0','pointers':values(0x4b33e0,'I',4)},
 'power_bracket_life':{'address':'0x4b33f0','values':values(0x4b33f0,'i',5)},
 'absorbed_item_routing':{'address':'0x44aeb0','values':values(0x44aeb0,'B',12)},
 'absorbed_item_targets':{'address':'0x44aea0','values':values(0x44aea0,'I',4)},
 'fill_by_type_for_power':{'address':'0x44aebc','values':values(0x44aebc,'I',5)},
 'fill_by_type_for_point':{'address':'0x44aed0','values':values(0x44aed0,'I',5)},
 'fill_by_type_for_other':{'address':'0x44aee4','values':values(0x44aee4,'I',5)},
 'death_reward_by_type':{'address':'0x44b590','values':values(0x44b590,'I',5)},
 'bonus_collect_by_type':{'address':'0x426fdc','values':values(0x426fdc,'I',5)},
 'retail_item_collect_targets':{'address':'0x426f7c','index':'itemType-1','values':values(0x426f7c,'I',18)},
 'may_absorb_by_ufo_type':{'address':'0x427214','values':values(0x427214,'I',5)},
}
tables['spawn_script_names']['strings']=[string(p) for p in tables['spawn_script_names']['pointers']]
constants={}
for va,fmt in [(0x4a3fa0,'d'),(0x4a4350,'d'),(0x4a3e28,'d'),(0x4a42c0,'f'),(0x4a4060,'f'),(0x4a4014,'f'),(0x4a4078,'d'),(0x4a43c0,'d'),(0x4a4088,'f'),(0x4a4108,'d'),(0x4a3d00,'d'),(0x4a3fd8,'d'),(0x4a43e0,'f'),(0x4a43e8,'d'),(0x4a43f0,'d'),(0x4a40e0,'d')]:constants[hex(va)]={'format':fmt,'value':values(va,fmt,1)[0]}
targets={0x4b4534,0x4b0cb0,0x44a8a0,0x44aca0,0x44af00}
xrefs=[]
for section in pe.sections:
 if not section.Characteristics&0x20000000:continue
 for ins in cs.disasm(section.get_data(),base+section.VirtualAddress):
  if not ins.id:continue
  hits=[]
  for op in ins.operands:
   if op.type==X86_OP_IMM and op.imm in targets:hits.append(hex(op.imm))
   if op.type==X86_OP_MEM and op.mem.base==0 and op.mem.index==0 and op.mem.disp in targets:hits.append(hex(op.mem.disp))
  if hits:xrefs.append({'address':hex(ins.address),'instruction':ins.mnemonic+' '+ins.op_str,'targets':hits})
out=TITLE/'artifacts/reverse';out.mkdir(parents=True,exist_ok=True)
assembly=[]
for name,(begin,end) in windows.items():
 assembly.append(f'; {name} [{begin:#x}, {end:#x})')
 for ins in cs.disasm(data(begin,end-begin),begin):assembly.append(f'{ins.address:08x} {ins.mnemonic:9} {ins.op_str}')
 assembly.append('')
(out/'ufo-disassembly.asm').write_text('\n'.join(assembly),encoding='utf-8')
report={'schema':'th12-ufo-static-inspection/1','originalExeSha256':identity,'wholeGameDeterminism':False,'originalGameExecuted':False,
 'windows':{name:[hex(begin),hex(end)] for name,(begin,end) in windows.items()},'tables':tables,'constants':constants,'linearTextXrefsNotCfgValidated':xrefs}
(out/'ufo-static-inspection.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
print(json.dumps({'output':str(out),'tables':tables,'constants':constants,'xrefCount':len(xrefs)},indent=2))
