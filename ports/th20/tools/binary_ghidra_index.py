"""Cross-link Ghidra's real function ranges with retained source diagnostics.

The resulting labels are provenance hints; they are not the contents of the
original source files and are not a gameplay-equivalence claim.
"""
import csv,json,pathlib,re
ROOT=pathlib.Path(__file__).resolve().parents[1]
OUT=ROOT/'analysis'/'ghidra'
def read(name):
    with (OUT/name).open(encoding='utf-8',newline='') as f:return list(csv.DictReader(f))
def write(name,fields,rows):
    with (OUT/name).open('w',encoding='utf-8',newline='') as f:
        w=csv.DictWriter(f,fieldnames=fields);w.writeheader();w.writerows(rows)
def main():
    functions=read('functions.csv');by_va={int(x['entry_va'],16):x for x in functions}
    ranges={}
    for row in read('function_ranges.csv'):
        ranges.setdefault(int(row['function_entry'],16),[]).append([row['range_start'],row['range_end_inclusive']])
    source_rows=[];grouped={}
    for row in read('string_xrefs.csv'):
        match=re.search(r'\\src\\([^:]+):(\d+)\s+(.*)',row['text'])
        if not match:continue
        path=match.group(1).replace('\\','/')
        f=by_va.get(int(row['function_entry'],16)) if row['function_entry'] else None
        item=dict(source_path=path,source_line=int(match.group(2)),type_or_context=match.group(3),string_va=row['to_va'],reference_va=row['from_va'],function_entry=row['function_entry'],function_name=f['name'] if f else '',body_bytes=f['body_bytes'] if f else '',pseudocode_file=f['pseudocode_file'] if f else '')
        source_rows.append(item);grouped.setdefault(path,[]).append(item)
    write('source_location_functions.csv',['source_path','source_line','type_or_context','string_va','reference_va','function_entry','function_name','body_bytes','pseudocode_file'],source_rows)
    md=['# Source diagnostic → Ghidra function index','', 'Names and line numbers below are strings retained in the supplied executable. They identify associated code regions; the original source files have not been recovered. Ghidra function boundaries and pseudocode remain subject to manual correction.','','| Retained source path | Ghidra function entries | Diagnostic references |','| --- | --- | --- |']
    for path,items in sorted(grouped.items()):
        entries=sorted(set(x['function_entry'] for x in items if x['function_entry']))
        links=[]
        for entry in entries:
            f=by_va[int(entry,16)]
            links.append(f"[{entry}]({f['pseudocode_file']})" if f['pseudocode_file'] else entry)
        md.append(f'| `{path}` | '+', '.join(links)+f' | {len(items)} |')
    (OUT/'source_module_index.md').write_text('\n'.join(md)+'\n',encoding='utf-8')
    native=json.loads((ROOT/'reports'/'native_validation.json').read_text(encoding='utf-8'))
    verified=[]
    for name,evidence in native['evidence'].items():
        f=by_va.get(int(evidence['va'],16))
        verified.append(dict(reconstructed_cpp_name=name,machine_code_va=evidence['va'],machine_code_end_exclusive=evidence['end_exclusive'],machine_code_sha256=evidence['machine_code_sha256'],comparison_cases=native['comparisons'][name],ghidra_name=f['name'] if f else '',ghidra_signature=f['signature'] if f else '',pseudocode_file=f['pseudocode_file'] if f else ''))
    write('native_validation_links.csv',list(verified[0]),verified)
    successful=[f for f in functions if f['decompiled']=='true'];failed=[f for f in functions if f['decompiled']!='true' and f['external']!='true']
    missing=[f['pseudocode_file'] for f in successful if not (OUT/f['pseudocode_file']).is_file()]
    warnings=[]
    for f in successful:
        p=OUT/f['pseudocode_file']
        if not p.is_file():continue
        text=p.read_text(encoding='utf-8')
        lines=[x.strip() for x in text.splitlines() if 'WARNING:' in x]
        bad_data='halt_baddata' in text or 'halt_unimplemented' in text
        if lines or bad_data:warnings.append(dict(function_entry=f['entry_va'],pseudocode_file=f['pseudocode_file'],warning_count=len(lines),contains_bad_or_unimplemented_data_marker=bad_data,warning_lines=' | '.join(lines)))
    write('pseudocode_warning_files.csv',['function_entry','pseudocode_file','warning_count','contains_bad_or_unimplemented_data_marker','warning_lines'],warnings)
    summary=dict(function_rows=len(functions),pseudocode_success=len(successful),nonexternal_failed=len(failed),failed_entries=[f['entry_va'] for f in failed],export_complete_for_discovered_functions=not failed and not missing,pseudocode_bytes=sum((OUT/f['pseudocode_file']).stat().st_size for f in successful if (OUT/f['pseudocode_file']).is_file()),files_with_decompiler_warnings=len(warnings),files_with_bad_or_unimplemented_data_markers=sum(x['contains_bad_or_unimplemented_data_marker'] for x in warnings),source_diagnostic_references=len(source_rows),retained_source_paths_with_references=len(grouped),source_references_with_function=sum(bool(x['function_entry']) for x in source_rows),machine_code_validated_function_links=len(verified),missing_pseudocode_files=missing,status=('Pseudocode exported for every discovered function. ' if not failed else 'Partial automatic decompilation; failed functions are listed explicitly. ')+'Automatic analysis can miss or misidentify functions. C-like pseudocode is not compilable C++ and not proof of gameplay equivalence.')
    (OUT/'export_validation.json').write_text(json.dumps(summary,ensure_ascii=False,indent=2),encoding='utf-8')
    print(json.dumps(summary,ensure_ascii=False,indent=2))
    if missing:raise SystemExit('Missing pseudocode files')
if __name__=='__main__':main()
