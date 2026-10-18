"""Merge the single ECL retry into the authoritative export statistics.

The original first-pass results are retained as evidence, so a successful retry
does not erase the initial timeout or turn pseudocode into validated source.
"""
import csv,pathlib,shutil
ROOT=pathlib.Path(__file__).resolve().parents[1]
OUT=ROOT/'analysis'/'ghidra'
def props(path):
    return dict(line.split('=',1) for line in path.read_text(encoding='utf-8').splitlines() if '=' in line)
def read_csv(path):
    with path.open(encoding='utf-8',newline='') as f:
        r=csv.DictReader(f);return r.fieldnames,list(r)
def write_csv(path,fields,rows):
    with path.open('w',encoding='utf-8',newline='') as f:
        w=csv.DictWriter(f,fieldnames=fields);w.writeheader();w.writerows(rows)
def main():
    retry=props(OUT/'ecl_retry.properties')
    for name in ['functions.csv','decompile_failures.csv','summary.properties']:
        backup=OUT/(pathlib.Path(name).stem+'_first_pass'+pathlib.Path(name).suffix)
        if not backup.exists():shutil.copy2(OUT/name,backup)
    fields,functions=read_csv(OUT/'functions.csv')
    failfields,failures=read_csv(OUT/'decompile_failures.csv')
    if retry['success']=='true':
        if not (OUT/retry['pseudocode_file']).is_file():raise RuntimeError('Retry pseudocode is missing')
        for f in functions:
            if f['entry_va']==retry['entry_va']:
                f['decompiled']='true';f['pseudocode_file']=retry['pseudocode_file']
        failures=[f for f in failures if f['entry_va']!=retry['entry_va']]
        combined=OUT/'th20_pseudocode.c';text=combined.read_text(encoding='utf-8')
        marker='/* Failed to decompile 0x0048c010 FUN_0048c010 */'
        if marker in text:combined.write_text(text.replace(marker,(OUT/retry['pseudocode_file']).read_text(encoding='utf-8')),encoding='utf-8')
    else:
        for f in failures:
            if f['entry_va']==retry['entry_va']:
                f['error']='Initial 45-second timeout; 300-second retry: '+retry.get('error','')
    write_csv(OUT/'functions.csv',fields,functions)
    write_csv(OUT/'decompile_failures.csv',failfields,failures)
    summary=props(OUT/'summary.properties')
    summary['pseudocode_success_count']=str(sum(f['decompiled']=='true' for f in functions))
    summary['pseudocode_failure_count']=str(len(failures))
    summary['ecl_retry_success']=retry['success'];summary['ecl_retry_elapsed_ms']=retry['elapsed_ms']
    summary['export_complete_for_discovered_functions']=str(len(failures)==0).lower()
    (OUT/'summary.properties').write_text(''.join(k+'='+v+'\n' for k,v in summary.items()),encoding='utf-8')
    print('Retry merged:',retry['success'],'Success:',summary['pseudocode_success_count'],'Failure:',len(failures))
if __name__=='__main__':main()
