"""Report source hashes and real unresolved symbols, without pretending to link."""
from pathlib import Path
import argparse,hashlib,json,subprocess
HERE=Path(__file__).resolve().parent
def main():
    ap=argparse.ArgumentParser();ap.add_argument('library',type=Path);ap.add_argument('--dumpbin',required=True);args=ap.parse_args()
    output=subprocess.check_output([args.dumpbin,'/symbols',str(args.library)],text=True,encoding='utf-8',errors='replace')
    undefined={};defined=set()
    for line in output.splitlines():
        if 'External' not in line or '|' not in line:continue
        value=line.split('|',1)[1].strip();symbol=value.split()[0]
        if 'UNDEF' in line:undefined[symbol]=value
        else:defined.add(symbol)
    missing={symbol:value for symbol,value in undefined.items() if symbol not in defined and '@source@th20@@' in symbol}
    pending=[value for symbol,value in missing.items() if '@unrecovered@platform_window@' in symbol]
    files=list(HERE.glob('*.cpp'))+list(HERE.glob('*.hpp'))+[HERE/'dialogs.rc',HERE/'CMakeLists.txt',
        HERE/'../program_entry/program_entry.hpp',HERE/'../runtime_core/worker.hpp',HERE/'../runtime_core/worker.cpp']
    hashes={str(path.relative_to(HERE)).replace('\\','/'):hashlib.sha256(path.read_bytes()).hexdigest() for path in files}
    cpu=json.loads((HERE/'cpu_validation.json').read_text())
    resources=json.loads((HERE/'dialog_validation.json').read_text())
    report={'schema':'th20.platform_window.source_module.v1',
      'status':'compiled_static_module_with_unrecovered_dependencies',
      'library_sha256':hashlib.sha256(args.library.read_bytes()).hexdigest(),
      'source_hashes':hashes,'cpu_validation':cpu,'resource_validation':resources,
      'source_only_not_cpu_validated':['Window creation and callbacks','Direct3D creation/presentation/device reset and COM render state sequence',
        'Global initialization composition and shutdown integration','ANM surface binding and complete gameplay',
        'Present pacing and real device recovery','Surface copies and asynchronous PNG snapshots',
        'Graphics callback registration and scene transition integration','FrameStatistics timing and game/text dependencies'],
      'pending_domain_symbols':sorted(pending),'pending_domain_symbol_count':len(pending),
      'source_module_dependencies':sorted(missing.values()),
      'independent_executable_linkable':False,'original_executable_needed_by_library':False,
      'required_sdk_dll':'d3dx9_43.dll (ordinary DirectX SDK calls only)',
      'complete_source_reconstruction':False}
    (HERE/'module_status.json').write_text(json.dumps(report,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    print(f'{cpu["passed"]} checks passed; {len(pending)} pending domain symbols; full executable incomplete')
if __name__=='__main__':main()
