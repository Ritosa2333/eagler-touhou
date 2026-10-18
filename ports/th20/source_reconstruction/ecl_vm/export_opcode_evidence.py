"""Reproduce opcode/address evidence and actual-resource usage, never engine completion percentages."""
from pathlib import Path
import hashlib,json,struct,sys
from collections import Counter
ROOT=Path(__file__).resolve().parents[2]
HERE=Path(__file__).resolve().parent
sys.path.insert(0,str(ROOT/'source_reconstruction/audit'))
from audit_source import Image
manifest=json.loads((ROOT/'reports/source_manifest.json').read_text(encoding='utf-8'))
exe=Path(manifest['source_directory'])/'th20.exe'
digest=hashlib.sha256(exe.read_bytes()).hexdigest()
if digest!='a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897':raise ValueError('Original revision changed')
image=Image(exe)
names={0:'nop',1:'terminate',10:'return',11:'call',12:'jump',13:'jump_if_zero',14:'jump_if_nonzero',15:'call_async',16:'call_async_id',17:'stop_task',18:'set_task_flag',19:'clear_task_flag',20:'set_task_field_2c',21:'stop_all_async',22:'nop_22',23:'wait_integer',24:'wait_float',30:'nop_30',31:'nop_31',40:'enter_frame',41:'leave_frame',42:'push_integer',43:'store_integer',44:'push_float',45:'store_float',46:'select_integer_argument_pair',47:'select_float_argument_pair',50:'add_integer',51:'add_float',52:'subtract_integer',53:'subtract_float',54:'multiply_integer',55:'multiply_float',56:'divide_integer',57:'divide_float',58:'remainder_integer',59:'equal_integer',60:'equal_float',61:'not_equal_integer',62:'not_equal_float',63:'less_integer',64:'less_float',65:'less_equal_integer',66:'less_equal_float',67:'greater_integer',68:'greater_float',69:'greater_equal_integer',70:'greater_equal_float',71:'not_integer',72:'not_float',73:'logical_or',74:'logical_and',75:'bit_xor',76:'bit_or',77:'bit_and',78:'post_decrement',79:'sin',80:'cos',81:'polar',82:'wrap_angle',83:'negate_integer',84:'negate_float',85:'length_squared',86:'length',87:'atan2_points',88:'sqrt',89:'angle_difference',90:'rotate',91:'interpolate',92:'interpolate_tangents',93:'random_polar',94:'scaled_rotated_polar',95:'reflect_wrapped_angle',96:'wrapped_angle_branch_96',97:'wrapped_angle_sign'}
usage=Counter();files=[]
for path in sorted((ROOT/'reports/ecl_json').glob('*.json')):
    doc=json.loads(path.read_text(encoding='utf-8'))
    usage.update({int(k):v for k,v in doc.get('opcode_histogram',{}).items()})
    files.append(path.name)
cpu=json.loads((HERE/'cpu_validation.json').read_text(encoding='utf-8'))
records=[]
for opcode,name in sorted(names.items()):
    records.append(dict(opcode=opcode,name=name,dispatch_case_va=f'0x{struct.unpack("<I",image.at(0x53e128+opcode*4,4))[0]:08x}',
        implementation='vm.cpp',actual_instruction_occurrences=usage[opcode],
        cpu_direct_cases=cpu['comparisons'].get(f'opcode_{opcode}',0),
        validation_note='Source-owned async allocation/traversal; argument setup separately CPU compared' if opcode in [15,16] else 'Covered via synchronous_call_return/root_call_return' if opcode in [10,11] else 'See cpu_validation.json scope and edge limitations'))
result=dict(source_executable_sha256=digest,core_dispatch='0x0053b5c0',entity_dispatch='0x0048c010',implemented_core_opcodes=len(names),
    source_library_status='compiled_but_unlinked_to_complete_game',actual_resource_json_files=len(files),actual_resource_json_names=files,
    actual_instruction_count=sum(usage.values()),core_instruction_count=sum(usage[x] for x in names),
    remaining_entity_or_unknown_opcodes={str(k):v for k,v in sorted(usage.items()) if k not in names},
    opcodes=records,limitations=['Instruction occurrence counts measure static resource usage, not game completion or successful gameplay execution.',
        'Game-specific variable and entity opcode methods remain abstract interfaces; there is no default implementation.'])
(HERE/'opcode_table.json').write_text(json.dumps(result,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
print(json.dumps({k:result[k] for k in ['implemented_core_opcodes','actual_resource_json_files','actual_instruction_count','core_instruction_count']}))
