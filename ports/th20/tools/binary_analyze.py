"""Read-only PE inventory and x86 disassembly. Function boundaries are candidates.

Run with Python 3.12 and project-local pefile/capstone. It never executes the game.
"""
from __future__ import annotations
import argparse, collections, csv, hashlib, json, math, pathlib, re, sys
ROOT=pathlib.Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'.cache'/'binary_python'))
import pefile
from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_MODE_64, CS_OP_IMM, CS_OP_MEM

ANCHORS={0x422cb0:'candidate_rng_mod48271',0x4235f0:'candidate_rng_next',0x423ee0:'candidate_rng_locked',0x423ea0:'candidate_rng_bounded',0x429830:'candidate_rng_float',0x423f50:'candidate_timer_reset',0x423f80:'candidate_timer_set',0x423fe0:'candidate_timer_set_mode',0x4530f0:'candidate_timer_add',0x4533b0:'candidate_timer_tick',0x4292e0:'candidate_rate_access',0x452f50:'candidate_rate_multiply'}

def csv_write(path,fields,rows):
    with path.open('w',newline='',encoding='utf-8') as f:
        w=csv.DictWriter(f,fieldnames=fields);w.writeheader();w.writerows(rows)

def main():
    ap=argparse.ArgumentParser();ap.add_argument('exe',type=pathlib.Path);ap.add_argument('--out',type=pathlib.Path,default=ROOT/'analysis'/'binary');args=ap.parse_args()
    args.out.mkdir(parents=True,exist_ok=True)
    raw=args.exe.read_bytes();pe=pefile.PE(data=raw);base=pe.OPTIONAL_HEADER.ImageBase
    sections=[]
    for s in pe.sections:
        data=s.get_data();freq=collections.Counter(data);entropy=-sum((n/len(data))*math.log2(n/len(data)) for n in freq.values()) if data else 0
        sections.append(dict(name=s.Name.rstrip(b'\0').decode('ascii','replace'),va=f'0x{base+s.VirtualAddress:08x}',rva=s.VirtualAddress,virtual_size=s.Misc_VirtualSize,raw_offset=s.PointerToRawData,raw_size=s.SizeOfRawData,characteristics=f'0x{s.Characteristics:08x}',entropy=round(entropy,4)))
    imports=[]
    for module in getattr(pe,'DIRECTORY_ENTRY_IMPORT',[]):
        for imp in module.imports:
            imports.append(dict(dll=module.dll.decode(),name=imp.name.decode() if imp.name else f'ordinal_{imp.ordinal}',iat_va=f'0x{imp.address:08x}',hint=imp.hint,ordinal=imp.ordinal))
    csv_write(args.out/'imports.csv',['dll','name','iat_va','hint','ordinal'],imports)
    metadata=dict(source=str(args.exe),sha256=hashlib.sha256(raw).hexdigest(),size=len(raw),machine=f'0x{pe.FILE_HEADER.Machine:04x}',image_base=f'0x{base:08x}',entrypoint=f'0x{base+pe.OPTIONAL_HEADER.AddressOfEntryPoint:08x}',subsystem=pe.OPTIONAL_HEADER.Subsystem,pe_timestamp=pe.FILE_HEADER.TimeDateStamp,dll_characteristics=f'0x{pe.OPTIONAL_HEADER.DllCharacteristics:04x}',sections=sections,import_dlls=sorted(set(i['dll'] for i in imports)),import_count=len(imports),warnings=pe.get_warnings())
    strings=[]
    for pattern,encoding in [(rb'[\x20-\x7e]{5,}','ascii'),(rb'(?:[\x20-\x7e]\x00){5,}','utf-16le')]:
        for m in re.finditer(pattern,raw):
            try:va=base+pe.get_rva_from_offset(m.start())
            except Exception:va=0
            strings.append(dict(file_offset=f'0x{m.start():08x}',va=f'0x{va:08x}',encoding=encoding,text=m.group().decode(encoding)))
    strings.sort(key=lambda x:int(x['file_offset'],16))
    csv_write(args.out/'strings.csv',['file_offset','va','encoding','text'],strings)
    str_by_va={int(x['va'],16):x['text'] for x in strings};imports_by_va={int(x['iat_va'],16):x['dll']+'!'+x['name'] for x in imports}
    engine=Cs(CS_ARCH_X86,CS_MODE_32 if pe.FILE_HEADER.Machine==0x14c else CS_MODE_64);engine.detail=True;engine.skipdata=True
    executable=[s for s in pe.sections if s.Characteristics&0x20000000]
    def is_code(va):return any(base+s.VirtualAddress<=va<base+s.VirtualAddress+s.Misc_VirtualSize for s in executable)
    boundaries=collections.defaultdict(set);boundaries[base+pe.OPTIONAL_HEADER.AddressOfEntryPoint].add('entrypoint')
    for va in ANCHORS:
        if is_code(va):boundaries[va].add('manual_candidate_anchor')
    calls=[];xrefs=[];instruction_count=0;all_addresses=set();insn_sizes={}
    with (args.out/'disassembly.asm').open('w',encoding='utf-8') as asm:
        asm.write('; Full executable-section linear x86 disassembly; data can be misidentified as instructions.\n')
        asm.write('; Original source SHA256 '+metadata['sha256']+'\n')
        for section in executable:
            asm.write(f'\n; Section {section.Name!r}\n')
            data=section.get_data()[:section.Misc_VirtualSize];va_base=base+section.VirtualAddress
            for match in re.finditer(rb'\x55\x8b\xec',data):
                va=va_base+match.start()
                if va==va_base or data[max(0,match.start()-1):match.start()] in (b'\xcc',b'\x90',b'\xc3') or va%16==0:
                    boundaries[va].add('aligned_or_padded_ebp_prologue')
            for insn in engine.disasm(data,va_base):
                instruction_count+=1;all_addresses.add(insn.address);insn_sizes[insn.address]=insn.size
                suffix=[]
                if insn.id:
                    for operand in insn.operands:
                        target=None
                        if operand.type==CS_OP_IMM:target=operand.imm & 0xffffffff
                        elif operand.type==CS_OP_MEM and operand.mem.base==0 and operand.mem.index==0:target=operand.mem.disp & 0xffffffff
                        if target is None:continue
                        kind='immediate' if operand.type==CS_OP_IMM else 'absolute_memory'
                        if insn.mnemonic=='call':
                            kind='direct_call' if operand.type==CS_OP_IMM else 'indirect_absolute_call'
                            calls.append(dict(from_va=f'0x{insn.address:08x}',to_va=f'0x{target:08x}',kind=kind,symbol=imports_by_va.get(target,ANCHORS.get(target,''))))
                            if operand.type==CS_OP_IMM and is_code(target):boundaries[target].add('direct_call_target')
                        if target in str_by_va:suffix.append('string='+repr(str_by_va[target]));kind+=':string'
                        if target in imports_by_va:suffix.append(imports_by_va[target]);kind+=':import'
                        if target in ANCHORS:suffix.append(ANCHORS[target])
                        if is_code(target) or target in imports_by_va or target in str_by_va:
                            xrefs.append(dict(from_va=f'0x{insn.address:08x}',to_va=f'0x{target:08x}',kind=kind,instruction=insn.mnemonic+' '+insn.op_str,symbol=imports_by_va.get(target,str_by_va.get(target,ANCHORS.get(target,'')))))
                asm.write(f'{insn.address:08x}  {insn.bytes.hex(" "):<34} {insn.mnemonic:<10} {insn.op_str}')
                if suffix:asm.write(' ; '+'; '.join(suffix))
                asm.write('\n')
    csv_write(args.out/'calls.csv',['from_va','to_va','kind','symbol'],calls)
    csv_write(args.out/'xrefs.csv',['from_va','to_va','kind','instruction','symbol'],xrefs)
    callers=collections.Counter(int(x['to_va'],16) for x in calls if x['kind']=='direct_call')
    starts=sorted(boundaries)
    functions=[]
    for i,va in enumerate(starts):
        sec_end=min(base+s.VirtualAddress+s.Misc_VirtualSize for s in executable if base+s.VirtualAddress<=va<base+s.VirtualAddress+s.Misc_VirtualSize)
        end=min(starts[i+1] if i+1<len(starts) else sec_end,sec_end)
        functions.append(dict(address=f'0x{va:08x}',label=ANCHORS.get(va,f'sub_{va:08x}'),next_candidate=f'0x{end:08x}',span_upper_bound=end-va,direct_callers=callers[va],evidence=';'.join(sorted(boundaries[va])),linear_instruction_boundary=va in all_addresses))
    csv_write(args.out/'function_candidates.csv',['address','label','next_candidate','span_upper_bound','direct_callers','evidence','linear_instruction_boundary'],functions)
    metadata.update(string_count=len(strings),linear_instruction_count=instruction_count,function_candidates=len(functions),direct_and_import_calls=len(calls),xref_count=len(xrefs),function_boundary_caveat='Candidate starts inferred from calls/prologues; spans are not verified function sizes. Semantic anchor names are provisional labels supplied by manual analysis.')
    (args.out/'pe_summary.json').write_text(json.dumps(metadata,ensure_ascii=False,indent=2),encoding='utf-8')
    print(json.dumps(metadata,ensure_ascii=False,indent=2))

if __name__=='__main__':main()
