"""Bind source coverage to the actual ANM jump table and recovered asset DSLs."""
import hashlib,json,re,struct,sys
from collections import Counter
from pathlib import Path
HERE=Path(__file__).resolve().parent
ROOT=HERE.parents[1]
sys.path.insert(0,str(ROOT/'source_reconstruction/audit'))
from audit_source import Image
manifest=json.loads((ROOT/'reports/source_manifest.json').read_text(encoding='utf-8-sig'))
original=Path(manifest['source_directory'])/'th20.exe'
sha=hashlib.sha256(original.read_bytes()).hexdigest()
assert sha=='a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897'
pe=Image(original)
implemented=set(range(-1,8))|set(range(100,132))|set(range(200,214))|set(range(300,320))|set(range(400,442))|set(range(500,511))|set(range(600,635))
pseudocode=(ROOT/'analysis/ghidra/pseudocode/0042b5d0.c').read_text()
explicit={int(x,0) for x in re.findall(r'^      case ([\dxabcdef-]+):',pseudocode,re.M)}
assert not explicit-implemented
usage=Counter()
assets=[]
for path in sorted((ROOT/'scripts/recovered/anm').glob('*.anm.txt')):
    text=path.read_text(encoding='utf-8-sig');count=Counter(map(int,re.findall(r'\bins_(-?\d+)\(',text)))
    usage.update(count);assets.append({'path':str(path.relative_to(ROOT)),'sha256':hashlib.sha256(path.read_bytes()).hexdigest(),'instructions':sum(count.values())})
external_opcodes={300,301,304,319,418,500,501,502,503,504,505,506,508,510,600,601,602,609,610,633,634}
rows=[]
for op in sorted(implemented):
    index=pe.at(0x435284+op+1,1)[0]
    case=struct.unpack('<I',pe.at(0x435000+index*4,4))[0]
    rows.append({'opcode':op,'dispatch_case_va':hex(case),'original_explicit_case':op in explicit,'source_control_flow':True,'external_domain_calls':op in external_opcodes,'cpu_cases':0,'actual_instruction_occurrences':usage[op]})
validation=json.loads((HERE/'anm_vm_cpu_validation.json').read_text())
for row in rows:row['cpu_cases']=validation['comparisons'].get('opcode_'+str(row['opcode']),0)+validation['comparisons'].get('terminal_'+str(row['opcode']),0)
result={'original_sha256':sha,'status':'compiled_with_explicit_engine_dependencies','original_nondefault_cases':len(explicit),'source_cases_including_nop_and_label':len(rows),'unimplemented_original_cases':[],
        'default_case':'All remaining signed 16-bit opcodes advance by instruction size, as 0x42b8d3 -> 0x434dad; no missing known opcode is dispatched here.',
        'validation':{'path':'anm_vm_cpu_validation.json','sha256':hashlib.sha256((HERE/'anm_vm_cpu_validation.json').read_bytes()).hexdigest(),'checks':validation['total'],'failed':validation['failed']},
        'assets':assets,'actual_instruction_count':sum(usage.values()),'actual_unknown_opcodes':{str(k):v for k,v in usage.items() if k not in implemented},'opcodes':rows,
        'limitations':['No complete independently linked game or pixel-by-pixel gameplay validation.', 'Sprite resource binding, camera storage, allocator and creation/lifetime implementations must satisfy anm_environment extern declarations.', 'CPU checks count storage/result comparisons separately; they are not a count of independently recovered original functions.', 'Geometry allocation and child/effect creation are call-site recovery and were not original-CPU executed in this oracle.', 'Trig and fmod checks used finite inputs; the original CRT NaN error path remains unavailable in the isolated PE oracle.']}
(HERE/'anm_vm_opcode_table.json').write_text(json.dumps(result,indent=2,ensure_ascii=False),encoding='utf-8')
print(json.dumps({'original_cases':len(explicit),'actual_assets':len(assets),'actual_instructions':sum(usage.values()),'unknown_actual_opcodes':result['actual_unknown_opcodes'],'cpu_checks':validation['total']}))
